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

#include "Object.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "CellImpl.h"
#include "Chat.h"
#include "CinematicMgr.h"
#include "Common.h"
#include "Creature.h"
#include "DynamicTree.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Log.h"
#include "MapManager.h"
#include "MiscPackets.h"
#include "MovementPacketBuilder.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvPMgr.h"
#include "Player.h"
#include "SharedDefines.h"
#include "TargetedMovementGenerator.h"
#include "TemporarySummon.h"
#include "Totem.h"
#include "Transport.h"
#include "Unit.h"
#include "UpdateData.h"
#include "UpdateFieldFlags.h"
#include "UpdateMask.h"
#include "Util.h"
#include "Vehicle.h"
#include "VMapFactory.h"
#include "VMapManager2.h"
#include "World.h"
#include "WorldPacket.h"

#include "Physics.h"

#define STEALTH_VISIBILITY_UPDATE_TIMER 500

constexpr float VisibilityDistances[AsUnderlyingType(VisibilityDistanceType::Max)] =
{
    DEFAULT_VISIBILITY_DISTANCE,
    VISIBILITY_DISTANCE_TINY,
    VISIBILITY_DISTANCE_SMALL,
    VISIBILITY_DISTANCE_LARGE,
    VISIBILITY_DISTANCE_GIGANTIC,
    MAX_VISIBILITY_DISTANCE
};

Object::Object()
{
    m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPEMASK_OBJECT;

    m_uint32Values      = nullptr;
    m_valuesCount       = 0;
    _fieldNotifyFlags   = UF_FLAG_URGENT;

    m_inWorld           = false;
    m_isDestroyedObject = false;
    m_objectUpdated     = false;
}

WorldObject::~WorldObject()
{
    // this may happen because there are many !create/delete
    if (IsWorldObject() && m_currMap)
    {
        if (GetTypeId() == TYPEID_CORPSE)
        {
            TC_LOG_FATAL("misc", "Object::~Object Corpse guid=" UI64FMTD ", type=%d, entry=%u deleted but still in map!!",
                GetGUID().GetRawValue(), ((Corpse*)this)->GetType(), GetEntry());
            ASSERT(false);
        }
        ResetMap();
    }
}

Object::~Object()
{
    if (IsInWorld())
    {
        TC_LOG_FATAL("misc", "Object::~Object - guid=" UI64FMTD ", typeid=%d, entry=%u deleted but still in world!!", GetGUID().GetRawValue(), GetTypeId(), GetEntry());
        if (isType(TYPEMASK_ITEM))
            TC_LOG_FATAL("misc", "Item slot %u", ((Item*)this)->GetSlot());
        ASSERT(false);
        RemoveFromWorld();
    }

    if (m_objectUpdated)
    {
        TC_LOG_FATAL("misc", "Object::~Object - guid=" UI64FMTD ", typeid=%d, entry=%u deleted but still in update list!!", GetGUID().GetRawValue(), GetTypeId(), GetEntry());
        ASSERT(false);
    }

    delete [] m_uint32Values;
    m_uint32Values = nullptr;
}

void Object::_InitValues()
{
    m_uint32Values = new uint32[m_valuesCount];
    memset(m_uint32Values, 0, m_valuesCount*sizeof(uint32));

    _changesMask.SetCount(m_valuesCount);

    if (m_dynamicValuesCount)
    {
        m_dynamicValues.resize(m_dynamicValuesCount);
        m_dynamicChange.resize(m_dynamicValuesCount);

        for (size_t i = 0; i < m_dynamicValues.size(); ++i)
        {
            m_dynamicValues[i].Clear();
            m_dynamicChange[i] = false;
        }
    }

    m_objectUpdated = false;
}

void Object::_Create(ObjectGuid::LowType guidlow, uint32 entry, HighGuid guidhigh)
{
    if (!m_uint32Values) _InitValues();

    ObjectGuid guid = ObjectGuid(guidhigh, entry, guidlow);
    SetGuidValue(OBJECT_FIELD_GUID, guid);
    SetUInt16Value(OBJECT_FIELD_TYPE, 0, m_objectType);
    m_PackGUID.Set(guid);
}

std::string Object::_ConcatFields(uint16 startIndex, uint16 size) const
{
    std::ostringstream ss;
    for (uint16 index = 0; index < size; ++index)
        ss << GetUInt32Value(index + startIndex) << ' ';
    return ss.str();
}

void Object::AddToWorld()
{
    if (m_inWorld)
        return;

    ASSERT(m_uint32Values);

    m_inWorld = true;

    // synchronize values mirror with values array (changes will send in updatecreate opcode any way
    ClearUpdateMask(true);
}

void Object::RemoveFromWorld()
{
    if (!m_inWorld)
        return;

    m_inWorld = false;

    // if we remove from world then sending changes not required
    ClearUpdateMask(true);
}

void Object::BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const
{
    if (!target)
        return;

    uint8  updateType = UPDATETYPE_CREATE_OBJECT;
    uint16 flags      = m_updateFlag;

    /** lower flag1 **/
    if (target == this)                                      // building packet for yourself
        flags |= UPDATEFLAG_SELF;

    switch (GetGUID().GetHigh())
    {
        case HighGuid::Player:
        case HighGuid::Pet:
        case HighGuid::Corpse:
        case HighGuid::DynamicObject:
        case HighGuid::AreaTrigger:
            updateType = UPDATETYPE_CREATE_OBJECT2;
            break;
        case HighGuid::Unit:
        case HighGuid::Vehicle:
        {
            if (TempSummon const* summon = ToUnit()->ToTempSummon())
                if (summon->GetSummonerGUID().IsPlayer())
                    updateType = UPDATETYPE_CREATE_OBJECT2;

            break;
        }
        case HighGuid::GameObject:
        {
            if (ToGameObject()->GetOwnerGUID().IsPlayer())
                updateType = UPDATETYPE_CREATE_OBJECT2;
            break;
        }
        default:
            break;
    }

    if (WorldObject const* worldObject = dynamic_cast<WorldObject const*>(this))
    {
        if (worldObject->GetAIAnimKitId() || worldObject->GetMovementAnimKitId() || worldObject->GetMeleeAnimKitId())
            flags |= UPDATEFLAG_ANIMKITS;

        if (GetTypeId() == TYPEID_GAMEOBJECT || GetTypeId() == TYPEID_DYNAMICOBJECT)
            if (worldObject->GetTransGUID())
                flags |= UPDATEFLAG_GO_TRANSPORT_POSITION;
    }

    if (flags & UPDATEFLAG_STATIONARY_POSITION)
    {
        // UPDATETYPE_CREATE_OBJECT2 for some gameobject types...
        if (isType(TYPEMASK_GAMEOBJECT))
        {
            switch (ToGameObject()->GetGoType())
            {
                case GAMEOBJECT_TYPE_TRAP:
                case GAMEOBJECT_TYPE_DUEL_ARBITER:
                case GAMEOBJECT_TYPE_FLAGSTAND:
                case GAMEOBJECT_TYPE_FLAGDROP:
                    updateType = UPDATETYPE_CREATE_OBJECT2;
                    break;
                default:
                    break;
            }
        }
    }

    if (Unit const* unit = ToUnit())
        if (unit->GetVictim())
            flags |= UPDATEFLAG_HAS_TARGET;

    ByteBuffer buf;
    buf << uint8(updateType);
    buf << GetPackGUID();
    buf << uint8(m_objectTypeId);

    BuildMovementUpdate(&buf, flags);
    BuildValuesUpdate(updateType, &buf, target);
    data->AddUpdateBlock(buf);
}

void Object::SendUpdateToPlayer(Player* player)
{
    // send create update to player
    UpdateData upd(player->GetMapId());
    WorldPacket packet;

    BuildCreateUpdateBlockForPlayer(&upd, player);
    upd.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData* data, Player* target) const
{
    ByteBuffer buf;

    buf << uint8(UPDATETYPE_VALUES);
    buf << GetPackGUID();

    BuildValuesUpdate(UPDATETYPE_VALUES, &buf, target);

    data->AddUpdateBlock(buf);
}

void Object::BuildOutOfRangeUpdateBlock(UpdateData* data) const
{
    data->AddOutOfRangeGUID(GetGUID());
}

void Object::DestroyForPlayer(Player* target, bool onDeath) const
{
    ASSERT(target);

    if (isType(TYPEMASK_UNIT) || isType(TYPEMASK_PLAYER))
    {
        if (Battleground* bg = target->GetBattleground())
        {
            if (bg->IsArena())
            {
                ObjectGuid guid = GetGUID();
                WorldPacket data(SMSG_ARENA_UNIT_DESTROYED, 8);
                data.WriteBit(guid[3]);
                data.WriteBit(guid[2]);
                data.WriteBit(guid[6]);
                data.WriteBit(guid[0]);
                data.WriteBit(guid[4]);
                data.WriteBit(guid[5]);
                data.WriteBit(guid[7]);
                data.WriteBit(guid[1]);
                data.WriteByteSeq(guid[6]);
                data.WriteByteSeq(guid[1]);
                data.WriteByteSeq(guid[5]);
                data.WriteByteSeq(guid[7]);
                data.WriteByteSeq(guid[3]);
                data.WriteByteSeq(guid[4]);
                data.WriteByteSeq(guid[2]);
                data.WriteByteSeq(guid[0]);
                target->GetSession()->SendPacket(&data);
            }
        }
    }

    WorldPacket data(SMSG_DESTROY_OBJECT, 2 + 8);
    ObjectGuid guid(GetGUID());

    data.WriteBit(guid[3]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);

    //! If the following bool is true, the client will call "void CGUnit_C::OnDeath()" for this object.
    //! OnDeath() does for eg trigger death animation and interrupts certain spells/missiles/auras/sounds...
    data.WriteBit(onDeath);

    data.WriteBit(guid[7]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[5]);
    data.FlushBits();

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[5]);

    target->GetSession()->SendPacket(&data);
}

void Object::SendOutOfRangeForPlayer(Player* target) const
{
    ASSERT(target);
    UpdateData updateData(target->GetMapId());
    BuildOutOfRangeUpdateBlock(&updateData);
    WorldPacket packet;
    updateData.BuildPacket(&packet);
    target->SendDirectMessage(&packet);
}

int32 Object::GetInt32Value(uint16 index) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    return m_int32Values[index];
}

uint32 Object::GetUInt32Value(uint16 index) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    return m_uint32Values[index];
}

uint64 Object::GetUInt64Value(uint16 index) const
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, false));
    return *((uint64*)&(m_uint32Values[index]));
}

float Object::GetFloatValue(uint16 index) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    return m_floatValues[index];
}

uint8 Object::GetByteValue(uint16 index, uint8 offset) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    ASSERT(offset < 4);
    return *(((uint8*)&m_uint32Values[index])+offset);
}

uint16 Object::GetUInt16Value(uint16 index, uint8 offset) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    ASSERT(offset < 2);
    return *(((uint16*)&m_uint32Values[index])+offset);
}

ObjectGuid Object::GetGuidValue(uint16 index) const
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, false));
    return *((ObjectGuid*)&(m_uint32Values[index]));
}

uint32 Object::GetDynamicUInt32Value(uint32 tab, uint16 index) const
{
    ASSERT(tab < m_dynamicValues.size() || index < DynamicField::MaxSize);
    return m_dynamicValues[tab][index];
}

