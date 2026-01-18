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

#include "unga_ingoo.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_NET_TOS              = 122254,
    SPELL_THROW_SPEAR          = 115062,
    SPELL_CALLING_BIRDS        = 121911,
    SPELL_HOZEN_KNIFE_FIGHT    = 127510,
    SPELL_RUSTY_SHANK          = 127513,
    SPELL_FRENZIED_STRIKES     = 127561,
    SPELL_THROW_AXE            = 127553,
    SPELL_GONNA_OOK_YA         = 121865, // hook
    SPELL_GETTIN_SCURVY        = 121893,
    SPELL_GETTING_SMASHED      = 121883,
    SPELL_BANANA_SHIP          = 127523,
    SPELL_JUST_A_BANANA_TREE   = 131775,
    SPELL_BLADESTORM           = 78828,
    SPELL_STEALING_BREW        = 123137,
    SPELL_BANANA_TOSS          = 127412,
    SPELL_CLEAVE_TOSS          = 121871,
    SPELL_BO_REJUVINATING_MIST = 126667,
    SPELL_COLLECTED_BREW       = 130268, // emote attack off
    SPELL_RENEWING_MIST        = 126608,
    SPELL_ENCUMBERED           = 130636,
    SPELL_ENCUMBERED_EFF       = 130638,
};

enum Events
{
    EVENT_NET_TOS = 1,
    EVENT_THROW_SPEAR,
    EVENT_RUSTY_SHANK,
    EVENT_KNIFE_FIGHT,
    EVENT_COSMETIC,
    EVENT_FRENZIED_STRIKES,
    EVENT_THROW_AXE,
    EVENT_RIDE_LEAVE,
    EVENT_GONNA_OOK,
    EVENT_GETTIN_SCURVY,
    EVENT_GETTING_SMASHED,
    EVENT_BLADESTORM,
    EVENT_BANANA_TOSS,
    EVENT_TALK,
    EVENT_CLEAVE_TOSS,
    EVENT_INTRO,
    EVENT_ESCORT,
    EVENT_REJ_MIST,
    EVENT_SPECIAL,
};

// Make spawn any type of hozens on dest location with some variations
void DoSpawnHozensByType(Unit* me, ObjectGuid ownerGUID, float x, float y, float z, uint32 type, uint8 count)
{
    if (Unit* owner = ObjectAccessor::GetUnit(*me, ownerGUID))
    {
        if (count < 2)
            owner->SummonCreature(type, x, y, z, TEMPSUMMON_MANUAL_DESPAWN);
        else
        {
            for (uint8 i = 0; i < count; ++i)
                owner->SummonCreature(type, x + frand(-2.0f, 2.0f), y + frand(-3.0f, 3.0f), z, TEMPSUMMON_MANUAL_DESPAWN);
        }
    }
}

// Brewmaster Bo 62467
class npc_brewmaster_bo_escort : public CreatureScript
{
    public:
        npc_brewmaster_bo_escort() : CreatureScript("npc_brewmaster_bo_escort") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(ACTION_INTRO);

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_brewmaster_bo_escortAI : public ScriptedAI
        {
            npc_brewmaster_bo_escortAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            InstanceScript* instance;
            EventMap events, nonCombatEvents;
            bool intro, hIntro;
            uint32 wp, m_bGladiators;
            bool hasIntroLaunch;

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                intro          = false;
                hIntro         = false;
                hasIntroLaunch = false;
                wp            = 0;
                m_bGladiators = 0;
            }

