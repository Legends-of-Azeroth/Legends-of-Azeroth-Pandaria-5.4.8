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

#include "brewmoon_festival.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

/* @TODO:
1. We need more fucking COSMETICS SCENES
2. make a talking npc
*/

enum Events
{
    EVENT_PATH           = 1,
    EVENT_PATH_END       = 2,
    EVENT_BLOAT_PATH     = 3,
    EVENT_BLOAT_PATH_END = 4,
};

struct brewmoon_yaungol_typeAI : public ScriptedAI
{
    brewmoon_yaungol_typeAI(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap nonCombatEvents;
    bool onBloat;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
        Reset();
        onBloat = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        nonCombatEvents.Reset();
        me->SetHomePosition(*me);
    }
};

// Den Mother Moof 63518
class npc_den_mother_moof : public CreatureScript
{
    public:
        npc_den_mother_moof() : CreatureScript("npc_den_mother_moof") { }

        enum iSpells
        {
            SPELL_WHIRLWIND = 124378,
            SPELL_BURROW    = 124359,
        };

        enum iEvents
        {
            EVENT_WHIRLWIND      = 1,
            EVENT_BURROW         = 2,
            EVENT_BURROW_VEHICLE = 3,
        };

        struct npc_den_mother_moofAI : public ScriptedAI
        {
            npc_den_mother_moofAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            InstanceScript* instance;
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
                if (Creature* kieu = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ASSISTANT_KIEU) : ObjectGuid::Empty))
                    kieu->AI()->Talk(TALK_SPECIAL_3);

                events.ScheduleEvent(EVENT_WHIRLWIND, 6 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BURROW, urand(10 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_BURROW_VEHICLE)
                    me->CastSpell(summon, VEHICLE_SPELL_RIDE_HARDCODED, false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_BREWMOON_FESTIVAL, me->GetEntry());
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
                        case EVENT_WHIRLWIND:
                            if (me->IsOnVehicle())
                            {
                                events.RescheduleEvent(EVENT_WHIRLWIND, urand(7 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                                break;
                            }
                            DoCast(me, SPELL_WHIRLWIND);
                            events.ScheduleEvent(EVENT_WHIRLWIND, urand(8 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                            break;
                        case EVENT_BURROW:
                            DoCast(me, SPELL_BURROW);
                            DoCast(me, SPELL_BURROW_VEHICLE, true);
                            events.ScheduleEvent(EVENT_BURROW, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_den_mother_moofAI(creature);
        }
};

// Li Te 63520
class npc_li_te : public CreatureScript
{
    public:
        npc_li_te() : CreatureScript("npc_li_te") { }

        enum iSpells
        {
            SPELL_WATER_BOLT        = 124654,
            SPELL_WATER_SHELL_APPLY = 124653,
            SPELL_WATERSPOUT        = 139159,
            SPELL_WATERSPOUT_SUMMON = 124648,
        };

        enum iEvents
        {
            EVENT_WATER_SHELL = 1,
            EVENT_WATER_BOLT  = 2,
            EVENT_WATERSPOUT  = 3
        };

        struct npc_li_teAI : public ScriptedAI
        {
            npc_li_teAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            InstanceScript* instance;
            SummonList summons;

            void InitializeAI() override
            {
                SetCombatMovement(false);
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WATER_SHELL, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WATER_BOLT, urand(3 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WATERSPOUT, 6 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_BREWMOON_FESTIVAL, me->GetEntry());
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    me->SetHomePosition(*me);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTRO)
                {
                    me->RemoveAurasDueToSpell(SPELL_WATER_SHELL);
                    me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 9.0f);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_WATERSPOUT)
                    summon->CastSpell(summon, SPELL_WATERSPOUT, false);
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
                        case EVENT_WATER_BOLT:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_WATER_BOLT, false);

                            events.ScheduleEvent(EVENT_WATER_BOLT, urand(3 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            break;
                        case EVENT_WATER_SHELL:
                            Talk(TALK_INTRO); // announce
                            DoCast(me, SPELL_WATER_SHELL_APPLY);
                            events.ScheduleEvent(EVENT_WATER_SHELL, urand(30 * IN_MILLISECONDS, 40 * IN_MILLISECONDS));
                            break;
                        case EVENT_WATERSPOUT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 100.0f, true))
                                me->CastSpell(target, SPELL_WATERSPOUT_SUMMON, false);

                            events.ScheduleEvent(EVENT_WATERSPOUT, 6 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_li_teAI(creature);
        }
};

// Karsar the Bloodletter 63529
class npc_karsar_bloodletter : public CreatureScript
{
    public:
        npc_karsar_bloodletter() : CreatureScript("npc_karsar_bloodletter") { }

        enum iSpells
        {
            SPELL_BLOODLETTING = 124341,
        };

        enum iEvents
        {
            EVENT_BLOODLETTING = 1,
        };

        struct npc_karsar_bloodletterAI : public ScriptedAI
        {
            npc_karsar_bloodletterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BLOODLETTING, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(DATA_BREWMOON_FESTIVAL, me->GetEntry());
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_BLOODLETTING)
                    {
                        if (Unit* vict = me->GetVictim())
                            me->CastSpell(vict, SPELL_BLOODLETTING, false);

                        events.ScheduleEvent(EVENT_BLOODLETTING, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_karsar_bloodletterAI(creature);
        }
};

// Disturbed Saurok 63521
class npc_disturbed_saurok : public CreatureScript
{
    public:
        npc_disturbed_saurok() : CreatureScript("npc_disturbed_saurok") { }

        enum iSpells
        {
            SPELL_GASH = 126815,
        };

        enum iEvents
        {
            EVENT_GASH = 1,
        };

        struct npc_disturbed_saurokAI : public ScriptedAI
        {
            npc_disturbed_saurokAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_GASH, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_GASH)
                    {
                        if (Unit* vict = me->GetVictim())
                            me->CastSpell(vict, SPELL_GASH, false);

                        events.ScheduleEvent(SPELL_GASH, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_disturbed_saurokAI(creature);
        }
};

// Water Spirit 63519
class npc_brewmoon_water_spirit : public CreatureScript
{
    public:
        npc_brewmoon_water_spirit() : CreatureScript("npc_brewmoon_water_spirit") { }

        enum iSpells
        {
            SPELL_WATER_BOLT = 124654,
        };

        enum iEvents
        {
            EVENT_WATER_BOLT = 1,
        };

        struct npc_brewmoon_water_spiritAI : public ScriptedAI
        {
            npc_brewmoon_water_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WATER_BOLT, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_WATER_BOLT)
                    {
                        if (Unit* vict = me->GetVictim())
                            me->CastSpell(vict, SPELL_WATER_BOLT, false);

                        events.ScheduleEvent(EVENT_WATER_BOLT, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmoon_water_spiritAI(creature);
        }
};

// Binan villager 62784
class npc_binan_villager : public CreatureScript
{
    public:
        npc_binan_villager() : CreatureScript("npc_binan_villager") { }

        struct npc_binan_villagerAI : public ScriptedAI
        {
            npc_binan_villagerAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            bool introDone;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                introDone = false;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!introDone && who && who->GetTypeId() == TYPEID_PLAYER && (me->GetDBTableGUIDLow() == 546618 || me->GetDBTableGUIDLow() == 546210))
                {
                    introDone = true;
                    Talk(TALK_INTRO);
                }

                ScriptedAI::MoveInLineOfSight(who);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_binan_villagerAI(creature);
        }
};

// Fireworks Barrel 63940
class npc_brewmoon_fireworks_barrel : public CreatureScript
{
    public:
        npc_brewmoon_fireworks_barrel() : CreatureScript("npc_brewmoon_fireworks_barrel") { }

        enum iEvents
        {
            EVENT_FIREWORK_LAUNCH = 1,
        };

        struct npc_brewmoon_fireworks_barrelAI : public ScriptedAI
        {
            npc_brewmoon_fireworks_barrelAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                events.ScheduleEvent(EVENT_FIREWORK_LAUNCH, urand(3 * IN_MILLISECONDS, 46 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_FIREWORK_LAUNCH)
                    {
                        if (uint32 m_fireworkId = Trinity::Containers::SelectRandomContainerElement(fireworks))
                            me->CastSpell(me, m_fireworkId, false);

                        events.ScheduleEvent(EVENT_FIREWORK_LAUNCH, urand(3 * IN_MILLISECONDS, 46 * IN_MILLISECONDS));
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmoon_fireworks_barrelAI(creature);
        }
};

// Assistant Tart 62793
class npc_assistant_tart : public CreatureScript
{
    public:
        npc_assistant_tart() : CreatureScript("npc_assistant_tart") { }

        enum iEvents
        {
            EVENT_WATER_SHELL = 1,
            EVENT_WATER_BOLT = 2,
            EVENT_WATERSPOUT = 3
        };

        struct npc_assistant_tartAI : public ScriptedAI
        {
            npc_assistant_tartAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;
            bool introDone;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                introDone = false;
                Reset();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        if (!introDone)
                        {
                            introDone = true;
                            Talk(TALK_INTRO);
                        }
                        break;
                    case ACTION_BREWMOON_FESTIVAL:
                        if (Creature* m_boat = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_TARTS_BOAT) : ObjectGuid::Empty))
                            m_boat->AI()->DoAction(ACTION_INTRO);

                        Talk(TALK_SPECIAL_1);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_assistant_tartAI(creature);
        }
};

// Tarts Boat 63826
class npc_tarts_boat : public CreatureScript
{
    public:
        npc_tarts_boat() : CreatureScript("npc_tarts_boat") { }

        enum iEvents
        {
            EVENT_WATER_GLOBULE = 1,
        };

        struct npc_tarts_boatAI : public ScriptedAI
        {
            npc_tarts_boatAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                    if (Creature* tart = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ASSISTANT_TART) : ObjectGuid::Empty))
                        tart->AI()->Talk(TALK_SPECIAL_2);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                    {
                        if (Creature* m_tart = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ASSISTANT_TART) : ObjectGuid::Empty))
                            m_tart->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, false);

                        if (Creature* m_globule = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_PURE_WATER_GLOBE) : ObjectGuid::Empty))
                        {
                            Movement::MoveSplineInit init(me);
                            init.MoveTo(m_globule->GetPositionX() - frand(5.0f, 6.0f), m_globule->GetPositionY(), 475.452f);
                            init.SetVelocity(5.5f);
                            init.Launch();
                            nonCombatEvents.ScheduleEvent(EVENT_WATER_GLOBULE, me->GetSplineDuration());
                        }

                        break;
                    }
                    case ACTION_BREWMOON_FESTIVAL:
                        if (Creature* m_boat = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_TARTS_BOAT) : ObjectGuid::Empty))
                            m_boat->AI()->DoAction(ACTION_INTRO);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_WATER_GLOBULE)
                    {
                        if (Creature* m_globule = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_PURE_WATER_GLOBE) : ObjectGuid::Empty))
                            m_globule->RemoveAurasDueToSpell(SPELL_SPAWN_WATER_GLOBE);

                        me->GetMotionMaster()->MovePoint(0, me->GetHomePosition());
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tarts_boatAI(creature);
        }
};

