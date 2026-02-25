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

#include "dagger_in_the_dark.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "ScenarioMgr.h"

enum Spells
{
    SPELL_LIGHTNING_BOLT       = 133542,
    SPELL_LIGHTNING_BOLT_2     = 132752,
    SPELL_MOGU_RUNE_POWERUP    = 133471,
    SPELL_MOGU_POWER           = 133475,
    SPELL_CHAIN_HEAL           = 132652,
    SPELL_LIGHTNING_SHIELD     = 135999,
    SPELL_SHADOWSTEP           = 132649,
    SPELL_SHADOWSTEP_EFF       = 132647,
    SPELL_STEALTH              = 132653,
    SPELL_CRIPPLING_POISON     = 132748,
    SPELL_CRIPPLING_POISON_EFF = 132747,
    SPELL_FIXATE               = 132760,
    SPELL_WHIRLWIND            = 132757,
    SPELL_WHIRLWIND_EFF        = 132758,
    SPELL_SWING                = 133754, // for broodmaster
    SPELL_HATCH_SELECTOR       = 133752,
    SPELL_SWING_EFF            = 133753,
    SPELL_HATCH                = 133702,
    SPELL_HATCH_EFF            = 134336,
    SPELL_DEATH_NOVA           = 133804,
    SPELL_BIG_BAD_VOODOO_AT    = 133892,
    SPELL_BIG_BAD_VOODOO_VIS   = 133898, // channeled by Voljin
    SPELL_BIG_BAD_VOODOO_EFF   = 133896,
    SPELL_STALACTITE_LAUNCH    = 133360,
    SPELL_STALACTITE_MISSLE    = 133638,
    SPELL_SUMMON_STALACTITE    = 133572,
    SPELL_STRIKE               = 133707,
    SPELL_WRATH                = 132646,
    SPELL_ENTANGLING_ROOTS     = 132743,
    SPELL_THROW_SPEAR          = 132644,
    SPELL_LORD_CHARGE          = 97164,
    SPELL_WATER_JETS           = 133121,
    SPELL_STASIS               = 133548,
    SPELL_INVESTIGATION        = 133749,
    SPELL_INVESTIGATION_2      = 133750,
    SPELL_RIDE_VEHICLE         = 43671,
    SPELL_GAS_BOMB_EFF         = 133001,
    SPELL_GAS_BOMB             = 132992,
    SPELL_CHEAP_SHOT           = 133002,
    SPELL_SUMMON_HEALING_WARD  = 133006,
    SPELL_HEALING_WARD_AT      = 133005,
    SPELL_HEALING_WARD_EFF     = 133004,
};

enum Events
{
    EVENT_LIGHTNING_BOLT = 1,
    EVENT_CHAIN_HEAL,
    EVENT_LIGHTNING_SHIELD,
    EVENT_SHADOWSTEP,
    EVENT_FIXATE,
    EVENT_WHIRLWIND,
    EVENT_STRIKE,
    EVENT_WRATH,
    EVENT_ENTANGLING_ROOTS,
    EVENT_THROW_SPEAR,
    EVENT_WATER_JETS,
    EVENT_CALL_HELP,
    EVENT_STALACTITE,
    EVENT_DEATH_NOVA,
    EVENT_GAS_BOMB,
    EVENT_CHEAP_SHOT,
    EVENT_HEALING_WARD,
};

// Voljin 67414
class npc_dagger_in_dark_voljin : public CreatureScript
{
    public:
        npc_dagger_in_dark_voljin() : CreatureScript("npc_dagger_in_dark_voljin") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(ACTION_START_INTRO);

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        struct npc_dagger_in_dark_voljinAI : public ScriptedAI
        {
            npc_dagger_in_dark_voljinAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                hasStarted = false;
                reachCave  = false;
            }

            TaskScheduler scheduler;
            SummonList summons;
            EventMap events;
            bool hasStarted;
            bool reachCave;
            uint32 delay;

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
                delay = 0;

