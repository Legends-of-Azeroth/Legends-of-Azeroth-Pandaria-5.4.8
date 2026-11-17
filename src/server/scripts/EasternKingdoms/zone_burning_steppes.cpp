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

/* ScriptData
SDName: Burning_Steppes
SD%Complete: 0
SDComment:
SDCategory: Burning Steppes
EndScriptData */

#include "Random.h"

enum ScrappedGolemsType
{
    SPELL_CREATE_WAR_REAVER_PARTS_AB = 89413, // Sieve & Piston
    SPELL_CREATE_WAR_REAVER_PARTS_AC = 89416, // Sieve & Gearshaft
    SPELL_CREATE_WAR_REAVER_PARTS_AD = 89417, // Sieve & Core
    SPELL_CREATE_WAR_REAVER_PARTS_BC = 89418, // Piston & Gearshaft
    SPELL_UPPERCUTE                  = 10966,
    SPELL_FLAME_BLAST                = 84165,
    SPELL_BATTLE_READY               = 89353, // first defeat
    SPELL_WELL_HONED                 = 89352, // second defeat
    SPELL_PERFECTLY_TUNNED           = 89355, // credit after this
    SPELL_HEAVE                      = 91314,
    SPELL_FLAME_BLAST_TRAIN          = 91313,

    EVENT_UPPERCUTE = 1,
    EVENT_FLAME_BLAST,

    ITEM_OBSIDIAN_PISTON             = 63333,
    ITEM_FLUX_EXHAUST_SIEVE          = 63336,
    ITEM_THORIUM_GEARSHAFT           = 63335,
    ITEM_STONE_POWER_CORE            = 63334,

    GO_PART_AB                       = 206971,
    GO_PART_AC                       = 206972,
    GO_PART_AD                       = 206973,
    GO_PART_BC                       = 206974,

    QUEST_GOLEM_TRAINING             = 28227,

    ACTION_GOLEM_TRAINING = 0,

    TALK_DEFEAT_1 = 0,
    TALK_OVERRIDE,
    TALK_DEFEAT_2,
    TALK_TRAINING_SUCCESS,
};

const std::list<uint32> reaverSpellParts =
{
    SPELL_CREATE_WAR_REAVER_PARTS_AB,
    SPELL_CREATE_WAR_REAVER_PARTS_AC,
    SPELL_CREATE_WAR_REAVER_PARTS_AD,
    SPELL_CREATE_WAR_REAVER_PARTS_BC,
};

const std::map<uint32, std::array<uint32, 2>> golemParts =
{
    { GO_PART_AB, { ITEM_FLUX_EXHAUST_SIEVE,ITEM_OBSIDIAN_PISTON } },
    { GO_PART_AC, { ITEM_FLUX_EXHAUST_SIEVE, ITEM_THORIUM_GEARSHAFT } },
    { GO_PART_AD, { ITEM_FLUX_EXHAUST_SIEVE, ITEM_STONE_POWER_CORE } },
    { GO_PART_BC, { ITEM_OBSIDIAN_PISTON, ITEM_THORIUM_GEARSHAFT } },
};

