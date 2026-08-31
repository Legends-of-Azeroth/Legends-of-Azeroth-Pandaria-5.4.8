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

#include "ByteBuffer.h"
#include "TargetedMovementGenerator.h"
#include "Errors.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "World.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Player.h"
#include "Transport.h"
#include "Spell.h"

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T, D>::_setTargetLocation(T* owner, bool updateDestination)
{
    enum
    {
        REACH_TIME      = 1000, // How much time it takes for a pet to reach its owner
        LEADING_TIME    = 1000, // How far into the future to predict owner's movement
        LEADING_STEPS   = 4,    // How many segments to split the leading path into. 1 is fastest but can result in issues when moving on slopes
    };

    bool leadingTarget = false;

    if (!i_target.isValid() || !i_target->IsInWorld())
        return false;

    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE))
        return false;

    if (owner->GetTypeId() == TYPEID_UNIT && !i_target->isInAccessiblePlaceFor(owner->ToCreature()))
        return false;

    i_recalculateTravel = false;

    if (static_cast<D*>(this)->IsTargetReachableNow(owner, false) && (!updateDestination || owner->IsWithinLOSInMap(i_target.getTarget())))
        return false;

    if (owner->GetTotalAuraEffectValue(SPELL_AURA_MOD_DECREASE_SPEED) <= -99.9f) // Oh...
        return false;

    // Hack for Beth'tilac
    switch(owner->GetEntry())
    {
        case 52498: // Beth'tilac
            if (owner->GetMapId() == 720)
                if (owner->GetAI()->GetData(2) == 0 && (i_target->GetPositionZ() < 100.0f || i_target->IsPetGuardianStuff()))
                    return false;
            break;
        case 52447: // Cinderweb Spiderling
        case 53745: // Engorged Broodling
            if (owner->GetMapId() == 720)
                if (i_target->GetPositionZ() > 100.0f)
                    return false;
            break;
        case 56923: // Twilight Sapper
            if (owner->GetMotionMaster()->GetMotionSlot(MOTION_SLOT_CONTROLLED))
                return false;
            break;
    }

    float x, y, z;

    // Transport* currentTransport = owner->GetTransport();
    // uint64 currentTransportGUID = currentTransport ? currentTransport->GetGUID() : 0;
    // uint64 pathTransportGUID = i_path ? i_path->GetTransportGUID() : 0;
    // Transport* pathTransport = pathTransportGUID ? (pathTransportGUID == currentTransportGUID ? currentTransport : owner->GetMap()->GetTransport(pathTransportGUID)) : nullptr;

    if (updateDestination || !i_path ) // || pathTransportGUID && !pathTransport todo
    {
        if (!i_offset)
        {
            // to nearest random contact position
            i_target->GetRandomContactPoint(owner, x, y, z, CONTACT_DISTANCE, CONTACT_DISTANCE);
        }
        else if (!i_angle)
        {
            // caster chase
            i_target->GetContactPoint(owner, x, y, z, i_offset * frand(0.80f, 0.95f));
        }
        else
        {
            // to at i_offset distance from target and i_angle from target facing
            i_target->GetNearPoint(owner, x, y, z, owner->GetCombatReach(), i_offset, i_target->GetOrientation() + i_angle);
        }

        if (ShouldLeadTarget(owner))
        {
            leadingTarget = true;
            MovementFlags flags = (MovementFlags)i_target->GetUnitMovementFlags();

            if (flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))
            {
                // Calculate leading destination
                Position ownerPos = i_target->GetPosition();

                if (flags & MOVEMENTFLAG_FORWARD)
                    ownerPos.m_positionX += 1.0f;

                if (flags & MOVEMENTFLAG_BACKWARD)
                    ownerPos.m_positionX -= 1.0f;

                if (flags & MOVEMENTFLAG_STRAFE_LEFT)
                    ownerPos.m_positionY += 1.0f;

                if (flags & MOVEMENTFLAG_STRAFE_RIGHT)
                    ownerPos.m_positionY -= 1.0f;
                // TODO: Ascend/Descend, Pitch?

                uint32 const latency = i_target->GetTypeId() == TYPEID_PLAYER && (!i_target->movespline->Initialized() || i_target->movespline->Finalized()) ? i_target->ToPlayer()->GetSession()->GetLatency() : 0;
                float const direction = i_target->GetPosition().GetAngle(&ownerPos) + i_target->GetOrientation();
                float const distance = i_target->GetSpeed(GetTargetMoveType()) * (LEADING_TIME + std::min<uint32>(LEADING_TIME, latency * 2)) / IN_MILLISECONDS;

                Position leadingDest{ x, y, z };

                if (!(flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)))
                    leadingDest.m_positionZ += 0.5f; // To prevent stopping at various bumps on the road

                float stepDistance = distance / LEADING_STEPS;

                for (uint32 i = 0; i < LEADING_STEPS; ++i)
                {
                    Position prev = leadingDest;
                    owner->MovePositionToFirstCollision(leadingDest, stepDistance, direction - owner->GetOrientation());

                    if (prev.GetExactDist2dSq(&leadingDest) < std::min(CONTACT_DISTANCE, stepDistance) / 1.25f)
                        break;
                }

                leadingDest.GetPosition(x, y, z);
            }
        }
    }
    else
    {
        // the destination has not changed, we just need to refresh the path (usually speed change)
        G3D::Vector3 end { i_path->GetEndPosition() };
        // if (pathTransport)
        //     pathTransport->CalculatePassengerPosition(end.x, end.y, end.z);

        x = end.x;
        y = end.y;
        z = end.z;
    }

    if (!i_path)
        i_path = new PathGenerator(owner);
    // else if (pathTransportGUID != currentTransportGUID)
    //     i_path->UpdateNavMesh();

    if (owner->HasUnitState(UNIT_STATE_FOLLOW))
    {
        switch (owner->GetMapId())
        {
            case 489:
                i_path->SetPathLengthLimit(90.0f);
                break; // Warsong Gulch
            case 562:
                i_path->SetPathLengthLimit(80.0f);
                break; // Blade's Edge Arena
            default:
                i_path->SetPathLengthLimit(250.0f);
                break;
        }
    }

    // allow pets to use shortcut if no path found when following their master
    bool forceDest = (owner->GetTypeId() == TYPEID_UNIT && owner->ToCreature()->IsPet()
        && owner->HasUnitState(UNIT_STATE_FOLLOW)
        && i_target.getTarget()->GetGUID() == owner->ToCreature()->GetOwnerGUID());

    bool result = i_path->CalculatePath(x, y, z, forceDest);

    if (!result || (i_path->GetPathType() & PATHFIND_NOPATH) && !forceDest)
    {
        // Can't reach target
        i_recalculateTravel = true;

        if (updateDestination)
            i_recheckDistance.Reset(owner->GetMap()->IsBattlegroundOrArena() ? 100 : 1000);
        return false;
    }

    D::_addUnitStateMove(owner);
    i_targetReached = false;
    owner->AddUnitState(UNIT_STATE_CHASE);

    Movement::MoveSplineInit init(owner);

    if (i_path->GetPathType() == PATHFIND_NOPATH)
        init.MoveTo(x, y, z, false);
    else
        init.MovebyPath(i_path->GetPath());

    init.SetWalk(((D*)this)->EnableWalking());

    if (leadingTarget)
    {
        float pathDistance = i_path->GetPathType() == PATHFIND_NOPATH ? owner->GetExactDist(x, y, z) : i_path->GetLinearPathLength();
        UnitMoveType const mtype = static_cast<D*>(this)->EnableWalking() ? MOVE_WALK : MOVE_RUN;
        init.SetVelocity(std::max(2.5f, std::min(pathDistance, owner->IsInCombat() || owner->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED) ? owner->GetSpeed(mtype) : 2.5f * std::max(owner->GetSpeed(mtype), i_target->GetSpeed(GetTargetMoveType())))) * IN_MILLISECONDS / REACH_TIME);
    }

    // Using the same condition for facing target as the one that is used for SetInFront on movement end
    // - applies to ChaseMovementGenerator mostly
    if (i_angle == 0.f)
    {
        init.SetFacing(i_target.getTarget());
    }
    else
    {
        init.SetFacing(i_target->GetOrientation());
    }

    init.Launch();

    // Don't delay next update, since owner will never be "reachable" in leading destination
    if (leadingTarget)
        return true;

    if (updateDestination)
        if (!static_cast<D*>(this)->IsTargetReachableInDestination(owner))
            i_recheckDistance.Reset(owner->GetMap()->IsBattlegroundOrArena() ? 100 : 1000);

    return true;
}

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T, D>::DoUpdate(T* owner, uint32 time_diff)
{
    if (!i_target.isValid() || !i_target->IsInWorld())
        return false;

    if (!owner || !owner->IsAlive())
        return false;

    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE))
    {
        D::_clearUnitStateMove(owner);
        return true;
    }

    // prevent movement while casting spells with cast time or channel time
    if (owner->HasUnitState(UNIT_STATE_CASTING))
    {
        bool stop = true;
        Spell* spell = owner->GetCurrentSpell(CURRENT_GENERIC_SPELL);

        if (spell)
            for (auto&& effect : owner->GetAuraEffectsByType(SPELL_AURA_CAST_WHILE_WALKING))
                if (effect->IsAffectingSpell(spell->GetSpellInfo()))
                    stop = false;

        if (stop)
        {
            if (!owner->IsStopped())
                owner->StopMoving();
            return true;
        }
    }

    // prevent crash after creature killed pet
    if (static_cast<D*>(this)->_lostTarget(owner))
    {
        D::_clearUnitStateMove(owner);
        return true;
    }

    bool targetMoved = false;
    // Attempt to stop active movement if the target has moved and is now within reach
    if ((i_target->m_positionX != i_targetLastPosition.m_positionX || i_target->m_positionY != i_targetLastPosition.m_positionY || i_target->m_positionZ != i_targetLastPosition.m_positionZ)
        && (i_targetLastPosition.Relocate(i_target.getTarget()), static_cast<D*>(this)->IsTargetReachableNow(owner, true)))
    {
        if (!owner->IsStopped())
            owner->StopMoving();
    }
    else
    {
        i_recheckDistance.Update(time_diff);
        i_recheckLOS.Update(time_diff);

        if (i_recheckDistance.Passed())
        {
            i_recheckDistance.Reset(100);
            targetMoved = !static_cast<D*>(this)->IsTargetReachableInDestination(owner);

            if (i_recheckLOS.Passed())
                i_recheckLOS.Reset(owner->GetMap()->IsBattlegroundOrArena() ? 100 : 1000);
        }
    }

    // // Enter/exit transport if the target enters/exits transport
    // bool transportChanged = false;
    // Transport* myTransport = owner->GetTransport();
    // Transport* targetTransport = i_target->GetTransport();

    // if (myTransport != targetTransport)
    // {
    //     // Don't allow static spawns to step off transports
    //     if (myTransport && myTransport->GetStaticPassengers().find(owner) != myTransport->GetStaticPassengers().end())
    //     {
    //         if (!owner->IsStopped())
    //             owner->StopMoving();

    //         return true;
    //     }

    //     if (myTransport)
    //     {
    //         myTransport->RemovePassenger(owner);
    //         owner->setActive(false, ActiveFlags::OnTransport);
    //     }

    //     if (targetTransport)
    //         if (owner->AddToTransportIfNeeded(targetTransport, false, 10.0f))
    //             owner->setActive(true, ActiveFlags::OnTransport);

    //     targetMoved = true;
    //     transportChanged = true;
    // }

    if (i_recalculateTravel || targetMoved)
    {
        bool splineStarted = _setTargetLocation(owner, targetMoved);

        // if (!splineStarted && transportChanged)
        //     owner->SendMovementFlagUpdate();
    }

    if (owner->movespline->Finalized())
    {
        static_cast<D*>(this)->MovementInform(owner);

        if (i_angle == 0.f && !owner->HasInArc(0.01f, i_target.getTarget()))
            owner->SetInFront(i_target.getTarget());

        if (!i_targetReached)
        {
            i_targetReached = true;
            static_cast<D*>(this)->_reachTarget(owner);

            if (!i_pendingFacingUpdate && owner->HasUnitState(UNIT_STATE_FOLLOW))
            {
                i_pendingFacingUpdate = true;
                i_pendingFacingUpdateDelay.Reset(500);
            }
        }
    }

    return true;
}