// Assistant Kieu 62787
class npc_assistant_kieu : public CreatureScript
{
    public:
        npc_assistant_kieu() : CreatureScript("npc_assistant_kieu") { }

        enum iEvents
        {
            EVENT_PASTINTRO        = 1,
            EVENT_BREWMOON_PREPARE = 2,
            EVENT_PATH_TALK        = 3,
        };

        struct npc_assistant_kieuAI : public ScriptedAI
        {
            npc_assistant_kieuAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;
            bool introDone;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                introDone = false;
                Reset();
                me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), true);
                nonCombatEvents.ScheduleEvent(EVENT_PATH_TALK, 4 * IN_MILLISECONDS);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    if (Creature* m_sack = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SACK_OF_GRAIN) : ObjectGuid::Empty))
                        m_sack->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, false);
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        if (!introDone)
                        {
                            introDone = true;
                            me->StopMoving();
                            Talk(TALK_SPECIAL_1);
                            nonCombatEvents.CancelEvent(EVENT_PATH_TALK);
                            nonCombatEvents.ScheduleEvent(EVENT_PASTINTRO, 2 * IN_MILLISECONDS);
                        }
                        break;
                    case ACTION_BREWMOON_FESTIVAL:
                        me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                        Talk(TALK_SPECIAL_4);

                        nonCombatEvents.ScheduleEvent(EVENT_BREWMOON_PREPARE, 4.5 * IN_MILLISECONDS);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PASTINTRO:
                            Talk(TALK_SPECIAL_2);
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                            break;
                        case EVENT_PATH_TALK:
                            Talk(TALK_INTRO);
                            nonCombatEvents.ScheduleEvent(EVENT_PATH_TALK, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                        case EVENT_BREWMOON_PREPARE:
                            if (Creature* m_sack = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SACK_OF_GRAIN) : ObjectGuid::Empty))
                                me->GetMotionMaster()->MovePoint(0, *m_sack);

                            Talk(TALK_SPECIAL_5);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_assistant_kieuAI(creature);
        }
};

