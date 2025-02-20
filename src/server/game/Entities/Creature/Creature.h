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

#ifndef TRINITYCORE_CREATURE_H
#define TRINITYCORE_CREATURE_H

#include "Common.h"
#include "CreatureData.h"
#include "Unit.h"
#include "UpdateMask.h"
#include "ItemPrototype.h"
#include "LootMgr.h"
#include "Cell.h"
#include "GridObject.h"

#include <list>

class CreatureAI;
class CreatureGroup;
class Group;
class Quest;
class Player;
class SpellInfo;
class WorldSession;

#define CREATURE_REGEN_INTERVAL 2 * IN_MILLISECONDS

#define MAX_EQUIPMENT_ITEMS 3



// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint32, CreatureTemplate> CreatureTemplateContainer;

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push, N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif



typedef std::unordered_map<uint16, CreatureBaseStats> CreatureBaseStatsContainer;

struct PointOfInterestLocale
{
    std::vector<std::string> Name;
};

struct EquipmentInfo
{
    uint32  ItemEntry[MAX_EQUIPMENT_ITEMS];
};

// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint8, EquipmentInfo> EquipmentInfoContainerInternal;
typedef std::unordered_map<uint32, EquipmentInfoContainerInternal> EquipmentInfoContainer;

// from `creature` table
struct CreatureData
{
    CreatureData() : dbData(true) { }
    uint32 id;                                              // entry in creature_template
    uint16 mapId;
    uint32 phaseMask;
    uint32 phaseid;
    uint32 phaseGroup;
    uint32 displayid;
    int8 equipmentId;
    float posX;
    float posY;
    float posZ;
    float orientation;
    uint32 spawntimesecs;
    uint32 spawntimesecs_max;
    float wander_distance;
    uint32 currentwaypoint;
    uint32 curhealth;
    uint32 curmana;
    uint8 movementType;
    uint16 spawnMask;
    uint32 npcflag;
    uint32 npcflag2;
    uint32 unit_flags;                                      // enum UnitFlags mask values
    uint32 unit_flags2;                                     // enum UnitFlags2 mask values
    uint32 dynamicflags;
    uint32 ScriptId;
    float WalkMode;
    bool dbData;
    uint32 gameEventId = 0;
};



// Benchmarked: Faster than std::map (insert/find)
typedef std::unordered_map<uint16, CreatureModelInfo> CreatureModelContainer;

enum InhabitTypeValues
{
    INHABIT_NONE   = 0,
    INHABIT_GROUND = 1,
    INHABIT_WATER  = 2,
    INHABIT_AIR    = 4,
    INHABIT_ROOT   = 8,
    INHABIT_ANYWHERE = INHABIT_GROUND | INHABIT_WATER | INHABIT_AIR | INHABIT_ROOT
};

inline InhabitTypeValues operator|(InhabitTypeValues a, InhabitTypeValues b) { return static_cast<InhabitTypeValues>(static_cast<std::underlying_type<InhabitTypeValues>::type>(a) | static_cast<std::underlying_type<InhabitTypeValues>::type>(b)); }

// Enums used by StringTextData::Type (CreatureEventAI)
enum ChatType
{
    CHAT_TYPE_SAY               = 0,
    CHAT_TYPE_YELL              = 1,
    CHAT_TYPE_TEXT_EMOTE        = 2,
    CHAT_TYPE_BOSS_EMOTE        = 3,
    CHAT_TYPE_WHISPER           = 4,
    CHAT_TYPE_BOSS_WHISPER      = 5,
    CHAT_TYPE_ZONE_YELL         = 6,
    CHAT_TYPE_END               = 255
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

typedef std::unordered_map<uint32, CreatureAddon> CreatureAddonContainer;

// Vendors
struct VendorItem
{
    VendorItem(uint32 _item, int32 _maxcount, uint32 _incrtime, uint32 _ExtendedCost, uint8 _Type)
        : item(_item), maxcount(_maxcount), incrtime(_incrtime), ExtendedCost(_ExtendedCost), Type(_Type) { }

