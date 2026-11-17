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
#include "steam_vault.h"

enum HydromancerThespia
{
    SAY_SUMMON                  = 0,
    SAY_AGGRO                   = 1,
    SAY_SLAY                    = 2,
    SAY_DEAD                    = 3,

    SPELL_LIGHTNING_CLOUD       = 25033,
    SPELL_LUNG_BURST            = 31481,
    SPELL_ENVELOPING_WINDS      = 31718,

    SPELL_WATER_BOLT_VOLLEY     = 34449,
    H_SPELL_WATER_BOLT_VOLLEY   = 37924
};

enum Events
{
    EVENT_LIGHTNING_CLOUD  = 1,
    EVENT_LUNGBURST        = 2,
    EVENT_ENVELOPING_WINDS = 3,
};

class boss_hydromancer_thespia : public CreatureScript
{
    public:
        boss_hydromancer_thespia() : CreatureScript("boss_hydromancer_thespia") { }

        struct boss_thespiaAI : public ScriptedAI
        {
            boss_thespiaAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void InitializeAI() override
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();

                if (instance)
                    instance->SetBossState(DATA_HYDROMANCER_THESPIA, NOT_STARTED);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEAD);

                if (instance)
                {
                    instance->SetBossState(DATA_HYDROMANCER_THESPIA, DONE);

                    if (instance)
                        if (GameObject* goHydroDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_ACCESS_PANEL_HYDRO)))
                            goHydroDoor->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_LIGHTNING_CLOUD, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_LUNGBURST, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENVELOPING_WINDS, 9 * IN_MILLISECONDS);

                if (instance)
                    instance->SetBossState(DATA_HYDROMANCER_THESPIA, IN_PROGRESS);
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
                        case EVENT_LIGHTNING_CLOUD:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_LIGHTNING_CLOUD);

                            //cast twice in Heroic mode
                            if (IsHeroic())
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                    DoCast(target, SPELL_LIGHTNING_CLOUD);

                            events.ScheduleEvent(EVENT_LIGHTNING_CLOUD, 15 * IN_MILLISECONDS + urand(1 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        case EVENT_LUNGBURST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_LUNG_BURST);

                            events.ScheduleEvent(EVENT_LUNGBURST, 7 * IN_MILLISECONDS + urand(1 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                            break;
                        case EVENT_ENVELOPING_WINDS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_ENVELOPING_WINDS);

                            //cast twice in Heroic mode
                            if (IsHeroic())
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                    DoCast(target, SPELL_ENVELOPING_WINDS);

                            events.ScheduleEvent(EVENT_ENVELOPING_WINDS, 9 * IN_MILLISECONDS + urand(1 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_thespiaAI(creature);
        }
};

class npc_coilfang_waterelemental : public CreatureScript
{
    public:
        npc_coilfang_waterelemental() : CreatureScript("npc_coilfang_waterelemental") { }

        struct npc_coilfang_waterelementalAI : public ScriptedAI
        {
            npc_coilfang_waterelementalAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 WaterBoltVolley_Timer;

            void Reset() override
            {
                WaterBoltVolley_Timer = 3000+rand()%3000;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (WaterBoltVolley_Timer <= diff)
                {
                    DoCast(me, SPELL_WATER_BOLT_VOLLEY);
                    WaterBoltVolley_Timer = 7000+rand()%5000;
                } else WaterBoltVolley_Timer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_coilfang_waterelementalAI(creature);
        }
};

void AddSC_boss_hydromancer_thespia()
{
    new boss_hydromancer_thespia();
    new npc_coilfang_waterelemental();
}
