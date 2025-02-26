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

#ifndef SCRIPTEDCREATURE_H_
#define SCRIPTEDCREATURE_H_

#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureAIImpl.h"
#include "InstanceScript.h"
#include "TaskScheduler.h"

#define CAST_AI(a, b)   (dynamic_cast<a*>(b))

class InstanceScript;

class SummonList
{
    public:
    typedef std::list<ObjectGuid> StorageType;
    typedef StorageType::iterator iterator;
    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::size_type size_type;
    typedef StorageType::value_type value_type;

    explicit SummonList(Creature* creature) : me(creature) { }

    // And here we see a problem of original inheritance approach. People started
    // to exploit presence of std::list members, so I have to provide wrappers

    iterator begin()
    {
        return storage_.begin();
    }

    const_iterator begin() const
    {
        return storage_.begin();
    }

    iterator end()
    {
        return storage_.end();
    }

    const_iterator end() const
    {
        return storage_.end();
    }

    iterator erase(iterator i)
    {
        return storage_.erase(i);
    }

    bool empty() const
    {
        return storage_.empty();
    }

    size_type size() const
    {
        return storage_.size();
    }

    void clear()
    {
        storage_.clear();
    }

    ObjectGuid front()
    {
        return storage_.front();
    }

    void Summon(Creature const* summon)
    {
        storage_.push_back(summon->GetGUID());
    }
    void Despawn(Creature const* summon)
    {
        storage_.remove(summon->GetGUID());
    }
    void DespawnEntry(uint32 entry);
    void DespawnAll(uint32 time = 0);

    template <typename T>
    void DespawnIf(T const &predicate)
    {
        storage_.remove_if(predicate);
    }

    void DoAction(uint32 entry, int32 info);

    template <class Predicate>
    void DoAction(int32 info, Predicate& predicate, uint16 max = 0)
    {
        // We need to use a copy of SummonList here, otherwise original SummonList would be modified
        StorageType listCopy = storage_;
        Trinity::Containers::RandomResizeList<ObjectGuid, Predicate>(listCopy, predicate, max);
        for (StorageType::iterator i = listCopy.begin(); i != listCopy.end();)
        {
            Creature* summon = Unit::GetCreature(*me, *i++);
            if (summon && summon->IsAIEnabled)
                summon->AI()->DoAction(info);
        }
    }

    void DoZoneInCombat(uint32 entry = 0);
    void RemoveNotExisting();
    bool HasEntry(uint32 entry) const;

    private:
    Creature* me;
    StorageType storage_;
};

class EntryCheckPredicate
{
    public:
        EntryCheckPredicate(uint32 entry) : _entry(entry) { }
        bool operator()(ObjectGuid guid) { return guid.GetEntry() == _entry; }

    private:
        uint32 _entry;
};

class DummyEntryCheckPredicate
{
    public:
    bool operator()(ObjectGuid)
    {
        return true;
    }
};

struct ScriptedAI : public CreatureAI
{
    explicit ScriptedAI(Creature* creature);
    virtual ~ScriptedAI() { }

    // *************
    //CreatureAI Functions
    // *************

    void AttackStartNoMove(Unit* target);

    //Called at World update tick
    virtual void UpdateAI(uint32 diff) override;

    // Called when AI is temporarily replaced or put back when possess is applied or removed
    virtual void OnPossess(bool /*apply*/) { }

    // *************
    // Variables
    // *************

    //Pointer to creature we are manipulating
    // Creature* me;

    //For fleeing
    bool IsFleeing;

    // *************
    //Pure virtual functions
    // *************

    // Called before JustEngagedWith even before the creature is in combat.
    void AttackStart(Unit* /*target*/) override;

    // *************
    //AI Helper Functions
    // *************

    //Start movement toward victim
    void DoStartMovement(Unit* target, float distance = 0.0f, float angle = 0.0f);

    //Start no movement on victim
    void DoStartNoMovement(Unit* target);

    //Stop attack of current victim
    void DoStopAttack();

