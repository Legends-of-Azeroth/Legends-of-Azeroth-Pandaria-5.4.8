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
#include "the_stonecore.h"

enum Spells
{
    SPELL_SAND_BLAST               = 80807,
    SPELL_LAVA_FISSURE_DUM         = 80798,
    SPELL_ERUPTION                 = 80800,
    SPELL_ERUPTION_AURA            = 80801,
    SPELL_STALACTITE_MISSLE        = 80643,
    SPELL_STALACTITE_AREA_AURA     = 80654,
    SPELL_STALACTITE_CALL          = 80656, // by encounter in fly phase
};

enum Events
{
    EVENT_SAND_BLAST               = 1,
    EVENT_LAVA_FISSURE             = 2,
    EVENT_FLY                      = 3,
    EVENT_GROUND                   = 4,
    EVENT_ERUPTION                 = 5,
    EVENT_STALACTITE               = 6,
    EVENT_COSMETIC_DONE            = 7,
};

enum Adds
{
    NPC_LAVA_FISSURE               = 43242,
    NPC_STALACTITE_TRIGGER         = 43159,
    NPC_STALACTITE__GROUND_TRIGGER = 43357,
};

Position slabhidegroundPos = { 1278.23f, 1212.27f, 247.28f, 0.0f };
Position slabhideflyPos    = { 1278.23f, 1212.27f, 257.28f, 0.0f };

class boss_slabhide : public CreatureScript
{
    public:
        boss_slabhide() : CreatureScript("boss_slabhide") { }

        struct boss_slabhideAI : public BossAI
        {
            boss_slabhideAI(Creature* creature) : BossAI(creature, DATA_SLABHIDE) { }

            EventMap events, nonCombatEvents;
            bool PreEventDone;
            ObjectGuid doorGuid;

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
                nonCombatEvents.ScheduleEvent(EVENT_STALACTITE, urand(1500, 3000));
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

                me->SetAnimTier(AnimTier::Fly);
                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();
                PreEventDone = false;
            }

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetCanFly(false);
                events.Reset();
                summons.DespawnAll();
                nonCombatEvents.Reset();
                HandleCloseEntranceRockwall();
                doorGuid = ObjectGuid::Empty;