// Brewmaster Boof 64901
class npc_brewmaster_boof : public CreatureScript
{
    public:
        npc_brewmaster_boof() : CreatureScript("npc_brewmaster_boof") { }

        enum iEvents
        {
            EVENT_INTRO            = 1,
            EVENT_BREWMOON_PREPARE = 2,
            EVENT_BREWMOON         = 3,
            EVENT_YAUNGOLS         = 4,
            EVENT_DEFENDERS_1      = 5,
            EVENT_DEFENDERS_2      = 6,
        };

        struct npc_brewmaster_boofAI : public ScriptedAI
        {
            npc_brewmaster_boofAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap nonCombatEvents;
            InstanceScript* instance;
            bool introDone;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                introDone = false;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!introDone && who && who->GetTypeId() == TYPEID_PLAYER)
                {
                    introDone = true;
                    Talk(TALK_INTRO);
                    nonCombatEvents.ScheduleEvent(EVENT_INTRO, 5 * IN_MILLISECONDS);
                }

                ScriptedAI::MoveInLineOfSight(who);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI));
                    Talk(TALK_SPECIAL_2);
                    nonCombatEvents.ScheduleEvent(EVENT_BREWMOON, 6.2 * IN_MILLISECONDS);
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        if (Player* triggering = me->FindNearestPlayer(80.0f))
                            Talk(TALK_SPECIAL_1, triggering);

                        nonCombatEvents.ScheduleEvent(EVENT_BREWMOON_PREPARE, 4 * IN_MILLISECONDS);
                        break;
                    case ACTION_BREWMOON_FESTIVAL:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO:
                        {
                            Movement::MoveSplineInit init(me);
                            for (auto itr : BrewmasterPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.Launch();
                            break;
                        }
                        case EVENT_BREWMOON_PREPARE:
                            me->GetMotionMaster()->MoveJump(BrewmasterBarrelPoint, 20.0f, 20.0f, EVENT_JUMP);
                            break;
                        case EVENT_BREWMOON:
                            Talk(TALK_SPECIAL_3);
                            nonCombatEvents.ScheduleEvent(EVENT_YAUNGOLS, 6 * IN_MILLISECONDS);
                            break;
                        case EVENT_YAUNGOLS:
                            Talk(TALK_SPECIAL_4);
                            nonCombatEvents.ScheduleEvent(EVENT_DEFENDERS_1, 5 * IN_MILLISECONDS);

                            if (Creature* Hsieh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COMMANDER_HSIEH) : ObjectGuid::Empty))
                                Hsieh->AI()->Talk(TALK_INTRO);
                            break;
                        case EVENT_DEFENDERS_1:
                            Talk(TALK_SPECIAL_5);
                            if (Creature* Nian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MISTWEAVER_NIAN) : ObjectGuid::Empty))
                                Nian->AI()->Talk(TALK_INTRO);

                            nonCombatEvents.ScheduleEvent(EVENT_DEFENDERS_2, 7.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_DEFENDERS_2:
                            Talk(TALK_SPECIAL_6);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmaster_boofAI(creature);
        }
};

