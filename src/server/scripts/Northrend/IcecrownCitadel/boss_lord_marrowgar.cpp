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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "icecrown_citadel.h"
#include "Random.h"

enum ScriptTexts
{
    SAY_ENTER_ZONE              = 0,
    SAY_AGGRO                   = 1,
    SAY_BONE_STORM              = 2,
    SAY_BONESPIKE               = 3,
    SAY_KILL                    = 4,
    SAY_DEATH                   = 5,
    SAY_BERSERK                 = 6,
    EMOTE_BONE_STORM            = 7,
};

enum Spells
{
    // Lord Marrowgar
    SPELL_BONE_SLICE            = 69055,
    SPELL_BONE_STORM            = 69076,
    SPELL_BONE_SPIKE_GRAVEYARD  = 69057,
    SPELL_COLDFLAME_NORMAL      = 69140,
    SPELL_COLDFLAME_BONE_STORM  = 72705,

    // Bone Spike
    SPELL_IMPALED               = 69065,
    SPELL_RIDE_VEHICLE          = 46598,

    // Coldflame
    SPELL_COLDFLAME_PASSIVE     = 69145,
    SPELL_COLDFLAME_SUMMON      = 69147,
};

uint32 const BoneSpikeSummonId[3] = {69062, 72669, 72670};

enum Events
{
    EVENT_BONE_SPIKE_GRAVEYARD  = 1,
    EVENT_COLDFLAME             = 2,
    EVENT_BONE_STORM_BEGIN      = 3,
    EVENT_BONE_STORM_MOVE       = 4,
    EVENT_BONE_STORM_END        = 5,
    EVENT_ENABLE_BONE_SLICE     = 6,
    EVENT_ENRAGE                = 7,
    EVENT_WARN_BONE_STORM       = 8,

    EVENT_COLDFLAME_TRIGGER     = 9,
    EVENT_FAIL_BONED            = 10,
    EVENT_COMBAT_PULSE          = 11,
    EVENT_COLDFLAME_BONE_STORM  = 12,

    EVENT_GROUP_SPECIAL         = 1,
};

enum MovementPoints
{
    POINT_TARGET_BONESTORM_PLAYER   = 36612631,
    POINT_TARGET_COLDFLAME          = 36672631,
};

enum MiscInfo
{
    DATA_COLDFLAME_GUID             = 0,
    DATA_SPIKE_IMMUNE               = 1,
    DATA_COLDFLAME_SUMMON_SPELL     = 2,

    MAX_BONE_SPIKE_IMMUNE           = 3,
};

enum Actions
{
    ACTION_CLEAR_SPIKE_IMMUNITIES = 1,
    ACTION_TALK_ENTER_ZONE        = 2
};

class BoneSpikeTargetSelector 
{
    public:
        BoneSpikeTargetSelector(UnitAI* ai) : _ai(ai) { }

        bool operator()(Unit* unit) const
        {
            if (unit->GetTypeId() != TYPEID_PLAYER)
                return false;

            if (unit->HasAura(SPELL_IMPALED))
                return false;

            for (uint32 i = 0; i < MAX_BONE_SPIKE_IMMUNE; ++i)
                if (unit->GetGUID() == _ai->GetGUID(DATA_SPIKE_IMMUNE + i))
                    return false;

            return true;
        }

    private:
        UnitAI* _ai;
};

class BoneStormTargetSelector 
{
    public:
        BoneStormTargetSelector(WorldObject const* source, bool ignoreDistance) : _source(source), _ignoreDistance(ignoreDistance) { }

        bool operator()(Unit* unit) const
        {
            return unit->GetTypeId() == TYPEID_PLAYER && unit->IsAlive() && unit->GetPositionX() < -337.0f && (_ignoreDistance || _source->GetDistance2d(unit) > 8.0f);
        }

    private:
        WorldObject const* _source;
        bool _ignoreDistance;
};

class boss_lord_marrowgar : public CreatureScript
{
    public:
        boss_lord_marrowgar() : CreatureScript("boss_lord_marrowgar") { }