template<class T, typename D>
G3D::Vector3 TargetedMovementGeneratorMedium<T, D>::GetDestination(T* owner) const
{
    if (owner->movespline->Initialized() && !owner->movespline->Finalized())
    {
        G3D::Vector3 dest{ owner->movespline->FinalDestination() };

        // if (owner->movespline->onTransport)
        //     if (TransportBase* transport = owner->GetDirectTransport())
        //         transport->CalculatePassengerPosition(dest.x, dest.y, dest.z);

        return dest;
    }
    else
    {
        G3D::Vector3 dest;
        owner->GetPosition(dest.x, dest.y, dest.z);
        return dest;
    }
}

template<class T, typename D>
UnitMoveType TargetedMovementGeneratorMedium<T, D>::GetTargetMoveType() const
{
    if (i_target->GetTypeId() == TYPEID_PLAYER)
    {
        MovementFlags flags = (MovementFlags)i_target->GetUnitMovementFlags();

        if (flags & MOVEMENTFLAG_SWIMMING)
            return flags & MOVEMENTFLAG_BACKWARD ? MOVE_SWIM_BACK : MOVE_SWIM;

        if (flags & MOVEMENTFLAG_FLYING)
            return flags & MOVEMENTFLAG_BACKWARD ? MOVE_FLIGHT_BACK : MOVE_FLIGHT;

        if (flags & MOVEMENTFLAG_WALKING)
            return MOVE_WALK;

        return flags & MOVEMENTFLAG_BACKWARD ? MOVE_RUN_BACK : MOVE_RUN;
    }
    return static_cast<D const*>(this)->EnableWalking() ? MOVE_WALK : MOVE_RUN;
}

