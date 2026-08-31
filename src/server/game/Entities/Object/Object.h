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

#ifndef SF_OBJECT_H
#define SF_OBJECT_H

#include "Common.h"
#include "Map.h"
#include "ModelIgnoreFlags.h"
#include "MovementInfo.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Position.h"
#include "UpdateMask.h"
#include <G3D/Quat.h>

#include <set>
#include <string>
#include <sstream>
#include <array>

// Reasons for why object was flagged as active
enum class ActiveFlags : uint32
{
    InCombat            = 0x00000001, // Unit is active due to being in combat and needing to evade back to home position (Default reason for backwards compatibility)
    Script              = 0x00000002, // General purpose script reason
    ScriptPassive       = 0x00000004, // General purpose script reason (skipped if no players are in the map, use this primarily in instances) (not used currently)
    IsWorldObject       = 0x00000008, // Object is active due to being a world object
    SharedVision        = 0x00000010, // Unit is active because a player is sharing its vision (i.e. Mind Vision spell)
    SAI                 = 0x00000020, // Object was flagged as active from a SmartAI script
    OnTransport         = 0x00000040, // Object is active due to being on a MapObject transport
    AuraGenerator       = 0x00000080, // Object is active because it emits an area aura which needs to eventually be removed when targets are out of range (not used currently)
    CustomVisibility    = 0x00000100, // Object was flagged as active in `object_visibility` table
    SplineWaypoints     = 0x00000200, // Object is active due to using a WaypointMovementGenerator with spline path which needs to be continuously updated to match what clients see (not used currently)

    // If object has ONLY any of these flags set - it will not trigger grid updates when there are no players in the map
    OnlyInNonEmptyMapsMask = ScriptPassive | CustomVisibility | SplineWaypoints,

    None = 0,
};

class Corpse;
class Creature;
class CreatureAI;
class DynamicObject;
class GameObject;
class InstanceScript;
class Player;
class TempSummon;
class Transport;
class Unit;
class UpdateData;
class WorldObject;
class WorldPacket;
class ZoneScript;
struct FactionTemplateEntry;
struct PositionFullTerrainStatus;
struct QuaternionData;
enum ZLiquidStatus : uint32;

typedef std::unordered_map<Player*, UpdateData> UpdateDataMapType;

float const DEFAULT_COLLISION_HEIGHT = 2.03128f; // Most common value in dbc

class TC_GAME_API Object
{
    public:
        virtual ~Object();

        bool IsInWorld() const { return m_inWorld; }

        virtual void AddToWorld();
        virtual void RemoveFromWorld();

        ObjectGuid GetGUID() const { return GetGuidValue(OBJECT_FIELD_GUID); }
        PackedGuid const& GetPackGUID() const { return m_PackGUID; }
        uint32 GetEntry() const { return GetUInt32Value(OBJECT_FIELD_ENTRY_ID); }
        void SetEntry(uint32 entry) { SetUInt32Value(OBJECT_FIELD_ENTRY_ID, entry); }

        float GetObjectScale() const { return GetFloatValue(OBJECT_FIELD_SCALE); }
        virtual void SetObjectScale(float scale) { SetFloatValue(OBJECT_FIELD_SCALE, scale); }

        TypeID GetTypeId() const { return m_objectTypeId; }
        bool isType(uint16 mask) const { return (mask & m_objectType); }

        virtual void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const;
        void SendUpdateToPlayer(Player* player);

        void BuildValuesUpdateBlockForPlayer(UpdateData* data, Player* target) const;
        void BuildOutOfRangeUpdateBlock(UpdateData* data) const;

        virtual void DestroyForPlayer(Player* target, bool onDeath = false) const;
        void SendOutOfRangeForPlayer(Player* target) const;

        int32 GetInt32Value(uint16 index) const;
        uint32 GetUInt32Value(uint16 index) const;
        uint64 GetUInt64Value(uint16 index) const;
        float GetFloatValue(uint16 index) const;
        uint8 GetByteValue(uint16 index, uint8 offset) const;
        uint16 GetUInt16Value(uint16 index, uint8 offset) const;
        uint32 GetDynamicUInt32Value(uint32 tab, uint16 index) const;
        ObjectGuid GetGuidValue(uint16 index) const;

        void SetInt32Value(uint16 index, int32 value);
        void SetUInt32Value(uint16 index, uint32 value);
        void UpdateUInt32Value(uint16 index, uint32 value);
        void SetUInt64Value(uint16 index, uint64 value);
        void SetFloatValue(uint16 index, float value);
        void SetByteValue(uint16 index, uint8 offset, uint8 value);
        void SetUInt16Value(uint16 index, uint8 offset, uint16 value);
        void SetInt16Value(uint16 index, uint8 offset, int16 value) { SetUInt16Value(index, offset, (uint16)value); }
        void SetGuidValue(uint16 index, ObjectGuid value);
        void SetStatFloatValue(uint16 index, float value);
        void SetStatInt32Value(uint16 index, int32 value);
        void SetDynamicUInt32Value(uint32 tab, uint16 index, uint32 value);
        void RemoveDynamicValue(uint32 tab, uint16 index);

