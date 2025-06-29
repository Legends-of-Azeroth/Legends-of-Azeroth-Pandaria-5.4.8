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

#include "WorldPosition.h"

#include <iomanip>
#include <numeric>

#include "CellImpl.h"
#include "MMapFactory.h"
#include "MapManager.h"
#include "PathGenerator.h"
#include "Playerbots.h"
#include "StrategyContext.h"
#include "TransportMgr.h"
#include "VMapFactory.h"
#include "VMapManager2.h"

WorldPosition::WorldPosition(std::string const str)
{
    std::stringstream out(str);
    out >> this->m_mapId;
    out >> this->m_positionX;
    out >> this->m_positionY;
    out >> this->m_positionZ;
    out >> this->m_orientation;
}

WorldPosition::WorldPosition(uint32 mapId, const Position& pos)
    : WorldLocation(mapId, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation())
{
}

WorldPosition::WorldPosition(WorldObject const* wo)
{
    if (wo)
    {
        set(WorldLocation(wo->GetMapId(), wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ(),
            wo->GetOrientation()));
    }
}

WorldPosition::WorldPosition(std::vector<WorldPosition*> list, WorldPositionConst conType)
{
    uint32 size = list.size();
    if (!size)
        return;

    if (size == 1)
        set(*list.front());
    else if (conType == WP_RANDOM)
        set(*list[urand(0, size - 1)]);
    else if (conType == WP_CENTROID)
    {
        set(std::accumulate(list.begin(), list.end(), WorldLocation(list[0]->getMapId(), 0, 0, 0, 0),
            [size](WorldLocation i, WorldPosition* j)
            {
                i.m_positionX += j->getX() / size;
                i.m_positionY += j->getY() / size;
                i.m_positionZ += j->getZ() / size;
                i.NormalizeOrientation(i.m_orientation += j->getO() / size);
                return i;
            }));
    }
    else if (conType == WP_MEAN_CENTROID)
    {
        WorldPosition pos = WorldPosition(list, WP_CENTROID);
        set(*pos.closestSq(list));
    }
}

WorldPosition::WorldPosition(std::vector<WorldPosition> list, WorldPositionConst conType)
{
    uint32 size = list.size();
    if (!size)
        return;

    if (size == 1)
        set(list.front());
    else if (conType == WP_RANDOM)
        set(list[urand(0, size - 1)]);
    else if (conType == WP_CENTROID)
    {
        set(std::accumulate(list.begin(), list.end(), WorldLocation(list[0].getMapId(), 0, 0, 0, 0),
            [size](WorldLocation i, WorldPosition& j)
            {
                i.m_positionX += j.getX() / size;
                i.m_positionY += j.getY() / size;
                i.m_positionZ += j.getZ() / size;
                i.NormalizeOrientation(i.m_orientation += j.getO() / size);
                return i;
            }));
    }
    else if (conType == WP_MEAN_CENTROID)
    {
        WorldPosition pos = WorldPosition(list, WP_CENTROID);
        set(pos.closestSq(list));
    }
}

WorldPosition::WorldPosition(uint32 mapid, GridCoord grid)
    : WorldLocation(mapid, (int32(grid.x_coord) - CENTER_GRID_ID - 0.5)* SIZE_OF_GRIDS + CENTER_GRID_OFFSET,
        (int32(grid.y_coord) - CENTER_GRID_ID - 0.5)* SIZE_OF_GRIDS + CENTER_GRID_OFFSET, 0, 0)
{
}

WorldPosition::WorldPosition(uint32 mapid, CellCoord cell)
    : WorldLocation(
        mapid, (int32(cell.x_coord) - CENTER_GRID_CELL_ID - 0.5)* SIZE_OF_GRID_CELL + CENTER_GRID_CELL_OFFSET,
        (int32(cell.y_coord) - CENTER_GRID_CELL_ID - 0.5)* SIZE_OF_GRID_CELL + CENTER_GRID_CELL_OFFSET, 0, 0)
{
}

WorldPosition::WorldPosition(uint32 mapid, mGridCoord grid)
    : WorldLocation(mapid, (32 - grid.first)* SIZE_OF_GRIDS, (32 - grid.second)* SIZE_OF_GRIDS, 0, 0)
{
}

void WorldPosition::set(const WorldLocation& pos) { WorldRelocate(pos); }

void WorldPosition::set(const WorldPosition& pos)
{
    WorldRelocate(pos.m_mapId, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
}

void WorldPosition::set(const WorldObject* pos)
{
    WorldRelocate(pos->GetMapId(), pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), pos->GetOrientation());
}