    uint32 item;
    uint32 maxcount;                                        // 0 for infinity item amount
    uint32 incrtime;                                        // time for restore items amount if maxcount != 0
    uint32 ExtendedCost;
    uint8  Type;

    //helpers
    bool IsGoldRequired(ItemTemplate const* pProto) const { return pProto->Flags2 & ITEM_FLAGS_EXTRA_EXT_COST_REQUIRES_GOLD || !ExtendedCost; }
};
typedef std::vector<VendorItem*> VendorItemList;

struct VendorItemData
{
    VendorItemList m_items;

    VendorItem* GetItem(uint32 slot) const
    {
        if (slot >= m_items.size())
            return NULL;

        return m_items[slot];
    }
    bool Empty() const { return m_items.empty(); }
    uint32 GetItemCount() const { return m_items.size(); }
    void AddItem(uint32 item, int32 maxcount, uint32 ptime, uint32 ExtendedCost, uint8 type)
    {
        m_items.push_back(new VendorItem(item, maxcount, ptime, ExtendedCost, type));
    }
    bool RemoveItem(uint32 item_id, uint8 type);
    VendorItem const* FindItemCostPair(uint32 item_id, uint32 extendedCost, uint8 type) const;
    void Clear()
    {
        for (VendorItemList::const_iterator itr = m_items.begin(); itr != m_items.end(); ++itr)
            delete (*itr);
        m_items.clear();
    }
};

struct VendorItemCount
{
    explicit VendorItemCount(uint32 _item, uint32 _count)
        : itemId(_item), count(_count), lastIncrementTime(time(NULL)) { }

    uint32 itemId;
    uint32 count;
    time_t lastIncrementTime;
};

typedef std::list<VendorItemCount> VendorItemCounts;

struct TrainerSpell
{
    TrainerSpell() : spell(0), spellCost(0), reqSkill(0), reqSkillValue(0), reqLevel(0)
    {
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            learnedSpell[i] = 0;
    }

    uint32 spell;
    uint32 spellCost;
    uint32 reqSkill;
    uint32 reqSkillValue;
    uint32 reqLevel;
    uint32 learnedSpell[MAX_SPELL_EFFECTS];

    // helpers
    bool IsCastable() const { return learnedSpell[0] != spell; }
};

typedef std::unordered_map<uint32 /*spellid*/, TrainerSpell> TrainerSpellMap;

struct TrainerSpellData
{
    TrainerSpellData() : trainerType(0) { }
    ~TrainerSpellData() { spellList.clear(); }

    TrainerSpellMap spellList;
    uint32 trainerType;                                     // trainer type based at trainer spells, can be different from creature_template value.
                                                            // req. for correct show non-prof. trainers like weaponmaster, allowed values 0 and 2.
    TrainerSpell const* Find(uint32 spell_id) const;
};

typedef std::map<uint32, time_t> CreatureSpellCooldowns;

// max different by z coordinate for creature aggro reaction
#define CREATURE_Z_ATTACK_RANGE 3

#define MAX_VENDOR_ITEMS 150                                // Limitation in 4.x.x item count in SMSG_LIST_INVENTORY

//used for handling non-repeatable random texts
typedef std::vector<uint8> CreatureTextRepeatIds;
typedef std::unordered_map<uint8, CreatureTextRepeatIds> CreatureTextRepeatGroup;

class TC_GAME_API Creature : public Unit, public GridObject<Creature>, public MapObject
{
    public:

        explicit Creature(bool isWorldObject = false);
        virtual ~Creature();

        void AddToWorld() override;
        void RemoveFromWorld() override;

        void SetObjectScale(float scale) override;
        void SetDisplayId(uint32 displayId, float displayScale = 1.f) override;
        void SetDisplayFromModel(uint32 modelIdx);

        void DisappearAndDie();

        bool Create(uint32 guidlow, Map* map, uint32 phaseMask, uint32 Entry, uint32 vehId, uint32 team, float x, float y, float z, float ang, const CreatureData* data = NULL);
        bool LoadCreaturesAddon(bool reload = false);
        void SelectLevel(const CreatureTemplate* cinfo);
        void LoadEquipment(int8 id = 1, bool force = false);

