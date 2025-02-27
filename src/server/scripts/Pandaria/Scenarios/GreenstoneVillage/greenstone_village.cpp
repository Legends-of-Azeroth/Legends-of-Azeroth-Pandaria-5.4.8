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

#include "greenstone_village.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

std::vector<uint32> m_uiVillage
{
    NPC_SASSY_SEEDLING,
    NPC_CURSED_JADE,
    NPC_WILY_WOODLING,
    NPC_GREENSTONE_TERROR,
    NPC_DOOM_BLOOM,
    NPC_CURSED_BREW,
    NPC_BELLIGERENT_BLOSSOM,
};

enum Spells
{
    SPELL_COSMETIC_ATTACK      = 118179, // used for texture visual attack (wood trigger)
    SPELL_COSMETIC_WOOD_STRIKE = 130556,
    SPELL_KEG_DELIVERY_CREDIT  = 122935,
    SPELL_RIDE_ON_VEHICLE      = 43671,
    SPELL_GREENSTONE_KICKSTART = 123064,
    SPELL_GREENSTONE_SHARDS_D  = 119401,
    // destroyer
    SPELL_OVERPOWER            = 118865,
    SPELL_JADE_BLAZE_SUMM      = 122208,
    SPELL_JADE_BLAZE_AURA      = 122210,
    SPELL_JADE_BLAZE_PERIODIC  = 122212,
    // beast
    SPELL_JADE_POUNCE_JUMP     = 131209,
    SPELL_JADE_POUNCE_DAMAGE   = 119382,
    SPELL_JADE_STATUE          = 119364,
    // Vengeful Hui
    SPELL_GIANT_HUI            = 122213,
    SPELL_HUI_KNUCKLE          = 124303,
    SPELL_SUMM_SEEDLINGS       = 117664,
};

enum Events
{
    EVENT_COSMETIC = 1,
};

// Village owners 65271, 61343, 61342 ,61461, 61346, 61350, 61462, 62706
class npc_greenstone_village_owner : public CreatureScript
{
    public:
        npc_greenstone_village_owner() : CreatureScript("npc_greenstone_village_owner") { }

        enum iEvents
        {
            EVENT_INITIALIZE = 1,
            EVENT_DEFEAT     = 2,
            EVENT_SPECIAL_1  = 3,
            EVENT_SPECIAL_2  = 4,
        };                  

        struct npc_greenstone_village_ownerAI : public ScriptedAI
        {
            npc_greenstone_village_ownerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;
            std::list<Creature*> m_Pillagers;
            bool triggered, talked;
            float x, y;

            void InitializeAI() override
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
                triggered = false;
                talked = false;

                if (me->GetEntry() != NPC_COWARDLY_ZUE)
                {
                    nonCombatEvents.ScheduleEvent(EVENT_INITIALIZE, 1 * IN_MILLISECONDS);

                    if (me->GetEntry() != NPC_LA_THE_GENTLE)
                        me->HandleEmoteStateCommand(EMOTE_STATE_COWER);
                    else
                        me->HandleEmoteStateCommand(EMOTE_STATE_READY2H);
                }

                Reset();
            }


            void DoAction(int32 actionId) override
            {
                if (me->GetEntry() != NPC_LA_THE_GENTLE && actionId == ACTION_TALK && !talked)
                {
                    Talk(TALK_INTRO);
                    talked = true;
                }
            }

            bool HasPillagersDefeat()
            {
                for (auto&& itr : m_Pillagers)
                    if (itr->IsAlive())
                        return false;

                return true;
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    Talk(TALK_SPECIAL_2);
                    me->HandleEmoteStateCommand(EMOTE_STATE_SIT_CHAIR_LOW);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INITIALIZE:
                            for (auto&& itr : m_uiVillage)
                                GetCreatureListWithEntryInGrid(m_Pillagers, me, itr, 15.0f);

                            nonCombatEvents.ScheduleEvent(EVENT_DEFEAT, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_DEFEAT:
                            if (HasPillagersDefeat() && !triggered)
                            {
                                triggered = true;
                                Talk(TALK_PROTECTED);

                                if (instance && me->GetEntry() != NPC_LA_THE_GENTLE)
                                    instance->SendScenarioProgressUpdate(CriteriaProgressData(invVillagerCriteriaType.find(me->GetEntry())->second, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));
                                
                                switch (me->GetEntry())
                                {
                                    case NPC_GRACEFUL_SWAN:
                                        me->StopMoving();
                                        break;
                                    case NPC_MAYOR_LIN:
                                        nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_1, 3 * IN_MILLISECONDS);
                                        break;
                                }

                                me->HandleEmoteStateCommand(EMOTE_STATE_NONE);

                                if (instance)
                                    instance->SetData(DATA_RESCUE_VILLAGE, instance->GetData(DATA_RESCUE_VILLAGE) + 1);
                            }
                            else 
                                nonCombatEvents.ScheduleEvent(EVENT_DEFEAT, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_1:
                            Talk(TALK_SPECIAL_1);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL_2, 4 * IN_MILLISECONDS);
                            break;
                        case EVENT_SPECIAL_2:
                            GetPositionWithDistInOrientation(me, 4.0f, me->GetOrientation(), x, y);
                            me->GetMotionMaster()->MoveJump(x, y, 203.92f, 15.0f, 5.0f, EVENT_JUMP);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_ownerAI(creature);
        }
};