// Bataari Yaungol 63527
class npc_bataari_yaungol : public CreatureScript
{
    public:
        npc_bataari_yaungol() : CreatureScript("npc_bataari_yaungol") { }

        enum iSpells
        {
            SPELL_CLEAVE           = 125299,
            SPELL_HEROIC_LEAP_JUMP = 125302,
        };

        enum iEvents
        {
            EVENT_CLEAVE      = 1,
            EVENT_HEROIC_LEAP = 2,
        };

        struct npc_bataari_yaungolAI : public brewmoon_yaungol_typeAI
        {
            npc_bataari_yaungolAI(Creature* creature) : brewmoon_yaungol_typeAI(creature) { }

            EventMap events;
            std::list<Position> m_path;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEROIC_LEAP, urand(8 * IN_MILLISECONDS, 14.5 * IN_MILLISECONDS));
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_PASSAGE:
                        for (auto itr : AssaultPath2)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BRIDGE:
                        for (auto itr : AssaultPath1)
                            m_path.push_back(itr);
                        break;
                    case TYPE_WEST:
                        for (auto itr : AssaultPath3)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BLOAT:
                        onBloat = true;
                        for (auto itr : AssaultPath5)
                            m_path.push_back(itr);
                        break;
                }
                nonCombatEvents.ScheduleEvent(onBloat ? EVENT_BLOAT_PATH : EVENT_PATH, urand(1 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    me->SetHomePosition(*me);
                    me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PATH:
                        {
                            Movement::MoveSplineInit init(me);
                            for (auto itr : m_path)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 3.0f), itr.GetPositionY() + frand(-2.0f, 1.5f), itr.GetPositionZ()));

                            init.SetVelocity(4.75f);
                            init.Launch();
                            nonCombatEvents.ScheduleEvent(EVENT_PATH_END, me->GetSplineDuration());
                            break;
                        }
                        case EVENT_PATH_END:
                            me->SetHomePosition(*me);
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                            break;
                        case EVENT_BLOAT_PATH:
                            if (TempSummon* m_yBloat = me->SummonCreature(NPC_YAUNGOL_BOAT, *me, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                me->CastSpell(m_yBloat, VEHICLE_SPELL_RIDE_HARDCODED, false);

                                Movement::MoveSplineInit init(m_yBloat);
                                for (auto itr : m_path)
                                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 3.0f), itr.GetPositionY() + frand(-2.0f, 1.5f), itr.GetPositionZ()));

                                init.SetVelocity(4.75f);
                                init.Launch();
                                nonCombatEvents.ScheduleEvent(EVENT_BLOAT_PATH_END, m_yBloat->GetSplineDuration());
                                m_yBloat->DespawnOrUnsummon(m_yBloat->GetSplineDuration() + 1 * IN_MILLISECONDS);
                            }
                            break;
                        case EVENT_BLOAT_PATH_END:
                            me->ExitVehicle();
                            me->GetMotionMaster()->MoveJump(BloatExitPoint.GetPositionX() + frand(-3.0f, 4.0f), BloatExitPoint.GetPositionY() + frand(-1.0f, 2.0f), BloatExitPoint.GetPositionZ(), 20.0f, 20.0f, EVENT_JUMP);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CLEAVE:
                            if (Unit* m_victim = me->GetVictim())
                                me->CastSpell(m_victim, SPELL_CLEAVE, false);

                            events.ScheduleEvent(EVENT_CLEAVE, urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            break;
                        case EVENT_HEROIC_LEAP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, HeroicLeapPredicate(me)))
                                me->CastSpell(target, SPELL_HEROIC_LEAP_JUMP, false);

                            events.ScheduleEvent(EVENT_HEROIC_LEAP, urand(13.5 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct HeroicLeapPredicate 
                {
                    public:
                        HeroicLeapPredicate(Creature const* me) : _me(me) { }

                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) > 8.0f;
                        }

                    private:
                        Creature const* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bataari_yaungolAI(creature);
        }
};