        uint32 GetDBTableGUIDLow() const { return m_spawnId; }
        ObjectGuid::LowType GetSpawnId() const { return m_spawnId; }

        void Update(uint32 time) override;         // overwrited Unit::Update
        void GetRespawnPosition(float &x, float &y, float &z, float* ori = NULL, float* dist =NULL) const;

        void SetCorpseDelay(uint32 delay) { m_corpseDelay = delay; }
        uint32 GetCorpseDelay() const { return m_corpseDelay; }
        bool IsRacialLeader() const { return GetCreatureTemplate()->RacialLeader; }
        bool IsCivilian() const { return GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_CIVILIAN; }
        bool IsTrigger() const { return GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER; }
        bool IsGuard() const { return GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_GUARD; }
        
        CreatureMovementData const& GetMovementTemplate() const;
        bool CanWalk() const { return GetMovementTemplate().IsGroundAllowed(); }
        bool CanSwim() const override;
        bool CanEnterWater() const override;
        bool CanFly()  const override { return GetMovementTemplate().IsFlightAllowed() || IsFlying(); }
        bool CanHover() const { return GetMovementTemplate().Ground == CreatureGroundMovementType::Hover || IsHovering(); } 
        [[nodiscard]] bool HasSwimmingFlagOutOfCombat() const
        {
            return !_isMissingSwimmingFlagOutOfCombat;
        }
        void RefreshSwimmingFlag(bool recheck = false);
        // Used to dynamically change allowed path generator and movement flags behavior during scripts.
        // Can be used to allow ground-only creatures to temporarily fly, restrict flying creatures to the ground etc.
        void OverrideInhabitType(InhabitTypeValues inhabitType) {  } // todo remove in future 
        void ResetInhabitTypeOverride() { } // todo remove in future

        void SetReactState(ReactStates st) { m_reactState = st; }
        ReactStates GetReactState() { return m_reactState; }
        bool HasReactState(ReactStates state) const { return (m_reactState == state); }
        void InitializeReactState();

        /// @todo Rename these properly
        bool isCanInteractWithBattleMaster(Player* player, bool msg) const;
        bool isCanTrainingAndResetTalentsOf(Player* player) const;
        bool CanCreatureAttack(Unit const* victim, bool force = true) const;
        bool IsImmunedToSpell(SpellInfo const* spellInfo, uint32 effectMask) const override;
        bool IsImmunedToSpellEffect(SpellInfo const* spellInfo, uint32 index) const override; // override Unit::IsImmunedToSpellEffect
        bool isElite() const;
        bool isWorldBoss() const;

        bool IsDungeonBoss() const;

        uint8 GetLevelForTarget(WorldObject const* target) const override; // overwrite Unit::getLevelForTarget for boss level support

        bool IsInEvadeMode() const { return HasUnitState(UNIT_STATE_EVADE); }
        bool IsEvadingAttacks() const { return IsInEvadeMode() || CanNotReachTarget(); }

        bool AIM_Initialize(CreatureAI* ai = NULL);
        void Motion_Initialize();

        CreatureAI* AI() const { return reinterpret_cast<CreatureAI*>(GetAI()); }

        SpellSchoolMask GetMeleeDamageSchoolMask() const override { return m_meleeDamageSchoolMask; }
        void SetMeleeDamageSchool(SpellSchools school) { m_meleeDamageSchoolMask = SpellSchoolMask(1 << school); }

        void _AddCreatureSpellCooldown(uint32 spell_id, time_t end_time);
        void _AddCreatureCategoryCooldown(uint32 category, time_t apply_time);
        void AddCreatureSpellCooldown(uint32 spellid);
        bool HasSpellCooldown(uint32 spell_id) const;
        bool HasCategoryCooldown(uint32 spell_id) const;
        uint32 GetCreatureSpellCooldownDelay(uint32 spellId) const;
        virtual void ProhibitSpellSchool(SpellSchoolMask idSchoolMask, uint32 unTimeMs) override;