// War Reaver 7039
struct npc_burning_steppes_war_reaver : public customCreatureAI
{
    npc_burning_steppes_war_reaver(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_UPPERCUTE, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(SPELL_FLAME_BLAST, 8 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!roll_chance_i(80))
            return;

        DoCast(me, Trinity::Containers::SelectRandomContainerElement(reaverSpellParts));
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
            ExecuteTargetEvent(SPELL_UPPERCUTE, 10.5 * IN_MILLISECONDS, EVENT_UPPERCUTE, eventId);
            ExecuteTargetEvent(SPELL_FLAME_BLAST, urand(12 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_FLAME_BLAST, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// War Reaver Parts 206971, 206972, 206973, 206974
class go_burning_steppes_war_reaver_part : public GameObjectScript
{
    public:
        go_burning_steppes_war_reaver_part() : GameObjectScript("go_burning_steppes_war_reaver_part") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(go), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(go), 1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            // Send item to player
            auto itr = golemParts.find(go->GetEntry());
            player->AddItem(action == GOSSIP_ACTION_INFO_DEF + 1 ? itr->second[0] : itr->second[1], 1);

            player->CLOSE_GOSSIP_MENU();
            go->Delete();

            return true;
        }
};

// Chiseled Golem 48037
class npc_burning_steppes_chiseled_golem : public CreatureScript
{
    public:
        npc_burning_steppes_chiseled_golem() : CreatureScript("npc_burning_steppes_chiseled_golem") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->AI()->SetGUID(player->GetGUID());
                creature->AI()->DoAction(ACTION_GOLEM_TRAINING);
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetQuestStatus(QUEST_GOLEM_TRAINING) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_burning_steppes_chiseled_golemAI : public customCreatureAI
        {
            npc_burning_steppes_chiseled_golemAI(Creature* creature) : customCreatureAI(creature) { }

            TaskScheduler scheduler;
            ObjectGuid targetGUID;
            bool hasDefeat;

            void Reset() override
            {
                events.Reset();
                scheduler.CancelAll();
                targetGUID = ObjectGuid::Empty;
                hasDefeat = false;
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetFaction(1474); // default
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                targetGUID = guid;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_GOLEM_TRAINING)
                {
                    me->SetFaction(16);

                    if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                        AttackStart(target);
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (hasDefeat)
                    damage = 0;

                if (damage >= me->GetHealth() && !hasDefeat)
                {
                    damage = 0;
                    hasDefeat = true;

                    // select powerful depend of current
                    if (!me->HasAura(SPELL_BATTLE_READY))
                    {
                        me->PrepareChanneledCast(me->GetOrientation());
                        DoCast(me, SPELL_BATTLE_READY);
                        Talk(TALK_DEFEAT_1);

                        scheduler
                            .Schedule(Seconds(5), [this](TaskContext context)
                        {
                            me->SetFullHealth();
                            Talk(TALK_OVERRIDE);
                            hasDefeat = false;

                            if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                                me->RemoveChanneledCast(target->GetGUID());

                            events.ScheduleEvent(EVENT_FLAME_BLAST, 2 * IN_MILLISECONDS);
                        });
                    }
                    else if (!me->HasAura(SPELL_WELL_HONED))
                    {
                        me->PrepareChanneledCast(me->GetOrientation());
                        DoCast(me, SPELL_WELL_HONED);
                        Talk(TALK_DEFEAT_2);

                        scheduler
                            .Schedule(Seconds(5), [this](TaskContext context)
                        {
                            me->SetFullHealth();
                            Talk(TALK_OVERRIDE);
                            hasDefeat = false;

                            if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                                me->RemoveChanneledCast(target->GetGUID());

                            events.ScheduleEvent(EVENT_UPPERCUTE, 2 * IN_MILLISECONDS);
                        });
                    }
                    else // training success
                    {
                        Talk(TALK_TRAINING_SUCCESS);
                        DoCast(me, SPELL_PERFECTLY_TUNNED);
                        me->PrepareChanneledCast(me->GetOrientation());

                        if (Player* target = ObjectAccessor::GetPlayer(*me, targetGUID))
                            target->KilledMonsterCredit(me->GetEntry());

                        me->DespawnOrUnsummon(4 * IN_MILLISECONDS);
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    ExecuteTargetEvent(SPELL_HEAVE, 10.5 * IN_MILLISECONDS, EVENT_UPPERCUTE, eventId);
                    ExecuteTargetEvent(SPELL_FLAME_BLAST_TRAIN, urand(12 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_FLAME_BLAST, eventId, PRIORITY_CHANNELED);
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_burning_steppes_chiseled_golemAI(creature);
        }
};

void AddSC_burning_steppes()
{
    new creature_script<npc_burning_steppes_war_reaver>("npc_burning_steppes_war_reaver");
    new go_burning_steppes_war_reaver_part();
    new npc_burning_steppes_chiseled_golem();
}
