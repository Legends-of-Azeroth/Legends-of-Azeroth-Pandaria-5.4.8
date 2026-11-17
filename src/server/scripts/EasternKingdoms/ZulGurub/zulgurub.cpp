/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

#include"ScriptPCH.h"
#include "zulgurub.h"

enum Spells
{
    TIKI_BURN = 96861,

    CURSED_ARTIFACT_VISUAL = 97040
};

enum Texts
{
    GOSSIP_TEXT_ARTIFACT_CANNOT_BE_ACTIVATED = 17952,
    GOSSIP_TEXT_ARTIFACT_CAN_BE_ACTIVATED    = 17954,
    GOSSIP_TEXT_ARTIFACT_ACTIVATED           = 17955
};

enum Menus
{
    GOSSIP_MENU_ARTIFACT_ACTIVATE = 12765
};

enum Options
{
    GOSSIP_OPTION_ARTIFACT_ACTIVATE = 0
};

enum Creatures
{    
    NPC_RITUAL_TIKI_MASK = 52364
};

enum Events
{
    EVENT_TIKI_BURN = 1,

    EVENT_ARTIFACT_SUMMON = 1
};

class npc_ritual_tiki_mask : public CreatureScript
{
    public:
        npc_ritual_tiki_mask() : CreatureScript("npc_ritual_tiki_mask") { }

        struct npc_ritual_tiki_maskAI : public ScriptedAI
        {
            npc_ritual_tiki_maskAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetCanFly(true);
                me->SetDisableGravity(true);
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->SetCanFly(false);
                me->SetDisableGravity(false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_TIKI_BURN, urand(5000, 6000));
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
                        case EVENT_TIKI_BURN:
                            DoCast(me, TIKI_BURN);
                            events.ScheduleEvent(EVENT_TIKI_BURN, urand(10000, 15000));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ritual_tiki_maskAI>(creature);
        }
};

class npc_cache_of_madness_artifact : public CreatureScript
{
    public:
        npc_cache_of_madness_artifact() : CreatureScript("npc_cache_of_madness_artifact") { }
        
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ARTIFACT_ACTIVATED, creature->GetGUID());
                if (npc_cache_of_madness_artifactAI* ai = (npc_cache_of_madness_artifactAI*)creature->GetAI()) 
                {
                    ai->events.SetPhase(1);
                    ai->events.ScheduleEvent(EVENT_ARTIFACT_SUMMON, 5 * IN_MILLISECONDS);
                }
                return true;
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (!player)
                return true;

            if (npc_cache_of_madness_artifactAI* ai = (npc_cache_of_madness_artifactAI*)creature->GetAI())
                if (ai->events.IsInPhase(1))
                    return true;

            if (!player->HasSkill(SKILL_ARCHAEOLOGY) || player->GetSkillValue(SKILL_ARCHAEOLOGY) < 225)
                return true;

            if (InstanceScript* instance = player->GetInstanceScript())
            {
                uint32 dataId;
                switch (creature->GetEntry())
                {
                    case NPC_ARTIFACT_DWARVEN:
                        dataId = DATA_ARTIFACT_DWARVEN;
                        break;
                    case NPC_ARTIFACT_ELVEN:
                        dataId = DATA_ARTIFACT_ELVEN;
                        break;
                    case NPC_ARTIFACT_TROLL:
                        dataId = DATA_ARTIFACT_TROLL;
                        break;
                    case NPC_ARTIFACT_FOSSIL:
                        dataId = DATA_ARTIFACT_FOSSIL;
                        break;
                    default:
                        return true;
                }
                ObjectGuid guid = instance->GetGuidData(dataId);
                EncounterState state = instance->GetBossState(DATA_CACHE_OF_MADNESS_BOSS);
                if (creature->GetGUID() == guid && (state == NOT_STARTED || state == TO_BE_DECIDED))
                {
                    player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_ARTIFACT_ACTIVATE, GOSSIP_OPTION_ARTIFACT_ACTIVATE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ARTIFACT_CAN_BE_ACTIVATED, creature->GetGUID());
                    return true;
                }
            }

            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ARTIFACT_CANNOT_BE_ACTIVATED, creature->GetGUID());
            return true;
        }

        struct npc_cache_of_madness_artifactAI : public ScriptedAI
        {
            EventMap events;

            npc_cache_of_madness_artifactAI(Creature* creature) : ScriptedAI(creature) { }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARTIFACT_SUMMON:
                        {
                            int summonId = 0;
                            switch (me->GetEntry())
                            {
                                case NPC_ARTIFACT_DWARVEN:
                                    summonId = NPC_ARTIFACT_ACTIVE_DWARVEN;
                                    break;
                                case NPC_ARTIFACT_ELVEN:
                                    summonId = NPC_ARTIFACT_ACTIVE_ELVEN;
                                    break;
                                case NPC_ARTIFACT_TROLL:
                                    summonId = NPC_ARTIFACT_ACTIVE_TROLL;
                                    break;
                                case NPC_ARTIFACT_FOSSIL:
                                    summonId = NPC_ARTIFACT_ACTIVE_FOSSIL;
                                    break;
                                default:
                                    break;
                            }
                            if (summonId)
                            {
                                Position pos = me->GetPosition();
                                me->SummonCreature(summonId, pos);
                                me->SetPhaseMask(2, true);
                            }
                            if (InstanceScript* instance = me->GetInstanceScript())
                                instance->SetData(DATA_ARTIFACTS_ACTIVATED, instance->GetData(DATA_ARTIFACTS_ACTIVATED) + 1);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_cache_of_madness_artifactAI>(creature);
        }
};

class npc_cache_of_madness_artifact_active : public CreatureScript
{
    public:
        npc_cache_of_madness_artifact_active() : CreatureScript("npc_cache_of_madness_artifact_active") { }

        struct npc_cache_of_madness_artifact_activeAI : public ScriptedAI
        {
            npc_cache_of_madness_artifact_activeAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->CastSpell(me, CURSED_ARTIFACT_VISUAL, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_cache_of_madness_artifact_activeAI>(creature);
        }
};

void AddSC_zulgurub()
{
    new npc_ritual_tiki_mask();
    new npc_cache_of_madness_artifact();
    new npc_cache_of_madness_artifact_active();
};