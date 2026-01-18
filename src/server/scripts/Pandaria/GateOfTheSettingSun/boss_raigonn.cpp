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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "gate_of_the_setting_sun.h"
#include "Vehicle.h"
#include "Spline.h"
#include "MoveSplineInit.h"
#include "PassiveAI.h"

enum eSpells
{
    // Raigonn
    SPELL_IMPERVIOUS_CARAPACE       = 107118,   
    SPELL_BATTERING_HEADBUTT_EMOTE  = 118685,
    SPELL_BATTERING_HEADBUTT        = 111671,
    SPELL_BATTERING_HEADBUTT_STUN   = 130772,
    SPELL_BROKEN_CARAPACE           = 111742,
    SPELL_BROKEN_CARAPACE_DAMAGE    = 107146,
    SPELL_FIXATE                    = 111723,
    SPELL_STOMP                     = 111728,

    // Protectorat
    SPELL_HIVE_MIND                 = 107314,

    // Engulfer
    SPELL_ENGULFING_WINDS           = 107274,

    // Swarm Bringer
    SPELL_SCREECHING_SWARM          = 111600,

    // Misc
    WORLDSTATE_MANTID_SWARM         = 12252,
};

enum ePhases
{
    PHASE_WEAK_SPOT     = 1,
    PHASE_VULNERABILITY = 2,
};

enum eActions
{
    ACTION_WEAK_SPOT_DEAD   = 1,
};

enum eEvents
{
    EVENT_RAIGONN_CHARGE           = 1,
    EVENT_RAIGON_MOVE_BACK         = 2,
    EVENT_SUMMON_PROTECTORAT       = 3,
    EVENT_SUMMON_ENGULFER          = 4,
    EVENT_SUMMON_SWARM_BRINGER     = 5,
    EVENT_FIXATE                   = 6,
    EVENT_FIXATE_STOP              = 7,
    EVENT_STOMP                    = 8,
    EVENT_BATTERING_HEADBUTT_EMOTE = 9,
    EVENT_BATTERING_HEADBUTT       = 10,
    EVENT_INITIALIZE               = 11,
};

enum eTalks
{
    TALK_WEAK_POINT = 0,
    TALK_SWARM      = 1,
    TALK_ENGULFER   = 2,
    TALK_PROTECT    = 3,
};

Position chargePos[4] =
{
    { 958.30f, 2386.92f, 297.43f, 0.0f },
    { 958.30f, 2458.59f, 300.29f, 0.0f },
    { 958.30f, 2241.68f, 296.10f, 0.0f },
    { 958.30f, 2330.15f, 296.18f, 0.0f }
};

class boss_raigonn : public CreatureScript
{
    public:
        boss_raigonn() : CreatureScript("boss_raigonn") { }

        struct boss_raigonnAI : public BossAI
        {
            boss_raigonnAI(Creature* creature) : BossAI(creature, DATA_RAIGONN), vehicle(creature->GetVehicleKit()) { }

