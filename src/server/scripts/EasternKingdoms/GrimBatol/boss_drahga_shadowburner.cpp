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

#include "grim_batol.h"
#include "Spell.h"
#include "ScriptPCH.h"
#include "Vehicle.h"

// todo: починить spell_flaming_fixate

enum ScriptTexts
{
    // drahga
    SAY_AGGRO                          = 0,
    SAY_KILL                           = 2,
    SAY_DEATH                          = 3,
    SAY_ADDS                           = 4,
    SAY_VALIONA                        = 5,
                                       
    // valiona                         
    SAY_ENTER                          = 0,
    SAY_LEAVE                          = 1,
};

enum Spells
{
    SPELL_BURNING_SHADOWBOLT           = 75245,
    SPELL_INVOCATION_OF_FLAME          = 75222,
    SPELL_FLAMING_FIXATE               = 82850,
    SPELL_INVOKED_FLAME                = 75235,
    SPELL_SUPERNOVA                    = 75238,
    SPELL_TWILIGHT_PROTECTION          = 76303,
    SPELL_TWILIGHT_SHIFT               = 75328,
    SPELL_SHREDDING_SWIPE              = 75271,
    SPELL_SEEPING_TWILIGHT_DUMMY       = 75318,
    SPELL_SEEPING_TWILIGHT             = 75274,
    SPELL_SEEPING_TWILIGHT_DMG         = 75317,
    SPELL_VALIONAS_FLAME               = 75321,
    SPELL_DEVOURING_FLAMES             = 90950,
};

enum Adds
{
    NPC_INVOCATION_OF_FLAME_STALKER    = 40355,
    NPC_INVOKED_FLAMING_SPIRIT         = 40357,
    NPC_VALIONA                        = 40320,
    NPC_SEEPING_TWILIGHT               = 40365,
    NPC_DEVOURING_FLAMES               = 48798,
};

enum Events
{
    EVENT_BURNING_SHADOWBOLT           = 1,
    EVENT_INVOCATION_OF_FLAME          = 2,
    EVENT_SELECT_TARGET                = 3,
    EVENT_VALIONAS_FLAME               = 4,
    EVENT_SHREDDING_SWIPE              = 5,
    EVENT_VALIONA_MOVE                 = 6,
};

enum Actions
{
    ACTION_VALIONA_LEAVE               = 1,
};

const Position drahgavalionaPos[3] =
{
    { -373.35f, -659.56f, 253.56f, 3.44f },
    { -373.35f, -659.56f, 282.26f, 3.44f },
    { -414.04f, -681.35f, 267.09f, 3.63f }
};

const Position DrahgaLeavePoint =  { -396.02f, -674.77f, 265.14f, 0.35f };
const Position ValioneLeavePoint = { -373.56f, -613.11f, 299.54f, 0.93f };

class boss_drahga_shadowburner : public CreatureScript
{
    public:
        boss_drahga_shadowburner() : CreatureScript("boss_drahga_shadowburner") { }

        struct boss_drahga_shadowburnerAI : public BossAI
        {
            boss_drahga_shadowburnerAI(Creature* creature) : BossAI(creature, DATA_DRAHGA_SHADOWBURNER)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->setActive(true);
            }

