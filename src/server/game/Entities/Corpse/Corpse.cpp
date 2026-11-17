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

#include "Common.h"
#include "Corpse.h"
#include "GameTime.h"
#include "Player.h"
#include "UpdateMask.h"
#include "ObjectAccessor.h"
#include "DatabaseEnv.h"
#include "Opcodes.h"
#include "GossipDef.h"
#include "World.h"

Corpse::Corpse(CorpseType type) : WorldObject(type != CORPSE_BONES), m_type(type)
{
    m_objectType |= TYPEMASK_CORPSE;
    m_objectTypeId = TYPEID_CORPSE;

    m_updateFlag = UPDATEFLAG_STATIONARY_POSITION;

    m_valuesCount = CORPSE_END;
    m_dynamicValuesCount = CORPSE_DYNAMIC_END;

    m_time = GameTime::GetGameTime();

    lootForBody = false;
    lootRecipient = nullptr;
}

Corpse::~Corpse() { }

void Corpse::AddToWorld()
{
    ///- Register the corpse for guid lookup
    if (!IsInWorld())
        GetMap()->GetObjectsStore().Insert<Corpse>(GetGUID(), this);

    Object::AddToWorld();
}

void Corpse::RemoveFromWorld()
{
    ///- Remove the corpse from the accessor
    if (IsInWorld())
        GetMap()->GetObjectsStore().Remove<Corpse>(GetGUID());

    Object::RemoveFromWorld();
}

bool Corpse::Create(ObjectGuid::LowType guidlow)
{
    Object::_Create(guidlow, 0, HighGuid::Corpse);
    return true;
}

bool Corpse::Create(uint32 guidlow, Player* owner)
{
    ASSERT(owner);

    Relocate(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), owner->GetOrientation());

    if (!IsPositionValid())
    {
        TC_LOG_ERROR("entities.player", "Corpse (guidlow {}, owner {}) not created. Suggested coordinates isn't valid (X: {} Y: {})",
            guidlow, owner->GetName().c_str(), owner->GetPositionX(), owner->GetPositionY());
        return false;
    }

    //we need to assign owner's map for corpse
    //in other way we will get a crash in Corpse::SaveToDB()
    SetMap(owner->GetMap());

    WorldObject::_Create(guidlow, HighGuid::Corpse, owner->GetPhaseMask(), owner->GetPhases());

    SetObjectScale(1);
    SetUInt64Value(CORPSE_FIELD_OWNER, owner->GetGUID());

    _cellCoord = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());

    return true;
}

void Corpse::SaveToDB()
{
    // prevent DB data inconsistence problems and duplicates
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    DeleteFromDB(trans);

    uint16 index = 0;
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CORPSE);
    stmt->setUInt32(index++, GetGUID().GetCounter());                                 // corpseGuid
    stmt->setUInt32(index++, GetOwnerGUID().GetCounter());                            // guid
    stmt->setFloat (index++, GetPositionX());                                         // posX
    stmt->setFloat (index++, GetPositionY());                                         // posY
    stmt->setFloat (index++, GetPositionZ());                                         // posZ
    stmt->setFloat (index++, GetOrientation());                                       // orientation
    stmt->setUInt16(index++, GetMapId());                                             // mapId
    stmt->setUInt32(index++, GetUInt32Value(CORPSE_FIELD_DISPLAY_ID));                // displayId
    stmt->setString(index++, _ConcatFields(CORPSE_FIELD_ITEMS, EQUIPMENT_SLOT_END));   // itemCache
    stmt->setUInt32(index++, GetUInt32Value(CORPSE_FIELD_BYTES_1));                   // bytes1
    stmt->setUInt32(index++, GetUInt32Value(CORPSE_FIELD_BYTES_2));                   // bytes2
    stmt->setUInt8 (index++, GetUInt32Value(CORPSE_FIELD_FLAGS));                     // flags
    stmt->setUInt8 (index++, GetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS));             // dynFlags
    stmt->setUInt32(index++, uint32(m_time));                                         // time
    stmt->setUInt8 (index++, GetType());                                              // corpseType
    stmt->setUInt32(index++, GetInstanceId());                                        // instanceId
    stmt->setUInt32(index++, GetPhaseMask());                                         // phaseMask
    trans->Append(stmt);

    // TODO: fix corpse phases

    CharacterDatabase.CommitTransaction(trans);
}

void Corpse::DeleteFromDB(CharacterDatabaseTransaction trans)
{
    DeleteFromDB(GetOwnerGUID(), trans);
}

void Corpse::DeleteFromDB(ObjectGuid const& ownerGuid, CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CORPSE);
    stmt->setUInt32(0, ownerGuid.GetCounter());
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

bool Corpse::LoadCorpseFromDB(uint32 guid, Field* fields)
{
    //        0     1     2     3            4      5          6          7       8       9      10        11    12          13          14          15         16
    // SELECT posX, posY, posZ, orientation, mapId, displayId, itemCache, bytes1, bytes2, flags, dynFlags, time, corpseType, instanceId, phaseMask, corpseGuid, guid FROM corpse

    uint32 ownerGuid = fields[16].GetUInt32();
    float posX   = fields[0].GetFloat();
    float posY   = fields[1].GetFloat();
    float posZ   = fields[2].GetFloat();
    float o      = fields[3].GetFloat();
    uint32 mapId = fields[4].GetUInt16();

    Object::_Create(guid, 0, HighGuid::Corpse);

    SetObjectScale(1);
    SetUInt32Value(CORPSE_FIELD_DISPLAY_ID, fields[5].GetUInt32());
    _LoadIntoDataField(fields[6].GetCString(), CORPSE_FIELD_ITEMS, EQUIPMENT_SLOT_END);
    SetUInt32Value(CORPSE_FIELD_BYTES_1, fields[7].GetUInt32());
    SetUInt32Value(CORPSE_FIELD_BYTES_2, fields[8].GetUInt32());
    SetUInt32Value(CORPSE_FIELD_FLAGS, fields[9].GetUInt8());
    SetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS, fields[10].GetUInt8());
    SetGuidValue(CORPSE_FIELD_OWNER, ObjectGuid(HighGuid::Player, ownerGuid));

    m_time = time_t(fields[11].GetUInt32());

    uint32 instanceId  = fields[13].GetUInt32();
    uint32 phaseMask   = fields[14].GetUInt32();

    // place
    SetLocationInstanceId(instanceId);
    SetLocationMapId(mapId);
    SetPhaseMask(phaseMask, false);
    Relocate(posX, posY, posZ, o);

    if (!IsPositionValid())
    {
        TC_LOG_ERROR("entities.player", "Corpse (guid: {}, owner: {}) is not created, given coordinates are not valid (X: {}, Y: {}, Z: {})",
            GetGUID().GetCounter(), GetOwnerGUID().GetCounter(), posX, posY, posZ);
        return false;
    }

    _cellCoord = Trinity::ComputeCellCoord(GetPositionX(), GetPositionY());
    return true;
}

bool Corpse::IsExpired(time_t t) const
{
    // Deleted character
    if (!sWorld->GetCharacterNameData(GetOwnerGUID()))
        return true;

    if (m_type == CORPSE_BONES)
        return m_time < t - 60 * MINUTE;
    else
        return m_time < t - 3 * DAY;
}