        bool AddGuidValue(uint16 index, ObjectGuid value);
        bool RemoveGuidValue(uint16 index, ObjectGuid value);

        void ApplyModUInt32Value(uint16 index, int32 val, bool apply);
        void ApplyModInt32Value(uint16 index, int32 val, bool apply);
        void ApplyModUInt64Value(uint16 index, int32 val, bool apply);
        void ApplyModPositiveFloatValue(uint16 index, float val, bool apply);
        void ApplyModSignedFloatValue(uint16 index, float val, bool apply);
        void ApplyPercentModFloatValue(uint16 index, float val, bool apply);

        void SetFlag(uint16 index, uint32 newFlag);
        void RemoveFlag(uint16 index, uint32 oldFlag);
        void ToggleFlag(uint16 index, uint32 flag);
        bool HasFlag(uint16 index, uint32 flag) const;
        void ApplyModFlag(uint16 index, uint32 flag, bool apply);

        void SetByteFlag(uint16 index, uint8 offset, uint8 newFlag);
        void RemoveByteFlag(uint16 index, uint8 offset, uint8 newFlag);
        void ToggleByteFlag(uint16 index, uint8 offset, uint8 flag);
        bool HasByteFlag(uint16 index, uint8 offset, uint8 flag) const;

        void SetFlag64(uint16 index, uint64 newFlag);
        void RemoveFlag64(uint16 index, uint64 oldFlag);
        void ToggleFlag64(uint16 index, uint64 flag);
        bool HasFlag64(uint16 index, uint64 flag) const;
        void ApplyModFlag64(uint16 index, uint64 flag, bool apply);

        void ClearUpdateMask(bool remove);

        uint16 GetValuesCount() const { return m_valuesCount; }

        virtual bool hasQuest(uint32 /* quest_id */) const { return false; }
        virtual bool hasInvolvedQuest(uint32 /* quest_id */) const { return false; }
        virtual void BuildUpdate(UpdateDataMapType&) { }
        void BuildFieldsUpdate(Player*, UpdateDataMapType &) const;

        void SetFieldNotifyFlag(uint16 flag) { _fieldNotifyFlags |= flag; }
        void RemoveFieldNotifyFlag(uint16 flag) { _fieldNotifyFlags &= ~flag; }

        // FG: some hacky helpers
        void ForceValuesUpdateAtIndex(uint32);
        void ForceDynamicValuesUpdateTabAtIndex(uint32, uint16);

        bool IsDestroyedObject() const { return m_isDestroyedObject; }
        void SetDestroyedObject(bool destroyed) { m_isDestroyedObject = destroyed; }

        inline bool IsWorldObject() const { return isType(TYPEMASK_WORLDOBJECT); }
        static WorldObject* ToWorldObject(Object* o) { return o ? o->ToWorldObject() : nullptr; }
        static WorldObject const* ToWorldObject(Object const* o) { return o ? o->ToWorldObject() : nullptr; }
        WorldObject* ToWorldObject() { if (IsWorldObject()) return reinterpret_cast<WorldObject*>(this); else return nullptr; }
        WorldObject const* ToWorldObject() const { if (IsWorldObject()) return reinterpret_cast<WorldObject const*>(this); else return nullptr; }

        inline bool IsPlayer() const { return GetTypeId() == TYPEID_PLAYER; }
        static Player* ToPlayer(Object* o) { return o ? o->ToPlayer() : nullptr; }
        static Player const* ToPlayer(Object const* o) { return o ? o->ToPlayer() : nullptr; }        
        Player* ToPlayer() { if (IsPlayer()) return reinterpret_cast<Player*>(this); else return nullptr; }
        Player const* ToPlayer() const { if (IsPlayer()) return reinterpret_cast<Player const*>(this); else return nullptr;  }

        inline bool IsCreature() const { return GetTypeId() == TYPEID_UNIT; }
        Creature* ToCreature() { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature*>(this); else return nullptr; }
        Creature const* ToCreature() const { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature const*>(this); else return nullptr; }

        inline bool IsUnit() const { return isType(TYPEMASK_UNIT); }
        static Unit* ToUnit(Object* o) { return o ? o->ToUnit() : nullptr; }
        static Unit const* ToUnit(Object const* o) { return o ? o->ToUnit() : nullptr; }
        Unit* ToUnit() { if (isType(TYPEMASK_UNIT)) return reinterpret_cast<Unit*>(this); else return nullptr; }
        Unit const* ToUnit() const { if (isType(TYPEMASK_UNIT)) return reinterpret_cast<Unit const*>(this); else return nullptr; }