        struct boss_lord_marrowgarAI : public BossAI
        {
            boss_lord_marrowgarAI(Creature* creature) : BossAI(creature, DATA_LORD_MARROWGAR)
            {
                _boneStormDuration = RAID_MODE<uint32>(20000, 30000, 20000, 30000);
                _baseSpeed = creature->GetSpeedRate(MOVE_RUN);
                _coldflameLastPos.Relocate(creature);
                _introDone = false;
                _boneSlice = false;
            }

            void Reset() override
            {
                _Reset();
                me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                me->RemoveAurasDueToSpell(SPELL_BONE_STORM);
                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                events.ScheduleEvent(EVENT_ENABLE_BONE_SLICE, 10000);
                events.ScheduleEvent(EVENT_BONE_SPIKE_GRAVEYARD, 15000, EVENT_GROUP_SPECIAL);
                events.ScheduleEvent(EVENT_COLDFLAME, 5000, EVENT_GROUP_SPECIAL);
                events.ScheduleEvent(EVENT_WARN_BONE_STORM, 40000);
                events.ScheduleEvent(EVENT_ENRAGE, 600000);
                _boneSlice = false;
                _boneSpikeImmune.clear();
                _coldflameSummonSpell = 0;

                std::list<Creature*> spikes;
                GetCreatureListWithEntryInGrid(spikes, me, NPC_BONE_SPIKE, 100.0f);
                GetCreatureListWithEntryInGrid(spikes, me, NPC_BONE_SPIKE_2, 100.0f);
                GetCreatureListWithEntryInGrid(spikes, me, NPC_BONE_SPIKE_3, 100.0f);
                for (auto&& spike : spikes)
                    spike->DespawnOrUnsummon();

                me->GetMap()->SetWorldState(WORLDSTATE_BONED, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_LORD_MARROWGAR, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(LIGHT_S_HAMMER_TELEPORT);
                    return;
                }

                Talk(SAY_AGGRO);

                me->setActive(true);
                events.ScheduleEvent(EVENT_COMBAT_PULSE, 3000);
                instance->SetBossState(DATA_LORD_MARROWGAR, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();
            }

            void JustReachedHome() override
            {
                _JustReachedHome();
                instance->SetBossState(DATA_LORD_MARROWGAR, FAIL);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->GetExactDist(who) <= 5.0f)
                    BossAI::MoveInLineOfSight(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BONE_SPIKE_GRAVEYARD:
                        {
                            if (me->HasUnitState(UNIT_STATE_CASTING))
                            {
                                events.RescheduleEvent(EVENT_BONE_SPIKE_GRAVEYARD, 100, EVENT_GROUP_SPECIAL);
                                break;
                            }

                            if (!instance->instance->IsHeroic() && me->GetThreatManager().GetThreatListSize() == 1)
                                break;

                            DoCast(me, SPELL_BONE_SPIKE_GRAVEYARD);

                            events.RescheduleEvent(EVENT_BONE_SPIKE_GRAVEYARD, urand(15000, 20000), EVENT_GROUP_SPECIAL);
                            break;
                        }
                        case EVENT_COLDFLAME:
                            _coldflameLastPos.Relocate(me);
                            _coldflameTarget = ObjectGuid::Empty;

                            DoCastAOE(SPELL_COLDFLAME_NORMAL, true);

                            events.ScheduleEvent(EVENT_COLDFLAME, 5000, EVENT_GROUP_SPECIAL);
                            break;
                        case EVENT_COLDFLAME_BONE_STORM:
                            _coldflameLastPos.Relocate(me);
                            _coldflameTarget = ObjectGuid::Empty;

                            DoCast(me, SPELL_COLDFLAME_BONE_STORM, true);
                            break;
                        case EVENT_WARN_BONE_STORM:
                            _boneSlice = false;
                            Talk(EMOTE_BONE_STORM);

                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                            me->FinishSpell(CURRENT_MELEE_SPELL, false);
                            DoCast(me, SPELL_BONE_STORM);

                            events.CancelEvent(EVENT_COLDFLAME);
                            events.ScheduleEvent(EVENT_BONE_STORM_BEGIN, 3050);
                            break;
                        case EVENT_BONE_STORM_BEGIN:
                            if (!me->HasAura(SPELL_BONE_STORM))
                                DoCast(me, SPELL_BONE_STORM, true); // Forcibly add aura if it wasn't applied by now for some reason

                            if (Aura* pStorm = me->GetAura(SPELL_BONE_STORM))
                                pStorm->SetDuration(int32(_boneStormDuration));

                            me->SetSpeed(MOVE_RUN, _baseSpeed*3.0f, true);

                            Talk(SAY_BONE_STORM);

                            if (!IsHeroic())
                                events.CancelEvent(EVENT_BONE_SPIKE_GRAVEYARD);
                            events.ScheduleEvent(EVENT_BONE_STORM_END, _boneStormDuration+1);
                            events.ScheduleEvent(EVENT_WARN_BONE_STORM, 85000);
                            // no break here
                        case EVENT_BONE_STORM_MOVE:
                        {
                            events.ScheduleEvent(EVENT_BONE_STORM_MOVE, _boneStormDuration/4);

                            std::list<Unit*> targetList;
                            // Select all targets inside the room farther than 8 yards from the boss
                            SelectTargetList(targetList, BoneStormTargetSelector(me, false), 100, SELECT_TARGET_FARTHEST);
                            // If no target was found farther than 8 yards - select among all targets
                            if (targetList.empty())
                                SelectTargetList(targetList, BoneStormTargetSelector(me, true), 100, SELECT_TARGET_FARTHEST);
                            // If no target was found at all - just skip this step
                            if (targetList.empty())
                                break;

                            // Select farthermost target with 30% chance, if fails - select second-most farther target with 30% chance, and so on
                            Unit* unit = nullptr;
                            targetList.sort(Trinity::ObjectDistanceOrderPred(me, false));
                            for (std::list<Unit*>::const_iterator itr = targetList.begin(); itr != targetList.end() && !unit; ++itr)
                                if (roll_chance_i(30))
                                    unit = *itr;

                            // If randomized selection fails - just pick a random target
                            if (!unit)
                                unit = Trinity::Containers::SelectRandomContainerElement(targetList);

                            if (unit)
                                me->GetMotionMaster()->MovePoint(POINT_TARGET_BONESTORM_PLAYER, unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ());
                            break;
                        }
                        case EVENT_BONE_STORM_END:
                            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
                                me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            me->SetSpeed(MOVE_RUN, _baseSpeed, true);

                            me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);

                            events.CancelEvent(EVENT_BONE_STORM_MOVE);
                            events.ScheduleEvent(EVENT_ENABLE_BONE_SLICE, 10000);
                            events.ScheduleEvent(EVENT_COLDFLAME, 5000, EVENT_GROUP_SPECIAL);
                            if (!IsHeroic())
                                events.RescheduleEvent(EVENT_BONE_SPIKE_GRAVEYARD, urand(15000, 20000), EVENT_GROUP_SPECIAL);
                            break;
                        case EVENT_ENABLE_BONE_SLICE:
                            _boneSlice = true;
                            break;
                        case EVENT_ENRAGE:
                            DoCast(me, SPELL_BERSERK, true);
                            Talk(SAY_BERSERK);
                            break;
                        case EVENT_COMBAT_PULSE:
                            DoZoneInCombat();
                            events.ScheduleEvent(EVENT_COMBAT_PULSE, 3000);
                            break;
                    }
                }

