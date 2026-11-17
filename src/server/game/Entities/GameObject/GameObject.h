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

#ifndef TRINITYCORE_GAMEOBJECT_H
#define TRINITYCORE_GAMEOBJECT_H

#include "Common.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "Object.h"
#include "GridObject.h"
#include "GameObjectData.h"
#include "LootMgr.h"
#include "DBCStores.h"

class GameObjectAI;
class Group;
class Transport;
enum SpellTargetCheckTypes : uint8;


// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint32, GameObjectTemplate> GameObjectTemplateContainer;
typedef std::unordered_map<uint32, GameObjectTemplateAddon> GameObjectTemplateAddonContainer;

class OPvPCapturePoint;
struct TransportAnimation;

union GameObjectValue
{
    //6 GAMEOBJECT_TYPE_TRAP
    struct
    {
        SpellTargetCheckTypes TargetSearcherCheckType;
    } Trap;
    //11 GAMEOBJECT_TYPE_TRANSPORT
    struct
    {
        uint32 PathProgress;
        TransportAnimation const* AnimationInfo;
        uint32 CurrentSeg;
    } Transport;
    //25 GAMEOBJECT_TYPE_FISHINGHOLE
    struct
    {
        uint32 MaxOpens;
    } FishingHole;
    //29 GAMEOBJECT_TYPE_CAPTURE_POINT
    struct
    {
        OPvPCapturePoint *OPvPObj;
    } CapturePoint;
    //33 GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING
    struct
    {
        uint32 Health;
        uint32 MaxHealth;
    } Building;
};

// `gameobject_addon` table
struct GameObjectAddon
{
    ::InvisibilityType InvisibilityType;
    uint32 InvisibilityValue;
};

typedef std::unordered_map<uint32, GameObjectAddon> GameObjectAddonContainer;

// client side GO show states
enum GOState
{
    GO_STATE_ACTIVE             = 0,                        // show in world as used and not reset (closed door open)
    GO_STATE_READY              = 1,                        // show in world as ready (closed door close)
    GO_STATE_ACTIVE_ALTERNATIVE = 2,                        // show in world as used in alt way and not reset (closed door open by cannon fire)
    GO_STATE_PREPARE_TRANSPORT  = 24                        // reset transport to startpoint after endpoint
};

// from `gameobject`
struct GameObjectData
{
    explicit GameObjectData() : dbData(true) { }
    uint32 id;                                              // entry in gamobject_template
    uint16 mapid;
    uint32 phaseMask;
    uint32 phaseid;
    uint32 phaseGroup;
    float posX;
    float posY;
    float posZ;
    float orientation;
    G3D::Quat rotation;
    int32  spawntimesecs;
    uint32 animprogress;
    GOState go_state;
    uint16 spawnMask;
    uint8 artKit;
    uint32 ScriptId;
    bool dbData;
    uint32 gameEventId = 0;
};

// For containers:  [GO_NOT_READY]->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED->GO_READY        -> ...
// For bobber:      GO_NOT_READY  ->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED-><deleted>
// For door(closed):[GO_NOT_READY]->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED->GO_READY(close) -> ...
// For door(open):  [GO_NOT_READY]->GO_READY (open) ->GO_ACTIVATED (close)->GO_JUST_DEACTIVATED->GO_READY(open)  -> ...
enum LootState
{
    GO_NOT_READY = 0,
    GO_READY,                                               // can be ready but despawned, and then not possible activate until spawn
    GO_ACTIVATED,
    GO_JUST_DEACTIVATED
};

class Unit;
class GameObjectModel;

// 5 sec for bobber catch
#define FISHING_BOBBER_READY_TIME 5

class TC_GAME_API GameObject : public WorldObject, public GridObject<GameObject>, public MapObject
{
    public:
        explicit GameObject();
        ~GameObject();