struct greenstone_village_pillager_typeAI : public ScriptedAI
{
    greenstone_village_pillager_typeAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap nonCombatEvents;

    void InitializeAI() override
    {
        me->SetReactState(REACT_AGGRESSIVE);
        me->setActive(true);

        Reset();

        // init cosmetic wood strike
        if (Creature* woodTarget = GetClosestCreatureWithEntry(me, NPC_MONSTER_TARGET_WOOD, 5.0f, true))
            if (me->GetEntry() != NPC_SASSY_SEEDLING)
                nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, urand(3 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        nonCombatEvents.Reset();
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_COSMETIC)
            {
                me->CastSpell(me, SPELL_COSMETIC_ATTACK, true);
                me->CastSpell(me, SPELL_COSMETIC_WOOD_STRIKE, true);
                nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, urand(3 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class npc_greenstone_pillager_base : public CreatureScript
{
    public:
        npc_greenstone_pillager_base() : CreatureScript("npc_greenstone_pillager_base") { }

        struct npc_greenstone_pillager_baseAI : public greenstone_village_pillager_typeAI
        {
            npc_greenstone_pillager_baseAI(Creature* creature) : greenstone_village_pillager_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_pillager_baseAI(creature);
        }
};

// Wily Woodling 61155
class npc_greenstone_wily_woodling : public CreatureScript
{
    public:
        npc_greenstone_wily_woodling() : CreatureScript("npc_greenstone_wily_woodling") { }

        enum iEvents
        {
            EVENT_WRATH = 1,
            EVENT_REJV  = 2,
        };

        enum iSpells
        {
            SPELL_WRATH        = 21807,
            SPELL_REJUVENATION = 142625,
        };

        struct npc_greenstone_wily_woodlingAI : public greenstone_village_pillager_typeAI
        {
            npc_greenstone_wily_woodlingAI(Creature* creature) : greenstone_village_pillager_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WRATH, 1500);
                events.ScheduleEvent(EVENT_REJV, 8 * IN_MILLISECONDS);
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
                        case EVENT_WRATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                                me->CastSpell(target, SPELL_WRATH, false);

                            events.ScheduleEvent(EVENT_WRATH, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                            break;
                        case EVENT_REJV:
                            if (!HealthBelowPct(70))
                            {
                                events.RescheduleEvent(EVENT_REJV, 5 * IN_MILLISECONDS);
                                break;
                            }

                            me->CastSpell(me, SPELL_REJUVENATION, false);
                            events.ScheduleEvent(EVENT_REJV, urand(8 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_wily_woodlingAI(creature);
        }
};

// Greenstone Terror 61159
class npc_greenstone_village_terror : public CreatureScript
{
    public:
        npc_greenstone_village_terror() : CreatureScript("npc_greenstone_village_terror") { }

        enum iEvents
        {
            EVENT_TERROR_SHARDS        = 1,
            EVENT_TERROR_SHARDS_REMOVE = 2,
        };

        enum iSpells
        {
            SPELL_TERROR_SHARDS = 119394,
        };

        struct npc_greenstone_village_terrorAI : public greenstone_village_pillager_typeAI
        {
            npc_greenstone_village_terrorAI(Creature* creature) : greenstone_village_pillager_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_TERROR_SHARDS, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_TERROR_SHARDS)
                    {
                        if (Unit* target = me->GetVictim())
                            me->CastSpell(target, SPELL_TERROR_SHARDS);

                        events.ScheduleEvent(EVENT_TERROR_SHARDS, urand(8 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_terrorAI(creature);
        }
};

// Belligerent Blossom 61183
class npc_greenstone_belligerent_blossom : public CreatureScript
{
    public:
        npc_greenstone_belligerent_blossom() : CreatureScript("npc_greenstone_terror") { }

        enum iEvents
        {
            EVENT_POLLEN_PUFF = 1,
        };

        enum iSpells
        {
            SPELL_POLLEN_PUFF = 118819,
        };

        struct npc_greenstone_belligerent_blossomAI : public greenstone_village_pillager_typeAI
        {
            npc_greenstone_belligerent_blossomAI(Creature* creature) : greenstone_village_pillager_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_POLLEN_PUFF, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
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
                    if (eventId == EVENT_POLLEN_PUFF)
                    {
                        DoCast(me, SPELL_POLLEN_PUFF);
                        events.ScheduleEvent(EVENT_POLLEN_PUFF, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_belligerent_blossomAI(creature);
        }
};

// Cursed Brew 62637
class npc_greenstone_village_cursed_brew : public CreatureScript
{
    public:
        npc_greenstone_village_cursed_brew() : CreatureScript("npc_greenstone_village_cursed_brew") { }

        enum iEvents
        {
            EVENT_BREW_BUBLE = 1,
            EVENT_KEG_SLAM   = 2,
        };

        enum iSpells
        {
            SPELL_KEG_SLAM    = 131124,
            SPELL_BREW_BUBBLE = 131143,
        };

        struct npc_greenstone_village_cursed_brewAI : public ScriptedAI
        {
            npc_greenstone_village_cursed_brewAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            InstanceScript* instance;
            EventMap events;
            SummonList summons;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_KEG_SLAM, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BREW_BUBLE, urand(9 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
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
                        case EVENT_BREW_BUBLE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                target->CastSpell(target, SPELL_BREW_BUBBLE, false);
                            else if (Unit* target = me->GetVictim())
                                target->CastSpell(target, SPELL_BREW_BUBBLE, false);

                            events.ScheduleEvent(EVENT_BREW_BUBLE, urand(9 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        case EVENT_KEG_SLAM:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_KEG_SLAM, false);

                            events.ScheduleEvent(EVENT_KEG_SLAM, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_cursed_brewAI(creature);
        }
};

// Cursed Jade 61174
class npc_greenstone_cursed_jade : public CreatureScript
{
    public:
        npc_greenstone_cursed_jade() : CreatureScript("npc_greenstone_cursed_brew") { }

        enum iSpells
        {
            SPELL_OVERPOWER = 118865,
        };

        struct npc_greenstone_cursed_jadeAI : public ScriptedAI
        {
            npc_greenstone_cursed_jadeAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            bool hasArrived;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                hasArrived = false;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!hasArrived && HealthBelowPct(60))
                {
                    hasArrived = true;
                    me->CastSpell(me, SPELL_OVERPOWER, true);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_cursed_jadeAI(creature);
        }
};

// Brewmaster Tzu 61203
class npc_greenstone_village_brewmaster_tzu : public CreatureScript
{
    public:
        npc_greenstone_village_brewmaster_tzu() : CreatureScript("npc_greenstone_village_brewmaster_tzu") { }

        enum iEvents
        {
            EVENT_SPECIAL = 1,
        };

        struct npc_greenstone_village_brewmaster_tzuAI : public ScriptedAI
        {
            npc_greenstone_village_brewmaster_tzuAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;
            std::list<Creature*> m_Pillagers;
            bool triggered, talked;

            void InitializeAI() override
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
                Reset();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                    {
                        me->SetVisible(true);
                        me->SetFaction(2110);

                        // Select any monstrosity
                        uint32 m_uiMonstrosity = urand(0, 1) ? NPC_BEAST_OF_JADE : NPC_JADE_DESTROYER;

                        if (Creature* monstrosity = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(m_uiMonstrosity) : ObjectGuid::Empty))
                        {
                            monstrosity->SetVisible(true);
                            monstrosity->SetFaction(16);
                        }
                        break;
                    }
                    case ACTION_SPECIAL_1:
                        Talk(TALK_SPECIAL_2);
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        Talk(TALK_INTRO);
                        nonCombatEvents.ScheduleEvent(EVENT_SPECIAL, 3500);
                        break;
                    case 1:
                        Talk(TALK_SPECIAL_1);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_SPECIAL)
                    {
                        Talk(TALK_PROTECTED);
                        me->GetMotionMaster()->MovePoint(1, TzuPath[1]);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_brewmaster_tzuAI(creature);
        }
};

// Greenstone Monstrosity 62619, 66772
class npc_greenstone_village_monstrosity : public CreatureScript
{
    public:
        npc_greenstone_village_monstrosity() : CreatureScript("npc_greenstone_village_monstrosity") { }

        enum iEvents
        {
            EVENT_OVERPOWER   = 1,
            EVENT_JADE_BLAZE  = 2,
            EVENT_JADE_POUNCE = 3,
            EVENT_JADE_STATUE = 4,
        };

        struct npc_greenstone_village_monstrosityAI : public ScriptedAI
        {
            npc_greenstone_village_monstrosityAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            InstanceScript* instance;
            EventMap events;
            SummonList summons;
            uint32 m_eFirst, m_eSecond;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_JADE_DESTROYER:
                        m_eFirst = EVENT_JADE_BLAZE;
                        m_eSecond = EVENT_OVERPOWER;
                        break;
                    case NPC_BEAST_OF_JADE:
                        m_eFirst = EVENT_JADE_POUNCE;
                        m_eSecond = EVENT_JADE_STATUE;
                        break;
                }

                events.ScheduleEvent(m_eFirst, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                events.ScheduleEvent(m_eSecond, urand(29 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->CastSpell(summon, SPELL_JADE_BLAZE_AURA, false);
                summon->CastSpell(summon, SPELL_JADE_BLAZE_PERIODIC, false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_RESCUE_DOJO, DONE);
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
                        case EVENT_JADE_BLAZE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_JADE_BLAZE_SUMM, false);
                            else if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_JADE_BLAZE_SUMM, false);

                            events.ScheduleEvent(EVENT_JADE_BLAZE, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        case EVENT_JADE_POUNCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                me->CastSpell(target, SPELL_JADE_POUNCE_JUMP, false);
                            else if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_JADE_POUNCE_DAMAGE, false);

                            events.ScheduleEvent(EVENT_JADE_POUNCE, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        case EVENT_JADE_STATUE:
                            me->CastSpell(me, SPELL_JADE_STATUE, false);
                            events.ScheduleEvent(EVENT_JADE_STATUE, urand(29 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                            break;
                        case EVENT_OVERPOWER:
                            me->CastSpell(me, SPELL_OVERPOWER, false);
                            events.ScheduleEvent(EVENT_OVERPOWER, urand(29 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_monstrosityAI(creature);
        }
};

// Burgled Barrel 62682
class npc_greenstone_village_burgled_barrel : public CreatureScript
{
    public:
        npc_greenstone_village_burgled_barrel() : CreatureScript("npc_greenstone_village_burgled_barrel") { }

        struct npc_greenstone_village_burgled_barrelAI : public ScriptedAI
        {
            npc_greenstone_village_burgled_barrelAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            bool hasAchievFail;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                hasAchievFail = false;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!hasAchievFail)
                {
                    hasAchievFail = true;
                    me->GetMap()->SetWorldState(WORLDSTATE_PERFECT_DELIVERY, 0);
                }
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                clicker->CastSpell(me, SPELL_RIDE_ON_VEHICLE, false);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_burgled_barrelAI(creature);
        }
};

// Greenstone Massons 62989, 62990, 62988
class npc_greenstone_village_masson : public CreatureScript
{
    public:
        npc_greenstone_village_masson() : CreatureScript("npc_greenstone_village_masson") { }

        struct npc_greenstone_village_massonAI : public ScriptedAI
        {
            npc_greenstone_village_massonAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap nonCombatEvents;
            bool hasRevive;

            void Reset() override
            {
                instance = me->GetInstanceScript();
                hasRevive = false;
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            }

            void DoAction(int32 actionId) override
            {
                if (!hasRevive && actionId == ACTION_INTRO)
                {
                    if (instance)
                        instance->SendScenarioProgressUpdate(CriteriaProgressData(invstonecutterCriteriaType.find(me->GetEntry())->second, 1, instance->GetScenarioGUID(), time(NULL), 0, 0));

                    hasRevive = true;
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, urand(1500, 2000));
                }
            }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                clicker->CastSpell(me, SPELL_GREENSTONE_KICKSTART, false);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_COSMETIC)
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_EAT);
                        Talk(TALK_INTRO);
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_village_massonAI(creature);
        }
};

// Vengeful Hui 61156
class npc_greenstone_vengeful_hui : public CreatureScript
{
    public:
        npc_greenstone_vengeful_hui() : CreatureScript("npc_greenstone_vengeful_hui") { }

        enum iEvents
        {
            EVENT_INTRO     = 1,
            EVENT_KNUCKLE   = 2,
            EVENT_SEEDLINGS = 3,
        };

        struct npc_greenstone_vengeful_huiAI : public ScriptedAI
        {
            npc_greenstone_vengeful_huiAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            InstanceScript* instance;
            EventMap events, nonCombatEvents;
            SummonList summons;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (me->HasAura(SPELL_GIANT_HUI))
                    me->CastSpell(me, SPELL_GIANT_HUI, false);

                nonCombatEvents.Reset();
                events.ScheduleEvent(EVENT_SEEDLINGS, urand(8 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_KNUCKLE, 6500);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTRO)
                {
                    me->SetVisible(true);
                    me->SetFaction(16);
                    Talk(TALK_INTRO);
                    Talk(TALK_PROTECTED); // ann

                    Movement::MoveSplineInit init(me);
                    for (auto itr : HuiPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.SetSmooth();
                    init.SetUncompressed();
                    init.Launch();
                    nonCombatEvents.ScheduleEvent(EVENT_INTRO, me->GetSplineDuration());
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                
                if (Unit* target = me->getAttackerForHelper())
                    summon->AI()->AttackStart(target);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_DEFEAT_VENGEFUL_HUI, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INTRO)
                    {
                        me->CastSpell(me, SPELL_GIANT_HUI, false);
                        me->SetHomePosition(*me);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_KNUCKLE:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_HUI_KNUCKLE, false);

                            events.ScheduleEvent(EVENT_KNUCKLE, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        case EVENT_SEEDLINGS:
                            DoCast(me, SPELL_SUMM_SEEDLINGS);
                            events.ScheduleEvent(EVENT_SEEDLINGS, urand(11 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_greenstone_vengeful_huiAI(creature);
        }
};

// Keg Delivery Creadit 122935
class spell_keg_delivery_credit : public SpellScriptLoader
{
    public:
        spell_keg_delivery_credit() : SpellScriptLoader("spell_keg_delivery_credit") { }

        class spell_keg_delivery_credit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_keg_delivery_credit_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit())
                {
                    if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                        instance->SetData(DATA_RECOVER_BURGLED_BARRELS, instance->GetData(DATA_RECOVER_BURGLED_BARRELS) + 1);

                    GetCaster()->Kill(GetHitUnit());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_keg_delivery_credit_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_keg_delivery_credit_SpellScript();
        }
};

// Greenstone Kickstart 123064
class spell_greenstone_kickstart : public SpellScriptLoader
{
    public:
        spell_greenstone_kickstart() : SpellScriptLoader("spell_greenstone_kickstart") { }

        class spell_greenstone_kickstart_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_greenstone_kickstart_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit() && GetHitUnit()->ToCreature())
                {
                    if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                        instance->SetData(DATA_GREENSTONE_MASONS, instance->GetData(DATA_GREENSTONE_MASONS) + 1);

                    GetHitUnit()->ToCreature()->AI()->DoAction(ACTION_INTRO);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_greenstone_kickstart_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_greenstone_kickstart_SpellScript();
        }
};

// Jade Pounce 131209
class spell_greenstone_jade_pounce : public SpellScriptLoader
{
    public:
        spell_greenstone_jade_pounce() : SpellScriptLoader("spell_greenstone_jade_pounce") { }

        class spell_greenstone_jade_pounce_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_greenstone_jade_pounce_SpellScript);

            void HandleJumpEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster() && GetHitUnit())
                    GetCaster()->CastSpell(GetHitUnit(), SPELL_JADE_POUNCE_DAMAGE, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_greenstone_jade_pounce_SpellScript::HandleJumpEffect, EFFECT_0, SPELL_EFFECT_JUMP);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_greenstone_jade_pounce_SpellScript();
        }
};

// Greenstone Shards 115662
class spell_terror_shards_triggered_effect : public SpellScriptLoader
{
    public:
        spell_terror_shards_triggered_effect() : SpellScriptLoader("spell_terror_shards_triggered_effect") { }

        class spell_terror_shards_triggered_effect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_terror_shards_triggered_effect_AuraScript);

            void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->AddAura(SPELL_GREENSTONE_SHARDS_D, GetCaster());
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_terror_shards_triggered_effect_AuraScript::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_terror_shards_triggered_effect_AuraScript();
        }
};

// Brew Bubble 131143
class spell_greenstone_brew_bubble : public SpellScriptLoader
{
    public:
        spell_greenstone_brew_bubble() : SpellScriptLoader("spell_greenstone_brew_bubble") { }

        class spell_greenstone_brew_bubble_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_greenstone_brew_bubble_AuraScript);

            void OnAuraEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    if (Creature* bubble = GetClosestCreatureWithEntry(GetCaster(), NPC_BREW_BUBLE, 10.0f, true))
                        bubble->DespawnOrUnsummon();
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_greenstone_brew_bubble_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_greenstone_brew_bubble_AuraScript();
        }
};

// Volatile Greenstone Brew 119090
class spell_greenstone_volatile_greenstone_brew : public SpellScript
{
    PrepareSpellScript(spell_greenstone_volatile_greenstone_brew);

    void HandleOnCast()
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            instance->instance->SetWorldState(WORLDSTATE_SAVE_IT_FOR_LATER, 0);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_greenstone_volatile_greenstone_brew::HandleOnCast);
    }
};

// AreaTriggers in greenstone village 8083,8469,7869,8472,7868,7873,7872
class AreaTrigger_at_greenstone_village : public AreaTriggerScript
{
    public:
        AreaTrigger_at_greenstone_village() : AreaTriggerScript("AreaTrigger_at_greenstone_village") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                Creature* m_owner = NULL;

                switch (trigger->ID)
                {
                    case AT_INTRO:
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_COWARDLY_ZUE));
                        break;
                    case AT_BEFORE_SHUNG:
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_PORTLY_SHUNG));
                        break;
                    case AT_BEFORE_LIUPO:
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_WOODCARVER_LIUPO));
                        break;
                    case AT_BEFORE_LIN:
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_MAYOR_LIN));
                        break;
                    case AT_BEFORE_MEILA:
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_MEILA));
                        break;
                    case AT_BEFORE_SWAN:
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_GRACEFUL_SWAN));
                        break;
                    case AT_BEFORE_RINJI: 
                        m_owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_SCRIBE_RINJI));
                        break;
                }

                if (m_owner && m_owner->AI())
                    m_owner->AI()->DoAction(ACTION_TALK);          
            }

            return false;
    }
};

