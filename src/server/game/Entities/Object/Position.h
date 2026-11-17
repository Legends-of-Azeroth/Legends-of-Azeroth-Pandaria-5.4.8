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

#ifndef Trinity_game_Position_h__
#define Trinity_game_Position_h__

#include "Define.h"
#include <string>
#include <cmath>

class ByteBuffer;
class WorldObject;

struct TC_GAME_API Position
{
    Position()
        : m_positionX(0.0f), m_positionY(0.0f), m_positionZ(0.0f), m_orientation(0.0f) { }

    Position(float x, float y, float z = 0.0f, float o = 0.0f)
        : m_positionX(x), m_positionY(y), m_positionZ(z), m_orientation(NormalizeOrientation(o)) { }

    struct PositionXYZStreamer
    {
        explicit PositionXYZStreamer(Position& pos) : m_pos(&pos) { }
        Position* m_pos;
    };

    struct PositionXYZOStreamer
    {
        explicit PositionXYZOStreamer(Position& pos) : m_pos(&pos) { }
        Position* m_pos;
    };

    float m_positionX;
    float m_positionY;
    float m_positionZ;
    float m_orientation;
public:

    bool operator==(Position const &a);

    inline bool operator!=(Position const &a)
    {
        return !(operator==(a));
    }

    void Relocate(float x, float y) { m_positionX = x; m_positionY = y;}
    void Relocate(float x, float y, float z) { m_positionX = x; m_positionY = y; m_positionZ = z; }
    void Relocate(float x, float y, float z, float orientation) { m_positionX = x; m_positionY = y; m_positionZ = z; SetOrientation(orientation); }
    void Relocate(Position const &pos) { m_positionX = pos.m_positionX; m_positionY = pos.m_positionY; m_positionZ = pos.m_positionZ; SetOrientation(pos.m_orientation); }
    void Relocate(Position const* pos) { m_positionX = pos->m_positionX; m_positionY = pos->m_positionY; m_positionZ = pos->m_positionZ; SetOrientation(pos->m_orientation); }
    
    void RelocateOffset(Position const &offset);
    void RelocateOffset(float angle, float distance, float offsetZ = 0.0f);

    void SetOrientation(float orientation) { m_orientation = NormalizeOrientation(orientation); }

    float GetPositionX() const { return m_positionX; }
    float GetPositionY() const { return m_positionY; }
    float GetPositionZ() const { return m_positionZ; }
    float GetOrientation() const { return m_orientation; }

    void GetPosition(float &x, float &y) const
        { x = m_positionX; y = m_positionY; }
    void GetPosition(float &x, float &y, float &z) const
        { x = m_positionX; y = m_positionY; z = m_positionZ; }
    void GetPosition(float &x, float &y, float &z, float &o) const
        { x = m_positionX; y = m_positionY; z = m_positionZ; o = m_orientation; }

    Position GetPosition() const { return *this; }


    Position::PositionXYZStreamer PositionXYZStream()
    {
        return PositionXYZStreamer(*this);
    }
    Position::PositionXYZOStreamer PositionXYZOStream()
    {
        return PositionXYZOStreamer(*this);
    }

    bool IsPositionValid() const;

    constexpr float GetExactDist2dSq(const float x, const float y) const
    {
        float dx = x - m_positionX;
        float dy = y - m_positionY;
        return dx*dx + dy*dy;
    }
    constexpr float GetExactDist2dSq(Position const& pos) const { return GetExactDist2dSq(pos.m_positionX, pos.m_positionY); }
    constexpr float GetExactDist2dSq(Position const* pos) const { return GetExactDist2dSq(*pos); }

    float GetExactDist2d(const float x, const float y) const { return std::sqrt(GetExactDist2dSq(x, y)); }
    float GetExactDist2d(Position const& pos) const { return GetExactDist2d(pos.m_positionX, pos.m_positionY); }
    float GetExactDist2d(Position const* pos) const { return GetExactDist2d(*pos); }

    constexpr float GetExactDistSq(float x, float y, float z) const
    {
        float dz = z - m_positionZ;
        return GetExactDist2dSq(x, y) + dz*dz;
    }
    constexpr float GetExactDistSq(Position const& pos) const { return GetExactDistSq(pos.m_positionX, pos.m_positionY, pos.m_positionZ); }
    constexpr float GetExactDistSq(Position const* pos) const { return GetExactDistSq(*pos); }

