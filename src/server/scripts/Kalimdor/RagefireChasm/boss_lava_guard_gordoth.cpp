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

class boss_lava_guard_gordoth : public CreatureScript
{
    public:
        boss_lava_guard_gordoth() : CreatureScript("boss_lava_guard_gordoth") { }

        enum iSpells
        {
            SPELL_ENRAGE         = 145692,
            SPELL_SIESMIC_SLAM   = 120024,
            SPELL_GROUND_RUPTURE = 119999,
        };

        enum iEvents
        {
            EVENT_SLAM   = 1,
            EVENT_RUPTURE= 2,
        };

        struct boss_lava_guard_gordothAI : public BossAI
        {
            boss_lava_guard_gordothAI(Creature* creature) : BossAI(creature, BOSS_GORDOTH) { }

            bool hasEnrage;

            void InitializeAI() override
            {
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                summons.DespawnAll();
                hasEnrage = false;

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
                    instance->SetBossState(BOSS_GORDOTH, IN_PROGRESS);
                }

                events.ScheduleEvent(EVENT_RUPTURE, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SLAM, urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(30) && !hasEnrage)
                {
                    hasEnrage = true;
                    me->CastSpell(me, SPELL_ENRAGE, false);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(BOSS_GORDOTH, DONE);
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
                        case EVENT_SLAM:
                            DoCast(me, SPELL_SIESMIC_SLAM);
                            events.ScheduleEvent(EVENT_SLAM, urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                        case EVENT_RUPTURE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_GROUND_RUPTURE, false);
                            else if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_GROUND_RUPTURE);

                            events.ScheduleEvent(EVENT_RUPTURE, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lava_guard_gordothAI(creature);
        }
};

void AddSC_boss_lava_guard_gordoth()
{
    new boss_lava_guard_gordoth();
}