template<class T, typename D>
bool TargetedMovementGeneratorMedium<T, D>::ShouldLeadTarget(T* owner) const
{
    return owner->HasUnitState(UNIT_STATE_FOLLOW) && owner->CanLeadFollowTarget(i_target.getTarget());
}

//-----------------------------------------------//
template<class T>
bool ChaseMovementGenerator<T>::IsTargetReachableInDestination(T* owner) const
{
    G3D::Vector3 dest{ this->GetDestination(owner) };

    float desiredDist;

    if (this->i_offset)
        desiredDist = owner->GetCombatReach() + this->i_target->GetCombatReach() + this->i_offset;
    else
        desiredDist = std::max(NOMINAL_MELEE_RANGE, owner->GetCombatReach() + this->i_target->GetCombatReach() + 4.0f / 3.0f);

    return this->i_target->GetExactDistSq(dest.x, dest.y, dest.z) <= desiredDist * desiredDist
        && (!this->i_recheckLOS.Passed() || this->i_target->IsWithinLOS(dest.x, dest.y, dest.z));
}

template<class T>
bool ChaseMovementGenerator<T>::IsTargetReachableNow(T* owner, bool duringMovement) const
{
    if (this->i_offset && this->i_angle)
        return false; // Don't interrupt movement if we want to be at a specific angle
    if (this->i_angle)
        return false; // Don't interrupt movement if we want to be at a specific angle
    if (this->i_offset)
        return owner->IsWithinCombatRange(this->i_target.getTarget(), this->i_offset * 0.8f) // Interrupt movement if the target is within the desired distance
            && owner->IsWithinLOSInMap(this->i_target.getTarget()); // Interrupt movement if the target is within the desired distance and los

    // Allow units to stop prematurely, otherwise they might run through each other before stopping
    // Skip LOS check for performance reasons. It will eventually be rechecked in IsTargetReachableInDestination anyway.
    return owner->IsWithinRangedMeleeRange(this->i_target.getTarget(), 0.0f); // IsWithinRangedMeleeRange instead of IsWithinMeleeRange to skip range increase while moving

}

