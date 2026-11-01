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

#include "PetPackets.h"
#include "World.h"

void WorldPackets::Pet::DismissCritter::Read()
{
    ObjectGuid guid;

    guid[4] = _worldPacket.ReadBit();
    guid[6] = _worldPacket.ReadBit();
    guid[7] = _worldPacket.ReadBit();
    guid[5] = _worldPacket.ReadBit();
    guid[1] = _worldPacket.ReadBit();
    guid[0] = _worldPacket.ReadBit();
    guid[2] = _worldPacket.ReadBit();
    guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(guid[2]);
    _worldPacket.ReadByteSeq(guid[4]);
    _worldPacket.ReadByteSeq(guid[5]);
    _worldPacket.ReadByteSeq(guid[0]);
    _worldPacket.ReadByteSeq(guid[1]);
    _worldPacket.ReadByteSeq(guid[7]);
    _worldPacket.ReadByteSeq(guid[3]);
    _worldPacket.ReadByteSeq(guid[6]);

    Guid = guid;
}

void WorldPackets::Pet::PetNameQuery::Read()
{
    ObjectGuid petGuid;
    ObjectGuid petNumber;

    petNumber[0] = _worldPacket.ReadBit();
    petNumber[5] = _worldPacket.ReadBit();
    petGuid[1] = _worldPacket.ReadBit();
    petGuid[7] = _worldPacket.ReadBit();
    petNumber[7] = _worldPacket.ReadBit();
    petGuid[6] = _worldPacket.ReadBit();
    petGuid[4] = _worldPacket.ReadBit();
    petGuid[5] = _worldPacket.ReadBit();
    petGuid[0] = _worldPacket.ReadBit();
    petNumber[3] = _worldPacket.ReadBit();
    petNumber[6] = _worldPacket.ReadBit();
    petNumber[2] = _worldPacket.ReadBit();
    petGuid[3] = _worldPacket.ReadBit();
    petGuid[2] = _worldPacket.ReadBit();
    petNumber[1] = _worldPacket.ReadBit();
    petNumber[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(petNumber[2]);
    _worldPacket.ReadByteSeq(petNumber[1]);
    _worldPacket.ReadByteSeq(petNumber[0]);
    _worldPacket.ReadByteSeq(petNumber[7]);
    _worldPacket.ReadByteSeq(petGuid[5]);
    _worldPacket.ReadByteSeq(petGuid[0]);
    _worldPacket.ReadByteSeq(petNumber[6]);
    _worldPacket.ReadByteSeq(petGuid[4]);
    _worldPacket.ReadByteSeq(petNumber[5]);
    _worldPacket.ReadByteSeq(petGuid[2]);
    _worldPacket.ReadByteSeq(petGuid[6]);
    _worldPacket.ReadByteSeq(petNumber[3]);
    _worldPacket.ReadByteSeq(petGuid[3]);
    _worldPacket.ReadByteSeq(petNumber[4]);
    _worldPacket.ReadByteSeq(petGuid[1]);
    _worldPacket.ReadByteSeq(petGuid[7]);

    PetGuid = petGuid;
    PetNumber = petNumber;
}

void WorldPackets::Pet::PetCastSpell::Read()
{
    uint8 castCount;
    uint32 spellId;
    uint8 castFlags;

    ObjectGuid targetGUID;
    ObjectGuid transportDstGUID;
    ObjectGuid itemGUID;
    ObjectGuid transportSrcGUID;
    ObjectGuid petGuid;
    ObjectGuid transportGUID;
    ObjectGuid guid20;

    bool bit388 = false;
    bool bit389 = false;
    bool bit412 = false;
    bool hasUnkMovementField = false;
    bool hasTransport = false;
    bool hasTransportTime2 = false;
    bool hasTransportTime3 = false;
    bool hasMovementFlags = false;
    bool hasMovementFlags2 = false;
    bool hasFallDirection = false;
    bool hasFallData = false;
    bool hasPitch = false;
    bool hasOrientation = false;
    bool hasSplineElevation = false;
    bool hasTimestamp = false;

    uint32 targetStrLenght = 0;
    uint32 movementForcesCount = 0;
    uint32 archeologyCount = 0;
    uint32 targetFlags = 0;

    WorldLocation dstLoc, srcLoc;

    float missileSpeed = 0.0f;
    float elevation = 0.0f;

    bool hasDestLocation = _worldPacket.ReadBit();
    petGuid[7] = _worldPacket.ReadBit();
    bool hasMissileSpeed = !_worldPacket.ReadBit();
    bool hasSrcLocation = _worldPacket.ReadBit();
    petGuid[1] = _worldPacket.ReadBit();
    archeologyCount = _worldPacket.ReadBits(2);
    bool hasTargetMask = !_worldPacket.ReadBit();
    petGuid[4] = _worldPacket.ReadBit();
    _worldPacket.ReadBit();
    petGuid[6] = _worldPacket.ReadBit();
    bool hasTargetString = !_worldPacket.ReadBit();
    _worldPacket.ReadBit();
    bool hasMovement = _worldPacket.ReadBit();
    bool hasCastFlags = !_worldPacket.ReadBit();
    bool hasSpellId = !_worldPacket.ReadBit();
    petGuid[0] = _worldPacket.ReadBit();
    petGuid[5] = _worldPacket.ReadBit();
    petGuid[2] = _worldPacket.ReadBit();
    for (uint32 i = 0; i < archeologyCount; ++i)
        _worldPacket.ReadBits(2);                            // archeologyType
    petGuid[3] = _worldPacket.ReadBit();
    bool hasGlyphIndex = !_worldPacket.ReadBit();
    bool hasCastCount = !_worldPacket.ReadBit();
    bool hasElevation = !_worldPacket.ReadBit();

    if (hasMovement)
    {
        hasOrientation = !_worldPacket.ReadBit();
        hasSplineElevation = !_worldPacket.ReadBit();
        bit388 = _worldPacket.ReadBit();
        guid20[5] = _worldPacket.ReadBit();
        guid20[7] = _worldPacket.ReadBit();
        hasMovementFlags2 = !_worldPacket.ReadBit();
        hasTimestamp = !_worldPacket.ReadBit();
        hasFallData = _worldPacket.ReadBit();
        hasMovementFlags = !_worldPacket.ReadBit();
        hasUnkMovementField = !_worldPacket.ReadBit();

        if (hasMovementFlags)
            _worldPacket.ReadBits(30);                       // Movement Flags

        bit389 = _worldPacket.ReadBit();
        guid20[6] = _worldPacket.ReadBit();
        hasTransport = _worldPacket.ReadBit();
        guid20[0] = _worldPacket.ReadBit();
        movementForcesCount = _worldPacket.ReadBits(22);

        if (hasTransport)
        {
            hasTransportTime2 = _worldPacket.ReadBit();
            hasTransportTime3 = _worldPacket.ReadBit();
            transportGUID[5] = _worldPacket.ReadBit();
            transportGUID[6] = _worldPacket.ReadBit();
            transportGUID[4] = _worldPacket.ReadBit();
            transportGUID[0] = _worldPacket.ReadBit();
            transportGUID[1] = _worldPacket.ReadBit();
            transportGUID[2] = _worldPacket.ReadBit();
            transportGUID[7] = _worldPacket.ReadBit();
            transportGUID[3] = _worldPacket.ReadBit();
        }

        guid20[1] = _worldPacket.ReadBit();

        if (hasMovementFlags2)
            _worldPacket.ReadBits(13);                       // Movement Flags 2

        guid20[3] = _worldPacket.ReadBit();
        guid20[2] = _worldPacket.ReadBit();
        bit412 = _worldPacket.ReadBit();
        hasPitch = !_worldPacket.ReadBit();
        guid20[4] = _worldPacket.ReadBit();

        if (hasFallData)
            hasFallDirection = _worldPacket.ReadBit();
    }

    if (hasDestLocation)
    {
        transportDstGUID[2] = _worldPacket.ReadBit();
        transportDstGUID[0] = _worldPacket.ReadBit();
        transportDstGUID[1] = _worldPacket.ReadBit();
        transportDstGUID[4] = _worldPacket.ReadBit();
        transportDstGUID[5] = _worldPacket.ReadBit();
        transportDstGUID[6] = _worldPacket.ReadBit();
        transportDstGUID[3] = _worldPacket.ReadBit();
        transportDstGUID[7] = _worldPacket.ReadBit();
    }

    if (hasCastFlags)
        castFlags = _worldPacket.ReadBits(5);                // Cast Flags

    itemGUID[2] = _worldPacket.ReadBit();
    itemGUID[4] = _worldPacket.ReadBit();
    itemGUID[7] = _worldPacket.ReadBit();
    itemGUID[0] = _worldPacket.ReadBit();
    itemGUID[6] = _worldPacket.ReadBit();
    itemGUID[1] = _worldPacket.ReadBit();
    itemGUID[5] = _worldPacket.ReadBit();
    itemGUID[3] = _worldPacket.ReadBit();

    if (hasTargetMask)
        targetFlags = _worldPacket.ReadBits(20);             // Target Flags

    if (hasTargetString)
        targetStrLenght = _worldPacket.ReadBits(7);          // Target String

    if (hasSrcLocation)
    {
        transportSrcGUID[2] = _worldPacket.ReadBit();
        transportSrcGUID[0] = _worldPacket.ReadBit();
        transportSrcGUID[3] = _worldPacket.ReadBit();
        transportSrcGUID[1] = _worldPacket.ReadBit();
        transportSrcGUID[6] = _worldPacket.ReadBit();
        transportSrcGUID[7] = _worldPacket.ReadBit();
        transportSrcGUID[4] = _worldPacket.ReadBit();
        transportSrcGUID[5] = _worldPacket.ReadBit();
    }

    targetGUID[6] = _worldPacket.ReadBit();
    targetGUID[0] = _worldPacket.ReadBit();
    targetGUID[3] = _worldPacket.ReadBit();
    targetGUID[4] = _worldPacket.ReadBit();
    targetGUID[2] = _worldPacket.ReadBit();
    targetGUID[1] = _worldPacket.ReadBit();
    targetGUID[5] = _worldPacket.ReadBit();
    targetGUID[7] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(petGuid[2]);
    _worldPacket.ReadByteSeq(petGuid[6]);
    _worldPacket.ReadByteSeq(petGuid[3]);

    for (uint32 i = 0; i < archeologyCount; ++i)
    {
        _worldPacket.read_skip<uint32>();                    // unk1
        _worldPacket.read_skip<uint32>();                    // unk2
    }
    _worldPacket.ReadByteSeq(petGuid[1]);
    _worldPacket.ReadByteSeq(petGuid[7]);
    _worldPacket.ReadByteSeq(petGuid[0]);
    _worldPacket.ReadByteSeq(petGuid[4]);
    _worldPacket.ReadByteSeq(petGuid[5]);

    if (hasDestLocation)
    {
        _worldPacket.ReadByteSeq(transportDstGUID[4]);
        _worldPacket.ReadByteSeq(transportDstGUID[1]);
        _worldPacket.ReadByteSeq(transportDstGUID[7]);
        dstLoc.m_positionZ = _worldPacket.read<float>();     // Position Z
        dstLoc.m_positionY = _worldPacket.read<float>();     // Position Y
        _worldPacket.ReadByteSeq(transportDstGUID[6]);
        _worldPacket.ReadByteSeq(transportDstGUID[3]);
        dstLoc.m_positionX = _worldPacket.read<float>();     // Position X
        _worldPacket.ReadByteSeq(transportDstGUID[2]);
        _worldPacket.ReadByteSeq(transportDstGUID[5]);
        _worldPacket.ReadByteSeq(transportDstGUID[0]);
    }

    if (hasMovement)
    {
        if (hasPitch)
            _worldPacket.read_skip<float>();                 // Pitch

        if (hasTransport)
        {
            if (hasTransportTime3)
                _worldPacket.read_skip<int32>();             // Transport Time 3

            if (hasTransportTime2)
                _worldPacket.read_skip<int32>();             // Transport Time 2

            _worldPacket.read_skip<uint8>();                 // Transport Seat
            _worldPacket.read_skip<float>();                 // Orientation
            _worldPacket.read_skip<float>();                 // Position Z
            _worldPacket.ReadByteSeq(transportGUID[2]);
            _worldPacket.read_skip<int32>();                 // Transport Time
            _worldPacket.ReadByteSeq(transportGUID[3]);
            _worldPacket.read_skip<float>();                 // Position X
            _worldPacket.ReadByteSeq(transportGUID[6]);
            _worldPacket.ReadByteSeq(transportGUID[5]);
            _worldPacket.ReadByteSeq(transportGUID[7]);
            _worldPacket.ReadByteSeq(transportGUID[0]);
            _worldPacket.read_skip<float>();                 // Position Y
            _worldPacket.ReadByteSeq(transportGUID[4]);
            _worldPacket.ReadByteSeq(transportGUID[1]);
        }

        if (hasUnkMovementField)
            _worldPacket.read_skip<int32>();

        for (uint32 i = 0; i < movementForcesCount; ++i)
            _worldPacket.read_skip<uint32>();                // Movement Forces

        _worldPacket.ReadByteSeq(guid20[3]);

        if (hasOrientation)
            _worldPacket.read_skip<float>();                 // Orientation

        _worldPacket.ReadByteSeq(guid20[5]);

        if (hasFallData)
        {
            _worldPacket.read_skip<float>();                 // Z Speed

            if (hasFallDirection)
            {
                _worldPacket.read_skip<float>();             // CosAngle
                _worldPacket.read_skip<float>();             // XY Speed
                _worldPacket.read_skip<float>();             // SinAngle
            }
            _worldPacket.read_skip<int32>();                 // FallTime
        }

        if (hasTimestamp)
            _worldPacket.read_skip<int32>();                 // Timestamp

        _worldPacket.ReadByteSeq(guid20[6]);
        _worldPacket.read_skip<float>();                     // Position X
        _worldPacket.ReadByteSeq(guid20[1]);
        _worldPacket.read_skip<float>();                     // Position Z
        _worldPacket.ReadByteSeq(guid20[2]);
        _worldPacket.ReadByteSeq(guid20[7]);
        _worldPacket.ReadByteSeq(guid20[0]);
        _worldPacket.read_skip<float>();                     // Position Y
        _worldPacket.ReadByteSeq(guid20[4]);

        if (hasSplineElevation)
            _worldPacket.read_skip<float>();                 // Spline Elevation
    }

    if (hasSrcLocation)
    {
        _worldPacket.ReadByteSeq(transportSrcGUID[3]);
        _worldPacket.ReadByteSeq(transportSrcGUID[4]);
        _worldPacket.ReadByteSeq(transportSrcGUID[2]);
        _worldPacket.ReadByteSeq(transportSrcGUID[1]);
        _worldPacket.ReadByteSeq(transportSrcGUID[0]);
        _worldPacket.ReadByteSeq(transportSrcGUID[7]);
        srcLoc.m_positionZ = _worldPacket.read<float>();     // Position Z
        _worldPacket.ReadByteSeq(transportSrcGUID[6]);
        _worldPacket.ReadByteSeq(transportSrcGUID[5]);
        srcLoc.m_positionX = _worldPacket.read<float>();     // Position X
        srcLoc.m_positionY = _worldPacket.read<float>();     // Position Y
    }

    if (hasMissileSpeed)
        missileSpeed = _worldPacket.read<float>();           // missileSpeed

    _worldPacket.ReadByteSeq(itemGUID[1]);
    _worldPacket.ReadByteSeq(itemGUID[2]);
    _worldPacket.ReadByteSeq(itemGUID[5]);
    _worldPacket.ReadByteSeq(itemGUID[7]);
    _worldPacket.ReadByteSeq(itemGUID[4]);
    _worldPacket.ReadByteSeq(itemGUID[6]);
    _worldPacket.ReadByteSeq(itemGUID[3]);
    _worldPacket.ReadByteSeq(itemGUID[0]);

    _worldPacket.ReadByteSeq(targetGUID[1]);
    _worldPacket.ReadByteSeq(targetGUID[5]);
    _worldPacket.ReadByteSeq(targetGUID[7]);
    _worldPacket.ReadByteSeq(targetGUID[3]);
    _worldPacket.ReadByteSeq(targetGUID[0]);
    _worldPacket.ReadByteSeq(targetGUID[2]);
    _worldPacket.ReadByteSeq(targetGUID[4]);
    _worldPacket.ReadByteSeq(targetGUID[6]);

    if (hasElevation)
        elevation = _worldPacket.read<float>();              // Elevation

    if (hasCastCount)
        castCount = _worldPacket.read<uint8>();              // Cast Count

    if (hasTargetString)
        _worldPacket.ReadString(targetStrLenght);

    if (hasGlyphIndex)
        _worldPacket.read_skip<int32>();                     // Glyph Index

    if (hasSpellId)
        spellId = _worldPacket.read<uint32>();

    CastCount = castCount;
    SpellId = spellId;
    CastFlags = castFlags;
    TargetFlags = targetFlags;
    TargetGuid = targetGUID;
    ItemGuid = itemGUID;
    TransportDstGuid = transportDstGUID;
    TransportSrcGuid = transportSrcGUID;
    PetGuid = petGuid;
    DstLoc = dstLoc;
    SrcLoc = srcLoc;
    MissileSpeed = missileSpeed;
    Elevation = elevation;
}

void WorldPackets::Pet::LearnPetSpecializationGroup::Read()
{
    ObjectGuid guid;

    Index = _worldPacket.read<uint32>();

    guid[5] = _worldPacket.ReadBit();
    guid[7] = _worldPacket.ReadBit();
    guid[3] = _worldPacket.ReadBit();
    guid[0] = _worldPacket.ReadBit();
    guid[6] = _worldPacket.ReadBit();
    guid[4] = _worldPacket.ReadBit();
    guid[1] = _worldPacket.ReadBit();
    guid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(guid[7]);
    _worldPacket.ReadByteSeq(guid[5]);
    _worldPacket.ReadByteSeq(guid[4]);
    _worldPacket.ReadByteSeq(guid[3]);
    _worldPacket.ReadByteSeq(guid[0]);
    _worldPacket.ReadByteSeq(guid[2]);
    _worldPacket.ReadByteSeq(guid[6]);
    _worldPacket.ReadByteSeq(guid[1]);

    Guid = guid;
}

void WorldPackets::Pet::PetAbandon::Read()
{
    Pet[7] = _worldPacket.ReadBit();
    Pet[3] = _worldPacket.ReadBit();
    Pet[4] = _worldPacket.ReadBit();
    Pet[2] = _worldPacket.ReadBit();
    Pet[5] = _worldPacket.ReadBit();
    Pet[6] = _worldPacket.ReadBit();
    Pet[1] = _worldPacket.ReadBit();
    Pet[0] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Pet[0]);
    _worldPacket.ReadByteSeq(Pet[2]);
    _worldPacket.ReadByteSeq(Pet[5]);
    _worldPacket.ReadByteSeq(Pet[6]);
    _worldPacket.ReadByteSeq(Pet[7]);
    _worldPacket.ReadByteSeq(Pet[1]);
    _worldPacket.ReadByteSeq(Pet[4]);
    _worldPacket.ReadByteSeq(Pet[3]);
}