                if (reachCave && me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_THE_BROODMASTER) != DONE) // continue path after combat leave
                {
                    scheduler.CancelAll();
                    me->StopMoving();
                    Movement::MoveSplineInit init(me);
                    for (auto&& itr : voljinCavePath)
                    {
                        if (itr.GetPositionZ() < me->GetPositionZ())
                            continue;

                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));
                    }

                    init.SetUncompressed();
                    init.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        if (me->GetInstanceScript())
                            me->GetInstanceScript()->SetData(DATA_THE_SOURCE, DONE);
                    });
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEALING_WARD, 10 * IN_MILLISECONDS);

                if (reachCave)
                {
                    me->SetHomePosition(*me);
                    scheduler.CancelAll();
                    me->StopMoving();
                }
            }

            void AttackStart(Unit* target) override
            {
                if (target && (target->GetEntry() == NPC_CORRUPTED_DARKHATCHED_EGG || target->GetEntry() == NPC_MATURE_SAUROK_EGG))
                    return;

                if (target && me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_HEALING_WARD)
                    summon->CastSpell(summon, SPELL_HEALING_WARD_AT, true);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if (hasStarted)
                            break;

                        hasStarted = true;
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Talk(TALK_INTRO);

                        if (me->GetInstanceScript())
                            me->GetInstanceScript()->SetData(DATA_THE_MISSION, DONE);

                        delay = 3800;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            if (Creature* rakgor = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR) : ObjectGuid::Empty))
                                rakgor->AI()->Talk(TALK_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3400), [this](TaskContext context)
                        {
                            if (Creature* rakgor = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR) : ObjectGuid::Empty))
                                rakgor->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 17719);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                        {
                            // Fly out of there
                            if (Creature* rakgor = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR) : ObjectGuid::Empty))
                            {
                                Movement::MoveSplineInit init(rakgor);
                                for (auto&& itr : korkronLeavePath)
                                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                                init.SetFly();
                                init.SetVelocity(8.0f);
                                init.Launch();

                                rakgor->DespawnOrUnsummon(rakgor->GetSplineDuration());
                            }
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
                        {
                            // Fly out of there
                            if (Creature* bodyguard = ObjectAccessor::GetCreature(*me, getAnyKorkronBodyguardGUID()))
                            {
                                bodyguard->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 17719);

                                Movement::MoveSplineInit init(bodyguard);
                                for (auto&& itr : korkronLeavePath)
                                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                                init.SetFly();
                                init.SetVelocity(8.0f);
                                init.Launch();

                                bodyguard->DespawnOrUnsummon(bodyguard->GetSplineDuration());
                            }
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_1);

                            // Fly out of there
                            if (Creature* bodyguard = ObjectAccessor::GetCreature(*me, getAnyKorkronBodyguardGUID()))
                            {
                                bodyguard->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 17719);

                                Movement::MoveSplineInit init(bodyguard);
                                for (auto&& itr : korkronLeavePath)
                                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                                init.SetFly();
                                init.SetVelocity(8.0f);
                                init.Launch();

                                bodyguard->DespawnOrUnsummon(bodyguard->GetSplineDuration());
                            }
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 4800), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_2);

                            Movement::MoveSplineInit init(me);
                            for (auto&& itr : voljinFirstPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetWalk(true);
                            init.Launch();

                            delay = me->GetSplineDuration();
                            scheduler
                                .Schedule(Milliseconds(delay += 1000), [this](TaskContext context)
                            {
                                Talk(TALK_SPECIAL_3);
                                me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 29261);
                                me->SetAnimTier(AnimTier::Ground);
                            });

                            scheduler
                                .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
                            {
                                me->StopMoving();

                                Movement::MoveSplineInit init(me);
                                for (auto&& itr : voljinSecondPath)
                                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                                init.SetVelocity(9.6f);
                                init.Launch();

                                // Grizzle too wanna go with us!
                                if (Creature* gearslip = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIZZLE_GEARSLIP) : ObjectGuid::Empty))
                                    gearslip->AI()->DoAction(ACTION_START_INTRO);

                                scheduler
                                    .Schedule(Milliseconds(17300), [this](TaskContext context)
                                {
                                    Talk(TALK_SPECIAL_4);
                                });

                                delay = me->GetSplineDuration();
                                scheduler
                                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                                {
                                    Talk(TALK_SPECIAL_5);
                                });

                                scheduler
                                    .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
                                {
                                    me->SetHomePosition(*me);
                                    me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);

                                    if (me->GetInstanceScript())
                                        me->GetInstanceScript()->SetData(DATA_WE_RIDE, DONE);
                                });
                            });
                        });
                        break;
                    case ACTION_MEET_LIZARD_LORD:
                        scheduler
                            .Schedule(Milliseconds(1000), [this](TaskContext context)
                        {
                            me->SetHomePosition(voljinLizzardPoint);
                            me->GetMotionMaster()->MovePoint(0, voljinLizzardPoint);
                            Talk(TALK_SPECIAL_8);
                        });
                        break;
                    case ACTION_LIZARD_DIED:
                        delay = 1500;

                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_10);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_11);

                            // Move to boat
                            me->GetMotionMaster()->MovePoint(0, boatPath[0]);

                            if (Creature* grizzle = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIZZLE_GEARSLIP) : ObjectGuid::Empty))
                                grizzle->AI()->DoAction(ACTION_MOVE_TO_BOAT);
                        });
                        break;
                    case ACTION_VOLJIN_LEAVE_BOAT:
                        me->ExitVehicle();
                        me->GetMotionMaster()->MoveJump(voljinLeaveBoatPoint, 20.0f, 20.0f);
                        me->SetHomePosition(voljinLeaveBoatPoint);
                        delay = 4000;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_14);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 2600), [this](TaskContext context)
                        {
                            if (Creature* grizzle = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIZZLE_GEARSLIP) : ObjectGuid::Empty))
                                grizzle->AI()->Talk(TALK_SPECIAL_4);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
                        {
                            if (Creature* grizzle = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIZZLE_GEARSLIP) : ObjectGuid::Empty))
                                grizzle->AI()->Talk(TALK_SPECIAL_5);

                            // Move to lair
                            reachCave = true;

                            Movement::MoveSplineInit init(me);
                            for (auto&& itr : voljinCavePath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetUncompressed();
                            init.Launch();

                            scheduler
                                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                            {
                                if (me->GetInstanceScript())
                                    me->GetInstanceScript()->SetData(DATA_THE_SOURCE, DONE);
                            });
                        });
                        break;
                    case ACTION_REACH_BROODMASTER:
                        me->SetHomePosition(*me);
                        Talk(TALK_SPECIAL_15);

                        scheduler
                            .Schedule(Milliseconds(4000), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_16);
                        });
                        break;
                    case ACTION_DEFEAT_BROODMASTER:
                        scheduler
                            .Schedule(Milliseconds(4650), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_19);
                        });
                        break;
                    case ACTION_INVESTIGATE_DONE:
                        Talk(TALK_SPECIAL_22);

                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        delay = 6500;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_23);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 8600), [this](TaskContext context)
                        {
                            if (Creature* bloodrazor = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR + 1) : ObjectGuid::Empty))
                                me->SetFacingToObject(bloodrazor);

                            Talk(TALK_SPECIAL_24);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 7000), [this](TaskContext context)
                        {
                            if (Creature* bloodrazor = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR + 1) : ObjectGuid::Empty))
                                bloodrazor->AI()->Talk(TALK_SPECIAL_4);
                        });


                        scheduler
                            .Schedule(Milliseconds(delay += 12600), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_25);

                            if (Creature* bloodrazor = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR + 1) : ObjectGuid::Empty))
                                bloodrazor->AI()->DoAction(ACTION_GONNA_KILL_VOLJIN);
                        });
                        break;
                    case ACTION_BLOODRAZOR_SLAIN:
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        delay = 3500;

                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_27);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 4800), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_28);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_29);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3600), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_30);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 9700), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_31);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_32);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6200), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_33);
                        });
                        break;
                    case ACTION_BIG_BAD_VOOODOO:
                        Talk(TALK_SPECIAL_18);
                        me->GetMotionMaster()->MovePoint(0, bigBadVooodooPos);

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                        {
                            me->SetHomePosition(*me);
                            DoCast(me, SPELL_BIG_BAD_VOODOO_AT);
                            DoCast(me, SPELL_BIG_BAD_VOODOO_VIS);
                        });
                        break;
                }
            }

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
                    switch (eventId)
                    {
                        case EVENT_LIGHTNING_BOLT:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_LIGHTNING_BOLT);

                            events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 2.1 * IN_MILLISECONDS);
                            break;
                        case EVENT_HEALING_WARD:
                        {
                            Position pos = me->GetNearPosition(8.0f, frand(0.0f, 2 * M_PI));
                            DoCast(me, SPELL_SUMMON_HEALING_WARD);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                ObjectGuid getAnyKorkronBodyguardGUID()
                {
                    std::list<Creature*> bodyguards;
                    GetCreatureListWithEntryInGrid(bodyguards, me, NPC_KORKRON_BODYGUARD, 30.0f);

                    bodyguards.remove_if([=](Creature* target) { return target && target->GetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID) == 17719; });

                    if (bodyguards.empty())
                        return ObjectGuid::Empty;

                    return Trinity::Containers::SelectRandomContainerElement(bodyguards)->GetGUID();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_dagger_in_dark_voljinAI(creature);
        }
};

