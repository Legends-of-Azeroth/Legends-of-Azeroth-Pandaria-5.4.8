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
    SAY_AGGRO              = 0,
    SAY_KILL               = 1,
    SAY_SKILL              = 2,
    SAY_DEATH              = 3,
};

enum Spells
{
    SPELL_GROUND_SLAM      = 78903,
    SPELL_SHATTER          = 78807,
    SPELL_PARALYZE         = 92426,
    SPELL_PARALYZE_DMG     = 94661,
    SPELL_ENRAGE           = 80467,
    SPELL_SHIELD           = 78835,
    SPELL_BULWARK          = 78939,
};

enum Events
{
    EVENT_GROUND_SLAM      = 1,
    EVENT_SHATTER          = 2,
    EVENT_PARALYZE         = 3,
    EVENT_SHIELD           = 4,
    EVENT_BULWARK          = 5,
};

class boss_ozruk : public CreatureScript
{
    public:
        boss_ozruk() : CreatureScript("boss_ozruk") { }

        struct boss_ozrukAI : public BossAI
        {
            boss_ozrukAI(Creature* creature) : BossAI(creature, DATA_OZRUK)
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
            bool bEnrage;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                bEnrage = false;
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_SHATTER, 21000);
                events.ScheduleEvent(EVENT_GROUND_SLAM, 7000);
                events.ScheduleEvent(EVENT_BULWARK, 80000);
                events.ScheduleEvent(EVENT_SHIELD, 3000);
                Talk(SAY_AGGRO);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(20) && !bEnrage)
                {
                    DoCast(me, SPELL_ENRAGE);
                    bEnrage = true;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GROUND_SLAM:
                            DoCast(SPELL_GROUND_SLAM);
                            events.ScheduleEvent(EVENT_GROUND_SLAM, 15000);
                            break;
                        case EVENT_SHATTER:
                            if (IsHeroic())
                            {
                                events.RescheduleEvent(EVENT_GROUND_SLAM, 15000);
                                DoCast(SPELL_PARALYZE);
                            }
                            DoCast(SPELL_SHATTER);
                            events.ScheduleEvent(EVENT_SHATTER, 20000);
                            break;
                        case EVENT_SHIELD:
                            DoCast(SPELL_SHIELD);
                            events.ScheduleEvent(EVENT_SHIELD, 15000);
                            break;
                        case EVENT_BULWARK:
                            DoCast(SPELL_BULWARK);
                            events.ScheduleEvent(EVENT_BULWARK, 15000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_ozrukAI>(creature);
        }
};

void AddSC_boss_ozruk()
{
    new boss_ozruk();
}