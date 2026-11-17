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

#include "ragefire_chasm.h"

class boss_dark_shaman_koranthal : public CreatureScript
{
    public:
        boss_dark_shaman_koranthal() : CreatureScript("boss_dark_shaman_koranthal") { }

        enum iSpells
        {
            SPELL_TWISTED_ELEMENTS = 119300,
            SPELL_SHADOW_STORM     = 119971,
        };

        enum iEvents
        {
            EVENT_TWISTED_ELEMENTS = 1,
            EVENT_SHADOW_STORM     = 2,
        };

        struct boss_dark_shaman_koranthalAI : public BossAI
        {
            boss_dark_shaman_koranthalAI(Creature* creature) : BossAI(creature, BOSS_DARK_SHAMAN_CORANTHAL) { }

            void InitializeAI() override
            {
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* who) override
            {
                // @TODO: Set in combat for other protectors
                _JustEngagedWith();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(BOSS_DARK_SHAMAN_CORANTHAL, IN_PROGRESS);
                }

                events.ScheduleEvent(EVENT_TWISTED_ELEMENTS, urand(4 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOW_STORM, 20 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(BOSS_DARK_SHAMAN_CORANTHAL, DONE);
                }
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
                        case EVENT_TWISTED_ELEMENTS:
                            if (me->HasAura(SPELL_SHADOW_STORM))
                            {
                                events.RescheduleEvent(EVENT_TWISTED_ELEMENTS, 8 * IN_MILLISECONDS);
                                break;
                            }

                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_TWISTED_ELEMENTS, false);

                            events.ScheduleEvent(EVENT_TWISTED_ELEMENTS, urand(4 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                            break;
                        case EVENT_SHADOW_STORM:
                            me->CastSpell(me, SPELL_SHADOW_STORM, false);
                            events.ScheduleEvent(EVENT_SHADOW_STORM, 20 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_dark_shaman_koranthalAI(creature);
        }
};

void AddSC_boss_dark_shaman_koranthal()
{
    new boss_dark_shaman_koranthal();
}