// Grizzle Gearslip 67782
struct npc_dagger_in_dark_grizzle_gearslip : public ScriptedAI
{
    npc_dagger_in_dark_grizzle_gearslip(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override { }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                Talk(TALK_INTRO);
                me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 25871);

                scheduler
                    .Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    Movement::MoveSplineInit init(me);
                    for (auto&& itr : voljinSecondPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Path().push_back(G3D::Vector3(additionalGrizzlePoint.GetPositionX(), additionalGrizzlePoint.GetPositionY(), additionalGrizzlePoint.GetPositionZ()));

                    init.SetVelocity(10.0f);
                    init.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        me->SetUInt32Value(UNIT_FIELD_MOUNT_DISPLAY_ID, 0);
                    });
                });
                break;
            case ACTION_MOVE_TO_BOAT:
                me->GetMotionMaster()->MovePoint(0, boatPath[1]);

                uint32 delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_2);

                    if (Creature* springDrifter = me->FindNearestCreature(NPC_SPRING_DRIFTER, 50.0f, true))
                        springDrifter->DespawnOrUnsummon();

                    if (Creature* saurokSlayer = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SPRING_SAUROK_SLAYER) : ObjectGuid::Empty))
                        saurokSlayer->AI()->DoAction(ACTION_START_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
                {
                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                        voljin->AI()->Talk(TALK_SPECIAL_12);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4800), [this](TaskContext context)
                {
                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                        voljin->AI()->Talk(TALK_SPECIAL_13);

                    if (Creature* saurokSlayer = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SPRING_SAUROK_SLAYER) : ObjectGuid::Empty))
                        saurokSlayer->AI()->DoAction(ACTION_IN_CAVE);
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Mogu Rune 67686
struct npc_dagger_in_dark_mogu_rune : public customCreatureAI
{
    npc_dagger_in_dark_mogu_rune(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->SetObjectScale(2.0f);

        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            DoCast(me, SPELL_MOGU_RUNE_POWERUP);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Darkhatched Shaman 67255
struct npc_dagger_in_dark_darkhatched_shaman : public customCreatureAI
{
    npc_dagger_in_dark_darkhatched_shaman(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_LIGHTNING_SHIELD);
        events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CHAIN_HEAL, 11 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING_SHIELD, 10 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_LIGHTNING_BOLT, 2.5 * IN_MILLISECONDS, EVENT_LIGHTNING_BOLT, eventId);
            ExecuteTargetEvent(SPELL_CHAIN_HEAL, 12 * IN_MILLISECONDS, EVENT_CHAIN_HEAL, eventId, PRIORITY_ALLY);
            ExecuteTargetEvent(SPELL_LIGHTNING_SHIELD, 10 * IN_MILLISECONDS, EVENT_LIGHTNING_SHIELD, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_LIZARD_LORD, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_SORCERER, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_CORRUOTED_DARKHATCHED_SLAYER, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_FLASH_HUNTER, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_ORACLE, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_SHAMAN, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_SKULKER, 35.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DARKHATCHED_SKINFLAYER, 35.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }
};

// Darkhatched Skulker 67257
struct npc_dagger_in_dark_darkhatched_skulker : public customCreatureAI
{
    npc_dagger_in_dark_darkhatched_skulker(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
        DoCast(me, SPELL_STEALTH);
        DoCast(me, SPELL_CRIPPLING_POISON);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_SHADOWSTEP);
        events.ScheduleEvent(EVENT_SHADOWSTEP, 10 * IN_MILLISECONDS);
    }