// Bataari Flamecaller 63526
class npc_bataari_flamecaller : public CreatureScript
{
    public:
        npc_bataari_flamecaller() : CreatureScript("npc_bataari_flamecaller") { }

        enum iSpells
        {
            SPELL_FLAME_SHELL = 125292,
            SPELL_FIREBALL    = 125289,
        };

        enum iEvents
        {
            EVENT_FLAME_SHELL = 1,
            EVENT_FIREBALL    = 2,
        };

        struct npc_bataari_flamecallerAI : public brewmoon_yaungol_typeAI
        {
            npc_bataari_flamecallerAI(Creature* creature) : brewmoon_yaungol_typeAI(creature) { }

            EventMap events;
            std::list<Position> m_path;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FIREBALL, urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FLAME_SHELL, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_PASSAGE:
                        for (auto&& itr : AssaultPath2)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BRIDGE:
                        for (auto&& itr : AssaultPath1)
                            m_path.push_back(itr);
                        break;
                    case TYPE_WEST:
                        for (auto&& itr : AssaultPath3)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BLOAT:
                        onBloat = true;
                        for (auto&& itr : AssaultPath5)
                            m_path.push_back(itr);
                        break;
                }
                nonCombatEvents.ScheduleEvent(EVENT_PATH, urand(1 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PATH:
                        {
                            Movement::MoveSplineInit init(me);
                            for (auto itr : m_path)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 3.0f), itr.GetPositionY() + frand(-2.0f, 1.5f), itr.GetPositionZ()));