    //Cast spell by spell info
    void DoCastSpell(Unit* target, SpellInfo const* spellInfo, bool triggered = false);

    //Plays a sound to all nearby players
    void DoPlaySoundToSet(WorldObject* source, uint32 soundId);

    //Drops all threat to 0%. Does not remove players from the threat list
    void DoResetThreat();

    float DoGetThreat(Unit* unit);
    void DoModifyThreatPercent(Unit* unit, int32 pct);

    void DoTeleportTo(float x, float y, float z, uint32 time = 0);
    void DoTeleportTo(float const pos [4]);

    //Teleports a player without dropping threat (only teleports to same map)
    void DoTeleportPlayer(Unit* unit, float x, float y, float z, float o);
    void DoTeleportAll(float x, float y, float z, float o);

    //Returns friendly unit with the most amount of hp missing from max hp
    Unit* DoSelectLowestHpFriendly(float range, uint32 minHPDiff = 1);

    //Returns a list of friendly CC'd units within range
    std::list<Creature*> DoFindFriendlyCC(float range);

    //Returns a list of all friendly units missing a specific buff within range
    std::list<Creature*> DoFindFriendlyMissingBuff(float range, uint32 spellId);

    //Return a player with at least minimumRange from me
    Player* GetPlayerAtMinimumRange(float minRange);

    //Spawns a creature relative to me
    Creature* DoSpawnCreature(uint32 entry, float offsetX, float offsetY, float offsetZ, float angle, uint32 type, uint32 despawntime);

    bool HealthBelowPct(uint32 pct) const
    {
        return me->HealthBelowPct(pct);
    }
    bool HealthAbovePct(uint32 pct) const
    {
        return me->HealthAbovePct(pct);
    }

    //Returns spells that meet the specified criteria from the creatures spell list
    SpellInfo const* SelectSpell(Unit* target, uint32 school, uint32 mechanic, SelectTargetType targets, uint32 powerCostMin, uint32 powerCostMax, float rangeMin, float rangeMax, SelectEffect effect);

    void SetEquipmentSlots(bool loadDefault, int32 mainHand = EQUIP_NO_CHANGE, int32 offHand = EQUIP_NO_CHANGE, int32 ranged = EQUIP_NO_CHANGE);

    // Used to control if MoveChase() is to be used or not in AttackStart(). Some creatures does not chase victims
    // NOTE: If you use SetCombatMovement while the creature is in combat, it will do NOTHING - This only affects AttackStart
    //       You should make the necessary to make it happen so.
    //       Remember that if you modified _isCombatMovementAllowed (e.g: using SetCombatMovement) it will not be reset at Reset().
    //       It will keep the last value you set.
    void SetCombatMovement(bool allowMovement);
    bool IsCombatMovementAllowed() const
    {
        return _isCombatMovementAllowed;
    }

    bool EnterEvadeIfOutOfCombatArea(uint32 const diff, const float distance = 100.0f);

    // return true for heroic mode. i.e.
    //   - for dungeon in mode 10-heroic,
    //   - for raid in mode 10-Heroic
    //   - for raid in mode 25-heroic
    // DO NOT USE to check raid in mode 25-normal.
    bool IsHeroic() const
    {
        return _isHeroic;
    }

    // return the dungeon or raid difficulty or LFR.
    Difficulty GetDifficulty() const
    {
        return _difficulty;
    }

    // return true for 25 man or 25 man heroic mode
    bool Is25ManRaid() const
    {
        return (_difficulty == RAID_DIFFICULTY_25MAN_NORMAL || _difficulty == RAID_DIFFICULTY_25MAN_HEROIC || _difficulty == RAID_DIFFICULTY_25MAN_LFR);
    }

    template<class T> inline
    const T& DUNGEON_MODE(const T& normal5, const T& heroic5) const
    {
        switch (_difficulty)
        {
            case DUNGEON_DIFFICULTY_NORMAL:
                return normal5;
            case DUNGEON_DIFFICULTY_HEROIC:
                return heroic5;
            default:
                break;
        }

        return normal5; // Return Normal Difficulty spell by default.
    }