    void DamageDealt(Unit* /*victim*/, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (damageType == DIRECT_DAMAGE)
            me->RemoveAurasDueToSpell(SPELL_STEALTH);
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
            ExecuteTargetEvent(SPELL_SHADOWSTEP, 10 * IN_MILLISECONDS, EVENT_SHADOWSTEP, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Darkhatched Skinflayer 67258
struct npc_dagger_in_dark_darkhatched_skinflayer : public customCreatureAI
{
    npc_dagger_in_dark_darkhatched_skinflayer(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who, SPELL_FIXATE);
        events.ScheduleEvent(EVENT_WHIRLWIND, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FIXATE, 19 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
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
            ExecuteTargetEvent(SPELL_WHIRLWIND, 19 * IN_MILLISECONDS, EVENT_WHIRLWIND, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_FIXATE, 20 * IN_MILLISECONDS, EVENT_FIXATE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Broodmaster Noshi 67264
struct npc_dagger_in_dark_broodmaster_noshi : public customCreatureAI
{
    npc_dagger_in_dark_broodmaster_noshi(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    bool hasTriggered;
    bool reachLowHealth;

    void Reset() override
    {
        events.Reset();
        hasTriggered = false;
        reachLowHealth = false;
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(me, SPELL_SWING);

        if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
            voljin->Attack(me, true);

        events.ScheduleEvent(EVENT_STALACTITE, 10 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_THE_BROODMASTER, DONE);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (HealthBelowPct(30) && !hasTriggered)
        {
            hasTriggered = true;
            events.ScheduleEvent(EVENT_DEATH_NOVA, 0.5 * IN_MILLISECONDS);
        }

        if (HealthBelowPct(20) && !reachLowHealth)
        {
            reachLowHealth = true;

            // Achievement
            std::list<Creature*> eggs;
            GetCreatureListWithEntryInGrid(eggs, me, NPC_MATURE_SAUROK_EGG, 200.0f);

            eggs.remove_if([=](Creature* target) {return target && !target->IsAlive(); });

            if (eggs.empty())
                return;

            me->GetMap()->SetWorldState(WORLDSTATE_NO_EGG_LEFT_BEHIND, 0);
        }
    }

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
            if (eventId == EVENT_DEATH_NOVA)
            {
                if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                    voljin->AI()->DoAction(ACTION_BIG_BAD_VOOODOO);
            }

            ExecuteTargetEvent(SPELL_STALACTITE_LAUNCH, 11.5 * IN_MILLISECONDS, EVENT_STALACTITE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_DEATH_NOVA, 0, EVENT_DEATH_NOVA, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Stalactite 67755
struct npc_dagger_in_dark_stalactite : public customCreatureAI
{
    npc_dagger_in_dark_stalactite(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_STALACTITE_LAUNCH);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Corrupted Dark Hatched Egg 67798
struct npc_dagger_in_dark_corrupted_dark_hatched_egg : public customCreatureAI
{
    npc_dagger_in_dark_corrupted_dark_hatched_egg(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override 
    {
        SetCombatMovement(false);
    }

    void JustEngagedWith(Unit* who) override
    {
        me->Kill(me);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Corrupted Darkhatched Slayer 67832
struct npc_dagger_in_dark_corrupted_darkhatched_slayer : public customCreatureAI
{
    npc_dagger_in_dark_corrupted_darkhatched_slayer(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_STRIKE, 6 * IN_MILLISECONDS);
    }

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
            ExecuteTargetEvent(SPELL_STRIKE, 15 * IN_MILLISECONDS, EVENT_STRIKE, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Darkhatched Oracle 67256
struct npc_dagger_in_dark_darkhatched_oracle : public customCreatureAI
{
    npc_dagger_in_dark_darkhatched_oracle(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_WRATH, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, 7.5 * IN_MILLISECONDS);
    }

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
            ExecuteTargetEvent(SPELL_WRATH, 2.1 * IN_MILLISECONDS, EVENT_WRATH, eventId);
            ExecuteTargetEvent(SPELL_ENTANGLING_ROOTS, 11 * IN_MILLISECONDS, EVENT_ENTANGLING_ROOTS, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Darkhatched Flesh-Hunter 67259
struct npc_dagger_in_dark_darkhatched_flesh_hunter : public customCreatureAI
{
    npc_dagger_in_dark_darkhatched_flesh_hunter(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_THROW_SPEAR, 2 * IN_MILLISECONDS);
    }

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
            ExecuteTargetEvent(SPELL_THROW_SPEAR, urand(6 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_THROW_SPEAR, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Darkhatched Lizard-Lord 67263
struct npc_dagger_in_dark_darkhatched_lizard_lord : public customCreatureAI
{
    npc_dagger_in_dark_darkhatched_lizard_lord(Creature* creature) : customCreatureAI(creature) 
    {
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;
    ObjectGuid targetGUID;

    void Reset() override
    {
        summons.DespawnAll();
        events.Reset();
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_WATER_JETS, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CALL_HELP, 21 * IN_MILLISECONDS);

        if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
            voljin->AI()->Talk(TALK_SPECIAL_9);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;
            me->SetVisible(true);

            Movement::MoveSplineInit init(me);
            for (auto&& itr : lizardLordPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetUncompressed();
            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->SetHomePosition(*me);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
            });
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetFaction(1375); // required for water jets hit
    }

    void JustDied(Unit* /*killer*/) override
    {
        summons.DespawnAll();

        if (me->GetInstanceScript())
        {
            me->GetInstanceScript()->DoRemoveAurasDueToSpellOnPlayers(SPELL_FIXATE);
            me->GetInstanceScript()->SetData(DATA_GUARDS, DONE);
        }
    }


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
            switch (eventId)
            {
                case EVENT_CALL_HELP:
                    HandleSummonGuards();
                    events.ScheduleEvent(EVENT_CALL_HELP, 50 * IN_MILLISECONDS);
                    break;
                case EVENT_WATER_JETS:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        DoCast(target, SPELL_WATER_JETS);
                        me->PrepareChanneledCast(me->GetAngle(target));
                        events.ScheduleEvent(EVENT_WATER_JETS, 14.5 * IN_MILLISECONDS);

                        scheduler
                            .Schedule(Milliseconds(3500), [this](TaskContext context)
                        {
                            me->RemoveChanneledCast(targetGUID);
                        });
                    }
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void HandleSummonGuards()
    {
        Talk(TALK_INTRO);
        std::vector<uint32> guardians = { NPC_DARKHATCHED_ORACLE, NPC_DARKHATCHED_SKINFLAYER, NPC_DARKHATCHED_FLASH_HUNTER };

        for (uint32 i = 0; i < 3; i++)
            if (Creature* guard = me->SummonCreature(guardians[i], darkhatchedSwarmPos[i], TEMPSUMMON_MANUAL_DESPAWN))
                guard->SetInCombatWithZone();
    }
};

// Spring Saurok Slayer 67706
struct npc_dagger_in_dark_spring_saurok_slayer : public customCreatureAI
{
    npc_dagger_in_dark_spring_saurok_slayer(Creature* creature) : customCreatureAI(creature)
    {
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;

    void Reset() override
    {
        summons.DespawnAll();
        events.Reset();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
            {
                me->SetVisible(true);

                if (Creature* gun = me->SummonCreature(NPC_GOBLIN_CANNON, *me, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    gun->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    gun->EnterVehicle(me, 0);
                }
                break;
            }
            case ACTION_IN_CAVE:
            {
                if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                    voljin->EnterVehicle(me, 1);

                if (Creature* grizzle = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GRIZZLE_GEARSLIP) : ObjectGuid::Empty))
                    grizzle->EnterVehicle(me, 2);

                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();

                Movement::MoveSplineInit init(me);
                for (auto&& itr : slayerPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                init.SetWalk(true);
                init.SetFly();
                init.SetUncompressed();
                init.Launch();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), 1, [this](TaskContext context)
                {
                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(DATA_INTO_THE_CAVE, DONE);
                });

                scheduler
                    .Schedule(Milliseconds(1000), 2, [this](TaskContext context)
                {
                    std::list<Creature*> sourcerers;
                    GetCreatureListWithEntryInGrid(sourcerers, me, NPC_DARKHATCHED_SORCERER, 35.0f);

                    sourcerers.remove_if([=](Creature* target) { return target && !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); });

                    for (auto&& itr : sourcerers)
                    {
                        itr->AI()->Talk(TALK_INTRO);
                        itr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        itr->SetInCombatWithZone();
                        itr->CastSpell(me, SPELL_STASIS, false);
                    }

                    context.Repeat(Seconds(1));
                });
                break;
            }
            case ACTION_APPLY_STASIS:
                scheduler.CancelGroup(1);
                me->StopMoving();
                break;
            case ACTION_REMOVE_STASIS:
                if (me->HasAura(SPELL_STASIS))
                    break;

                me->StopMoving();
                scheduler
                    .Schedule(Milliseconds(1000), 1, [this](TaskContext context)
                {
                    Movement::MoveSplineInit init(me);
                    for (auto&& itr : slayerPath)
                    {
                        if (itr.GetPositionY() < me->GetPositionY())
                            continue;

                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));
                    }

                    init.SetWalk(true);
                    init.SetFly();
                    init.SetUncompressed();
                    init.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), 1, [this](TaskContext context)
                    {
                        if (me->GetInstanceScript())
                            me->GetInstanceScript()->SetData(DATA_INTO_THE_CAVE, DONE);
                    });
                });
                
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Rakgor Bloodrazor 67266
struct npc_dagger_in_dark_rakgor_bloodrazor : public customCreatureAI
{
    npc_dagger_in_dark_rakgor_bloodrazor(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        if (who)
            DoCast(who, SPELL_CHEAP_SHOT);

        events.ScheduleEvent(EVENT_CHEAP_SHOT, 15 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_GAS_BOMB, 3 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(TALK_SPECIAL_12);

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_DEFEAT_RAKGOR, DONE);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_DEFEAT_BROODMASTER:
            {
                me->SetVisible(true);

                // Summon our guards & move forward
                for (auto&& itr : bodyguardsCavePos)
                    me->SummonCreature(NPC_KORKRON_BODYGUARD, itr, TEMPSUMMON_MANUAL_DESPAWN);

                Movement::MoveSplineInit init(me);
                for (auto&& itr : bloodrazorCavePath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                init.SetUncompressed();
                init.Launch();

                uint32 delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4700), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_2);

                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                        voljin->GetMotionMaster()->MovePoint(0, voljinLastStand);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7400), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_3);

                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                        me->SetFacingToObject(voljin);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6300), [this](TaskContext context)
                {
                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                        voljin->AI()->Talk(TALK_SPECIAL_20);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
                {
                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                        voljin->AI()->Talk(TALK_SPECIAL_21);

                    for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
                        if (Player* player = itr.GetSource())
                            sScenarioMgr->SendScenarioState(player, 1095, DATA_INVESTIGATION, 0);

                    // Allow to use mogu tablets
                    std::list<Creature*> tablets;
                    GetCreatureListWithEntryInGrid(tablets, me, NPC_BROKEN_MOGU_TABLET, 200.0f);
                    GetCreatureListWithEntryInGrid(tablets, me, NPC_BROKEN_MOGU_TABLET_2, 200.0f);

                    for (auto&& itr : tablets)
                    {
                        itr->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                        itr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                });
                break;
            }
            case ACTION_GONNA_KILL_VOLJIN:
                if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                {
                    // Move forward to him
                    me->SetWalk(true);
                    float dist = me->GetExactDist2d(voljin) - 2.0f;
                    float x = me->GetPositionX() + dist * cos(me->GetAngle(voljin));
                    float y = me->GetPositionY() + dist * sin(me->GetAngle(voljin));
                    float z = me->GetPositionZ();
                    me->UpdateAllowedPositionZ(x, y, z);
                    me->GetMotionMaster()->MovePoint(0, x, y, z);

                    uint32 delay = me->GetSplineDuration() + 3000;
                    scheduler
                        .Schedule(Milliseconds(delay), [this](TaskContext context)
                    {
                        Talk(TALK_SPECIAL_5);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 6600), [this](TaskContext context)
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK1H);

                        if (Creature* newVoljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                            newVoljin->SetStandState(UNIT_STAND_STATE_DEAD);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
                    {
                        if (Creature* newVoljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                            newVoljin->AI()->Talk(TALK_SPECIAL_26);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
                    {
                        me->SetHomePosition(*me);
                        Talk(TALK_SPECIAL_5);
                        me->SetFaction(14);
                        me->SetInCombatWithZone();

                        // our guardians too
                        std::list<Creature*> korkronGuardians;
                        GetCreatureListWithEntryInGrid(korkronGuardians, me, NPC_KORKRON_BODYGUARD, 100.0f);

                        for (auto&& itr : korkronGuardians)
                        {
                            itr->SetFaction(14);
                            itr->SetInCombatWithZone();
                        }
                    });
                }
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_GAS_BOMB)
                Talk(urand(TALK_SPECIAL_6, TALK_SPECIAL_9));

            ExecuteTargetEvent(SPELL_CHEAP_SHOT, 12 * IN_MILLISECONDS, EVENT_CHEAP_SHOT, eventId);
            ExecuteTargetEvent(SPELL_GAS_BOMB, 10 * IN_MILLISECONDS, EVENT_GAS_BOMB, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Broken Mogu Tablet 67862, 67863
struct npc_dagger_in_dark_broken_mogu_tablet : public customCreatureAI
{
    npc_dagger_in_dark_broken_mogu_tablet(Creature* creature) : customCreatureAI(creature) 
    {
        hasTriggered = 0;
    }

    uint32 hasTriggered;

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        clicker->CastSpell(clicker, me->GetEntry() == NPC_BROKEN_MOGU_TABLET ? SPELL_INVESTIGATION : SPELL_INVESTIGATION_2, false);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_TABLET_USING)
            return hasTriggered;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_TABLET_USING)
            hasTriggered = data;
    }
};

// Goblin Cannon 67694
struct npc_dagger_in_dark_goblin_cannon : public customCreatureAI
{
    npc_dagger_in_dark_goblin_cannon(Creature* creature) : customCreatureAI(creature) { }

    uint32 hasTriggered;

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (me->HasAura(SPELL_RIDE_VEHICLE))
            return;

        clicker->CastSpell(me, SPELL_RIDE_VEHICLE, true);
    }
};

// Shadowstep 132649
class spell_dagger_in_dark_shadowstep_selector : public SpellScript
{
    PrepareSpellScript(spell_dagger_in_dark_shadowstep_selector);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_SHADOWSTEP_EFF, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dagger_in_dark_shadowstep_selector::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dagger_in_dark_shadowstep_selector::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Shadowstep Eff 132647
class spell_dagger_in_dark_shadowstep_eff : public SpellScript
{
    PrepareSpellScript(spell_dagger_in_dark_shadowstep_eff);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->AttackStart(target);
                caster->SetReactState(REACT_PASSIVE);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dagger_in_dark_shadowstep_eff::HandleEffectHit, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Hatch Selector 133752
class spell_dagger_in_dark_hatch_selector : public SpellScript
{
    PrepareSpellScript(spell_dagger_in_dark_hatch_selector);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_MATURE_SAUROK_EGG; });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dagger_in_dark_hatch_selector::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Hatch Eff 133702
class spell_dagger_in_dark_hatch_eff : public AuraScript
{
    PrepareAuraScript(spell_dagger_in_dark_hatch_eff);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* egg = GetOwner()->ToCreature())
        {
            egg->CastSpell(egg, SPELL_HATCH_EFF, true);
            egg->Kill(egg);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_dagger_in_dark_hatch_eff::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Death Nova 133804
class spell_dagger_in_dark_death_nova : public SpellScript
{
    PrepareSpellScript(spell_dagger_in_dark_death_nova);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_BIG_BAD_VOODOO_EFF); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dagger_in_dark_death_nova::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Water jets 133121
class spell_dagger_in_dark_water_jets : public SpellScript
{
    PrepareSpellScript(spell_dagger_in_dark_water_jets);

    void HandleBeforeCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->Talk(TALK_SPECIAL_1);
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            if (target->GetInstanceScript())
                target->GetInstanceScript()->SetData(WATER_JETS_SLAIN_DATA, 1);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DARKHATCHED_SHAMAN && target->GetEntry() != NPC_DARKHATCHED_SKULKER && target->GetEntry() != NPC_DARKHATCHED_SKINFLAYER && target->GetEntry() != NPC_DARKHATCHED_ORACLE && target->GetEntry() != NPC_DARKHATCHED_FLASH_HUNTER; });
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_dagger_in_dark_water_jets::HandleBeforeCast);
        OnEffectHitTarget += SpellEffectFn(spell_dagger_in_dark_water_jets::HandleEffectHit, EFFECT_3, SPELL_EFFECT_INSTAKILL);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dagger_in_dark_water_jets::FilterTargets, EFFECT_3, TARGET_UNIT_CONE_ENEMY_110);
    }
};

// Stasis 133548
class spell_dagger_in_dark_stasis : public AuraScript
{
    PrepareAuraScript(spell_dagger_in_dark_stasis);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* boat = GetOwner()->ToCreature())
        {
            boat->AI()->DoAction(ACTION_APPLY_STASIS);

            if (Creature* grizzle = ObjectAccessor::GetCreature(*boat, boat->GetInstanceScript() ? boat->GetInstanceScript()->GetGuidData(NPC_GRIZZLE_GEARSLIP) : ObjectGuid::Empty))
                grizzle->AI()->Talk(TALK_SPECIAL_3);
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* boat = GetOwner()->ToCreature())
            boat->AI()->DoAction(ACTION_REMOVE_STASIS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_dagger_in_dark_stasis::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_dagger_in_dark_stasis::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_DECREASE_SPEED, AURA_EFFECT_HANDLE_REAL);
    }
};

// Investigation 133749, 133750
class spell_dagger_in_dark_investigation : public SpellScript
{
    PrepareSpellScript(spell_dagger_in_dark_investigation);

    void HandleAfterCast()
    {
        if (Unit* caster = GetCaster())
        {
            uint32 tabletEntry = GetSpellInfo()->Id == SPELL_INVESTIGATION ? NPC_BROKEN_MOGU_TABLET : NPC_BROKEN_MOGU_TABLET_2;

            if (Creature* moguTablet = caster->FindNearestCreature(tabletEntry, 100.0f, true))
            {
                if (moguTablet->AI()->GetData(TYPE_TABLET_USING))
                    return;

                moguTablet->AI()->SetData(TYPE_TABLET_USING, 1);
                moguTablet->AI()->Talk(TALK_INTRO);
                moguTablet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                // Update Criteria
                uint32 criteriaId = GetSpellInfo()->Id == SPELL_INVESTIGATION ? CRITERIA_WEST_STATUE : CRITERIA_EAST_STATUE;

                if (caster->GetInstanceScript())
                {
                    caster->GetInstanceScript()->SendScenarioProgressUpdate(CriteriaProgressData(criteriaId, 1, caster->GetInstanceScript()->GetScenarioGUID(), time(NULL), 0, 0));
                    caster->GetInstanceScript()->SetData(DATA_TABLETS_COUNT, 1);
                }
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_dagger_in_dark_investigation::HandleAfterCast);
    }
};

// 556. Summoned by 133471 - Mogu Rune Powerup
class sat_dagger_in_dark_mogu_rune_powerup : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
        {
            itr->CastSpell(itr, SPELL_MOGU_POWER, true);

            if (GetCaster() && GetCaster()->ToCreature())
            {
                GetCaster()->ToCreature()->AI()->Talk(TALK_INTRO, itr);
                GetCaster()->ToCreature()->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
        }
    }
};

// 573. Summoned by 133892 - Big Bad Voodoo
class sat_dagger_in_dark_big_bad_voodoo : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_BIG_BAD_VOODOO_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_BIG_BAD_VOODOO_EFF);
    }
};

// 514. Summoned by 132992 - Gas Bomb
class sat_dagger_in_dark_gas_bomb : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_GAS_BOMB_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_GAS_BOMB_EFF);
    }
};