void Object::BuildMovementUpdate(ByteBuffer* data, uint16 flags) const
{
    bool hasLiving = flags & UPDATEFLAG_LIVING;
    bool hasStacionaryPostion = flags & UPDATEFLAG_STATIONARY_POSITION;
    bool hasGobjectRotation = flags & UPDATEFLAG_ROTATION;
    bool hasTransportPosition = flags & UPDATEFLAG_GO_TRANSPORT_POSITION;
    bool hasTarget = flags & UPDATEFLAG_HAS_TARGET;
    bool hasTransport = flags & UPDATEFLAG_TRANSPORT;
    bool hasVehicle = flags & UPDATEFLAG_VEHICLE;
    bool hasAnimKits = flags & UPDATEFLAG_ANIMKITS;

    AreaTrigger const* atrigger = ToAreaTrigger();
    bool hasAreaTriggerData = atrigger != nullptr;
    bool hasAreaTriggerSpline = false;
    bool hasAreaTriggerScale = false;
    bool hasMoveCurveId = false;

    if (atrigger && atrigger->HasAreaTriggerTransport())
        hasTransportPosition = true;

    if (atrigger)
    {
        hasAreaTriggerSpline = atrigger->HasSpline();
        hasAreaTriggerScale = (atrigger->GetScaleX() != 0.0f || atrigger->GetScaleY() != 0.0f);
    }

    bool hasFallData = false;
    bool hasFallDirection = false;
    bool hasSpline = false;
    bool hasPitch = false;
    bool hasSplineElevation = false;
    bool hasUnitTransport = false;

    uint32 movementFlags = 0;
    uint32 movementFlagsExtra = 0;

    data->WriteBit(0);
    data->WriteBit(hasAnimKits);
    data->WriteBit(hasLiving);
    data->WriteBit(0);
    data->WriteBit(0);
    data->WriteBits(0, 22);
    data->WriteBit(hasVehicle);
    data->WriteBit(0);
    data->WriteBit(0);
    data->WriteBit(hasTransport);
    data->WriteBit(hasGobjectRotation);
    data->WriteBit(0);
    data->WriteBit(flags & UPDATEFLAG_SELF);
    data->WriteBit(hasTarget);
    data->WriteBit(0);
    data->WriteBit(0);
    data->WriteBit(0);
    data->WriteBit(hasAreaTriggerData);
    data->WriteBit(hasTransportPosition);
    data->WriteBit(0);
    data->WriteBit(hasStacionaryPostion);

    if (hasLiving)
    {
        Unit const* self = ToUnit();
        ObjectGuid guid = GetGUID();

        hasFallDirection = self->HasUnitMovementFlag(MOVEMENTFLAG_FALLING);
        hasFallData = hasFallDirection || self->m_movementInfo.jump.fallTime != 0;
        movementFlags = self->GetUnitMovementFlags();
        movementFlagsExtra = self->GetExtraUnitMovementFlags();
        hasSpline = self->IsSplineEnabled() && self->GetTypeId() != TYPEID_PLAYER;
        hasPitch = self->HasUnitMovementFlag(MovementFlags(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || self->HasExtraUnitMovementFlag(MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING);
        hasSplineElevation = self->HasUnitMovementFlag(MOVEMENTFLAG_SPLINE_ELEVATION);
        hasUnitTransport = self->m_movementInfo.transport.guid;

        if (GetTypeId() == TYPEID_UNIT)
            movementFlags &= MOVEMENTFLAG_MASK_CREATURE_ALLOWED;

        //// Prevent client disconnect
        //if (!G3D::fuzzyEq(self->m_movementInfo.splineElevation, 0.0f) && (movementFlags & MOVEMENTFLAG_SPLINE_ELEVATION) == 0)
        //    movementFlags |= MOVEMENTFLAG_SPLINE_ELEVATION;

        data->WriteBit(guid[2]);
        data->WriteBit(0);
        data->WriteBit(!hasPitch);
        data->WriteBit(hasUnitTransport);
        data->WriteBit(0);

        if (hasUnitTransport)
        {
            ObjectGuid transGuid = self->m_movementInfo.transport.guid;
            data->WriteBit(transGuid[4]);
            data->WriteBit(transGuid[2]);
            data->WriteBit(self->m_movementInfo.transport.time3 && !self->m_movementInfo.transport.guid.IsEmpty());
            data->WriteBit(transGuid[0]);
            data->WriteBit(transGuid[1]);
            data->WriteBit(transGuid[3]);
            data->WriteBit(transGuid[6]);
            data->WriteBit(transGuid[7]);
            data->WriteBit(self->m_movementInfo.transport.time2 && !self->m_movementInfo.transport.guid.IsEmpty());
            data->WriteBit(transGuid[5]);
        }

        data->WriteBit(!self->m_movementInfo.time);
        data->WriteBit(guid[6]);
        data->WriteBit(guid[4]);
        data->WriteBit(guid[3]);

        data->WriteBit(G3D::fuzzyEq(self->GetOrientation(), 0.0f)); //!G3D::fuzzyEq(self->GetOrientation(), 0.0f)

        data->WriteBit(!self->GetMovementCounter());
        data->WriteBit(guid[5]);
        data->WriteBits(0, 22); // Forces
        data->WriteBit(!movementFlags);
        data->WriteBits(0, 19);
        data->WriteBit(hasFallData);

        if (movementFlags)
            data->WriteBits(movementFlags, 30);

        data->WriteBit(!hasSplineElevation);
        data->WriteBit(hasSpline);
        data->WriteBit(0);
        data->WriteBit(guid[0]);
        data->WriteBit(guid[7]);
        data->WriteBit(guid[1]);

        if (hasSpline)
            Movement::PacketBuilder::WriteCreateBits(*self->movespline, *data);

        data->WriteBit(!movementFlagsExtra);

        if (hasFallData)
            data->WriteBit(hasFallDirection);

        if (movementFlagsExtra)
           data->WriteBits(movementFlagsExtra, 13);
    }

    if (hasTransportPosition)
    {
        WorldObject const* self = static_cast<WorldObject const*>(this);
        ObjectGuid transGuid = self->m_movementInfo.transport.guid;
        data->WriteBit(transGuid[4]);
        data->WriteBit(transGuid[1]);
        data->WriteBit(transGuid[0]);
        data->WriteBit(self->m_movementInfo.transport.time2 && !self->m_movementInfo.transport.guid.IsEmpty());
        data->WriteBit(transGuid[6]);
        data->WriteBit(transGuid[5]);
        data->WriteBit(transGuid[3]);
        data->WriteBit(transGuid[2]);
        data->WriteBit(transGuid[7]);
        data->WriteBit(self->m_movementInfo.transport.time3 && !self->m_movementInfo.transport.guid.IsEmpty());
    }

    if (hasAreaTriggerData)
    {
        data->WriteBit(false);
        data->WriteBit(false);
        data->WriteBit(false);
        data->WriteBit(false);
        data->WriteBit(false);
        data->WriteBit(false);
        data->WriteBit(hasAreaTriggerScale);   // scale
        data->WriteBit(false);
        data->WriteBit(hasAreaTriggerSpline);
        data->WriteBit(false);
        if (hasAreaTriggerSpline)
            data->WriteBits(atrigger->GetPath().size(), 20);
        data->WriteBit(false);
        data->WriteBit(false);
        data->WriteBit(hasMoveCurveId);
    }

    if (hasAnimKits)
    {
        WorldObject const* self = static_cast<WorldObject const*>(this);
        data->WriteBit(self->GetMovementAnimKitId() == 0);
        data->WriteBit(self->GetMeleeAnimKitId() == 0);
        data->WriteBit(self->GetAIAnimKitId() == 0);
    }

    if (hasTarget)
    {
        Unit const* self = ToUnit();
        ObjectGuid victimGuid = self->GetVictim()->GetGUID();
        data->WriteBit(victimGuid[4]);
        data->WriteBit(victimGuid[6]);
        data->WriteBit(victimGuid[5]);
        data->WriteBit(victimGuid[2]);
        data->WriteBit(victimGuid[0]);
        data->WriteBit(victimGuid[1]);
        data->WriteBit(victimGuid[3]);
        data->WriteBit(victimGuid[7]);
    }

    data->FlushBits();

    if (hasLiving)
    {
        Unit const* self = ToUnit();
        ObjectGuid guid = GetGUID();

        if (hasUnitTransport)
        {
            ObjectGuid transGuid = self->m_movementInfo.transport.guid;

            data->WriteByteSeq(transGuid[7]);
            *data << float(self->GetTransOffsetX());

            if (self->m_movementInfo.transport.time3 && !self->m_movementInfo.transport.guid.IsEmpty())
                *data << uint32(self->m_movementInfo.transport.time3);

            *data << float(self->GetTransOffsetO());
            *data << float(self->GetTransOffsetY());
            data->WriteByteSeq(transGuid[4]);
            data->WriteByteSeq(transGuid[1]);
            data->WriteByteSeq(transGuid[3]);
            *data << float(self->GetTransOffsetZ());
            data->WriteByteSeq(transGuid[5]);

            if (self->m_movementInfo.transport.time2 && !self->m_movementInfo.transport.guid.IsEmpty())
                *data << uint32(self->m_movementInfo.transport.time2);

            data->WriteByteSeq(transGuid[0]);
            *data << int8(self->GetTransSeat());
            data->WriteByteSeq(transGuid[6]);
            data->WriteByteSeq(transGuid[2]);
            *data << uint32(self->GetTransTime());
        }

        data->WriteByteSeq(guid[4]);

        if (hasSpline)
            Movement::PacketBuilder::WriteCreateData(*self->movespline, *data);

        *data << float(self->GetSpeed(MOVE_FLIGHT));

        if (self->GetMovementCounter())
            *data << uint32(self->GetMovementCounter());

        data->WriteByteSeq(guid[2]);

        if (hasFallData)
        {
            if (hasFallDirection)
            {
                *data << float(self->m_movementInfo.jump.xyspeed);
                *data << float(self->m_movementInfo.jump.cosAngle);
                *data << float(self->m_movementInfo.jump.sinAngle);
            }

            *data << uint32(self->m_movementInfo.jump.fallTime);
            *data << float(self->m_movementInfo.jump.zspeed);
        }

        data->WriteByteSeq(guid[1]);
        *data << float(self->GetSpeed(MOVE_TURN_RATE));

        if (self->m_movementInfo.time)
            *data << uint32(self->m_movementInfo.time);

        *data << float(self->GetSpeed(MOVE_RUN_BACK));

        if (hasSplineElevation)
            *data << float(self->m_movementInfo.splineElevation);

        data->WriteByteSeq(guid[7]);
        *data << float(self->GetSpeed(MOVE_PITCH_RATE));
        *data << float(self->GetPositionX());

        if (hasPitch)
            *data << float(self->m_movementInfo.pitch);

        if (!G3D::fuzzyEq(self->GetOrientation(), 0.0f))
            *data << float(Position::NormalizeOrientation(self->GetOrientation()));

        *data << float(self->GetSpeed(MOVE_WALK));
        *data << float(self->GetPositionY());
        *data << float(self->GetSpeed(MOVE_FLIGHT_BACK));
        data->WriteByteSeq(guid[3]);
        data->WriteByteSeq(guid[5]);
        data->WriteByteSeq(guid[6]);
        data->WriteByteSeq(guid[0]);
        *data << float(self->GetSpeed(MOVE_SWIM_BACK));
        *data << float(self->GetSpeed(MOVE_RUN));
        *data << float(self->GetSpeed(MOVE_SWIM));
        *data << float(self->GetPositionZMinusOffset());
    }

    if (hasAreaTriggerData)
    {
        if (hasAreaTriggerSpline)
        {
            for (auto&& itr : atrigger->GetPath())
                *data << float(itr.y) << float(itr.x) << float(itr.z);
        }

        *data << uint32(atrigger->GetMovementTime());
        if (hasAreaTriggerScale)
        {
            *data << float(atrigger->GetScaleX()); // scale
            *data << float(atrigger->GetScaleY()); // scale
        }
    }

    if (hasTransportPosition)
    {
        WorldObject const* self = static_cast<WorldObject const*>(this);
        ObjectGuid transGuid = self->m_movementInfo.transport.guid;

        if (self->m_movementInfo.transport.time2 && self->m_movementInfo.transport.guid)
            *data << uint32(self->m_movementInfo.transport.time2);

        *data << float(self->GetTransOffsetY());
        *data << int8(self->GetTransSeat());
        *data << float(self->GetTransOffsetX());
        data->WriteByteSeq(transGuid[2]);
        data->WriteByteSeq(transGuid[4]);
        data->WriteByteSeq(transGuid[1]);

        if (self->m_movementInfo.transport.time3 && self->m_movementInfo.transport.guid)
            *data << uint32(self->m_movementInfo.transport.time3);

        *data << uint32(self->GetTransTime());

        *data << float(self->GetTransOffsetO());
        *data << float(self->GetTransOffsetZ());

        data->WriteByteSeq(transGuid[6]);
        data->WriteByteSeq(transGuid[0]);
        data->WriteByteSeq(transGuid[5]);
        data->WriteByteSeq(transGuid[3]);
        data->WriteByteSeq(transGuid[7]);
    }

    if (hasTarget)
    {
        Unit const* self = ToUnit();
        ObjectGuid victimGuid = self->GetVictim()->GetGUID();
        data->WriteByteSeq(victimGuid[7]);
        data->WriteByteSeq(victimGuid[1]);
        data->WriteByteSeq(victimGuid[5]);
        data->WriteByteSeq(victimGuid[2]);
        data->WriteByteSeq(victimGuid[6]);
        data->WriteByteSeq(victimGuid[3]);
        data->WriteByteSeq(victimGuid[0]);
        data->WriteByteSeq(victimGuid[4]);
    }

    if (hasVehicle)
    {
        *data << uint32(ToUnit()->GetVehicleKit()->GetVehicleInfo()->m_ID);
        *data << float(GetTypeId() == TYPEID_UNIT ? ToCreature()->GetOriginalOrientation() : ToUnit()->GetOrientation());
    }

    if (hasStacionaryPostion)
    {
        WorldObject const* self = static_cast<WorldObject const*>(this);
        *data << float(self->GetStationaryY());
        *data << float(self->GetStationaryZ());
        *data << float(self->GetStationaryO());
        *data << float(self->GetStationaryX());
    }

    if (hasAnimKits)
    {
        WorldObject const* self = static_cast<WorldObject const*>(this);
        if (self->GetAIAnimKitId())
            *data << uint16(self->GetAIAnimKitId());
        if (self->GetMeleeAnimKitId())
            *data << uint16(self->GetMeleeAnimKitId());
        if (self->GetMovementAnimKitId())
            *data << uint16(self->GetMovementAnimKitId());
    }

    if (hasTransport)
    {
        GameObject const* go = ToGameObject();

        if (go && go->ToTransport())
            *data << uint32(go->GetGOValue()->Transport.PathProgress);
        else
            *data << uint32(getMSTime());
    }

    if (hasGobjectRotation)
        *data << uint64(ToGameObject()->GetPackedWorldRotation());

     if (hasLiving && hasSpline)
        Movement::PacketBuilder::WriteFacingTargetPart(*ToUnit()->movespline, *data);
}

void Object::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
{
    if (!target)
        return;

    UpdateBuilder builder;
    builder.SetSource(updateType == UPDATETYPE_VALUES ? _changesMask.GetBits() : m_uint32Values, m_valuesCount);
    builder.SetDest(data);

    uint32* flags = nullptr;
    uint32 visibleFlag = GetUpdateFieldData(target, flags);

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (_fieldNotifyFlags & flags[index] || (builder.GetSrcBit(index) && (flags[index] & visibleFlag)))
        {
            builder.SetDestBit(index);
            *data << m_uint32Values[index];
        }
    }

    builder.Finish();
    BuildDynamicValuesUpdate(updateType, data);
}

void Object::BuildDynamicValuesUpdate(uint8 updatetype, ByteBuffer *data) const
{
    if (!m_dynamicValuesCount)
    {
        *data << uint8(0);
        return;
    }

    // Dynamic Fields (5.0.5 MoP new fields)
    uint32 dynamicTabMask = 0;
    std::vector<uint32> dynamicFieldsMask;
    dynamicFieldsMask.resize(m_dynamicValues.size(), 0);

    for (size_t i = 0; i < m_dynamicValues.size(); i++)
    {
        if (!m_dynamicChange[i] && updatetype == UPDATETYPE_VALUES)
            continue;

        int32 pos = 0;
        for (uint16 index = 0; index < DynamicField::MaxSize; index++)
            if (m_dynamicValues[i].IsSet(index))
                dynamicFieldsMask[i] |= 1 << pos++;

        if (dynamicFieldsMask[i])
            dynamicTabMask |= 1 << i;
    }

    *data << uint8(1);  // Blocks count.
    *data << uint32(dynamicTabMask);
    for (size_t i = 0; i < m_dynamicValues.size(); ++i)
    {
        if ((1 << i) & dynamicTabMask)
        {
            *data << uint8(1);  // that & 07F == size of next mask in int32, i dunno wtf is this.
            *data << uint32(dynamicFieldsMask[i]);
            for (uint16 index = 0; index < DynamicField::MaxSize; index++)
                if (m_dynamicValues[i].IsSet(index))
                    *data << uint32(m_dynamicValues[i][index]);
        }
    }
}

void Object::ClearUpdateMask(bool remove)
{
    _changesMask.Clear();

    if (m_objectUpdated)
    {
        std::fill(m_dynamicChange.begin(), m_dynamicChange.end(), false);

        if (remove)
            RemoveFromUpdate();
        m_objectUpdated = false;
    }
}

void Object::BuildFieldsUpdate(Player* player, UpdateDataMapType& data_map) const
{
    UpdateDataMapType::iterator iter = data_map.find(player);

    if (iter == data_map.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = data_map.insert(UpdateDataMapType::value_type(player, UpdateData(player->GetMapId())));
        ASSERT(p.second);
        iter = p.first;
    }

    BuildValuesUpdateBlockForPlayer(&iter->second, iter->first);
}

uint32 Object::GetUpdateFieldData(Player const* target, uint32*& flags) const
{
    uint32 visibleFlag = UF_FLAG_PUBLIC | UF_FLAG_VIEWER_DEPENDENT;

    if (target == this)
        visibleFlag |= UF_FLAG_PRIVATE;

    switch (GetTypeId())
    {
        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
            flags = ItemUpdateFieldFlags;
            if (((Item*)this)->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER | UF_FLAG_ITEM_OWNER;
            break;
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
        {
            Player* plr = ToUnit()->GetCharmerOrOwnerPlayerOrPlayerItself();
            flags = UnitUpdateFieldFlags;
            if (ToUnit()->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;

            if (HasFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO))
                if (ToUnit()->HasAuraTypeWithCaster(SPELL_AURA_EMPATHY, target->GetGUID()))
                    visibleFlag |= UF_FLAG_SPECIAL_INFO;

            if (plr && plr->IsInSameRaidWith(target))
                visibleFlag |= UF_FLAG_PARTY_MEMBER;
            break;
        }
        case TYPEID_GAMEOBJECT:
            flags = GameObjectUpdateFieldFlags;
            if (ToGameObject()->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;
            break;
        case TYPEID_DYNAMICOBJECT:
            flags = DynamicObjectUpdateFieldFlags;
            if (((DynamicObject*)this)->GetCasterGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;
            break;
        case TYPEID_CORPSE:
            flags = CorpseUpdateFieldFlags;
            if (ToCorpse()->GetOwnerGUID() == target->GetGUID())
                visibleFlag |= UF_FLAG_OWNER;
            break;
        case TYPEID_AREATRIGGER:
            flags = AreaTriggerUpdateFieldFlags;
            break;
        case TYPEID_OBJECT:
            break;
        default:
            break;
    }

    return visibleFlag;
}

void Object::_LoadIntoDataField(std::string const& data, uint32 startOffset, uint32 count)
{
    if (data.empty())
        return;

    Tokenizer tokens(data, ' ', count);

    if (tokens.size() != count)
        return;

    for (uint32 index = 0; index < count; ++index)
    {
        m_uint32Values[startOffset + index] = atol(tokens[index]);
        _changesMask.SetBit(startOffset + index);
    }
}

void Object::SetInt32Value(uint16 index, int32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_int32Values[index] != value)
    {
        m_int32Values[index] = value;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::SetUInt32Value(uint16 index, uint32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_uint32Values[index] != value)
    {
        m_uint32Values[index] = value;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::SetDynamicUInt32Value(uint32 tab, uint16 index, uint32 value)
{
    ASSERT(tab < m_dynamicValues.size() && index < DynamicField::MaxSize);
    m_dynamicChange[tab] = true;

    if (m_dynamicValues[tab][index] != value || !m_dynamicValues[tab].IsSet(index))
    {
        m_dynamicValues[tab].Set(index, value);
        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::RemoveDynamicValue(uint32 tab, uint16 index)
{
    ASSERT(tab < m_dynamicValues.size() && index < DynamicField::MaxSize);
    m_dynamicChange[tab] = true;
    m_dynamicValues[tab].Remove(index);
}

void Object::UpdateUInt32Value(uint16 index, uint32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    m_uint32Values[index] = value;
    _changesMask.SetBit(index);
}

void Object::SetUInt64Value(uint16 index, uint64 value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((uint64*)&(m_uint32Values[index])) != value)
    {
        m_uint32Values[index] = PAIR64_LOPART(value);
        m_uint32Values[index + 1] = PAIR64_HIPART(value);
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

bool Object::AddGuidValue(uint16 index, ObjectGuid value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (value && !*((ObjectGuid*)&(m_uint32Values[index])))
    {
        *((ObjectGuid*)&(m_uint32Values[index])) = value;
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();

        return true;
    }

    return false;
}

bool Object::RemoveGuidValue(uint16 index, ObjectGuid value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (value && *((ObjectGuid*)&(m_uint32Values[index])) == value)
    {
        m_uint32Values[index] = 0;
        m_uint32Values[index + 1] = 0;
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();

        return true;
    }

    return false;
}

void Object::SetFloatValue(uint16 index, float value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_floatValues[index] != value)
    {
        m_floatValues[index] = value;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        TC_LOG_ERROR("misc", "Object::SetByteValue: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[index] >> (offset * 8)) != value)
    {
        m_uint32Values[index] &= ~uint32(uint32(0xFF) << (offset * 8));
        m_uint32Values[index] |= uint32(uint32(value) << (offset * 8));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 2)
    {
        TC_LOG_ERROR("misc", "Object::SetUInt16Value: wrong offset %u", offset);
        return;
    }

    if (uint16(m_uint32Values[index] >> (offset * 16)) != value)
    {
        m_uint32Values[index] &= ~uint32(uint32(0xFFFF) << (offset * 16));
        m_uint32Values[index] |= uint32(uint32(value) << (offset * 16));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::SetGuidValue(uint16 index, ObjectGuid value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((ObjectGuid*)&(m_uint32Values[index])) != value)
    {
        *((ObjectGuid*)&(m_uint32Values[index])) = value;
        _changesMask.SetBit(index);
        _changesMask.SetBit(index + 1);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::SetStatFloatValue(uint16 index, float value)
{
    if (value < 0)
        value = 0.0f;

    SetFloatValue(index, value);
}

void Object::SetStatInt32Value(uint16 index, int32 value)
{
    if (value < 0)
        value = 0;

    SetUInt32Value(index, uint32(value));
}

void Object::ApplyModUInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetUInt32Value(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetUInt32Value(index, cur);
}

void Object::ApplyModInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetInt32Value(index);
    cur += (apply ? val : -val);
    SetInt32Value(index, cur);
}

void Object::ApplyModSignedFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    SetFloatValue(index, cur);
}

void Object::ApplyPercentModFloatValue(uint16 index, float val, bool apply)
{
    float value = GetFloatValue(index);
    ApplyPercentModFloatVar(value, val, apply);
    SetFloatValue(index, value);
}

void Object::ApplyModPositiveFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetFloatValue(index, cur);
}

void Object::SetFlag(uint16 index, uint32 newFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[index];
    uint32 newval = oldval | newFlag;

    if (oldval != newval)
    {
        m_uint32Values[index] = newval;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::RemoveFlag(uint16 index, uint32 oldFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    ASSERT(m_uint32Values);

    uint32 oldval = m_uint32Values[index];
    uint32 newval = oldval & ~oldFlag;

    if (oldval != newval)
    {
        m_uint32Values[index] = newval;
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::ToggleFlag(uint16 index, uint32 flag)
{
    if (HasFlag(index, flag))
        RemoveFlag(index, flag);
    else
        SetFlag(index, flag);
}

bool Object::HasFlag(uint16 index, uint32 flag) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    return (m_uint32Values[index] & flag) != 0;    
}

void Object::ApplyModFlag(uint16 index, uint32 flag, bool apply)
{
    if (apply) SetFlag(index, flag); else RemoveFlag(index, flag);
}

void Object::SetByteFlag(uint16 index, uint8 offset, uint8 newFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        TC_LOG_ERROR("misc", "Object::SetByteFlag: wrong offset %u", offset);
        return;
    }

    if (!(uint8(m_uint32Values[index] >> (offset * 8)) & newFlag))
    {
        m_uint32Values[index] |= uint32(uint32(newFlag) << (offset * 8));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::RemoveByteFlag(uint16 index, uint8 offset, uint8 oldFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        TC_LOG_ERROR("misc", "Object::RemoveByteFlag: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[index] >> (offset * 8)) & oldFlag)
    {
        m_uint32Values[index] &= ~uint32(uint32(oldFlag) << (offset * 8));
        _changesMask.SetBit(index);

        if (m_inWorld && !m_objectUpdated)
            AddToUpdate();
    }
}

void Object::ToggleByteFlag(uint16 index, uint8 offset, uint8 flag)
{
    if (HasByteFlag(index, offset, flag))
        RemoveByteFlag(index, offset, flag);
    else
        SetByteFlag(index, offset, flag);
}

bool Object::HasByteFlag(uint16 index, uint8 offset, uint8 flag) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    ASSERT(offset < 4);
    return (((uint8*)&m_uint32Values[index])[offset] & flag) != 0;
}

void Object::SetFlag64(uint16 index, uint64 newFlag)
{
    uint64 oldval = GetUInt64Value(index);
    uint64 newval = oldval | newFlag;
    SetUInt64Value(index, newval);
}

void Object::RemoveFlag64(uint16 index, uint64 oldFlag)
{
    uint64 oldval = GetUInt64Value(index);
    uint64 newval = oldval & ~oldFlag;
    SetUInt64Value(index, newval);
}

void Object::ToggleFlag64(uint16 index, uint64 flag)
{
    if (HasFlag64(index, flag))
        RemoveFlag64(index, flag);
    else
        SetFlag64(index, flag);
}

bool Object::HasFlag64(uint16 index, uint64 flag) const
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, false));
    return (GetUInt64Value(index) & flag) != 0;
}

void Object::ApplyModFlag64(uint16 index, uint64 flag, bool apply)
{
    if (apply) SetFlag64(index, flag); else RemoveFlag64(index, flag);
}

bool Object::PrintIndexError(uint32 index, bool set) const
{
    TC_LOG_ERROR("misc", "Attempt %s non-existed value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, GetTypeId(), m_objectType);

    // ASSERT must fail after function call
    return false;
}

void MovementInfo::OutDebug()
{
    TC_LOG_INFO("misc", "MOVEMENT INFO");
    TC_LOG_INFO("misc", "guid " UI64FMTD, guid);
    TC_LOG_INFO("misc", "flags %s (%u)", Movement::MovementFlags_ToString(flags).c_str(), flags);
    TC_LOG_INFO("misc", "flags2 %s (%u)", Movement::MovementFlagsExtra_ToString(flags2).c_str(), flags2);
    TC_LOG_INFO("misc", "time %u current time %u", time, getMSTime());
    TC_LOG_INFO("misc", "position: `%s`", pos.ToString().c_str());
    if (transport.guid)
    {
        TC_LOG_INFO("misc", "TRANSPORT:");
        TC_LOG_INFO("misc", "guid: " UI64FMTD, transport.guid.GetRawValue());
        TC_LOG_INFO("misc", "position: `%s`", transport.pos.ToString().c_str());
        TC_LOG_INFO("misc", "seat: %i", transport.seat);
        TC_LOG_INFO("misc", "time: %u", transport.time);
        if (flags2 & MOVEMENTFLAG2_INTERPOLATED_MOVEMENT)
            TC_LOG_INFO("misc", "time2: %u", transport.time2);
        if (transport.time3)
            TC_LOG_INFO("misc", "time3: %u", transport.time3);
    }

    if ((flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING))
        TC_LOG_INFO("misc", "pitch: %f", pitch);

    if (flags & MOVEMENTFLAG_FALLING || jump.fallTime)
    {
        TC_LOG_INFO("misc", "fallTime: %u j_zspeed: %f", jump.fallTime, jump.zspeed);
        if (flags & MOVEMENTFLAG_FALLING)
            TC_LOG_INFO("misc", "j_sinAngle: %f j_cosAngle: %f j_xyspeed: %f", jump.sinAngle, jump.cosAngle, jump.xyspeed);
    }

    if (flags & MOVEMENTFLAG_SPLINE_ELEVATION)
        TC_LOG_INFO("misc", "splineElevation: %f", splineElevation);
}

WorldObject::WorldObject(bool isWorldObject): Object(), WorldLocation(), LastUsedScriptID(0),
m_name(""), m_isActive(false), m_isWorldObject(isWorldObject), m_zoneScript(nullptr),
m_transport(nullptr), m_zoneId(0), m_areaId(0), m_outdoors(false), m_liquidStatus(LIQUID_MAP_NO_WATER),
m_currMap(nullptr), m_InstanceId(0), m_phaseMask(PHASEMASK_NORMAL), m_explicitSeerGuid(),
m_stealthVisibilityUpdateTimer(STEALTH_VISIBILITY_UPDATE_TIMER)
{
    m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE | GHOST_VISIBILITY_GHOST);
    m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
}

void WorldObject::SetWorldObject(bool on)
{
    if (!IsInWorld())
        return;

    GetMap()->AddObjectToSwitchList(this, on);
}

bool WorldObject::IsWorldObject() const
{
    if (m_isWorldObject)
        return true;

    if (ToCreature() && ToCreature()->m_isTempWorldObject)
        return true;

    return false;
}


void WorldObject::setActive(bool on, ActiveFlags flag)
{
    if (GetTypeId() == TYPEID_PLAYER)
        return;

    // Set or remove a flag indicating the reason why this object should be active
    if (on)
        m_activeFlags = (ActiveFlags)((uint32)m_activeFlags | (uint32)flag);
    else
        m_activeFlags = (ActiveFlags)((uint32)m_activeFlags & ~(uint32)flag);

    // Only allow active objects to be turned off if all reasons are removed
    if (m_activeFlags != ActiveFlags::None)
        on = true;

    if (m_isActive == on)
        return;

    m_isActive = on;

    // Allow active to be turned off even if the object is no longer in world, but still has a map set.
    // Removing them from active list (or even an attempt of removal) will cause no harm, but adding them to the list might.
    if (on && !IsInWorld())
        return;

    Map *map = FindMap();
    if (!map)
        return;

    if (on)
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->AddToActive(this->ToCreature());
        else if (GetTypeId() == TYPEID_GAMEOBJECT)
            map->AddToActive((GameObject*)this);
        else if (GetTypeId() == TYPEID_DYNAMICOBJECT)
            map->AddToActive((DynamicObject*)this);
    }
    else
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->RemoveFromActive(this->ToCreature());
        else if (GetTypeId() == TYPEID_GAMEOBJECT)
            map->RemoveFromActive((GameObject*)this);
        else if (GetTypeId() == TYPEID_DYNAMICOBJECT)
            map->RemoveFromActive((DynamicObject*)this);
    }
}

void WorldObject::SetFarVisible(bool on)
{
    if (GetTypeId() == TYPEID_PLAYER)
        return;

    m_isFarVisible = on;
}

void WorldObject::SetVisibilityDistanceOverride(VisibilityDistanceType type)
{
    ASSERT(type < VisibilityDistanceType::Max);
    if (GetTypeId() == TYPEID_PLAYER)
        return;

    m_visibilityDistanceOverride = VisibilityDistances[AsUnderlyingType(type)];
}

void WorldObject::CleanupsBeforeDelete(bool /*finalCleanup*/)
{
    if (IsInWorld())
        RemoveFromWorld();

    if (Transport* transport = GetTransport())
        transport->RemovePassenger(this);
}

void WorldObject::_Create(uint32 guidlow, HighGuid guidhigh, uint32 phaseMask, std::set<uint32> const& phaseIds)
{
    Object::_Create(guidlow, 0, guidhigh);
    m_phaseMask = phaseMask;
    for (auto itr : phaseIds)
        _phases.insert(itr);
}

void WorldObject::UpdatePositionData()
{
    PositionFullTerrainStatus data;
    GetMap()->GetFullTerrainStatusForPosition(GetPhaseMask(), GetPositionX(), GetPositionY(), GetPositionZ(), data, MAP_ALL_LIQUIDS, GetCollisionHeight());
    ProcessPositionDataChanged(data);
}

void WorldObject::ProcessPositionDataChanged(PositionFullTerrainStatus const& data)
{
    m_zoneId = m_areaId = data.areaId;
    if (AreaTableEntry const* area = sAreaTableStore.LookupEntry(m_areaId))
        if (area->ParentAreaID)
            m_zoneId = area->ParentAreaID;
    m_outdoors = data.outdoors;
    m_staticFloorZ = data.floorZ;
    m_liquidStatus = data.liquidStatus;
}

void WorldObject::AddToWorld()
{
    Object::AddToWorld();
    GetMap()->GetZoneAndAreaId(GetPhaseMask(), m_zoneId, m_areaId, GetPositionX(), GetPositionY(), GetPositionZ()); 
}

void WorldObject::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    DestroyForNearbyPlayers();

    Object::RemoveFromWorld();
}

uint32 WorldObject::GetZoneId() const
{
    return GetBaseMap()->GetZoneId(GetPhaseMask(), m_positionX, m_positionY, m_positionZ);
}

uint32 WorldObject::GetAreaId() const
{
    return GetBaseMap()->GetAreaId(GetPhaseMask(), m_positionX, m_positionY, m_positionZ);
}

void WorldObject::GetZoneAndAreaId(uint32& zoneid, uint32& areaid) const
{
    GetBaseMap()->GetZoneAndAreaId(GetPhaseMask(), zoneid, areaid, m_positionX, m_positionY, m_positionZ);
}

InstanceScript* WorldObject::GetInstanceScript()
{
    Map* map = GetMap();
    return map->IsDungeon() || map->IsScenario() ? ((InstanceMap*)map)->GetInstanceScript() : NULL;
}

float WorldObject::GetDistanceZ(const WorldObject* obj) const
{
    float dz = fabs(GetPositionZ() - obj->GetPositionZ());
    float sizefactor = GetObjectSize() + obj->GetObjectSize();
    float dist = dz - sizefactor;
    return (dist > 0 ? dist : 0);
}

bool WorldObject::_IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D, bool incOwnRadius, bool incTargetRadius) const
{
    float sizefactor = GetObjectSize() + obj->GetObjectSize();
    float maxdist = dist2compare + sizefactor;

    if (GetTransport() && obj->GetTransport() &&  obj->GetTransport()->GetGUID() == GetTransport()->GetGUID())
    {
        float dtx = m_movementInfo.transport.pos.m_positionX - obj->m_movementInfo.transport.pos.m_positionX;
        float dty = m_movementInfo.transport.pos.m_positionY - obj->m_movementInfo.transport.pos.m_positionY;
        float disttsq = dtx * dtx + dty * dty;
        if (is3D)
        {
            float dtz = m_movementInfo.transport.pos.m_positionZ - obj->m_movementInfo.transport.pos.m_positionZ;
            disttsq += dtz * dtz;
        }
        return disttsq < (maxdist * maxdist);
    }

    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx*dx + dy*dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz*dz;
    }

    return distsq < maxdist * maxdist;
}

float WorldObject::GetDistance(const WorldObject* obj) const
{
    float d = GetExactDist(obj) - GetObjectSize() - obj->GetObjectSize();
    return d > 0.0f ? d : 0.0f;
}

float WorldObject::GetDistance(const Position &pos) const
{
    float d = GetExactDist(&pos) - GetObjectSize();
    return d > 0.0f ? d : 0.0f;
}

float WorldObject::GetDistance(float x, float y, float z) const
{
    float d = GetExactDist(x, y, z) - GetObjectSize();
    return d > 0.0f ? d : 0.0f;
}

float WorldObject::GetDistance2d(const WorldObject* obj) const
{
    float d = GetExactDist2d(obj) - GetObjectSize() - obj->GetObjectSize();
    return d > 0.0f ? d : 0.0f;
}

float WorldObject::GetDistance2d(float x, float y) const
{
    float d = GetExactDist2d(x, y) - GetObjectSize();
    return d > 0.0f ? d : 0.0f;
}

bool WorldObject::IsSelfOrInSameMap(const WorldObject* obj) const
{
    if (this == obj)
        return true;
    return IsInMap(obj);
}

bool WorldObject::IsInMap(const WorldObject* obj) const
{
    if (obj)
        return IsInWorld() && obj->IsInWorld() && (GetMap() == obj->GetMap());
    return false;
}

bool WorldObject::IsWithinDist3d(float x, float y, float z, float dist) const
{
    return IsInDist(x, y, z, dist + GetObjectSize());
}

bool WorldObject::IsWithinDist3d(const Position* pos, float dist) const
{
    return IsInDist(pos, dist + GetObjectSize());
}

bool WorldObject::IsWithinDist2d(float x, float y, float dist) const
{
    return IsInDist2d(x, y, dist + GetObjectSize());
}

bool WorldObject::IsWithinDist2d(const Position* pos, float dist) const
{
    return IsInDist2d(pos, dist + GetObjectSize());
}

bool WorldObject::IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D /*= true*/) const
{
    return obj && _IsWithinDist(obj, dist2compare, is3D);
}

bool WorldObject::IsWithinDistInMap(WorldObject const* obj, float dist2compare, bool is3D /*= true*/, bool incOwnRadius /*= true*/, bool incTargetRadius /*= true*/) const
{
    return obj && IsInMap(obj) && InSamePhase(obj) && _IsWithinDist(obj, dist2compare, is3D, incOwnRadius, incTargetRadius);
}

Position WorldObject::GetHitSpherePointFor(Position const& dest) const
{
    G3D::Vector3 vThis(GetPositionX(), GetPositionY(), GetPositionZ() + GetCollisionHeight());
    G3D::Vector3 vObj(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ());
    G3D::Vector3 contactPoint = vThis + (vObj - vThis).directionOrZero() * std::min(dest.GetExactDist(GetPosition()), GetCombatReach());

    return Position(contactPoint.x, contactPoint.y, contactPoint.z, GetAbsoluteAngle(contactPoint.x, contactPoint.y));
}

bool WorldObject::IsWithinLOS(float ox, float oy, float oz, VMAP::ModelIgnoreFlags ignoreFlags) const
{
    if (IsInWorld())
    {
        oz += GetCollisionHeight();
        float x, y, z;
        if (GetTypeId() == TYPEID_PLAYER)
        {
            GetPosition(x, y, z);
            z += GetCollisionHeight();
        }
        else
            GetHitSpherePointFor({ ox, oy, oz }, x, y, z);

        return GetMap()->isInLineOfSight(x, y, z, ox, oy, oz, GetPhaseMask(), ignoreFlags);  // missing checks todo 
    }

    return true;
}

bool WorldObject::IsWithinLOSInMap(const WorldObject* obj, VMAP::ModelIgnoreFlags ignoreFlags) const
{
    if (!IsInMap(obj))
        return false;

    float ox, oy, oz;
    if (obj->GetTypeId() == TYPEID_PLAYER)
    {
        obj->GetPosition(ox, oy, oz);
        oz += GetCollisionHeight();
    }
    else
        obj->GetHitSpherePointFor({ GetPositionX(), GetPositionY(), GetPositionZ() + GetCollisionHeight() }, ox, oy, oz);

    float x, y, z;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        GetPosition(x, y, z);
        z += GetCollisionHeight();
    }
    else
        GetHitSpherePointFor({ obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ() + obj->GetCollisionHeight() }, x, y, z);

    if (obj->GetTypeId() == TYPEID_UNIT)
        switch (obj->GetEntry())
        {
            // Hack fix for Burning Tendons (Spine of Deathwing)
            case 56341:
            case 56575:
            // Hack fix of Ozumat (Throne of the Tides)
            case 44566:
            // Hack fix for Ice Wall (Throne of Thunder)
            case 69582:
            // Hack fix for Ice Tomb (Siege of Orgrimmar)
            case 69398:
            // Hack fix for Encase in Amber (Siege of Orgrimmar)
            case 71407:
            // Hack fix for Siege Engineer (Siege of Orgrimmar)
            case 71984:
            // Hack fix for Incompleted Drakari Colossus ( Isle of Thunder)
            case 69347:
                return true;
            default:
                break;
        }

    // AoE spells
    if (GetTypeId() == TYPEID_UNIT)
        switch (GetEntry())
        {
            // Hack fix for Burning Tendons (Spine of Deathwing)
            case 56341:
            case 56575:
            // Hack fix for Ice Wall (Throne of Thunder)
            case 69582:
            // Hack fix for Ice Tomb (Siege of Orgrimmar)
            case 69398:
            // Hack fix for Encase in Amber (Siege of Orgrimmar)
            case 71407:
            // Hack fix for Siege Engineer (Siege of Orgrimmar)
            case 71984:
            // Hack fix for Incompleted Drakari Colossus ( Isle of Thunder)
            case 69347:
                return true;
            default:
                break;
        }

    // Hack fix for Alysrazor
    if (GetMapId() == 720 && GetAreaId() == 5766)
        if ((GetTypeId() == TYPEID_PLAYER) || (obj->GetTypeId() == TYPEID_PLAYER))
            return true;

    return GetMap()->isInLineOfSight(x, y, z, ox, oy, oz, GetPhaseMask(), ignoreFlags); // missing checks todo 
}

void WorldObject::GetHitSpherePointFor(Position const& dest, float& x, float& y, float& z) const
{
    Position pos = GetHitSpherePointFor(dest);
    x = pos.GetPositionX();
    y = pos.GetPositionY();
    z = pos.GetPositionZ();
}

bool WorldObject::GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D /* = true */) const
{
    float dx1 = GetPositionX() - obj1->GetPositionX();
    float dy1 = GetPositionY() - obj1->GetPositionY();
    float distsq1 = dx1*dx1 + dy1*dy1;
    if (is3D)
    {
        float dz1 = GetPositionZ() - obj1->GetPositionZ();
        distsq1 += dz1*dz1;
    }

    float dx2 = GetPositionX() - obj2->GetPositionX();
    float dy2 = GetPositionY() - obj2->GetPositionY();
    float distsq2 = dx2*dx2 + dy2*dy2;
    if (is3D)
    {
        float dz2 = GetPositionZ() - obj2->GetPositionZ();
        distsq2 += dz2*dz2;
    }

    return distsq1 < distsq2;
}

bool WorldObject::IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D /* = true */) const
{
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx*dx + dy*dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz*dz;
    }

    float sizefactor = GetObjectSize() + obj->GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange2d(float x, float y, float minRange, float maxRange) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float distsq = dx*dx + dy*dy;

    float sizefactor = GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange3d(float x, float y, float z, float minRange, float maxRange) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float distsq = dx*dx + dy*dy + dz*dz;

    float sizefactor = GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

void Position::RelocateOffset(const Position & offset)
{
    m_positionX = GetPositionX() + (offset.GetPositionX() * std::cos(GetOrientation()) + offset.GetPositionY() * std::sin(GetOrientation() + M_PI));
    m_positionY = GetPositionY() + (offset.GetPositionY() * std::cos(GetOrientation()) + offset.GetPositionX() * std::sin(GetOrientation()));
    m_positionZ = GetPositionZ() + offset.GetPositionZ();
    SetOrientation(GetOrientation() + offset.GetOrientation());
}

void Position::RelocateOffset(float angle, float distance, float offsetZ)
{
    angle += GetOrientation();
    m_positionX += cos(angle) * distance;
    m_positionY += sin(angle) * distance;
    m_positionZ += offsetZ;
}

void Position::GetPositionOffsetTo(const Position & endPos, Position & retOffset) const
{
    float dx = endPos.GetPositionX() - GetPositionX();
    float dy = endPos.GetPositionY() - GetPositionY();

    retOffset.m_positionX = dx * std::cos(GetOrientation()) + dy * std::sin(GetOrientation());
    retOffset.m_positionY = dy * std::cos(GetOrientation()) - dx * std::sin(GetOrientation());
    retOffset.m_positionZ = endPos.GetPositionZ() - GetPositionZ();
    retOffset.SetOrientation(endPos.GetOrientation() - GetOrientation());
}

float Position::GetAngle(const Position* obj) const
{
    if (!obj)
        return 0;

    return GetAngle(obj->GetPositionX(), obj->GetPositionY());
}

// Return angle in range 0..2*pi
float Position::GetAngle(const float x, const float y) const
{
    float dx = x - GetPositionX();
    float dy = y - GetPositionY();

    float ang = atan2(dy, dx);
    ang = (ang >= 0) ? ang : 2 * M_PI + ang;
    return ang;
}

void Position::GetSinCos(const float x, const float y, float &vsin, float &vcos) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;

    if (fabs(dx) < 0.001f && fabs(dy) < 0.001f)
    {
        float angle = (float)rand_norm()*static_cast<float>(2*M_PI);
        vcos = std::cos(angle);
        vsin = std::sin(angle);
    }
    else
    {
        float dist = sqrt((dx*dx) + (dy*dy));
        vcos = dx / dist;
        vsin = dy / dist;
    }
}

bool Position::HasInArc(float arc, const Position* obj, float border) const
{
    // always have self in arc
    if (obj == this)
        return true;

    // move arc to range 0.. 2*pi
    arc = NormalizeOrientation(arc);

    float angle = GetAngle(obj);
    angle -= GetOrientation();

    // move angle to range -pi ... +pi
    angle = NormalizeOrientation(angle);
    if (angle > M_PI)
        angle -= 2.0f*M_PI;

    float lborder = -1 * (arc/border);                        // in range -pi..0
    float rborder = (arc/border);                             // in range 0..pi
    return ((angle >= lborder) && (angle <= rborder));
}

bool WorldObject::IsInBetween(const Position* obj1, const Position* obj2, float size) const
{
    if (!obj1 || !obj2)
        return false;

    float dist = GetExactDist2d(obj1->GetPositionX(), obj1->GetPositionY());

    // not using sqrt() for performance
    if ((dist * dist) >= obj1->GetExactDist2dSq(obj2->GetPositionX(), obj2->GetPositionY()))
        return false;

    if (!size)
        size = GetObjectSize() / 2;

    float angle = obj1->GetAngle(obj2);

    // not using sqrt() for performance
    return (size * size) >= GetExactDist2dSq(obj1->GetPositionX() + std::cos(angle) * dist, obj1->GetPositionY() + std::sin(angle) * dist);
}

bool WorldObject::IsInAxe(const WorldObject* obj1, const WorldObject* obj2, float size) const
{
    if (!obj1 || !obj2)
        return false;

    float dist = GetExactDist2d(obj1->GetPositionX(), obj1->GetPositionY());

    if (!size)
        size = GetObjectSize() / 2;

    float angle = obj1->GetAngle(obj2);

    // not using sqrt() for performance
    return (size * size) >= GetExactDist2dSq(obj1->GetPositionX() + cos(angle) * dist, obj1->GetPositionY() + sin(angle) * dist);
}

bool WorldObject::isInFront(WorldObject const* target,  float arc) const
{
    return HasInArc(arc, target);
}

bool WorldObject::isInBack(WorldObject const* target, float arc) const
{
    return !HasInArc(2 * M_PI - arc, target);
}

void WorldObject::GetRandomPoint(const Position &pos, float distance, float &rand_x, float &rand_y, float &rand_z) const
{
    if (!distance)
    {
        pos.GetPosition(rand_x, rand_y, rand_z);
        return;
    }

    // angle to face `obj` to `this`
    float angle = (float)rand_norm()*static_cast<float>(2*M_PI);
    float new_dist = (float)rand_norm()*static_cast<float>(distance);
    new_dist = distance * (new_dist > 1 ? new_dist - 2 : new_dist);

    rand_x = pos.m_positionX + new_dist * std::cos(angle);
    rand_y = pos.m_positionY + new_dist * std::sin(angle);
    rand_z = pos.m_positionZ;

    Trinity::NormalizeMapCoord(rand_x);
    Trinity::NormalizeMapCoord(rand_y);
    UpdateGroundPositionZ(rand_x, rand_y, rand_z);            // update to LOS height if available
}

Position WorldObject::GetRandomPoint(const Position &srcPos, float distance) const
{
    float x, y, z;
    GetRandomPoint(srcPos, distance, x, y, z);
    return Position(x, y, z, GetOrientation());
}

void WorldObject::UpdateGroundPositionZ(float x, float y, float &z) const
{
    float new_z = GetMapHeight(x, y, z);
    if (new_z > INVALID_HEIGHT)
        z = new_z + (isType(TYPEMASK_UNIT) ? static_cast<Unit const*>(this)->GetHoverOffset() : 0.0f);
}

bool IgnoreMultipleFloors(uint32 entry)
{
    switch (entry)
    {
        // Ulduar
        case 33293: // XT-002 Deconstructor
        case 33343: // XS-013 Scrapbot
        case 33344: // XM-024 Pummeller
        case 33346: // XE-321 Boombot
        case 34271: // XD-175 Compactobot
        case 34269: // XR-949 Salvagebot
        case 33329: // Heart of the Deconstructor
        case 34267: // Parts Recovery Technician
            return true;
        default:
            return false;
    }
}

/**
 * @brief Get the minimum height of a object that should be in water
 * to start floating/swim
 *
 * @return float
 */
float WorldObject::GetMinHeightInWater() const
{
    // have a fun with Archimedes' formula
    auto height = GetCollisionHeight();
    auto width = GetCollisionWidth();
    auto weight = getWeight(height, width, 1040); // avg human specific weight
    auto heightOutOfWater = getOutOfWater(width, weight, 10202) * 4.0f; // avg human density
    auto heightInWater = height - heightOutOfWater;
    return (height > heightInWater ? heightInWater : (height - (height / 3)));
}

void WorldObject::UpdateAllowedPositionZ(float x, float y, float &z, float* groundZ) const
{
    if (GetMapId() == 669) // Blackwing Descent
    {
        if (GetTypeId() == TYPEID_UNIT && GetEntry() == 41948)
            return;

        float const floor = (x + 108.2840673f) * (x + 108.2840673f) + (y + 224.4677698f) * (y + 224.4677698f) < 400 ? 6.489f : 8.472f;
        if (z < floor)
            z = floor;

        if (GetTypeId() == TYPEID_UNIT && (GetEntry() == 41376 && !ToUnit()->HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY) || GetEntry() == 41270 || GetEntry() == 41918)) // Nefarian or Onyxia or Animated Bone Warrior
            if (z > 6.489f)
                z = 6.489f;
    }

    // TODO: Allow transports to be part of dynamic vmap tree
    if (GetTransport())
    {
        if (groundZ)
            *groundZ = z;

        return;
    }

    if (Unit const* unit = ToUnit())
    {
        if (!unit->CanFly())
        {
            bool canSwim = unit->CanSwim();
            float ground_z = z;
            float max_z;
            if (canSwim)
                max_z = GetMapWaterOrGroundLevel(x, y, z, &ground_z);
            else
                max_z = ground_z = GetMapHeight(x, y, z);

            if (max_z > INVALID_HEIGHT)
            {
                // hovering units cannot go below their hover height
                float hoverOffset = unit->GetHoverOffset();
                max_z += hoverOffset;
                ground_z += hoverOffset;

                if (z > max_z)
                    z = max_z;
                else if (z < ground_z)
                    z = ground_z;
            }

            if (groundZ)
                *groundZ = ground_z;
        }
        else
        {
            float ground_z = GetMapHeight(x, y, z) + unit->GetHoverOffset();
            if (z < ground_z)
                z = ground_z;

            if (groundZ)
               *groundZ = ground_z;
        }
    }
    else
    {
        float ground_z = GetMapHeight(x, y, z);
        if (ground_z > INVALID_HEIGHT)
            z = ground_z;

        if (groundZ)
            *groundZ = ground_z;
    }
}