                // We should not melee attack when storming
                if (me->HasAura(SPELL_BONE_STORM))
                    return;

                // 10 seconds since encounter start Bone Slice replaces melee attacks
                if (_boneSlice && !me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_BONE_SLICE);

                DoMeleeAttackIfReady();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || pointId != POINT_TARGET_BONESTORM_PLAYER)
                    return;

                // lock movement
                me->GetMotionMaster()->MoveIdle();
                //DoStartNoMovement(me->GetVictim());
                events.RescheduleEvent(EVENT_COLDFLAME_BONE_STORM, 1000);
            }

            Position const* GetLastColdflamePosition() const
            {
                return &_coldflameLastPos;
            }

            ObjectGuid GetGUID(int32 type /* = 0 */) const override
            {
                switch (type)
                {
                    case DATA_COLDFLAME_GUID:
                        return _coldflameTarget;
                    case DATA_SPIKE_IMMUNE + 0:
                    case DATA_SPIKE_IMMUNE + 1:
                    case DATA_SPIKE_IMMUNE + 2:
                    {
                        uint32 index = uint32(type - DATA_SPIKE_IMMUNE);
                        if (index < _boneSpikeImmune.size())
                            return _boneSpikeImmune[index];

                        break;
                    }
                }

                return ObjectGuid::Empty;
            }

            void SetGUID(ObjectGuid guid, int32 type /* = 0 */) override
            {
                switch (type)
                {
                    case DATA_COLDFLAME_GUID:
                        _coldflameTarget = guid;
                        break;
                    case DATA_SPIKE_IMMUNE:
                        _boneSpikeImmune.push_back(guid);
                        break;
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_COLDFLAME_SUMMON_SPELL:
                        _coldflameSummonSpell = data;
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_COLDFLAME_SUMMON_SPELL:
                        return _coldflameSummonSpell;
                }

                return 0;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_CLEAR_SPIKE_IMMUNITIES:
                        _boneSpikeImmune.clear();
                        break;
                    case ACTION_TALK_ENTER_ZONE:
                        if (!_introDone && me->IsAlive())
                        {
                            Talk(SAY_ENTER_ZONE);
                            _introDone = true;
                        }
                        break;
                    default:
                        break;
                }
            }

        private:
            Position _coldflameLastPos;
            std::vector<ObjectGuid> _boneSpikeImmune;
            ObjectGuid _coldflameTarget;
            uint32 _coldflameSummonSpell;
            uint32 _boneStormDuration;
            float _baseSpeed;
            bool _introDone;
            bool _boneSlice;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<boss_lord_marrowgarAI>(creature);
        }
};

