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
#include "halls_of_origination.h"

#define GOSSIP_SENDER_HALLS_OF_ORIGINATION 644

Position halls_of_origination_locs[] =
{
    { -536.372f,  193.001f, 80.238f,  0.000224f },
    { -506.194f, -337.028f, 162.363f, 1.55334f  },
    { -276.288f,  366.781f, 75.8439f, 3.08918f  },
};

enum ScriptTexts
{
    // Brann Bronzebeard
    SAY_0                       = 0,
    SAY_1                       = 1,
    SAY_2                       = 2,
    SAY_3                       = 3,
    SAY_4                       = 4,
    SAY_5                       = 5,
    SAY_6                       = 6,
    SAY_7                       = 7,
    SAY_8                       = 8,
    SAY_9                       = 9,
    SAY_10                      = 10,
};

enum Spells
{
    // Air Warden
    SPELL_WIND_SNEAR            = 77334,

    // Flame Warden
    SPELL_RAGING_INFERNO        = 77241,
    SPELL_RAGING_INFERNO_DMG    = 77262,
    SPELL_LAVA_ERUPTION         = 77273,

    // Water Warden
    SPELL_BUBBLE_BOUND          = 77336,

    // Earth Warden
    SPELL_IMPALE                = 77235,
    SPELL_ROCKWAVE              = 77234,
};

enum Events
{
    // Air Warden
    EVENT_WIND_SNEAR            = 1,

    // Flame Warden
    EVENT_LAVA_ERUPTION         = 2,
    EVENT_RAGING_INFERNO        = 3,

    // Water Warden         
    EVENT_BUBBLE_BOUND          = 4,

    // Earth Warden
    EVENT_IMPALE                = 5,
    EVENT_ROCKWAVE              = 6,

    // Brann Bronzebeard
    EVENT_TALK_0                = 7,
    EVENT_TALK_1                = 8,
    EVENT_TALK_2                = 9,
    EVENT_TALK_3                = 10,
    EVENT_TALK_4                = 11,
    EVENT_TALK_5                = 12,
    EVENT_TALK_6                = 13,
    EVENT_TALK_7                = 14,
    EVENT_TALK_8                = 15,
    EVENT_TALK_9                = 16,
    EVENT_TALK_10               = 17,
    EVENT_TALK_11               = 18,
};

enum Adds
{
    NPC_WHIRLING_WINDS          = 41245, // 77321
};

enum Actions
{
    ACTION_TALK_1               = 1,
    ACTION_TALK_2               = 2,
    ACTION_TALK_3               = 3,
    ACTION_TALK_4               = 4,
    ACTION_START_EVENT          = 5,
};

class npc_air_warden : public CreatureScript
{
    public:
        npc_air_warden() : CreatureScript("npc_air_warden") { }

        struct npc_air_wardenAI : public ScriptedAI
        {
            npc_air_wardenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
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
                   me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }
 
            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_WIND_SNEAR, urand(2000, 6000));
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->SetData(DATA_WARDENS, 1);
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
                        case EVENT_WIND_SNEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_WIND_SNEAR);
                            events.ScheduleEvent(EVENT_WIND_SNEAR, urand(7000, 10000));
                            break;                                                 
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
 
        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_air_wardenAI>(creature);
        }

};

class npc_flame_warden : public CreatureScript
{
    public:
        npc_flame_warden() : CreatureScript("npc_flame_warden") { }

        struct npc_flame_wardenAI : public ScriptedAI
        {
            npc_flame_wardenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
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
                   me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }
 
            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LAVA_ERUPTION, urand(4000, 7000));
                events.ScheduleEvent(EVENT_RAGING_INFERNO, urand(10000, 12000));
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->SetData(DATA_WARDENS, 1);
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
                        case EVENT_LAVA_ERUPTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_LAVA_ERUPTION);
                            events.ScheduleEvent(EVENT_LAVA_ERUPTION, urand(8000, 12000));
                            break;
                        case EVENT_RAGING_INFERNO:
                            DoCastAOE(SPELL_RAGING_INFERNO);
                            events.ScheduleEvent(EVENT_RAGING_INFERNO, urand(20000, 25000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };
 
        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_flame_wardenAI>(creature);
        }
};

class npc_water_warden : public CreatureScript
{
    public:
        npc_water_warden() : CreatureScript("npc_water_warden") { }

        struct npc_water_wardenAI : public ScriptedAI
        {
            npc_water_wardenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
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
                   me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }
 
            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BUBBLE_BOUND, urand(10000, 15000));
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->SetData(DATA_WARDENS, 1);
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
                        case EVENT_BUBBLE_BOUND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_BUBBLE_BOUND);
                            events.ScheduleEvent(EVENT_BUBBLE_BOUND, urand(10000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_water_wardenAI>(creature);
        }
};

class npc_earth_warden : public CreatureScript
{
    public:
        npc_earth_warden() : CreatureScript("npc_earth_warden") { }

        struct npc_earth_wardenAI : public ScriptedAI
        {
            npc_earth_wardenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
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
                   me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }
 
            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_IMPALE, urand(6000, 10000));
                events.ScheduleEvent(EVENT_ROCKWAVE, urand(12000, 15000));
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                instance->SetData(DATA_WARDENS, 1);
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
                        case EVENT_IMPALE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_IMPALE);
                            events.ScheduleEvent(EVENT_IMPALE, urand(10000, 15000));
                            break;
                        case EVENT_ROCKWAVE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_ROCKWAVE);
                            events.ScheduleEvent(EVENT_ROCKWAVE, urand(15000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_earth_wardenAI>(creature);
        }
};