        inline bool IsGameObject() const { return GetTypeId() == TYPEID_GAMEOBJECT; }
        GameObject* ToGameObject() { if (GetTypeId() == TYPEID_GAMEOBJECT) return reinterpret_cast<GameObject*>(this); else return nullptr; }
        GameObject const* ToGameObject() const { if (GetTypeId() == TYPEID_GAMEOBJECT) return reinterpret_cast<GameObject const*>(this); else return nullptr; }

        inline bool IsCorpse() const { return GetTypeId() == TYPEID_CORPSE; }
        Corpse* ToCorpse() { if (GetTypeId() == TYPEID_CORPSE) return reinterpret_cast<Corpse*>(this); else return nullptr; }
        Corpse const* ToCorpse() const { if (GetTypeId() == TYPEID_CORPSE) return reinterpret_cast<Corpse const*>(this); else return nullptr; }

        inline bool IsDynObject() const { return GetTypeId() == TYPEID_DYNAMICOBJECT; }
        DynamicObject* ToDynObject() { if (GetTypeId() == TYPEID_DYNAMICOBJECT) return reinterpret_cast<DynamicObject*>(this); else return nullptr; }
        DynamicObject const* ToDynObject() const { if (GetTypeId() == TYPEID_DYNAMICOBJECT) return reinterpret_cast<DynamicObject const*>(this); else return nullptr; }

        inline bool IsAreaTrigger() const { return GetTypeId() == TYPEID_AREATRIGGER; }
        AreaTrigger* ToAreaTrigger() { if (GetTypeId() == TYPEID_AREATRIGGER) return reinterpret_cast<AreaTrigger*>(this); else return nullptr; }
        AreaTrigger const* ToAreaTrigger() const { if (GetTypeId() == TYPEID_AREATRIGGER) return reinterpret_cast<AreaTrigger const*>(this); else return nullptr; }

    protected:
        Object();

        void _InitValues();
        void _Create(ObjectGuid::LowType guidlow, uint32 entry, HighGuid guidhigh);
        std::string _ConcatFields(uint16 startIndex, uint16 size) const;
        void _LoadIntoDataField(std::string const& data, uint32 startOffset, uint32 count);

        uint32 GetUpdateFieldData(Player const* target, uint32*& flags) const;

        void BuildMovementUpdate(ByteBuffer* data, uint16 flags) const;
        void BuildDynamicValuesUpdate(uint8 updatetype, ByteBuffer *data) const;
        virtual void BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, Player* target) const;
        virtual void AddToUpdate() = 0;
        virtual void RemoveFromUpdate() = 0;

        class DynamicField
        {
        public:
            DynamicField() { Clear(); }

            static size_t const MaxSize = 32;

            void Set(uint16 field, uint32 val)
            {
                ASSERT(field < MaxSize);
                _values[field] = val;
                _mask |= 1 << field;
            }

            void Remove(uint16 field)
            {
                _values[field] = 0;
                _mask &= ~(1 << field);
            }

            bool IsSet(uint16 field) const
            {
                return _mask & (1 << field);
            }

            uint32 operator[](uint16 field) const
            {
                return _values[field];
            }

            void Clear()
            {
                std::fill(_values.begin(), _values.end(), 0);
            }

        private:
            std::array<uint32, MaxSize> _values;
            uint32 _mask = 0;
        };

        std::vector<DynamicField> m_dynamicValues;
        std::vector<bool> m_dynamicChange;
        size_t m_dynamicValuesCount = 0;

        uint16 m_objectType;

        TypeID m_objectTypeId;
        uint16 m_updateFlag;

        union
        {
            int32  *m_int32Values;
            uint32 *m_uint32Values;
            float  *m_floatValues;
        };

        UpdateMask _changesMask;

        uint16 m_valuesCount;

        uint16 _fieldNotifyFlags;

        bool m_objectUpdated;

    private:
        bool m_inWorld;
        bool m_isDestroyedObject;

        PackedGuid m_PackGUID;

        // for output helpfull error messages from asserts
        bool PrintIndexError(uint32 index, bool set) const;
        Object(Object const& right) = delete;
        Object(Object&& right) = delete;
        Object& operator=(Object const& right) = delete;
        Object& operator=(Object&& right) = delete;

};

template <class T_VALUES, class T_FLAGS, class FLAG_TYPE, size_t ARRAY_SIZE>
class FlaggedValuesArray32
{
    public:
        FlaggedValuesArray32()
        {
            memset(&m_values, 0x00, sizeof(T_VALUES) * ARRAY_SIZE);
            m_flags = 0;
        }