            uint8 stage;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->ExitVehicle();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                summons.DespawnAll();
                stage = 0;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    if (me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_BURNING_SHADOWBOLT)
                        for (uint8 i = 0; i < 3; ++i)
                            if (spell->Effects[i].Effect == SPELL_EFFECT_INTERRUPT_CAST)
                                me->InterruptSpell(CURRENT_GENERIC_SPELL);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->GetVehicle())
                    damage = 0;
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_BURNING_SHADOWBOLT, urand(2000, 5000));
                events.ScheduleEvent(EVENT_INVOCATION_OF_FLAME, 10000);
            }

            void DoAction(int32 actionId) override
            {
                events.ScheduleEvent(EVENT_BURNING_SHADOWBOLT, urand(2000, 5000));
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                if (Player* itr = me->FindNearestPlayer(80.0f))
                    me->AI()->AttackStart(itr);
            }
            
            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
                summons.DespawnAll();
            }

            void KilledUnit(Unit* /*victim*/) override
            {                
                Talk(SAY_KILL);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == 0)
                {
                    if (Creature* valiona = me->SummonCreature(NPC_VALIONA, drahgavalionaPos[0]))
                    {
                        Talk(SAY_VALIONA);
                        valiona->SetReactState(REACT_PASSIVE);
                        valiona->SetCanFly(true);
                        me->EnterVehicle(valiona, 0);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                if (me->HealthBelowPct(30) && stage == 0)
                {
                    stage = 1;
                    DoCast(me, SPELL_TWILIGHT_PROTECTION);
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(0, DrahgaLeavePoint);
                    events.CancelEvent(EVENT_BURNING_SHADOWBOLT);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BURNING_SHADOWBOLT:
                            me->CastCustomSpell(SPELL_BURNING_SHADOWBOLT, SPELLVALUE_MAX_TARGETS, 1, 0, false);
                            events.ScheduleEvent(EVENT_BURNING_SHADOWBOLT, urand(8000, 15000));
                            break;
                        case EVENT_INVOCATION_OF_FLAME:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                            {
                                me->SummonCreature(NPC_INVOCATION_OF_FLAME_STALKER,
                                target->GetPositionX(),
                                target->GetPositionY(),
                                target->GetPositionZ(),
                                0.0f, TEMPSUMMON_TIMED_DESPAWN, 6000ms);
                                Talk(SAY_ADDS);
                            }
                            events.ScheduleEvent(EVENT_INVOCATION_OF_FLAME, 20000);
                            break;
                    }
                }
                if (!me->GetVehicle())
                    DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_drahga_shadowburnerAI>(creature);
        }
};

class npc_drahga_valiona : public CreatureScript
{
    public:
        npc_drahga_valiona() : CreatureScript("npc_drahga_valiona") { }

        struct npc_drahga_valionaAI : public ScriptedAI
        {
            npc_drahga_valionaAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events, nonCombatEvents;
            SummonList summons;
            uint8 stage, wp;

            void IsSummonedBy(Unit* summoner) override
            {
                wp = 1;
                nonCombatEvents.ScheduleEvent(EVENT_VALIONA_MOVE, 100);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
            }

            void Reset() override
            {
                summons.DespawnAll();
                events.Reset();
                stage = 0;
                wp = 1;
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_VALIONAS_FLAME, urand(10000, 15000));
                events.ScheduleEvent(EVENT_SHREDDING_SWIPE, urand(8000, 10000));
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    wp++;

