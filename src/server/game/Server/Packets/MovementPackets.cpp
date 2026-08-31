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

#include "MovementPackets.h"
#include <G3D/g3dmath.h>
#include <cmath>
#include "Log.h"
#include "Opcodes.h"

bool WorldPackets::Movement::ReadMovementInfo(WorldPacket& data, MovementInfo& mi, ::Movement::ExtraMovementStatusElement* extras /*= nullptr*/, uint32* mountDisplayId /*= nullptr*/, bool* hasMountDisplayId /*= nullptr*/)
{
    MovementStatusElements const* sequence = GetMovementStatusElementsSequence(data.GetOpcode());
    if (!sequence)
    {
        TC_LOG_ERROR("network", "ReadMovementInfo: No movement sequence found for opcode %s", GetOpcodeNameForLogging(static_cast<OpcodeClient>(data.GetOpcode())).c_str());
        return false;
    }

    bool _hasMountDisplayId = false;
    bool hasMovementFlags = false;
    bool hasMovementFlags2 = false;
    bool hasTimestamp = false;
    bool hasOrientation = false;
    bool hasTransportData = false;
    bool hasTransportTime2 = false;
    bool hasTransportTime3 = false;
    bool hasPitch = false;
    bool hasFallData = false;
    bool hasFallDirection = false;
    bool hasSplineElevation = false;
    bool hasCounter = false;
    uint32 forcesCount = 0u;

    ObjectGuid guid;
    ObjectGuid tguid;

    for (; *sequence != MSEEnd; ++sequence)
    {
        MovementStatusElements const& element = *sequence;

        switch (element)
        {
            case MSEHasGuidByte0:
            case MSEHasGuidByte1:
            case MSEHasGuidByte2:
            case MSEHasGuidByte3:
            case MSEHasGuidByte4:
            case MSEHasGuidByte5:
            case MSEHasGuidByte6:
            case MSEHasGuidByte7:
                guid[element - MSEHasGuidByte0] = data.ReadBit();
                break;
            case MSEHasTransportGuidByte0:
            case MSEHasTransportGuidByte1:
            case MSEHasTransportGuidByte2:
            case MSEHasTransportGuidByte3:
            case MSEHasTransportGuidByte4:
            case MSEHasTransportGuidByte5:
            case MSEHasTransportGuidByte6:
            case MSEHasTransportGuidByte7:
                if (hasTransportData)
                    tguid[element - MSEHasTransportGuidByte0] = data.ReadBit();
                break;
            case MSEGuidByte0:
            case MSEGuidByte1:
            case MSEGuidByte2:
            case MSEGuidByte3:
            case MSEGuidByte4:
            case MSEGuidByte5:
            case MSEGuidByte6:
            case MSEGuidByte7:
                data.ReadByteSeq(guid[element - MSEGuidByte0]);
                break;
            case MSETransportGuidByte0:
            case MSETransportGuidByte1:
            case MSETransportGuidByte2:
            case MSETransportGuidByte3:
            case MSETransportGuidByte4:
            case MSETransportGuidByte5:
            case MSETransportGuidByte6:
            case MSETransportGuidByte7:
                if (hasTransportData)
                    data.ReadByteSeq(tguid[element - MSETransportGuidByte0]);
                break;
            case MSEHasMovementFlags:
                hasMovementFlags = !data.ReadBit();
                break;
            case MSEHasMovementFlags2:
                hasMovementFlags2 = !data.ReadBit();
                break;
            case MSEHasTimestamp:
                hasTimestamp = !data.ReadBit();
                break;
            case MSEHasOrientation:
                hasOrientation = !data.ReadBit();
                break;
            case MSEHasTransportData:
                hasTransportData = data.ReadBit();
                break;
            case MSEHasTransportTime2:
                if (hasTransportData)
                    hasTransportTime2 = data.ReadBit();
                break;
            case MSEHasTransportTime3:
                if (hasTransportData)
                    hasTransportTime3 = data.ReadBit();
                break;
            case MSEHasPitch:
                hasPitch = !data.ReadBit();
                break;
            case MSEHasFallData:
                hasFallData = data.ReadBit();
                break;
            case MSEHasFallDirection:
                if (hasFallData)
                    hasFallDirection = data.ReadBit();
                break;
            case MSEHasSplineElevation:
                hasSplineElevation = !data.ReadBit();
                break;
            case MSEHasSpline:
                data.ReadBit();
                break;
            case MSEHasMountDisplayId:
                _hasMountDisplayId = !data.ReadBit();
                break;
            case MSEMountDisplayIdWithCheck: // Fallback here
                if (!_hasMountDisplayId)
                    break;
            case MSEMountDisplayIdWithoutCheck:
            {
                uint32 mountDisplayIdVal;
                data >> mountDisplayIdVal;
                if (mountDisplayId)
                    *mountDisplayId = mountDisplayIdVal;
                if (hasMountDisplayId)
                    *hasMountDisplayId = true;
                break;
            }
            case MSEMovementFlags:
                if (hasMovementFlags)
                    mi.flags = data.ReadBits(30);
                break;
            case MSEMovementFlags2:
                if (hasMovementFlags2)
                    mi.flags2 = data.ReadBits(13);
                break;
            case MSETimestamp:
                if (hasTimestamp)
                    data >> mi.time;
                break;
            case MSEPositionX:
                data >> mi.pos.m_positionX;
                break;
            case MSEPositionY:
                data >> mi.pos.m_positionY;
                break;
            case MSEPositionZ:
                data >> mi.pos.m_positionZ;
                break;
            case MSEOrientation:
                if (hasOrientation)
                    mi.pos.SetOrientation(data.read<float>());
                break;
            case MSETransportPositionX:
                if (hasTransportData)
                    data >> mi.transport.pos.m_positionX;
                break;
            case MSETransportPositionY:
                if (hasTransportData)
                    data >> mi.transport.pos.m_positionY;
                break;
            case MSETransportPositionZ:
                if (hasTransportData)
                    data >> mi.transport.pos.m_positionZ;
                break;
            case MSETransportOrientation:
                if (hasTransportData)
                    mi.transport.pos.SetOrientation(data.read<float>());
                break;
            case MSETransportSeat:
                if (hasTransportData)
                    data >> mi.transport.seat;
                break;
            case MSETransportTime:
                if (hasTransportData)
                    data >> mi.transport.time;
                break;
            case MSETransportTime2:
                if (hasTransportData && hasTransportTime2)
                    data >> mi.transport.time2;
                break;
            case MSETransportTime3:
                if (hasTransportData && hasTransportTime3)
                    data >> mi.transport.time3;
                break;
            case MSEPitch:
                if (hasPitch)
                    mi.pitch = G3D::wrap(data.read<float>(), float(-M_PI), float(M_PI));
                break;
            case MSEFallTime:
                if (hasFallData)
                    data >> mi.jump.fallTime;
                break;
            case MSEFallVerticalSpeed:
                if (hasFallData)
                    data >> mi.jump.zspeed;
                break;
            case MSEFallCosAngle:
                if (hasFallData && hasFallDirection)
                    data >> mi.jump.cosAngle;
                break;
            case MSEFallSinAngle:
                if (hasFallData && hasFallDirection)
                    data >> mi.jump.sinAngle;
                break;
            case MSEFallHorizontalSpeed:
                if (hasFallData && hasFallDirection)
                    data >> mi.jump.xyspeed;
                break;
            case MSESplineElevation:
                if (hasSplineElevation)
                    data >> mi.splineElevation;
                break;
            case MSEForcesCount:
                forcesCount = data.ReadBits(22);
                break;
            case MSEForces:
                for (uint32 i = 0; i < forcesCount; i++)
                    data.read_skip<uint32>();
                break;
            case MSEHasCounter:
                hasCounter = !data.ReadBit();
                break;
            case MSECounter:
                if (hasCounter)
                    data.read_skip<uint32>();
                break;
            case MSECount:
                data.read_skip<uint32>();
                break;
            case MSEZeroBit:
            case MSEOneBit:
                data.ReadBit();
                break;
            case MSEExtraElement:
                extras->ReadNextElement(data);
                break;
            default:
                ASSERT(::Movement::PrintInvalidSequenceElement(element, __FUNCTION__));
                break;
        }
    }

    mi.guid = guid;
    mi.transport.guid = tguid;
    return true;
}