                            init.SetVelocity(4.75f);
                            init.Launch();
                            nonCombatEvents.ScheduleEvent(EVENT_PATH_END, me->GetSplineDuration());
                            break;
                        }
                        case EVENT_PATH_END:
                            me->SetHomePosition(*me);
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIREBALL:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_FIREBALL, false);

                            events.ScheduleEvent(EVENT_FIREBALL, urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            break;
                        case EVENT_FLAME_SHELL:
                            DoCast(me, SPELL_FLAME_SHELL);
                            events.ScheduleEvent(EVENT_FLAME_SHELL, 20 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bataari_flamecallerAI(creature);
        }
};

// Bataari Outrunner 64165
class npc_bataari_outrunner : public CreatureScript
{
    public:
        npc_bataari_outrunner() : CreatureScript("npc_bataari_outrunner") { }

        enum iSpells
        {
            SPELL_QUICKEN = 125298,
        };

        enum iEvents
        {
            EVENT_QUICKEN = 1,
        };

        struct npc_bataari_outrunnerAI : public brewmoon_yaungol_typeAI
        {
            npc_bataari_outrunnerAI(Creature* creature) : brewmoon_yaungol_typeAI(creature) { }

            EventMap events;
            std::list<Position> m_path;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_QUICKEN, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_PASSAGE:
                        for (auto&& itr : AssaultPath2)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BRIDGE:
                        for (auto&& itr : AssaultPath1)
                            m_path.push_back(itr);
                        break;
                    case TYPE_WEST:
                        for (auto&& itr : AssaultPath3)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BLOAT:
                        onBloat = true;
                        for (auto&& itr : AssaultPath5)
                            m_path.push_back(itr);
                        break;
                }
                nonCombatEvents.ScheduleEvent(EVENT_PATH, urand(1 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PATH:
                        {
                            Movement::MoveSplineInit init(me);
                            for (auto itr : m_path)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 3.0f), itr.GetPositionY() + frand(-2.0f, 1.5f), itr.GetPositionZ()));