        // For spells with Cone targets
        void PrepareChanneledCast(float facing, uint32 spellId = 0, bool triggered = false);
        void RemoveChanneledCast(ObjectGuid target = ObjectGuid::Empty);
        void FixateOnTarget(ObjectGuid targetGUID, uint32 timer = 0);

        bool HasSpell(uint32 spellID) const override;

        void SetPhaseMask(uint32 newPhaseMask, bool update) override;// overwrite Unit::SetPhaseMask
        bool UpdateEntry(uint32 entry, uint32 team=ALLIANCE, const CreatureData* data = nullptr);

        void UpdateMovementFlags(bool force = false);

        void SetFlying(bool enable);

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;

        int8 GetOriginalEquipmentId() const { return m_originalEquipmentId; }
        uint8 GetCurrentEquipmentId() { return m_equipmentId; }
        void SetCurrentEquipmentId(uint8 id) { m_equipmentId = id; }

        float GetSpellDamageMod(int32 Rank) const;

        void SetGossip(bool enabled, uint32 menuId = 0)
        {
            ApplyModFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP, enabled);
            m_overrideGossipMenuID = menuId;
        }
        uint32 GetGossipMenuId() const { return m_overrideGossipMenuID ? m_overrideGossipMenuID : GetCreatureTemplate()->GossipMenuId; }