bool Position::IsPositionValid() const
{
    return Trinity::IsValidMapCoord(m_positionX, m_positionY, m_positionZ, GetOrientation());
}

float WorldObject::GetGridActivationRange() const
{
    if (isActiveObject())
    {
        if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->GetCinematicMgr()->IsOnCinematic())
            return std::max(DEFAULT_VISIBILITY_INSTANCE, GetMap()->GetVisibilityRange());

        return GetMap()->GetVisibilityRange();
    }

    if (Creature const* thisCreature = ToCreature())
        return thisCreature->m_SightDistance;

    return 0.0f;
}

float WorldObject::GetVisibilityRange() const
{
    if (HasCustomVisibility())
        return GetCustomVisibilityDistance();
    //if (isActiveObject() && !ToPlayer())
    //    return MAX_VISIBILITY_DISTANCE;
    else
        return GetMap()->GetVisibilityRange();
}

float WorldObject::GetSightRange(const WorldObject* target) const
{
    if (ToUnit())
    {
        if (ToPlayer())
        {
            if (target && target->HasCustomVisibility())
                return target->GetCustomVisibilityDistance();
            else if (GetMapId() == 967 && GetAreaId() == 5893) // Dragon Soul - Maelstorm
                return 500.0f;
            else if (GetMapId() == 754) // Throne of the Four Winds
                return MAX_VISIBILITY_DISTANCE;
            else if (ToPlayer()->GetCinematicMgr()->IsOnCinematic())
                return DEFAULT_VISIBILITY_INSTANCE;
            else
                return GetMap()->GetVisibilityRange();
        }
        else if (ToCreature())
            return ToCreature()->m_SightDistance;
        else
            return SIGHT_RANGE_UNIT;
    }

    if (ToDynObject() && isActiveObject())
    {
        return GetMap()->GetVisibilityRange();
    }

    return 0.0f;
}