template<class T>
void ChaseMovementGenerator<T>::_reachTarget(T* owner)
{
    if (owner->IsWithinMeleeRange(this->i_target.getTarget()))
        owner->Attack(this->i_target.getTarget(), true);
}

template<>
void ChaseMovementGenerator<Player>::DoInitialize(Player* owner)
{
    owner->AddUnitState(UNIT_STATE_CHASE | UNIT_STATE_CHASE_MOVE);
    _setTargetLocation(owner, true);
}

template<>
void ChaseMovementGenerator<Creature>::DoInitialize(Creature* owner)
{
    owner->SetWalk(false);
    owner->AddUnitState(UNIT_STATE_CHASE | UNIT_STATE_CHASE_MOVE);
    _setTargetLocation(owner, true);
}

template<class T>
void ChaseMovementGenerator<T>::DoFinalize(T* owner)
{
    owner->ClearUnitState(UNIT_STATE_CHASE | UNIT_STATE_CHASE_MOVE);
}

template<class T>
void ChaseMovementGenerator<T>::DoReset(T* owner)
{
    DoInitialize(owner);
}

template<class T>
void ChaseMovementGenerator<T>::MovementInform(T* /*unit*/) { }

template<>
void ChaseMovementGenerator<Creature>::MovementInform(Creature* unit)
{
    // Pass back the GUIDLow of the target. If it is pet's owner then PetAI will handle
    if (unit->AI())
        unit->AI()->MovementInform(CHASE_MOTION_TYPE, i_target.getTarget()->GetGUID().GetCounter());
}

