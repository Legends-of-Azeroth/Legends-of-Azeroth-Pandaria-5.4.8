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

#include "siege_of_niuzao_temple.h"

static const Position bridgePos[] = 
{
    {1888.998f, 5177.15f, 143.3173f, 0.0f},
    {1808.063f, 5251.847f, 142.6636f, 0.0f}
};

class boss_wing_leader_neronok : public CreatureScript
{
    enum Yells
    {
        SAY_AGGRO,
        SAY_FLIGHT,
        SAY_WIPE,
        SAY_DEATH,
        SAY_SLAY,
        EMOTE_WINDS
    };

    enum Spells
    {
        SPELL_HURL_BRICK        = 121762,
        SPELL_CAUSTIC_PITCH     = 121443,
        SPELL_QUICK_DRY_RESIN   = 121447,
        SPELL_GUSTING_WINDS     = 121282,
    };

    enum Events
    {
        EVENT_HAUL_BRICK        = 1,
        EVENT_CAUSTIC_PITCH,
        EVENT_QUICK_DRY_RESIN,
        EVENT_SWITCH_PHASE,
        EVENT_GUSTING_WINDS,
        EVENT_FLY_OTHER_END,
        EVENT_LAND,
        EVENT_RE_ENGAGE,
    };

    enum 
    {
        EVENT_GROUP_MOVEMENT        = 1,
        EVENT_GROUP_COMBAT          = 2,

        ACTION_INTERRUPT            = 1,
        NPC_CHUM_KIU                = 64517
    };

    public:
        boss_wing_leader_neronok() : CreatureScript("boss_wing_leader_neronok") { }

        struct boss_wing_leader_neronokAI : public BossAI
        {
            boss_wing_leader_neronokAI(Creature* creature) : BossAI(creature, BOSS_NERONOK) 
            { 
                SetCombatMovement(false);
                me->setActive(true);
            }

            uint8 phase, side;
            bool interrupted, HasFly;

            void Reset() override
            {
                interrupted = false;
                side        = 0;
                phase       = 0;
                HasFly      = false;
                SetCombatMovement(false);
                events.Reset();
                me->SetCanFly(false);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetSpeed(MOVE_FLIGHT, 5.5f);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                _Reset();
            }

            void ClearDebuffs()
            {
                if (AreaTrigger * trigger = me->GetAreaTrigger(SPELL_GUSTING_WINDS))
                    trigger->SetDuration(0);

                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_QUICK_DRY_RESIN);
                std::list<DynamicObject *> dList;
                me->GetDynObjectList(dList, 121443);

                for (auto&& itr : dList)
                    itr->SetDuration(0);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTERRUPT)
                {
                    interrupted = true;
                    events.CancelEvent(EVENT_GUSTING_WINDS);
                    if (AreaTrigger * trigger = me->GetAreaTrigger(SPELL_GUSTING_WINDS))
                        trigger->SetDuration(0);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (side < 2)
                {
                    uint8 threshold = side > 0 ? 40 : 70;
                    if (me->HealthBelowPctDamaged(threshold, damage) && !HasFly)
                    {
                        HasFly = true;
                        interrupted = false;
                        Talk(SAY_FLIGHT);
                        Talk(EMOTE_WINDS);
                        me->SetReactState(REACT_PASSIVE);
                        me->AttackStop();
                        me->SetTarget(ObjectGuid::Empty);
                        me->SetCanFly(true);
                        me->SendMovementFlagUpdate();
                        me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                        me->SetAnimTier(AnimTier::Hover);
                        me->SetDisableGravity(true);
                        me->GetMotionMaster()->MovePoint(0, bridgePos[phase]);
                        events.CancelEventGroup(EVENT_GROUP_COMBAT);
                        events.CancelEventGroup(EVENT_GROUP_MOVEMENT);
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        events.ScheduleEvent(EVENT_FLY_OTHER_END, 100, EVENT_GROUP_MOVEMENT);
                        break;
                    case 1:
                        side++;
                        break;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void JustReachedHome() override
            {
                Talk(SAY_WIPE);
                _JustReachedHome();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ClearDebuffs();
                BossAI::EnterEvadeMode();
            }

            void JustDied(Unit* /*killer*/) override
            {
                ClearDebuffs();
                Talk(SAY_DEATH);
                _JustDied();

                me->SummonCreature(NPC_CHUM_KIU, 1851.226f, 5214.163f, 131.2519f, 4.03f);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_HAUL_BRICK, 1000, EVENT_GROUP_COMBAT);
                events.ScheduleEvent(EVENT_CAUSTIC_PITCH, 3000, EVENT_GROUP_COMBAT);
                events.ScheduleEvent(EVENT_QUICK_DRY_RESIN, 8000, EVENT_GROUP_COMBAT);
                _JustEngagedWith();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HAUL_BRICK:
                            if (Unit*  victim = me->GetVictim())
                                if (!victim->IsWithinMeleeRange(me))
                                    DoCast(victim, SPELL_HURL_BRICK, false);

                            events.ScheduleEvent(EVENT_HAUL_BRICK, 2000, EVENT_GROUP_COMBAT);
                            break;
                        case EVENT_CAUSTIC_PITCH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                me->CastSpell(target->GetPositionX(), target->GetPositionY(), me->GetPositionZ(), SPELL_CAUSTIC_PITCH, true); // we won`t flying pools
                            }
                            //DoCast(SELECT_TARGET_RANDOM, SPELL_CAUSTIC_PITCH, false);
                            events.ScheduleEvent(EVENT_CAUSTIC_PITCH, 5000, EVENT_GROUP_COMBAT);
                            break;
                        case EVENT_QUICK_DRY_RESIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_QUICK_DRY_RESIN, false);

                            events.ScheduleEvent(EVENT_QUICK_DRY_RESIN, 8000, EVENT_GROUP_COMBAT);
                            break;
                        case EVENT_GUSTING_WINDS:
                            if (interrupted)
                                break;

                            DoCast(me, SPELL_GUSTING_WINDS, false);
                            events.ScheduleEvent(EVENT_GUSTING_WINDS, 10000, EVENT_GROUP_COMBAT);
                            break;
                        case EVENT_FLY_OTHER_END:
                            me->GetMotionMaster()->MovePoint(1, bridgePos[side > 0 ? --phase : ++phase]);
                            events.ScheduleEvent(EVENT_LAND, me->GetSplineDuration() + 500, EVENT_GROUP_MOVEMENT);
                            break;
                        case EVENT_LAND:
                        {
                            Position pos;
                            pos.Relocate(me);
                            pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), true);
                            me->GetMotionMaster()->MoveLand(2, pos);
                            events.ScheduleEvent(EVENT_RE_ENGAGE, me->GetSplineDuration() + 500, EVENT_GROUP_MOVEMENT);
                            break;
                        }
                        case EVENT_RE_ENGAGE:
                            HasFly = false;
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Unit* victim = me->GetVictim())
                            {
                                AttackStart(victim);
                                me->SetTarget(victim->GetGUID());
                            }

                            events.ScheduleEvent(EVENT_HAUL_BRICK, 1000, EVENT_GROUP_COMBAT);
                            events.ScheduleEvent(EVENT_CAUSTIC_PITCH, 3000, EVENT_GROUP_COMBAT);
                            events.ScheduleEvent(EVENT_QUICK_DRY_RESIN, 8000, EVENT_GROUP_COMBAT);
                            events.ScheduleEvent(EVENT_GUSTING_WINDS, 2000, EVENT_GROUP_COMBAT);
                            break;

                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_wing_leader_neronokAI(creature);
        }
};