bool WorldObject::CheckPrivateObjectOwnerVisibility(WorldObject const* seer) const
{
    if (!IsPrivateObject())
        return true;

    // Owner of this private object
    if (_privateObjectOwner == seer->GetGUID())
        return true;

    // Another private object of the same owner
    if (_privateObjectOwner == seer->GetPrivateObjectOwner())
        return true;

    if (Player const* playerSeer = seer->ToPlayer())
        if (playerSeer->IsInGroup(_privateObjectOwner))
            return true;

    return false;
}

bool WorldObject::CanSeeOrDetect(WorldObject const* obj, bool ignoreStealth, bool distanceCheck) const
{
    if (this == obj)
        return true;

    if (obj->IsNeverVisibleFor(this, ignoreStealth) || CanNeverSee(obj))
        return false;

    if (obj->IsAlwaysVisibleFor(this) || CanAlwaysSee(obj))
        return true;

    if (!obj->CheckPrivateObjectOwnerVisibility(this))
        return false;

    bool corpseVisibility = false;
    if (distanceCheck)
    {
        bool corpseCheck = false;
        if (Player const* thisPlayer = ToPlayer())
        {
            if (thisPlayer->isDead() && thisPlayer->GetHealth() > 0 && // Cheap way to check for ghost state
                !(obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & GHOST_VISIBILITY_GHOST))
            {
                if (Corpse* corpse = thisPlayer->GetCorpse())
                {
                    corpseCheck = true;
                    if (corpse->IsWithinDist(thisPlayer, GetSightRange(obj), false))
                        if (corpse->IsWithinDist(obj, GetSightRange(obj), false))
                            corpseVisibility = true;
                }
            }
        }

        WorldObject const* viewpoint = this;
        if (Player const* player = this->ToPlayer())
        {
            viewpoint = player->GetViewpoint();

            // Check Allow visible by entry
            if (auto info = sObjectMgr->GetObjectVisibilityStateData(obj->GetEntry()))
                if (info->type == (obj->GetTypeId() == TYPEID_GAMEOBJECT ? ObjectVisibilityState::objectType::GameObject : ObjectVisibilityState::objectType::Creature) && player->GetQuestStatus(info->questId) != info->questState)
                    return false;

            // Allow visible by guid
            if (auto info = sObjectMgr->GetObjectVisibilityStateData(-(int32)obj->GetGUID().GetCounter()))
                if (info->type == (obj->GetTypeId() == TYPEID_GAMEOBJECT ? ObjectVisibilityState::objectType::GameObject : ObjectVisibilityState::objectType::Creature) && player->GetQuestStatus(info->questId) != info->questState)
                    return false;
        }

        if (!viewpoint)
            viewpoint = this;

        if (!corpseCheck && !viewpoint->IsWithinDist(obj, GetSightRange(obj), false))
            return false;
    }

    // GM visibility off or hidden NPC
    if (!obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM))
    {
        // Stop checking other things for GMs
        if (m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GM))
            return true;
    }
    else
        return m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GM) >= obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM);

    // Ghost players, Spirit Healers, and some other NPCs
    if (!corpseVisibility && !(obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GHOST)))
    {
        // Alive players can see dead players in some cases, but other objects can't do that
        if (Player const* thisPlayer = ToPlayer())
        {
            if (Player const* objPlayer = obj->ToPlayer())
            {
                if (thisPlayer->GetTeam() != objPlayer->GetTeam() || !thisPlayer->IsGroupVisibleFor(objPlayer))
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }

    if (obj->IsInvisibleDueToDespawn())
        return false;

    if (!CanDetect(obj, ignoreStealth))
        return false;

    if (obj->GetTypeId() == TYPEID_DYNAMICOBJECT) // Update raid marker visibility.
        if (((DynamicObject*)obj)->GetType() == DYNAMIC_OBJECT_RAID_MARKER)
            if (Player const* thisPlayer = ToPlayer())
                if (!thisPlayer->GetGroup() || ((DynamicObject*)obj)->GetCasterGUID() != thisPlayer->GetGroup()->GetGUID())
                    return false;

    return true;
}

bool WorldObject::CanNeverSee(WorldObject const* obj) const
{
    // Archaeology Finds can be seen only by owner
    if (obj->GetTypeId() == TYPEID_GAMEOBJECT && (obj->GetEntry() == 203071 || obj->GetEntry() == 203078 || obj->GetEntry() == 204282 || obj->GetEntry() == 202655 ||
        obj->GetEntry() == 206836 || obj->GetEntry() == 207187 || obj->GetEntry() == 207188 || obj->GetEntry() == 207189 || obj->GetEntry() == 207190 ||
        obj->GetEntry() == 211163 || obj->GetEntry() == 211174 || obj->GetEntry() == 218950))
    {
        if (uint64 ownerGUID = obj->GetUInt64Value(GAMEOBJECT_FIELD_CREATED_BY))
            if (GetGUID() != ownerGUID)
                return true;
    }

    Player const* player = ToPlayer();
    if (obj->GetTypeId() == TYPEID_GAMEOBJECT && player && !player->IsGameMaster())
    {
        // remove hack , fix in future
        // auto itr = timelessRareChestsMap.find(obj->GetEntry());
        // if (itr != timelessRareChestsMap.end())
        //     if (player->GetQuestStatus(itr->second) == QUEST_STATUS_REWARDED || player->IsWeeklyQuestDone(itr->second) || player->IsDailyQuestDone(itr->second))
        //         return true;
    }

    if (GetMap() != obj->GetMap() || !InSamePhase(obj))
        return true;

    if (obj->m_explicitSeerGuid == GetGUID())
        return false;

    // if this and obj do not have common seer, they should not see each other
    return m_explicitSeerGuid != obj->m_explicitSeerGuid;
}

bool WorldObject::CanDetect(WorldObject const* obj, bool ignoreStealth) const
{
    const WorldObject* seer = this;

    // Pets don't have detection, they use the detection of their masters
    if (Unit const* thisUnit = ToUnit())
        if (Unit* controller = thisUnit->GetCharmerOrOwner())
            seer = controller;

    if (obj->IsAlwaysDetectableFor(seer))
        return true;

    if (!ignoreStealth && !seer->CanDetectInvisibilityOf(obj))
        return false;

    if (!ignoreStealth && !seer->CanDetectStealthOf(obj))
        return false;

    return true;
}

bool WorldObject::CanDetectInvisibilityOf(WorldObject const* obj) const
{
    uint32 mask = obj->m_invisibility.GetFlags() & m_invisibilityDetect.GetFlags();

    // Check for not detected types
    if (mask != obj->m_invisibility.GetFlags())
        return false;

    for (uint32 i = 0; i < TOTAL_INVISIBILITY_TYPES; ++i)
    {
        if (!(mask & (1 << i)))
            continue;

        int32 objInvisibilityValue = obj->m_invisibility.GetValue(InvisibilityType(i));
        int32 ownInvisibilityDetectValue = m_invisibilityDetect.GetValue(InvisibilityType(i));

        // Too low value to detect
        if (ownInvisibilityDetectValue < objInvisibilityValue)
            return false;
    }

    return true;
}

bool WorldObject::CanDetectStealthOf(WorldObject const* obj) const
{
    // Combat reach is the minimal distance (both in front and behind),
    //   and it is also used in the range calculation.
    // One stealth point increases the visibility range by 0.3 yard.

    if (!obj->m_stealth.GetFlags())
        return true;

    float distance = GetExactDist(obj);
    float combatReach = 0.0f;

    Unit const* unit = ToUnit();
    if (unit)
        combatReach = unit->GetCombatReach();

    if (distance < combatReach)
        return true;

    // Hack for Subterfuge. When rogue opens and Subterfuge triggered - player is still invisible and mob just goes to evade.
    if (Player const* player = obj->ToPlayer())
        if (GetTypeId() == TYPEID_UNIT && player->IsInCombat() && player->HasAura(115192))
            if (const_cast<Unit*>(unit)->GetThreatManager().getThreatList().size() == 1)
                if (const_cast<Unit*>(unit)->GetThreatManager().getThreat(const_cast<Player*>(player)) > 0.0f)
                    return true;

    if (!HasInArc(M_PI, obj))
        return false;

    GameObject const* go = ToGameObject();
    for (uint32 i = 0; i < TOTAL_STEALTH_TYPES; ++i)
    {
        if (!(obj->m_stealth.GetFlags() & (1 << i)))
            continue;

        if (unit && unit->HasAuraTypeWithMiscvalue(SPELL_AURA_DETECT_STEALTH, i))
            return true;

        // Starting points
        int32 detectionValue = 30;

        // Level difference: 5 point / level, starting from level 1.
        // There may be spells for this and the starting points too, but
        // not in the DBCs of the client.
        detectionValue += int32(GetLevelForTarget(obj) - 1) * 5;

        // Apply modifiers
        detectionValue += m_stealthDetect.GetValue(StealthType(i));
        if (go)
            if (Unit* owner = go->GetOwner())
                detectionValue -= int32(owner->GetLevelForTarget(this) - 1) * 5;

        detectionValue -= obj->m_stealth.GetValue(StealthType(i));

        // Calculate max distance
        float visibilityRange = float(detectionValue) * 0.3f + combatReach;

        if (visibilityRange > MAX_PLAYER_STEALTH_DETECT_RANGE)
            visibilityRange = MAX_PLAYER_STEALTH_DETECT_RANGE;

        if (distance > visibilityRange)
            return false;
    }

    return true;
}

void Object::ForceValuesUpdateAtIndex(uint32 i)
{
    _changesMask.SetBit(i);
    if (m_inWorld && !m_objectUpdated)
        AddToUpdate();
}

void Object::ForceDynamicValuesUpdateTabAtIndex(uint32 tab, uint16 index)
{
    m_dynamicChange[tab] = true;
    if (m_inWorld && !m_objectUpdated)
        AddToUpdate();
}

namespace Trinity
{
    class MonsterChatBuilder
    {
        public:
            MonsterChatBuilder(WorldObject const* obj, ChatMsg msgtype, int32 textId, uint32 language, WorldObject const* target)
                : i_object(obj), i_msgtype(msgtype), i_textId(textId), i_language(Language(language)), i_target(target) { }

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetTrinityString(i_textId, loc_idx);

                /// @todo i_object.GetName() also must be localized?
                ChatHandler::BuildChatPacket(data, i_msgtype, i_language, i_object, i_target, text, 0, "", loc_idx);
            }

        private:
            WorldObject const* i_object;
            ChatMsg i_msgtype;
            int32 i_textId;
            Language i_language;
            WorldObject const* i_target;
    };

    class MonsterCustomChatBuilder
    {
        public:
            MonsterCustomChatBuilder(WorldObject const* obj, ChatMsg msgtype, const char* text, uint32 language, WorldObject const* target)
                : i_object(obj), i_msgtype(msgtype), i_text(text), i_language(Language(language)), i_target(target) { }

            void operator()(WorldPacket& data, LocaleConstant loc_idx)
            {
                /// @todo i_object.GetName() also must be localized?
                ChatHandler::BuildChatPacket(data, i_msgtype, i_language, i_object, i_target, i_text, 0, "", loc_idx);
            }

        private:
            WorldObject const* i_object;
            ChatMsg i_msgtype;
            const char* i_text;
            Language i_language;
            WorldObject const* i_target;
    };
}                                                           // namespace Trinity