void WorldPackets::Pet::PetStopAttack::Read()
{
    PetGUID[7] = _worldPacket.ReadBit();
    PetGUID[5] = _worldPacket.ReadBit();
    PetGUID[1] = _worldPacket.ReadBit();
    PetGUID[6] = _worldPacket.ReadBit();
    PetGUID[0] = _worldPacket.ReadBit();
    PetGUID[2] = _worldPacket.ReadBit();
    PetGUID[4] = _worldPacket.ReadBit();
    PetGUID[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetGUID[2]);
    _worldPacket.ReadByteSeq(PetGUID[5]);
    _worldPacket.ReadByteSeq(PetGUID[0]);
    _worldPacket.ReadByteSeq(PetGUID[4]);
    _worldPacket.ReadByteSeq(PetGUID[1]);
    _worldPacket.ReadByteSeq(PetGUID[7]);
    _worldPacket.ReadByteSeq(PetGUID[6]);
    _worldPacket.ReadByteSeq(PetGUID[3]);
}

void WorldPackets::Pet::PetSpellAutocast::Read()
{
    _worldPacket >> SpellID;

    PetGUID[0] = _worldPacket.ReadBit();
    PetGUID[4] = _worldPacket.ReadBit();
    PetGUID[2] = _worldPacket.ReadBit();
    PetGUID[6] = _worldPacket.ReadBit();
    PetGUID[1] = _worldPacket.ReadBit();
    PetGUID[5] = _worldPacket.ReadBit();
    PetGUID[3] = _worldPacket.ReadBit();
    PetGUID[7] = _worldPacket.ReadBit();

    AutocastEnabled = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetGUID[5]);
    _worldPacket.ReadByteSeq(PetGUID[0]);
    _worldPacket.ReadByteSeq(PetGUID[4]);
    _worldPacket.ReadByteSeq(PetGUID[1]);
    _worldPacket.ReadByteSeq(PetGUID[7]);
    _worldPacket.ReadByteSeq(PetGUID[2]);
    _worldPacket.ReadByteSeq(PetGUID[3]);
    _worldPacket.ReadByteSeq(PetGUID[6]);
}