            void Reset() override
            {
                _Reset();

                isInFight = false;
                me->SetReactState(REACT_PASSIVE);
                me->AddAura(SPELL_IMPERVIOUS_CARAPACE, me);
                me->SetFullHealth();

                Phase = PHASE_WEAK_SPOT;
                chargeEvents.Reset();
                scheduler.CancelAll();
                chargeEvents.ScheduleEvent(EVENT_RAIGONN_CHARGE, 1 * IN_MILLISECONDS);
                chargeEvents.ScheduleEvent(EVENT_INITIALIZE, 1 * IN_MILLISECONDS);

                if (instance)
                {
                    instance->SetData(DATA_RAIGONN, NOT_STARTED);
                    instance->SetData(DATA_ARTILLERY_STATE, NOT_STARTED);
                }

                me->GetMap()->SetWorldState(WORLDSTATE_MANTID_SWARM, 1);
            }
            
            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (instance->GetBossState(DATA_RIMOK) == DONE)
                {
                    if (!isInFight)
                    {
                        isInFight = true;
                        _JustEngagedWith();
                        Talk(TALK_WEAK_POINT);
                        me->setRegeneratingHealth(false);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                        events.ScheduleEvent(EVENT_SUMMON_PROTECTORAT, urand(15, 30) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SUMMON_ENGULFER, urand(15, 30) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SUMMON_SWARM_BRINGER, urand(15, 30) * IN_MILLISECONDS);

                        if (instance)
                        {
                            if (Creature* weakPoint = instance->instance->GetCreature(instance->GetGuidData(NPC_WEAK_SPOT)))
                            {
                                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, weakPoint);
                                weakPoint->SetFaction(16);
                                weakPoint->ClearUnitState(UNIT_STATE_UNATTACKABLE);
                                weakPoint->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            }

                            instance->SetBossState(DATA_RAIGONN, IN_PROGRESS);
                            instance->SetData(DATA_RAIGONN, IN_PROGRESS);
                            instance->SetData(DATA_ARTILLERY_STATE, IN_PROGRESS);
                        }
                    }
                }
                else
                    damage = 0;
            }

            void EnterEvadeMode() override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_RAIGONN, FAIL);
                    instance->SetData(DATA_RAIGONN, FAIL);

                    if (Creature* weakPoint = instance->instance->GetCreature(instance->GetGuidData(NPC_WEAK_SPOT)))
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, weakPoint);
                }

                BossAI::EnterEvadeMode();

                me->RemoveAurasDueToSpell(SPELL_BROKEN_CARAPACE);
                me->RemoveAurasDueToSpell(SPELL_BROKEN_CARAPACE_DAMAGE);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_WEAK_SPOT_DEAD)
                {
                    me->CastStop();
                    me->StopMoving();
                    me->SetReactState(REACT_AGGRESSIVE);
                    
                    events.Reset();
                    chargeEvents.Reset();
                    Phase = PHASE_VULNERABILITY;
                    events.ScheduleEvent(EVENT_FIXATE, 30 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_STOMP, 16 * IN_MILLISECONDS);

                    me->RemoveAurasDueToSpell(SPELL_IMPERVIOUS_CARAPACE);
                    me->CastSpell(me, SPELL_BROKEN_CARAPACE, true);
                    me->CastSpell(me, SPELL_BROKEN_CARAPACE_DAMAGE, true);

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        AttackStart(target);

                    if (instance)
                        instance->SetData(DATA_ARTILLERY_STATE, SPECIAL);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void RemoveWeakSpotPassengers()
            {
                if (Creature* weakPoint = instance->instance->GetCreature(instance->GetGuidData(NPC_WEAK_SPOT)))
                {
                    if (Vehicle* weakVehicle = weakPoint->GetVehicleKit())
                    {
                        weakVehicle->RemoveAllPassengers();

                        // client-side issue
                        scheduler
                            .Schedule(Milliseconds(500), [this](TaskContext context)
                        {
                            if (Creature* wPoint = instance->instance->GetCreature(instance->GetGuidData(NPC_WEAK_SPOT)))
                                me->CastSpell(wPoint->GetPositionX(), wPoint->GetPositionY(), wPoint->GetPositionZ(), SPELL_BATTERING_HEADBUTT, true);
                        });
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (uint32 eventId = chargeEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RAIGON_MOVE_BACK:
                        {
                            uint32 eventBrasierProgress = instance->GetBossState(DATA_RIMOK);
                            uint8 baseMovement = eventBrasierProgress != DONE ? 0 : 2;
                            Movement::MoveSplineInit init(me);
                            init.MoveTo(chargePos[baseMovement + 1].GetPositionX(), chargePos[baseMovement + 1].GetPositionY(), chargePos[baseMovement + 1].GetPositionZ());
                            init.SetVelocity(5.0f);
                            init.SetOrientationInversed();
                            init.Launch();

                            chargeEvents.ScheduleEvent(EVENT_BATTERING_HEADBUTT_EMOTE, me->GetSplineDuration());
                            chargeEvents.ScheduleEvent(EVENT_RAIGONN_CHARGE, me->GetSplineDuration() + 2);
                            break;
                        }
                        case EVENT_BATTERING_HEADBUTT_EMOTE:
                            me->CastSpell(me, SPELL_BATTERING_HEADBUTT_EMOTE, false);
                            break;
                        case EVENT_RAIGONN_CHARGE:
                        {
                            me->CastStop();
                            uint32 eventBrasierProgress = instance->GetBossState(DATA_RIMOK);
                            uint8 baseMovement = eventBrasierProgress != DONE ? 0 : 2;
                            Movement::MoveSplineInit init(me);
                            init.MoveTo(chargePos[baseMovement].GetPositionX(), chargePos[baseMovement].GetPositionY(), chargePos[baseMovement].GetPositionZ());
                            init.SetVelocity(37.0f);
                            init.Launch();

                            chargeEvents.ScheduleEvent(EVENT_BATTERING_HEADBUTT, me->GetSplineDuration());
                            break;
                        }
                        case EVENT_BATTERING_HEADBUTT:
                            RemoveWeakSpotPassengers();
                            chargeEvents.ScheduleEvent(EVENT_RAIGON_MOVE_BACK, 3 * IN_MILLISECONDS);
                            break;
                        case EVENT_INITIALIZE:
                            if (vehicle)
                            {
                                if (Creature* weakSpot = me->SummonCreature(NPC_WEAK_SPOT, *me))
                                {
                                    weakSpot->EnterVehicle(me, 0);

                                    if (instance)
                                        instance->SetData64(NPC_WEAK_SPOT, weakSpot->GetGUID());
                                }
                            }
                            break;
                    }
                }

                chargeEvents.Update(diff);
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUMMON_PROTECTORAT:
                            Talk(TALK_PROTECT);
                            for (uint8 i = 0; i < 8; ++i)
                                 if (Creature* summon = me->SummonCreature(NPC_KRIKTHIK_PROTECTORAT, frand(941.0f, 974.0f), 2374.85f, 296.67f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                                     if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                         if (summon->IsAIEnabled)
                                             summon->AI()->AttackStart(target);

                            events.ScheduleEvent(EVENT_SUMMON_PROTECTORAT, urand(30, 45) * IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMON_ENGULFER:
                            Talk(TALK_ENGULFER);
                            for (uint8 i = 0; i < 3; ++i)
                                me->SummonCreature(NPC_KRIKTHIK_ENGULFER, frand(941.0f, 974.0f), me->GetPositionY(), me->GetPositionZ() + 30.0f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);

                            events.ScheduleEvent(EVENT_SUMMON_ENGULFER, urand(95, 105) * IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMON_SWARM_BRINGER:
                            Talk(TALK_SWARM);
                            if (Creature* summon = me->SummonCreature(NPC_KRIKTHIK_SWARM_BRINGER, frand(941.0f, 974.0f), 2374.85f, 296.67f, 4.73f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000))
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                   if (summon->IsAIEnabled)
                                       summon->AI()->AttackStart(target);

                            events.ScheduleEvent(EVENT_SUMMON_ENGULFER, urand(35, 50) * IN_MILLISECONDS);
                            break;
                        case EVENT_FIXATE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            {
                                DoCast(target, SPELL_FIXATE);
                                AttackStart(target);
                                me->SetReactState(REACT_PASSIVE);
                            }

                            me->ClearUnitState(UNIT_STATE_CASTING);
                            me->SetReactState(REACT_PASSIVE);
                            events.ScheduleEvent(EVENT_FIXATE_STOP, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_FIXATE_STOP:
                            events.Reset();
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.ScheduleEvent(EVENT_FIXATE, 30 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_STOMP, urand(14, 20) * IN_MILLISECONDS);
                            break;
                        case EVENT_STOMP:
                            me->CastSpell(me, SPELL_STOMP, false);
                            break;
                    }  
                }

                if (Phase == PHASE_VULNERABILITY)
                    DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_RAIGONN, DONE);
                }
                summons.DespawnAll();
            }

            void GetPassengerEnterPosition(Unit*, int8, Position& pos) override
            {
                // Taken from sniffs (but it represented by a single sample)
                pos = { 11.7774f, 0.0f, 17.862f };
            }

        private:
            Vehicle* vehicle;
            bool isInFight;
            uint8 Phase;
            EventMap chargeEvents;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_raigonnAI(creature);
        }
};

class npc_raigonn_weak_spot : public CreatureScript
{
    public:
        npc_raigonn_weak_spot() : CreatureScript("npc_raigonn_weak_spot") { }

        struct npc_raigonn_weak_spotAI : public ScriptedAI
        {
            npc_raigonn_weak_spotAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->setRegeneratingHealth(false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                if (instance)
                    if (Creature* raigonn = Unit::GetCreature(*me, instance->GetGuidData(DATA_RAIGONN)))
                        if (raigonn->IsAIEnabled)
                            raigonn->AI()->DoAction(ACTION_WEAK_SPOT_DEAD);
            }

            // Just fucking bunch of hacks
            void GetPassengerEnterPosition(Unit* unit, int8, Position&) override
            {
                unit->SetDisableGravity(true);
            }

            void GetPassengerExitPosition(Unit* unit, int8, Position&) override
            {
                unit->SetDisableGravity(false);
            }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        { 
            return new npc_raigonn_weak_spotAI(creature);
        }
};

class npc_krikthik_protectorat : public CreatureScript
{
    public:
        npc_krikthik_protectorat() : CreatureScript("npc_krikthik_protectorat") { }

        struct npc_krikthik_protectoratAI : public ScriptedAI
        {
            npc_krikthik_protectoratAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoZoneInCombat();
                hasCastHiveMind = false;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!hasCastHiveMind && me->HealthBelowPctDamaged(20, damage))
                {
                    hasCastHiveMind = true;
                    me->CastSpell(me, SPELL_HIVE_MIND, true);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_MANTID_SWARM, 0);
            }

        private:
            bool hasCastHiveMind;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_krikthik_protectoratAI(creature);
        }
};