void WorldObject::SendMessageToSet(WorldPacket const* data, bool self) const
{
    if (IsInWorld())
        SendMessageToSetInRange(data, GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, self);
}

void WorldObject::SendMessageToSetInRange(WorldPacket const* data, float dist, bool /*self*/) const
{
    Trinity::MessageDistDeliverer notifier(this, data, dist);
    VisitNearbyWorldObject(dist, notifier, false, true);
}

void WorldObject::SendMessageToSet(WorldPacket const* data, Player const* skipped_rcvr) const
{
    Trinity::MessageDistDeliverer notifier(this, data, GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, false, skipped_rcvr);
    VisitNearbyWorldObject(GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, notifier, false, true);
}

void WorldObject::SendObjectDeSpawnAnim(ObjectGuid guid)
{
    WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);

    data.WriteBit(guid[0]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[5]);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);

    SendMessageToSet(&data, true);
}

void WorldObject::SetMap(Map* map)
{
    ASSERT(map);
    ASSERT(!IsInWorld() || GetTypeId() == TYPEID_CORPSE);
    if (m_currMap == map) // command add npc: first create, than loadfromdb
        return;
    if (m_currMap)
    {
        TC_LOG_FATAL("misc", "WorldObject::SetMap: obj %u new map %u %u, old map %u %u", (uint32)GetTypeId(), map->GetId(), map->GetInstanceId(), m_currMap->GetId(), m_currMap->GetInstanceId());
        ASSERT(false);
    }
    m_currMap = map;
    m_mapId = map->GetId();
    m_InstanceId = map->GetInstanceId();
    if (IsWorldObject())
        m_currMap->AddWorldObject(this);
}

void WorldObject::ResetMap()
{
    ASSERT(m_currMap);
    ASSERT(!IsInWorld());
    if (IsWorldObject())
        m_currMap->RemoveWorldObject(this);
    m_currMap = NULL;
    //maybe not for corpse
    //m_mapId = 0;
    //m_InstanceId = 0;
}

Map const* WorldObject::GetBaseMap() const
{
    ASSERT(m_currMap);
    return m_currMap->GetParent();
}

void WorldObject::AddObjectToRemoveList()
{
    ASSERT(m_uint32Values);

    Map* map = FindMap();
    if (!map)
    {
        TC_LOG_ERROR("misc", "Object (TypeId: %u Entry: %u GUID: %u) at attempt add to move list not have valid map (Id: %u).", GetTypeId(), GetEntry(), GetGUID().GetCounter(), GetMapId());
        return;
    }

    map->AddObjectToRemoveList(this);
}