            void Reset() override
            {
                events.Reset();
                nonCombatEvents.Reset();

                if (!hIntro)
                {
                    hIntro = true;
                    Talk(TALK_SPECIAL_3);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 8 * IN_MILLISECONDS);
                }
                else
                    nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 100);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!intro && who && who->GetTypeId() == TYPEID_PLAYER)
                {
                    intro = true;
                    Talk(TALK_INTRO);
                }

                ScriptedAI::MoveInLineOfSight(who);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_REJ_MIST, 5 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        if (hasIntroLaunch)
                            break;

                        hasIntroLaunch = true;
                        Talk(TALK_SPECIAL_1);
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        nonCombatEvents.ScheduleEvent(EVENT_INTRO, 1500);
                        break;
                    case ACTION_HAVER_DEATH:
                        Talk(TALK_SPECIAL_5);
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 100);
                        break;
                    case ACTION_GLADIATORS_DEATH:
                        m_bGladiators++;

                        if (m_bGladiators >= 2)
                            nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 100);
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                ScriptedAI::EnterEvadeMode();
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;
                me->SetHomePosition(*me);

                switch (pointId)
                {
                    case 0:
                        Talk(TALK_SPECIAL_2);
                        DoSpawnHozensByType(me, me->GetGUID(), HozenInitPoint.GetPositionX(), HozenInitPoint.GetPositionY(), HozenInitPoint.GetPositionZ(), NPC_UNGA_SPEARSCAMP, 3);
                        break;
                    case 4:
                        me->GetMotionMaster()->Clear();
                        Talk(TALK_SPECIAL_4);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                        break;
                    case 5:
                        DoCast(me, SPELL_COLLECTED_BREW);

                        if (Creature* ungaKeg = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_UNGA_BREW_KEG)))
                            ungaKeg->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

                        nonCombatEvents.ScheduleEvent(EVENT_SPECIAL, 6 * IN_MILLISECONDS);
                        break;
                    case 6:
                        // Move forward if players killed them before we reach this point
                        if (m_bGladiators >= 2)
                        {
                            nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 100);
                            break;
                        }

                        Talk(TALK_SPECIAL_7);

                        for (auto&& itr : gladiators)
                            if (Creature* m_glad = ObjectAccessor::GetCreature(*me, instance->GetGuidData(itr)))
                                m_glad->AI()->DoAction(ACTION_INTRO);
                        break;
                    case 12:
                        Talk(TALK_SPECIAL_8);
                        instance->SetData(DATA_ESCORT, DONE);
                        break;
                    default:
                        nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 100);
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
                            me->GetMotionMaster()->MovePoint(0, BoInitPoint);
                            break;
                        case EVENT_ESCORT:
                            if (me->IsInCombat())
                                break;

                            // Moving Without Walk...
                            me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->Clear();

                            if (me->GetUnitMovementFlags() == MOVEMENTFLAG_WALKING)
                                me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

                            if (wp > 0 && wp <= 12)
                                me->GetMotionMaster()->MovePoint(wp, BoWaypoints[wp - 1]);
                            break;
                        case EVENT_SPECIAL:
                            Talk(TALK_SPECIAL_6);
                            nonCombatEvents.ScheduleEvent(EVENT_ESCORT, 2 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (!UpdateVictim())
                {
                    if (me->IsInCombat())
                    {
                        // Handle Reset
                        me->CombatStop();
                        Reset();
                    }

                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_REJ_MIST)
                    {
                        if (Player* itr = me->FindNearestPlayer(35.0f))
                            DoCast(itr, SPELL_BO_REJUVINATING_MIST);

                        events.ScheduleEvent(EVENT_REJ_MIST, 15 * IN_MILLISECONDS);
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmaster_bo_escortAI(creature);
        }
};

// Brewmaster Bo 62491
class npc_brewmaster_bo_brew : public CreatureScript
{
    public:
        npc_brewmaster_bo_brew() : CreatureScript("npc_brewmaster_bo_brew") { }

        enum iEvents
        {
            EVENT_TALK_1 = 1,
            EVENT_TALK_2,
            EVENT_TALK_3,
            EVENT_ANN,
            EVENT_TALK_5,
            EVENT_TALK_6,
            EVENT_TALK_7,
        };

        struct npc_brewmaster_bo_brewAI : public ScriptedAI
        {
            npc_brewmaster_bo_brewAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            TaskScheduler scheduler;
            uint8 m_bcount;
            uint32 delay;
            uint32 kegRunner;
            uint32 kegStilled;

            void Reset() override
            {
                delay = 0;
                kegRunner = 1;
                kegStilled = 0;
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        Talk(TALK_INTRO);
                        me->SetVisible(true);
                        DoCast(me, SPELL_THROW_KEG);

                        delay = 8000;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_1);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_2);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 5500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_3);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 10000), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_4);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_5);
                        });
                        break;
                    case ACTION_CAPTAIN_ASSAULT:
                        Talk(TALK_SPECIAL_6);
                        break;
                    case ACTION_CAPTAIN_DEFEAT:
                        Talk(TALK_SPECIAL_7);
                        delay = 7500;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_8);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
                        {
                            me->GetMotionMaster()->MoveJump(BanquestPos, 15.0f, 20.0f, EVENT_JUMP);
                        });
                        break;
                }
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                    Talk(TALK_SPECIAL_9);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_KEG_RUNNED:
                        return kegRunner;
                    case TYPE_KEG_STILLED:
                        return kegStilled;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_KEG_RUNNED:
                        kegRunner = data;
                        break;
                    case TYPE_KEG_STILLED:
                        kegStilled = data;
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_brewmaster_bo_brewAI(creature);
        }
};