class npc_krikthik_engulfer : public CreatureScript
{
    public:
        npc_krikthik_engulfer() : CreatureScript("npc_krikthik_engulfer") { }

        enum eEvents
        {
            EVENT_ENGULFING_WINDS = 1
        };

        struct npc_krikthik_engulferAI : public ScriptedAI
        {
            npc_krikthik_engulferAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MoveRandom(25.0f);
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_ENGULFING_WINDS, urand(7.5, 12.5) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_MANTID_SWARM, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_ENGULFING_WINDS)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                        me->CastSpell(target, SPELL_ENGULFING_WINDS, false);
                    events.ScheduleEvent(EVENT_ENGULFING_WINDS, urand(7.5, 12.5) * IN_MILLISECONDS);
                }
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_krikthik_engulferAI(creature);
        }
};

class npc_krikthik_swarm_bringer : public CreatureScript
{
    public:
        npc_krikthik_swarm_bringer() : CreatureScript("npc_krikthik_swarm_bringer") { }

        enum eEvents
        {
            EVENT_SCREECHING_SWARM = 1
        };

        struct npc_krikthik_swarm_bringerAI : public ScriptedAI
        {
            npc_krikthik_swarm_bringerAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoZoneInCombat();
                events.ScheduleEvent(EVENT_SCREECHING_SWARM, urand(17.5, 22.5) * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_MANTID_SWARM, 0);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SCREECHING_SWARM)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                        me->CastSpell(target, SPELL_SCREECHING_SWARM, false);
                    events.ScheduleEvent(EVENT_SCREECHING_SWARM, urand(17.5, 22.5) * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_krikthik_swarm_bringerAI(creature);
        }
};