TempSummon* Map::SummonCreature(uint32 entry, Position const& pos, SummonPropertiesEntry const* properties /*= NULL*/, uint32 duration /*= 0*/, Unit* summoner /*= NULL*/, uint32 spellId /*= 0*/, uint32 vehId /*= 0*/, ObjectGuid privateObjectOwner /*= ObjectGuid::Empty*/)
{
    if (!Trinity::IsValidMapCoord(pos.GetPositionX(), pos.GetPositionY()))
    {
        TC_LOG_ERROR("shitlog", "Map::SummonCreature entry: %u, spell: %u, x: %f, y: %f\n", entry, spellId, pos.GetPositionX(), pos.GetPositionY());
        return nullptr;
    }

    uint32 mask = UNIT_MASK_SUMMON;
    if (properties)
    {
        switch (properties->Category)
        {
            case SUMMON_CATEGORY_PET:
                mask = UNIT_MASK_GUARDIAN;
                break;
            case SUMMON_CATEGORY_PUPPET:
                mask = UNIT_MASK_PUPPET;
                break;
            case SUMMON_CATEGORY_VEHICLE:
                mask = UNIT_MASK_MINION;
                break;
            case SUMMON_CATEGORY_WILD:
            case SUMMON_CATEGORY_ALLY:
            case SUMMON_CATEGORY_UNK:
            {
                switch (properties->Type)
                {
                    case SUMMON_TYPE_PET:
                    case SUMMON_TYPE_MINION:
                    case SUMMON_TYPE_GUARDIAN:
                    case SUMMON_TYPE_GUARDIAN2:
                        mask = UNIT_MASK_GUARDIAN;
                        break;
                    case SUMMON_TYPE_TOTEM:
                    case SUMMON_TYPE_LIGHTWELL:
                    case SUMMON_TYPE_STATUE:
                        mask = UNIT_MASK_TOTEM;
                        break;
                    case SUMMON_TYPE_VEHICLE:
                    case SUMMON_TYPE_VEHICLE2:
                        mask = UNIT_MASK_SUMMON;
                        break;
                    case SUMMON_TYPE_MINIPET:
                        mask = UNIT_MASK_MINION;
                        break;
                    default:
                        if (entry == 60199 || entry == 47649 || entry == 59172) // Rune of Power, Wild Mushrooms (druid), Wild Mushroom: Plague (dk)
                            mask = UNIT_MASK_TOTEM;
                        else if (properties->Flags & 512 || properties->Flags & 2 ||  // Mirror Image, Summon Gargoyle, Void Tendrils
                            entry == 64347 ||  // hack for q31314
                            spellId == 142917) // Xan'tish's Flute
                            mask = UNIT_MASK_GUARDIAN;
                        break;
                }
                break;
            }
            default:
                return nullptr;
        }
    }

    uint32 phase = PHASEMASK_NORMAL;
    std::set<uint32> phases;
    uint32 team = 0;
    if (summoner)
    {
        phase = summoner->GetPhaseMask();
        if (summoner->GetTypeId() == TYPEID_PLAYER)
            team = summoner->ToPlayer()->GetTeam();

        phases = summoner->GetPhases();
    }

    TempSummon* summon = nullptr;
    switch (mask)
    {
        case UNIT_MASK_SUMMON:
            summon = new TempSummon(properties, summoner, false);
            break;
        case UNIT_MASK_GUARDIAN:
            summon = new Guardian(properties, summoner, false);
            break;
        case UNIT_MASK_PUPPET:
            summon = new Puppet(properties, summoner);
            break;
        case UNIT_MASK_TOTEM:
            summon = new Totem(properties, summoner);
            break;
        case UNIT_MASK_MINION:
            summon = new Minion(properties, summoner, false);
            break;
    }

    if (!summon->HasUnitTypeMask(UNIT_MASK_CONTROLABLE_GUARDIAN))
        summon->HideSummonedBy();

    if (!summon->Create(GenerateLowGuid<HighGuid::Unit>(), this, phase, entry, vehId, team, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
    {
        delete summon;
        return nullptr;
    }

    for (auto phaseId : phases)
        summon->SetPhased(phaseId, false, true);

    summon->SetUInt32Value(UNIT_FIELD_CREATED_BY_SPELL, spellId);

    summon->SetHomePosition(pos);
    summon->AddToTransportIfNeeded(summoner ? summoner->GetTransport() : nullptr, true);

    summon->InitStats(duration);

    summon->SetPrivateObjectOwner(privateObjectOwner);

    AddToMap(summon->ToCreature());
    summon->UpdateCastingSpeed();
    summon->InitSummon();
    summon->UpdateHasteRegen();

    if (summoner)
    {
        if (summon->IsInMap(summoner))
            summoner->AddSummon(summon);
        else if (summoner->FindMap() != summon->FindMap()) // Okay, owner isn't in world, just some shit in scripts on instance unload/etc or summon was despawned (yeah, its's possible)
        {
            TC_LOG_ERROR("shitlog", "Map::SummonCreature spellId: %u, owner (" UI64FMTD ", entry: %u) map: %u (instance: %u), summon (" UI64FMTD ", entry: %u, in world: %u) map: %u (instance: %u)\n", spellId, summoner->GetGUID().GetRawValue(), summoner->GetEntry(), summoner->GetMap()->GetId(), summoner->GetInstanceId(), summon->GetGUID().GetRawValue(), summon->GetEntry(), summon->IsInWorld(), summon->GetMap()->GetId(), summon->GetInstanceId());
        }
    }

    //ObjectAccessor::UpdateObjectVisibility(summon);

    return summon;
}

/**
* Summons group of creatures.
*
* @param group Id of group to summon.
* @param list  List to store pointers to summoned creatures.
*/

void Map::SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list /*= NULL*/)
{
    std::vector<TempSummonData> const* data = sObjectMgr->GetSummonGroup(GetId(), SUMMONER_TYPE_MAP, group);
    if (!data)
        return;

    for (std::vector<TempSummonData>::const_iterator itr = data->begin(); itr != data->end(); ++itr)
        if (TempSummon* summon = SummonCreature(itr->entry, itr->pos, NULL, itr->time))
            if (list)
                list->push_back(summon);
}

void WorldObject::SetZoneScript()
{
    if (Map* map = FindMap())
    {
        // if (map->IsDungeon() || map->IsScenario())
        //     m_zoneScript = (ZoneScript*)((InstanceMap*)map)->GetInstanceScript();
        if (InstanceMap* instanceMap = map->ToInstanceMap())
            m_zoneScript = reinterpret_cast<ZoneScript*>(instanceMap->GetInstanceScript());        
        else if (!map->IsBattlegroundOrArena())
        {
            if (Battlefield* bf = sBattlefieldMgr->GetBattlefieldToZoneId(GetZoneId()))
                m_zoneScript = bf;
            else
                m_zoneScript = sOutdoorPvPMgr->GetZoneScript(GetZoneId());
        }
    }
}

TempSummon* WorldObject::SummonCreature(uint32 entry, const Position &pos, TempSummonType spwtype, uint32 duration, uint32 vehId, ObjectGuid privateObjectOwner /*= ObjectGuid::Empty*/)
{
    if (Map* map = FindMap())
    {
        if (TempSummon* summon = map->SummonCreature(entry, pos, nullptr, duration, ToUnit(), 0, vehId, privateObjectOwner))
        {
            summon->SetTempSummonType(spwtype);
            return summon;
        }
    }

    return NULL;
}

TempSummon* WorldObject::SummonCreature(uint32 id, float x, float y, float z, float ang /*= 0*/, TempSummonType spwtype /*= TEMPSUMMON_MANUAL_DESPAWN*/, uint32 despwtime /*= 0*/, ObjectGuid privateObjectOwner /*= ObjectGuid::Empty*/)
{
    if (!x && !y && !z)
    {
        GetClosePoint(x, y, z, GetObjectSize());
        ang = GetOrientation();
    }
    Position pos;
    pos.Relocate(x, y, z, ang);
    return SummonCreature(id, pos, spwtype, despwtime, 0, privateObjectOwner);
}

GameObject* WorldObject::SummonGameObject(uint32 entry, float x, float y, float z, float ang, G3D::Quat const& rotation, uint32 respawnTime, GOSummonType summonType)
{
    if (!IsInWorld())
        return NULL;

    GameObjectTemplate const* goinfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!goinfo)
    {
        TC_LOG_ERROR("sql.sql", "Gameobject template %u not found in database!", entry);
        return NULL;
    }

    Map* map = GetMap();
    GameObject* go = new GameObject();
    if (!go->Create(map->GenerateLowGuid<HighGuid::GameObject>(), entry, map, GetPhaseMask(), x, y, z, ang, rotation, 100, GO_STATE_READY))
    {
        delete go;
        return NULL;
    }

    go->AddToTransportIfNeeded(GetTransport());

    for (auto phase : GetPhases())
        go->SetPhased(phase, false, true);

    go->SetRespawnTime(respawnTime);
    if (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT && summonType == GO_SUMMON_TIMED_OR_CORPSE_DESPAWN) //not sure how to handle this
        ToUnit()->AddGameObject(go);
    else
        go->SetSpawnedByDefault(false);

    map->AddToMap(go);
    return go;
}

Creature* WorldObject::SummonTrigger(float x, float y, float z, float ang, uint32 duration, CreatureAI* (*GetAI)(Creature*))
{
    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
    Creature* summon = SummonCreature(WORLD_TRIGGER, x, y, z, ang, summonType, duration);
    if (!summon)
        return NULL;

    //summon->SetName(GetName());
    if (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT)
    {
        summon->SetFaction(((Unit*)this)->GetFaction());
        summon->SetLevel(((Unit*)this)->GetLevel());
    }

    if (GetAI)
        summon->AIM_Initialize(GetAI(summon));
    return summon;
}

/**
* Summons group of creatures. Should be called only by instances of Creature and GameObject classes.
*
* @param group Id of group to summon.
* @param list  List to store pointers to summoned creatures.
*/
void WorldObject::SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list /*= NULL*/)
{
    ASSERT((GetTypeId() == TYPEID_GAMEOBJECT || GetTypeId() == TYPEID_UNIT) && "Only GOs and creatures can summon npc groups!");

    std::vector<TempSummonData> const* data = sObjectMgr->GetSummonGroup(GetEntry(), GetTypeId() == TYPEID_GAMEOBJECT ? SUMMONER_TYPE_GAMEOBJECT : SUMMONER_TYPE_CREATURE, group);
    if (!data)
        return;

    for (std::vector<TempSummonData>::const_iterator itr = data->begin(); itr != data->end(); ++itr)
        if (TempSummon* summon = SummonCreature(itr->entry, itr->pos, itr->type, itr->time))
            if (list)
                list->push_back(summon);
}

Creature* WorldObject::FindNearestCreature(uint32 entry, float range, bool alive) const
{
    Creature* creature = nullptr;
    Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*this, entry, alive, range);
    Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(this, creature, checker);
    VisitNearbyObject(range, searcher);
    return creature;
}

std::list<Creature*> WorldObject::FindNearestCreatures(uint32 entry, float range) const
{
    std::list<Creature*> creatureList;    
    GetCreatureListWithEntryInGrid(creatureList, entry, range);   
    return creatureList;
}

std::list<Creature*> WorldObject::FindNearestCreatures(std::list<uint32> entrys, float range) const
{
    std::list<Creature*> creatureList;

    for (std::list<uint32>::iterator itr = entrys.begin(); itr != entrys.end(); ++itr)
        GetCreatureListWithEntryInGrid(creatureList, (*itr), range);
    return creatureList;
}

std::vector<Creature*> WorldObject::FindNearestCreatures(uint32 entry, float range, bool alive) const
{
    std::list<Creature*> creatureList;
    std::vector<Creature*> returnList;
    GetCreatureListWithEntryInGrid(creatureList, entry, range);
    
    for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
    {
        if ((*itr)->IsAlive() == alive)
            returnList.push_back(*itr);
    }
    return returnList;
}

Player* WorldObject::FindNearestPlayer(float range) const
{
    Player* player = NULL;
    Trinity::NearestPlayerInObjectRangeCheck checker(this, range);
    Trinity::PlayerLastSearcher<Trinity::NearestPlayerInObjectRangeCheck> searcher(this, player, checker);
    VisitNearbyObject(range, searcher);
    return player;
}

std::list<Player*> WorldObject::GetNearestPlayersList(float range, bool alive)
{
    std::list<Player*> players;
    Trinity::AnyPlayerInObjectRangeCheck checker(this, range, alive);
    Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(this, players, checker);
    VisitNearbyWorldObject(range, searcher);
    return players;
}

GameObject* WorldObject::FindNearestGameObject(uint32 entry, float range) const
{
    GameObject* go = NULL;
    Trinity::NearestGameObjectEntryInObjectRangeCheck checker(*this, entry, range);
    Trinity::GameObjectLastSearcher<Trinity::NearestGameObjectEntryInObjectRangeCheck> searcher(this, go, checker);
    VisitNearbyGridObject(range, searcher);
    return go;
}

GameObject* WorldObject::FindNearestGameObjectOfType(GameobjectTypes type, float range) const
{
    GameObject* go = NULL;
    Trinity::NearestGameObjectTypeInObjectRangeCheck checker(*this, type, range);
    Trinity::GameObjectLastSearcher<Trinity::NearestGameObjectTypeInObjectRangeCheck> searcher(this, go, checker);
    VisitNearbyGridObject(range, searcher);
    return go;
}

void WorldObject::GetGameObjectListWithEntryInGrid(std::list<GameObject*>& gameobjectList, uint32 entry, float maxSearchRange) const
{
    CellCoord pair(Trinity::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::AllGameObjectsWithEntryInRange check(this, entry, maxSearchRange);
    Trinity::GameObjectListSearcher<Trinity::AllGameObjectsWithEntryInRange> searcher(this, gameobjectList, check);
    TypeContainerVisitor<Trinity::GameObjectListSearcher<Trinity::AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(this->GetMap()), *this, maxSearchRange);
}

void WorldObject::GetCreatureListWithEntryInGrid(std::list<Creature*>& creatureList, uint32 entry, float maxSearchRange) const
{
    CellCoord pair(Trinity::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::AllCreaturesOfEntryInRange check(this, entry, maxSearchRange);
    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(this, creatureList, check);
    TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(this->GetMap()), *this, maxSearchRange);
}

void WorldObject::GetPlayerListInGrid(std::list<Player*>& playerList, float maxSearchRange) const
{
    Trinity::AnyPlayerInObjectRangeCheck checker(this, maxSearchRange);
    Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(this, playerList, checker);
    this->VisitNearbyWorldObject(maxSearchRange, searcher);
}

void WorldObject::GetCreatureListWithEntryInGridAppend(std::list<Creature*>& creatureList, uint32 entry, float maxSearchRange) const
{
    std::list<Creature*> tempList;
    GetCreatureListWithEntryInGrid(tempList, entry, maxSearchRange);
    creatureList.sort();
    tempList.sort();
    creatureList.merge(tempList);
}