// 515. Summoned by 133005 - Healing Ward
class sat_dagger_in_dark_healing_ward : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_HEALING_WARD_EFF, true);

            // Achievement
            if (Creature* bloodrazor = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_RAKGOR_BLOODRAZOR + 1) : ObjectGuid::Empty))
                if (bloodrazor->IsInCombat())
                    target->GetMap()->SetWorldState(WORLDSTATE_U_MEAN_THATS_NOT_VOID_ZONE, 0);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_HEALING_WARD_EFF);
    }
};

void AddSC_dagger_in_the_dark()
{
    new npc_dagger_in_dark_voljin();
    new creature_script<npc_dagger_in_dark_grizzle_gearslip>("npc_dagger_in_dark_grizzle_gearslip");
    new creature_script<npc_dagger_in_dark_mogu_rune>("npc_dagger_in_dark_mogu_rune");
    new creature_script<npc_dagger_in_dark_darkhatched_shaman>("npc_dagger_in_dark_darkhatched_shaman");
    new creature_script<npc_dagger_in_dark_darkhatched_skulker>("npc_dagger_in_dark_darkhatched_skulker");
    new creature_script<npc_dagger_in_dark_darkhatched_skinflayer>("npc_dagger_in_dark_darkhatched_skinflayer");
    new creature_script<npc_dagger_in_dark_broodmaster_noshi>("npc_dagger_in_dark_broodmaster_noshi");
    new creature_script<npc_dagger_in_dark_stalactite>("npc_dagger_in_dark_stalactite");
    new creature_script<npc_dagger_in_dark_corrupted_dark_hatched_egg>("npc_dagger_in_dark_corrupted_dark_hatched_egg");
    new creature_script<npc_dagger_in_dark_corrupted_darkhatched_slayer>("npc_dagger_in_dark_corrupted_darkhatched_slayer");
    new creature_script<npc_dagger_in_dark_darkhatched_oracle>("npc_dagger_in_dark_darkhatched_oracle");
    new creature_script<npc_dagger_in_dark_darkhatched_flesh_hunter>("npc_dagger_in_dark_darkhatched_flesh_hunter");
    new creature_script<npc_dagger_in_dark_darkhatched_lizard_lord>("npc_dagger_in_dark_darkhatched_lizard_lord");
    new creature_script<npc_dagger_in_dark_spring_saurok_slayer>("npc_dagger_in_dark_spring_saurok_slayer");
    new creature_script<npc_dagger_in_dark_rakgor_bloodrazor>("npc_dagger_in_dark_rakgor_bloodrazor");
    new creature_script<npc_dagger_in_dark_broken_mogu_tablet>("npc_dagger_in_dark_broken_mogu_tablet");
    new creature_script<npc_dagger_in_dark_goblin_cannon>("npc_dagger_in_dark_goblin_cannon");
    
    new spell_script<spell_dagger_in_dark_shadowstep_selector>("spell_dagger_in_dark_shadowstep_selector");
    new spell_script<spell_dagger_in_dark_shadowstep_eff>("spell_dagger_in_dark_shadowstep_eff");
    new spell_script<spell_dagger_in_dark_hatch_selector>("spell_dagger_in_dark_hatch_selector");
    new aura_script<spell_dagger_in_dark_hatch_eff>("spell_dagger_in_dark_hatch_eff");
    new spell_script<spell_dagger_in_dark_death_nova>("spell_dagger_in_dark_death_nova");
    new spell_script<spell_dagger_in_dark_water_jets>("spell_dagger_in_dark_water_jets");
    new aura_script<spell_dagger_in_dark_stasis>("spell_dagger_in_dark_stasis");
    new spell_script<spell_dagger_in_dark_investigation>("spell_dagger_in_dark_investigation");
    new atrigger_script<sat_dagger_in_dark_mogu_rune_powerup>("sat_dagger_in_dark_mogu_rune_powerup");
    new atrigger_script<sat_dagger_in_dark_big_bad_voodoo>("sat_dagger_in_dark_big_bad_voodoo");
    new atrigger_script<sat_dagger_in_dark_gas_bomb>("sat_dagger_in_dark_gas_bomb");
    new atrigger_script<sat_dagger_in_dark_healing_ward>("sat_dagger_in_dark_healing_ward");
}