//-----------------------------------------------//
template<>
bool FollowMovementGenerator<Creature>::EnableWalking() const
{
    return i_target.isValid() && (i_target->IsWalking() || i_target->movespline && i_target->movespline->Initialized() && !i_target->movespline->Finalized() && i_target->movespline->IsWalking());
}

template<>
bool FollowMovementGenerator<Player>::EnableWalking() const
{
    return false;
}

template<class T>
bool FollowMovementGenerator<T>::IsTargetReachableInDestination(T* owner) const
{
    if (this->ShouldLeadTarget(owner))
        if (this->i_target->HasUnitMovementFlag(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))
            return false;

    G3D::Vector3 dest{ this->GetDestination(owner) };

    float desiredDist;

    if (this->i_offset)
        desiredDist = owner->GetCombatReach() + this->i_offset + (this->i_angle ? CONTACT_DISTANCE : 0); // For !i_angle case offset was already reduced in _setTargetLocation, no need for additional threshold
    else
        desiredDist = owner->GetCombatReach() + this->i_target->GetCombatReach() + CONTACT_DISTANCE;

    return this->i_target->GetExactDistSq(dest.x, dest.y, dest.z) <= desiredDist * desiredDist;
}

template<class T>
bool FollowMovementGenerator<T>::IsTargetReachableNow(T* owner, bool duringMovement) const
{
    // Always follow to the end of the path
    return false;
}

template<>
void FollowMovementGenerator<Player>::DoInitialize(Player* owner)
{
    owner->AddUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
    _setTargetLocation(owner, true);
}