                    switch (pointId)
                    {
                        case 1002:
                            me->DespawnOrUnsummon();
                            break;
                        case 1:
                            nonCombatEvents.ScheduleEvent(EVENT_VALIONA_MOVE, 100);
                            break;
                        case 2:
                            me->SetCanFly(false);
                            me->SetDisableGravity(false);
                            me->SetReactState(REACT_AGGRESSIVE);

                            if (Player* itr = me->FindNearestPlayer(80.0f))
                                me->AI()->AttackStart(itr);
                            break;
                    }
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (me->GetVehicleKit())
                    me->GetVehicleKit()->RemoveAllPassengers();
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_VALIONA_MOVE)
                    {
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->MovePoint(wp, drahgavalionaPos[wp]);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                if (me->HealthBelowPct(30) && stage == 0)
                {
                    stage = 1;
                    events.Reset();
                    DoCast(me, SPELL_TWILIGHT_SHIFT);
                    me->SetReactState(REACT_PASSIVE);
                    if (me->GetVehicleKit())
                        me->GetVehicleKit()->RemoveAllPassengers();
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MovePoint(1002, ValioneLeavePoint);
                    if (Creature* drahga = Unit::GetCreature(*me, instance->GetGuidData(DATA_DRAHGA_SHADOWBURNER)))
                    {
                        drahga->RemoveAurasDueToSpell(SPELL_TWILIGHT_PROTECTION);
                        drahga->AI()->DoAction(ACTION_VALIONA_LEAVE);
                    }
                    Talk(SAY_LEAVE);
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_VALIONAS_FLAME:
                            if (IsHeroic())
                                DoCast(me, SPELL_DEVOURING_FLAMES);
                            else
                                DoCast(me, SPELL_VALIONAS_FLAME);
                            events.ScheduleEvent(EVENT_VALIONAS_FLAME, urand(15000, 22000));
                            break;
                        case EVENT_SHREDDING_SWIPE:
                            DoCast(me->GetVictim(), SPELL_SHREDDING_SWIPE);
                            events.ScheduleEvent(EVENT_SHREDDING_SWIPE, urand(20000, 22000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_drahga_valionaAI>(creature);
        }
};

class npc_invocation_of_flame_stalker : public CreatureScript
{
    public:
        npc_invocation_of_flame_stalker() : CreatureScript("npc_invocation_of_flame_stalker") { }

        struct npc_invocation_of_flame_stalkerAI : public ScriptedAI
        {
            npc_invocation_of_flame_stalkerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }
            
            InstanceScript* instance;
            
            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_INVOKED_FLAMING_SPIRIT:
                        if (Creature* drahga = Unit::GetCreature(*me, instance->GetGuidData(DATA_DRAHGA_SHADOWBURNER)))
                            if (Unit* target = drahga->GetAI()->SelectTarget(SELECT_TARGET_RANDOM))
                                summon->AI()->AttackStart(target);
                        break;
                }
            }

            void Reset() override
            {
                DoCast(me, SPELL_INVOCATION_OF_FLAME);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_invocation_of_flame_stalkerAI>(creature);
        }
};

class npc_invoked_flaming_spirit : public CreatureScript
{
    public:

        npc_invoked_flaming_spirit() : CreatureScript("npc_invoked_flaming_spirit") { }

        struct npc_invoked_flaming_spiritAI : public ScriptedAI
        {
            npc_invoked_flaming_spiritAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetSpeed(MOVE_RUN, 0.8f);
                me->SetSpeed(MOVE_WALK, 0.8f);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                instance = creature->GetInstanceScript();
            }
            
            InstanceScript* instance;

            void Reset() override
            {
                DoCast(me, SPELL_INVOKED_FLAME);
            }

            void JustDied(Unit* /*target*/) override
            {
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_DRAHGA_SHADOWBURNER) != IN_PROGRESS)
                    me->DespawnOrUnsummon();

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_invoked_flaming_spiritAI>(creature);
        }
};

class npc_seeping_twilight : public CreatureScript
{
    public:
        npc_seeping_twilight() : CreatureScript("npc_seeping_twilight"){}

        struct npc_seeping_twilightAI : public ScriptedAI
        {
            npc_seeping_twilightAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoCast(me, SPELL_SEEPING_TWILIGHT_DUMMY);
                DoCast(me, SPELL_SEEPING_TWILIGHT);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_seeping_twilightAI>(creature);
        }
};

class spell_drahga_supernova : public SpellScriptLoader
{
    public:
        spell_drahga_supernova() : SpellScriptLoader("spell_drahga_supernova") { }

        class spell_drahga_supernova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_drahga_supernova_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (!GetCaster())
                    return;

                GetCaster()->ToCreature()->DespawnOrUnsummon(500);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_drahga_supernova_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_drahga_supernova_SpellScript();
        }
};

void AddSC_boss_drahga_shadowburner()
{
    new boss_drahga_shadowburner();
    new npc_invocation_of_flame_stalker();
    new npc_invoked_flaming_spirit();
    new npc_drahga_valiona();
    new npc_seeping_twilight();
    new spell_drahga_supernova();
}
