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
#include "arcatraz.h"

enum Spells
{
    SPELL_VOID_ZONE              = 36119,
    SPELL_SEED_OF_THE_CORRUPTION = 36123,
    SPELL_VOID_ZONE_DAMAGE       = 36120,
    SPELL_SHADOW_NOVA            = 36127,
    SPELL_SHADOW_NOVA_HC         = 39005,
};

enum Events
{
    EVENT_VOID_ZONE   = 1,
    EVENT_SEED        = 2,
    EVENT_SHADOW_NOVA = 3,
};

enum Yells
{
    TALK_AGGRO       = 0,
    TALK_SLAY        = 1,
    TALK_SHADOW_NOVA = 2,
    TALK_DEATH       = 3,
};

class boss_zereketh_the_unbound : public CreatureScript
{
    public:
        boss_zereketh_the_unbound() : CreatureScript("boss_zereketh_the_unbound") { }

        struct boss_zereketh_the_unboundAI : public BossAI
        {
            boss_zereketh_the_unboundAI(Creature* creature) : BossAI(creature, TYPE_ZEREKETH) { }

            void Reset() override
            {
                _Reset();
                events.Reset();

                if (instance)
                    instance->SetData(TYPE_ZEREKETH, NOT_STARTED);
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();
                Talk(TALK_AGGRO);

                events.ScheduleEvent(EVENT_VOID_ZONE, 14 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SEED, 25 * IN_MILLISECONDS);
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
                    instance->SetData(TYPE_ZEREKETH, FAIL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_DEATH);

                if (instance)
                    instance->SetData(TYPE_ZEREKETH, DONE);
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
                        case EVENT_VOID_ZONE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_VOID_ZONE, true);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_VOID_ZONE, true);

                            events.ScheduleEvent(EVENT_VOID_ZONE, urand(6 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            events.ScheduleEvent(EVENT_SHADOW_NOVA, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        case EVENT_SEED:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SEED_OF_THE_CORRUPTION);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_SEED_OF_THE_CORRUPTION);

                            events.ScheduleEvent(EVENT_SEED, 25 * IN_MILLISECONDS);
                            break;
                        case EVENT_SHADOW_NOVA:
                            Talk(TALK_SHADOW_NOVA);
                            DoCast(me, IsHeroic() ? SPELL_SHADOW_NOVA_HC : SPELL_SHADOW_NOVA);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_zereketh_the_unboundAI(creature);
        }
};

class npc_zerekethvoidzone : public CreatureScript
{
    public:
        npc_zerekethvoidzone() : CreatureScript("npc_zerekethvoidzone") { }

        struct npc_zerekethvoidzoneAI : public ScriptedAI
        {
            npc_zerekethvoidzoneAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetUInt32Value(UNIT_FIELD_NPC_FLAGS, 0);
                me->SetFaction(16);

                DoCast(me, SPELL_VOID_ZONE_DAMAGE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zerekethvoidzoneAI(creature);
        }
};

void AddSC_boss_zereketh_the_unbound()
{
    new boss_zereketh_the_unbound();
    new npc_zerekethvoidzone();
}