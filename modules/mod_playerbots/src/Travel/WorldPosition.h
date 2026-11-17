/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_WORLD_POSITION_H
#define _PLAYERBOT_WORLD_POSITION_H

#include "AiObject.h"
#include "CreatureData.h"
#include "GameObject.h"
#include "GridDefines.h"
#include "PlayerbotAIConfig.h"

#include <boost/functional/hash.hpp>
#include <random>

class GuidPosition;
class ObjectGuid;
class Quest;
class Player;
class PlayerbotAI;
class CreatureData;
class WorldObject;

struct GameObjectData;
struct CreatureTemplate;
struct GameObjectTemplate;
struct QuestStatusData;

namespace G3D
{
    class Vector2;
    class Vector3;
    class Vector4;
}  // namespace G3D

// Constructor types for WorldPosition
enum WorldPositionConst
{
    WP_RANDOM = 0,
    WP_CENTROID = 1,
    WP_MEAN_CENTROID = 2,
    WP_CLOSEST = 3
};

enum TravelState
{
    TRAVEL_STATE_IDLE = 0,
    TRAVEL_STATE_TRAVEL_PICK_UP_QUEST = 1,
    TRAVEL_STATE_WORK_PICK_UP_QUEST = 2,
    TRAVEL_STATE_TRAVEL_DO_QUEST = 3,
    TRAVEL_STATE_WORK_DO_QUEST = 4,
    TRAVEL_STATE_TRAVEL_HAND_IN_QUEST = 5,
    TRAVEL_STATE_WORK_HAND_IN_QUEST = 6,
    TRAVEL_STATE_TRAVEL_RPG = 7,
    TRAVEL_STATE_TRAVEL_EXPLORE = 8,
    MAX_TRAVEL_STATE
};

enum TravelStatus
{
    TRAVEL_STATUS_NONE = 0,
    TRAVEL_STATUS_PREPARE = 1,
    TRAVEL_STATUS_TRAVEL = 2,
    TRAVEL_STATUS_WORK = 3,
    TRAVEL_STATUS_COOLDOWN = 4,
    TRAVEL_STATUS_EXPIRED = 5,
    MAX_TRAVEL_STATUS
};

class QuestTravelDestination;

// A quest destination container for quick lookup of all destinations related to a quest.
struct QuestContainer
{
    std::vector<QuestTravelDestination*> questGivers;
    std::vector<QuestTravelDestination*> questTakers;
    std::vector<QuestTravelDestination*> questObjectives;
};

typedef std::pair<int32, int32> mGridCoord;

// Extension of WorldLocation with distance functions.
class WorldPosition : public WorldLocation
{
public:
    // Constructors
    WorldPosition() : WorldLocation() {};
    WorldPosition(WorldLocation const& loc) : WorldLocation(loc) {}
    WorldPosition(WorldPosition const& pos) : WorldLocation(pos), visitors(pos.visitors) {}
    WorldPosition(std::string const str);
    WorldPosition(uint32 mapid, float x, float y, float z = 0.f, float orientation = 0.f)
        : WorldLocation(mapid, x, y, z, orientation)
    {
    }
    WorldPosition(uint32 mapId, const Position& pos);
    WorldPosition(WorldObject const* wo);
    WorldPosition(std::vector<WorldPosition*> list, WorldPositionConst conType);
    WorldPosition(std::vector<WorldPosition> list, WorldPositionConst conType);
    WorldPosition(uint32 mapid, GridCoord grid);
    WorldPosition(uint32 mapid, CellCoord cell);
    WorldPosition(uint32 mapid, mGridCoord grid);

    //Setters
    void set(const WorldLocation& pos);
    void set(const WorldObject* wo);
    void set(const WorldPosition& pos);
    void setMapId(uint32 id);
    void setX(float x);
    void setY(float y);
    void setZ(float z);
    void setO(float o);

    void addVisitor() { ++visitors; }

    void remVisitor() { --visitors; }

    // Getters
    operator bool() const;
    friend bool operator==(WorldPosition const& p1, const WorldPosition& p2);
    friend bool operator!=(WorldPosition const& p1, const WorldPosition& p2);

    WorldPosition& operator=(WorldPosition const&) = default;
    WorldPosition& operator+=(WorldPosition const& p1);
    WorldPosition& operator-=(WorldPosition const& p1);

    uint32 getMapId();
    float getX();
    float getY();
    float getZ();
    float getO();

    G3D::Vector3 getVector3();
    std::string const print();

    std::string const to_string();

    void printWKT(std::vector<WorldPosition> points, std::ostringstream& out, uint32 dim = 0, bool loop = false);
    void printWKT(std::ostringstream& out) { printWKT({ *this }, out); }