typedef boss_lord_marrowgar::boss_lord_marrowgarAI MarrowgarAI;

class npc_coldflame : public CreatureScript
{
    public:
        npc_coldflame() : CreatureScript("npc_coldflame") { }

        struct npc_coldflameAI : public ScriptedAI
        {
            npc_coldflameAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void IsSummonedBy(Unit* owner) override
            {
                if (owner->GetTypeId() != TYPEID_UNIT || !owner->GetAI())
                    return;

                float ang;
                bool omnidirection = true;
                switch (owner->GetAI()->GetData(DATA_COLDFLAME_SUMMON_SPELL))
                {
                    case 72701: ang = owner->GetOrientation() + M_PI / 4; break;
                    case 72702: ang = owner->GetOrientation() - M_PI / 4; break;
                    case 72703: ang = owner->GetOrientation() + 3 * M_PI / 4; break;
                    case 72704: ang = owner->GetOrientation() - 3 * M_PI / 4; break;
                    default: omnidirection = false; break;
                }

                Position pos;
                if (omnidirection)
                {
                    // omnidirection case
                    if (MarrowgarAI* marrowgarAI = CAST_AI(MarrowgarAI, owner->GetAI()))
                    {
                        Position const* ownerPos = marrowgarAI->GetLastColdflamePosition();
                        // Already set above by looking up which spell it was summoned with.
                        // The trigger may spawn in a different direction if the summoner is near a wall.
                        // If done the way it used to be, more than one trigger could be sent in the same direction.
                        //ang = ownerPos->GetAngle(me);
                        ang = Position::NormalizeOrientation(ang);
                        me->SetOrientation(ang);
                        pos = owner->GetNearPosition(2.5f, ang - owner->GetOrientation());
                    }
                }
                else
                {
                    // random target case
                    Player* target = ObjectAccessor::GetPlayer(*owner, owner->GetAI()->GetGUID(DATA_COLDFLAME_GUID));
                    if (!target)
                    {
                        me->DespawnOrUnsummon();
                        return;
                    }

                    ang = owner->GetAngle(target);
                    me->SetOrientation(ang);
                    pos = owner->GetNearPosition(owner->GetObjectSize() / 2.0f, ang - owner->GetOrientation());
                }

                //me->UpdateGroundPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ, 10.0f, 50.0f);
                me->UpdateGroundPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);

