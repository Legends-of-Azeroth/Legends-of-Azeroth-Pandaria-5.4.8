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
#include "the_slave_pens.h"

enum Spells
{
    SPELL_TAINTED_STONESKIN_TOTEM = 31985,
    SPELL_TAINTED_EARTHGRAB_TOTEM = 31981,
    SPELL_CORRUPTED_NOVE_TOTEM    = 31991,
    SPELL_MENNUS_HEALING_WARD     = 34980,
    SPELL_LIGHTNING_BOLT          = 35010,

    /*Totem Spells*/
    SPELL_EARTHGRAB               = 31983,
    SPELL_FIRE_NOVA               = 31992,
    SPELL_STONESKIN               = 31986,
    SPELL_HEALING_WARD_PASSIVE    = 34978,
};

enum Events
{
    EVENT_STONESKIN_TOTEM = 1,
    EVENT_EARTHGRAB_TOTEM = 2,
    EVENT_NOVE_TOTEM      = 3,
    EVENT_HEALING_WARD    = 4,
    EVENT_LIGHTNING_BOLT  = 5,
};

enum Talks
{
    TALK_AGGRO = 0,
    TALK_SLAY  = 1,
    TALK_DEATH = 2,
};

class boss_mennu_the_betrayer : public CreatureScript
{
    public:
        boss_mennu_the_betrayer() : CreatureScript("boss_mennu_the_betrayer") { }

        struct boss_mennu_the_betrayerAI : public BossAI
        {
            boss_mennu_the_betrayerAI(Creature* creature) : BossAI(creature, DATA_MENNU_THE_BETRAYER) { }

            void Reset() override
            {
                _Reset();
                events.Reset();

                if (instance)
                    instance->SetData(DATA_MENNU_THE_BETRAYER, NOT_STARTED);
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(TALK_AGGRO);
                _JustEngagedWith();

                events.ScheduleEvent(EVENT_STONESKIN_TOTEM, urand(18 * IN_MILLISECONDS, 26 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EARTHGRAB_TOTEM, urand(19 * IN_MILLISECONDS, 26.2 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_NOVE_TOTEM, urand(20 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEALING_WARD, urand(5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 3 * IN_MILLISECONDS);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();

                if (instance)
                    instance->SetData(DATA_MENNU_THE_BETRAYER, FAIL);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);
                summon->AddUnitState(UNIT_STATE_ROOT);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);

                switch (summon->GetEntry())
                {
                    case NPC_EARTHGRAB_TOTEM:
                        summon->SetDisplayId(20230);
                        summon->CastSpell(summon, SPELL_EARTHGRAB, false);
                        break;
                    case NPC_NOVA_TOTEM:
                        summon->SetDisplayId(20231);
                        summon->CastSpell(summon, SPELL_FIRE_NOVA, false);
                        break;
                    case NPC_STONESKIN_TOTEM:
                        summon->SetDisplayId(16997);
                        summon->CastSpell(summon, SPELL_STONESKIN, false);
                        break;
                    case NPC_HEALING_WARD:
                        summon->CastSpell(summon, SPELL_HEALING_WARD_PASSIVE, false);
                        break;
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_DEATH);
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
                        case EVENT_STONESKIN_TOTEM:
                            DoCast(me, SPELL_TAINTED_STONESKIN_TOTEM);
                            events.ScheduleEvent(EVENT_STONESKIN_TOTEM, urand(18 * IN_MILLISECONDS, 26 * IN_MILLISECONDS));
                            break;
                        case EVENT_EARTHGRAB_TOTEM:
                            DoCast(me, SPELL_TAINTED_EARTHGRAB_TOTEM);
                            events.ScheduleEvent(EVENT_EARTHGRAB_TOTEM, urand(19 * IN_MILLISECONDS, 26.2 * IN_MILLISECONDS));
                            break;
                        case EVENT_NOVE_TOTEM:
                            DoCast(me, SPELL_CORRUPTED_NOVE_TOTEM);
                            events.ScheduleEvent(EVENT_NOVE_TOTEM, urand(20 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                            break;
                        case EVENT_HEALING_WARD:
                            DoCast(me, SPELL_MENNUS_HEALING_WARD);
                            events.ScheduleEvent(EVENT_HEALING_WARD, urand(8 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
                            break;
                        case EVENT_LIGHTNING_BOLT:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_LIGHTNING_BOLT);

                            events.ScheduleEvent(EVENT_LIGHTNING_BOLT, urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_mennu_the_betrayerAI(creature);
        }
};

void AddSC_boss_mennu_the_betrayer()
{
    new boss_mennu_the_betrayer();
}