void WorldPackets::Pet::PetAction::Read()
{
    _worldPacket >> Action;

    // Position
    _worldPacket >> Y >> Z >> X;

    PetGUID[1] = _worldPacket.ReadBit();
    PetGUID[0] = _worldPacket.ReadBit();
    PetGUID[6] = _worldPacket.ReadBit();
    PetGUID[7] = _worldPacket.ReadBit();
    PetGUID[5] = _worldPacket.ReadBit();
    TargetGUID[7] = _worldPacket.ReadBit();
    PetGUID[2] = _worldPacket.ReadBit();
    PetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[6] = _worldPacket.ReadBit();
    TargetGUID[3] = _worldPacket.ReadBit();
    TargetGUID[0] = _worldPacket.ReadBit();
    TargetGUID[2] = _worldPacket.ReadBit();
    TargetGUID[5] = _worldPacket.ReadBit();
    PetGUID[4] = _worldPacket.ReadBit();
    TargetGUID[4] = _worldPacket.ReadBit();
    TargetGUID[1] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetGUID[7]);
    _worldPacket.ReadByteSeq(PetGUID[6]);
    _worldPacket.ReadByteSeq(PetGUID[1]);
    _worldPacket.ReadByteSeq(PetGUID[2]);
    _worldPacket.ReadByteSeq(PetGUID[5]);
    _worldPacket.ReadByteSeq(PetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[5]);
    _worldPacket.ReadByteSeq(PetGUID[3]);
    _worldPacket.ReadByteSeq(TargetGUID[0]);
    _worldPacket.ReadByteSeq(TargetGUID[1]);
    _worldPacket.ReadByteSeq(TargetGUID[7]);
    _worldPacket.ReadByteSeq(TargetGUID[4]);
    _worldPacket.ReadByteSeq(TargetGUID[6]);
    _worldPacket.ReadByteSeq(TargetGUID[2]);
    _worldPacket.ReadByteSeq(TargetGUID[3]);
    _worldPacket.ReadByteSeq(PetGUID[0]);
}