    uint32 getVisitors() { return visitors; }

    bool isOverworld();
    bool isInWater();
    bool isUnderWater();

    WorldPosition relPoint(WorldPosition* center);
    WorldPosition offset(WorldPosition* center);
    float size();

    // Slow distance function using possible map transfers.
    float distance(WorldPosition* center);
    float distance(WorldPosition center) { return distance(&center); }

    float fDist(WorldPosition* center);
    float fDist(WorldPosition center) { return fDist(&center); }

    template <class T>
    std::pair<T, WorldPosition> closest(std::vector<std::pair<T, WorldPosition>> list)
    {
        return *std::min_element(list.begin(), list.end(),
            [this](std::pair<T, WorldPosition> i, std::pair<T, WorldPosition> j)
            { return this->distance(i.second) < this->distance(j.second); });
    }

    template <class T>
    std::pair<T, WorldPosition> closest(std::vector<T> list)
    {
        return closest(GetPosList(list));
    }

    // Returns the closest point from the list.
    WorldPosition* closest(std::vector<WorldPosition*> list)
    {
        return *std::min_element(list.begin(), list.end(),
            [this](WorldPosition* i, WorldPosition* j)
            { return this->distance(i) < this->distance(j); });
    }

    WorldPosition closest(std::vector<WorldPosition> list)
    {
        return *std::min_element(list.begin(), list.end(),
            [this](WorldPosition i, WorldPosition j)
            { return this->distance(i) < this->distance(j); });
    }

    // Quick square distance in 2d plane.
    float sqDistance2d(WorldPosition center)
    {
        return (getX() - center.getX()) * (getX() - center.getX()) +
            (getY() - center.getY()) * (getY() - center.getY());
    }

    // Quick square distance calculation without map check. Used for getting the minimum distant points.
    float sqDistance(WorldPosition center)
    {
        return (getX() - center.getX()) * (getX() - center.getX()) +
            (getY() - center.getY()) * (getY() - center.getY()) +
            (getZ() - center.getZ()) * (getZ() - center.getZ());
    }

    float sqDistance2d(WorldPosition* center)
    {
        return (getX() - center->getX()) * (getX() - center->getX()) +
            (getY() - center->getY()) * (getY() - center->getY());
    }

    float sqDistance(WorldPosition* center)
    {
        return (getX() - center->getX()) * (getX() - center->getX()) +
            (getY() - center->getY()) * (getY() - center->getY()) +
            (getZ() - center->getZ()) * (getZ() - center->getZ());
    }

    // Returns the closest point of the list. Fast but only works for the same map.
    WorldPosition* closestSq(std::vector<WorldPosition*> list)
    {
        return *std::min_element(list.begin(), list.end(),
            [this](WorldPosition* i, WorldPosition* j)
            { return this->sqDistance(i) < this->sqDistance(j); });
    }

    WorldPosition closestSq(std::vector<WorldPosition> list)
    {
        return *std::min_element(list.begin(), list.end(),
            [this](WorldPosition i, WorldPosition j)
            { return this->sqDistance(i) < this->sqDistance(j); });
    }

    float getAngleTo(WorldPosition endPos)
    {
        float ang = atan2(endPos.getY() - getY(), endPos.getX() - getX());
        return (ang >= 0) ? ang : 2 * static_cast<float>(M_PI) + ang;
    }

    float getAngleBetween(WorldPosition dir1, WorldPosition dir2) { return abs(getAngleTo(dir1) - getAngleTo(dir2)); }

    WorldPosition lastInRange(std::vector<WorldPosition> list, float minDist = -1.f, float maxDist = -1.f);
    WorldPosition firstOutRange(std::vector<WorldPosition> list, float minDist = -1.f, float maxDist = -1.f);

    float mSign(WorldPosition* p1, WorldPosition* p2)
    {
        return (getX() - p2->getX()) * (p1->getY() - p2->getY()) - (p1->getX() - p2->getX()) * (getY() - p2->getY());
    }

    bool isInside(WorldPosition* p1, WorldPosition* p2, WorldPosition* p3);

    // Map functions. Player independent.
    MapEntry const* getMapEntry();
    uint32 getInstanceId();
    Map* getMap();
    float getHeight();  // remove const - whipowill

    std::set<Transport*> getTransports(uint32 entry = 0);

    GridCoord getGridCoord() { return Trinity::ComputeGridCoord(getX(), getY()); };
    std::vector<GridCoord> getGridCoord(WorldPosition secondPos);
    std::vector<WorldPosition> fromGridCoord(GridCoord GridCoord);