void WorldPackets::Movement::ClientPlayerMovement::Read()
{
    WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status, nullptr, &_mountDisplayId, &_hasMountDisplayId);
}

void WorldPackets::Movement::MoveTeleportAck::Read()
{
    _worldPacket >> MoveTime >> Flags;

    MoverGUID[0] = _worldPacket.ReadBit();
    MoverGUID[7] = _worldPacket.ReadBit();
    MoverGUID[3] = _worldPacket.ReadBit();
    MoverGUID[5] = _worldPacket.ReadBit();
    MoverGUID[4] = _worldPacket.ReadBit();
    MoverGUID[6] = _worldPacket.ReadBit();
    MoverGUID[1] = _worldPacket.ReadBit();
    MoverGUID[2] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(MoverGUID[4]);
    _worldPacket.ReadByteSeq(MoverGUID[1]);
    _worldPacket.ReadByteSeq(MoverGUID[6]);
    _worldPacket.ReadByteSeq(MoverGUID[7]);
    _worldPacket.ReadByteSeq(MoverGUID[0]);
    _worldPacket.ReadByteSeq(MoverGUID[2]);
    _worldPacket.ReadByteSeq(MoverGUID[5]);
    _worldPacket.ReadByteSeq(MoverGUID[3]);
}

void WorldPackets::Movement::MoveSetActiveMover::Read()
{
    _worldPacket.ReadBit();

    MoverGUID[3] = _worldPacket.ReadBit();
    MoverGUID[0] = _worldPacket.ReadBit();
    MoverGUID[2] = _worldPacket.ReadBit();
    MoverGUID[1] = _worldPacket.ReadBit();
    MoverGUID[5] = _worldPacket.ReadBit();
    MoverGUID[4] = _worldPacket.ReadBit();
    MoverGUID[7] = _worldPacket.ReadBit();
    MoverGUID[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(MoverGUID[3]);
    _worldPacket.ReadByteSeq(MoverGUID[4]);
    _worldPacket.ReadByteSeq(MoverGUID[5]);
    _worldPacket.ReadByteSeq(MoverGUID[2]);
    _worldPacket.ReadByteSeq(MoverGUID[7]);
    _worldPacket.ReadByteSeq(MoverGUID[0]);
    _worldPacket.ReadByteSeq(MoverGUID[1]);
    _worldPacket.ReadByteSeq(MoverGUID[6]);
}

void WorldPackets::Movement::MoveKnockBackAck::Read()
{
    WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status);
}