void WorldObject::GetCreaturesWithEntryInRange(std::list<Creature*>& creatureList, float radius, uint32 entry)
{
    CellCoord pair(Trinity::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Trinity::AllCreaturesOfEntryInRange check(this, entry, radius);
    Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(this, creatureList, check);

    TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange>, WorldTypeMapContainer> world_visitor(searcher);
    cell.Visit(pair, world_visitor, *(this->GetMap()), *this, radius);

    TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange>, GridTypeMapContainer> grid_visitor(searcher);
    cell.Visit(pair, grid_visitor, *(this->GetMap()), *this, radius);
}

/*
namespace Trinity
{
    class NearUsedPosDo
    {
        public:
            NearUsedPosDo(WorldObject const& obj, WorldObject const* searcher, float angle, ObjectPosSelector& selector)
                : i_object(obj), i_searcher(searcher), i_angle(angle), i_selector(selector) { }

            void operator()(Corpse*) const { }
            void operator()(DynamicObject*) const { }

            void operator()(Creature* c) const
            {
                // skip self or target
                if (c == i_searcher || c == &i_object)
                    return;

                float x, y, z;

                if (!c->IsAlive() || c->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED) ||
                    !c->GetMotionMaster()->GetDestination(x, y, z))
                {
                    x = c->GetPositionX();
                    y = c->GetPositionY();
                }

                add(c, x, y);
            }

            template<class T>
                void operator()(T* u) const
            {
                // skip self or target
                if (u == i_searcher || u == &i_object)
                    return;

                float x, y;

                x = u->GetPositionX();
                y = u->GetPositionY();

                add(u, x, y);
            }

            // we must add used pos that can fill places around center
            void add(WorldObject* u, float x, float y) const
            {
                // u is too nearest/far away to i_object
                if (!i_object.IsInRange2d(x, y, i_selector.m_dist - i_selector.m_size, i_selector.m_dist + i_selector.m_size))
                    return;

                float angle = i_object.GetAngle(u)-i_angle;

                // move angle to range -pi ... +pi
                while (angle > M_PI)
                    angle -= 2.0f * M_PI;
                while (angle < -M_PI)
                    angle += 2.0f * M_PI;

                // dist include size of u
                float dist2d = i_object.GetDistance2d(x, y);
                i_selector.AddUsedPos(u->GetObjectSize(), angle, dist2d + i_object.GetObjectSize());
            }
        private:
            WorldObject const& i_object;
            WorldObject const* i_searcher;
            float              i_angle;
            ObjectPosSelector& i_selector;
    };
}                                                           // namespace Trinity
*/

//===================================================================================================

// @todo: replace with WorldObject::UpdateAllowedPositionZ
static float NormalizeZforCollision(WorldObject* obj, float x, float y, float z)
{
    float ground = obj->GetMap()->GetHeight(obj->GetPhaseMask(), x, y, MAX_HEIGHT, true, MAX_HEIGHT * 2);
    float floor = obj->GetMap()->GetHeight(obj->GetPhaseMask(), x, y, z, true, MAX_HEIGHT * 2);
    float helper = fabs(ground - z) <= fabs(floor - z) ? ground : floor;
    if (z > helper) // must be above ground
    {
        if (Unit* unit = obj->ToUnit())
        {
            if (unit->CanFly())
                return z;
        }
        LiquidData liquid_status;
        ZLiquidStatus res = obj->GetMap()->GetLiquidStatus(obj->GetPhaseMask(), x, y, z, MAP_ALL_LIQUIDS, &liquid_status);
        if (res && liquid_status.level > helper) // water must be above ground
        {
            if (liquid_status.level > z) // z is underwater
                return z;
            else
                return fabs(liquid_status.level - z) <= fabs(helper - z) ? liquid_status.level : helper;
        }
    }
    return helper;
}

void WorldObject::GetNearPoint2D(float &x, float &y, float distance2d, float absAngle) const
{
    x = GetPositionX() + (GetObjectSize() + distance2d) * std::cos(absAngle);
    y = GetPositionY() + (GetObjectSize() + distance2d) * std::sin(absAngle);

    Trinity::NormalizeMapCoord(x);
    Trinity::NormalizeMapCoord(y);
}

void WorldObject::GetNearPoint(WorldObject const* /*searcher*/, float &x, float &y, float &z, float searcher_size, float distance2d, float absAngle) const
{
    Position pos = GetPosition();
    const_cast<WorldObject*>(this)->MovePositionToFirstCollision(pos, distance2d + searcher_size, absAngle - GetOrientation());
    pos.GetPosition(x, y);
    //GetNearPoint2D(x,y,distance2d+searcher_size,absAngle);
    z = GetPositionZ();

    UpdateAllowedPositionZ(x, y, z);
}

void WorldObject::GetClosePoint(float &x, float &y, float &z, float size, float distance2d /*= 0*/, float angle /*= 0*/) const
{
    // angle calculated from current orientation
    GetNearPoint(NULL, x, y, z, size, distance2d, GetOrientation() + angle);
}

Position WorldObject::GetNearPosition(float dist, float angle)
{
    Position pos = GetPosition();
    MovePosition(pos, dist, angle);
    return pos;
}

Position WorldObject::GetFirstCollisionPosition(float dist, float angle)
{
    Position pos = GetPosition();
    MovePositionToFirstCollision(pos, dist, angle);
    return pos;
}

Position WorldObject::GetRandomNearPosition(float radius)
{
    Position pos = GetPosition();
    MovePosition(pos, radius * (float)rand_norm(), (float)rand_norm() * static_cast<float>(2 * M_PI));
    return pos;
}

void WorldObject::GetContactPoint(const WorldObject* obj, float &x, float &y, float &z, float distance2d /*= CONTACT_DISTANCE*/) const
{
    // angle to face `obj` to `this` using distance includes size of `obj`
    GetNearPoint(obj, x, y, z, obj->GetObjectSize(), distance2d, GetAngle(obj));
}

void WorldObject::MovePositionToFirstCollosionBySteps(Position& pos, float dist, float angle, float heightCheckInterval, bool allowInAir)
{
    static float const InitialGroundSnapThreshold = 2.0f;
    static float const HeightCheckThresholdMultiplier = 2.0f; // Making this larger will allow for steeper slopes
    static float const CollisionCheckOffsetZ = 1.0f;

    Map* map = GetMap();
    float destx, desty, destz;

    angle += GetOrientation();
    pos.GetPosition(destx, desty, destz);
    Position lastGroundPos = pos;

    // Snap to ground...
    destz = NormalizeZforCollision(this, destx, desty, destz);
    // ...unless underwater
    bool swimming = m_movementInfo.flags & MOVEMENTFLAG_SWIMMING && GetMap()->IsInWater(GetPhaseMask(), destx, desty, lastGroundPos.m_positionZ);
    if (allowInAir)
        swimming = true;
    if (swimming && destz < lastGroundPos.m_positionZ)
        destz = lastGroundPos.m_positionZ;

    // Adopt ground position as blink origin, unless there was no ground (example: The Frozen Throne in Icecrown Citadel)
    if (destz != VMAP_INVALID_HEIGHT_VALUE)
        pos.m_positionZ = destz;

    float distanceMoved = 0;
    bool skippingAir = swimming;
    float distanceDifference;
    while ((distanceDifference = std::min(heightCheckInterval, dist - distanceMoved)) > 0.05f)
    {
        destx += distanceDifference * cos(angle);
        desty += distanceDifference * sin(angle);
        destz = NormalizeZforCollision(this, destx, desty, skippingAir ? lastGroundPos.m_positionZ : destz);

        float heightDifference = destz - (skippingAir ? lastGroundPos.m_positionZ : pos.m_positionZ);
        // We encountered a wall and we stop here
        if (heightDifference > distanceDifference * HeightCheckThresholdMultiplier)
            break;
        // Maintain at least the initial height when underwater
        swimming = (m_movementInfo.flags & MOVEMENTFLAG_SWIMMING && GetMap()->IsInWater(GetPhaseMask(), destx, desty, lastGroundPos.m_positionZ));
        if (!swimming && allowInAir)
            swimming = true;
        if (swimming)
        {
            if (destz < lastGroundPos.m_positionZ)
                destz = lastGroundPos.m_positionZ;
        }
        // We're near a drop - mark current location and try to find a ground of the other side
        else if (heightDifference < -distanceDifference * HeightCheckThresholdMultiplier)
        {
            if (!skippingAir)
            {
                skippingAir = true;
                lastGroundPos.Relocate(pos);
                destz = pos.m_positionZ;
            }
        }
        // We found a ground after the drop - temporarily restore pos to last ground position to check LOS properly
        else if (skippingAir)
        {
            skippingAir = false;
            pos.Relocate(lastGroundPos);
        }

        float outz;
        if (VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(GetMapId(),
            pos.m_positionX, pos.m_positionY, pos.m_positionZ + CollisionCheckOffsetZ,
            destx, desty, destz + CollisionCheckOffsetZ,
            destx, desty, outz, -0.5f)
            || map->getObjectHitPos(GetPhaseMask(),
                pos.m_positionX, pos.m_positionY, pos.m_positionZ + CollisionCheckOffsetZ,
                destx, desty, destz + CollisionCheckOffsetZ,
                destx, desty, outz, -0.5f)
            )
        {
            // move forward a bit to preserve relativity
            destx += 0.01f * cos(angle);
            desty += 0.01f * sin(angle);
            destz = outz;
            //distanceMoved = dist; // To make sure we end this loop on the next condition check (Apparently it works better without this line...)
        }

        distanceMoved += distanceDifference;

        Trinity::NormalizeMapCoord(destx);
        Trinity::NormalizeMapCoord(desty);
        if (!skippingAir)
            destz = NormalizeZforCollision(this, destx, desty, destz);

        pos.Relocate(destx, desty, destz, GetOrientation());
    }

    // If we've encountered a drop before - restore last grounded position
    if (skippingAir && !swimming)
    {
        pos.Relocate(lastGroundPos);
        Trinity::NormalizeMapCoord(pos.m_positionX);
        Trinity::NormalizeMapCoord(pos.m_positionY);
        UpdateGroundPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
        pos.SetOrientation(GetOrientation());
    }
}

float WorldObject::GetObjectSize() const
{
    return (m_valuesCount > UNIT_FIELD_COMBAT_REACH) ? m_floatValues[UNIT_FIELD_COMBAT_REACH] : DEFAULT_PLAYER_BOUNDING_RADIUS;
}

void WorldObject::MovePosition(Position &pos, float dist, float angle)
{
    angle += GetOrientation();
    float destx, desty, destz, ground, floor;
    destx = pos.m_positionX + dist * std::cos(angle);
    desty = pos.m_positionY + dist * std::sin(angle);

    // Prevent invalid coordinates here, position is unchanged
    if (!Trinity::IsValidMapCoord(destx, desty, pos.m_positionZ))
    {
        TC_LOG_FATAL("misc", "WorldObject::MovePosition: Object (TypeId: %u Entry: %u GUID: %u) has invalid coordinates X: %f and Y: %f were passed!",
            GetTypeId(), GetEntry(), GetGUID().GetCounter(), destx, desty);
        return;
    }

    ground = GetMapHeight(destx, desty, MAX_HEIGHT);
    floor = GetMapHeight(destx, desty, pos.m_positionZ);
    destz = std::fabs(ground - pos.m_positionZ) <= std::fabs(floor - pos.m_positionZ) ? ground : floor;

    float step = dist/10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (std::fabs(pos.m_positionZ - destz) > 6)
        {
            destx -= step * std::cos(angle);
            desty -= step * std::sin(angle);
            ground = GetMapHeight(destx, desty, MAX_HEIGHT);
            floor = GetMapHeight(destx, desty, pos.m_positionZ);
            destz = std::fabs(ground - pos.m_positionZ) <= std::fabs(floor - pos.m_positionZ) ? ground : floor;
        }
        // we have correct destz now
        else
        {
            pos.Relocate(destx, desty, destz);
            break;
        }
    }

    Trinity::NormalizeMapCoord(pos.m_positionX);
    Trinity::NormalizeMapCoord(pos.m_positionY);
    UpdateGroundPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    pos.SetOrientation(GetOrientation());
}

void WorldObject::MovePositionToFirstCollision(Position &pos, float dist, float angle, float offsetZ)
{
    angle += GetOrientation();
    float destx, desty, destz;
    destx = pos.m_positionX + dist * std::cos(angle);
    desty = pos.m_positionY + dist * std::sin(angle);
    destz = pos.m_positionZ;

    // Prevent invalid coordinates here, position is unchanged
    if (!Trinity::IsValidMapCoord(destx, desty))
    {
        TC_LOG_FATAL("misc", "WorldObject::MovePositionToFirstCollision invalid coordinates X: %f and Y: %f were passed!", destx, desty);
        return;
    }

    // Use a detour raycast to get our first collision point
    PathGenerator path(this);
    path.SetUseRaycast(true);
    path.CalculatePath(destx, desty, destz, false);

    // Check for valid path types before we proceed
    if (!(path.GetPathType() & PATHFIND_NOT_USING_PATH))
        if (path.GetPathType() & ~(PATHFIND_NORMAL | PATHFIND_SHORTCUT | PATHFIND_INCOMPLETE | PATHFIND_FARFROMPOLY_END))
            return;

    G3D::Vector3 result = path.GetPath().back();
    destx = result.x;
    desty = result.y;
    destz = result.z;

    // check static LOS
    float halfHeight = GetCollisionHeight() * 0.5f;
    bool col = false;

    // Unit is flying, check for potential collision via vmaps
    if (path.GetPathType() & PATHFIND_NOT_USING_PATH)
    {
        col = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(GetMapId(),
            pos.m_positionX, pos.m_positionY, pos.m_positionZ + halfHeight,
            destx, desty, destz + halfHeight,
            destx, desty, destz, -0.5f);

        destz -= halfHeight;

        // Collided with static LOS object, move back to collision point
        if (col)
        {
            destx -= CONTACT_DISTANCE * std::cos(angle);
            desty -= CONTACT_DISTANCE * std::sin(angle);
            dist = std::sqrt((pos.m_positionX - destx) * (pos.m_positionX - destx) + (pos.m_positionY - desty) * (pos.m_positionY - desty));
        }
    }

    // check dynamic collision
    col = GetMap()->getObjectHitPos(GetPhaseMask(),
        pos.m_positionX, pos.m_positionY, pos.m_positionZ + halfHeight,
        destx, desty, destz + halfHeight,
        destx, desty, destz, -0.5f);

    destz -= halfHeight;

    // Collided with a gameobject, move back to collision point
    if (col)
    {
        destx -= CONTACT_DISTANCE * std::cos(angle);
        desty -= CONTACT_DISTANCE * std::sin(angle);
        dist = std::sqrt((pos.m_positionX - destx)*(pos.m_positionX - destx) + (pos.m_positionY - desty) * (pos.m_positionY - desty));
    }

    float groundZ = VMAP_INVALID_HEIGHT_VALUE;
    Trinity::NormalizeMapCoord(pos.m_positionX);
    Trinity::NormalizeMapCoord(pos.m_positionY);
    UpdateAllowedPositionZ(destx, desty, destz, &groundZ);

    pos.SetOrientation(GetOrientation());
    pos.Relocate(destx, desty, destz);

    // position has no ground under it (or is too far away)
    if (groundZ <= INVALID_HEIGHT)
    {
        if (Unit const* unit = ToUnit())
        {
            // unit can fly, ignore.
            if (unit->CanFly())
                return;

            // fall back to gridHeight if any
            float gridHeight = GetMap()->GetGridHeight(pos.m_positionX, pos.m_positionY);
            if (gridHeight > INVALID_HEIGHT)
                pos.m_positionZ = gridHeight + unit->GetHoverOffset();
        }
    }
}

void WorldObject::SetPhaseMask(uint32 newPhaseMask, bool update)
{
    m_phaseMask = newPhaseMask;

    if (update && IsInWorld())
        UpdateObjectVisibility();
}

void WorldObject::RebuildTerrainSwaps()
{
    // Clear all terrain swaps, will be rebuilt below
    // Reason for this is, multiple phases can have the same terrain swap, we should not remove the swap if another phase still use it
    _terrainSwaps.clear();

    // Check all applied phases for terrain swap and add it only once
    for (uint32 phaseId : _phases)
    {
        if (std::vector<uint32> const* swaps = sObjectMgr->GetPhaseTerrainSwaps(phaseId))
        {
            for (uint32 const& swap : *swaps)
            {
                // only add terrain swaps for current map
                MapEntry const* mapEntry = sMapStore.LookupEntry(swap);
                if (!mapEntry || mapEntry->rootPhaseMap != int32(GetMapId()))
                    continue;

                if (sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_TERRAIN_SWAP, swap, this))
                    _terrainSwaps.insert(swap);
            }
        }
    }

    // get default terrain swaps, only for current map always
    if (std::vector<uint32> const* mapSwaps = sObjectMgr->GetDefaultTerrainSwaps(GetMapId()))
        for (uint32 const& swap : *mapSwaps)
            if (sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_TERRAIN_SWAP, swap, this))
                _terrainSwaps.insert(swap);

    // online players have a game client with world map display
    if (GetTypeId() == TYPEID_PLAYER)
        RebuildWorldMapAreaSwaps();
}

void WorldObject::RebuildWorldMapAreaSwaps()
{
    // Clear all world map area swaps, will be rebuilt below
    _worldMapSwaps.clear();

    // get ALL default terrain swaps, if we are using it (condition is true) 
    // send the worldmaparea for it, to see swapped worldmaparea in client from other maps too, not just from our current
    TerrainPhaseInfo const& defaults = sObjectMgr->GetDefaultTerrainSwapStore();
    for (TerrainPhaseInfo::const_iterator itr = defaults.begin(); itr != defaults.end(); ++itr)
        for (uint32 const& swap : itr->second)
            if (std::vector<uint32> const* uiMapSwaps = sObjectMgr->GetTerrainWorldMaps(swap))
                if (sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_TERRAIN_SWAP, swap, this))
                    for (uint32 worldMapAreaId : *uiMapSwaps)
                        _worldMapSwaps.insert(worldMapAreaId);

    // Check all applied phases for world map area swaps
    for (uint32 phaseId : _phases)
        if (std::vector<uint32> const* swaps = sObjectMgr->GetPhaseTerrainSwaps(phaseId))
            for (uint32 const& swap : *swaps)
                if (std::vector<uint32> const* uiMapSwaps = sObjectMgr->GetTerrainWorldMaps(swap))
                    if (sConditionMgr->IsObjectMeetingNotGroupedConditions(CONDITION_SOURCE_TYPE_TERRAIN_SWAP, swap, this))
                        for (uint32 worldMapAreaId : *uiMapSwaps)
                            _worldMapSwaps.insert(worldMapAreaId);
}

// Updates Area based phases, does not remove phases from auras
// Phases from gm commands are not taken into calculations, they can be lost!!
void WorldObject::UpdateAreaAndZonePhase()
{
    bool updateNeeded = false;
    PhaseAreaInfo const& allAreasPhases = sObjectMgr->GetAreaAndZonePhases();
    uint32 zoneAndArea[] = { GetZoneId(), GetAreaId() };

    // We first remove all phases from other areas & zones
    for (auto itr = allAreasPhases.begin(); itr != allAreasPhases.end(); ++itr)
        for (PhaseInfoStruct const& phase : itr->second)
            if (!IsInArea(GetAreaId(), itr->first))
                updateNeeded = SetPhased(phase.id, false, false) || updateNeeded; // not in area, remove phase, true if there was something removed

    // Then we add the phases from this area and zone if conditions are met
    // Zone is done before Area, so if Area does not meet condition, the phase will be removed
    for (uint32 area : zoneAndArea)
    {
        if (std::vector<PhaseInfoStruct> const* currentPhases = sObjectMgr->GetPhasesForArea(area))
        {
            for (PhaseInfoStruct const& phaseInfoStruct : *currentPhases)
            {
                bool apply = sConditionMgr->IsObjectMeetToConditions(this, phaseInfoStruct.Conditions);

                // add or remove phase depending of condition
                updateNeeded = SetPhased(phaseInfoStruct.id, false, apply) || updateNeeded;
            }
        }
    }

    // do not remove a phase if it would be removed by an area but we have the same phase from an aura
    if (Unit* unit = ToUnit())
    {
        Unit::AuraEffectList const& auraPhaseList = unit->GetAuraEffectsByType(SPELL_AURA_PHASE);
        for (Unit::AuraEffectList::const_iterator itr = auraPhaseList.begin(); itr != auraPhaseList.end(); ++itr)
        {
            uint32 phase = uint32((*itr)->GetMiscValueB());
            bool up = SetPhased(phase, false, true);
            if (!updateNeeded && up)
                updateNeeded = true;
        }
        Unit::AuraEffectList const& auraPhaseGroupList = unit->GetAuraEffectsByType(SPELL_AURA_PHASE_GROUP);
        for (Unit::AuraEffectList::const_iterator itr = auraPhaseGroupList.begin(); itr != auraPhaseGroupList.end(); ++itr)
        {
            bool up = false;
            uint32 phaseGroup = uint32((*itr)->GetMiscValueB());
            std::set<uint32> const& phases = GetPhasesForGroup(phaseGroup);
            for (uint32 phase : phases)
                up = SetPhased(phase, false, true);
            if (!updateNeeded && up)
                updateNeeded = true;
        }
    }

    // only update visibility and send packets if there was a change in the phase list
    if (updateNeeded && GetTypeId() == TYPEID_PLAYER && IsInWorld())
        ToPlayer()->GetSession()->SendSetPhaseShift(GetPhases(), GetTerrainSwaps(), GetWorldMapSwaps());

    // only update visibilty once, to prevent objects appearing for a moment while adding in multiple phases
    if (updateNeeded && IsInWorld())
        UpdateObjectVisibility();
}