    template<class T> inline
    const T& DUNGEON_MODE(const T& normal5, const T& heroic10, const T& challenge5) const
    {
        switch (_difficulty)
        {
            case DUNGEON_DIFFICULTY_NORMAL:
                return normal5;
            case DUNGEON_DIFFICULTY_HEROIC:
                return heroic10;
            case DUNGEON_DIFFICULTY_CHALLENGE:
                return challenge5;
            default:
                break;
        }

        return normal5; // Return Normal Difficulty spell by default.
    }

    template<class T> inline
    const T& SCENARIO_MODE(const T& normal5, const T& heroic5) const
    {
        switch (_difficulty)
        {
            case SCENARIO_DIFFICULTY_NORMAL:
                return normal5;
            case SCENARIO_DIFFICULTY_HEROIC:
                return heroic5;
            default:
                break;
        }

        return normal5; // Return Normal Difficulty spell by default.
    }

    template<class T> inline
        const T& RAID_MODE(const T& normal10, const T& normal25) const
    {
        switch (_difficulty)
        {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                return normal10;
            case RAID_DIFFICULTY_25MAN_NORMAL:
                return normal25;
            default:
                break;
        }

        return normal10; // Return Normal Difficulty spell by default.
    }

    template<class T> inline
        const T& RAID_MODE(const T& normal10, const T& normal25, const T& heroic10, const T& heroic25) const
    {
        switch (_difficulty)
        {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                return normal10;
            case RAID_DIFFICULTY_25MAN_NORMAL:
                return normal25;
            case RAID_DIFFICULTY_10MAN_HEROIC:
                return heroic10;
            case RAID_DIFFICULTY_25MAN_HEROIC:
                return heroic25;
            default:
                break;
        }

        return normal10; // Return Normal Difficulty spell by default.
    }

    template<class T> inline
    const T& RAID_MODE(const T& normal10, const T& normal25, const T& heroic10, const T& heroic25, const T& lfr) const
    {
        switch (_difficulty)
        {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                return normal10;
            case RAID_DIFFICULTY_25MAN_NORMAL:
                return normal25;
            case RAID_DIFFICULTY_10MAN_HEROIC:
                return heroic10;
            case RAID_DIFFICULTY_25MAN_HEROIC:
                return heroic25;
            case RAID_DIFFICULTY_25MAN_LFR:
                return lfr;
            default:
                break;
        }

        return normal10; // Return Normal Difficulty spell by default.
    }

    template<class T> inline
    const T& RAID_MODE(const T& normal10, const T& normal25, const T& heroic10, const T& heroic25, const T& flex, const T& lfr) const
    {
        switch (_difficulty)
        {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                return normal10;
            case RAID_DIFFICULTY_25MAN_NORMAL:
                return normal25;
            case RAID_DIFFICULTY_10MAN_HEROIC:
                return heroic10;
            case RAID_DIFFICULTY_25MAN_HEROIC:
                return heroic25;
            case RAID_DIFFICULTY_1025MAN_FLEX:
                return flex;
            case RAID_DIFFICULTY_25MAN_LFR:
                return lfr;
            default:
                break;
        }

        return normal10; // Return Normal Difficulty spell by default.
    }

    private:
    Difficulty _difficulty;
    uint32 _evadeCheckCooldown;
    bool _isCombatMovementAllowed;
    bool _isHeroic;
};

class BossAI : public ScriptedAI
{
    public:
        BossAI(Creature* creature, uint32 bossId);
        virtual ~BossAI() { }

        InstanceScript* const instance;

        BossBoundaryMap const* GetBoundary() const
        {
            return _boundary;
        }

        void JustSummoned(Creature* summon) override;
        void SummonedCreatureDespawn(Creature* summon) override;

        virtual void UpdateAI(uint32 diff) override;

        // Hook used to execute events scheduled into EventMap without the need
        // to override UpdateAI
        // note: You must re-schedule the event within this method if the event
        // is supposed to run more than once
        virtual void ExecuteEvent(uint32 /*eventId*/) { }