                me->NearTeleportTo(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), me->GetOrientation());
                if (omnidirection)
                    DoCast(SPELL_COLDFLAME_SUMMON);
                _events.ScheduleEvent(EVENT_COLDFLAME_TRIGGER, 500);
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);

                if (_events.ExecuteEvent() == EVENT_COLDFLAME_TRIGGER)
                {
                    Position newPos = me->GetNearPosition(5.0f, 0.0f);
                    //me->UpdateGroundPositionZ(newPos.GetPositionX(), newPos.GetPositionY(), newPos.m_positionZ, 10.0f, 50.0f);
                    me->UpdateGroundPositionZ(newPos.GetPositionX(), newPos.GetPositionY(), newPos.m_positionZ);
                    me->NearTeleportTo(newPos.GetPositionX(), newPos.GetPositionY(), newPos.GetPositionZ(), me->GetOrientation());
                    DoCast(SPELL_COLDFLAME_SUMMON);
                    _events.ScheduleEvent(EVENT_COLDFLAME_TRIGGER, 500);
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_coldflameAI>(creature);
        }
};

class npc_bone_spike : public CreatureScript
{
    public:
        npc_bone_spike() : CreatureScript("npc_bone_spike") { }

        struct npc_bone_spikeAI : public ScriptedAI
        {
            npc_bone_spikeAI(Creature* creature) : ScriptedAI(creature), _hasTrappedUnit(false)
            {
                me->ApplySpellImmune(0, IMMUNITY_ID, 49560, true); // after fix - grip redirect not  use mechanic grip
                me->ApplySpellImmune(0, IMMUNITY_ID, 33786, true); // exploit with cyclone
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                //ASSERT(creature->GetVehicleKit());
                SetCombatMovement(false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (TempSummon* summ = me->ToTempSummon())
                    if (Unit* trapped = summ->GetSummoner())
                        trapped->RemoveAurasDueToSpell(SPELL_IMPALED);

                me->DespawnOrUnsummon(1);
            }

            void KilledUnit(Unit* victim) override
            {
                me->DespawnOrUnsummon(1);
                victim->RemoveAurasDueToSpell(SPELL_IMPALED);
            }

            void PassengerBoarded(Unit* passenger, int8 seatId, bool apply) override
            {
                if (passenger && !apply)
                    KilledUnit(passenger);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(summoner, SPELL_IMPALED);
                summoner->CastSpell(me, SPELL_RIDE_VEHICLE, true);
                _events.ScheduleEvent(EVENT_FAIL_BONED, 8000);
                if (summoner->HasUnitState(UNIT_STATE_FOLLOW_FORMATION))
                    summoner->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
                _hasTrappedUnit = true;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!_hasTrappedUnit)
                    return;

                _events.Update(diff);

                if (_events.ExecuteEvent() == EVENT_FAIL_BONED)
                    me->GetMap()->SetWorldState(WORLDSTATE_BONED, 0);
            }

        private:
            EventMap _events;
            bool _hasTrappedUnit;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetIcecrownCitadelAI<npc_bone_spikeAI>(creature);
        }
};

class spell_marrowgar_coldflame : public SpellScriptLoader
{
    public:
        spell_marrowgar_coldflame() : SpellScriptLoader("spell_marrowgar_coldflame") { }