template<>
void FollowMovementGenerator<Creature>::DoInitialize(Creature* owner)
{
    owner->AddUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
    _setTargetLocation(owner, true);
}

template<class T>
void FollowMovementGenerator<T>::DoFinalize(T* owner)
{
    owner->ClearUnitState(UNIT_STATE_FOLLOW | UNIT_STATE_FOLLOW_MOVE);
}

template<class T>
void FollowMovementGenerator<T>::DoReset(T* owner)
{
    DoInitialize(owner);
}

template<class T>
void FollowMovementGenerator<T>::MovementInform(T* /*unit*/) { }

template<>
void FollowMovementGenerator<Creature>::MovementInform(Creature* unit)
{
    // Pass back the GUIDLow of the target. If it is pet's owner then PetAI will handle
    if (unit->AI())
        unit->AI()->MovementInform(FOLLOW_MOTION_TYPE, i_target.getTarget()->GetGUID().GetCounter());
}

//-----------------------------------------------//
template bool TargetedMovementGeneratorMedium<Player, ChaseMovementGenerator<Player> >::_setTargetLocation(Player*, bool);
template bool TargetedMovementGeneratorMedium<Player, FollowMovementGenerator<Player> >::_setTargetLocation(Player*, bool);
template bool TargetedMovementGeneratorMedium<Creature, ChaseMovementGenerator<Creature> >::_setTargetLocation(Creature*, bool);
template bool TargetedMovementGeneratorMedium<Creature, FollowMovementGenerator<Creature> >::_setTargetLocation(Creature*, bool);
template bool TargetedMovementGeneratorMedium<Player, ChaseMovementGenerator<Player> >::DoUpdate(Player*, uint32);
template bool TargetedMovementGeneratorMedium<Player, FollowMovementGenerator<Player> >::DoUpdate(Player*, uint32);
template bool TargetedMovementGeneratorMedium<Creature, ChaseMovementGenerator<Creature> >::DoUpdate(Creature*, uint32);
template bool TargetedMovementGeneratorMedium<Creature, FollowMovementGenerator<Creature> >::DoUpdate(Creature*, uint32);

template bool ChaseMovementGenerator<Player>::IsTargetReachableInDestination(Player*) const;
template bool ChaseMovementGenerator<Creature>::IsTargetReachableInDestination(Creature*) const;
template bool ChaseMovementGenerator<Player>::IsTargetReachableNow(Player*, bool) const;
template bool ChaseMovementGenerator<Creature>::IsTargetReachableNow(Creature*, bool) const;
template void ChaseMovementGenerator<Player>::_reachTarget(Player*);
template void ChaseMovementGenerator<Creature>::_reachTarget(Creature*);
template void ChaseMovementGenerator<Player>::DoFinalize(Player*);
template void ChaseMovementGenerator<Creature>::DoFinalize(Creature*);
template void ChaseMovementGenerator<Player>::DoReset(Player*);
template void ChaseMovementGenerator<Creature>::DoReset(Creature*);
template void ChaseMovementGenerator<Player>::MovementInform(Player*);

template bool FollowMovementGenerator<Player>::IsTargetReachableInDestination(Player*) const;
template bool FollowMovementGenerator<Creature>::IsTargetReachableInDestination(Creature*) const;
template bool FollowMovementGenerator<Player>::IsTargetReachableNow(Player*, bool) const;
template bool FollowMovementGenerator<Creature>::IsTargetReachableNow(Creature*, bool) const;
template void FollowMovementGenerator<Player>::DoFinalize(Player*);
template void FollowMovementGenerator<Creature>::DoFinalize(Creature*);
template void FollowMovementGenerator<Player>::DoReset(Player*);
template void FollowMovementGenerator<Creature>::DoReset(Creature*);
template void FollowMovementGenerator<Player>::MovementInform(Player*);
