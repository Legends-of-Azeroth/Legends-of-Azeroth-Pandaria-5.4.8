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
    SPELL_THRASH       = 3391,
    SPELL_ACID_BREATH  = 12891,
    SPELL_DEEP_SLUMBER = 12890,
    SPELL_WAR_STOMP    = 11876,
};

enum Events
{
    EVENT_THRASH       = 1,
    EVENT_ACID_BREATH  = 2,
    EVENT_DEEP_SLUMBER = 3,
    EVENT_WAR_STOMP    = 4,
};

class boss_shade_of_eranikus : public CreatureScript
{
    public:
        boss_shade_of_eranikus() : CreatureScript("boss_shade_of_eranikus") { }

        struct boss_shade_of_eranikusAI : public BossAI
        {
            boss_shade_of_eranikusAI(Creature* creature) : BossAI(creature, DATA_ERANIKUS) { }

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
                me->HandleEmoteStateCommand(EMOTE_STATE_DROWNED);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(SAY_AGGRO);
                DoZoneInCombat();
                instance->SetBossState(DATA_ERANIKUS, IN_PROGRESS);

                events.ScheduleEvent(EVENT_THRASH, urand(2 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ACID_BREATH, urand(7 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEEP_SLUMBER, urand(14 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WAR_STOMP, urand(17 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
            }

            void DoAction(int32 actionId) override
            {
                me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                instance->SetData(DATA_ERANIKUS, DONE);
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
                        case EVENT_THRASH:
                            events.ScheduleEvent(EVENT_THRASH, urand(3 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            DoCast(me, SPELL_THRASH);
                            break;
                        case EVENT_ACID_BREATH:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_ACID_BREATH, false);

                            events.ScheduleEvent(EVENT_ACID_BREATH, urand(13 * IN_MILLISECONDS, 32 * IN_MILLISECONDS));
                            break;
                        case EVENT_DEEP_SLUMBER:
                            if (Unit* vict = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(vict, SPELL_DEEP_SLUMBER, false);

                            events.ScheduleEvent(EVENT_DEEP_SLUMBER, urand(18 * IN_MILLISECONDS, 39 * IN_MILLISECONDS));
                            break;
                        case EVENT_WAR_STOMP:
                            DoCast(me, SPELL_WAR_STOMP);
                            events.ScheduleEvent(EVENT_WAR_STOMP, urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_shade_of_eranikusAI(creature);
        }
};

class at_sunket_temple_eranikus : public AreaTriggerScript
{
    public:
        at_sunket_temple_eranikus() : AreaTriggerScript("at_sunket_temple_eranikus") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_HAKKAR) == DONE && instance->GetBossState(DATA_JAMMAL) == DONE && instance->GetBossState(DATA_WARDENS) == DONE)
                    if (Creature* m_eranikus = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_SHADE_OF_ERANIKUS)))
                        m_eranikus->AI()->DoAction(ACTION_ERANIKUS_DREAM);
            }

            return false;
    }
};

void AddSC_shade_of_eranikus()
{
    new boss_shade_of_eranikus();
    new at_sunket_temple_eranikus();
}