        class spell_marrowgar_coldflame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marrowgar_coldflame_SpellScript);

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                targets.clear();
                // select any unit but not the tank (by owners threatlist)
                Unit* target = GetCaster()->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 1, -GetCaster()->GetObjectSize(), true, -SPELL_IMPALED);
                if (!target)
                    target = GetCaster()->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true); // or the tank if its solo
                if (!target)
                    return;

                targets.push_back(target);
            }

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_marrowgar_coldflame_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_marrowgar_coldflame_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marrowgar_coldflame_SpellScript();
        }
};

class spell_marrowgar_coldflame_bonestorm : public SpellScriptLoader
{
    public:
        spell_marrowgar_coldflame_bonestorm() : SpellScriptLoader("spell_marrowgar_coldflame_bonestorm") { }

        class spell_marrowgar_coldflame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marrowgar_coldflame_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                for (uint8 i = 0; i < 4; ++i)
                    GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue() + i), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_marrowgar_coldflame_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marrowgar_coldflame_SpellScript();
        }
};

class spell_marrowgar_coldflame_bonestorm_triggered : public SpellScriptLoader
{
    public:
        spell_marrowgar_coldflame_bonestorm_triggered() : SpellScriptLoader("spell_marrowgar_coldflame_bonestorm_triggered") { }

        class spell_marrowgar_coldflame_bonestorm_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marrowgar_coldflame_bonestorm_triggered_SpellScript);

            void HandleHit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (UnitAI* ai = caster->GetAI())
                    {
                        // Set this data immediately before trigger summon
                        ai->SetData(DATA_COLDFLAME_SUMMON_SPELL, GetSpellInfo()->Id);
                        ai->SetGUID(GetSpell()->m_targets.GetObjectTargetGUID(), DATA_COLDFLAME_GUID);
                    }
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_marrowgar_coldflame_bonestorm_triggered_SpellScript::HandleHit, EFFECT_1, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marrowgar_coldflame_bonestorm_triggered_SpellScript();
        }
};

class spell_marrowgar_coldflame_damage : public SpellScriptLoader
{
    public:
        spell_marrowgar_coldflame_damage() : SpellScriptLoader("spell_marrowgar_coldflame_damage") { }

        class spell_marrowgar_coldflame_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_marrowgar_coldflame_damage_AuraScript);

            bool CanBeAppliedOn(Unit* target)
            {
                if (target->HasAura(SPELL_IMPALED))
                    return false;

                if (target->GetExactDist2d(GetOwner()) > GetSpellInfo()->Effects[EFFECT_0].CalcRadius())
                    return false;

                if (Aura* aur = target->GetAura(GetId()))
                    if (aur->GetOwner() != GetOwner())
                        return false;

               return true;
           }

            void Register() override
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_marrowgar_coldflame_damage_AuraScript::CanBeAppliedOn);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_marrowgar_coldflame_damage_AuraScript();
        }
};

class spell_marrowgar_bone_spike_graveyard : public SpellScriptLoader
{
    public:
        spell_marrowgar_bone_spike_graveyard() : SpellScriptLoader("spell_marrowgar_bone_spike_graveyard") { }

        class spell_marrowgar_bone_spike_graveyard_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marrowgar_bone_spike_graveyard_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                for (uint32 i = 0; i < 3; ++i)
                    if (!sSpellMgr->GetSpellInfo(BoneSpikeSummonId[i]))
                        return false;

