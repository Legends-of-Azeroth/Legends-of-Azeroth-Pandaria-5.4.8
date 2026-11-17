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

#include "ScriptPCH.h"
#include "the_stonecore.h"

enum Spells
{
    SPELL_CRYSTAL_BARRAGE         = 86881,
    SPELL_CRYSTAL_BARRAGE_SUM     = 92012,
    SPELL_CRYSTAL_SHARD_DMG       = 92122,
    SPELL_DUMPENING_WAVE          = 82415,
    SPELL_SUBMERGE                = 53421,
    SPELL_THRASHING_CHARGE_DMG    = 81828,
    SPELL_THRASHING_CHARGE_SUM    = 81816,
    SPELL_THRASHING_CHARGE_DUM    = 81801,
    SPELL_ROCK_BORE               = 80028,
};

enum Events
{
    EVENT_CRYSTAL_BARRAGE         = 1,
    EVENT_DUMPENING_WAVE          = 2,
    EVENT_MERGE                   = 3,
    EVENT_SUBMERGE                = 4,
    EVENT_CRYSTAL_BARRAGE_H       = 5,
    EVENT_THRASHING_CHARGE        = 6,
    EVENT_ROCK_BORER              = 7,
    EVENT_ROCK_BORE               = 8,
    EVENT_CRYSTAL_SHARD_MOVE      = 9,
};

enum Adds
{
    NPC_ROCK_BORER                = 43917,
    NPC_CRYSTAL_SHARD             = 49267,
    NPC_THRASHING_CHARGE          = 43743,
};

class boss_corborus : public CreatureScript
{
    public:
        boss_corborus() : CreatureScript("boss_corborus") { }

        struct boss_corborusAI : public BossAI
        {
            boss_corborusAI(Creature* creature) : BossAI(creature, DATA_CORBORUS), summons(me)
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
            }

            EventMap events;
            SummonList summons;
            uint8 stage;
            Position barragePos;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                stage = 0;
                me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);    
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_DUMPENING_WAVE, 5000);
                events.ScheduleEvent(EVENT_CRYSTAL_BARRAGE, 7000);
                events.ScheduleEvent(EVENT_SUBMERGE, 30000);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                switch (summon->GetEntry())
                {
                    case NPC_THRASHING_CHARGE:
                        summon->CastSpell(summon, SPELL_THRASHING_CHARGE_DUM, false);
                        break;
                    case NPC_ROCK_BORER:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            summon->GetThreatManager().AddThreat(target, 10.0f);
                            summon->Attack(target, true);
                            summon->GetMotionMaster()->MoveChase(target);
                        }
                        break;
                    case NPC_CRYSTAL_SHARD:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            summon->GetThreatManager().AddThreat(target, 10.0f);
                            summon->Attack(target, true);
                            summon->GetMotionMaster()->MoveChase(target);
                        }
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                {
                    me->SetHomePosition(*me);

                    if (Creature* Manastorm = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_MILLHOUSE_MANASTORM)))
                        Manastorm->AI()->DoAction(ACTION_MANASTORM_PULL);
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
                        case EVENT_DUMPENING_WAVE:
                            DoCast(SPELL_DUMPENING_WAVE);
                            events.ScheduleEvent(EVENT_DUMPENING_WAVE, 10000);
                            break;
                        case EVENT_CRYSTAL_BARRAGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                            {
                                DoCast(target, SPELL_CRYSTAL_BARRAGE);
                                if (IsHeroic())
                                {
                                    barragePos = target->GetPosition();
                                    events.ScheduleEvent(EVENT_CRYSTAL_BARRAGE_H, 4000);
                                }    
                            }
                            events.ScheduleEvent(EVENT_CRYSTAL_BARRAGE, 15000);
                            break;
                        case EVENT_CRYSTAL_BARRAGE_H:
                            for (uint8 i = 0; i < 5; i++)
                            {
                                me->CastSpell(
                                    barragePos.GetPositionX(),
                                    barragePos.GetPositionY(),
                                    barragePos.GetPositionZ(),
                                    SPELL_CRYSTAL_BARRAGE_SUM, true);
                            }  
                            break;
                        case EVENT_SUBMERGE:
                            me->RemoveAllAuras();
                            me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            DoCast(me, SPELL_SUBMERGE);
                            events.Reset();
                            events.ScheduleEvent(EVENT_THRASHING_CHARGE, 4000);
                            events.ScheduleEvent(EVENT_ROCK_BORER, 3000);
                            events.ScheduleEvent(EVENT_MERGE, 20000);
                            break;
                        case EVENT_MERGE:
                            me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            events.Reset();
                            events.ScheduleEvent(EVENT_DUMPENING_WAVE, 5000);
                            events.ScheduleEvent(EVENT_CRYSTAL_BARRAGE, 7000);
                            events.ScheduleEvent(EVENT_SUBMERGE, 30000);
                            break;
                        case EVENT_THRASHING_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_THRASHING_CHARGE_SUM, true);
                            events.ScheduleEvent(EVENT_THRASHING_CHARGE, 4000);
                            break;
                        case EVENT_ROCK_BORER:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                me->SummonCreature(NPC_ROCK_BORER,
                                    target->GetPositionX()+urand(3, 5),
                                    target->GetPositionY()+urand(3, 5),
                                    target->GetPositionZ(), 0.0f);
                            }
                            events.ScheduleEvent(EVENT_ROCK_BORER, 3000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_corborusAI>(creature);
        }
};

class npc_rock_borer : public CreatureScript
{
    public:
        npc_rock_borer() : CreatureScript("npc_rock_borer") { }

        struct npc_rock_borerAI : public ScriptedAI
        {
            npc_rock_borerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ROCK_BORE, 2000);
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
                        case EVENT_ROCK_BORE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_ROCK_BORE);
                            events.ScheduleEvent(EVENT_ROCK_BORE, 5000);
                            return;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_rock_borerAI>(creature);
        }
};

class npc_crystal_shard : public CreatureScript
{
    public:
        npc_crystal_shard() : CreatureScript("npc_crystal_shard") { }

        struct npc_crystal_shardAI : public ScriptedAI
        {
            npc_crystal_shardAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetSpeed(MOVE_WALK, 0.5f);
                me->SetSpeed(MOVE_RUN, 0.5f);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                if (Unit* target = me->SelectNearestTarget(3.0f))
                {
                    DoCast(target, SPELL_CRYSTAL_SHARD_DMG);
                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crystal_shardAI>(creature);
        }
};

void AddSC_boss_corborus()
{
    new boss_corborus();
    new npc_rock_borer();
    new npc_crystal_shard();
}