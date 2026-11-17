/**
 *
 * Utility library to define some global function for geometric calculations
 *
 */

#ifndef _TRINITY_GEOMETRY_H
#define _TRINITY_GEOMETRY_H

#include <cstdlib>
#include <math.h>

[[nodiscard]] inline float getAngle(float startX, float startY, float destX, float destY)
{
    auto dx = destX - startX;
    auto dy = destY - startY;

    auto ang = std::atan2(dy, dx);
    ang = (ang >= 0) ? ang : 2 * float(M_PI) + ang;
    return ang;
}

[[nodiscard]] inline float getSlopeAngle(float startX, float startY, float startZ, float destX, float destY, float destZ)
{
    float floorDist = std::sqrt(pow(startY - destY, 2.0f) + pow(startX - destX, 2.0f));
    return atan(std::abs(destZ - startZ) / std::abs(floorDist));
}

[[nodiscard]] inline float getSlopeAngleAbs(float startX, float startY, float startZ, float destX, float destY, float destZ)
{
    return std::abs(getSlopeAngle(startX, startY, startZ, destX, destY, destZ));
}

[[nodiscard]] inline double getCircleAreaByRadius(double radius)
{
    return radius * radius * M_PI;
}

[[nodiscard]] inline double getCirclePerimeterByRadius(double radius)
{
    return radius * M_PI;
}

[[nodiscard]] inline double getCylinderVolume(double height, double radius)
{
    return height * getCircleAreaByRadius(radius);
}

#endif // _TRINITY_GEOMETRY_H