        T_FLAGS  GetFlags() const { return m_flags; }
        bool     HasFlag(FLAG_TYPE flag) const { return m_flags & (1 << flag); }
        void     AddFlag(FLAG_TYPE flag) { m_flags |= (1 << flag); }
        void     DelFlag(FLAG_TYPE flag) { m_flags &= ~(1 << flag); }

        T_VALUES GetValue(FLAG_TYPE flag) const { return m_values[flag]; }
        void     SetValue(FLAG_TYPE flag, T_VALUES value) { m_values[flag] = value; }
        void     AddValue(FLAG_TYPE flag, T_VALUES value) { m_values[flag] += value; }

    private:
        T_VALUES m_values[ARRAY_SIZE];
        T_FLAGS m_flags;
};

enum MapObjectCellMoveState
{
    MAP_OBJECT_CELL_MOVE_NONE, //not in move list
    MAP_OBJECT_CELL_MOVE_ACTIVE, //in move list
    MAP_OBJECT_CELL_MOVE_INACTIVE, //in move list but should not move
};

class MapObject
{
        friend class Map; //map for moving creatures
        friend class ObjectGridLoader; //grid loader for loading creatures

    protected:
        MapObject() : _moveState(MAP_OBJECT_CELL_MOVE_NONE)
        {
            _newPosition.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
        }

    private:
        Cell _currentCell;
        Cell const& GetCurrentCell() const { return _currentCell; }
        void SetCurrentCell(Cell const& cell) { _currentCell = cell; }

        MapObjectCellMoveState _moveState;
        Position _newPosition;
        void SetNewCellPosition(float x, float y, float z, float o)
        {
            _moveState = MAP_OBJECT_CELL_MOVE_ACTIVE;
            _newPosition.Relocate(x, y, z, o);
        }
};

namespace CustomVisibility
{
    enum class Type : uint8
    {
        Creature = 1,
        GameObject,
        DynamicObject,
    };
    enum class Importance : uint8
    {
        Critical = 1,
        Large,
        DistantScenery,
        FlyingScenery,
        GroundScenery,
        FlyingClutter,
        GroundClutter,
    };
    struct Info
    {
        CustomVisibility::Type Type;
        uint32 Entry;
        float Distance;
        bool Active;
        CustomVisibility::Importance Importance;
        std::string Comment;
    };
}

class TC_GAME_API WorldObject : public Object, public WorldLocation
{
    protected:
        explicit WorldObject(bool isWorldObject); //note: here it means if it is in grid object list or world object list
    public:
        virtual ~WorldObject();

        virtual void Update(uint32 /*time_diff*/) { }

        void _Create(uint32 guidlow, HighGuid guidhigh, uint32 phaseMask, std::set<uint32> const& phaseIds);
        void AddToWorld() override;
        void RemoveFromWorld() override;

        void GetNearPoint2D(float &x, float &y, float distance, float absAngle) const;
        void GetNearPoint(WorldObject const* searcher, float &x, float &y, float &z, float searcher_size, float distance2d, float absAngle) const;
        void GetClosePoint(float &x, float &y, float &z, float size, float distance2d = 0, float angle = 0) const;
        void MovePosition(Position &pos, float dist, float angle);
        Position GetNearPosition(float dist, float angle);
        Position GetPositionAlternate() const
        {
            return *this;
        }
        void MovePositionToFirstCollision(Position &pos, float dist, float angle, float offsetZ = 0.0f);
        inline bool IsOnGround()
        {
            float z = this->GetPositionZ();
            float ground_z = GetMap()->GetHeight(m_phaseMask, m_positionX, m_positionY, MAX_HEIGHT, true);
            return fabs(z - ground_z) < 1.0f;
        }
        Position GetFirstCollisionPosition(float dist, float angle);
        Position GetRandomNearPosition(float radius);
        void GetContactPoint(WorldObject const* obj, float &x, float &y, float &z, float distance2d = CONTACT_DISTANCE) const;

        virtual float GetCombatReach() const { return 0.0f; } // overridden (only) in Unit
        void MovePositionToFirstCollosionBySteps(Position& pos, float dist, float angle, float heightCheckInterval = 2.0f, bool allowInAir = false);

        float GetObjectSize() const;
        void UpdateGroundPositionZ(float x, float y, float &z) const;
        void UpdateAllowedPositionZ(float x, float y, float &z, float* groundZ = nullptr) const;

        void GetRandomPoint(Position const &srcPos, float distance, float &rand_x, float &rand_y, float &rand_z) const;
        Position GetRandomPoint(Position const &srcPos, float distance) const;

        uint32 GetInstanceId() const { return m_InstanceId; }