// Unga Spearscamp 62474
struct npc_unga_spearscamp : public ScriptedAI
{
    npc_unga_spearscamp(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    InstanceScript* instance;
    EventMap events;

    void IsSummonedBy(Unit* summoner) override
    {
        Movement::MoveSplineInit init(me);
        for (auto itr : SpearscampPath)
            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY() + frand(-1.5f, 1.5f), itr.GetPositionZ()));

        init.SetSmooth();
        init.SetUncompressed();
        init.Launch();
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_NET_TOS, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_THROW_SPEAR, urand(3 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
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
                case EVENT_NET_TOS:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true, -SPELL_NET_TOS))
                        me->CastSpell(target, SPELL_NET_TOS, false);

                    events.ScheduleEvent(EVENT_NET_TOS, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    break;
                case EVENT_THROW_SPEAR:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_THROW_SPEAR);

                    events.ScheduleEvent(EVENT_THROW_SPEAR, urand(6 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Unga Bird Haver 62475
struct npc_unga_bird_haver : public ScriptedAI
{
    npc_unga_bird_haver(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) 
    { 
        m_bcount = 0;
    }

    InstanceScript* instance;
    EventMap events;
    uint8 m_bcount;

    void Reset() override
    {
        events.Reset();
        DoCast(me, SPELL_CALLING_BIRDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INTRO && ++m_bcount >= 8)
        {
            me->RemoveAurasDueToSpell(SPELL_CALLING_BIRDS);
            events.ScheduleEvent(EVENT_RUSTY_SHANK, 3 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_KNIFE_FIGHT, 5.5 * IN_MILLISECONDS);
        }
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
                case EVENT_RUSTY_SHANK:
                    if (Unit* victim = me->GetVictim())
                        DoCast(victim, SPELL_RUSTY_SHANK);

                    events.ScheduleEvent(EVENT_RUSTY_SHANK, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
                case EVENT_KNIFE_FIGHT:
                    DoCast(me, SPELL_HOZEN_KNIFE_FIGHT);
                    events.ScheduleEvent(EVENT_RUSTY_SHANK, 13 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Unga Gladiators 62486, 62485
struct npc_unga_gladiators : public ScriptedAI
{
    npc_unga_gladiators(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    InstanceScript* instance;
    EventMap events, nonCombatEvents;

    void Reset() override
    {
        events.Reset();
        nonCombatEvents.Reset();

        nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, 1 * IN_MILLISECONDS);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        nonCombatEvents.Reset();
        me->RemoveAurasDueToSpell(SPELL_FRENZIED_STRIKES);

        if (me->GetEntry() == NPC_GAGOON)
            events.ScheduleEvent(EVENT_THROW_AXE, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
        else
            events.ScheduleEvent(EVENT_FRENZIED_STRIKES, urand(5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INTRO)
        {
            nonCombatEvents.Reset();
            me->GetMotionMaster()->MovePoint(0, GladInit);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_COSMETIC)
            {
                me->AddAura(SPELL_FRENZIED_STRIKES, me);
                nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
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
                case EVENT_THROW_AXE:
                    DoCastAOE(SPELL_THROW_AXE);
                    events.ScheduleEvent(EVENT_THROW_AXE, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                    break;
                case EVENT_FRENZIED_STRIKES:
                    DoCast(me, SPELL_FRENZIED_STRIKES);
                    events.ScheduleEvent(EVENT_FRENZIED_STRIKES, urand(8 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Unga ZipLine 62500
struct npc_unga_zipline_to_ship : public ScriptedAI
{
    npc_unga_zipline_to_ship(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

        Movement::MoveSplineInit init(me);
        for (auto itr : ZipLine)
            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

        init.SetSmooth();
        init.SetUncompressed();
        init.SetFly();
        init.Launch();

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            if (Vehicle* Passengers = me->GetVehicleKit())
                Passengers->RemoveAllPassengers();

            me->DespawnOrUnsummon(1000);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Captain Ook 62465
struct npc_unga_captain_ook : public ScriptedAI
{
    npc_unga_captain_ook(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    InstanceScript* instance;
    EventMap events, nonCombatEvents;

    void InitializeAI() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
        Reset();
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        nonCombatEvents.Reset();
        events.ScheduleEvent(EVENT_GONNA_OOK, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_GETTIN_SCURVY, 20 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_GETTING_SMASHED, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_INTRO:
            {
                float x, y;
                GetPositionWithDistInOrientation(me, 30.0f, me->GetOrientation(), x, y);
                me->SummonCreature(NPC_OOK_SHIP, x, y, 0.001f, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                Talk(TALK_INTRO);
                break;
            }
            case ACTION_CAPTAIN_ASSAULT:
            {
                me->ExitVehicle();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetHomePosition(*me);
                break;
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (instance)
            instance->SetData(DATA_CAPTAIN_OOK, DONE);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_COSMETIC)
                nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, urand(9 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
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
                case EVENT_GONNA_OOK:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 120.0f, true))
                        target->CastSpell(me, SPELL_GONNA_OOK_YA, true);

                    events.ScheduleEvent(EVENT_GONNA_OOK, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    break;
                case EVENT_GETTIN_SCURVY:
                    Talk(TALK_SPECIAL_1);
                    DoCast(me, SPELL_GETTIN_SCURVY);
                    events.ScheduleEvent(EVENT_GETTIN_SCURVY, 45 * IN_MILLISECONDS);
                    break;
                case EVENT_GETTING_SMASHED:
                    DoCast(me, SPELL_GETTING_SMASHED);
                    events.ScheduleEvent(EVENT_GETTING_SMASHED, urand(10 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Banana Ship 64367
struct npc_unga_banana_ship : public ScriptedAI
{
    npc_unga_banana_ship(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        DoCast(me, SPELL_BANANA_SHIP);
        DoCast(me, SPELL_PIRATE_SHIP_SAIL);
        summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

        Movement::MoveSplineInit init(me);
        init.MoveTo(BananaShipInit.GetPositionX(), BananaShipInit.GetPositionY(), BananaShipInit.GetPositionZ());
        init.SetSmooth();
        init.SetUncompressed();
        init.Launch();

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            if (Creature* Ook = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_CAPTAIN_OOK) : ObjectGuid::Empty))
                Ook->AI()->DoAction(ACTION_CAPTAIN_ASSAULT);

            me->DespawnOrUnsummon();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Unga Pirates 63256, 62508, 67027, 62590, 62506
struct npc_unga_pirate : public ScriptedAI
{
    npc_unga_pirate(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events, nonCombatEvents;

    void InitializeAI() override
    {
        if (!me->GetDBTableGUIDLow())
            DoAction(ACTION_INTRO);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        switch (me->GetEntry())
        {
            case NPC_UNGA_SCALLYWAG:
                events.ScheduleEvent(EVENT_BANANA_TOSS, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                break;
            case NPC_UNGA_HOZEKATEER:
                events.ScheduleEvent(EVENT_BLADESTORM, urand(4 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                break;
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INTRO)
        {
            switch (me->GetEntry())
            {
                case NPC_HOZEN_PIRATE_SHIP:
                    me->CastSpell(me, SPELL_PIRATE_SHIP_SAIL, false);
                    me->setRegeneratingHealth(false);
                    me->GetMotionMaster()->MovePoint(1, BananaShipInit);
                    break;
                case NPC_JUNGLE_BREWSTEALER:
                {
                    me->CastSpell(me, SPELL_JUST_A_BANANA_TREE, false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                    me->StopMoving();
                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 7; i < 12; ++i)
                        init.Path().push_back(G3D::Vector3(BoWaypoints[i].GetPositionX(), BoWaypoints[i].GetPositionY(), BoWaypoints[i].GetPositionZ()));
                
                    init.SetSmooth();
                    init.SetUncompressed();
                    init.Launch();
                    nonCombatEvents.ScheduleEvent(EVENT_COSMETIC, me->GetSplineDuration() + 1 * IN_MILLISECONDS);
                    break;
                }
                case NPC_UNGA_BREWSTEALER:
                case NPC_UNGA_SCALLYWAG:
                case NPC_UNGA_HOZEKATEER:
                {
                    float x, y;
                    GetPositionWithDistInOrientation(me, frand(50.0f, 52.0f), me->GetOrientation(), x, y);
                    me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
                    nonCombatEvents.ScheduleEvent(EVENT_TALK, urand(1 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    break;
            }
            }
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case 0:
                me->SetHomePosition(*me);
                break;
            case 1:
            {
                uint8 m_rnd = urand(3, 5);
                for (uint8 i = 0; i < m_rnd; ++i)
                    me->SummonCreature(NPC_UNGA_SCALLYWAG, me->GetPositionX() + frand(-6.0f, -2.0f), me->GetPositionY() + frand(-1.5f, 1.5f), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);

                me->DespawnOrUnsummon(1 * IN_MILLISECONDS);
                break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);
        scheduler.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_COSMETIC:
                {
                    if (Creature* brewMaster = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BO_BREW) : ObjectGuid::Empty))
                        brewMaster->AI()->Talk(TALK_SPECIAL_12);

                    me->StopMoving();
                    DoCast(me, SPELL_STEALING_BREW);

                    Movement::MoveSplineInit initx(me);
                    initx.MoveTo(G3D::Vector3(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ()));

                    initx.SetSmooth();
                    initx.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        // If reach home pos - remove 10 beer in progress
                        if (me->IsAlive() && me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_BARRELS_PROGRESS) > 9)
                        {
                            me->GetInstanceScript()->SetData(DATA_BARRELS_PROGRESS, me->GetInstanceScript()->GetData(DATA_BARRELS_PROGRESS) - 10);

                            if (Creature* boBrew = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BO_BREW) : ObjectGuid::Empty))
                                boBrew->AI()->SetData(TYPE_KEG_STILLED, 1);
                        }

                        me->DespawnOrUnsummon();

                    });
                    break;
                }
                case EVENT_TALK:
                    Talk(TALK_INTRO);
                    break;
            }
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
                case EVENT_BANANA_TOSS:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 120.0f, true))
                        me->CastSpell(target, SPELL_BANANA_TOSS, false);
                
                    events.ScheduleEvent(EVENT_BANANA_TOSS, urand(6 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                    break;
                case EVENT_BLADESTORM:
                    DoCast(me, SPELL_BLADESTORM);
                    events.ScheduleEvent(EVENT_BLADESTORM, urand(10 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Unstable Keg 62713
struct npc_unstable_keg : public ScriptedAI
{
    npc_unstable_keg(Creature* creature) : ScriptedAI(creature) { }

    bool hasTriggered;

    void Reset() override
    {
        hasTriggered = false;
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasTriggered)
            return;

        hasTriggered = true;
        DoCast(me, SPELL_POPPED_CORK);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }
};

// Ookie 62468, Chihozen Binono 62498
struct npc_unga_ookie : public ScriptedAI
{
    npc_unga_ookie(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
        events.ScheduleEvent(EVENT_CLEAVE_TOSS, 5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(TALK_SPECIAL_2);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_CLEAVE_TOSS)
            {
                Talk(TALK_SPECIAL_1);

                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                    DoCast(target, SPELL_CLEAVE_TOSS);

                events.ScheduleEvent(EVENT_CLEAVE_TOSS, urand(7 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Brew Containers 215165, 212273, 212248, 212252, 212253, 212256
class go_brew_container : public GameObjectScript
{
    public:
        go_brew_container() : GameObjectScript("go_brew_container") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            uint32 diff = 0;
            uint32 pBrew = 0;

            if (!player->GetAura(SPELL_UNGA_BREW_COLLECTED_AURA))
            {
                diff = 1;
                player->CastSpell(player, SPELL_UNGA_BREW_COLLECTED_AURA, false);
            }

            switch (go->GetEntry())
            {
                case GO_BIGGA_UNGA_KEG:
                case GO_BIGGA_UNGA_KEG_2:
                    pBrew = 10;
                    break;
                case GO_UNGA_KEG:
                case GO_UNGA_KEG_2:
                    pBrew = 5;
                    break;
                case GO_UNGA_MUG:
                case GO_UNGA_MUG_2:
                    pBrew = 2;
                    break;
            }

            if (Aura* m_AuraBrew = player->GetAura(SPELL_UNGA_BREW_COLLECTED_AURA))
            {
                if (pBrew)
                    m_AuraBrew->SetStackAmount(m_AuraBrew->GetStackAmount() + pBrew - diff);
            }

            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
            return true;
        }
};

// Calling Birds 121911
class spell_calling_birds : public AuraScript
{
    PrepareAuraScript(spell_calling_birds);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            DoSpawnHozensByType(owner, owner->GetGUID(), owner->GetPositionX(), owner->GetPositionY(), 22.136f, NPC_TRAINED_JUNGLE_LORY, 8);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_calling_birds::OnApply, EFFECT_0, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Throw Keg 130269
class spell_unga_throw_keg : public SpellScript
{
    PrepareSpellScript(spell_unga_throw_keg);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->HandleEmoteStateCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_unga_throw_keg::HandleAfterCast);
    }
};

// Mug Collected 121906
class spell_unga_mug_collected : public SpellScript
{
    PrepareSpellScript(spell_unga_mug_collected);

    void HandleDummyEffect(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            uint32 diff = 0;

            if (!caster->GetAura(SPELL_UNGA_BREW_COLLECTED_AURA))
            {
                diff = 1;
                caster->CastSpell(caster, SPELL_UNGA_BREW_COLLECTED_AURA, false);
            }

            if (Aura* brewAura = caster->GetAura(SPELL_UNGA_BREW_COLLECTED_AURA))
                brewAura->SetStackAmount(brewAura->GetStackAmount() + 3 - diff);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_unga_mug_collected::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Unga Jungle Brew Collected 121878
class spell_unga_jungle_brew_collected : public AuraScript
{
    PrepareAuraScript(spell_unga_jungle_brew_collected);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 25 && GetOwner() && GetOwner()->ToUnit())
            GetOwner()->ToUnit()->CastSpell(GetOwner()->ToUnit(), SPELL_ENCUMBERED, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_unga_jungle_brew_collected::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_SET_VEHICLE_ID, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Encumbered 130638
class spell_unga_encumbered : public AuraScript
{
    PrepareAuraScript(spell_unga_encumbered);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* brewMaster = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_BREWMASTER_BO_BREW) : ObjectGuid::Empty))
                brewMaster->AI()->Talk(TALK_SPECIAL_17, owner);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_unga_encumbered::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Popped Cork 122436
class spell_unga_popped_cork : public AuraScript
{
    PrepareAuraScript(spell_unga_popped_cork);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_unga_popped_cork::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// AreaTrigger 8146
class AreaTrigger_at_unga_ingoo_cauldron : public AreaTriggerScript
{
    public:
        AreaTrigger_at_unga_ingoo_cauldron() : AreaTriggerScript("AreaTrigger_at_unga_ingoo_cauldron") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (Aura* brew = player->GetAura(SPELL_UNGA_BREW_COLLECTED_AURA))
            {
                if (player->GetInstanceScript())
                    player->GetInstanceScript()->SetData(DATA_BARRELS_PROGRESS, player->GetInstanceScript()->GetData(DATA_BARRELS_PROGRESS) + brew->GetStackAmount());

                player->RemoveAurasDueToSpell(SPELL_UNGA_BREW_COLLECTED_AURA);
                player->RemoveAurasDueToSpell(SPELL_ENCUMBERED);
                player->RemoveAurasDueToSpell(SPELL_ENCUMBERED_EFF);
                player->CastSpell(CauldronPos.GetPositionX(), CauldronPos.GetPositionY(), CauldronPos.GetPositionZ(), SPELL_THROW_KEG_IN_CAULDRON, true);
            }

            return false;
        }
};

// 346. Summoned by 126667 - Bo Rejuvinating Mist
class sat_bo_rejuvinating_mist : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_RENEWING_MIST, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_RENEWING_MIST);
    }
};

void AddSC_unga_ingoo()
{
    new npc_brewmaster_bo_escort();
    new npc_brewmaster_bo_brew();
    new creature_script<npc_unga_spearscamp>("npc_unga_spearscamp");
    new creature_script<npc_unga_bird_haver>("npc_unga_bird_haver");
    new creature_script<npc_unga_gladiators>("npc_unga_gladiators");
    new creature_script<npc_unga_zipline_to_ship>("npc_unga_zipline_to_ship");
    new creature_script<npc_unga_captain_ook>("npc_unga_captain_ook");
    new creature_script<npc_unga_banana_ship>("npc_unga_banana_ship");
    new creature_script<npc_unga_pirate>("npc_unga_pirate");
    new creature_script<npc_unstable_keg>("npc_unstable_keg");
    new creature_script<npc_unga_ookie>("npc_unga_ookie");
    new go_brew_container();

    new aura_script<spell_calling_birds>("spell_calling_birds");
    new spell_script<spell_unga_throw_keg>("spell_unga_throw_keg");
    new spell_script<spell_unga_mug_collected>("spell_unga_mug_collected");
    new aura_script<spell_unga_jungle_brew_collected>("spell_unga_jungle_brew_collected");
    new aura_script<spell_unga_encumbered>("spell_unga_encumbered");
    new aura_script<spell_unga_popped_cork>("spell_unga_popped_cork");
    new AreaTrigger_at_unga_ingoo_cauldron();
    new atrigger_script<sat_bo_rejuvinating_mist>("sat_bo_rejuvinating_mist");
}