                            init.SetVelocity(4.75f);
                            init.Launch();
                            nonCombatEvents.ScheduleEvent(EVENT_PATH_END, me->GetSplineDuration());
                            break;
                        }
                        case EVENT_PATH_END:
                            me->SetHomePosition(*me);
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_QUICKEN)
                    {
                        DoCast(me, SPELL_QUICKEN);
                        events.ScheduleEvent(EVENT_QUICKEN, urand(2.5*IN_MILLISECONDS, 2.8*IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bataari_outrunnerAI(creature);
        }
};

// Bataari War Yeti 64185
class npc_bataari_war_yeti : public CreatureScript
{
    public:
        npc_bataari_war_yeti() : CreatureScript("npc_bataari_war_yeti") { }

        enum iSpells
        {
            SPELL_YETI_SMASH_JUMP = 125330,
            SPELL_GROUND_SLAM     = 125315,
        };

        enum iEvents
        {
            EVENT_YETI_SMASH  = 1,
            EVENT_GROUND_SLAM = 2,
        };

        struct npc_bataari_war_yetiAI : public brewmoon_yaungol_typeAI
        {
            npc_bataari_war_yetiAI(Creature* creature) : brewmoon_yaungol_typeAI(creature) { }

            EventMap events;
            std::list<Position> m_path;

            void Reset() override
            {
                events.Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_YETI_SMASH, urand(9 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_GROUND_SLAM, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_PASSAGE:
                        for (auto&& itr : AssaultPath2)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BRIDGE:
                        for (auto&& itr : AssaultPath1)
                            m_path.push_back(itr);
                        break;
                    case TYPE_WEST:
                        for (auto&& itr : AssaultPath3)
                            m_path.push_back(itr);
                        break;
                    case TYPE_BLOAT:
                        onBloat = true;
                        for (auto&& itr : AssaultPath5)
                            m_path.push_back(itr);
                        break;
                }
                nonCombatEvents.ScheduleEvent(EVENT_PATH, urand(1 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_YAUNGOL_ATTACK, IN_PROGRESS);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PATH:
                        {
                            Movement::MoveSplineInit init(me);
                            for (auto itr : m_path)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 3.0f), itr.GetPositionY() + frand(-2.0f, 1.5f), itr.GetPositionZ()));

                            init.SetVelocity(4.75f);
                            init.Launch();
                            nonCombatEvents.ScheduleEvent(EVENT_PATH_END, me->GetSplineDuration());
                            break;
                        }
                        case EVENT_PATH_END:
                            me->SetHomePosition(*me);
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_YETI_SMASH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 100.0f, true))
                                me->CastSpell(target, SPELL_YETI_SMASH_JUMP, false);

                            events.ScheduleEvent(EVENT_YETI_SMASH, urand(13 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        case EVENT_GROUND_SLAM:
                            DoCast(me, SPELL_GROUND_SLAM);
                            events.ScheduleEvent(EVENT_GROUND_SLAM, urand(19 * IN_MILLISECONDS, 29 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bataari_war_yetiAI(creature);
        }
};

// Warbringer Qobi 63528
class npc_warbringer_qobi : public CreatureScript
{
    public:
        npc_warbringer_qobi() : CreatureScript("npc_warbringer_qobi") { }

        enum iSpells
        {
            SPELL_THROW_OIL         = 125413,
            SPELL_FIRELINE_SUMM     = 125392,
            SPELL_OIL_SLICK         = 125418,
            SPELL_SCENARIO_COMPLETE = 127910,
        };

        enum iEvents
        {
            EVENT_THROW_OIL = 1,
            EVENT_FIRELINE  = 2,
            EVENT_PATH_END  = 3,
        };

        struct npc_warbringer_qobiAI : public ScriptedAI
        {
            npc_warbringer_qobiAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events, nonCombatEvents;
            InstanceScript* instance;
            SummonList summons;
            uint32 m_LineStep;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                Reset();
                summons.DespawnAll();

                Movement::MoveSplineInit init(me);
                for (auto itr : AssaultPath4)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 3.0f), itr.GetPositionY() + frand(-2.0f, 1.5f), itr.GetPositionZ()));

                init.SetVelocity(4.75f);
                init.Launch();
                nonCombatEvents.ScheduleEvent(EVENT_PATH_END, me->GetSplineDuration());

                if (Creature* Hsieh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_COMMANDER_HSIEH) : ObjectGuid::Empty))
                    Hsieh->AI()->Talk(TALK_SPECIAL_1);
            }

            void Reset() override
            {
                events.Reset();
                m_LineStep = 4;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_THROW_OIL, urand(10.5 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FIRELINE, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_FLAMELINE)
                    return m_LineStep;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_FLAMELINE)
                    m_LineStep = data;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_WARBRINGER_QOBI, DONE);

                    for (auto&& itr : instance->instance->GetPlayers())
                        if (Player* target = itr.GetSource())
                            target->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_SCENARIO_COMPLETE);
                }

                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_OIL_SLICK)
                    summon->CastSpell(summon, SPELL_OIL_SLICK, false);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_PATH_END)
                    {
                        me->SetHomePosition(*me);
                        me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                    }
                    break;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_THROW_OIL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                me->CastSpell(target, SPELL_THROW_OIL, false);

                            events.ScheduleEvent(EVENT_THROW_OIL, urand(10.5 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                        case EVENT_FIRELINE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 100.0f, true))
                                me->CastSpell(target, SPELL_FIRELINE_SUMM, false);

                            events.ScheduleEvent(EVENT_FIRELINE, urand(18 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_warbringer_qobiAI(creature);
        }
};

// Fireline 64265
class npc_brewmoon_fireline : public CreatureScript
{
    public:
        npc_brewmoon_fireline() : CreatureScript("npc_brewmoon_fireline") { }

        enum iSpells
        {
            SPELL_FIRELINE_SUMM = 125392,
        };

        enum iEvents
        {
            EVENT_FIRELINE = 1,
        };

        struct npc_brewmoon_firelineAI : public ScriptedAI
        {
            npc_brewmoon_firelineAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            uint32 step;
            float x, y;