    float GetExactDist(float x, float y, float z) const { return std::sqrt(GetExactDistSq(x, y, z)); }
    float GetExactDist(Position const& pos) const { return GetExactDist(pos.m_positionX, pos.m_positionY, pos.m_positionZ); }
    float GetExactDist(Position const* pos) const { return GetExactDist(*pos); }

    void GetPositionOffsetTo(Position const & endPos, Position & retOffset) const;

    float GetAbsoluteAngle(float x, float y) const
    {
        float dx = x - m_positionX;
        float dy = y - m_positionY;
        return NormalizeOrientation(std::atan2(dy, dx));
    }
    float GetAbsoluteAngle(Position const& pos) const { return GetAbsoluteAngle(pos.m_positionX, pos.m_positionY); }
    float GetAbsoluteAngle(Position const* pos) const { return GetAbsoluteAngle(*pos); }
    float ToAbsoluteAngle(float relAngle) const { return NormalizeOrientation(relAngle + m_orientation); }

    float GetAngle(Position const* pos) const;
    float GetAngle(float x, float y) const;
    float GetRelativeAngle(Position const* pos) const
        { return GetAngle(pos) - m_orientation; }
    float GetRelativeAngle(float x, float y) const { return GetAngle(x, y) - m_orientation; }
    void GetSinCos(float x, float y, float &vsin, float &vcos) const;

    constexpr bool IsInDist2d(float x, float y, float dist) const
        { return GetExactDist2dSq(x, y) < dist * dist; }
    constexpr bool IsInDist2d(Position const& pos, float dist) const
        { return GetExactDist2dSq(pos) < dist * dist; }
    constexpr bool IsInDist2d(Position const* pos, float dist) const
        { return GetExactDist2dSq(pos) < dist * dist; }

    constexpr bool IsInDist(float x, float y, float z, float dist) const
        { return GetExactDistSq(x, y, z) < dist * dist; }
    constexpr bool IsInDist(Position const& pos, float dist) const
        { return GetExactDistSq(pos) < dist * dist; }
    constexpr bool IsInDist(Position const* pos, float dist) const
        { return GetExactDistSq(pos) < dist * dist; }

    bool IsWithinBox(Position const& boxOrigin, float length, float width, float height) const;

    bool IsWithinVerticalCylinder(Position const& cylinderOrigin, float radius, float height, bool isDoubleVertical = false) const;

    bool HasInArc(float arcangle, Position const* pos, float border = 2.0f) const;
    bool HasInLine(WorldObject const* target, float width) const;
    std::string ToString() const;

    // constrain arbitrary radian orientation to interval [0,2*PI)
    static float NormalizeOrientation(float o);

    // (-PI, PI)
    static float NormalizePitch(float o)
    {
        if (o > -M_PI && o < M_PI)
            return o;

        o = NormalizeOrientation(o + M_PI) - M_PI;

        return o;
    }
};
ByteBuffer& operator>>(ByteBuffer& buf, Position::PositionXYZOStreamer const& streamer);
ByteBuffer& operator<<(ByteBuffer& buf, Position::PositionXYZStreamer const& streamer);
ByteBuffer& operator>>(ByteBuffer& buf, Position::PositionXYZStreamer const& streamer);
ByteBuffer& operator<<(ByteBuffer& buf, Position::PositionXYZOStreamer const& streamer);
#define MAPID_INVALID 0xFFFFFFFF

class WorldLocation : public Position
{
    public:
        explicit WorldLocation(uint32 _mapid = MAPID_INVALID, float _x = 0, float _y = 0, float _z = 0, float _o = 0)
            : m_mapId(_mapid) { Relocate(_x, _y, _z, _o); }
        WorldLocation(const WorldLocation &loc) { WorldRelocate(loc); }

        WorldLocation(uint32 mapId, Position const& position)
            : Position(position), m_mapId(mapId) { }

        void WorldRelocate(WorldLocation const& loc) { m_mapId = loc.GetMapId(); Relocate(loc); }
        void WorldRelocate(WorldLocation const* loc) { m_mapId = loc->GetMapId(); Relocate(loc); }
        void WorldRelocate(uint32 mapId, Position const& pos) { m_mapId = mapId; Relocate(pos); }
        void WorldRelocate(uint32 mapId = MAPID_INVALID, float x = 0.f, float y = 0.f, float z = 0.f, float o = 0.f)
        {
            m_mapId = mapId;
            Relocate(x, y, z, o);
        }

        WorldLocation GetWorldLocation() const { return *this; }

        uint32 GetMapId() const { return m_mapId; }

        uint32 m_mapId;
};

#endif // Trinity_game_Position_h__
