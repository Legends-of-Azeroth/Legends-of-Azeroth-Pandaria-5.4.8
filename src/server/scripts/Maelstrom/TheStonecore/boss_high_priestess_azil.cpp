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

enum ScriptTexts
{
    SAY_AGGRO    = 3,
    SAY_DEATH    = 2,
    SAY_SKILL    = 1,
};

enum Spells
{
    SPELL_CURSE_OF_BLOOD            = 79345,
    SPELL_ARCANE_SHIELD             = 79050,
    SPELL_SUMMON_GRAVITY_WELL       = 79340,
    SPELL_GRAVITY_WELL_AURA_0       = 79245,
    SPELL_GRAVITY_WELL_AURA_1       = 79244,
    SPELL_GRAVITY_WELL_AURA_1_T     = 79251,
    SPELL_GRAVITY_WELL_AURA_MOD     = 92475,
    SPELL_GRAVITY_WELL_AURA_DMG     = 79249,
};

enum Events
{
    EVENT_CURSE_OF_BLOOD    = 1,
    EVENT_SHIELD            = 2,
    EVENT_GRAVITY_WELL      = 3,
    EVENT_GRAVITY_WELL_1    = 4,
    EVENT_SUMMON_ADDS       = 5,
    EVENT_MOVE              = 6,
    EVENT_MOVE_2            = 7,
};

enum Adds
{
    NPC_ADVOUT_FOLLOWER     = 42428,
    NPC_GRAVITY_WELL        = 42499,
};

enum
{
    WORLD_STATE_ROTTEN_TO_THE_CORE = 8336,
};

Position highpriestessazilAddsPos[2] = 
{
    {1263.20f, 966.03f, 205.81f, 0.0f},
    {1278.51f, 1021.72f, 209.08f, 0.0f}
};

Position highpriestessazilStandPos = {1337.79f, 963.39f, 214.18f, 1.8f};

class boss_high_priestess_azil : public CreatureScript
{
    public:
        boss_high_priestess_azil() : CreatureScript("boss_high_priestess_azil") { }

        struct boss_high_priestess_azilAI : public BossAI
        {
            boss_high_priestess_azilAI(Creature* creature) : BossAI(creature, DATA_HIGH_PRIESTESS_AZIL), summons(me)
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

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
                events.Reset();

                me->GetMap()->SetWorldState(WORLD_STATE_ROTTEN_TO_THE_CORE, 0);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                switch (summon->GetEntry())
                {
                    case NPC_ADVOUT_FOLLOWER:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        {
                            summon->GetThreatManager().AddThreat(target, 10.0f);
                            summon->Attack(target, true);
                            summon->GetMotionMaster()->MoveChase(target);
                        }
                        break;
                }
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
                events.ScheduleEvent(EVENT_SHIELD, 35000);
                events.ScheduleEvent(EVENT_CURSE_OF_BLOOD, 3000);
                events.ScheduleEvent(EVENT_SUMMON_ADDS, urand(5000, 7000));
                events.ScheduleEvent(EVENT_GRAVITY_WELL, 10000);

                me->GetMap()->SetWorldState(WORLD_STATE_ROTTEN_TO_THE_CORE, 1);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
                Talk(SAY_DEATH);

                me->GetMap()->SetWorldState(WORLD_STATE_ROTTEN_TO_THE_CORE, 0);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (pointId)
                    {
                        case 1:
                            events.ScheduleEvent(EVENT_MOVE_2, 20000);
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
                        case EVENT_SUMMON_ADDS:
                            for (uint8 i = 0; i < 3; i++)
                                me->SummonCreature(NPC_ADVOUT_FOLLOWER, highpriestessazilAddsPos[urand(0,1)]);
                            events.ScheduleEvent(EVENT_SUMMON_ADDS, urand(5000, 7000));
                            break;
                        case EVENT_CURSE_OF_BLOOD:
                            DoCast(SPELL_CURSE_OF_BLOOD);
                            events.ScheduleEvent(EVENT_CURSE_OF_BLOOD, 15000);
                            break;
                        case EVENT_GRAVITY_WELL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_SUMMON_GRAVITY_WELL);
                            events.ScheduleEvent(EVENT_GRAVITY_WELL, 15000);
                            break;
                        case EVENT_SHIELD:
                            Talk(SAY_SKILL);
                            SetCombatMovement(false);
                            events.CancelEvent(EVENT_CURSE_OF_BLOOD);
                            DoCast(me, SPELL_ARCANE_SHIELD);
                            events.ScheduleEvent(EVENT_MOVE, 2000);
                            break;
                        case EVENT_MOVE:
                            me->GetMotionMaster()->MovePoint(1, highpriestessazilStandPos);
                            break;
                        case EVENT_MOVE_2:
                            me->RemoveAurasDueToSpell(SPELL_ARCANE_SHIELD);
                            SetCombatMovement(true);
                            if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                            events.ScheduleEvent(EVENT_CURSE_OF_BLOOD, 3000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_high_priestess_azilAI>(creature);
        }
};

class npc_gravity_well : public CreatureScript
{
    public:
        npc_gravity_well() : CreatureScript("npc_gravity_well") { }

        struct npc_gravity_wellAI : public ScriptedAI
        {
            npc_gravity_wellAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint32 uidespawnTimer;

            void Reset() override
            {
                uidespawnTimer = 20000;
                events.ScheduleEvent(EVENT_GRAVITY_WELL_1, 3000);
                DoCast(me, SPELL_GRAVITY_WELL_AURA_0);
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
                    case EVENT_GRAVITY_WELL_1:
                        me->RemoveAurasDueToSpell(SPELL_GRAVITY_WELL_AURA_0);
                        DoCast(me, SPELL_GRAVITY_WELL_AURA_1);
                        break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_gravity_wellAI>(creature);
        }
};

class spell_high_priestess_azil_gravity_well_script : public SpellScriptLoader
{
    public:
        spell_high_priestess_azil_gravity_well_script() : SpellScriptLoader("spell_high_priestess_azil_gravity_well_script") { }

        class spell_high_priestess_azil_gravity_well_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_high_priestess_azil_gravity_well_script_SpellScript);


            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_GRAVITY_WELL_AURA_DMG, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_high_priestess_azil_gravity_well_script_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_high_priestess_azil_gravity_well_script_SpellScript();
        }
};

void AddSC_boss_high_priestess_azil()
{
    new boss_high_priestess_azil();
    new npc_gravity_well();
    new spell_high_priestess_azil_gravity_well_script();
}