            void Reset() override
            {
                x = 0.0f;
                y = 0.0f;
                events.Reset();
                instance = me->GetInstanceScript();
                DoCast(me, SPELL_FIRELINES);

                if (Creature* m_Qobi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WARBRINGER_QOBI) : ObjectGuid::Empty))
                    step = m_Qobi->AI()->GetData(TYPE_FLAMELINE);

                if (step < 33)
                    events.ScheduleEvent(EVENT_FIRELINE, 500);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_FIRELINE)
                    {
                        GetPositionWithDistInOrientation(me, (float)step, me->GetOrientation(), x, y);
                        me->CastSpell(x, y, me->GetPositionZ(), SPELL_FIRELINE_SUMM, true);
                        step += 4;

                        if (Creature* m_Qobi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WARBRINGER_QOBI) : ObjectGuid::Empty))
                            m_Qobi->ToCreature()->AI()->SetData(TYPE_FLAMELINE, step);
                    }
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmoon_firelineAI(creature);
        }
};

// Burrow Cosmetic 124752
class spell_burrow_cosmetic : public SpellScriptLoader
{
    public:
        spell_burrow_cosmetic() : SpellScriptLoader("spell_burrow_cosmetic") { }

        class spell_burrow_cosmetic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_burrow_cosmetic_AuraScript);

            void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* m_owner = GetOwner()->ToUnit())
                    m_owner->SummonCreature(NPC_DEN_MOTHER_MOOF, *m_owner, TEMPSUMMON_MANUAL_DESPAWN);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_burrow_cosmetic_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_burrow_cosmetic_AuraScript();
        }
};

// Heroic Leap 125302, Yeti Smash 125330
class spell_brewmoon_heroic_leap : public SpellScriptLoader
{
    public:
        spell_brewmoon_heroic_leap() : SpellScriptLoader("spell_brewmoon_heroic_leap") { }

        class spell_brewmoon_heroic_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brewmoon_heroic_leap_SpellScript);

            void HandlePreventEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* m_caster = GetCaster())
                    if (SpellInfo const* m_spellType = GetSpellInfo())
                        m_caster->CastSpell(m_caster, m_spellType->Id == 125330 ? SPELL_YETI_SMASH_EFF : SPELL_HEROIC_LEAP_EFF, false);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_brewmoon_heroic_leap_SpellScript::HandlePreventEffect, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_brewmoon_heroic_leap_SpellScript();
        }
};

// Whirlwind 124378
class spell_brewmoon_whirlwind : public SpellScriptLoader
{
    public:
        spell_brewmoon_whirlwind() : SpellScriptLoader("spell_brewmoon_whirlwind") { }

        class spell_brewmoon_whirlwind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brewmoon_whirlwind_AuraScript);

            void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                    owner->ClearUnitState(UNIT_STATE_CASTING);

            }

            void Register() override
            {
                OnEffectApply += AuraEffectRemoveFn(spell_brewmoon_whirlwind_AuraScript::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_brewmoon_whirlwind_AuraScript();
        }
};

// Areatriggers 8554
class AreaTrigger_at_brewmoon_festival : public AreaTriggerScript
{
    public:
        AreaTrigger_at_brewmoon_festival() : AreaTriggerScript("AreaTrigger_at_brewmoon_festival") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (Creature* m_aTart = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_ASSISTANT_TART)))
                    m_aTart->AI()->DoAction(ACTION_INTRO);

                instance->DoCastSpellOnPlayers(SPELL_WATER_WALKING);
            }

            return false;
        }
};

void AddSC_brewmoon_festival()
{
    new npc_den_mother_moof();
    new npc_li_te();
    new npc_karsar_bloodletter();
    new npc_disturbed_saurok();
    new npc_brewmoon_water_spirit();
    new npc_binan_villager();
    new npc_brewmoon_fireworks_barrel();
    new npc_assistant_tart();
    new npc_tarts_boat();
    new npc_assistant_kieu();
    new npc_brewmaster_boof();
    new npc_bataari_yaungol();
    new npc_bataari_flamecaller();
    new npc_bataari_outrunner();
    new npc_bataari_war_yeti();
    new npc_warbringer_qobi();
    new npc_brewmoon_fireline();
    new spell_burrow_cosmetic();
    new spell_brewmoon_heroic_leap();
    new spell_brewmoon_whirlwind();
    new AreaTrigger_at_brewmoon_festival();
}