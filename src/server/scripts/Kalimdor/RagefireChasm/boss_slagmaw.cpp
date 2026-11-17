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

class boss_slagmaw : public CreatureScript
{
    public:
        boss_slagmaw() : CreatureScript("boss_slagmaw") { }

        enum iSpells
        {
            SPELL_LAVA_SPIT         = 119434,
            SPELL_COSMETIC_SUBMERGE = 140483, // apply some flags and play anim up from lava at remove
            SPELL_SUBMERGE          = 120384, // apply some flags and player anim down to lava at apply
        };

        enum iEvents
        {
            EVENT_LAVA_SPIT = 1,
            EVENT_SUBMERGE  = 2,
        };

        struct boss_slagmawAI : public BossAI
        {
            boss_slagmawAI(Creature* creature) : BossAI(creature, BOSS_SLAGMAW) { }

            uint8 m_cPoint;

            void InitializeAI() override
            {
                SetCombatMovement(false);
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                summons.DespawnAll();
                m_cPoint = 1;

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
                    instance->SetBossState(BOSS_SLAGMAW, IN_PROGRESS);
                }

                events.ScheduleEvent(EVENT_LAVA_SPIT, urand(6 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SUBMERGE, 20 * IN_MILLISECONDS);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    me->CastSpell(me, SPELL_COSMETIC_SUBMERGE, true);
                    me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(BOSS_SLAGMAW, DONE);
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
                        case EVENT_LAVA_SPIT:
                            if (me->HasAura(SPELL_SUBMERGE) || me->HasAura(SPELL_COSMETIC_SUBMERGE))
                            {
                                events.RescheduleEvent(EVENT_LAVA_SPIT, 5 * IN_MILLISECONDS);
                                break;
                            }

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_LAVA_SPIT, false);
                            else if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_LAVA_SPIT);

                            events.ScheduleEvent(EVENT_LAVA_SPIT, urand(6 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                            break;
                        case EVENT_SUBMERGE:
                            std::vector<uint8> m_PointType = { 1, 2, 3 };

                            // Remove From Temp container value, that was used before (real random)
                            if (m_cPoint)
                                m_PointType.erase(std::find(m_PointType.begin(), m_PointType.end(), m_cPoint));

                            // Select new position from updated container
                            m_cPoint = Trinity::Containers::SelectRandomContainerElement(m_PointType);
                            me->CastSpell(me, SPELL_SUBMERGE, false);
                            me->GetMotionMaster()->MovePoint(0, SlagMawPoints[m_cPoint - 1]);

                            events.ScheduleEvent(EVENT_SUBMERGE, 30 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_slagmawAI(creature);
        }
};

void AddSC_boss_slagmaw()
{
    new boss_slagmaw();
}