        virtual void SetPhaseMask(uint32 newPhaseMask, bool update);
        uint32 GetPhaseMask() const { return m_phaseMask; }
        bool InSamePhase(WorldObject const* obj) const;
        bool InSamePhase(uint32 phasemask) const { return (GetPhaseMask() & phasemask); }

        virtual bool SetPhased(uint32 id, bool update, bool apply);
        bool HasPhaseList(uint32 phase);
        void ClearPhases(bool update = false);
        bool IsPhased(WorldObject const* obj) const;
        bool IsPhased(uint32 phase) const { return _phases.find(phase) != _phases.end(); }
        std::set<uint32> const& GetPhases() const { return _phases; }
        bool SetTerrainSwap(uint32 id, bool update, bool apply);
        bool IsTerrainSwaped(uint32 terrainSwap) const { return _terrainSwaps.find(terrainSwap) != _terrainSwaps.end(); }
        std::set<uint32> const& GetTerrainSwaps() const { return _terrainSwaps; }
        void ClearTerrainSwaps(bool update = false);
        bool SetWorldMapSwap(uint32 id, bool update, bool apply);
        bool IsWorldMapSwaped(uint32 worldMapSwap) const { return _worldMapSwaps.find(worldMapSwap) != _worldMapSwaps.end(); }
        std::set<uint32> const& GetWorldMapSwaps() const { return _worldMapSwaps; }
        void RebuildTerrainSwaps();
        void RebuildWorldMapAreaSwaps();
        void ClearWorldMapSwap(bool update = false);
        void UpdateAreaAndZonePhase();

        uint32 GetZoneId() const;
        uint32 GetAreaId() const;
        void GetZoneAndAreaId(uint32& zoneid, uint32& areaid) const;

        bool IsOutdoors() const { return m_outdoors; }
        ZLiquidStatus GetLiquidStatus() const { return m_liquidStatus; }

        InstanceScript* GetInstanceScript();

        std::string const& GetName() const { return m_name; }
        void SetName(std::string const& newname) { m_name=newname; }

        virtual std::string const& GetNameForLocaleIdx(LocaleConstant /*locale_idx*/) const { return m_name; }

        float GetDistance(WorldObject const* obj) const;
        float GetDistance(Position const &pos) const;
        float GetDistance(float x, float y, float z) const;
        float GetDistance2d(WorldObject const* obj) const;
        float GetDistance2d(float x, float y) const;
        float GetDistanceZ(WorldObject const* obj) const;