        virtual void ScheduleTasks() { }

        void Reset() override { _Reset(); }
        void JustEngagedWith(Unit* /*who*/) override { _JustEngagedWith(); }
        void JustDied(Unit* /*killer*/) override { _JustDied(); }
        void JustReachedHome() override { _JustReachedHome(); }


    protected:
        void _Reset();
        bool _JustEngagedWith();
        void _JustDied();
        void _JustReachedHome();
        void _DespawnAtEvade();

        bool CheckInRoom()
        {
            if (CheckBoundary(me))
                return true;

            EnterEvadeMode();
            return false;
        }

        bool CheckInArea(const uint32 diff, uint32 areaId)
        {
            if (_checkareaTimer <= diff)
                _checkareaTimer = 3000;
            else
            {
                _checkareaTimer -= diff;
                return true;
            }

            if (me->GetAreaId() != areaId)
            {
                EnterEvadeMode();
                return false;
            }

            return true;
        }

        bool CheckBoundary(Unit* who);
        void TeleportCheaters();

        EventMap events;
        mutable SummonList summons;
        TaskScheduler scheduler;

        private:
        BossBoundaryMap const* const _boundary;
        uint32 const _bossId;
        uint32 _checkareaTimer;
};

class WorldBossAI : public ScriptedAI
{
    public:
        WorldBossAI(Creature* creature);
        virtual ~WorldBossAI() { }

        void JustSummoned(Creature* summon) override;
        void SummonedCreatureDespawn(Creature* summon) override;

        virtual void UpdateAI(uint32 diff) override;

        // Hook used to execute events scheduled into EventMap without the need
        // to override UpdateAI
        // note: You must re-schedule the event within this method if the event
        // is supposed to run more than once
        virtual void ExecuteEvent(uint32 /*eventId*/) { }

        void Reset() override { _Reset(); }
        void JustEngagedWith(Unit* /*who*/) override { _JustEngagedWith(); }
        void JustDied(Unit* /*killer*/) override { _JustDied(); }

    protected:
        void _Reset();
        void _JustEngagedWith();
        void _JustDied();

        EventMap events;
        SummonList summons;
};

class CasterMovement
{
    public:
        CasterMovement(Creature* me, uint32 updateInterval = 500) : _me(me), _updateInterval(updateInterval) { }

        virtual void Chase(Unit* target);
        virtual void Update(uint32 diff, Unit* victim = NULL);

        bool IsChasingInMelee() const { return _chasingInMelee; }
        bool IsUnableToCast() const { return _unableToCast; }

        CasterMovement& Spell(uint32 spellId);
        CasterMovement& MeleeDistance(float distance) { _meleeDistance = distance; return *this; }
        CasterMovement& CasterDistance(float distance) { _casterDistance = distance; return *this; }
        CasterMovement& CasterDistanceBySpell(uint32 spellId, bool onlyCloser = false);
        CasterMovement& MeleeWhenUnableToCast(bool on = true) { _meleeWhenUnableToCast = on; return *this; }

    protected:
        Creature* _me;
        uint32 _updateInterval;

        std::vector<uint32> _spells;
        float _meleeDistance = 0.0f;
        float _casterDistance = 30.0f;
        bool _meleeWhenUnableToCast = true;

        uint32 _movementUpdateTimer = 0;
        bool _chasingInMelee = false;
        bool _unableToCast = false;

        void UpdateMovement(Unit* target, bool force = false);
};

struct SpellDummyAI : public ScriptedAI
{
    SpellDummyAI(Creature* creature, bool canMove = false)
        : ScriptedAI(creature)
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
        if (!canMove)
        {
            me->StopMoving();
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
        }
    }

    void DamageTaken(Unit*, uint32& damage) override
    {
        damage = 0;
    }

    void UpdateAI(uint32) override { }
    void EnterEvadeMode() override { }
};

enum TargetPriority
{
    PRIORITY_VICTIM,
    PRIORITY_NOT_VICTIM,
    PRIORITY_CHANNELED,
    PRIORITY_SELF,
    PRIORITY_ALLY,
};