                me->SetAnimTier(AnimTier::Ground);
                me->OverrideInhabitType(INHABIT_GROUND);
                me->UpdateMovementFlags();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_FLY, 50000);
                events.ScheduleEvent(EVENT_SAND_BLAST, 10000);
                events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(10000, 15000));

                if (GameObject* goRockwall = me->SummonGameObject(GO_ROCKDOOR_ENTERANCE_SLABHIDE, SlabhideEntranceDoor.GetPositionX(), SlabhideEntranceDoor.GetPositionY(), SlabhideEntranceDoor.GetPositionZ(), SlabhideEntranceDoor.GetOrientation(), { 0.0f, 0.0f, 0.469471f, 0.882948f }, 0))
                    doorGuid = goRockwall->GetGUID();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->SetCanFly(false);
                HandleCloseEntranceRockwall();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (pointId)
                    {
                        case 1:
                            events.ScheduleEvent(EVENT_GROUND, 10000);

                            if (TempSummon* Bunny = me->SummonCreature(NPC_GENERAL_BUNNY_JMF, 1272.27f, 1212.80f, 247.15f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * IN_MILLISECONDS)))
                                Bunny->CastSpell(Bunny, SPELL_STALACTITE_CALL, false); // not sure about that, but if encounter not AT GROUND, then stalactite makes spawn under encounter
                            break;
                        case 2:
                            me->SetCanFly(false);
                            SetCombatMovement(true);
                            me->SetAnimTier(AnimTier::Ground);
                            me->OverrideInhabitType(INHABIT_GROUND);
                            me->UpdateMovementFlags();
                            events.ScheduleEvent(EVENT_SAND_BLAST, 10000);
                            events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(15000, 28000));
                            events.ScheduleEvent(EVENT_FLY, 50000);
                            AttackStart(me->GetVictim());
                            break;
                    }
                }
            }

            void DoAction(int32 actionId) override
            {
                if (!PreEventDone && actionId == ACTION_STALACTITE_REMOVE)
                {
                    PreEventDone = true;
                    nonCombatEvents.Reset();

                    Movement::MoveSplineInit init(me);
                    for (auto itr : SlabhidePath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.SetSmooth();
                    init.Launch();
                    nonCombatEvents.ScheduleEvent(EVENT_COSMETIC_DONE, me->GetSplineDuration());
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STALACTITE:
                            HandleInitStalactiteArray();
                            nonCombatEvents.ScheduleEvent(EVENT_STALACTITE, urand(8 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                            break;
                        case EVENT_COSMETIC_DONE:
                            summons.DespawnAll();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                            me->SetFacingTo(3.58f);
                            me->SetHomePosition(*me);
                            me->SetAnimTier(AnimTier::Ground);
                            me->OverrideInhabitType(INHABIT_GROUND);
                            me->UpdateMovementFlags();
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FLY:
                            events.Reset();
                            SetCombatMovement(false);
                            me->SetAnimTier(AnimTier::Fly);
                            me->OverrideInhabitType(INHABIT_AIR);
                            me->UpdateMovementFlags();
                            me->SetCanFly(true);
                            me->GetMotionMaster()->MovePoint(1, slabhideflyPos);
                            break;
                        case EVENT_GROUND:
                            events.Reset();
                            me->GetMotionMaster()->MovePoint(2, slabhidegroundPos);
                            break;
                        case EVENT_SAND_BLAST:
                            DoCast(SPELL_SAND_BLAST);
                            events.ScheduleEvent(EVENT_SAND_BLAST, 10000);
                            break;
                        case EVENT_LAVA_FISSURE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->SummonCreature(NPC_LAVA_FISSURE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f);
                            events.ScheduleEvent(EVENT_LAVA_FISSURE, urand(15000, 18000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                void HandleInitStalactiteArray()
                {
                    uint32 sCount = urand(8, 16);

                    for (uint32 i = 0; i < sCount; ++i)
                    {
                        uint32 pPos = urand(0, 4);
                        me->SummonCreature(NPC_STALACTITE_TRASH, StalactitePreEventPos[pPos].GetPositionX() + frand(-10.0f, 10.0f), StalactitePreEventPos[pPos].GetPositionY() + frand(-7.0f, 7.0f), StalactitePreEventPos[pPos].GetPositionZ() + 2.0f, StalactitePreEventPos[pPos].GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(20 * IN_MILLISECONDS));
                    }
                }

                void HandleCloseEntranceRockwall()
                {
                    if (GameObject* goRockwall = ObjectAccessor::GetGameObject(*me, doorGuid))
                        goRockwall->Delete();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_slabhideAI>(creature);
        }
};

// Stalactite 43159, 43357
class npc_stalactite : public CreatureScript
{
    public:
        npc_stalactite() : CreatureScript("npc_stalactite") { }

        struct npc_stalactiteAI : public ScriptedAI
        {
            npc_stalactiteAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 delay;

            void IsSummonedBy(Unit* summoner) override
            {
                delay = 0;
                me->m_Events.Schedule(delay += urand(1500, 6500), 1, [this]()
                {
                    me->CastSpell(me, SPELL_STALACTITE_AREA_AURA, true);
                });

                me->m_Events.Schedule(delay += 6000, 2, [this]()
                {
                    me->CastSpell(me, SPELL_STALACTITE_MISSLE, true);
                });
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_stalactiteAI>(creature);
        }
};

class npc_lava_fissure : public CreatureScript
{
    public:
        npc_lava_fissure() : CreatureScript("npc_lava_fissure") { }

        struct npc_lava_fissureAI : public ScriptedAI
        {
            npc_lava_fissureAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint32 uidespawnTimer;

            void Reset() override
            {
                uidespawnTimer = DUNGEON_MODE(15000, 33000);
                events.ScheduleEvent(EVENT_ERUPTION, DUNGEON_MODE(5000, 3000));
                DoCast(SPELL_LAVA_FISSURE_DUM);
            }

            void UpdateAI(uint32 diff) override
            {
                if (uidespawnTimer <= diff)
                    me->DespawnOrUnsummon();
                else
                    uidespawnTimer -= diff;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ERUPTION:
                            me->RemoveAurasDueToSpell(SPELL_LAVA_FISSURE_DUM);
                            DoCast(SPELL_ERUPTION);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_lava_fissureAI>(creature);
        }
};

// AreaTrigger 6070
class AreaTrigger_at_behind_slabhide : public AreaTriggerScript
{
    public:
        AreaTrigger_at_behind_slabhide() : AreaTriggerScript("AreaTrigger_at_behind_slabhide") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* Slabhide = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SLABHIDE)))
                    Slabhide->AI()->DoAction(ACTION_STALACTITE_REMOVE);

            return false;
        }
};

void AddSC_boss_slabhide()
{
    new boss_slabhide();
    new npc_stalactite();
    new npc_lava_fissure();
    new AreaTrigger_at_behind_slabhide();
}