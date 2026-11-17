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

#include "Position.h"
#include "ByteBuffer.h"

#include <G3D/g3dmath.h>
#include <G3D/Quat.h>

#include <sstream>

bool Position::operator==(Position const &a)
{
    return (G3D::fuzzyEq(a.m_positionX, m_positionX) &&
            G3D::fuzzyEq(a.m_positionY, m_positionY) &&
            G3D::fuzzyEq(a.m_positionZ, m_positionZ) &&
            G3D::fuzzyEq(a.m_orientation, m_orientation));
}

bool Position::IsWithinBox(Position const& boxOrigin, float length, float width, float height) const
{
    // rotate the WorldObject position instead of rotating the whole cube, that way we can make a simplified
    // is-in-cube check and we have to calculate only one point instead of 4

    // 2PI = 360*, keep in mind that ingame orientation is counter-clockwise
    double rotation = 2 * M_PI - boxOrigin.GetOrientation();
    double sinVal = std::sin(rotation);
    double cosVal = std::cos(rotation);

    float BoxDistX = GetPositionX() - boxOrigin.GetPositionX();
    float BoxDistY = GetPositionY() - boxOrigin.GetPositionY();

    float rotX = float(boxOrigin.GetPositionX() + BoxDistX * cosVal - BoxDistY * sinVal);
    float rotY = float(boxOrigin.GetPositionY() + BoxDistY * cosVal + BoxDistX * sinVal);

    // box edges are parallel to coordiante axis, so we can treat every dimension independently :D
    float dz = GetPositionZ() - boxOrigin.GetPositionZ();
    float dx = rotX - boxOrigin.GetPositionX();
    float dy = rotY - boxOrigin.GetPositionY();
    if ((std::fabs(dx) > length) ||
        (std::fabs(dy) > width)  ||
        (std::fabs(dz) > height))
        return false;

    return true;
}

bool Position::IsWithinVerticalCylinder(Position const& cylinderOrigin, float radius, float height, bool isDoubleVertical) const
{
    float verticalDelta = GetPositionZ() - cylinderOrigin.GetPositionZ();
    bool isValidPositionZ = isDoubleVertical ? std::abs(verticalDelta) <= height : 0 <= verticalDelta && verticalDelta <= height;

    return isValidPositionZ && IsInDist2d(cylinderOrigin, radius);
}

bool Position::HasInLine(WorldObject const* target, float width) const
{
    if (!HasInArc(M_PI, target))
        return false;
    width += target->GetObjectSize();
    float angle = GetRelativeAngle(target);
    return fabs(sin(angle)) * GetExactDist2d(target->GetPositionX(), target->GetPositionY()) < width;
}

std::string Position::ToString() const
{
    std::stringstream sstr;
    sstr << "X: " << m_positionX << " Y: " << m_positionY << " Z: " << m_positionZ << " O: " << m_orientation;
    return sstr.str();
}

float Position::NormalizeOrientation(float o)
{
    // fmod only supports positive numbers. Thus we have
    // to emulate negative numbers
    if (o < 0)
    {
        float mod = o *-1;
        mod = std::fmod(mod, 2.0f * static_cast<float>(M_PI));
        mod = -mod + 2.0f * static_cast<float>(M_PI);
        return mod;
    }
    return std::fmod(o, 2.0f * static_cast<float>(M_PI));
}

ByteBuffer& operator>>(ByteBuffer& buf, Position::PositionXYZOStreamer const& streamer)
{
    float x, y, z, o;
    buf >> x >> y >> z >> o;
    streamer.m_pos->Relocate(x, y, z, o);
    return buf;
}
ByteBuffer& operator<<(ByteBuffer& buf, Position::PositionXYZStreamer const& streamer)
{
    float x, y, z;
    streamer.m_pos->GetPosition(x, y, z);
    buf << x << y << z;
    return buf;
}

ByteBuffer& operator>>(ByteBuffer& buf, Position::PositionXYZStreamer const& streamer)
{
    float x, y, z;
    buf >> x >> y >> z;
    streamer.m_pos->Relocate(x, y, z);
    return buf;
}

ByteBuffer& operator<<(ByteBuffer& buf, Position::PositionXYZOStreamer const& streamer)
{
    float x, y, z, o;
    streamer.m_pos->GetPosition(x, y, z, o);
    buf << x << y << z << o;
    return buf;
}