bool WorldObject::HasPhaseList(uint32 phase)
{
    return _phases.find(phase) != _phases.end();
}

bool WorldObject::SetPhased(uint32 id, bool update, bool apply)
{
    if (id)
    {
        if (apply)
        {
            if (HasPhaseList(id)) // do not run the updates if we are already in this phase
                return false;
            _phases.insert(id);
        }
        else
        {
            if (!HasPhaseList(id)) // do not run the updates if we are not in this phase
                return false;
            _phases.erase(id);
        }
    }

    RebuildTerrainSwaps();

    if (update && IsInWorld())
        UpdateObjectVisibility();

    return true;
}

void WorldObject::ClearPhases(bool update)
{
    _phases.clear();

    RebuildTerrainSwaps();

    if (update && IsInWorld())
        UpdateObjectVisibility();
}

bool WorldObject::IsPhased(WorldObject const* obj) const
{
    if (_phases.empty() && obj->GetPhases().empty())
        return true;

    if (_phases.empty() && obj->IsPhased(169))
        return true;
    
    if (obj->GetPhases().empty() && IsPhased(169))
        return true;

    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->IsGameMaster())
        return true;

    return Trinity::Containers::Intersects(_phases.begin(), _phases.end(), obj->GetPhases().begin(), obj->GetPhases().end());
}

bool WorldObject::InSamePhase(WorldObject const* obj) const
{
    return InSamePhase(obj->GetPhaseMask()) && IsPhased(obj);
}

bool WorldObject::SetTerrainSwap(uint32 id, bool update, bool apply)
{
    if (id)
    {
        if (apply)
        {
            if (IsTerrainSwaped(id)) // do not run the updates if we are already in this terrain swap
                return false;
            _terrainSwaps.insert(id);
        }
        else
        {
            if (!IsTerrainSwaped(id)) // do not run the updates if we are not in this terrain swap
                return false;
            _terrainSwaps.erase(id);
        }
    }

    if (update && IsInWorld())
        UpdateObjectVisibility();
    return true;
}

void WorldObject::ClearTerrainSwaps(bool update)
{
    _terrainSwaps.clear();

    if (update && IsInWorld())
        UpdateObjectVisibility();
}

bool WorldObject::SetWorldMapSwap(uint32 id, bool update, bool apply)
{
    if (id)
    {
        if (apply)
        {
            if (IsWorldMapSwaped(id)) // do not run the updates if we are already in this world map swap
                return false;
            _worldMapSwaps.insert(id);
        }
        else
        {
            if (!IsWorldMapSwaped(id)) // do not run the updates if we are not in this world map swap
                return false;
            _worldMapSwaps.erase(id);
        }
    }

    if (update && IsInWorld())
        UpdateObjectVisibility();
    return true;
}

void WorldObject::ClearWorldMapSwap(bool update)
{
    _worldMapSwaps.clear();

    if (update && IsInWorld())
        UpdateObjectVisibility();
}

void WorldObject::PlayDistanceSound(uint32 sound_id, Player* target /*= nullptr*/)
{
    WorldPackets::Misc::PlayObjectSound packet;
    packet.SoundKitID = sound_id;
    packet.SourceObjectGUID = GetGUID();
    packet.TargetObjectGUID = target ? target->GetGUID() : GetGUID();

    if (target)
        target->SendDirectMessage(packet.Write());
    else
        SendMessageToSet(packet.Write(), true);
}

void WorldObject::PlayDirectSound(uint32 sound_id, Player* target /*= nullptr*/)
{
    WorldPackets::Misc::PlaySound packet;
    packet.SourceObjectGUID = GetGUID();
    packet.SoundKitID = sound_id;
    if (target)
        target->SendDirectMessage(packet.Write());
    else
        SendMessageToSet(packet.Write(), true);
}

void WorldObject::PlayDirectMusic(uint32 musicId, Player* target /*= nullptr*/)
{
    if (target)
        target->SendDirectMessage(WorldPackets::Misc::PlayMusic(musicId).Write());
    else
        SendMessageToSet(WorldPackets::Misc::PlayMusic(musicId).Write(), true);
}

void WorldObject::DestroyForNearbyPlayers()
{
    if (!IsInWorld())
        return;

    std::list<Player*> targets;
    Trinity::AnyPlayerInObjectRangeCheck check(this, GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, false);
    Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(this, targets, check);
    VisitNearbyWorldObject(GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, searcher, false, true);
    for (std::list<Player*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
    {
        Player* player = (*iter);

        if (player == this)
            continue;

        if (!player->HaveAtClient(this))
            continue;

        if (isType(TYPEMASK_UNIT) && ((Unit*)this)->GetCharmerGUID() == player->GetGUID()) /// @todo this is for puppet
            continue;

        DestroyForPlayer(player);
        player->m_clientGUIDs.erase(GetGUID());
        player->GetVignetteMgr().OnWorldObjectDisappear(this);
    }
}

void WorldObject::UpdateObjectVisibility(bool /*forced*/)
{
    //updates object's visibility for nearby players
    Trinity::VisibleChangesNotifier notifier(*this);
    VisitNearbyWorldObject(GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, notifier, false, true);
}

struct StealthVisibilityUpdateNotifier
{
    StealthVisibilityUpdateNotifier(WorldObject* me) : _me(me) { }

    template<class T> void Visit(GridRefManager<T> &) { }
    void Visit(PlayerMapType& m)
    {
        for (auto&& itr : m)
            itr.GetSource()->UpdateVisibilityOf(_me);
    }

    WorldObject* _me;
};

void WorldObject::UpdateStealthVisibility(uint32 diff)
{
    if (!m_stealth.GetFlags()) // Object is not stealthed
        return;

    m_stealthVisibilityUpdateTimer.Update(diff);
    if (!m_stealthVisibilityUpdateTimer.Passed())
        return;
    m_stealthVisibilityUpdateTimer.Reset(STEALTH_VISIBILITY_UPDATE_TIMER);

    StealthVisibilityUpdateNotifier notifier(this);
    VisitNearbyWorldObject(MAX_PLAYER_STEALTH_DETECT_RANGE, notifier);
}

struct WorldObjectChangeAccumulator
{
    UpdateDataMapType& i_updateDatas;
    WorldObject& i_object;
    std::set<uint64> plr_list;
    WorldObjectChangeAccumulator(WorldObject &obj, UpdateDataMapType &d) : i_updateDatas(d), i_object(obj) { }
    void Visit(PlayerMapType &m)
    {
        Player* source = NULL;
        for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            source = iter->GetSource();

            BuildPacket(source);

            if (!source->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = source->GetSharedVisionList().begin();
                for (; it != source->GetSharedVisionList().end(); ++it)
                    BuildPacket(*it);
            }
        }
    }

    void Visit(CreatureMapType &m)
    {
        Creature* source = NULL;
        for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            source = iter->GetSource();
            if (!source->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = source->GetSharedVisionList().begin();
                for (; it != source->GetSharedVisionList().end(); ++it)
                    BuildPacket(*it);
            }
        }
    }

    void Visit(DynamicObjectMapType &m)
    {
        DynamicObject* source = NULL;
        for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            source = iter->GetSource();
            ObjectGuid guid = source->GetCasterGUID();

            if (guid.IsPlayer())
            {
                //Caster may be NULL if DynObj is in removelist
                if (Player* caster = ObjectAccessor::FindPlayer(guid))
                    if (caster->GetGuidValue(PLAYER_FIELD_FARSIGHT_OBJECT) == source->GetGUID())
                        BuildPacket(caster);
            }
        }
    }

    void BuildPacket(Player* player)
    {
        // Only send update once to a player
        if (plr_list.find(player->GetGUID()) == plr_list.end() && player->HaveAtClient(&i_object))
        {
            i_object.BuildFieldsUpdate(player, i_updateDatas);
            plr_list.insert(player->GetGUID());
        }
    }

    template<class SKIP> void Visit(GridRefManager<SKIP> &) { }
};

void WorldObject::BuildUpdate(UpdateDataMapType& data_map)
{
    CellCoord p = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());
    Cell cell(p);
    cell.SetNoCreate();
    WorldObjectChangeAccumulator notifier(*this, data_map);
    TypeContainerVisitor<WorldObjectChangeAccumulator, WorldTypeMapContainer > player_notifier(notifier);
    Map& map = *GetMap();
    //we must build packets for all visible players
    cell.Visit(p, player_notifier, map, *this, GetVisibilityRange() + 2 * World::Visibility_RelocationLowerLimit, true);

    ClearUpdateMask(false);
}

ObjectGuid WorldObject::GetTransGUID() const
{
    if (GetTransport())
        return GetTransport()->GetGUID();
    return ObjectGuid::Empty;
}

float WorldObject::GetFloorZ() const
{
    if (!IsInWorld())
        return m_staticFloorZ;
    return std::max<float>(m_staticFloorZ, GetMap()->GetGameObjectFloor(GetPhaseMask(), GetPositionX(), GetPositionY(), GetPositionZ() + Z_OFFSET_FIND_HEIGHT));
}

float WorldObject::GetMapWaterOrGroundLevel(float x, float y, float z, float* ground/* = nullptr*/) const
{
    return GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), x, y, z, ground,
        isType(TYPEMASK_UNIT) ? !static_cast<Unit const*>(this)->HasAuraType(SPELL_AURA_WATER_WALK) : false,
        GetCollisionHeight());
}

float WorldObject::GetMapHeight(float x, float y, float z, bool vmap/* = true*/, float distanceToSearch/* = DEFAULT_HEIGHT_SEARCH*/) const
{
    if (z != MAX_HEIGHT)
        z += Z_OFFSET_FIND_HEIGHT;

    return GetMap()->GetHeight(GetPhaseMask(), x, y, z, vmap, distanceToSearch);
}

bool WorldObject::AddToTransportIfNeeded(Transport* transport, bool setHome, float extents)
{
    if (!transport)
        return false;

    if (GetTypeId() == TYPEID_UNIT)
    {
        switch (GetEntry())
        {
            case 37813: // Deathbringer Saurfang
            case 38508: // Blood Beast
                return false;
        }
    }

    // Some transports don't have GeoBox in dbc, sadly, assume object is always inside bounds
    if (GameObjectDisplayInfoEntry const* displayInfo = sGameObjectDisplayInfoStore.LookupEntry(transport->GetGOInfo()->displayId))
        if (displayInfo->minX != 0 || displayInfo->maxX != 0 || displayInfo->minY != 0 || displayInfo->maxY != 0 || displayInfo->minZ != 0 || displayInfo->maxZ != 0)
            if (!transport->IsAtInteractDistance(GetPosition(), extents))
                return false;

    float x, y, z, o;
    GetPosition(x, y, z, o);
    transport->AddPassenger(this);
    transport->CalculatePassengerOffset(x, y, z, &o);
    m_movementInfo.transport.pos.Relocate(x, y, z, o);

    if (setHome)
        if (Creature* creature = ToCreature())
            creature->SetTransportHomePosition(x, y, z, o);

    // Go from world rotation to transport-local rotation
    if (GameObject* go = ToGameObject())
        go->SetWorldRotation(transport->GetWorldRotation().inverse() * go->GetWorldRotation());

    return true;
}

WMOAreaTableEntry const* WorldObject::GetWMOArea() const
{
    Map * map = GetMap();
    if (!map)
        return NULL;

    uint32 mogpFlags;
    int32 adtId, rootId, groupId;

    if (map->GetAreaInfo(m_phaseMask, GetPositionX(), GetPositionY(), GetPositionZ(), mogpFlags, adtId, rootId, groupId))
    {
        const WMOAreaTableEntry * wmoEntry = GetWMOAreaTableEntryByTripple(rootId, adtId, groupId);
        if (wmoEntry)
            return wmoEntry;
    }

    return NULL;
}

uint32 WorldObject::GetWMOAreaId() const
{
    if (WMOAreaTableEntry const * wmoEntry = GetWMOArea())
        return wmoEntry->Id;
    return 0;
}

bool WorldObject::IsSpawnedInLFRDungeon() const
{
    if (Map* map = FindMap())
        if (map->IsRaid() && map->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
            if (InstanceMap* instance = dynamic_cast<InstanceMap*>(map))
                if (instance->IsLFGMap())
                    return true;
    return false;
}

void WorldObject::AddToUpdate()
{
    if (!m_currMap)
    {
        TC_LOG_ERROR("shitlog", "WorldObject::AddToUpdate - NULL in m_currMap (typeId %u, entry %u, guid " UI64FMTD ")",
            GetTypeId(), GetEntry(), GetGUID().GetRawValue());
        return;
    }

    m_currMap->AddUpdateObject(this);
    m_objectUpdated = true;
}

void WorldObject::RemoveFromUpdate()
{
    if (!m_currMap)
    {
        TC_LOG_ERROR("shitlog", "WorldObject::RemoveFromUpdate - NULL in m_currMap (typeId %u, entry %u, guid " UI64FMTD ")",
            GetTypeId(), GetEntry(), GetGUID().GetRawValue());
        return;
    }

    m_currMap->RemoveUpdateObject(this);
    m_objectUpdated = false;
}

void WorldObject::LoadCustomVisibility()
{
    // Make use of available retail data about AOI (Area of Interest) unless we have custom data set up
    float distance = 0;
    // For creatures, similar flags reside in CreatureStaticFlags, which we don't have access to
    if (GameObject const* go = ToGameObject())
    {
        // Following numbers are not confirmed on retail
        if (go->GetGOInfo()->HasTrueInfiniteAOI()) // Should be visible cross-maps, of course we're not going to implement that
            distance = 50000;
        else if (go->GetGOInfo()->HasInfiniteAOI())
            distance = 50000;
        else if (go->GetGOInfo()->HasGiganticAOI())
            distance = 500;
        else if (go->GetGOInfo()->HasLargeAOI())
            distance = 250;
    }

    if (CustomVisibility::Info const* info = sObjectMgr->GetCustomVisibilityInfo(GetTypeId(), GetEntry()))
    {
        if (info->Importance > (CustomVisibility::Importance)sWorld->getIntConfig(CONFIG_CUSTOM_VISIBILITY_ZONE_THRESHOLD))
        {
            SetCustomVisibilityDistance(distance, false);
            return;
        }

        bool zoneOnly = info->Importance > (CustomVisibility::Importance)sWorld->getIntConfig(CONFIG_CUSTOM_VISIBILITY_MAP_THRESHOLD);
        SetCustomVisibilityDistance(std::max(info->Distance, distance), zoneOnly);
        setActive(info->Active, ActiveFlags::CustomVisibility);
        return;
    }

    SetCustomVisibilityDistance(distance, false);
}

void WorldObject::SetCustomVisibilityDistance(float distance, bool zoneOnly)
{
    m_customVisibilityDistance = distance;

    if (m_hasCustomVisibility == (m_customVisibilityDistance > 0) && m_customVisibilityZoneOnly == zoneOnly)
    {
        UpdateObjectVisibility();
        return;
    }

    if (GetTypeId() == TYPEID_PLAYER)
        return;

    bool switchedZoneOnly = m_customVisibilityZoneOnly != zoneOnly;
    m_hasCustomVisibility = m_customVisibilityDistance > 0;
    m_customVisibilityZoneOnly = zoneOnly;

    if (!IsInWorld())
        return;

    Map* map = FindMap();
    if (!map)
        return;

    if (m_hasCustomVisibility)
    {
        if (switchedZoneOnly)
            map->RemoveCustomVisibilityObject(this, GetCustomVisibilityZoneID());

        SetCustomVisibilityZoneID(zoneOnly ? GetZoneId() : 0);

        map->AddCustomVisibilityObject(this, GetCustomVisibilityZoneID());
    }
    else
        map->RemoveCustomVisibilityObject(this, GetCustomVisibilityZoneID());

    UpdateObjectVisibility();
}

Battleground* WorldObject::GetBattlegorund() const
{
    if (IsInWorld() && m_currMap && m_currMap->IsBattlegroundOrArena())
        return ((BattlegroundMap*)m_currMap)->GetBG();
    return nullptr;
}