// AreaTriggers behind tzu 8102
class AreaTrigger_at_behind_tzu : public AreaTriggerScript
{
    public:
        AreaTrigger_at_behind_tzu() : AreaTriggerScript("AreaTrigger_at_behind_tzu") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (player && player->IsOnVehicle() && player->GetVehicleBase() && player->GetVehicleBase()->ToCreature())
                {
                    player->CastSpell(player->GetVehicleBase(), SPELL_KEG_DELIVERY_CREDIT, false);

                    Creature* owner = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_BREWMASTER_TZU));

                    if (owner && owner->AI())
                        owner->AI()->DoAction(ACTION_SPECIAL_1);
                }
            }

            return false;
        }
};

void AddSC_greenstone_village()
{
    new npc_greenstone_village_owner();
    new npc_greenstone_pillager_base();
    new npc_greenstone_wily_woodling();
    new npc_greenstone_village_terror();
    new npc_greenstone_belligerent_blossom();
    new npc_greenstone_village_cursed_brew();
    new npc_greenstone_cursed_jade();
    new npc_greenstone_village_brewmaster_tzu();
    new npc_greenstone_village_monstrosity();
    new npc_greenstone_village_burgled_barrel();
    new npc_greenstone_village_masson();
    new npc_greenstone_vengeful_hui();
    new spell_keg_delivery_credit();
    new spell_greenstone_kickstart();
    new spell_greenstone_jade_pounce();
    new spell_terror_shards_triggered_effect();
    new spell_greenstone_brew_bubble();
    new spell_script<spell_greenstone_volatile_greenstone_brew>("spell_greenstone_volatile_greenstone_brew");
    new AreaTrigger_at_greenstone_village();
    new AreaTrigger_at_behind_tzu();
}