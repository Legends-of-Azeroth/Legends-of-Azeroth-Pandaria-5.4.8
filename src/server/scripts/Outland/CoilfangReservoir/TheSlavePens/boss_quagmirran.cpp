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
    SPELL_CLEAVE                = 40504,
    SPELL_UPPERCUT              = 32055,
    SPELL_ACID_SPRAY            = 38153,
    SPELL_POISON_BOLT_VOLLEY    = 34780,
    SPELL_POISON_BOLT_VOLLEY_HC = 39340,
};

enum Events
{
    EVENT_CLEAVE             = 1,
    EVENT_UPPERCUT           = 2,
    EVENT_ACID_SPRAY         = 3,
    EVENT_POISON_BOLT_VOLLEY = 4,
};

class boss_quagmirran : public CreatureScript
{
    public:
        boss_quagmirran() : CreatureScript("boss_quagmirran") { }

        struct boss_quagmirranAI : public BossAI
        {
            boss_quagmirranAI(Creature* creature) : BossAI(creature, DATA_QUAGMIRRAN) { }

            void Reset() override
            {
                _Reset();
                events.Reset();

                if (instance)
                    instance->SetData(DATA_QUAGMIRRAN, NOT_STARTED);
            }

            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();

                events.ScheduleEvent(EVENT_CLEAVE, urand(9 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_UPPERCUT, urand(20 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_POISON_BOLT_VOLLEY, urand(25 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();

                if (instance)
                    instance->SetData(DATA_QUAGMIRRAN, FAIL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
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
                        case EVENT_CLEAVE:
                            if (Unit* victim = me->GetVictim())
                                DoCast(victim, SPELL_CLEAVE, true);

                            events.ScheduleEvent(EVENT_CLEAVE, urand(9.5 * IN_MILLISECONDS, 34.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_UPPERCUT:
                            if (Unit* victim = me->GetVictim())
                                DoCast(victim, SPELL_UPPERCUT);

                            events.ScheduleEvent(EVENT_UPPERCUT, urand(20 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            events.ScheduleEvent(EVENT_ACID_SPRAY, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_ACID_SPRAY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_ACID_SPRAY);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_ACID_SPRAY);
                            break;
                        case EVENT_POISON_BOLT_VOLLEY:
                            DoCastAOE(IsHeroic() ? SPELL_POISON_BOLT_VOLLEY_HC : SPELL_POISON_BOLT_VOLLEY);
                            events.ScheduleEvent(EVENT_POISON_BOLT_VOLLEY, urand(27 * IN_MILLISECONDS, 31 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_quagmirranAI(creature);
        }
};

enum eQuestMisc
{
    QUEST_LOST_IN_ACTIONS = 29563,
};

class npc_naturalist_bite : public CreatureScript
{
    public:
        npc_naturalist_bite() : CreatureScript("npc_naturalist_bite") { }
    
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
    
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->KilledMonsterCredit(creature->GetEntry());
    
            player->CLOSE_GOSSIP_MENU();
    
            return true;
        }
    
        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());
    
            if (player->GetQuestStatus(QUEST_LOST_IN_ACTIONS) != QUEST_STATUS_INCOMPLETE)
                return false;
    
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

void AddSC_boss_quagmirran()
{
    new boss_quagmirran();
    new npc_naturalist_bite();
}