                return true;
            }

            bool Load() override
            {
                return GetCaster()->GetTypeId() == TYPEID_UNIT && GetCaster()->IsAIEnabled;
            }

            SpellCastResult CheckCast()
            {
                return GetCaster()->GetAI()->SelectTarget(SELECT_TARGET_RANDOM, 0, BoneSpikeTargetSelector(GetCaster()->GetAI())) ? SPELL_CAST_OK : SPELL_FAILED_NO_VALID_TARGETS;
            }

            void HandleSpikes(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Creature* marrowgar = GetCaster()->ToCreature())
                {
                    CreatureAI* marrowgarAI = marrowgar->AI();
                    uint8 boneSpikeCount = uint8(GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1);

                    std::list<Unit*> targets;
                    marrowgarAI->SelectTargetList(targets, BoneSpikeTargetSelector(marrowgarAI), boneSpikeCount, SELECT_TARGET_RANDOM);
                    if (targets.empty())
                        return;

                    uint32 i = 0;
                    for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr, ++i)
                    {
                        Unit* target = *itr;
                        target->CastSpell(target, BoneSpikeSummonId[i], true);
                    }

                    marrowgarAI->Talk(SAY_BONESPIKE);
                }
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_marrowgar_bone_spike_graveyard_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_marrowgar_bone_spike_graveyard_SpellScript::HandleSpikes, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marrowgar_bone_spike_graveyard_SpellScript();
        }
};

class spell_marrowgar_bone_storm : public SpellScriptLoader
{
    public:
        spell_marrowgar_bone_storm() : SpellScriptLoader("spell_marrowgar_bone_storm") { }

        class spell_marrowgar_bone_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marrowgar_bone_storm_SpellScript);

            void RecalculateDamage()
            {
                float sqrt = std::max(sqrtf(GetHitUnit()->GetExactDist2d(GetCaster())), 2.5f);
                if (sqrt < 5.4f)
                    sqrt = sqrt / 2.5f;
                SetHitDamage(int32(GetHitDamage() / sqrt));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_marrowgar_bone_storm_SpellScript::RecalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marrowgar_bone_storm_SpellScript();
        }
};

class spell_marrowgar_bone_slice : public SpellScriptLoader
{
    public:
        spell_marrowgar_bone_slice() : SpellScriptLoader("spell_marrowgar_bone_slice") { }

        class spell_marrowgar_bone_slice_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_marrowgar_bone_slice_SpellScript);

            bool Load() override
            {
                _targetCount = 0;
                return true;
            }

            void ClearSpikeImmunities()
            {
                GetCaster()->GetAI()->DoAction(ACTION_CLEAR_SPIKE_IMMUNITIES);
            }

            void CountTargets(std::list<WorldObject*>& targets)
            {
                _targetCount = std::min<uint32>(targets.size(), GetSpellInfo()->MaxAffectedTargets);
            }

            void SplitDamage()
            {
                // Mark the unit as hit, even if the spell missed or was dodged/parried
                GetCaster()->GetAI()->SetGUID(GetHitUnit()->GetGUID(), DATA_SPIKE_IMMUNE);

                if (!_targetCount)
                    return; // This spell can miss all targets

                SetHitDamage(GetHitDamage() / _targetCount);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_marrowgar_bone_slice_SpellScript::ClearSpikeImmunities);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_marrowgar_bone_slice_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnHit += SpellHitFn(spell_marrowgar_bone_slice_SpellScript::SplitDamage);
            }

            uint32 _targetCount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_marrowgar_bone_slice_SpellScript();
        }
};

class at_lord_marrowgar_entrance : public AreaTriggerScript
{
    public:
        at_lord_marrowgar_entrance() : AreaTriggerScript("at_lord_marrowgar_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* lordMarrowgar = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_LORD_MARROWGAR)))
                    lordMarrowgar->AI()->DoAction(ACTION_TALK_ENTER_ZONE);

            return true;
        }

};

void AddSC_boss_lord_marrowgar()
{
    new boss_lord_marrowgar();
    new npc_coldflame();
    new npc_bone_spike();
    new spell_marrowgar_coldflame();
    new spell_marrowgar_coldflame_bonestorm();
    new spell_marrowgar_coldflame_bonestorm_triggered();
    new spell_marrowgar_coldflame_damage();
    new spell_marrowgar_bone_spike_graveyard();
    new spell_marrowgar_bone_storm();
    new spell_marrowgar_bone_slice();
    new at_lord_marrowgar_entrance();
}
