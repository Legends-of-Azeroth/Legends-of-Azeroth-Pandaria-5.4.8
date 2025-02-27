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

#include "UpdateMask.h"
#include "Opcodes.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "GridNotifiers.h"
#include "CellImpl.h"
#include "ScriptMgr.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Transport.h"

DynamicObject::DynamicObject(bool isWorldObject) : WorldObject(isWorldObject),
    _aura(NULL), _removedAura(NULL), _caster(NULL), _duration(0), _isViewpoint(false)
{
    m_objectType |= TYPEMASK_DYNAMICOBJECT;
    m_objectTypeId = TYPEID_DYNAMICOBJECT;

    m_updateFlag = UPDATEFLAG_STATIONARY_POSITION;

    m_valuesCount = DYNAMICOBJECT_END;
}

DynamicObject::~DynamicObject()
{
    // make sure all references were properly removed
    ASSERT(!_aura);
    ASSERT(!_caster);
    ASSERT(!_isViewpoint);
    delete _removedAura;
}

void DynamicObject::AddToWorld()
{
    ///- Register the dynamicObject for guid lookup and for caster
    if (!IsInWorld())
    {
        GetMap()->GetObjectsStore().Insert<DynamicObject>(GetGUID(), this);
        WorldObject::AddToWorld();

        if (GetType() != DYNAMIC_OBJECT_RAID_MARKER)
            BindToCaster();
    }
}

void DynamicObject::RemoveFromWorld()
{
    ///- Remove the dynamicObject from the accessor and from all lists of objects in world
    if (IsInWorld())
    {
        if (_isViewpoint)
            RemoveCasterViewpoint();

        if (_aura)
            RemoveAura();

        // dynobj could get removed in Aura::RemoveAura
        if (!IsInWorld())
            return;

        if (GetType() != DYNAMIC_OBJECT_RAID_MARKER)
            UnbindFromCaster();

        WorldObject::RemoveFromWorld();
        GetMap()->GetObjectsStore().Remove<DynamicObject>(GetGUID());
    }
}

bool DynamicObject::CreateDynamicObject(uint32 guidlow, Unit* caster, SpellInfo const* spell, Position const& pos, float radius, DynamicObjectType type)
{
    _type = type;

    SetMap(caster->GetMap());
    Relocate(pos);
    if (!IsPositionValid())
    {
        TC_LOG_ERROR("misc", "DynamicObject (spell %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)", spell->Id, GetPositionX(), GetPositionY());
        return false;
    }

    WorldObject::_Create(guidlow, HighGuid::DynamicObject, caster->GetPhaseMask(), caster->GetPhases());

    SetEntry(spell->Id);
    SetObjectScale(1);
    SetUInt64Value(DYNAMICOBJECT_FIELD_CASTER, caster->GetGUID());

    if (type == DYNAMIC_OBJECT_RAID_MARKER) // The group is considered the caster.
    {
        ASSERT(caster->GetTypeId() == TYPEID_PLAYER && caster->ToPlayer()->GetGroup() && "DYNAMIC_OBJECT_RAID_MARKER must only be casted by players which are in the group.");
        SetUInt64Value(DYNAMICOBJECT_FIELD_CASTER, caster->ToPlayer()->GetGroup()->GetGUID());
    }
    else
        SetUInt64Value(DYNAMICOBJECT_FIELD_CASTER, caster->GetGUID());

    // The lower word of DYNAMICOBJECT_BYTES must be 0x0001. This value means that the visual radius will be overriden
    // by client for most of the "ground patch" visual effect spells and a few "skyfall" ones like Hurricane.
    // If any other value is used, the client will _always_ use the radius provided in DYNAMICOBJECT_RADIUS, but
    // precompensation is necessary (eg radius *= 2) for many spells. Anyway, blizz sends 0x0001 for all the spells
    // I saw sniffed...

    // Blizz set visual spell Id in 3 first byte of DYNAMICOBJECT_BYTES after 5.X
    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spell->Id);
    if (spellInfo)
    {
        uint32 visual = spellInfo->SpellVisual[0] ? spellInfo->SpellVisual[0] : spellInfo->SpellVisual[1];
        if ((spellInfo->Id == 5740 || spellInfo->Id == 104232) && caster->HasAura(101508)) // The Codex of Xerrath
            visual = 25761;
        SetUInt32Value(DYNAMICOBJECT_BYTES, 0x10000000 | visual);
    }

    SetUInt32Value(DYNAMICOBJECT_FIELD_SPELL_ID, spell->Id);
    SetFloatValue(DYNAMICOBJECT_FIELD_RADIUS, radius);
    SetUInt32Value(DYNAMICOBJECT_FIELD_CAST_TIME, getMSTime());

    LoadCustomVisibility();

    if (IsWorldObject())
        setActive(true, ActiveFlags::IsWorldObject);    //must before add to map to be put in world container

    // This object must be added to transport before adding to map for the client to properly display it
    if (AddToTransportIfNeeded(caster->GetTransport()))
        setActive(true, ActiveFlags::OnTransport);

    if (!GetMap()->AddToMap(this))
    {
        // Returning false will cause the object to be deleted - remove from transport
        if (Transport* transport = GetTransport())
            transport->RemovePassenger(this);
        return false;
    }

    return true;
}