        void BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, Player* target) const override;

        void AddToWorld() override;
        void RemoveFromWorld() override;
        void CleanupsBeforeDelete(bool finalCleanup = true);

        bool Create(ObjectGuid::LowType guidlow, uint32 name_id, Map* map, uint32 phaseMask, float x, float y, float z, float ang, G3D::Quat const& rotation, uint32 animprogress, GOState go_state, uint32 artKit = 0);
        void Update(uint32 p_time) override;
        static GameObject* GetGameObject(WorldObject& object, ObjectGuid guid);
        GameObjectTemplate const* GetGOInfo() const { return m_goInfo; }
        GameObjectTemplateAddon const* GetTemplateAddon() const { return m_goTemplateAddon; }
        GameObjectData const* GetGOData() const { return m_goData; }
        GameObjectValue const* GetGOValue() const { return &m_goValue; }

        bool IsTransport() const;
        bool IsDynTransport() const;
        bool IsDestructibleBuilding() const;

        uint32 GetDBTableGUIDLow() const { return m_DBTableGuid; }

        // z_rot, y_rot, x_rot - rotation angles around z, y and x axes
        void SetWorldRotationAngles(float z_rot, float y_rot, float x_rot);
        void SetWorldRotation(G3D::Quat rotation);
        void SetParentRotation(G3D::Quat const& rotation);      // transforms(rotates) transport's path
        int64 GetPackedWorldRotation() const { return m_packedRotation; }
        G3D::Quat const& GetWorldRotation() const { return m_worldRotation; }
        void SetOrientation(float orientation) = delete;

        // overwrite WorldObject function for proper name localization
        std::string const& GetNameForLocaleIdx(LocaleConstant locale_idx) const override;

        void SaveToDB();
        void SaveToDB(uint32 mapid, uint16 spawnMask, uint32 phaseMask);
        bool LoadFromDB(uint32 guid, Map* map) { return LoadGameObjectFromDB(guid, map, false); }
        bool LoadGameObjectFromDB(uint32 guid, Map* map, bool addToMap = true);
        void DeleteFromDB();

        void SetOwnerGUID(ObjectGuid owner)
        {
            // Owner already found and different than expected owner - remove object from old owner
            if (owner && GetOwnerGUID() && GetOwnerGUID() != owner)
            {
                ASSERT(false);
            }
            m_spawnedByDefault = false;                     // all object with owner is despawned after delay
            SetGuidValue(GAMEOBJECT_FIELD_CREATED_BY, owner);
        }
        ObjectGuid GetOwnerGUID() const { return GetGuidValue(GAMEOBJECT_FIELD_CREATED_BY); }
        Unit* GetOwner() const;

        void SetSpellId(uint32 id)
        {
            m_spawnedByDefault = false;                     // all summoned object is despawned after delay
            m_spellId = id;
        }
        uint32 GetSpellId() const { return m_spellId;}

        void ForcedDespawn(uint32 msTimeToDespawn = 0);

        time_t GetRespawnTime() const { return m_respawnTime; }
        time_t GetRespawnTimeEx() const
        {
            time_t now = time(NULL);
            if (m_respawnTime > now)
                return m_respawnTime;
            else
                return now;
        }

        void SetRespawnTime(int32 respawn)
        {
            m_respawnTime = respawn > 0 ? time(NULL) + respawn : 0;
            m_respawnDelayTime = respawn > 0 ? respawn : 0;
        }
        void Respawn();
        bool isSpawned() const
        {
            return m_respawnDelayTime == 0 ||
                (m_respawnTime > 0 && !m_spawnedByDefault) ||
                (m_respawnTime == 0 && m_spawnedByDefault);
        }
        bool isSpawnedByDefault() const { return m_spawnedByDefault; }
        void SetSpawnedByDefault(bool b) { m_spawnedByDefault = b; }
        uint32 GetRespawnDelay() const { return m_respawnDelayTime; }
        void Refresh();
        void Delete();
        void getFishLoot(Loot* loot, Player* loot_owner);
        GOState GetGoState() const { return GOState(GetByteValue(GAMEOBJECT_BYTES_1, 0)); }
        void SetGoState(GOState state);
        GameobjectTypes GetGoType() const { return GameobjectTypes(GetByteValue(GAMEOBJECT_BYTES_1, 1)); }
        void SetGoType(GameobjectTypes type) { SetByteValue(GAMEOBJECT_BYTES_1, 1, type); }
        void SetGoHealth(uint8 health) { SetByteValue(GAMEOBJECT_BYTES_1, 3, health); }
        virtual uint32 GetTransportPeriod() const;
        uint8 GetGoArtKit() const { return GetByteValue(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 1); }
        void SetGoArtKit(uint8 artkit);
        uint8 GetGoAnimProgress() const { return GetByteValue(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 3); }
        void SetGoAnimProgress(uint8 animprogress) { SetByteValue(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 3, animprogress); }
        void SetGameobjectTransparence(uint8 transparency) { SetByteValue(GAMEOBJECT_FIELD_STATE_SPELL_VISUAL_ID, 0, transparency); }
        static void SetGoArtKit(uint8 artkit, GameObject* go, uint32 lowguid = 0);

        void SetPhaseMask(uint32 newPhaseMask, bool update) override;
        bool SetPhased(uint32 id, bool update, bool apply);
        void EnableCollision(bool enable);
        void UpdateCollision();

        void Use(Unit* user);

        LootState getLootState() const { return m_lootState; }
        // Note: unit is only used when s = GO_ACTIVATED
        void SetLootState(LootState s, Unit* unit = NULL);

        uint16 GetLootMode() { return m_LootMode; }
        bool HasLootMode(uint16 lootMode) { return m_LootMode & lootMode; }
        void SetLootMode(uint16 lootMode) { m_LootMode = lootMode; }
        void AddLootMode(uint16 lootMode) { m_LootMode |= lootMode; }
        void RemoveLootMode(uint16 lootMode) { m_LootMode &= ~lootMode; }
        void ResetLootMode() { m_LootMode = LOOT_MODE_DEFAULT; }

        void AddToSkillupList(uint32 PlayerGuidLow) { m_SkillupList.push_back(PlayerGuidLow); }
        bool IsInSkillupList(uint32 PlayerGuidLow) const
        {
            for (std::list<uint32>::const_iterator i = m_SkillupList.begin(); i != m_SkillupList.end(); ++i)
                if (*i == PlayerGuidLow)
                    return true;

            return false;
        }
        void ClearSkillupList() { m_SkillupList.clear(); }

        void AddUniqueUse(Player* player);
        void AddUse() { ++m_usetimes; }

        uint32 GetUseCount() const { return m_usetimes; }
        uint32 GetUniqueUseCount() const { return m_unique_users.size(); }

        void SaveRespawnTime();

        Loot        loot;

        Player* GetLootRecipient() const;
        Group* GetLootRecipientGroup() const;
        bool IsLootRecipientGroupMember(uint64 guid) const { return m_lootRecipientGroupMembers.empty() || m_lootRecipientGroupMembers.find(guid) != m_lootRecipientGroupMembers.end(); }
        void SetLootRecipient(Unit* unit);
        bool IsLootAllowedFor(Player const* player) const;
        bool HasLootRecipient() const { return m_lootRecipient || m_lootRecipientGroup; }
        uint32 m_groupLootTimer;                            // (msecs)timer used for group loot
        uint32 lootingGroupLowGUID;                         // used to find group which is looting

        bool hasQuest(uint32 quest_id) const;
        bool hasInvolvedQuest(uint32 quest_id) const;
        bool ActivateToQuest(Player* target) const;
        void UseDoorOrButton(uint32 time_to_restore = 0, bool alternative = false, Unit* user = NULL);
                                                            // 0 = use `gameobject`.`spawntimesecs`
        void ResetDoorOrButton();

        void TriggeringLinkedGameObject(uint32 trapEntry, Unit* target);

        bool IsNeverVisibleFor(WorldObject const* seer, bool allowServersideObjects = false) const override;
        bool IsAlwaysVisibleFor(WorldObject const* seer) const;
        bool IsInvisibleDueToDespawn() const;

        uint8 getLevelForTarget(WorldObject const* target) const
        {
            if (Unit* owner = GetOwner())
                return owner->GetLevelForTarget(target);

            return 1;
        }

        GameObject* LookupFishingHoleAround(float range);

        void CastSpell(Unit* target, uint32 spell);
        void SendCustomAnim(uint32 anim);
        SpellInfo const* GetSpellForLock(Player const* player) const;
        bool IsAtInteractDistance(Player const* player, SpellInfo const* spell = nullptr) const;
        bool IsAtInteractDistance(Position const& pos, float radius) const;

        void ModifyHealth(int32 change, Unit* attackerOrHealer = NULL, uint32 spellId = 0);
        // sets GameObject type 33 destruction flags and optionally default health for that state
        void SetDestructibleState(GameObjectDestructibleState state, Player* eventInvoker = NULL, bool setHealth = false);
        GameObjectDestructibleState GetDestructibleState() const
        {
            if (HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DESTROYED))
                return GO_DESTRUCTIBLE_DESTROYED;
            if (HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_DAMAGED))
                return GO_DESTRUCTIBLE_DAMAGED;
            return GO_DESTRUCTIBLE_INTACT;
        }

        void EventInform(uint32 eventId);

        virtual uint32 GetScriptId() const;
        GameObjectAI* AI() const { return m_AI; }

        std::string GetAIName() const;
        void SetDisplayId(uint32 displayid);
        uint32 GetDisplayId() const { return GetUInt32Value(GAMEOBJECT_FIELD_DISPLAY_ID); }

        uint32 GetFaction() const { return GetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE); }
        void SetFaction(uint32 faction) { SetUInt32Value(GAMEOBJECT_FIELD_FACTION_TEMPLATE, faction); }

        GameObjectModel* m_model;
        void GetRespawnPosition(float &x, float &y, float &z, float* ori = NULL) const;

        Transport* ToTransport() { if (GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT || GetGOInfo()->type == GAMEOBJECT_TYPE_MO_TRANSPORT) return reinterpret_cast<Transport*>(this); else return NULL; }
        Transport const* ToTransport() const { if (GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT || GetGOInfo()->type == GAMEOBJECT_TYPE_MO_TRANSPORT) return reinterpret_cast<Transport const*>(this); else return NULL; }

        float GetStationaryX() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionX(); return GetPositionX(); }
        float GetStationaryY() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionY(); return GetPositionY(); }
        float GetStationaryZ() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetPositionZ(); return GetPositionZ(); }
        float GetStationaryO() const { if (GetGOInfo()->type != GAMEOBJECT_TYPE_MO_TRANSPORT) return m_stationaryPosition.GetOrientation(); return GetOrientation(); }
        void RelocateStationaryPosition(float x, float y, float z, float o) { m_stationaryPosition.Relocate(x, y, z, o); }

        uint16 GetAIAnimKitId() const override { return _animKitId; }
        void SetAnimKitId(uint16 animKitId, bool oneshot);

        void UpdateModelPosition();

        // Event handler
        EventProcessor m_Events;
        
        bool AIM_Initialize();
        void AddDelayedEvent(uint64 timeOffset, std::function<void()>&& function);
    protected:
        void CreateModel();
        void UpdateModel();                                 // updates model in case displayId were changed
        uint32      m_spellId;
        time_t      m_respawnTime;                          // (secs) time of next respawn (or despawn if GO have owner()),
        uint32      m_respawnDelayTime;                     // (secs) if 0 then current GO state no dependent from timer
        LootState   m_lootState;
        bool        m_spawnedByDefault;
        time_t      m_cooldownTime;                         // used as internal reaction delay time store (not state change reaction).
        uint32      m_trapCooldownTime = 0;                 // higher resolution timer than time_t, used only for traps

        GOState     m_prevGoState;                          // What state to set whenever resetting

        std::list<uint32> m_SkillupList;

        Player* m_ritualOwner;                              // used for GAMEOBJECT_TYPE_SUMMONING_RITUAL where GO is not summoned (no owner)
        std::set<ObjectGuid> m_unique_users;
        uint32 m_usetimes;

        typedef std::map<uint32, ObjectGuid> ChairSlotAndUser;
        ChairSlotAndUser ChairListSlots;

        uint32 m_DBTableGuid;                               ///< For new or temporary gameobjects is 0 for saved it is lowguid
        GameObjectTemplate const* m_goInfo;
        GameObjectTemplateAddon const* m_goTemplateAddon;
        GameObjectData const* m_goData;
        GameObjectValue m_goValue;

        int64 m_packedRotation;
        G3D::Quat m_worldRotation;
        Position m_stationaryPosition;

        ObjectGuid m_lootRecipient;
        uint32 m_lootRecipientGroup;
        std::set<uint64> m_lootRecipientGroupMembers;
        uint16 m_LootMode;                                  // bitmask, default LOOT_MODE_DEFAULT, determines what loot will be lootable

    private:
        void RemoveFromOwner();
        void SwitchDoorOrButton(bool activate, bool alternative = false);
        void UpdatePackedRotation();

        //! Object distance/size - overridden from Object::_IsWithinDist. Needs to take in account proper GO size.
        bool _IsWithinDist(WorldObject const* obj, float dist2compare, bool /*is3D*/, bool /*incOwnRadius*/, bool /*incTargetRadius*/) const override
        {
            //! Following check does check 3d distance
            return IsAtInteractDistance(obj->GetPosition(), dist2compare);
        }
        GameObjectAI* m_AI;
        uint16 _animKitId;
        FunctionProcessor _functions_delayed;
};

class ForcedGoDespawnDelayEvent : public BasicEvent
{
    public:
        ForcedGoDespawnDelayEvent(GameObject& owner) : BasicEvent(), m_owner(owner) { }
        bool Execute(uint64 e_time, uint32 p_time);

    private:
        GameObject& m_owner;
};

#endif