struct customCreatureAI : public ScriptedAI
{
    customCreatureAI(Creature* creature) : ScriptedAI(creature), summons(me), isCaster(false) { }

    virtual ObjectGuid GetLowestFriendGUID() { return ObjectGuid::Empty; }

    void ExecuteTargetEvent(uint32 spell_id, uint32 repeat, uint32 event_id, uint32 curEventId, TargetPriority pTarget = PRIORITY_VICTIM)
    {
        if (curEventId != event_id)
            return;

        bool triggered = false;

        switch (spell_id)
        {
            case 129040: // Sinister Strike
                triggered = true;
                break;
        }

        if (isCaster)
        {
            me->StopMoving();

            if (Unit* vict = me->GetVictim())
            {
                if (me->GetExactDist2d(vict) > 45.0f)
                    DoStartMovement(vict);
                else
                    DoStartNoMovement(vict);
            }
        }

        switch (pTarget)
        {
            case PRIORITY_VICTIM:
                if (Unit* target = me->GetVictim())
                    DoCast(target, spell_id, triggered);
                break;
            case PRIORITY_NOT_VICTIM:
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                    DoCast(target, spell_id, triggered);
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                    DoCast(target, spell_id, triggered);
                break;
            case PRIORITY_CHANNELED:
                if (Unit* target = me->GetVictim())
                    targetGUID = target->GetGUID();
            
                if (SpellInfo const* sInfo = sSpellMgr->GetSpellInfo(spell_id))
                {
                    me->PrepareChanneledCast(me->GetOrientation(), spell_id);
            
                    // Some cone spells working like cast + aura
                    uint32 delay = sInfo->GetMaxDuration() ? sInfo->GetMaxDuration() + sInfo->CastTimeEntry->CastTime + 500 : sInfo->CastTimeEntry->CastTime + 500;

                    if (spell_id == 147646) // rending swipe
                        delay = 3000;

                    me->m_Events.Schedule(delay, 1, [this]()
                    {
                        // Prevent if evade
                        if (me->IsInCombat())
                            me->RemoveChanneledCast(targetGUID);
                        else
                            me->SetReactState(REACT_AGGRESSIVE);
                    });
                }
                break;
            case PRIORITY_SELF:
                DoCast(me, spell_id, triggered);
                break;
            case PRIORITY_ALLY:
                if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendGUID()))
                    DoCast(target, spell_id);
                break;
        }
        me->_AddCreatureSpellCooldown(spell_id, time(nullptr) + 2000 / IN_MILLISECONDS); // we need to add GCD  and does account for creatures with many executes.
        if (repeat)
            events.ScheduleEvent(event_id, repeat);
    }

    protected:
        EventMap events, nonCombatEvents;
        SummonList summons;
        bool isCaster;
        ObjectGuid targetGUID = ObjectGuid::Empty;

};

class AssistBehavior
{
public:
    AssistBehavior(Creature* c) : me(c) { }

    void Update(uint32 diff);
private:
    uint32 m_assistTimer = 0;
    ObjectGuid m_assistTarget = ObjectGuid::Empty;
    Creature* me;
};

// SD2 grid searchers.
Creature* GetClosestCreatureWithEntry(WorldObject* source, uint32 entry, float maxSearchRange, bool alive = true);
GameObject* GetClosestGameObjectWithEntry(WorldObject* source, uint32 entry, float maxSearchRange);
void GetCreatureListWithEntryInGrid(std::list<Creature*>& list, WorldObject* source, uint32 entry, float maxSearchRange);
void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& list, WorldObject* source, uint32 entry, float maxSearchRange);
void GetPlayerListInGrid(std::list<Player*>& list, WorldObject* source, float maxSearchRange);
void GetPositionWithDistInOrientation(Unit* pUnit, float dist, float orientation, float& x, float& y);
void GetRandPosFromCenterInDist(float centerX, float centerY, float dist, float& x, float& y);
Player* GetFarthestPlayerInArea(WorldObject* owner, float range);

#endif // SCRIPTEDCREATURE_H_