// Quick-Dry Resin - 121447
class spell_quick_dry_resin : public SpellScriptLoader
{
    enum
    {
        SPELL_ENCASED_IN_RESIN      = 121448,
        SPELL_SCREEN_EFFECT         = 122063,
        SPELL_INVIGORATED           = 121449,
        SPELL_QUICK_DRY_RESIN       = 121447,
    };

    public:
        spell_quick_dry_resin() : SpellScriptLoader("spell_quick_dry_resin") { }

        class spell_quick_dry_resin_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_quick_dry_resin_AuraScript)

            void PeriodicTick(AuraEffect const * aurEff)
            {
                if (Unit* target = GetUnitOwner())
                {
                    // real value unknown
                    int32 powerAmt = target->GetPower(POWER_ALTERNATE_POWER);

                    if (powerAmt >= 100)
                    {
                        target->RemoveAurasDueToSpell(aurEff->GetId());
                        target->RemoveAurasDueToSpell(SPELL_SCREEN_EFFECT);
                        target->CastSpell(target, SPELL_ENCASED_IN_RESIN, true);
                        return;
                    }
                    else if (powerAmt >= 80)
                    {
                        if (!target->HasAura(SPELL_SCREEN_EFFECT))
                            target->CastSpell(target, SPELL_SCREEN_EFFECT, true);
                    }
                    else if (target->HasAura(SPELL_SCREEN_EFFECT) && powerAmt <= 10)
                    {
                        target->RemoveAurasDueToSpell(SPELL_SCREEN_EFFECT);
                        target->RemoveAurasDueToSpell(SPELL_QUICK_DRY_RESIN);
                        target->CastSpell(target, SPELL_INVIGORATED, true);
                        return;
                    }

                    target->SetPower(POWER_ALTERNATE_POWER, powerAmt + 5);
                }
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetUnitOwner();

                if (!caster || !target)
                    return;

                target->SetPower(POWER_ALTERNATE_POWER, 50);
                target->AddAura(SPELL_SCREEN_EFFECT, target);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetUnitOwner();

                if (!caster || !target)
                    return;

                target->RemoveAurasDueToSpell(SPELL_SCREEN_EFFECT);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_quick_dry_resin_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_quick_dry_resin_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_quick_dry_resin_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript * GetAuraScript() const
        {
            return new spell_quick_dry_resin_AuraScript();
        }
};

// Gusting Winds - 121282
class spell_neronok_gusting_winds : public SpellScriptLoader
{
    public:
        spell_neronok_gusting_winds() : SpellScriptLoader("spell_neronok_gusting_winds") { }

        class spell_neronok_gusting_winds_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_neronok_gusting_winds_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetUnitOwner())
                {
                    owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
                    owner->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                }
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetUnitOwner())
                {
                    owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
                    owner->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);

                    if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_CANCEL
                        && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                        return;
                    
                    if (Creature* creature = owner->ToCreature())
                        creature->AI()->DoAction(1);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_neronok_gusting_winds_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_neronok_gusting_winds_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_neronok_gusting_winds_AuraScript();
        }
};

// 343. Summoned by 121282 - Gusting Winds
class sat_neronok_gusting_winds : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                if (target->IsAlive() && !target->HasForcedMovement())
                    target->SendApplyMovementForce(true, *caster, -playerBaseMoveSpeed[MOVE_RUN]);
                else if (!target->IsAlive() && target->HasForcedMovement())
                    target->SendApplyMovementForce(false, *caster);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                if (target->HasForcedMovement())
                    target->SendApplyMovementForce(false, *caster);
    }
};


void AddSC_wing_leader_neronok()
{
    new boss_wing_leader_neronok();
    new spell_quick_dry_resin();
    new spell_neronok_gusting_winds();
    new atrigger_script<sat_neronok_gusting_winds>("sat_neronok_gusting_winds");
}