class vehicle_artillery : public CreatureScript
{
    public:
        vehicle_artillery() : CreatureScript("vehicle_artillery") { }

        struct vehicle_artilleryAI : public PassiveAI
        {
            vehicle_artilleryAI(Creature* creature) : PassiveAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                PassengerGUID = ObjectGuid::Empty;
            }

            ObjectGuid PassengerGUID;

            void OnSpellClick(Unit* clicker, bool&) override
            {
                PassengerGUID = clicker->GetGUID();
                Unit* artilery = me;
                me->m_Events.Schedule(1.5f * IN_MILLISECONDS, [=]()
                {
                    if (Creature* weakSpot = ObjectAccessor::GetCreature(*artilery, instance->GetGuidData(NPC_WEAK_SPOT)))
                    {
                        if (weakSpot->GetVehicleKit() && artilery->GetVehicleKit())
                        {
                            if (Unit* passenger = artilery->GetVehicleKit()->GetPassenger(0))
                            {
                                passenger->ExitVehicle();

                                if (uint32 availableSeatCount = weakSpot->GetVehicleKit()->GetAvailableSeatCount())
                                    passenger->EnterVehicle(weakSpot);
                            }
                        }
                    }
                });
            }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new vehicle_artilleryAI(creature);
        }
};

class npc_engulfing_winds : public CreatureScript
{
    public:
        npc_engulfing_winds() : CreatureScript("npc_engulfing_winds") { }

