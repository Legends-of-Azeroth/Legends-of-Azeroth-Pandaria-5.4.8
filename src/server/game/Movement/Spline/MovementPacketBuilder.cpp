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

#include "MovementPacketBuilder.h"
#include "MoveSpline.h"
#include "WorldPacket.h"
#include "Object.h"
#include "Unit.h"

namespace Movement
{
    inline void operator << (ByteBuffer& b, const Vector3& v)
    {
        b << v.x << v.y << v.z;
    }

    inline void operator >> (ByteBuffer& b, Vector3& v)
    {
        b >> v.x >> v.y >> v.z;
    }

    MonsterMoveType PacketBuilder::GetMonsterMoveType(MoveSpline const& moveSpline)
    {
        switch (moveSpline.splineflags & MoveSplineFlag::Mask_Final_Facing)
        {
            case MoveSplineFlag::Final_Target:
                return MonsterMoveFacingTarget;
            case MoveSplineFlag::Final_Angle:
                return MonsterMoveFacingAngle;
            case MoveSplineFlag::Final_Point:
                return MonsterMoveFacingPoint;
            default:
                return MonsterMoveNormal;
        }
    }

    void PacketBuilder::WriteStopMovement(Vector3 const& pos, uint32 splineId, ByteBuffer& data, Unit* unit)
    {

        ObjectGuid guid = unit->GetGUID();
        ObjectGuid transport = unit->GetTransGUID();

        data << float(pos.z);
        data << float(pos.x);
        data << uint32(splineId);
        data << float(pos.y);
        data << float(0.f); // Most likely transport Y
        data << float(0.f); // Most likely transport Z
        data << float(0.f); // Most likely transport X

        data.WriteBit(1); // Parabolic speed // esi+4Ch
        data.WriteBit(guid[0]);
        data.WriteBits(MonsterMoveStop, 3);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBits(0,  20);
        data.WriteBit(1);
        data.WriteBit(guid[3]);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[4]);
        data.WriteBit(1);
        data.WriteBit(guid[5]);
        data.WriteBits(0, 22); // WP count
        data.WriteBit(guid[6]);
        data.WriteBit(0); // Fake bit
        data.WriteBit(transport[7]);
        data.WriteBit(transport[1]);
        data.WriteBit(transport[3]);
        data.WriteBit(transport[0]);
        data.WriteBit(transport[6]);
        data.WriteBit(transport[4]);
        data.WriteBit(transport[5]);
        data.WriteBit(transport[2]);
        data.WriteBit(0); // Send no block
        data.WriteBit(0);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[1]);

        data.FlushBits();

        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(transport[6]);
        data.WriteByteSeq(transport[4]);
        data.WriteByteSeq(transport[1]);
        data.WriteByteSeq(transport[7]);
        data.WriteByteSeq(transport[0]);
        data.WriteByteSeq(transport[3]);
        data.WriteByteSeq(transport[5]);
        data.WriteByteSeq(transport[2]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[4]);
    }

    void WriteLinearPath(Spline<int32> const& spline, ByteBuffer& data)
    {
        uint32 last_idx = spline.getPointCount() - 3;
        Vector3 const* real_path = &spline.getPoint(1);

        if (last_idx > 1)
        {
            Vector3 middle = (real_path[0] + real_path[last_idx]) / 2.f;
            Vector3 offset;
            // first and last points already appended
            for (uint32 i = 1; i < last_idx; ++i)
            {
                offset = middle - real_path[i];
                data.appendPackXYZ(offset.x, offset.y, offset.z);
            }
        }
    }

    static void WriteCatmullRomPath(Spline<int32> const& spline, ByteBuffer& data)
    {
        for (int i = 2; i < spline.getPointCount() - 1; i++)
            data << spline.getPoint(i).y << spline.getPoint(i).x << spline.getPoint(i).z;
    }

    static void WriteCatmullRomCyclicPath(Spline<int32> const& spline, ByteBuffer& data)
    {
        for (int i = 2; i < spline.getPointCount() - 2; i++)
            data << spline.getPoint(i).y << spline.getPoint(i).x << spline.getPoint(i).z;
    }

    void PacketBuilder::WriteMonsterMove(const MoveSpline& moveSpline, WorldPacket& data, Unit* unit)
    {
        ObjectGuid guid = unit->GetGUID();
        ObjectGuid transport = unit->GetTransGUID();
        MonsterMoveType type = GetMonsterMoveType(moveSpline);
        G3D::Vector3 const& firstPoint = moveSpline.spline.getPoint(moveSpline.spline.first());

        MoveSplineFlag splineflags = moveSpline.splineflags;
        uint32 sendSplineFlags = splineflags.raw();
        if (sendSplineFlags & MoveSplineFlag::TransportEnter)
            sendSplineFlags = MoveSplineFlag::TransportEnter;
        int32 duration = moveSpline.Duration();
        if (sendSplineFlags & MoveSplineFlag::TransportEnter)
            duration = 0;
        int8 seat = unit->GetTransSeat();

        data << float(firstPoint.z);
        data << float(firstPoint.x);
        data << uint32(moveSpline.GetId());
        data << float(firstPoint.y);
        data << float(0.f); // Most likely transport Y
        data << float(0.f); // Most likely transport Z
        data << float(0.f); // Most likely transport X

        data.WriteBit(!moveSpline.splineflags.parabolic);
        data.WriteBit(guid[0]);
        data.WriteBits(type, 3);

        if (type == MonsterMoveFacingTarget)
        {
            ObjectGuid targetGuid = moveSpline.facing.target;
            data.WriteBit(targetGuid[6]);
            data.WriteBit(targetGuid[4]);
            data.WriteBit(targetGuid[3]);
            data.WriteBit(targetGuid[0]);
            data.WriteBit(targetGuid[5]);
            data.WriteBit(targetGuid[7]);
            data.WriteBit(targetGuid[1]);
            data.WriteBit(targetGuid[2]);
        }

        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(seat == -1);

        uint32 uncompressedSplineCount;
        if (moveSpline.splineflags & MoveSplineFlag::UncompressedPath)
        {
            if (moveSpline.splineflags.cyclic)
                uncompressedSplineCount = moveSpline.spline.getPointCount() - 4;    // WriteCatmullRomCyclicPath
            else
                uncompressedSplineCount = moveSpline.spline.getPointCount() - 3;    // WriteCatmullRomPath
        }
        else
            uncompressedSplineCount = 1;

        data.WriteBits(uncompressedSplineCount,  20);

        data.WriteBit(!sendSplineFlags);
        data.WriteBit(guid[3]);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(1);
        data.WriteBit(!duration);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[4]);
        data.WriteBit(1);
        data.WriteBit(guid[5]);

        int32 compressedSplineCount = moveSpline.splineflags & MoveSplineFlag::UncompressedPath ? 0 : moveSpline.spline.getPointCount() - 4;
        data.WriteBits(compressedSplineCount, 22); // WP count

        data.WriteBit(guid[6]);
        data.WriteBit(0); // Fake bit

        data.WriteBit(transport[7]);
        data.WriteBit(transport[1]);
        data.WriteBit(transport[3]);
        data.WriteBit(transport[0]);
        data.WriteBit(transport[6]);
        data.WriteBit(transport[4]);
        data.WriteBit(transport[5]);
        data.WriteBit(transport[2]);

        data.WriteBit(0); // Send no block
        data.WriteBit(0);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[1]);

        data.FlushBits();

        if (compressedSplineCount)
            WriteLinearPath(moveSpline.spline, data);

        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(transport[6]);
        data.WriteByteSeq(transport[4]);
        data.WriteByteSeq(transport[1]);
        data.WriteByteSeq(transport[7]);
        data.WriteByteSeq(transport[0]);
        data.WriteByteSeq(transport[3]);
        data.WriteByteSeq(transport[5]);
        data.WriteByteSeq(transport[2]);

        if (moveSpline.splineflags & MoveSplineFlag::UncompressedPath)
        {
            if (moveSpline.splineflags.cyclic)
                WriteCatmullRomCyclicPath(moveSpline.spline, data);
            else
                WriteCatmullRomPath(moveSpline.spline, data);
        }
        else
        {
            G3D::Vector3 const& point = moveSpline.spline.getPoint(moveSpline.spline.getPointCount() - 2);
            data << point.y << point.x << point.z;
        }

        if (type == MonsterMoveFacingTarget)
        {
            ObjectGuid targetGuid = moveSpline.facing.target;
            data.WriteByteSeq(targetGuid[5]);
            data.WriteByteSeq(targetGuid[7]);
            data.WriteByteSeq(targetGuid[0]);
            data.WriteByteSeq(targetGuid[4]);
            data.WriteByteSeq(targetGuid[3]);
            data.WriteByteSeq(targetGuid[2]);
            data.WriteByteSeq(targetGuid[6]);
            data.WriteByteSeq(targetGuid[1]);
        }

        data.WriteByteSeq(guid[5]);
        if (moveSpline.splineflags.parabolic)
            data << float(moveSpline.vertical_acceleration);

        if (type == MonsterMoveFacingAngle)
            data << float(moveSpline.facing.angle);

        data.WriteByteSeq(guid[3]);

        if (sendSplineFlags)
            data << uint32(sendSplineFlags);

        data.WriteByteSeq(guid[6]);

        if (type == MonsterMoveFacingPoint)
            data << moveSpline.facing.f.x << moveSpline.facing.f.y << moveSpline.facing.f.z;

        data.WriteByteSeq(guid[0]);

        if (seat != -1)
            data << seat;

        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[4]);

        if (duration)
            data << uint32(duration);
    }

    void PacketBuilder::WriteCreateBits(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (!data.WriteBit(!moveSpline.Finalized()))
            return;

        data.WriteBit(moveSpline.splineflags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation));
        data.WriteBit((moveSpline.splineflags & MoveSplineFlag::Parabolic) && moveSpline.effect_start_time < moveSpline.Duration());
        data.WriteBit(0); // NYI Block
        data.WriteBits(moveSpline.getPath().size(), 20);
        data.WriteBits(moveSpline.spline.mode(), 2);
        data.WriteBits(moveSpline.splineflags.raw(), 25);
    }

    void PacketBuilder::WriteCreateData(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (!moveSpline.Finalized())
        {
            MoveSplineFlag const& splineFlags = moveSpline.splineflags;
            MonsterMoveType type = GetMonsterMoveType(moveSpline);

            data << moveSpline.timePassed();
            data << float(1.f);                             // splineInfo.duration_mod_next; added in 3.1
            data << float(1.f);                             // splineInfo.duration_mod; added in 3.1

            uint32 nodes = moveSpline.getPath().size();
            for (uint32 i = 0; i < nodes; ++i)
            {
                data << float(moveSpline.getPath()[i].x);
                data << float(moveSpline.getPath()[i].z);
                data << float(moveSpline.getPath()[i].y);
            }

            if (splineFlags & (MoveSplineFlag::Parabolic | MoveSplineFlag::Animation))
                data << moveSpline.effect_start_time;       // added in 3.1

            data << uint8(type);

            if (type == MonsterMoveFacingAngle)
                data << float(moveSpline.facing.angle);

            if (type == MonsterMoveFacingPoint)
                data << moveSpline.facing.f.x << moveSpline.facing.f.z << moveSpline.facing.f.y;

            if ((splineFlags & MoveSplineFlag::Parabolic) && moveSpline.effect_start_time < moveSpline.Duration())
                data << float(moveSpline.vertical_acceleration);   // added in 3.1

            //    NYI block here
            data << moveSpline.Duration();

        }

        Vector3 destination = moveSpline.isCyclic() ? Vector3::zero() : moveSpline.FinalDestination();

        data << float(destination.x);
        data << float(destination.z);
        data << moveSpline.GetId();
        data << float(destination.y);

    }

    void PacketBuilder::WriteFacingTargetPart(MoveSpline const& moveSpline, ByteBuffer& data)
    {
        if (GetMonsterMoveType(moveSpline) == MonsterMoveFacingTarget && !moveSpline.Finalized())
        {
            ObjectGuid facingGuid = moveSpline.facing.target;
            data.WriteBit(facingGuid[4]);
            data.WriteBit(facingGuid[7]);
            data.WriteBit(facingGuid[0]);
            data.WriteBit(facingGuid[5]);
            data.WriteBit(facingGuid[1]);
            data.WriteBit(facingGuid[2]);
            data.WriteBit(facingGuid[3]);
            data.WriteBit(facingGuid[6]);

            data.WriteByteSeq(facingGuid[4]);
            data.WriteByteSeq(facingGuid[2]);
            data.WriteByteSeq(facingGuid[0]);
            data.WriteByteSeq(facingGuid[5]);
            data.WriteByteSeq(facingGuid[6]);
            data.WriteByteSeq(facingGuid[3]);
            data.WriteByteSeq(facingGuid[1]);
            data.WriteByteSeq(facingGuid[7]);
        }
    }

    void PacketBuilder::WriteSplineSync(MoveSpline const& mov, ByteBuffer& data, Unit* unit)
    {
        ObjectGuid guid = unit->GetGUID();
        data.WriteBit(guid[6]);
data.WriteBit(guid[4]);
data.WriteBit(guid[2]);
data.WriteBit(guid[7]);
data.WriteBit(guid[1]);
data.WriteBit(guid[3]);
data.WriteBit(guid[0]);
data.WriteBit(guid[5]);
        data.WriteByteSeq(guid[2]);
data.WriteByteSeq(guid[7]);
data.WriteByteSeq(guid[5]);
data.WriteByteSeq(guid[1]);
data.WriteByteSeq(guid[4]);
data.WriteByteSeq(guid[6]);
data.WriteByteSeq(guid[0]);
data.WriteByteSeq(guid[3]);
        data << float(mov.timePassed() / mov.Duration());
    }
}