void WorldPackets::Movement::MoveHoverAck::Read()
{
    WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status);
}

void WorldPackets::Movement::MoveWaterWalkAck::Read()
{
    WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status);
}

void WorldPackets::Movement::MoveSetCollisionHeightAck::Read()
{
    static MovementStatusElements const heightElements[] = { MSEExtraFloat, MSEExtra2Bits };
    ::Movement::ExtraMovementStatusElement extras(heightElements);
    WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status, &extras);
    Height = extras.Data.floatData;
    Reason = extras.Data.extra2BitsData;
}

void WorldPackets::Movement::MovementForceAck::Read()
{
    if (_worldPacket.GetOpcode() == CMSG_MOVE_APPLY_MOVEMENT_FORCE_ACK)
    {
        static MovementStatusElements const unkElements[] = { MSEExtraFloat, MSEExtraInt32, MSEExtraFloat };
        ::Movement::ExtraMovementStatusElement extras(unkElements);
        WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status, &extras);
        Speed = extras.Data.floatData;
        UnkInt = extras.Data.extraInt32Data;
        DirectionX = extras.Data.floatData2;
    }
    else
        WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status);
}

void WorldPackets::Movement::MoveSpeedChangeAck::Read()
{
    static MovementStatusElements const speedElement = MSEExtraFloat;
    ::Movement::ExtraMovementStatusElement extras(&speedElement);
    WorldPackets::Movement::ReadMovementInfo(_worldPacket, Status, &extras);
    Speed = extras.Data.floatData;
}

void WorldPackets::Movement::MoveSetCanTurnWhileFallingAck::Read()
{
}

void WorldPackets::Movement::SummonResponse::Read()
{
    SummonerGUID[1] = _worldPacket.ReadBit();
    SummonerGUID[3] = _worldPacket.ReadBit();
    SummonerGUID[5] = _worldPacket.ReadBit();
    SummonerGUID[2] = _worldPacket.ReadBit();
    Accept = _worldPacket.ReadBit();
    SummonerGUID[7] = _worldPacket.ReadBit();
    SummonerGUID[0] = _worldPacket.ReadBit();
    SummonerGUID[4] = _worldPacket.ReadBit();
    SummonerGUID[6] = _worldPacket.ReadBit();

    _worldPacket.ReadByteSeq(SummonerGUID[0]);
    _worldPacket.ReadByteSeq(SummonerGUID[1]);
    _worldPacket.ReadByteSeq(SummonerGUID[6]);
    _worldPacket.ReadByteSeq(SummonerGUID[3]);
    _worldPacket.ReadByteSeq(SummonerGUID[5]);
    _worldPacket.ReadByteSeq(SummonerGUID[4]);
    _worldPacket.ReadByteSeq(SummonerGUID[2]);
    _worldPacket.ReadByteSeq(SummonerGUID[7]);
}