        enum eEvents
        {
            EVENT_RANDOM_MOVEMENT = 1
        };

        enum eSpells
        {
            SPELL_ENGULFING_WINDS = 107278
        };

        struct npc_engulfing_windsAI : public ScriptedAI
        {
            npc_engulfing_windsAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, SPELL_ENGULFING_WINDS, false);
                events.ScheduleEvent(EVENT_RANDOM_MOVEMENT, 2 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_RANDOM_MOVEMENT)
                {
                    me->GetMotionMaster()->MoveRandom(frand(13.f, 18.f));
                    events.ScheduleEvent(EVENT_RANDOM_MOVEMENT, urand(2, 4) * IN_MILLISECONDS);
                }
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_engulfing_windsAI(creature);
        }
};

class StompTargetSelector
{
    public:
        StompTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            if (Creature* cre = object->ToCreature())
                if (cre->GetEntry() == 59820 || cre->GetEntry() == 58844 || cre->GetEntry() == 58824)
                    return false;

            return true;
        }
};

class spell_raigonn_stomp : public SpellScriptLoader
{
    public:
        spell_raigonn_stomp() : SpellScriptLoader("spell_raigonn_stomp") { }

        class spell_raigonn_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_raigonn_stomp_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targetList)
            {
                targetList.remove_if (StompTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_raigonn_stomp_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_raigonn_stomp_SpellScript();
        }
};

class BatteringHeadbuttEffectTargetSelector
{
    public:
        BatteringHeadbuttEffectTargetSelector() { }

        bool operator()(Player* object)
        {
            return object
                && object->GetPositionY() > 2372.0f && object->GetPositionY() < 2360.0f
                && object->GetPositionX() > 997.0f  && object->GetPositionX() < 919.0f;
        }
};

// Battering Headbutt 111671
class spell_raigonn_battering_headbutt : public SpellScriptLoader
{
    public:
        spell_raigonn_battering_headbutt() : SpellScriptLoader("spell_raigonn_battering_headbutt") { }

        class spell_raigonn_battering_headbutt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_raigonn_battering_headbutt_SpellScript);

            void HandleSpellEffectHit(SpellEffIndex /*idx*/)
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<Player*> PlayersOnGates;
                    GetPlayerListInGrid(PlayersOnGates, caster, 100.0f);

                    PlayersOnGates.remove_if(BatteringHeadbuttEffectTargetSelector());

                    for (auto&& itr : PlayersOnGates)
                        if (!itr->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
                            itr->AddAura(SPELL_BATTERING_HEADBUTT_STUN, itr);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_raigonn_battering_headbutt_SpellScript::HandleSpellEffectHit, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_raigonn_battering_headbutt_SpellScript();
        }
};

void AddSC_boss_raigonn()
{
    new boss_raigonn();
    new npc_raigonn_weak_spot();
    new npc_krikthik_protectorat();
    new npc_krikthik_engulfer();
    new npc_krikthik_swarm_bringer();
    new vehicle_artillery();
    new npc_engulfing_winds();
    new spell_raigonn_stomp();
    new spell_raigonn_battering_headbutt();
}
