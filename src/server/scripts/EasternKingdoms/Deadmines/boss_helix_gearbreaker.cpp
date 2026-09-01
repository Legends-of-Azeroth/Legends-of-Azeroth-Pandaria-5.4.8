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

#include "ScriptPCH.h"
#include "deadmines.h"
#include "Vehicle.h"

// todo: ����������� ������ �������, ������� ������� �����

enum ScriptTexts
{
    SAY_DEATH    = 0,
    SAY_KILL     = 1,
    SAY_OAF_DEAD = 2,
    SAY_SPELL1   = 3,
    SAY_SPELL2   = 4,
    SAY_AGGRO    = 5,

    SAY_OAF1     = 0,
};

enum Spells
{
    SPELL_CHEST_BOMB            = 88352,
    SPELL_CHEST_BOMB_DMG        = 88250,

    SPELL_OAF_SMASH             = 88300,
    SPELL_CHARGE                = 88288,
    SPELL_FORCE_PLAYER_RIDE_OAF = 88278,
    SPELL_RIDE_OAF              = 88277,
    SPELL_HELIX_RIDE            = 88337,
};

enum Adds
{
    NPC_STICKY_BOMB = 47314,
    NPC_HELIX_CREW  = 49136,
    NPC_MINE_RAT    = 51462,
};

enum Events
{
    EVENT_THROW_HELIX   = 5,
    EVENT_PICKUP_HELIX  = 6,
    EVENT_STICKY_BOMB   = 7,
    EVENT_HELIX_CREW    = 8,
    EVENT_CHARGE_OAF0   = 1,
    EVENT_CHARGE_OAF1   = 2,
    EVENT_CHARGE_OAF2   = 3,
    EVENT_CHARGE_OAF3   = 4,
    EVENT_BOMB_READY    = 9,
    EVENT_BOMB_EXPLODE  = 10,
    EVENT_CHEST_BOMB    = 11,
};

enum Points
{
    POINT_START = 1,
    POINT_END   = 2,
};

enum Actions
{
    ACTION_CHARGE   = 1,
};

const Position lumberingoafPos[3] =
{
    {-301.93f, -516.32f, 51.71f, 2.03f},
    {-289.98f, -528.06f, 49.75f, 1.59f},
    {-289.67f, -488.46f, 49.80f, 1.54f} 
};

const Position helixcrewPos[4] =
{
    {-295.26f,-503.38f,60.16f, 0.0f},
    {-280.85f,-503.98f,60.16f, 0.0f},
    {-291.45f,-503.37f,60.16f, 0.0f},
    {-285.63f,-504.04f,60.16f, 0.0f}
};

const Position oafPos[1] =
{
    {-302.361f, -516.346f, 52.0315f, 0.174533f},
};

class boss_helix_gearbreaker : public CreatureScript
{
    public:
        boss_helix_gearbreaker() : CreatureScript("boss_helix_gearbreaker") { }

        struct boss_helix_gearbreakerAI : public BossAI
        {
            boss_helix_gearbreakerAI(Creature* creature) : BossAI(creature, DATA_HELIX) { }

            void InitializeAI() override
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }
            
            void Reset() override 
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                if (summon->GetEntry() == NPC_LUMBERING_OAF)
                {
                    Talk(SAY_OAF_DEAD);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_STICKY_BOMB, 8000);
                if (IsHeroic())
                {
                    events.ScheduleEvent(EVENT_CHEST_BOMB, urand(8000, 12000));
                    events.ScheduleEvent(EVENT_HELIX_CREW, 5000);
                }
                Talk(SAY_AGGRO);
                DoZoneInCombat();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }
            
            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);

                if (Creature* Oaf = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_OAF)))
                    me->Kill(Oaf);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (!me->GetVehicle())
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
                    {
                        //Talk(SAY_OAF_DEAD);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        return;
                    }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHEST_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                Talk(SAY_SPELL1);
                                DoCast(target, SPELL_CHEST_BOMB);
                            }
                            events.ScheduleEvent(EVENT_CHEST_BOMB, urand(15000, 20000));
                            break;
                        case EVENT_STICKY_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                me->SummonCreature(NPC_STICKY_BOMB, 
                                    target->GetPositionX(),
                                    target->GetPositionY(),
                                    target->GetPositionZ(),
                                    0.0f);
                            }
                            events.ScheduleEvent(EVENT_STICKY_BOMB, urand(8000, 10000));
                            break;
                        case EVENT_HELIX_CREW:
                            for (uint8 i = 0; i < 4; i++)
                                me->SummonCreature(NPC_HELIX_CREW, helixcrewPos[i], TEMPSUMMON_CORPSE_DESPAWN);
                            break;
                    }
                }

                if (!me->GetVehicle())
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_helix_gearbreakerAI>(creature);
        }
};