    CellCoord getCellCoord() { return Trinity::ComputeCellCoord(getX(), getY()); }
    std::vector<WorldPosition> fromCellCoord(CellCoord cellCoord);
    std::vector<WorldPosition> gridFromCellCoord(CellCoord cellCoord);

    mGridCoord getmGridCoord()
    {
        return std::make_pair((int32)(CENTER_GRID_ID - getX() / SIZE_OF_GRIDS),
            (int32)(CENTER_GRID_ID - getY() / SIZE_OF_GRIDS));
    }

    std::vector<mGridCoord> getmGridCoords(WorldPosition secondPos);
    std::vector<WorldPosition> frommGridCoord(mGridCoord GridCoord);

    void loadMapAndVMap(uint32 mapId, uint8 x, uint8 y);

    void loadMapAndVMap() { loadMapAndVMap(getMapId(), getmGridCoord().first, getmGridCoord().second); }

    void loadMapAndVMaps(WorldPosition secondPos);

    // Display functions
    WorldPosition getDisplayLocation();
    float getDisplayX() { return getDisplayLocation().getY() * -1.0; }

    float getDisplayY() { return getDisplayLocation().getX(); }

    uint16 getAreaId();
    AreaTableEntry const* getArea();
    std::string const getAreaName(bool fullName = true, bool zoneName = false);

    std::vector<WorldPosition> fromPointsArray(std::vector<G3D::Vector3> path);

    // Pathfinding
    std::vector<WorldPosition> getPathStepFrom(WorldPosition startPos, Unit* bot);
    std::vector<WorldPosition> getPathFromPath(std::vector<WorldPosition> startPath, Unit* bot, uint8 maxAttempt = 40);

    std::vector<WorldPosition> getPathFrom(WorldPosition startPos, Unit* bot)
    {
        return getPathFromPath({ startPos }, bot);
    }

    std::vector<WorldPosition> getPathTo(WorldPosition endPos, Unit* bot) { return endPos.getPathFrom(*this, bot); }

    bool isPathTo(std::vector<WorldPosition> path, float maxDistance = sPlayerbotAIConfig->targetPosRecalcDistance)
    {
        return !path.empty() && distance(path.back()) < maxDistance;
    };
    bool cropPathTo(std::vector<WorldPosition>& path, float maxDistance = sPlayerbotAIConfig->targetPosRecalcDistance);
    bool canPathTo(WorldPosition endPos, Unit* bot) { return endPos.isPathTo(getPathTo(endPos, bot)); }

    float getPathLength(std::vector<WorldPosition> points)
    {
        float dist = 0.f;
        for (auto& p : points)
        {
            if (&p == &points.front())
                dist = 0.f;
            else
                dist += std::prev(&p, 1)->distance(p);
        }

        return dist;
    }

    bool GetReachableRandomPointOnGround(Player* bot, float radius, bool randomRange = true);

    uint32 getUnitsAggro(GuidVector& units, Player* bot);
private:
    uint32 visitors = 0;
};

inline ByteBuffer& operator<<(ByteBuffer& b, WorldPosition& guidP)
{
    b << guidP.getMapId();
    b << guidP.getX();
    b << guidP.getY();
    b << guidP.getZ();
    b << guidP.getO();
    b << guidP.getVisitors();
    return b;
}

inline ByteBuffer& operator>>(ByteBuffer& b, [[maybe_unused]] WorldPosition& g)
{
    uint32 mapid;
    float coord_x;
    float coord_y;
    float coord_z;
    float orientation;
    uint32 visitors = 0;
    b >> mapid;
    b >> coord_x;
    b >> coord_y;
    b >> coord_z;
    b >> orientation;
    b >> visitors;

    return b;
}

class GuidPosition : public ObjectGuid, public WorldPosition
{
public:
    GuidPosition() : ObjectGuid(), WorldPosition(), loadedFromDB(false) { }
    GuidPosition(WorldObject* wo);
    GuidPosition(CreatureData const& creData);
    GuidPosition(GameObjectData const& goData);
    CreatureTemplate const* GetCreatureTemplate();
    GameObjectTemplate const* GetGameObjectTemplate();

    WorldObject* GetWorldObject();
    Creature* GetCreature();
    Unit* GetUnit();
    GameObject* GetGameObject();
    Player* GetPlayer();

    bool HasNpcFlag(NPCFlags flag);

    bool isDead();  // For loaded grids check if the unit/object is unloaded/dead.

    operator bool() const { return !IsEmpty(); }
    bool operator==(ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
    bool operator!=(ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
    bool operator<(ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

private:
    bool loadedFromDB;
};

#endif /*_PLAYERBOT_WORLD_POSITION_H*/