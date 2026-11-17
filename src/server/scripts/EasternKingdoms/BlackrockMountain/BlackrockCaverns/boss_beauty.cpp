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
#include "blackrock_caverns.h"

//todo: реализовать касты щенков, берсерк при убийстве ранти, взрыв magma split
enum Spells
{
    SPELL_BERSERK                  = 82395,
    SPELL_BERSERKER_CHARGE         = 76030,
    SPELL_FLAMEBREAK               = 76032,
    SPELL_MAGMA_SPLIT              = 76031,
    SPELL_TERRIFYING_ROAR          = 76028,
};

enum events
{
    EVENT_MAGMA_SPLIT              = 1,
    EVENT_TERRIFYING_ROAR          = 2,
    EVENT_FLAMEBREAK               = 3,
    EVENT_BERSERKER_CHARGE         = 4,
};

enum Adds
{
    NPC_SPOT                       = 40011,
    NPC_BUSTER                     = 40013,
    NPC_LUCKY                      = 40008,
    NPC_RUNTY                      = 40015,
};

const Position pupsPos[2] = 
{
    { 116.70f, 572.05f, 76.45f, 0.04f },
    { 119.81f, 588.90f, 76.35f, 6.22f }
};

class boss_beauty : public CreatureScript
{
    public:
        boss_beauty() : CreatureScript("boss_beauty") { }
     
        struct boss_beautyAI : public BossAI
        {
            boss_beautyAI(Creature* creature) : BossAI(creature, DATA_BEAUTY)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->setActive(true);
            }

            void Reset() override
            {
                _Reset();
                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_BERSERKER_CHARGE, 1000);
                events.ScheduleEvent(EVENT_FLAMEBREAK, 15000);
                events.ScheduleEvent(EVENT_MAGMA_SPLIT, 18000);
                events.ScheduleEvent(EVENT_TERRIFYING_ROAR, 30000);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }
     
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERKER_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                                DoCast(target, SPELL_BERSERKER_CHARGE);
                            events.ScheduleEvent(EVENT_BERSERKER_CHARGE, 10000);
                            break;
                        case EVENT_FLAMEBREAK:
                            DoCast(me, SPELL_FLAMEBREAK);
                            events.ScheduleEvent(EVENT_FLAMEBREAK, 15000);
                            break;
                        case EVENT_MAGMA_SPLIT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_MAGMA_SPLIT);
                            events.ScheduleEvent(EVENT_MAGMA_SPLIT, 18000);
                            break;
                        case EVENT_TERRIFYING_ROAR:
                            DoCast(SPELL_TERRIFYING_ROAR);
                            events.ScheduleEvent(EVENT_TERRIFYING_ROAR, 30000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                //Creature* pups[2];
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_beautyAI>(creature);
        }
};
 
void AddSC_boss_beauty()
{
    new boss_beauty();
}
