#ifndef _TRINITY_PHYSICS_H
#define _TRINITY_PHYSICS_H

#include "Geometry.h"

using namespace std;

[[nodiscard]] inline float getWeight(float height, float width, float specificWeight)
{
    auto volume = getCylinderVolume(height, width / 2.0f);
    auto weight = volume * specificWeight;
    return weight;
}

/**
 * @brief Get the height immersed in water
 *
 * @param height
 * @param width
 * @param weight specific weight
 * @return float
 */
[[nodiscard]] inline float getOutOfWater(float width, float weight, float density)
{
    auto baseArea = getCircleAreaByRadius(width / 2.0f);
    return weight / (baseArea * density);
}

#endif