void WorldPosition::setMapId(uint32 id)
{
    m_mapId = id;
}

void WorldPosition::setX(float x) { m_positionX = x; }

void WorldPosition::setY(float y) { m_positionY = y; }

void WorldPosition::setZ(float z) { m_positionZ = z; }

void WorldPosition::setO(float o) { m_orientation = o; }

WorldPosition::operator bool() const
{
    return GetMapId() != 0 || GetPositionX() != 0 || GetPositionY() != 0 || GetPositionZ() != 0;
}

bool operator==(WorldPosition const& p1, const WorldPosition& p2)
{
    return p1.GetMapId() == p2.GetMapId() && p2.GetPositionX() == p1.GetPositionX() &&
        p2.GetPositionY() == p1.GetPositionY() && p2.GetPositionZ() == p1.GetPositionZ() &&
        p2.GetOrientation() == p1.GetOrientation();
}

bool operator!=(WorldPosition const& p1, const WorldPosition& p2) { return !(p1 == p2); }

WorldPosition& WorldPosition::operator+=(WorldPosition const& p1)
{
    m_positionX += p1.GetPositionX();
    m_positionY += p1.GetPositionY();
    m_positionZ += p1.GetPositionZ();
    return *this;
}

WorldPosition& WorldPosition::operator-=(WorldPosition const& p1)
{
    m_positionX -= p1.GetPositionX();
    m_positionY -= p1.GetPositionY();
    m_positionZ -= p1.GetPositionZ();
    return *this;
}

uint32 WorldPosition::getMapId() { return GetMapId(); }

float WorldPosition::getX() { return GetPositionX(); }

float WorldPosition::getY() { return GetPositionY(); }

float WorldPosition::getZ() { return GetPositionZ(); }

float WorldPosition::getO() { return GetOrientation(); }

bool WorldPosition::isOverworld()
{
    return GetMapId() == 0 || GetMapId() == 1 || GetMapId() == 530 || GetMapId() == 571;
}

bool WorldPosition::isInWater()
{
    return getMap() ? getMap()->IsInWater(PHASEMASK_NORMAL, GetPositionX(), GetPositionY(), GetPositionZ())
        : false;
};

bool WorldPosition::isUnderWater()
{
    return getMap() ? getMap()->IsUnderWater(PHASEMASK_NORMAL, GetPositionX(), GetPositionY(), GetPositionZ())
        : false;
};

WorldPosition WorldPosition::relPoint(WorldPosition* center)
{
    return WorldPosition(GetMapId(), GetPositionX() - center->GetPositionX(), GetPositionY() - center->GetPositionY(),
        GetPositionZ() - center->GetPositionZ(), GetOrientation());
}

WorldPosition WorldPosition::offset(WorldPosition* center)
{
    return WorldPosition(GetMapId(), GetPositionX() + center->GetPositionX(), GetPositionY() + center->GetPositionY(),
        GetPositionZ() + center->GetPositionZ(), GetOrientation());
}

float WorldPosition::size()
{
    return sqrt(pow(GetPositionX(), 2.0) + pow(GetPositionY(), 2.0) + pow(GetPositionZ(), 2.0));
}

float WorldPosition::distance(WorldPosition* center)
{
    if (GetMapId() == center->getMapId())
        return relPoint(center).size();

    // this -> mapTransfer | mapTransfer -> center
    //return sTravelMgr->mapTransDistance(*this, *center);

    return 0.0f;
};

float WorldPosition::fDist(WorldPosition* center)
{
    if (GetMapId() == center->getMapId())
        return sqrt(sqDistance2d(center));

    // this -> mapTransfer | mapTransfer -> center
    //return sTravelMgr->fastMapTransDistance(*this, *center);

    return 0.0f;
};

// When moving from this along list return last point that falls within range.
// Distance is move distance along path.
WorldPosition WorldPosition::lastInRange(std::vector<WorldPosition> list, float minDist, float maxDist)
{
    WorldPosition rPoint;

    float startDist = 0.0f;

    // Enter the path at the closest point.
    for (auto& p : list)
    {
        float curDist = distance(p);
        if (startDist < curDist || p == list.front())
            startDist = curDist + 0.1f;
    }

    float totalDist = 0.0f;

    // Follow the path from the last nearest point
    // Return last point in range.
    for (auto& p : list)
    {
        float curDist = distance(p);

        if (totalDist > 0)  // We have started the path. Keep counting.
            totalDist += p.distance(std::prev(&p, 1));

        if (curDist == startDist)  // Start the path here.
            totalDist = startDist;

        if (minDist > 0 && totalDist < minDist)
            continue;

        if (maxDist > 0 && totalDist > maxDist)
            continue;  // We do not break here because the path may loop back and have a second startDist point.

        rPoint = p;
    }

    return rPoint;
};

