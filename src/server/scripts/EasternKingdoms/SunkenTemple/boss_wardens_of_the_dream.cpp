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
#include "sunken_temple.h"

enum ScriptTexts
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,
    SAY_KILL = 2,
};

enum Spells
{
    SPELL_ACID_BREATH = 12884,
    SPELL_WING_FLAP   = 12882,
};

enum Events
{
    EVENT_ACID_BREATH = 1,
    EVENT_WING_FLAP   = 2,
};

class boss_wardens_of_the_dream : public CreatureScript
{
    public:
        boss_wardens_of_the_dream() : CreatureScript("boss_wardens_of_the_dream") { }

        struct boss_wardens_of_the_dreamAI : public BossAI
        {
            boss_wardens_of_the_dreamAI(Creature* creature) : BossAI(creature, DATA_WARDENS) { }

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
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                DoZoneInCombat();
                instance->SetBossState(DATA_WARDENS, IN_PROGRESS);

                events.ScheduleEvent(EVENT_ACID_BREATH, urand(3 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WING_FLAP, urand(9 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                summons.DespawnAll();
                scheduler.CancelAll();
                Talk(SAY_DEATH);
                instance->SetData(DATA_WARDENS, DONE);
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
                        case EVENT_ACID_BREATH:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_ACID_BREATH);

                            events.ScheduleEvent(EVENT_ACID_BREATH, urand(6 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                            break;
                        case EVENT_WING_FLAP:
                            DoCast(me, SPELL_WING_FLAP);
                            events.ScheduleEvent(EVENT_WING_FLAP, urand(19 * IN_MILLISECONDS, 34 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_wardens_of_the_dreamAI(creature);
        }
};

void AddSC_wardens_of_the_dream()
{
    new boss_wardens_of_the_dream();
}