void WorldPackets::Pet::PetSetAction::Read()
{
    _worldPacket >> Index >> Action;

    PetGUID[1] = _worldPacket.ReadBit();
    PetGUID[0] = _worldPacket.ReadBit();
    PetGUID[5] = _worldPacket.ReadBit();
    PetGUID[3] = _worldPacket.ReadBit();
    PetGUID[2] = _worldPacket.ReadBit();
    PetGUID[7] = _worldPacket.ReadBit();
    PetGUID[6] = _worldPacket.ReadBit();
    PetGUID[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetGUID[5]);
    _worldPacket.ReadByteSeq(PetGUID[6]);
    _worldPacket.ReadByteSeq(PetGUID[7]);
    _worldPacket.ReadByteSeq(PetGUID[3]);
    _worldPacket.ReadByteSeq(PetGUID[2]);
    _worldPacket.ReadByteSeq(PetGUID[1]);
    _worldPacket.ReadByteSeq(PetGUID[4]);
    _worldPacket.ReadByteSeq(PetGUID[0]);
}

void WorldPackets::Pet::PetRename::Read()
{
    _worldPacket >> PetNumber;

    HasName = !_worldPacket.ReadBit();
    IsDeclined = _worldPacket.ReadBit();

    uint8 declineNameLenght[MAX_DECLINED_NAME_CASES] = { 0, 0, 0, 0, 0 };
    if (IsDeclined)
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            declineNameLenght[i] = _worldPacket.ReadBits(7);

    if (HasName)
        Name = _worldPacket.ReadString(_worldPacket.ReadBits(8));

    if (IsDeclined)
    {
        DeclinedNames.resize(MAX_DECLINED_NAME_CASES);
        for (uint8 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            DeclinedNames[i] = _worldPacket.ReadString(declineNameLenght[i]);
    }
}