        bool IsSelfOrInSameMap(WorldObject const* obj) const;
        bool IsInMap(WorldObject const* obj) const;
        bool IsWithinDist3d(float x, float y, float z, float dist) const;
        bool IsWithinDist3d(Position const* pos, float dist) const;
        bool IsWithinDist2d(float x, float y, float dist) const;
        bool IsWithinDist2d(Position const* pos, float dist) const;
        // use only if you will sure about placing both object at same map
        bool IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D = true) const;
        bool IsWithinDistInMap(WorldObject const* obj, float dist2compare, bool is3D = true, bool incOwnRadius = true, bool incTargetRadius = true) const;
        bool IsWithinLOS(float x, float y, float z, VMAP::ModelIgnoreFlags ignoreFlags = VMAP::ModelIgnoreFlags::Nothing) const;
        bool IsWithinLOSInMap(WorldObject const* obj, VMAP::ModelIgnoreFlags ignoreFlags = VMAP::ModelIgnoreFlags::Nothing) const;
        Position GetHitSpherePointFor(Position const& dest) const;
        void GetHitSpherePointFor(Position const& dest, float& x, float& y, float& z) const;
        bool GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D = true) const;
        bool IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D = true) const;
        bool IsInRange2d(float x, float y, float minRange, float maxRange) const;
        bool IsInRange3d(float x, float y, float z, float minRange, float maxRange) const;
        bool isInFront(WorldObject const* target, float arc = float(M_PI)) const;
        bool isInBack(WorldObject const* target, float arc = float(M_PI)) const;

        bool IsInBetween(Position const* obj1, Position const* obj2, float size = 0) const;
        bool IsInAxe(const WorldObject* obj1, const WorldObject* obj2, float size = 0) const;

        virtual void CleanupsBeforeDelete(bool finalCleanup = true);  // used in destructor or explicitly before mass creature delete to remove cross-references to already deleted units

        virtual void SendMessageToSet(WorldPacket const* data, bool self) const;
        virtual void SendMessageToSetInRange(WorldPacket const* data, float dist, bool self) const;
        virtual void SendMessageToSet(WorldPacket const* data, Player const* skipped_rcvr) const;

        virtual uint8 GetLevelForTarget(WorldObject const* /*target*/) const { return 1; }

        void PlayDistanceSound(uint32 sound_id, Player* target = NULL);
        void PlayDirectSound(uint32 sound_id, Player* target = NULL);
        void PlayDirectMusic(uint32 music_id, Player* target = NULL);

        void SendObjectDeSpawnAnim(ObjectGuid guid);

        virtual void SaveRespawnTime() { }
        void AddObjectToRemoveList();

        float GetGridActivationRange() const;
        float GetVisibilityRange() const;
        float GetSightRange(WorldObject const* target = NULL) const;
        bool CanSeeOrDetect(WorldObject const* obj, bool ignoreStealth = false, bool distanceCheck = false) const;

        void SetExplicitSeerGuid(uint64 guid) { m_explicitSeerGuid = guid; }

        FlaggedValuesArray32<int32, uint32, StealthType, TOTAL_STEALTH_TYPES> m_stealth;
        FlaggedValuesArray32<int32, uint32, StealthType, TOTAL_STEALTH_TYPES> m_stealthDetect;

        FlaggedValuesArray32<int32, uint32, InvisibilityType, TOTAL_INVISIBILITY_TYPES> m_invisibility;
        FlaggedValuesArray32<int32, uint32, InvisibilityType, TOTAL_INVISIBILITY_TYPES> m_invisibilityDetect;

        FlaggedValuesArray32<int32, uint32, ServerSideVisibilityType, TOTAL_SERVERSIDE_VISIBILITY_TYPES> m_serverSideVisibility;
        FlaggedValuesArray32<int32, uint32, ServerSideVisibilityType, TOTAL_SERVERSIDE_VISIBILITY_TYPES> m_serverSideVisibilityDetect;

        virtual void SetMap(Map* map);
        virtual void ResetMap();
        Map* GetMap() const { ASSERT(m_currMap); return m_currMap; }
        Map* FindMap() const { return m_currMap; }
        //used to check all object's GetMap() calls when object is not in world!

        //this function should be removed in nearest time...
        Map const* GetBaseMap() const;

        void SetZoneScript();
        ZoneScript* GetZoneScript() const { return m_zoneScript; }

        TempSummon* SummonCreature(uint32 id, Position const &pos, TempSummonType spwtype = TEMPSUMMON_MANUAL_DESPAWN, uint32 despwtime = 0, uint32 vehId = 0, ObjectGuid privateObjectOwner = ObjectGuid::Empty);
        TempSummon* SummonCreature(uint32 id, float x, float y, float z, float ang = 0, TempSummonType spwtype = TEMPSUMMON_MANUAL_DESPAWN, uint32 despwtime = 0, ObjectGuid privateObjectOwner = ObjectGuid::Empty);
        GameObject* SummonGameObject(uint32 entry, float x, float y, float z, float ang, G3D::Quat const& rotation, uint32 respawnTime, GOSummonType summonType = GO_SUMMON_TIMED_OR_CORPSE_DESPAWN);

        Creature* SummonTrigger(float x, float y, float z, float ang, uint32 dur, CreatureAI* (*GetAI)(Creature*) = NULL);
        void SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list = NULL);

        Creature* FindNearestCreature(uint32 entry, float range, bool alive = true) const;
        std::list<Creature*> FindNearestCreatures(uint32 entry, float range) const;
        std::list<Creature*> FindNearestCreatures(std::list<uint32> entrys, float range) const;
        std::vector<Creature*> FindNearestCreatures(uint32 entry, float range, bool alive) const;
        GameObject* FindNearestGameObject(uint32 entry, float range) const;
        GameObject* FindNearestGameObjectOfType(GameobjectTypes type, float range) const;

        Player* FindNearestPlayer(float range) const;
        std::list<Player*>  GetNearestPlayersList(float range, bool alive = true);


        void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList, uint32 uiEntry, float fMaxSearchRange) const;
        void GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, uint32 uiEntry, float fMaxSearchRange) const;
        void GetPlayerListInGrid(std::list<Player*>& lList, float fMaxSearchRange) const;

        void GetCreatureListWithEntryInGridAppend(std::list<Creature*>& lList, uint32 uiEntry, float fMaxSearchRange) const;

        void GetCreaturesWithEntryInRange(std::list<Creature*>& creatureList, float radius, uint32 entry);

        void DestroyForNearbyPlayers();
        virtual void UpdateObjectVisibility(bool forced = true);
        void UpdateStealthVisibility(uint32 diff);
        void UpdatePositionData();
        
        void BuildUpdate(UpdateDataMapType&) override;

        bool isActiveObject() const { return m_isActive; }
        void setActive(bool on, ActiveFlags flag = ActiveFlags::InCombat);
        ActiveFlags GetActiveFlags() const { return m_activeFlags; }
        bool IsFarVisible() const { return m_isFarVisible; }
        void SetFarVisible(bool on);
        bool IsVisibilityOverridden() const { return m_visibilityDistanceOverride.has_value(); }
        void SetVisibilityDistanceOverride(VisibilityDistanceType type);        
        void SetWorldObject(bool apply);
        bool IsPermanentWorldObject() const { return m_isWorldObject; }
        bool IsWorldObject() const;

        WMOAreaTableEntry const * GetWMOArea() const;
        uint32 GetWMOAreaId() const;

        template<class NOTIFIER> void VisitNearbyObject(float const& radius, NOTIFIER& notifier, bool loadGrids = false, bool ignoreRadiusLimit = false) const { if (IsInWorld()) GetMap()->VisitAll(GetPositionX(), GetPositionY(), radius, notifier, loadGrids, ignoreRadiusLimit); }
        template<class NOTIFIER> void VisitNearbyGridObject(float const& radius, NOTIFIER& notifier, bool loadGrids = false, bool ignoreRadiusLimit = false) const { if (IsInWorld()) GetMap()->VisitGrid(GetPositionX(), GetPositionY(), radius, notifier, loadGrids, ignoreRadiusLimit); }
        template<class NOTIFIER> void VisitNearbyWorldObject(float const& radius, NOTIFIER& notifier, bool loadGrids = false, bool ignoreRadiusLimit = false) const { if (IsInWorld()) GetMap()->VisitWorld(GetPositionX(), GetPositionY(), radius, notifier, loadGrids, ignoreRadiusLimit); }