        void SetOverrideVendorEntry(uint32 entry) { m_overrideVendorEntry = entry; }
        VendorItemData const* GetVendorItems() const;
        uint32 GetVendorItemCurrentCount(VendorItem const* vItem);
        uint32 UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count);

        TrainerSpellData const* GetTrainerSpells() const;

        CreatureTemplate const* GetCreatureTemplate() const { return m_creatureInfo; }
        CreatureData const* GetCreatureData() const { return m_creatureData; }
        CreatureAddon const* GetCreatureAddon() const;

        float GetSparringHealthLimit() const;

        std::string const& GetAIName() const;
        std::string GetScriptName() const;
        uint32 GetScriptId() const;

        // override WorldObject function for proper name localization
        std::string const& GetNameForLocaleIdx(LocaleConstant locale_idx) const override;

        void setDeathState(DeathState s) override;                   // override virtual Unit::setDeathState

        bool LoadFromDB(uint32 guid, Map* map) { return LoadCreatureFromDB(guid, map, false); }
        bool LoadCreatureFromDB(uint32 guid, Map* map, bool addToMap = true);
        void SaveToDB();
                                                            // overriden in Pet
        virtual void SaveToDB(uint32 mapid, uint16 spawnMask, uint32 phaseMask);
        void DeleteFromDB();                        // overriden in Pet

        Loot loot;
        bool lootForPickPocketed;
        bool lootForBody;
        void StartPickPocketRefillTimer();
        void ResetPickPocketRefillTimer() { m_pickpocketLootRestore = TimeValue::zero(); }
        Player* GetLootRecipient() const;
        Group* GetLootRecipientGroup() const;
        bool IsLootRecipientGroupMember(ObjectGuid guid) const { return m_lootRecipientGroupMembers.empty() || m_lootRecipientGroupMembers.find(guid) != m_lootRecipientGroupMembers.end(); }
        std::set<ObjectGuid> const& GetLootRecipients() const { return m_lootRecipients; }
        bool HasLootRecipient() const { return m_lootRecipient || m_lootRecipientGroup; }
        bool HasLootRecipient(Player const* player) const;
        bool IsTappedBy(Player const* player) const;                          // return true if the creature is tapped by the player or a member of his party.

        void SetLootRecipient(Unit* unit);
        void AddLootRecipient(ObjectGuid guid) { m_lootRecipients.insert(guid); } // Additional loot recipients, for rare mobs on Timeless Isle in generally
        void AllLootRemovedFromCorpse();

        uint16 GetLootMode() { return m_LootMode; }
        bool HasLootMode(uint16 lootMode) { return m_LootMode & lootMode; }
        void SetLootMode(uint16 lootMode) { m_LootMode = lootMode; }
        void AddLootMode(uint16 lootMode) { m_LootMode |= lootMode; }
        void RemoveLootMode(uint16 lootMode) { m_LootMode &= ~lootMode; }
        void ResetLootMode() { m_LootMode = LOOT_MODE_DEFAULT; }
        bool HasNormalLootMode() const { return m_hasNormalLootMode; }

        SpellInfo const* reachWithSpellAttack(Unit* victim);
        SpellInfo const* reachWithSpellCure(Unit* victim);

        uint32 m_spells[CREATURE_MAX_SPELLS];
        CreatureSpellCooldowns m_CreatureSpellCooldowns;
        CreatureSpellCooldowns m_CreatureCategoryCooldowns;

        bool CanStartAttack(Unit const* u, bool force) const;
        float GetAttackDistance(Unit const* player) const;
        float GetAggroRange(Unit const* target) const;

        void SendAIReaction(AiReaction reactionType);

        Unit* SelectNearestTarget(float dist = 0, bool playerOnly = false) const;
        Unit* SelectNearestTargetInAttackDistance(float dist = 0) const;
        Player* SelectNearestPlayer(float distance = 0) const;
        Player* SelectNearestPlayerNotGM(float distance = 0) const;
        Unit* SelectNearestHostileUnitInAggroRange(bool useLOS = false) const;

        void DoFleeToGetAssistance();
        void CallForHelp(float fRadius);
        void CallAssistance();
        void SetNoCallAssistance(bool val) { m_AlreadyCallAssistance = val; }
        void SetNoSearchAssistance(bool val) { m_AlreadySearchedAssistance = val; }
        bool HasSearchedAssistance() { return m_AlreadySearchedAssistance; }
        bool CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction = true) const;
        bool _IsTargetAcceptable(const Unit* target) const;

        MovementGeneratorType GetDefaultMovementType() const { return m_defaultMovementType; }
        void SetDefaultMovementType(MovementGeneratorType mgt) { m_defaultMovementType = mgt; }

        void RemoveCorpse(bool setSpawnTime = true);
        void SetCorpseRemoveTime(time_t time) { m_corpseRemoveTime = time; }

        void DespawnOrUnsummon(uint32 msTimeToDespawn = 0);

        time_t const& GetRespawnTime() const { return m_respawnTime; }
        time_t GetRespawnTimeEx() const;
        void SetRespawnTime(uint32 respawn) { m_respawnTime = respawn ? time(NULL) + respawn : 0; }
        void Respawn(bool force = false);
        void SaveRespawnTime();

        uint32 GetRespawnDelay() const { return m_respawnDelay; }
        uint32 GetRespawnDelayMax() const { return m_respawnDelayMax; }
        void SetRespawnDelay(uint32 delay) { m_respawnDelay = delay; }

        float GetWanderDistance() const { return m_wanderDistance; }
        void SetWanderDistance(float dist) { m_wanderDistance = dist; }

        void DoImmediateBoundaryCheck() { m_boundaryCheckTime = 0; }
        uint32 GetCombatPulseDelay() const { return m_combatPulseDelay; }
        void SetCombatPulseDelay(uint32 delay) // (secs) interval at which the creature pulses the entire zone into combat (only works in dungeons)
        {
            m_combatPulseDelay = delay;
            if (m_combatPulseTime == 0 || m_combatPulseTime > delay)
                m_combatPulseTime = delay;
        }

        float GetWalkMode() const { return m_WalkMode; }

        uint32 m_groupLootTimer;                            // (msecs)timer used for group loot
        uint32 lootingGroupLowGUID;                         // used to find group which is looting corpse

        void SendZoneUnderAttackMessage(Player* attacker);

        void SetInCombatWithZone();
        void SetCombatDistance(float dist) { m_CombatDistance = dist < 5.0f ? 5.0f : dist; }

        bool hasQuest(uint32 quest_id) const override;
        bool hasInvolvedQuest(uint32 quest_id)  const override;

        bool isRegeneratingHealth() { return m_regenHealth; }
        void setRegeneratingHealth(bool regenHealth) { m_regenHealth = regenHealth; }
        bool isRegeneratingMana() const { return m_regenMana; }
        void setRegeneratingMana(bool regenMana) { m_regenMana = regenMana; }
        virtual uint8 GetPetAutoSpellSize() const { return MAX_SPELL_CHARM; }
        virtual uint32 GetPetAutoSpellOnPos(uint8 pos) const;

        void SetCannotReachTarget(bool cannotReach);
        bool CanNotReachTarget() const { return m_cannotReachTarget; }

        void SetPosition(float x, float y, float z, float o);
        void SetPosition(const Position &pos) { SetPosition(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()); }

        void SetHomePosition(float x, float y, float z, float o) { m_homePosition.Relocate(x, y, z, o); if (m_originalOrientation < 0) m_originalOrientation = Position::NormalizeOrientation(o); }
        void SetHomePosition(Position const& pos) { m_homePosition.Relocate(pos); if (m_originalOrientation < 0) m_originalOrientation = Position::NormalizeOrientation(pos.GetOrientation()); }
        void GetHomePosition(float& x, float& y, float& z, float& ori) const { m_homePosition.GetPosition(x, y, z, ori); }
        Position const& GetHomePosition() const { return m_homePosition; }
        float GetOriginalOrientation() const { return m_originalOrientation; }

        void SetTransportHomePosition(float x, float y, float z, float o) { m_transportHomePosition.Relocate(x, y, z, o); }
        void SetTransportHomePosition(Position const& pos) { m_transportHomePosition.Relocate(pos); }
        void GetTransportHomePosition(float& x, float& y, float& z, float& ori) const { m_transportHomePosition.GetPosition(x, y, z, ori); }
        Position const& GetTransportHomePosition() const { return m_transportHomePosition; }

        uint32 GetWaypointPath(){return m_path_id;}
        void LoadPath(uint32 pathid) { m_path_id = pathid; }

        uint32 GetCurrentWaypointID() const { return m_waypointID; }
        void UpdateWaypointID(uint32 wpID) { m_waypointID = wpID; }

        void SearchFormation();
        CreatureGroup* GetFormation() {return m_formation;}
        void SetFormation(CreatureGroup* formation) {m_formation = formation;}

        Unit* SelectVictim();

        void SetDisableReputationGain(bool disable) { DisableReputationGain = disable; }
        bool IsReputationGainDisabled() { return DisableReputationGain; }
        bool IsDamageEnoughForLootingAndReward() const { return m_PlayerDamageReq == 0; }
        void LowerPlayerDamageReq(uint32 unDamage);
        void ResetPlayerDamageReq() { m_PlayerDamageReq = GetHealth() / 2; }
        uint32 m_PlayerDamageReq;

        uint32 GetOriginalEntry() const { return m_originalEntry; }
        void SetOriginalEntry(uint32 entry) { m_originalEntry = entry; }

        static float _GetDamageMod(int32 Rank);

        float m_SightDistance, m_CombatDistance, m_ReactDistance;

        bool m_isTempWorldObject; //true when possessed

        // Handling caster facing during spellcast
        void SetTarget(ObjectGuid guid);
        void FocusTarget(Spell const* focusSpell, WorldObject const* target);
        void ReleaseFocus(Spell const* focusSpell);

        // TC CreatureText
        CreatureTextRepeatIds GetTextRepeatGroup(uint8 textGroup);
        void SetTextRepeatId(uint8 textGroup, uint8 id);
        void ClearTextRepeatGroup(uint8 textGroup);

        void ApplyInstanceAuraIfNeeded();

        void RecalculateDynamicHealth(uint32 newHealth);

    protected:
        bool CreateFromProto(uint32 guidlow, uint32 Entry, uint32 vehId, uint32 team, const CreatureData* data = NULL);
        bool InitEntry(uint32 entry, uint32 team=ALLIANCE, const CreatureData* data=NULL);

        virtual void Regenerate();
        void RegenerateMana();
        void Regenerate(Powers power);

        uint32 m_overrideGossipMenuID = 0;

        // vendor items
        uint32 m_overrideVendorEntry = 0;
        VendorItemCounts m_vendorItemCounts;

        static float _GetHealthMod(int32 Rank);

        ObjectGuid m_lootRecipient;
        uint32 m_lootRecipientGroup;
        std::set<ObjectGuid> m_lootRecipients;
        std::set<ObjectGuid> m_lootRecipientGroupMembers;

        /// Timers
        time_t m_corpseRemoveTime;                          // (msecs)timer for death or corpse disappearance
        time_t m_respawnTime;                               // (secs) time of next respawn
        uint32 m_respawnDelay;                              // (secs) delay between corpse disappearance and respawning
        uint32 m_respawnDelayMax;                           // (secs) used for dynamic respawn
        uint32 m_corpseDelay;                               // (secs) delay between death and corpse disappearance
        TimeValue m_pickpocketLootRestore;
        float m_wanderDistance;
        uint32 m_boundaryCheckTime;                         // (msecs) remaining time for next evade boundary check
        uint32 m_combatPulseTime;                           // (msecs) remaining time for next zone-in-combat pulse
        uint32 m_combatPulseDelay;                          // (secs) how often the creature puts the entire zone in combat (only works in dungeons)        
        float m_WalkMode;

        ReactStates m_reactState;                           // for AI, not charmInfo

        MovementGeneratorType m_defaultMovementType;
        ObjectGuid::LowType m_spawnId;                      ///< For new or temporary creatures is 0 for saved it is lowguid
        uint8 m_equipmentId;
        int8 m_originalEquipmentId; // can be -1

        bool m_AlreadyCallAssistance;
        bool m_AlreadySearchedAssistance;
        bool m_cannotReachTarget;
        uint32 m_cannotReachTimer;
        
        bool m_regenHealth;
        bool m_regenMana = true;
        bool m_AI_locked;
        bool m_hasNormalLootMode = true;
        bool _isMissingSwimmingFlagOutOfCombat;

        SpellSchoolMask m_meleeDamageSchoolMask;
        uint32 m_originalEntry;

        Position m_homePosition;
        Position m_transportHomePosition;
        Position m_lastMovementFlagsUpdatePosition = Position{ INVALID_HEIGHT, INVALID_HEIGHT, INVALID_HEIGHT };
        float m_originalOrientation = -1;

        bool DisableReputationGain;

        CreatureTemplate const* m_creatureInfo;                 // Can differ from sObjectMgr->GetCreatureTemplate(GetEntry()) in difficulty mode > 0
        CreatureData const* m_creatureData;

        uint16 m_LootMode;                                  // Bitmask (default: LOOT_MODE_DEFAULT) that determines what loot will be lootable
        uint32 guid_transport;

        bool IsInvisibleDueToDespawn() const;
        bool CanAlwaysSee(WorldObject const* obj) const;

        uint32 dynamicHealthPlayersCount;

    public:
        void ForcedDespawn(uint32 timeMSToDespawn = 0);
    private:
        //WaypointMovementGenerator vars
        uint32 m_waypointID;
        uint32 m_path_id;

        //Formation var
        CreatureGroup* m_formation;
        bool m_triggerJustAppeared;

        Spell const* _focusSpell;   ///> Locks the target during spell cast for proper facing
        CreatureTextRepeatGroup m_textRepeat;
};

class AssistDelayEvent : public BasicEvent
{
    public:
        AssistDelayEvent(ObjectGuid victim, Unit& owner) : BasicEvent(), m_victim(victim), m_owner(owner) { }

        bool Execute(uint64 e_time, uint32 p_time);
        void AddAssistant(ObjectGuid guid) { m_assistants.push_back(guid); }
    private:
        AssistDelayEvent();

        ObjectGuid            m_victim;
        std::list<ObjectGuid> m_assistants;
        Unit&                 m_owner;
};

class ForcedDespawnDelayEvent : public BasicEvent
{
    public:
        ForcedDespawnDelayEvent(Creature& owner) : BasicEvent(), m_owner(owner) { }
        bool Execute(uint64 e_time, uint32 p_time);

    private:
        Creature& m_owner;
};

typedef std::unordered_map<uint32, float /*SparringHealthLimit*/> CreatureSparringTemplateMap;

#endif