// Todo: remove or adjust to above standard.
WorldPosition WorldPosition::firstOutRange(std::vector<WorldPosition> list, float minDist, float maxDist)
{
    WorldPosition rPoint;

    for (auto& p : list)
    {
        if (minDist > 0 && distance(p) < minDist)
            return p;

        if (maxDist > 0 && distance(p) > maxDist)
            return p;

        rPoint = p;
    }

    return rPoint;
}

// Returns true if (on the x-y plane) the position is inside the three points.
bool WorldPosition::isInside(WorldPosition* p1, WorldPosition* p2, WorldPosition* p3)
{
    if (getMapId() != p1->getMapId() != p2->getMapId() != p3->getMapId())
        return false;

    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = mSign(p1, p2);
    d2 = mSign(p2, p3);
    d3 = mSign(p3, p1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

MapEntry const* WorldPosition::getMapEntry() { return sMapStore.LookupEntry(GetMapId()); };

uint32 WorldPosition::getInstanceId()
{
    if (Map* map = sMapMgr->FindBaseMap(getMapId()))
        return map->GetInstanceId();

    return 0;
}

Map* WorldPosition::getMap()
{
    return sMapMgr->FindMap(GetMapId(), getMapEntry()->Instanceable() ? getInstanceId() : 0);
}

float WorldPosition::getHeight()  // remove const - whipowill
{
    return getMap()->GetHeight(getX(), getY(), getZ());
}

G3D::Vector3 WorldPosition::getVector3() { return G3D::Vector3(GetPositionX(), GetPositionY(), GetPositionZ()); }

std::string const WorldPosition::print()
{
    std::ostringstream out;
    out << GetMapId() << std::fixed << std::setprecision(2);
    out << ';' << GetPositionX();
    out << ';' << GetPositionY();
    out << ';' << GetPositionZ();
    out << ';' << GetOrientation();

    return out.str();
}

std::string const WorldPosition::to_string()
{
    std::stringstream out;
    out << GetMapId();
    out << GetPositionX();
    out << GetPositionY();
    out << GetPositionZ();
    out << GetOrientation();
    return out.str();
};

void WorldPosition::printWKT(std::vector<WorldPosition> points, std::ostringstream& out, uint32 dim, bool loop)
{
    switch (dim)
    {
    case 0:
        if (points.size() == 1)
            out << "\"POINT(";
        else
            out << "\"MULTIPOINT(";
        break;
    case 1:
        out << "\"LINESTRING(";
        break;
    case 2:
        out << "\"POLYGON((";
    }

    for (auto& p : points)
        out << p.getDisplayX() << " " << p.getDisplayY() << (!loop && &p == &points.back() ? "" : ",");

    if (loop)
        out << points.front().getDisplayX() << " " << points.front().getDisplayY();

    out << (dim == 2 ? "))\"," : ")\",");
}

WorldPosition WorldPosition::getDisplayLocation()
{
    //WorldPosition pos = sTravelNodeMap->getMapOffset(getMapId());
    //return offset(const_cast<WorldPosition*>(&pos));

    return WorldPosition{};
}

uint16 WorldPosition::getAreaId() { return sMapMgr->GetAreaId(PHASEMASK_NORMAL, getMapId(), getX(), getY(), getZ()); }

AreaTableEntry const* WorldPosition::getArea()
{
    uint16 areaId = getAreaId();
    if (!areaId)
        return nullptr;

    return sAreaTableStore.LookupEntry(areaId);
}

std::string const WorldPosition::getAreaName(bool fullName, bool zoneName)
{
    if (!isOverworld())
    {
        MapEntry const* map = sMapStore.LookupEntry(getMapId());
        if (map)
            return map->name;
    }

    AreaTableEntry const* area = getArea();
    if (!area)
        return "";

    std::string areaName = area->area_name;

    if (fullName)
    {
        uint16 zoneId = area->ParentAreaID;

        while (zoneId > 0)
        {
            AreaTableEntry const* parentArea = sAreaTableStore.LookupEntry(zoneId);
            if (!parentArea)
                break;

            std::string const subAreaName = parentArea->area_name;

            if (zoneName)
                areaName = subAreaName;
            else
                areaName = subAreaName + " " + areaName;

            zoneId = parentArea->ParentAreaID;
        }
    }

    return std::move(areaName);
}

std::set<Transport*> WorldPosition::getTransports(uint32 entry)
{
    /*
    if(!entry)
        return getMap()->m_transports;
    else
    {
    */
    std::set<Transport*> transports;
    /*
    for (auto transport : getMap()->m_transports)
        if(transport->GetEntry() == entry)
            transports.insert(transport);

    return transports;
}
*/
    return transports;
}

std::vector<GridCoord> WorldPosition::getGridCoord(WorldPosition secondPos)
{
    std::vector<GridCoord> retVec;

    int lx = std::min(getGridCoord().x_coord, secondPos.getGridCoord().x_coord);
    int ly = std::min(getGridCoord().y_coord, secondPos.getGridCoord().y_coord);
    int ux = std::max(getGridCoord().x_coord, secondPos.getGridCoord().x_coord);
    int uy = std::max(getGridCoord().y_coord, secondPos.getGridCoord().y_coord);

    int32 border = 1;

    lx = std::min(std::max(border, lx), MAX_NUMBER_OF_GRIDS - border);
    ly = std::min(std::max(border, ly), MAX_NUMBER_OF_GRIDS - border);
    ux = std::min(std::max(border, ux), MAX_NUMBER_OF_GRIDS - border);
    uy = std::min(std::max(border, uy), MAX_NUMBER_OF_GRIDS - border);

    for (int x = lx - border; x <= ux + border; x++)
    {
        for (int y = ly - border; y <= uy + border; y++)
        {
            retVec.push_back(GridCoord(x, y));
        }
    }

    return retVec;
}

std::vector<WorldPosition> WorldPosition::fromGridCoord(GridCoord gridCoord)
{
    std::vector<WorldPosition> retVec;
    GridCoord g;

    for (uint32 d = 0; d < 4; d++)
    {
        g = gridCoord;

        if (d == 1 || d == 2)
            g.inc_x(1);

        if (d == 2 || d == 3)
            g.inc_y(1);

        retVec.push_back(WorldPosition(getMapId(), g));
    }

    return retVec;
}

std::vector<WorldPosition> WorldPosition::fromCellCoord(CellCoord cellcoord)
{
    std::vector<WorldPosition> retVec;
    CellCoord p;

    for (uint32 d = 0; d < 4; d++)
    {
        p = cellcoord;

        if (d == 1 || d == 2)
            p.inc_x(1);

        if (d == 2 || d == 3)
            p.inc_y(1);

        retVec.push_back(WorldPosition(getMapId(), p));
    }
    return retVec;
}

std::vector<WorldPosition> WorldPosition::gridFromCellCoord(CellCoord cellCoord)
{
    Cell c(cellCoord);

    return fromGridCoord(GridCoord(c.GridX(), c.GridY()));
}

std::vector<std::pair<int32, int32>> WorldPosition::getmGridCoords(WorldPosition secondPos)
{
    std::vector<mGridCoord> retVec;

    int lx = std::min(getmGridCoord().first, secondPos.getmGridCoord().first);
    int ly = std::min(getmGridCoord().second, secondPos.getmGridCoord().second);
    int ux = std::max(getmGridCoord().first, secondPos.getmGridCoord().first);
    int uy = std::max(getmGridCoord().second, secondPos.getmGridCoord().second);
    int border = 1;

    // lx = std::min(std::max(border, lx), MAX_NUMBER_OF_GRIDS - border);
    // ly = std::min(std::max(border, ly), MAX_NUMBER_OF_GRIDS - border);
    // ux = std::min(std::max(border, ux), MAX_NUMBER_OF_GRIDS - border);
    // uy = std::min(std::max(border, uy), MAX_NUMBER_OF_GRIDS - border);

    for (int x = lx - border; x <= ux + border; x++)
    {
        for (int y = ly - border; y <= uy + border; y++)
        {
            retVec.push_back(std::make_pair(x, y));
        }
    }

    return retVec;
}

std::vector<WorldPosition> WorldPosition::frommGridCoord(mGridCoord GridCoord)
{
    std::vector<WorldPosition> retVec;
    mGridCoord g;

    for (uint32 d = 0; d < 4; d++)
    {
        g = GridCoord;

        if (d == 1 || d == 2)
            g.second++;
        if (d == 2 || d == 3)
            g.first++;

        retVec.push_back(WorldPosition(getMapId(), g));
    }

    return retVec;
}

void WorldPosition::loadMapAndVMap(uint32 mapId, uint8 x, uint8 y)
{
    
}

void WorldPosition::loadMapAndVMaps(WorldPosition secondPos)
{
    for (auto& grid : getmGridCoords(secondPos))
    {
        loadMapAndVMap(getMapId(), grid.first, grid.second);
    }
}

std::vector<WorldPosition> WorldPosition::fromPointsArray(std::vector<G3D::Vector3> path)
{
    std::vector<WorldPosition> retVec;
    for (auto p : path)
        retVec.push_back(WorldPosition(getMapId(), p.x, p.y, p.z, getO()));

    return retVec;
}

// A single pathfinding attempt from one position to another. Returns pathfinding status and path.
std::vector<WorldPosition> WorldPosition::getPathStepFrom(WorldPosition startPos, Unit* bot)
{
    if (!bot)
        return {};

    // Load mmaps and vmaps between the two points.
    loadMapAndVMaps(startPos);

    PathGenerator path(bot);
    path.CalculatePath(startPos.getX(), startPos.getY(), startPos.getZ());

    Movement::PointsArray points = path.GetPath();
    PathType type = path.GetPathType();

    if (sPlayerbotAIConfig->hasLog("pathfind_attempt_point.csv"))
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(1);
        printWKT({ startPos, *this }, out);
        sPlayerbotAIConfig->log("pathfind_attempt_point.csv", out.str().c_str());
    }

    if (sPlayerbotAIConfig->hasLog("pathfind_attempt.csv") && (type == PATHFIND_INCOMPLETE || type == PATHFIND_NORMAL))
    {
        std::ostringstream out;
        out << sPlayerbotAIConfig->GetTimestampStr() << "+00,";
        out << std::fixed << std::setprecision(1) << type << ",";
        printWKT(fromPointsArray(points), out, 1);
        sPlayerbotAIConfig->log("pathfind_attempt.csv", out.str().c_str());
    }

    if (type == PATHFIND_INCOMPLETE || type == PATHFIND_NORMAL)
        return fromPointsArray(points);

    return {};
}

bool WorldPosition::cropPathTo(std::vector<WorldPosition>& path, float maxDistance)
{
    if (path.empty())
        return false;

    auto bestPos = std::min_element(path.begin(), path.end(),
        [this](WorldPosition i, WorldPosition j)
        { return this->sqDistance(i) < this->sqDistance(j); });

    bool insRange = this->sqDistance(*bestPos) <= maxDistance * maxDistance;

    if (bestPos == path.end())
        return insRange;

    path.erase(std::next(bestPos), path.end());

    return insRange;
}

// A sequential series of pathfinding attempts. Returns the complete path and if the patfinder eventually found a way to
// the destination.
std::vector<WorldPosition> WorldPosition::getPathFromPath(std::vector<WorldPosition> startPath, Unit* bot,
    uint8 maxAttempt)
{
    // We start at the end of the last path.
    WorldPosition currentPos = startPath.back();

    // No pathfinding across maps.
    if (getMapId() != currentPos.getMapId())
        return {};

    std::vector<WorldPosition> subPath, fullPath = startPath;

    // Limit the pathfinding attempts
    for (uint32 i = 0; i < maxAttempt; i++)
    {
        // Try to pathfind to this position.
        subPath = getPathStepFrom(currentPos, bot);

        // If we could not find a path return what we have now.
        if (subPath.empty() || currentPos.distance(&subPath.back()) < sPlayerbotAIConfig->targetPosRecalcDistance)
            break;

        // Append the path excluding the start (this should be the same as the end of the startPath)
        fullPath.insert(fullPath.end(), std::next(subPath.begin(), 1), subPath.end());

        // Are we there yet?
        if (isPathTo(subPath))
            break;

        // Continue pathfinding.
        currentPos = subPath.back();
    }

    return fullPath;
}

bool WorldPosition::GetReachableRandomPointOnGround(Player* bot, float radius, bool randomRange)
{
    radius *= randomRange ? rand_norm() : 1.f;
    float angle = rand_norm() * static_cast<float>(2 * M_PI);
    m_positionX += radius * cosf(angle);
    m_positionY += radius * sinf(angle);

    return true;// getMap()->CanReachPositionAndGetValidCoords(bot, m_positionX, m_positionY, m_positionZ);
}

uint32 WorldPosition::getUnitsAggro(GuidVector& units, Player* bot)
{
    units.erase(std::remove_if(units.begin(), units.end(),
        [this, bot](ObjectGuid guid)
        {
            Creature* creature = ObjectAccessor::GetCreature(*bot, guid);
            if (!creature)
                return true;

            return sqDistance(WorldPosition(creature)) >
                creature->GetAttackDistance(bot) * creature->GetAttackDistance(bot);
        }),
        units.end());

    return units.size();
}

GuidPosition::GuidPosition(WorldObject* wo) : ObjectGuid(wo->GetGUID()), WorldPosition(wo), loadedFromDB(false) {}

GuidPosition::GuidPosition(CreatureData const& creData)
    : ObjectGuid(HighGuid::Unit, creData.id, creData.spawnId),
    WorldPosition(creData.mapId, creData.posX, creData.posY, creData.posZ, creData.orientation)
{
    loadedFromDB = true;
}

GuidPosition::GuidPosition(GameObjectData const& goData)
    : ObjectGuid(HighGuid::GameObject, goData.id),
    WorldPosition(goData.mapid, goData.posX, goData.posY, goData.posZ, goData.orientation)
{
    loadedFromDB = true;
}

CreatureTemplate const* GuidPosition::GetCreatureTemplate()
{
    return IsCreature() ? sObjectMgr->GetCreatureTemplate(GetEntry()) : nullptr;
}

GameObjectTemplate const* GuidPosition::GetGameObjectTemplate()
{
    return IsGameObject() ? sObjectMgr->GetGameObjectTemplate(GetEntry()) : nullptr;
}

WorldObject* GuidPosition::GetWorldObject()
{
    if (!*this)
        return nullptr;

    switch (GetHigh())
    {
    case HighGuid::Player:
        return ObjectAccessor::FindPlayer(*this);
    case HighGuid::Transport:
    case HighGuid::Mo_Transport:
    case HighGuid::GameObject:
        return GetGameObject();
    case HighGuid::Vehicle:
    case HighGuid::Unit:
        return GetCreature();
    case HighGuid::Pet:
        return getMap()->GetPet(*this);
    case HighGuid::DynamicObject:
        return getMap()->GetDynamicObject(*this);
    case HighGuid::Corpse:
        return getMap()->GetCorpse(*this);
    default:
        return nullptr;
    }

    return nullptr;
}

bool GuidPosition::HasNpcFlag(NPCFlags flag)
{
    return IsCreature() && GetCreatureTemplate()->npcflag & flag;
}

Creature* GuidPosition::GetCreature()
{
    if (!*this)
        return nullptr;

    if (loadedFromDB)
    {
        auto creatureBounds = getMap()->GetCreatureBySpawnIdStore().equal_range(GetCounter());
        if (creatureBounds.first != creatureBounds.second)
            return creatureBounds.second->second;

        return nullptr;
    }

    return getMap()->GetCreature(*this);
}

Unit* GuidPosition::GetUnit()
{
    if (!*this)
        return nullptr;

    if (loadedFromDB)
    {
        auto creatureBounds = getMap()->GetCreatureBySpawnIdStore().equal_range(GetCounter());
        if (creatureBounds.first != creatureBounds.second)
            return creatureBounds.second->second;

        return nullptr;
    }

    if (IsPlayer())
        return ObjectAccessor::FindPlayer(*this);

    if (IsPet())
        return getMap()->GetPet(*this);

    return GetCreature();
}

GameObject* GuidPosition::GetGameObject()
{
    if (!*this)
        return nullptr;

    if (loadedFromDB)
    {
        auto gameobjectBounds = getMap()->GetGameObjectBySpawnIdStore().equal_range(GetCounter());
        if (gameobjectBounds.first != gameobjectBounds.second)
            return gameobjectBounds.second->second;

        return nullptr;
    }

    return getMap()->GetGameObject(*this);
}

Player* GuidPosition::GetPlayer()
{
    if (!*this)
        return nullptr;

    if (IsPlayer())
        return ObjectAccessor::FindPlayer(*this);

    return nullptr;
}

bool GuidPosition::isDead()
{
    if (!getMap())
        return false;

    if (!getMap()->IsGridLoaded(getX(), getY()))
        return false;

    if (IsUnit() && GetUnit() && GetUnit()->IsInWorld() && GetUnit()->IsAlive())
        return false;

    if (IsGameObject() && GetGameObject() && GetGameObject()->IsInWorld())
        return false;

    return true;
}