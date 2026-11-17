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

enum ScriptedTexts
{
    // Lord Hiram Creed
    SAY_AGGRO   = 0,
    SAY_DRAGON  = 1,
    SAY_DEATH   = 2,
};

enum Spells
{
    // A Hidden Message
    SPELL_CHARGE_DECODER_RING               = 104480,
    SPELL_GIVE_PLAYER_SIGNED_CIPHER         = 104569,

    // To Catch a Thief
    SPELL_SEE_MOSTRASZ_IN_RRAVENHOLDT_CAVE  = 105968,
    SPELL_MOSTRASZS_VISION                  = 106015,

    // Lord Hiram Creed
    SPELL_BERSERK                           = 110055,
    SPELL_BLACKHOWLS_WILL                   = 109664,
    SPELL_BLACKHOWLS_RAGE                   = 109706,
    SPELL_CONSUMING_DARKNESS                = 109196,
    SPELL_SHADOW_BREATH                     = 109669,
    SPELL_SHADOW_BREATH_SUMMON              = 109673,
    SPELL_DRAGON_TRANSFORMATION             = 109232,
};

enum Events
{
    // Lord Hiram Creed
    EVENT_BERSERK               = 1,
    EVENT_BLACKHOWLS_WILL       = 2,
    EVENT_CONSUMING_DARKNESS    = 3,
    EVENT_SHADOW_BREATH         = 4,
    EVENT_CHECK_PLAYERS         = 5,
    EVENT_CHECK_DISTANCE        = 6,
    EVENT_CONTINUE              = 7,
};

enum Adds
{
    NPC_THAUMATURGE_RAFIR            = 57800,
    NPC_HIRAM_CREEDS_SHADOW_BREATH   = 58002,
};

enum QuestIds
{
    QUEST_TO_RAVENHOLDT         = 29934,
    QUEST_TO_CATCH_A_THIEF      = 29847,
    QUEST_OUR_MAN_IN_GILNEAS    = 30092,
};

enum QuestItems
{
    ITEM_CHARGING_DECORDER_RING = 74749,
    ITEM_CHARGED_DECORDER_RING  = 74748,
};

class item_charging_decorder_ring : public ItemScript
{
    public:
        item_charging_decorder_ring () : ItemScript("item_charging_decorder_ring") { }

        bool OnExpire(Player* player, ItemTemplate const* /*itemProto*/) override
        {
            player->KilledMonsterCredit(55691, ObjectGuid::Empty);
            player->AddItem(ITEM_CHARGED_DECORDER_RING, 1);
            return true;
        }
};

class npc_mostrasz_56314 : public CreatureScript
{
    public:
        npc_mostrasz_56314() : CreatureScript("npc_mostrasz_56314") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (player->GetQuestStatus(QUEST_TO_RAVENHOLDT) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(56314, ObjectGuid::Empty);
                player->TeleportTo(0, 14.685211f, -1463.118896f, 173.92f, 6.23f);
            }
            
            return true;
        }
};

class npc_wrathion_57777 : public CreatureScript
{
    public:
        npc_wrathion_57777() : CreatureScript("npc_wrathion_57777") { }

        struct npc_wrathion_57777AI : ScriptedAI
        {
            npc_wrathion_57777AI(Creature* creature) : ScriptedAI(creature) { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!me->IsWithinDistInMap(who, 20.0f))
                    return;

                if (who->ToPlayer()->GetQuestStatus(QUEST_TO_CATCH_A_THIEF) == QUEST_STATUS_INCOMPLETE)
                    who->ToPlayer()->KilledMonsterCredit(56426, ObjectGuid::Empty);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wrathion_57777AI(creature);
        }
};

class npc_lord_hiram_creed : public CreatureScript
{
    public:
        npc_lord_hiram_creed() : CreatureScript("npc_lord_hiram_creed") { }

        struct npc_lord_hiram_creedAI : ScriptedAI
        {
            npc_lord_hiram_creedAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            void Reset() override
            {
                events.Reset();
                me->SetReactState(REACT_DEFENSIVE);
                bDragon = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_BERSERK, 4 * MINUTE * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CONSUMING_DARKNESS, 6000);
                events.ScheduleEvent(EVENT_BLACKHOWLS_WILL, 14000);
                events.ScheduleEvent(EVENT_CHECK_PLAYERS, 2000);
                events.ScheduleEvent(EVENT_CHECK_DISTANCE, 5000);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_HIRAM_CREEDS_SHADOW_BREATH)
                {   
                    me->SetFacingToObject(summon);
                    DoCast(me, SPELL_SHADOW_BREATH);
                }
            }

            void JustDied(Unit* /*who*/) override
            {
                events.Reset();
                Talk(SAY_DEATH);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(75) && !bDragon)
                {
                    bDragon = true;
                    Talk(SAY_DRAGON);
                    events.DelayEvents(3000);
                    events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(10000, 25000));
                    DoCast(me, SPELL_DRAGON_TRANSFORMATION);
                    return;
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_PLAYERS:
                        {
                            std::list<Unit*> targets;
                            SelectTargetList(targets, PlayerClassSelector(), 1, SELECT_TARGET_RANDOM);
                            if (!targets.empty())
                            {
                                EnterEvadeMode();
                                return;
                            }
                            events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5000);
                            break;
                        }
                        case EVENT_CHECK_DISTANCE:
                            if (me->GetDistance(me->GetHomePosition()) > 100.0f)
                            {
                                EnterEvadeMode();
                                return;
                            }
                            events.ScheduleEvent(EVENT_CHECK_DISTANCE, 5000);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_BLACKHOWLS_WILL:
                            DoCastVictim(SPELL_BLACKHOWLS_WILL);
                            events.ScheduleEvent(EVENT_BLACKHOWLS_WILL, urand(20000, 25000));
                            break;
                        case EVENT_CONSUMING_DARKNESS:
                            DoCastVictim(SPELL_CONSUMING_DARKNESS);
                            events.ScheduleEvent(EVENT_CONSUMING_DARKNESS, 20000);
                            break;
                        case EVENT_SHADOW_BREATH:
                            me->SetReactState(REACT_PASSIVE);
                            DoCastVictim(SPELL_SHADOW_BREATH_SUMMON);
                            events.ScheduleEvent(EVENT_CONTINUE, 5000);
                            events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(23000, 28000));
                            break;
                        case EVENT_CONTINUE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            AttackStart(me->GetVictim());
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool bDragon;
            EventMap events;

            struct PlayerClassSelector
            {
                public:
                    PlayerClassSelector() {}

                    bool operator()(Unit const* target) const
                    {
                        if (target->GetTypeId() != TYPEID_PLAYER)
                            return false;

                        if (target->GetClass() == CLASS_ROGUE)
                            return false;

                        return true;
                    }
            };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lord_hiram_creedAI(creature);
        }
};

void AddSC_fangs_of_father_scripts()
{
    new item_charging_decorder_ring();
    new npc_mostrasz_56314();
    new npc_wrathion_57777();
    new npc_lord_hiram_creed();
}
