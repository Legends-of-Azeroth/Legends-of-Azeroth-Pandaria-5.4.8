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

#include "SpellPackets.h"
#include "SharedDefines.h"
#include "Spell.h"
#include "SpellInfo.h"

namespace WorldPackets::Spells
{

WorldPacket const* ClearTarget::Write()
{
    _worldPacket.WriteBit(Guid[6]);
    _worldPacket.WriteBit(Guid[2]);
    _worldPacket.WriteBit(Guid[0]);
    _worldPacket.WriteBit(Guid[4]);
    _worldPacket.WriteBit(Guid[7]);
    _worldPacket.WriteBit(Guid[1]);
    _worldPacket.WriteBit(Guid[3]);
    _worldPacket.WriteBit(Guid[5]);

    _worldPacket.WriteByteSeq(Guid[4]);
    _worldPacket.WriteByteSeq(Guid[0]);
    _worldPacket.WriteByteSeq(Guid[3]);
    _worldPacket.WriteByteSeq(Guid[5]);
    _worldPacket.WriteByteSeq(Guid[2]);
    _worldPacket.WriteByteSeq(Guid[7]);
    _worldPacket.WriteByteSeq(Guid[6]);
    _worldPacket.WriteByteSeq(Guid[1]);

    return &_worldPacket;
}

void UseItem::Read()
{
    bool hasTransport = false;
    bool hasTransportTime2 = false;
    bool hasTransportTime3 = false;
    bool hasFallData = false;
    bool hasFallDirection = false;
    bool hasTimestamp = false;
    bool hasSplineElevation = false;
    bool hasPitch = false;
    bool hasOrientation = false;
    bool hasUnkMovementField = false;
    uint32 unkMovementLoopCounter = 0;
    ObjectGuid movementTransportGuid = ObjectGuid::Empty;
    ObjectGuid movementGuid = ObjectGuid::Empty;

    uint32 targetStringLength = 0;

    _worldPacket >> Slot >> BagIndex;

    bool hasElevation = !_worldPacket.ReadBit();
    ItemGuid[6] = _worldPacket.ReadBit();
    bool hasTargetString = !_worldPacket.ReadBit();
    ItemGuid[1] = _worldPacket.ReadBit();
    bool hasCastFlags = !_worldPacket.ReadBit();
    HasDestLocation = _worldPacket.ReadBit();
    ItemGuid[2] = _worldPacket.ReadBit();
    ItemGuid[7] = _worldPacket.ReadBit();
    ItemGuid[0] = _worldPacket.ReadBit();
    bool hasTargetMask = !_worldPacket.ReadBit();
    bool hasMissileSpeed = !_worldPacket.ReadBit();
    bool hasMovement = _worldPacket.ReadBit();
    bool hasCastCount = !_worldPacket.ReadBit();
    bool hasSpellId = !_worldPacket.ReadBit();
    _worldPacket.ReadBit();
    bool hasGlyphIndex = !_worldPacket.ReadBit();
    _worldPacket.ReadBit();
    ItemGuid[4] = _worldPacket.ReadBit();
    HasSrcLocation = _worldPacket.ReadBit();
    ItemGuid[3] = _worldPacket.ReadBit();
    ItemGuid[5] = _worldPacket.ReadBit();
    uint8 researchDataCount = _worldPacket.ReadBits(2);

    for (uint8 i = 0; i < researchDataCount; ++i)
        _worldPacket.ReadBits(2);

    if (hasMovement)
    {
        hasPitch = !_worldPacket.ReadBit();
        hasTransport = _worldPacket.ReadBit();
        _worldPacket.ReadBit();

        if (hasTransport)
        {
            movementTransportGuid[7] = _worldPacket.ReadBit();
            movementTransportGuid[2] = _worldPacket.ReadBit();
            movementTransportGuid[4] = _worldPacket.ReadBit();
            movementTransportGuid[5] = _worldPacket.ReadBit();
            movementTransportGuid[6] = _worldPacket.ReadBit();
            movementTransportGuid[0] = _worldPacket.ReadBit();
            movementTransportGuid[1] = _worldPacket.ReadBit();
            hasTransportTime3 = _worldPacket.ReadBit();
            movementTransportGuid[4] = _worldPacket.ReadBit();
            hasTransportTime2 = _worldPacket.ReadBit();
        }

        movementGuid[6] = _worldPacket.ReadBit();
        movementGuid[2] = _worldPacket.ReadBit();
        movementGuid[1] = _worldPacket.ReadBit();
        unkMovementLoopCounter = _worldPacket.ReadBits(22);
        _worldPacket.ReadBit();
        bool hasMovementFlags2 = !_worldPacket.ReadBit();
        hasFallData = _worldPacket.ReadBit();
        movementGuid[5] = _worldPacket.ReadBit();
        hasSplineElevation = !_worldPacket.ReadBit();
        _worldPacket.ReadBit();
        movementGuid[7] = _worldPacket.ReadBit();
        movementGuid[0] = _worldPacket.ReadBit();

        if (hasFallData)
            hasFallDirection = _worldPacket.ReadBit();

        hasOrientation = !_worldPacket.ReadBit();
        movementGuid[4] = _worldPacket.ReadBit();
        movementGuid[3] = _worldPacket.ReadBit();
        hasTimestamp = !_worldPacket.ReadBit();
        hasUnkMovementField = !_worldPacket.ReadBit();
        bool hasMovementFlags = !_worldPacket.ReadBit();

        if (hasMovementFlags2)
            _worldPacket.ReadBits(13);

        if (hasMovementFlags)
            _worldPacket.ReadBits(30);
    }

    if (HasSrcLocation)
    {
        TransportSrcGuid[3] = _worldPacket.ReadBit();
        TransportSrcGuid[1] = _worldPacket.ReadBit();
        TransportSrcGuid[7] = _worldPacket.ReadBit();
        TransportSrcGuid[4] = _worldPacket.ReadBit();
        TransportSrcGuid[2] = _worldPacket.ReadBit();
        TransportSrcGuid[0] = _worldPacket.ReadBit();
        TransportSrcGuid[6] = _worldPacket.ReadBit();
        TransportSrcGuid[5] = _worldPacket.ReadBit();
    }

    if (HasDestLocation)
    {
        TransportDstGuid[2] = _worldPacket.ReadBit();
        TransportDstGuid[4] = _worldPacket.ReadBit();
        TransportDstGuid[1] = _worldPacket.ReadBit();
        TransportDstGuid[7] = _worldPacket.ReadBit();
        TransportDstGuid[6] = _worldPacket.ReadBit();
        TransportDstGuid[0] = _worldPacket.ReadBit();
        TransportDstGuid[3] = _worldPacket.ReadBit();
        TransportDstGuid[5] = _worldPacket.ReadBit();
    }

    if (hasTargetString)
        targetStringLength = _worldPacket.ReadBits(7);

    TargetGuid[1] = _worldPacket.ReadBit();
    TargetGuid[0] = _worldPacket.ReadBit();
    TargetGuid[5] = _worldPacket.ReadBit();
    TargetGuid[3] = _worldPacket.ReadBit();
    TargetGuid[6] = _worldPacket.ReadBit();
    TargetGuid[4] = _worldPacket.ReadBit();
    TargetGuid[7] = _worldPacket.ReadBit();
    TargetGuid[2] = _worldPacket.ReadBit();

    ItemTargetGuid[4] = _worldPacket.ReadBit();
    ItemTargetGuid[5] = _worldPacket.ReadBit();
    ItemTargetGuid[0] = _worldPacket.ReadBit();
    ItemTargetGuid[1] = _worldPacket.ReadBit();
    ItemTargetGuid[3] = _worldPacket.ReadBit();
    ItemTargetGuid[7] = _worldPacket.ReadBit();
    ItemTargetGuid[6] = _worldPacket.ReadBit();
    ItemTargetGuid[2] = _worldPacket.ReadBit();

    if (hasCastFlags)
        CastFlags = _worldPacket.ReadBits(5);

    if (hasTargetMask)
        TargetMask = _worldPacket.ReadBits(20);

    _worldPacket.ReadByteSeq(ItemGuid[0]);
    _worldPacket.ReadByteSeq(ItemGuid[5]);
    _worldPacket.ReadByteSeq(ItemGuid[6]);
    _worldPacket.ReadByteSeq(ItemGuid[3]);
    _worldPacket.ReadByteSeq(ItemGuid[4]);
    _worldPacket.ReadByteSeq(ItemGuid[2]);
    _worldPacket.ReadByteSeq(ItemGuid[1]);

    for (uint8 i = 0; i < researchDataCount; ++i)
    {
        _worldPacket.read_skip<uint32>();
        _worldPacket.read_skip<uint32>();
    }

    _worldPacket.ReadByteSeq(ItemGuid[7]);

    if (hasMovement)
    {
        for (uint8 i = 0; i != unkMovementLoopCounter; i++)
            _worldPacket.read_skip<uint32>();

        if (hasTransport)
        {
            _worldPacket.read_skip<float>();              // Transport Position Y
            _worldPacket.read_skip<float>();              // Transport Position Z
            _worldPacket.ReadByteSeq(movementTransportGuid[1]);

            if (hasTransportTime3)
                _worldPacket.read_skip<uint32>();         // Transport Time 3

            _worldPacket.ReadByteSeq(movementTransportGuid[7]);
            _worldPacket.ReadByteSeq(movementTransportGuid[5]);
            _worldPacket.ReadByteSeq(movementTransportGuid[2]);
            _worldPacket.ReadByteSeq(movementTransportGuid[4]);
            _worldPacket.read_skip<float>();              // Transport Position X
            _worldPacket.read_skip<float>();              // Transport Orientation
            _worldPacket.ReadByteSeq(movementTransportGuid[0]);
            _worldPacket.read_skip<int8>();               // Transport Seat
            _worldPacket.read_skip<uint32>();             // Transport Time
            _worldPacket.ReadByteSeq(movementTransportGuid[6]);
            _worldPacket.ReadByteSeq(movementTransportGuid[3]);

            if (hasTransportTime2)
                _worldPacket.read_skip<uint32>();         // Transport Time 2
        }

        if (hasFallData)
        {
            _worldPacket.read_skip<float>();              // Z Speed

            if (hasFallDirection)
            {
                _worldPacket.read_skip<float>();          // SinAngle
                _worldPacket.read_skip<float>();          // CosAngle
                _worldPacket.read_skip<float>();          // XY Speed
            }

            _worldPacket.read_skip<uint32>();             // FallTime
        }

        _worldPacket.ReadByteSeq(movementGuid[3]);
        _worldPacket.ReadByteSeq(movementGuid[7]);
        _worldPacket.ReadByteSeq(movementGuid[6]);
        _worldPacket.ReadByteSeq(movementGuid[1]);
        _worldPacket.read_skip<float>();                  // Position Y

        if (hasSplineElevation)
            _worldPacket.read_skip<float>();              // Spline Elevation

        if (hasUnkMovementField)
            _worldPacket.read_skip<uint32>();

        if (hasOrientation)
            _worldPacket.read_skip<float>();              // Orientation

        _worldPacket.ReadByteSeq(movementGuid[2]);
        _worldPacket.read_skip<float>();                  // Position Z

        if (hasTimestamp)
            _worldPacket.read_skip<uint32>();             // Timestamp

        _worldPacket.read_skip<float>();                  // Position X
        _worldPacket.ReadByteSeq(movementGuid[5]);
        _worldPacket.ReadByteSeq(movementGuid[0]);

        if (hasPitch)
            _worldPacket.read_skip<float>();              // Pitch

        _worldPacket.ReadByteSeq(movementGuid[4]);
    }

    if (HasDestLocation)
    {
        float x, y, z;

        _worldPacket.ReadByteSeq(TransportDstGuid[7]);
        _worldPacket >> x;
        _worldPacket.ReadByteSeq(TransportDstGuid[0]);
        _worldPacket.ReadByteSeq(TransportDstGuid[6]);
        _worldPacket.ReadByteSeq(TransportDstGuid[1]);
        _worldPacket.ReadByteSeq(TransportDstGuid[3]);
        _worldPacket >> y;
        _worldPacket.ReadByteSeq(TransportDstGuid[5]);
        _worldPacket >> z;
        _worldPacket.ReadByteSeq(TransportDstGuid[4]);
        _worldPacket.ReadByteSeq(TransportDstGuid[2]);

        DstLoc.Relocate(x, y, z);
    }

    _worldPacket.ReadByteSeq(ItemTargetGuid[6]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[7]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[2]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[0]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[3]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[4]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[1]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[5]);

    if (HasSrcLocation)
    {
        float x, y, z;

        _worldPacket.ReadByteSeq(TransportSrcGuid[7]);
        _worldPacket >> x;
        _worldPacket.ReadByteSeq(TransportSrcGuid[1]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[5]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[4]);
        _worldPacket >> z;
        _worldPacket.ReadByteSeq(TransportSrcGuid[6]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[0]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[3]);
        _worldPacket >> y;
        _worldPacket.ReadByteSeq(TransportSrcGuid[2]);

        SrcLoc.Relocate(x, y, z);
    }

    if (hasSpellId)
        _worldPacket >> SpellId;

    _worldPacket.ReadByteSeq(TargetGuid[1]);
    _worldPacket.ReadByteSeq(TargetGuid[4]);
    _worldPacket.ReadByteSeq(TargetGuid[3]);
    _worldPacket.ReadByteSeq(TargetGuid[6]);
    _worldPacket.ReadByteSeq(TargetGuid[2]);
    _worldPacket.ReadByteSeq(TargetGuid[0]);
    _worldPacket.ReadByteSeq(TargetGuid[7]);
    _worldPacket.ReadByteSeq(TargetGuid[5]);

    if (hasTargetString)
        TargetString = _worldPacket.ReadString(targetStringLength);

    if (hasElevation)
        _worldPacket >> Elevation;

    if (hasGlyphIndex)
        _worldPacket >> GlyphIndex;

    if (hasMissileSpeed)
        _worldPacket >> MissileSpeed;

    if (hasCastCount)
        _worldPacket >> CastCount;
}

void CastSpell::Read()
{
    bool hasTransport = false;
    bool hasTransportTime2 = false;
    bool hasTransportTime3 = false;
    bool hasFallData = false;
    bool hasFallDirection = false;
    bool hasTimestamp = false;
    bool hasSplineElevation = false;
    bool hasPitch = false;
    bool hasOrientation = false;
    bool hasUnkMovementField = false;
    uint32 unkMovementLoopCounter = 0;
    ObjectGuid movementTransportGuid = ObjectGuid::Empty;
    ObjectGuid movementGuid = ObjectGuid::Empty;

    uint32 targetStringLength = 0;

    _worldPacket.ReadBit(); // Fake bit
    bool hasTargetString = !_worldPacket.ReadBit();
    _worldPacket.ReadBit(); // Fake Bit
    bool hasCastCount = !_worldPacket.ReadBit();
    HasSrcLocation = _worldPacket.ReadBit();
    HasDestLocation = _worldPacket.ReadBit();
    bool hasSpellId = !_worldPacket.ReadBit();
    uint8 researchDataCount = _worldPacket.ReadBits(2);
    bool hasTargetMask = !_worldPacket.ReadBit();
    bool hasMissileSpeed = !_worldPacket.ReadBit();

    ResearchInfoVector.reserve(researchDataCount);
    for (uint8 i = 0; i < researchDataCount; ++i)
        ResearchInfoVector.emplace_back(uint8(_worldPacket.ReadBits(2)), 0, 0);

    bool hasGlyphIndex = !_worldPacket.ReadBit();
    bool hasMovement = _worldPacket.ReadBit();
    bool hasElevation = !_worldPacket.ReadBit();
    bool hasCastFlags = !_worldPacket.ReadBit();

    TargetGuid[5] = _worldPacket.ReadBit();
    TargetGuid[4] = _worldPacket.ReadBit();
    TargetGuid[2] = _worldPacket.ReadBit();
    TargetGuid[7] = _worldPacket.ReadBit();
    TargetGuid[1] = _worldPacket.ReadBit();
    TargetGuid[6] = _worldPacket.ReadBit();
    TargetGuid[3] = _worldPacket.ReadBit();
    TargetGuid[0] = _worldPacket.ReadBit();

    if (HasDestLocation)
    {
        TransportDstGuid[1] = _worldPacket.ReadBit();
        TransportDstGuid[3] = _worldPacket.ReadBit();
        TransportDstGuid[5] = _worldPacket.ReadBit();
        TransportDstGuid[0] = _worldPacket.ReadBit();
        TransportDstGuid[2] = _worldPacket.ReadBit();
        TransportDstGuid[6] = _worldPacket.ReadBit();
        TransportDstGuid[7] = _worldPacket.ReadBit();
        TransportDstGuid[4] = _worldPacket.ReadBit();
    }

    if (hasMovement)
    {
        unkMovementLoopCounter = _worldPacket.ReadBits(22);
        _worldPacket.ReadBit();
        movementGuid[4] = _worldPacket.ReadBit();
        hasTransport = _worldPacket.ReadBit();

        if (hasTransport)
        {
            hasTransportTime2 = _worldPacket.ReadBit();
            movementTransportGuid[7] = _worldPacket.ReadBit();
            movementTransportGuid[4] = _worldPacket.ReadBit();
            movementTransportGuid[1] = _worldPacket.ReadBit();
            movementTransportGuid[0] = _worldPacket.ReadBit();
            movementTransportGuid[6] = _worldPacket.ReadBit();
            movementTransportGuid[3] = _worldPacket.ReadBit();
            movementTransportGuid[5] = _worldPacket.ReadBit();
            hasTransportTime3 = _worldPacket.ReadBit();
            movementTransportGuid[2] = _worldPacket.ReadBit();
        }

        _worldPacket.ReadBit();
        movementGuid[7] = _worldPacket.ReadBit();
        hasOrientation = !_worldPacket.ReadBit();
        movementGuid[6] = _worldPacket.ReadBit();
        hasSplineElevation = !_worldPacket.ReadBit();
        hasPitch = !_worldPacket.ReadBit();
        movementGuid[0] = _worldPacket.ReadBit();
        _worldPacket.ReadBit();
        bool hasMovementFlags = !_worldPacket.ReadBit();
        hasTimestamp = !_worldPacket.ReadBit();
        hasUnkMovementField = !_worldPacket.ReadBit();

        if (hasMovementFlags)
            _worldPacket.ReadBits(30);

        movementGuid[1] = _worldPacket.ReadBit();
        movementGuid[3] = _worldPacket.ReadBit();
        movementGuid[2] = _worldPacket.ReadBit();
        movementGuid[5] = _worldPacket.ReadBit();
        hasFallData = _worldPacket.ReadBit();

        if (hasFallData)
            hasFallDirection = _worldPacket.ReadBit();

        bool hasMovementFlags2 = !_worldPacket.ReadBit();

        if (hasMovementFlags2)
            _worldPacket.ReadBits(13);
    }

    ItemTargetGuid[1] = _worldPacket.ReadBit();
    ItemTargetGuid[0] = _worldPacket.ReadBit();
    ItemTargetGuid[7] = _worldPacket.ReadBit();
    ItemTargetGuid[4] = _worldPacket.ReadBit();
    ItemTargetGuid[6] = _worldPacket.ReadBit();
    ItemTargetGuid[5] = _worldPacket.ReadBit();
    ItemTargetGuid[3] = _worldPacket.ReadBit();
    ItemTargetGuid[2] = _worldPacket.ReadBit();

    if (HasSrcLocation)
    {
        TransportSrcGuid[4] = _worldPacket.ReadBit();
        TransportSrcGuid[5] = _worldPacket.ReadBit();
        TransportSrcGuid[3] = _worldPacket.ReadBit();
        TransportSrcGuid[0] = _worldPacket.ReadBit();
        TransportSrcGuid[7] = _worldPacket.ReadBit();
        TransportSrcGuid[1] = _worldPacket.ReadBit();
        TransportSrcGuid[6] = _worldPacket.ReadBit();
        TransportSrcGuid[2] = _worldPacket.ReadBit();
    }

    if (hasTargetMask)
        TargetMask = _worldPacket.ReadBits(20);

    if (hasCastFlags)
        CastFlags = _worldPacket.ReadBits(5);

    if (hasTargetString)
        targetStringLength = _worldPacket.ReadBits(7);

    for (uint8 i = 0; i < researchDataCount; ++i)
    {
        ResearchInfoVector[i].count = _worldPacket.read<uint32>();
        ResearchInfoVector[i].itemId = _worldPacket.read<uint32>();
    }

    if (hasMovement)
    {
        _worldPacket.read_skip<float>();                  // Position X
        _worldPacket.ReadByteSeq(movementGuid[0]);

        if (hasTransport)
        {
            _worldPacket.ReadByteSeq(movementTransportGuid[2]);
            _worldPacket.read_skip<int8>();               // Transport Seat
            _worldPacket.ReadByteSeq(movementTransportGuid[3]);
            _worldPacket.ReadByteSeq(movementTransportGuid[7]);
            _worldPacket.read_skip<float>();              // Transport Position X
            _worldPacket.ReadByteSeq(movementTransportGuid[5]);

            if (hasTransportTime3)
                _worldPacket.read_skip<uint32>();         // Transport Time 3

            _worldPacket.read_skip<float>();              // Transport Position Z
            _worldPacket.read_skip<float>();              // Transport Position Y

            _worldPacket.ReadByteSeq(movementTransportGuid[6]);
            _worldPacket.ReadByteSeq(movementTransportGuid[1]);
            _worldPacket.read_skip<float>();              // Transport Orientation

            _worldPacket.ReadByteSeq(movementTransportGuid[4]);

            if (hasTransportTime2)
                _worldPacket.read_skip<uint32>();         // Transport Time 2

            _worldPacket.ReadByteSeq(movementTransportGuid[0]);
            _worldPacket.read_skip<uint32>();             // Transport Time
        }

        _worldPacket.ReadByteSeq(movementGuid[5]);

        if (hasFallData)
        {
            _worldPacket.read_skip<uint32>();             // FallTime
            _worldPacket.read_skip<float>();              // Z Speed

            if (hasFallDirection)
            {
                _worldPacket.read_skip<float>();          // SinAngle
                _worldPacket.read_skip<float>();          // XY Speed
                _worldPacket.read_skip<float>();          // CosAngle
            }
        }

        if (hasSplineElevation)
            _worldPacket.read_skip<float>();              // Spline Elevation

        _worldPacket.ReadByteSeq(movementGuid[6]);

        if (hasUnkMovementField)
            _worldPacket.read_skip<uint32>();

        _worldPacket.ReadByteSeq(movementGuid[4]);

        if (hasOrientation)
            _worldPacket.read_skip<float>();              // Orientation

        if (hasTimestamp)
            _worldPacket.read_skip<uint32>();             // Timestamp

        _worldPacket.ReadByteSeq(movementGuid[1]);

        if (hasPitch)
            _worldPacket.read_skip<float>();              // Pitch

        _worldPacket.ReadByteSeq(movementGuid[3]);

        for (uint8 i = 0; i != unkMovementLoopCounter; i++)
            _worldPacket.read_skip<uint32>();

        _worldPacket.read_skip<float>();                  // Position Y
        _worldPacket.ReadByteSeq(movementGuid[7]);
        _worldPacket.read_skip<float>();                  // Position Z
        _worldPacket.ReadByteSeq(movementGuid[2]);
    }

    _worldPacket.ReadByteSeq(ItemTargetGuid[4]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[2]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[1]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[5]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[7]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[3]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[6]);
    _worldPacket.ReadByteSeq(ItemTargetGuid[0]);

    if (HasDestLocation)
    {
        float x, y, z;
        _worldPacket.ReadByteSeq(TransportDstGuid[2]);
        _worldPacket >> x;
        _worldPacket.ReadByteSeq(TransportDstGuid[4]);
        _worldPacket.ReadByteSeq(TransportDstGuid[1]);
        _worldPacket.ReadByteSeq(TransportDstGuid[0]);
        _worldPacket.ReadByteSeq(TransportDstGuid[3]);
        _worldPacket >> y;
        _worldPacket.ReadByteSeq(TransportDstGuid[7]);
        _worldPacket >> z;
        _worldPacket.ReadByteSeq(TransportDstGuid[5]);
        _worldPacket.ReadByteSeq(TransportDstGuid[6]);
        DstLoc.Relocate(x, y, z);
    }

    _worldPacket.ReadByteSeq(TargetGuid[3]);
    _worldPacket.ReadByteSeq(TargetGuid[4]);
    _worldPacket.ReadByteSeq(TargetGuid[7]);
    _worldPacket.ReadByteSeq(TargetGuid[6]);
    _worldPacket.ReadByteSeq(TargetGuid[2]);
    _worldPacket.ReadByteSeq(TargetGuid[0]);
    _worldPacket.ReadByteSeq(TargetGuid[1]);
    _worldPacket.ReadByteSeq(TargetGuid[5]);

    if (HasSrcLocation)
    {
        float x, y, z;
        _worldPacket >> y;
        _worldPacket.ReadByteSeq(TransportSrcGuid[5]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[1]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[7]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[6]);
        _worldPacket >> x;
        _worldPacket.ReadByteSeq(TransportSrcGuid[3]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[2]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[0]);
        _worldPacket.ReadByteSeq(TransportSrcGuid[4]);
        _worldPacket >> z;
        SrcLoc.Relocate(x, y, z);
    }

    if (hasTargetString)
        TargetString = _worldPacket.ReadString(targetStringLength);

    if (hasMissileSpeed)
        _worldPacket >> MissileSpeed;

    if (hasElevation)
        _worldPacket >> Elevation;

    if (hasCastCount)
        _worldPacket >> CastCount;

    if (hasSpellId)
        _worldPacket >> SpellId;

    if (hasGlyphIndex)
        _worldPacket >> GlyphIndex;
}

void PetCancelAura::Read()
{
    SpellId = _worldPacket.read<uint32>();

    PetGuid[0] = _worldPacket.ReadBit();
    PetGuid[3] = _worldPacket.ReadBit();
    PetGuid[1] = _worldPacket.ReadBit();
    PetGuid[4] = _worldPacket.ReadBit();
    PetGuid[6] = _worldPacket.ReadBit();
    PetGuid[2] = _worldPacket.ReadBit();
    PetGuid[7] = _worldPacket.ReadBit();
    PetGuid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(PetGuid[0]);
    _worldPacket.ReadByteSeq(PetGuid[5]);
    _worldPacket.ReadByteSeq(PetGuid[3]);
    _worldPacket.ReadByteSeq(PetGuid[4]);
    _worldPacket.ReadByteSeq(PetGuid[7]);
    _worldPacket.ReadByteSeq(PetGuid[2]);
    _worldPacket.ReadByteSeq(PetGuid[6]);
    _worldPacket.ReadByteSeq(PetGuid[1]);
}

void SpellClick::Read()
{
    Guid[7] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[0] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();

    _worldPacket.ReadBit(); // Unk

    Guid[1] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[7]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[0]);
}