void DynamicObject::Update(uint32 p_time)
{
    // caster has to be always available and in the same map except for raid markers
    if (GetType() == DYNAMIC_OBJECT_RAID_MARKER)
    {
        Group* group = sGroupMgr->GetGroupByGUID(GetCasterGUID());
        if (!group || !group->HasRaidMarker(GetGUID()))
        {
            Remove();
            return;
        }
    }
    else
    {
        // Caster can be 'not in world' at the time dynamic objects update, but are not yet deleted in Unit destructor.
        if (!_caster || _caster->GetMap() != GetMap())
        {
            Remove();
            return;
        }
    }

    bool expired = false;

    if (_aura)
    {
        if (!_aura->IsRemoved())
            _aura->UpdateOwner(p_time, this);

        // _aura may be set to null in Aura::UpdateOwner call
        if (_aura && (_aura->IsRemoved() || _aura->IsExpired()))
            expired = true;
    }
    else
    {
        if (GetDuration() > int32(p_time))
            _duration -= p_time;
        else
            expired = true;
    }

    if (expired)
    {
        if (GetType() == DYNAMIC_OBJECT_RAID_MARKER)
            if (Group* group = sGroupMgr->GetGroupByGUID(GetCasterGUID()))
                group->RemoveRaidMarker(group->GetRaidMarkerByGuid(GetGUID()));

        Remove();
    }
    else
        sScriptMgr->OnDynamicObjectUpdate(this, p_time);
}

void DynamicObject::Remove()
{
    if (IsInWorld())
    {
        SendObjectDeSpawnAnim(GetGUID());
        RemoveFromWorld();
        AddObjectToRemoveList();
    }
}

int32 DynamicObject::GetDuration() const
{
    if (!_aura)
        return _duration;
    else
        return _aura->GetDuration();
}

void DynamicObject::SetDuration(int32 newDuration)
{
    if (!_aura)
        _duration = newDuration;
    else
        _aura->SetDuration(newDuration);
}

void DynamicObject::Delay(int32 delaytime)
{
    SetDuration(GetDuration() + delaytime);
}

void DynamicObject::SetAura(Aura* aura)
{
    ASSERT(!_aura && aura);
    _aura = aura;
}

void DynamicObject::RemoveAura()
{
    ASSERT(_aura && !_removedAura);
    _removedAura = _aura;
    _aura = NULL;
    if (!_removedAura->IsRemoved())
        _removedAura->_Remove(AURA_REMOVE_BY_DEFAULT);
}

void DynamicObject::SetCasterViewpoint()
{
    if (Player* caster = _caster->ToPlayer())
    {
        caster->SetViewpoint(this, true);
        _isViewpoint = true;
    }
}

void DynamicObject::RemoveCasterViewpoint()
{
    if (Player* caster = _caster->ToPlayer())
    {
        caster->SetViewpoint(this, false);
        _isViewpoint = false;
    }
}

void DynamicObject::BindToCaster()
{
    ASSERT(!_caster);
    _caster = ObjectAccessor::GetUnit(*this, GetCasterGUID());
    ASSERT(_caster);
    ASSERT(_caster->GetMap() == GetMap());
    _caster->_RegisterDynObject(this);
}

void DynamicObject::UnbindFromCaster()
{
    ASSERT(_caster);
    _caster->_UnregisterDynObject(this);
    _caster = NULL;
}

void DynamicObject::BuildValuesUpdate(uint8 updateType, ByteBuffer* data, Player* target) const
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

            if (index == DYNAMICOBJECT_BYTES)
                *data << ((m_uint32Values[index] & 0xFFFF0000) | GetVisualForTarget(target));
            else
                *data << m_uint32Values[index];
        }
    }

    builder.Finish();
    BuildDynamicValuesUpdate(updateType, data);
}

uint32 DynamicObject::GetVisualForTarget(Player const* target) const
{
    auto getVisualIfHostile = [this](Player const* target, uint32 hostileViusal)
    {
        if (GetMap()->IsBattlegroundOrArena())
        {
            if (GetCaster()->ToPlayer())
                if (GetCaster()->ToPlayer()->GetBGTeam() != target->GetBGTeam())
                    return hostileViusal;
        }
        else if (GetCaster()->IsHostileTo(target))
            return hostileViusal;
        return m_uint32Values[DYNAMICOBJECT_BYTES];
    };

    switch (GetSpellId())
    {
        case 43265:     // Death and Decay
            return getVisualIfHostile(target, 29872);
        case 76577:     // Smoke Bomb
            return getVisualIfHostile(target, 20733);
        case 118009:    // Desecrated Ground
            return getVisualIfHostile(target, 20722);
        default:
            break;
    }

    return m_uint32Values[DYNAMICOBJECT_BYTES];
}