class npc_water_warden_water_bubble : public CreatureScript
{
    public:
        npc_water_warden_water_bubble() : CreatureScript("npc_water_warden_water_bubble") { }

        struct npc_water_warden_water_bubbleAI : public ScriptedAI
        {
            npc_water_warden_water_bubbleAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                SetCombatMovement(false);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Unit* pOwner = me->GetOwner())
                    pOwner->RemoveAurasDueToSpell(SPELL_BUBBLE_BOUND);

                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!me->GetOwner())
                    me->DespawnOrUnsummon();

                if (me->GetOwner()->IsAlive())
                    me->DespawnOrUnsummon();

                if (!me->GetOwner()->HasAura(SPELL_BUBBLE_BOUND))
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_water_warden_water_bubbleAI>(creature);
        }
};

class npc_halls_of_origination_brann_bronzebeard : public CreatureScript
{
    public:
        npc_halls_of_origination_brann_bronzebeard() : CreatureScript("npc_halls_of_origination_brann_bronzebeard") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Вперед." : "Let's go.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == (GOSSIP_ACTION_INFO_DEF + 1))
            {
                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                creature->AI()->DoAction(ACTION_START_EVENT);
            }

            return true;
        }

        struct npc_halls_of_origination_brann_bronzebeardAI : public ScriptedAI
        {
            npc_halls_of_origination_brann_bronzebeardAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                creature->setActive(true);
            }
 
            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_TALK_1:
                        Talk(SAY_7);
                        break;
                    case ACTION_TALK_2:
                        Talk(SAY_8);
                        break;
                    case ACTION_TALK_3:
                        Talk(SAY_9);
                        break;
                    case ACTION_TALK_4:
                        Talk(SAY_10);
                        break;
                    case ACTION_START_EVENT:
                        events.ScheduleEvent(EVENT_TALK_0, 1000);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {        
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TALK_0:
                            Talk(SAY_0);
                            events.ScheduleEvent(EVENT_TALK_1, 8000);
                            break;
                        case EVENT_TALK_1:
                            instance->HandleGameObject(instance->GetGuidData(DATA_ANRAPHET_ENTRANCE_DOOR), true);
                            instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, EVENT_FASTER_THAN_LIGHT);
                            Talk(SAY_1);
                            events.ScheduleEvent(EVENT_TALK_2, 4000);
                            break;
                        case EVENT_TALK_2:
                            Talk(SAY_2);
                            events.ScheduleEvent(EVENT_TALK_3, 8000);
                            break;
                        case EVENT_TALK_3:
                            Talk(SAY_3);
                            events.ScheduleEvent(EVENT_TALK_4, 4000);
                            break;
                        case EVENT_TALK_4:
                            Talk(SAY_4);
                            events.ScheduleEvent(EVENT_TALK_5, 4000);
                            break;
                        case EVENT_TALK_5:
                            Talk(SAY_5);
                            events.ScheduleEvent(EVENT_TALK_6, 4000);
                            break;
                        case EVENT_TALK_6:
                            Talk(SAY_6);
                            break;
                    }
                }
            }
        };
 
        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_halls_of_origination_brann_bronzebeardAI>(creature);
        }
};

class go_halls_of_origination_transit_device : public GameObjectScript
{
    public:
        go_halls_of_origination_transit_device() : GameObjectScript("go_halls_of_origination_transit_device"){ }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_ANRAPHET) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться к Чертогу Огней." : "Teleport to The Vaults of Lights.", GOSSIP_SENDER_HALLS_OF_ORIGINATION, 2);
                if (instance->GetBossState(DATA_EARTHRAGER_PTAH) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться в гробницу Ярости Земли." : "Teleport to Tobm of the Earthrager.", GOSSIP_SENDER_HALLS_OF_ORIGINATION, 1);
                if (instance->GetBossState(DATA_TEMPLE_GUARDIAN_ANHUUR) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ru ? "Отправиться к платформе Творца." : "Teleport to Chamber of Prophecy.", GOSSIP_SENDER_HALLS_OF_ORIGINATION, 0);
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            if (action >= 4)
                return false;

            Position loc = halls_of_origination_locs[action];
            if (!player->IsInCombat())
                player->NearTeleportTo(loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ(), loc.GetOrientation(), false);
            return true;
        }
};

class go_halls_of_origination_transit_device_2 : public GameObjectScript
{
    public:
        go_halls_of_origination_transit_device_2() : GameObjectScript("go_halls_of_origination_transit_device_2"){ }

        bool OnGossipHello(Player* player, GameObject* /*go*/) override
         {
            if (player->IsInCombat())
                return true;

            return false;
        }
};

void AddSC_halls_of_origination()
{
    new npc_air_warden();
    new npc_flame_warden();
    new npc_water_warden();
    new npc_earth_warden();
    new npc_water_warden_water_bubble();
    new npc_halls_of_origination_brann_bronzebeard();
    new go_halls_of_origination_transit_device();
    new go_halls_of_origination_transit_device_2();
}