void GetMirrorImageData::Read()
{
    _worldPacket >> DisplayId;

    Guid[0] = _worldPacket.ReadBit();
    Guid[2] = _worldPacket.ReadBit();
    Guid[1] = _worldPacket.ReadBit();
    Guid[6] = _worldPacket.ReadBit();
    Guid[5] = _worldPacket.ReadBit();
    Guid[4] = _worldPacket.ReadBit();
    Guid[7] = _worldPacket.ReadBit();
    Guid[3] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(Guid[6]);
    _worldPacket.ReadByteSeq(Guid[0]);
    _worldPacket.ReadByteSeq(Guid[3]);
    _worldPacket.ReadByteSeq(Guid[5]);
    _worldPacket.ReadByteSeq(Guid[4]);
    _worldPacket.ReadByteSeq(Guid[2]);
    _worldPacket.ReadByteSeq(Guid[1]);
    _worldPacket.ReadByteSeq(Guid[7]);
}

void MissileTrajectoryCollision::Read()
{
    Z = _worldPacket.read<float>();
    CastCount = _worldPacket.read<uint8>();
    Y = _worldPacket.read<float>();
    X = _worldPacket.read<float>();
    SpellId = _worldPacket.read<uint32>();

    CasterGuid[5] = _worldPacket.ReadBit();
    CasterGuid[0] = _worldPacket.ReadBit();
    CasterGuid[2] = _worldPacket.ReadBit();
    CasterGuid[6] = _worldPacket.ReadBit();
    CasterGuid[7] = _worldPacket.ReadBit();
    CasterGuid[3] = _worldPacket.ReadBit();
    CasterGuid[1] = _worldPacket.ReadBit();
    CasterGuid[4] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(CasterGuid[1]);
    _worldPacket.ReadByteSeq(CasterGuid[5]);
    _worldPacket.ReadByteSeq(CasterGuid[3]);
    _worldPacket.ReadByteSeq(CasterGuid[2]);
    _worldPacket.ReadByteSeq(CasterGuid[7]);
    _worldPacket.ReadByteSeq(CasterGuid[0]);
    _worldPacket.ReadByteSeq(CasterGuid[6]);
    _worldPacket.ReadByteSeq(CasterGuid[4]);
}

void ChoiceResponse::Read()
{
    ResponseId = _worldPacket.read<uint32>();
    ChoiceId = _worldPacket.read<uint32>();
}

}