class npc_lumbering_oaf : public CreatureScript
{
    public:
        npc_lumbering_oaf() : CreatureScript("npc_lumbering_oaf") { }

        struct npc_lumbering_oafAI : public ScriptedAI
        {
            npc_lumbering_oafAI(Creature* c) : ScriptedAI(c) { }

            EventMap events;
            InstanceScript* instance;
            bool m_fake;

            void InitializeAI() override
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                m_fake = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetReactState(REACT_AGGRESSIVE);
            }
     
            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_CHARGE:
                        events.ScheduleEvent(EVENT_CHARGE_OAF1, 500);
                        break;
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_CHARGE_OAF0, 10000);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                Reset();
                me->GetMotionMaster()->MoveTargetedHome();
                uint32 corpseDelay = me->GetCorpseDelay();
                uint32 respawnDelay = me->GetRespawnDelay();

                me->SetCorpseDelay(1);
                me->SetRespawnDelay(29);

                me->DespawnOrUnsummon();

                me->SetCorpseDelay(corpseDelay);
                me->SetRespawnDelay(respawnDelay);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->GetHealth() <= damage && !m_fake)
                {
                    damage = 0;

                    if (Vehicle* vehicle = me->GetVehicleKit())
                        vehicle->RemoveAllPassengers();

                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->AttackStop();

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    events.Reset();
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (pointId)
                    {
                        case POINT_START:
                            events.ScheduleEvent(EVENT_CHARGE_OAF2, 500);
                            break;
                        case POINT_END:
                            events.ScheduleEvent(EVENT_CHARGE_OAF3, 500);
                            break;
                    }
                }
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
                        case EVENT_CHARGE_OAF0:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            {
                                me->SetReactState(REACT_PASSIVE);
                                DoCast(target, SPELL_CHARGE, true);
                            }
                            break;
                        case EVENT_CHARGE_OAF1:
                            me->GetMotionMaster()->MovePoint(POINT_START, lumberingoafPos[1]);
                            break;
                        case EVENT_CHARGE_OAF2:
                            me->GetMotionMaster()->MoveCharge(
                                lumberingoafPos[2].GetPositionX(),
                                lumberingoafPos[2].GetPositionY(),
                                lumberingoafPos[2].GetPositionZ(),
                                42.0f, POINT_END);
                            break;
                        case EVENT_CHARGE_OAF3:
                            DoCastAOE(SPELL_OAF_SMASH);
                            for (uint32 i = 0; i < 9; ++i)
                            {
                                Position src = { -290.447998f, -484.328003f, 49.887901f };
                                Position dst = { -289.535614f, -493.059448f, 50.117603f };
                                me->MovePosition(src, (float)rand_norm() * 2, (float)rand_norm() * M_PI * 2);
                                me->MovePosition(dst, (float)rand_norm() * 3, (float)rand_norm() * M_PI * 2);
                                if (Creature* rat = me->SummonCreature(NPC_MINE_RAT, src, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000ms))
                                    rat->GetMotionMaster()->MovePoint(0, dst, false);
                            }
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            me->RemoveAurasDueToSpell(SPELL_RIDE_OAF);
                            DoCastAOE(SPELL_HELIX_RIDE, true);
                            events.ScheduleEvent(EVENT_CHARGE_OAF0, 30000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_lumbering_oafAI>(creature);
        }
};

class npc_sticky_bomb : public CreatureScript
{
    public:
        npc_sticky_bomb() : CreatureScript("npc_sticky_bomb") { }

        enum iEvents
        {
            EVENT_ARMING_ARMED  = 1,
            EVENT_ARMING_ORANGE = 2,
            EVENT_ARMING_YELLOW = 3,
            EVENT_ARMING_RED    = 4,
            EVENT_BOOM          = 5,
            EVENT_CHECK_PLAYER  = 6,
        };

        enum iSpells
        {
            SPELL_ARMING_VISUAL_YELLOW         = 88315,
            SPELL_ARMING_VISUAL_RED            = 88317,
            SPELL_ARMING_VISUAL_ORANGE         = 88316,
            SPELL_STICKY_BOMB_ARMED_STATE      = 88319,
            SPELL_STICKY_BOMB_PERIODIC_TRIGGER = 88329,
            SPELL_EXPLODE                      = 88974,
        };

        struct npc_sticky_bombAI : public ScriptedAI
        {
            npc_sticky_bombAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool canBoom;