#ifdef MAP_BASED_RAND_GEN
        int32 irand(int32 min, int32 max) const     { return int32 (GetMap()->mtRand.randInt(max - min)) + min; }
        uint32 urand(uint32 min, uint32 max) const  { return GetMap()->mtRand.randInt(max - min) + min;}
        int32 rand32() const                        { return GetMap()->mtRand.randInt();}
        double rand_norm() const                    { return GetMap()->mtRand.randExc();}
        double rand_chance() const                  { return GetMap()->mtRand.randExc(100.0);}
#endif

        uint32  LastUsedScriptID;

        // Transports
        Transport* GetTransport() const { return m_transport; }
        float GetTransOffsetX() const { return m_movementInfo.transport.pos.GetPositionX(); }
        float GetTransOffsetY() const { return m_movementInfo.transport.pos.GetPositionY(); }
        float GetTransOffsetZ() const { return m_movementInfo.transport.pos.GetPositionZ(); }
        float GetTransOffsetO() const { return m_movementInfo.transport.pos.GetOrientation(); }
        uint32 GetTransTime()   const { return m_movementInfo.transport.time; }
        int8 GetTransSeat()     const { return m_movementInfo.transport.seat; }
        virtual ObjectGuid GetTransGUID() const;
        void SetTransport(Transport* t) { m_transport = t; }
        bool AddToTransportIfNeeded(Transport* transport, bool setHome = false, float extents = 0.0f); // Add the object to transport and adjust positions if it is inside transport's bounds + extents

        MovementInfo m_movementInfo;

        bool IsSpawnedInLFRDungeon() const;

        virtual float GetStationaryX() const { return GetPositionX(); }
        virtual float GetStationaryY() const { return GetPositionY(); }
        virtual float GetStationaryZ() const { return GetPositionZ(); }
        virtual float GetStationaryO() const { return GetOrientation(); }

        float GetFloorZ() const;
        [[nodiscard]] float GetMinHeightInWater() const;
        virtual float GetCollisionHeight() const { return 0.0f; }
        [[nodiscard]] virtual float GetCollisionWidth() const { return GetObjectSize(); }
        [[nodiscard]] virtual float GetCollisionRadius() const { return GetObjectSize() / 2; }

        float GetMapWaterOrGroundLevel(float x, float y, float z, float* ground = nullptr) const;
        float GetMapHeight(float x, float y, float z, bool vmap = true, float distanceToSearch = 50.0f) const; // DEFAULT_HEIGHT_SEARCH in map.h

        virtual uint16 GetAIAnimKitId() const { return 0; }
        virtual uint16 GetMovementAnimKitId() const { return 0; }
        virtual uint16 GetMeleeAnimKitId() const { return 0; }

        // Watcher
        bool IsPrivateObject() const { return !_privateObjectOwner.IsEmpty(); }
        ObjectGuid GetPrivateObjectOwner() const { return _privateObjectOwner; }
        void SetPrivateObjectOwner(ObjectGuid const& owner) { _privateObjectOwner = owner; }
        bool CheckPrivateObjectOwnerVisibility(WorldObject const* seer) const;

        void AddToUpdate() override;
        void RemoveFromUpdate() override;

        void LoadCustomVisibility();
        bool HasCustomVisibility() const { return m_hasCustomVisibility; }
        float GetCustomVisibilityDistance() const { return m_customVisibilityDistance; }
        void SetCustomVisibilityDistance(float distance, bool zoneOnly);
        bool IsCustomVisibilityZoneOnly() const { return m_customVisibilityZoneOnly; }
        uint32 GetCustomVisibilityZoneID() const { return m_customVisibilityZoneID; }
        void SetCustomVisibilityZoneID(uint32 zoneId) { m_customVisibilityZoneID = zoneId; }

        virtual Battleground* GetBattlegorund() const;

    protected:
        std::string m_name;
        bool m_isActive;
        ActiveFlags m_activeFlags = ActiveFlags::None;
        bool m_isFarVisible;
        Optional<float> m_visibilityDistanceOverride;        
        const bool m_isWorldObject;
        ZoneScript* m_zoneScript;

        // transports
        Transport* m_transport;

        virtual void ProcessPositionDataChanged(PositionFullTerrainStatus const& data);
        uint32 m_zoneId;
        uint32 m_areaId;
        float m_staticFloorZ;
        bool m_outdoors;
        ZLiquidStatus m_liquidStatus;

        //these functions are used mostly for Relocate() and Corpse/Player specific stuff...
        //use them ONLY in LoadFromDB()/Create() funcs and nowhere else!
        //mapId/instanceId should be set in SetMap() function!
        void SetLocationMapId(uint32 _mapId) { m_mapId = _mapId; }
        void SetLocationInstanceId(uint32 _instanceId) { m_InstanceId = _instanceId; }

        virtual bool CanNeverSee(WorldObject const* obj) const;
        virtual bool CanAlwaysSee([[maybe_unused]] WorldObject const* /*obj*/) const { return false; }
        virtual bool IsNeverVisibleFor([[maybe_unused]] WorldObject const* seer, [[maybe_unused]] bool allowServersideObjects = false) const { return !IsInWorld() || IsDestroyedObject(); }
        virtual bool IsAlwaysVisibleFor(WorldObject const* /*seer*/) const { return false; }
        virtual bool IsInvisibleDueToDespawn() const { return false; }
        //difference from IsAlwaysVisibleFor: 1. after distance check; 2. use owner or charmer as seer
        virtual bool IsAlwaysDetectableFor(WorldObject const* /*seer*/) const { return false; }
    private:
        Map* m_currMap;                                    //current object's Map location

        //uint32 m_mapId;                                     // object at map with map_id
        uint32 m_InstanceId;                                // in map copy with instance id
        uint32 m_phaseMask;                                 // in area phase state
        std::set<uint32> _phases;
        std::set<uint32> _terrainSwaps;
        std::set<uint32> _worldMapSwaps;

        ObjectGuid _privateObjectOwner;

        bool m_hasCustomVisibility = false;
        float m_customVisibilityDistance = 0;
        bool m_customVisibilityZoneOnly = false;
        uint32 m_customVisibilityZoneID = 0; // Used to know which Map::m_customVisibilityDistanceObjectsByZone container the object was added to

        virtual bool _IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D, bool incOwnRadius = true, bool incTargetRadius = true) const;

        bool CanDetect(WorldObject const* obj, bool ignoreStealth) const;
        bool CanDetectInvisibilityOf(WorldObject const* obj) const;
        bool CanDetectStealthOf(WorldObject const* obj) const;

        uint64 m_explicitSeerGuid;
        TimeTrackerSmall m_stealthVisibilityUpdateTimer;
};