            void InitializeAI() override
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
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                SetCombatMovement(false);
                canBoom = false;
            }
     
            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARMING_YELLOW, 1000);
            }

            ObjectGuid GetTarget()
            {
                ThreatContainer::StorageType const& threatlist = me->GetThreatManager().getThreatList();

                for (ThreatContainer::StorageType::const_iterator i = threatlist.begin(); i != threatlist.end(); ++i)
                    if (Unit* target = ObjectAccessor::GetUnit((*me), (*i)->getUnitGuid()))
                        if (target->GetTypeId() == TYPEID_PLAYER && me->GetExactDist2dSq(target) < 4 && !target->GetVehicle())
                            return target->GetGUID();

                return ObjectGuid::Empty;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARMING_YELLOW:
                            me->CastSpell(me, canBoom ? SPELL_ARMING_VISUAL_RED : SPELL_ARMING_VISUAL_YELLOW, true);
                            events.ScheduleEvent(EVENT_ARMING_ORANGE, 2000);
                            break;
                        case EVENT_ARMING_ORANGE:
                            me->CastSpell(me, canBoom ? SPELL_ARMING_VISUAL_RED : SPELL_ARMING_VISUAL_ORANGE, true);
                            events.ScheduleEvent(EVENT_ARMING_RED, 2000);
                            break;
                        case EVENT_ARMING_RED:
                            me->CastSpell(me, SPELL_ARMING_VISUAL_RED, true);
                            events.ScheduleEvent(canBoom ? EVENT_BOOM : EVENT_ARMING_ARMED, 1500);
                            break;
                        case EVENT_ARMING_ARMED:
                            canBoom = true;
                            me->CastSpell(me, SPELL_STICKY_BOMB_ARMED_STATE, true);
                            events.ScheduleEvent(EVENT_CHECK_PLAYER, 250);
                            events.ScheduleEvent(EVENT_ARMING_YELLOW, 14000);
                            break;
                        case EVENT_BOOM:
                            me->CastSpell(me, SPELL_STICKY_BOMB_PERIODIC_TRIGGER, true);
                            break;
                        case EVENT_CHECK_PLAYER:
                        {
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetTarget()))
                            {
                                me->CastSpell(target, SPELL_EXPLODE, true);
                                me->SetDisplayId(11686);
                                me->DespawnOrUnsummon(2000);
                                events.Reset();
                                break;
                            }

                            events.ScheduleEvent(EVENT_CHECK_PLAYER, 250);
                            break;
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_sticky_bombAI>(creature);
        }
};

class npc_helix_crew : public CreatureScript
{
    public:
        npc_helix_crew() : CreatureScript("npc_helix_crew") { }

        struct npc_helix_crewAI : public ScriptedAI
        {
            npc_helix_crewAI(Creature* creature) : ScriptedAI(creature), summons(me) 
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
                SetCombatMovement(false);
            }

            EventMap events;
            SummonList summons;

            void Reset() override
            {
                summons.DespawnAll();
                SetCombatMovement(false);
            }
     
            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_STICKY_BOMB, 8000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STICKY_BOMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                me->SummonCreature(NPC_STICKY_BOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000ms);
                            events.ScheduleEvent(EVENT_STICKY_BOMB, urand(15000, 20000));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_helix_crewAI>(creature);
        }
};

class spell_helix_chest_bomb : public SpellScriptLoader
{
    public:
        spell_helix_chest_bomb() : SpellScriptLoader("spell_helix_chest_bomb") { }


        class spell_helix_chest_bomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_helix_chest_bomb_AuraScript)

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = aurEff->GetBase()->GetUnitOwner())
                    target->CastSpell(target, SPELL_CHEST_BOMB_DMG, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_helix_chest_bomb_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
             }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_helix_chest_bomb_AuraScript();
        }
};

class spell_helix_force_player_to_ride_oaf : public SpellScriptLoader
{
    public:
        spell_helix_force_player_to_ride_oaf() : SpellScriptLoader("spell_helix_force_player_to_ride_oaf") { }


        class spell_helix_force_player_to_ride_oaf_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_helix_force_player_to_ride_oaf_SpellScript);


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetCaster(), SPELL_RIDE_OAF, true);
                GetCaster()->ToCreature()->AI()->DoAction(ACTION_CHARGE);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_helix_force_player_to_ride_oaf_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_helix_force_player_to_ride_oaf_SpellScript();
        }
};

void AddSC_boss_helix_gearbreaker()
{
    new boss_helix_gearbreaker();
    new npc_lumbering_oaf();
    new npc_helix_crew();
    new npc_sticky_bomb();
    new spell_helix_force_player_to_ride_oaf();
    new spell_helix_chest_bomb();
}