namespace Trinity
{
    template<class T>
    void RandomResizeList(std::list<T> &_list, uint32 _size)
    {
        while (_list.size() > _size)
        {
            typename std::list<T>::iterator itr = _list.begin();
            advance(itr, urand(0, _list.size() - 1));
            _list.erase(itr);
        }
    }
    template<class T, class Predicate>
    void RandomResizeList(std::list<T> &list, Predicate& predicate, uint32 size)
    {
        //! First use predicate filter
        std::list<T> listCopy;
        for (typename std::list<T>::iterator itr = list.begin(); itr != list.end(); ++itr)
        if (predicate(*itr))
            listCopy.push_back(*itr);

        if (size)
            RandomResizeList(listCopy, size);

        list = listCopy;
    }
    // Binary predicate to sort WorldObjects based on the distance to a reference WorldObject
    class ObjectDistanceOrderPred
    {
        public:
            ObjectDistanceOrderPred(WorldObject const* pRefObj, bool ascending = true) : m_refObj(pRefObj), m_ascending(ascending) { }
            bool operator()(WorldObject const* pLeft, WorldObject const* pRight) const
            {
                return m_ascending ? m_refObj->GetDistanceOrder(pLeft, pRight) : !m_refObj->GetDistanceOrder(pLeft, pRight);
            }
        private:
            WorldObject const* m_refObj;
            const bool m_ascending;
    };
}

#endif
