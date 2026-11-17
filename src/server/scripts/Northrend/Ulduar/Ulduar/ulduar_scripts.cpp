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
 
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ulduar.h"
#include "CombatAI.h"
#include "PassiveAI.h"

enum Events
{
    // Ironwork Cannon
    EVENT_FLAME_CANNON      = 1,
    EVENT_CHECK_AVAILABLE
};

enum Spells
{
    SPELL_FLAME_CANNON      = 62395
};

struct RangeCheck 
{
    RangeCheck(Unit* base, float min, float max) : __base(base), __mindist(min), __maxdist(max) { }

    bool operator()(Unit* const other) const
    {
        float dist = __base->GetDistance(other->GetPositionX(), other->GetPositionY(), other->GetPositionZ());
        if ( dist >= __mindist && dist <= __maxdist)
            return true;
        return false;
    }

    private:
        Unit* __base;
        float __mindist;
        float __maxdist;
};

class npc_ironwork_cannon : public CreatureScript
{
    public:
        npc_ironwork_cannon () : CreatureScript("npc_ironwork_cannon") { }

        struct npc_ironwork_cannonAI: public ScriptedAI
        {
            npc_ironwork_cannonAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetDisplayFromModel(1);
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 100);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (who->GetTypeId() != TYPEID_PLAYER && !who->IsVehicle())
                   _EnterEvadeMode();

                events.ScheduleEvent(EVENT_CHECK_AVAILABLE, 5*IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_FLAME_CANNON, 1.2*IN_MILLISECONDS);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->IsWithinDistInMap(who, 100.0f) && !me->IsInCombat() && (who->GetTypeId() == TYPEID_PLAYER || who->IsVehicle()))
                    me->AI()->AttackStart(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() && !me->IsInCombat())
                    return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_FLAME_CANNON:
                            if (Unit* dest = SelectTarget(SELECT_TARGET_RANDOM, 0, RangeCheck(me, 30.0f, 200.0f)))
                                DoCast(dest, SPELL_FLAME_CANNON);
                            events.ScheduleEvent(EVENT_FLAME_CANNON, 1.5*IN_MILLISECONDS);
                            break;
                        case EVENT_CHECK_AVAILABLE:
                            if (!me->FindNearestPlayer(100.0f) && (!me->FindNearestCreature(NPC_SALVAGED_SIEGE_ENGINE, 100.0f, true) || !me->FindNearestCreature(NPC_CHOPPER, 100.0f, true) || !me->FindNearestCreature(NPC_SALVAGED_DEMOLISHER, 100.0f, true)))
                            {
                                _EnterEvadeMode();
                                me->GetMotionMaster()->MoveTargetedHome();
                            }
                            events.ScheduleEvent(EVENT_CHECK_AVAILABLE, 5*IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ironwork_cannonAI>(creature);
        }
};

enum EventUlduar
{
    // npc lore keeper
    SAY_PENTARUS_1              = 0,
    SAY_PENTARUS_2              = 1,
    SAY_BRANN_1                 = 0,
    SAY_BRANN_2                 = 1,
    SAY_BRANN_3                 = 2,
    SAY_BRANN_4                 = 3,
    SAY_BRANN_5                 = 4,
     // Kirin Tor Battle-Mage and Hired Engineer mini event
    KIRIN_TOR_BATTLE_MAGE_SAY_1 = 0,
    KIRIN_TOR_BATTLE_MAGE_SAY_2 = 1,
    HIRED_ENGINEER_SAY_1        = 0,
    HIRED_ENGINEER_SAY_2        = 1,
    HIRED_ENGINEER_SAY_3        = 2,
    SAY_TOWER_OF_FLAME          = 0,
    SAY_TOWER_OF_STORM          = 1,
    SAY_TOWER_OF_FROST          = 2,
    SAY_TOWER_OF_LIFE           = 3,
    SAY_REPAIR_STATION          = 4,
    // brann says
    SAY_EVENT_1                 = 0,
    SAY_EVENT_2                 = 1,
    SAY_EVENT_3                 = 2,
    SAY_EVENT_4                 = 3,
    SAY_EVENT_5                 = 4,
    SAY_EVENT_6                 = 5,
    SAY_EVENT_7                 = 6,
    SAY_EVENT_8                 = 7,
    SAY_EVENT_BRANN             = 8,
    SAY_EVENT_BRANN2            = 9,
    SAY_EVENT_PREMATURE_END     = 10,
    SAY_EVENT_WHISPER           = 11
};

enum Actions
{
    ACTION_START             = 1,
    ACTION_DELLORAH_LEAVE    = 2,
    ACTION_DELLORAH_LEAVE_PRE= 3,
};

enum SpellsEvent
{
    SPELL_TRANSITUS_SHIELD_BEAM = 48310,
    SPELL_SUMMON_IRON_DWARF_WATCHER  = 63135,
};

enum Points
{
    POINT_DELLORAH_LEAVE = 1,
    POINT_DELLORAH_LEAVE_PREMATURE = 2,
};

class npc_ulduar_warmage : public CreatureScript
{
    public:
        npc_ulduar_warmage() : CreatureScript("npc_ulduar_warmage") { }

        struct npc_ulduar_warmageAI : public ScriptedAI
        {
            npc_ulduar_warmageAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            ObjectGuid targetGUID;

            void Reset() override
            {
                targetGUID = ObjectGuid::Empty;
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (me->IsNonMeleeSpellCasted(false))
                    return;

                if (me->GetEntry() == NPC_ULDUAR_WARMAGE)
                {
                    if (!targetGUID)
                    {
                        std::list<Creature*> orbList;
                        GetCreatureListWithEntryInGrid(orbList, me, NPC_TRANSITUS_SHIELD_DUMMY, 40.0f);
                        if (!orbList.empty())
                        {
                            for (auto&& orb : orbList)
                            {
                                if (orb->GetPositionY() < 1000)
                                {
                                    targetGUID = orb->GetGUID();
                                    orb->SetDisplayId(11686); // make invisible
                                    orb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    break;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (!targetGUID)
                        if (Creature* orb = GetClosestCreatureWithEntry(me, NPC_TRANSITUS_SHIELD_DUMMY, 40.0f))
                        {
                            orb->SetDisplayId(11686); // make invisible
                            orb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            targetGUID = orb->GetGUID();
                        }
                }

                if (Creature* orb = me->GetCreature(*me, targetGUID))
                    DoCast(orb, SPELL_TRANSITUS_SHIELD_BEAM);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ulduar_warmageAI(creature);
        }
};

class npc_brann_bronzebeard : public CreatureScript
{
    public:
        npc_brann_bronzebeard() : CreatureScript("npc_brann_bronzebeard") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance)
                return true;

            instance->instance->LoadGrid(364, -16); // make sure leviathan is loaded
            if (instance->GetBossState(BOSS_LEVIATHAN) != DONE && !instance->GetData(DATA_SHIELD_DISABLED))
            {
                player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_brann_bronzebeardAI : public ScriptedAI
        {
            npc_brann_bronzebeardAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                steppingBrann = 0;
                loaded = true;
            }

            void Reset() override
            {
                if (instance->GetData(DATA_SHIELD_DISABLED) || instance->GetBossState(BOSS_LEVIATHAN) == DONE)
                    loaded = false;
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 optionId) override
            {
                if (!player)
                    return false;

                if (instance->GetData(DATA_SHIELD_DISABLED) || instance->GetBossState(BOSS_LEVIATHAN) == DONE)
                    return false;

                if (menuId == 10355 && optionId == 0)
                {
                    player->CLOSE_GOSSIP_MENU();

                    if (Creature* loreKeeper = me->FindNearestCreature(NPC_LOREKEEPER, 200.0f))
                        loreKeeper->AI()->DoAction(ACTION_START);

                    SetGUID(player->GetGUID(), ACTION_START);
                }
                return true;
            }

            void EventBrann()
            {
                if (!steppingBrann)
                    return;

                switch (steppingBrann)
                {
                    case 1:
                        if (Player* player = ObjectAccessor::GetPlayer(*me, playerGUID))
                            Talk(SAY_BRANN_1, player);
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        JumpSteppingBrann(4000);
                        break;
                    case 2:
                        if (Creature* pentarus = me->FindNearestCreature(NPC_PENTARUS, 20.0f))
                            pentarus->AI()->Talk(SAY_PENTARUS_1);
                        JumpSteppingBrann(4000);
                        break;
                    case 3:
                        Talk(SAY_BRANN_2);
                        JumpSteppingBrann(8000);
                        break;
                    case 4:
                        if (Unit* pIngenieur = me->FindNearestCreature(NPC_ULDUAR_WARMAGE, 50.0f))
                            pIngenieur->GetMotionMaster()->MovePoint(0, -777.336f,-45.084f,429.843f);
                        if (Unit* bataille = me->FindNearestCreature(NPC_ULDUAR_WARMAGE, 50.0f))
                        {
                            bataille->SetWalk(true);
                            bataille->SetSpeed(MOVE_WALK, 1, true);
                            bataille->GetMotionMaster()->MovePoint(0, -682.470520f, -80.405426f, 427.573029f);
                            bataille->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                        }
                        if (Unit* mage = me->FindNearestCreature(NPC_TOR_MAGE, 50.0f))
                        {
                            mage->SetWalk(true);
                            mage->SetSpeed(MOVE_WALK, 1, true);
                            mage->GetMotionMaster()->MovePoint(0, -682.969788f, -75.015648f, 427.604187f);
                            mage->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                        }
                        if (Unit* pentarus = me->FindNearestCreature(NPC_PENTARUS, 20.0f))
                        {
                            pentarus->SetWalk(true);
                            pentarus->SetSpeed(MOVE_WALK, 1, true);
                            pentarus->GetMotionMaster()->MovePoint(0, -678.194580f, -77.385101f, 426.988464f);
                            pentarus->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                        }
                        me->SetWalk(true);
                        me->SetSpeed(MOVE_WALK, 1, true);
                        me->GetMotionMaster()->MovePoint(0, -674.219055f, -45.745911f, 426.139160f);
                        JumpSteppingBrann(19000);
                        break;
                    case 5:
                        if (Creature* pentarus = me->FindNearestCreature(NPC_PENTARUS, 50.0f))
                            pentarus->AI()->Talk(SAY_PENTARUS_2);
                        JumpSteppingBrann(5000);
                        break;
                    case 6:
                        Talk(SAY_BRANN_3);
                        JumpSteppingBrann(4000);
                        break;
                    case 7:
                        if (me->FindNearestGameObject(GO_CUPOLE, 250.0f))
                            me->FindNearestGameObject(GO_CUPOLE, 250.0f)->RemoveFromWorld();

                        if (Creature* dummy = me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f))
                            me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f)->RemoveFromWorld();
                        JumpSteppingBrann(100);
                        break;
                    case 8:
                    {
                        if (Creature* dummy = me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f))
                            me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f)->RemoveFromWorld();
                        instance->SetData(DATA_SHIELD_DISABLED, true);
                        instance->SetData(DATA_LEVI_VEHICLES, false);
                        JumpSteppingBrann(7900);
                        break;
                    }
                    case 9:
                        Talk(SAY_BRANN_4);
                        JumpSteppingBrann(10000);
                        break;
                    case 10:
                        Talk(SAY_BRANN_5);
                        JumpSteppingBrann(8000);
                        break;
                    default:
                        break;
                }
            }

            void JumpSteppingBrann(uint32 time)
            {
                stepTimer = time;
                steppingBrann++;
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                switch (type)
                {
                    case ACTION_START:
                        steppingBrann = 1;
                        playerGUID = guid;
                        EventBrann();
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (stepTimer <= diff)
                   EventBrann();
                else
                   stepTimer -= diff;

                if (!loaded)
                {
                    loaded = true;
                    me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    if (instance->GetBossState(BOSS_LEVIATHAN) != DONE)
                        instance->SetData(DATA_LEVI_VEHICLES, false);
                    if (me->FindNearestGameObject(GO_CUPOLE, 250.0f))
                        me->FindNearestGameObject(GO_CUPOLE, 250.0f)->RemoveFromWorld();
                    if (Creature* dummy = me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f))
                        me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f)->RemoveFromWorld();
                    if (Creature* dummy = me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f))
                        me->FindNearestCreature(NPC_TRANSITUS_SHIELD_DUMMY, 250.0f)->RemoveFromWorld();
                    if (Unit* pIngenieur = me->FindNearestCreature(NPC_ULDUAR_WARMAGE, 50.0f))
                        pIngenieur->GetMotionMaster()->MovePoint(0, -777.336f,-45.084f,429.843f);
                    if (Unit* bataille = me->FindNearestCreature(NPC_ULDUAR_WARMAGE, 50.0f))
                    {
                        bataille->SetWalk(true);
                        bataille->SetSpeed(MOVE_WALK, 1, true);
                        bataille->GetMotionMaster()->MovePoint(0, -682.470520f, -80.405426f, 427.573029f);
                        bataille->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                    }
                    if (Unit* mage = me->FindNearestCreature(NPC_TOR_MAGE, 50.0f))
                    {
                        mage->SetWalk(true);
                        mage->SetSpeed(MOVE_WALK, 1, true);
                        mage->GetMotionMaster()->MovePoint(0, -682.969788f, -75.015648f, 427.604187f);
                        mage->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                    }
                    if (Unit* pentarus = me->FindNearestCreature(NPC_PENTARUS, 20.0f))
                    {
                        pentarus->SetWalk(true);
                        pentarus->SetSpeed(MOVE_WALK, 1, true);
                        pentarus->GetMotionMaster()->MovePoint(0, -678.194580f, -77.385101f, 426.988464f);
                        pentarus->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                    }
                    me->SetWalk(true);
                    me->SetSpeed(MOVE_WALK, 1, true);
                    me->GetMotionMaster()->MovePoint(0, -674.219055f, -45.745911f, 426.139160f);
                }

                return;
            }

        private:
            uint32 stepTimer;
            uint32 steppingBrann;
            ObjectGuid playerGUID = ObjectGuid::Empty;
            bool loaded;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_brann_bronzebeardAI>(creature);
        }
};

class npc_ulduar_engineer : public CreatureScript
{
    public:
        npc_ulduar_engineer() : CreatureScript("npc_ulduar_engineer") { }

        struct npc_ulduar_engineerAI : public ScriptedAI
        {
            npc_ulduar_engineerAI(Creature* creature) : ScriptedAI(creature)
            {
                step = 0;
                me->m_Events.Schedule(1, [this]()
                {
                    if (me->FindNearestGameObject(GO_ULDUAR_TELEPORTER, 10.0f))
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                });
                SetCombatMovement(false);
            }

            void DoAction(int32 action) override
            {
                step = 1;
                stepTimer = 100;
                event();
            }

            void event()
            {
                if (!step)
                    return;

                switch (step)
                {
                    case 1:
                        JumpIntro(5000);
                        if (InstanceScript* instance = me->GetInstanceScript())
                        {
                            if (instance->GetData(DATA_COLOSSUS) == 2)
                            {
                                Talk(HIRED_ENGINEER_SAY_3);
                                step = 0;
                                // no return, NPC_ULDUAR_WARMAGE has to say his line regardless
                            }
                        }
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Creature* mage = me->FindNearestCreature(NPC_ULDUAR_WARMAGE, 10.0f))
                            mage->AI()->Talk(KIRIN_TOR_BATTLE_MAGE_SAY_1);
                        break;
                    case 2:
                        Talk(HIRED_ENGINEER_SAY_1);
                        JumpIntro(7000);
                        break;
                    case 3:
                        if (Creature* mage = me->FindNearestCreature(NPC_ULDUAR_WARMAGE, 10.0f))
                            mage->AI()->Talk(KIRIN_TOR_BATTLE_MAGE_SAY_2);
                        JumpIntro(10000);
                        break;
                    case 4:
                        Talk(HIRED_ENGINEER_SAY_2);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        step = 0;
                        break;
                    default:
                        break;
                }
            }

            void JumpIntro(uint32 time)
            {
                stepTimer = time;
                step++;
            }

            void UpdateAI(uint32 diff) override
            {
                if (stepTimer <= diff)
                   event();
                else
                   stepTimer -= diff;

                return;
            }

        private:
            uint32 stepTimer;
            uint32 step;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ulduar_engineerAI>(creature);
        }
};

class npc_ulduar_lorekeeper : public CreatureScript
{
    public:
        npc_ulduar_lorekeeper() : CreatureScript("npc_ulduar_lorekeeper") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (!instance)
                return true;

            instance->instance->LoadGrid(364, -16); // make sure leviathan is loaded
            if (instance->GetBossState(BOSS_LEVIATHAN) != DONE && !instance->GetData(DATA_SHIELD_DISABLED) && !instance->GetData(DATA_LEVI_HARD_MODE) && !creature->AI()->GetData(0))
            {
                player->PrepareGossipMenu(creature, player->GetDefaultGossipMenuForSource(creature));
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_ulduar_lorekeeperAI : public ScriptedAI
        {
            npc_ulduar_lorekeeperAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
                introStep = 0;
                event = false;
                playerGUID = ObjectGuid::Empty;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_START)
                {
                    if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                        dellorah->AI()->DoAction(ACTION_DELLORAH_LEAVE_PRE);

                    event = true;
                    return;
                }

                introStep = 1;
                introTimer = 100;
                Intro();
            }

            uint32 GetData(uint32 type) const override
            {
                return event;
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 optionId) override
            {
                if (!player)
                    return false;

                if (instance->GetData(DATA_SHIELD_DISABLED) || instance->GetData(DATA_LEVI_HARD_MODE) || instance->GetBossState(BOSS_LEVIATHAN) == DONE)
                    return false;

                if (menuId == 10477 && optionId == 0)
                {
                    instance->SetData(DATA_LEVI_HARD_MODE, true);

                    if (Creature* leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_LEVIATHAN)))
                        leviathan->AI()->DoAction(ACTION_START_HARD_MODE);

                    hardStep = 1;
                    hardTimer = 100;
                    HardMode(player->GetGUID());
                }
                return true;
            }

            void Intro()
            {
                if (!introStep)
                    return;

                if (event)
                    return;

                switch (introStep)
                {
                    case 1:
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            dellorah->AI()->Talk(SAY_EVENT_1);
                        JumpIntro(10000);
                        break;
                    case 2:
                        Talk(SAY_EVENT_1);
                        JumpIntro(10000);
                        break;
                    case 3:
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            dellorah->AI()->Talk(SAY_EVENT_2);
                        JumpIntro(10000);
                        break;
                    case 4:
                        Talk(SAY_EVENT_2);
                        JumpIntro(10000);
                        break;
                    case 5:
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            dellorah->AI()->Talk(SAY_EVENT_3);
                        JumpIntro(10000);
                        break;
                    case 6:
                        Talk(SAY_EVENT_3);
                        JumpIntro(10000);
                        break;
                    case 7:
                        Talk(SAY_EVENT_4);
                        JumpIntro(10000);
                        break;
                    case 8:
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            dellorah->AI()->Talk(SAY_EVENT_4);
                        JumpIntro(10000);
                        break;
                    case 9:
                        if (Creature* Rhydian = me->FindNearestCreature(NPC_RHYDIAN, 150.0f))
                        {
                            if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            {
                                dellorah->SetTarget(Rhydian->GetGUID());
                                dellorah->AI()->Talk(SAY_EVENT_5);
                            }
                        }
                        JumpIntro(6000);
                        break;
                    case 10:
                        if (Creature* Rhydian = me->FindNearestCreature(NPC_RHYDIAN, 150.0f))
                        {
                            if (Creature* brann = me->FindNearestCreature(NPC_BRANN_BRONZBEARD, 200.0f, true))
                            {
                                Rhydian->AI()->Talk(SAY_EVENT_7);
                                Rhydian->GetMotionMaster()->MovePoint(0, brann->GetPositionX() - 4, brann->GetPositionY(), brann->GetPositionZ());
                            }
                        }
                        JumpIntro(500);
                        break;
                    case 11:
                        if (Creature* Nognnanon = me->FindNearestCreature(NPC_NORGANNON, 150.0f))
                        {
                            if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            {
                                dellorah->SetTarget(Nognnanon->GetGUID());
                                dellorah->AI()->Talk(SAY_EVENT_6);
                            }
                        }
                        JumpIntro(10000);
                        break;
                    case 12:
                        Talk(SAY_EVENT_8);
                        if (Creature* Rhydian = me->FindNearestCreature(NPC_RHYDIAN, 200.0f))
                        {
                            if (Creature* brann = me->FindNearestCreature(NPC_BRANN_BRONZBEARD, 200.0f, true))
                            {
                                Rhydian->SetTarget(brann->GetGUID());
                                Rhydian->AI()->Talk(SAY_EVENT_WHISPER);
                            }
                        }
                        JumpIntro(10000);
                        break;
                    case 13:
                        if (Creature* Rhydian = me->FindNearestCreature(NPC_RHYDIAN, 200.0f))
                        {
                            if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 200.0f))
                                Rhydian->GetMotionMaster()->MovePoint(0, dellorah->GetPositionX() - 2, dellorah->GetPositionY(), dellorah->GetPositionZ());
                        }
                        JumpIntro(10000);
                        break;
                    case 14:
                        if (Creature* Rhydian = me->FindNearestCreature(NPC_RHYDIAN, 200.0f))
                        {
                            if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 200.0f))
                            {
                                Rhydian->GetMotionMaster()->MovePoint(0, dellorah->GetPositionX() - 2, dellorah->GetPositionY(), dellorah->GetPositionZ());
                                dellorah->AI()->Talk(SAY_EVENT_8);
                            }
                        }
                        event = true;
                        break;
                    default:
                        break;
                }
            }

            void HardMode(ObjectGuid guid)
            {
                if (!hardStep)
                    return;

                playerGUID = guid;
                event = true;

                switch (hardStep)
                {
                    case 1:
                        if (Creature* brann = me->FindNearestCreature(NPC_BRANN_BRONZBEARD, 200.0f, true))
                            brann->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 150.0f))
                            if (Player* player = ObjectAccessor::GetPlayer(*me, playerGUID))
                                dellorah->AI()->Talk(SAY_EVENT_BRANN, player);
                        JumpHard(4000);
                        break;
                    case 2:
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 200.0f, true))
                        {
                            if (Creature* brann = me->FindNearestCreature(NPC_BRANN_BRONZBEARD, 200.0f, true))
                                dellorah->GetMotionMaster()->MovePoint(0, brann->GetPositionX() - 4, brann->GetPositionY(), brann->GetPositionZ());
                        }
                        JumpHard(12000);
                        break;
                    case 3:
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 200.0f, true))
                        {
                            if (Creature* brann = me->FindNearestCreature(NPC_BRANN_BRONZBEARD, 200.0f, true))
                            {
                                dellorah->SetTarget(brann->GetGUID());
                                if (Player* player = ObjectAccessor::GetPlayer(*me, playerGUID))
                                    dellorah->AI()->Talk(SAY_EVENT_BRANN2, player);
                            }
                        }
                        JumpHard(5000);
                        break;
                    case 4:
                        if (Creature* brann = me->FindNearestCreature(NPC_BRANN_BRONZBEARD, 200.0f, true))
                            brann->AI()->SetGUID(playerGUID, ACTION_START);
                        if (Creature* dellorah = me->FindNearestCreature(NPC_DELORAH, 200.0f, true))
                        {
                            if (Creature* Keeper = me->FindNearestCreature(NPC_LOREKEEPER, 200.0f, true))
                            {
                                dellorah->SetTarget(ObjectGuid::Empty);
                                dellorah->AI()->DoAction(ACTION_DELLORAH_LEAVE);
                            }
                        }
                        JumpHard(5000);
                        break;
                    default:
                        break;
                }
            }

            void JumpIntro(uint32 timeIntro)
            {
                introTimer = timeIntro;
                introStep++;
            }

            void JumpHard(uint32 timeHard)
            {
                hardTimer = timeHard;
                hardStep++;
            }

            void UpdateAI(uint32 diff) override
            {
                if (introTimer <= diff)
                   Intro();
                else
                   introTimer -= diff;

                if (hardTimer <= diff && playerGUID)
                   HardMode(playerGUID);
                else
                   hardTimer -= diff;

                return;
            }

        private:
            uint32 introTimer;
            uint32 introStep;
            uint32 hardTimer;
            uint32 hardStep;
            ObjectGuid playerGUID;
            bool event;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ulduar_lorekeeperAI>(creature);
        }
};

Position const DellorahLeave[] =
{
    { -729.2102f, -54.20687f, 430.3027f },
    { -748.7102f, -63.70687f, 430.3027f },
    { -762.2102f, -78.20687f, 430.3027f },
    { -784.4602f, -98.95687f, 430.3027f },
    { -808.9602f, -121.7069f, 430.0527f },
    { -817.7102f, -127.9569f, 431.3027f },
    { -829.7102f, -127.9569f, 440.3027f },
    { -843.7102f, -128.7069f, 450.0527f },
    { -859.2102f, -132.2069f, 459.0527f },
    { -895.4602f, -141.2069f, 459.0527f },
    { -916.2640f, -143.4033f, 464.1398f },
};

// non-blizzlike
Position const DellorahLeavePremature[] =
{
    { -814.433472f, -149.438766f, 429.820496f },
    { -859.605042f, -148.812805f, 458.895416f },
    { -896.960327f, -149.138779f, 458.888092f },
    { -916.941528f, -148.930817f, 464.020111f },
};

class npc_high_explorer_dellorah : public CreatureScript
{
    public:
        npc_high_explorer_dellorah() : CreatureScript("npc_high_explorer_dellorah") { }

        struct npc_high_explorer_dellorahAI : public ScriptedAI
        {
            npc_high_explorer_dellorahAI(Creature* creature) : ScriptedAI(creature) { }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (point == POINT_DELLORAH_LEAVE)
                {
                    if (++waypointId < sizeof(DellorahLeave) / sizeof(DellorahLeave[0]))
                        me->GetMotionMaster()->MovePoint(POINT_DELLORAH_LEAVE, DellorahLeave[waypointId]);
                    else
                        me->DespawnOrUnsummon();
                }
                else if (point == POINT_DELLORAH_LEAVE_PREMATURE)
                {
                    if (++waypointId < sizeof(DellorahLeavePremature) / sizeof(DellorahLeavePremature[0]))
                        me->GetMotionMaster()->MovePoint(POINT_DELLORAH_LEAVE_PREMATURE, DellorahLeavePremature[waypointId]);
                    else
                        me->DespawnOrUnsummon();
                }
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_DELLORAH_LEAVE)
                    me->GetMotionMaster()->MovePoint(POINT_DELLORAH_LEAVE, DellorahLeave[waypointId]);
                else if (action == ACTION_DELLORAH_LEAVE_PRE)
                {
                    me->m_Events.Schedule( 5000, [this]() { Talk(SAY_EVENT_PREMATURE_END); });
                    me->m_Events.Schedule(10000, [this]() { me->GetMotionMaster()->MovePoint(POINT_DELLORAH_LEAVE_PREMATURE, DellorahLeavePremature[waypointId]); });
                }
            }

        private:
            uint32 waypointId = 0;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_high_explorer_dellorahAI>(creature);
        }
};

enum EventsDefender
{
    EVENT_SUNDER_ARMOR = 1,
    EVENT_HAMSTRING,
    EVENT_LIGHTNING_BOLT,
    EVENT_CHECK_AVAILABLE_TARGET,
};

enum SpellsDefender
{
    SPELL_SUNDER_ARMOR   = 50370,
    SPELL_HAMSTRING      = 62845,
    SPELL_LIGHTNING_BOLT = 57780,
    SPELL_CREDIT_MARKER  = 65387
};

class npc_ulduar_steelforged_defender : public CreatureScript
{
    public:
        npc_ulduar_steelforged_defender() : CreatureScript("npc_ulduar_steelforged_defender") { }

        struct npc_ulduar_steelforged_defenderAI : public ScriptedAI
        {
            npc_ulduar_steelforged_defenderAI(Creature* creature) : ScriptedAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HAMSTRING, urand(5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CHECK_AVAILABLE_TARGET, 5*IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustDied(Unit* killer) override
            {
                DoCastVictim(SPELL_CREDIT_MARKER, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(14 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                            break;
                        case EVENT_HAMSTRING:
                            if (me->IsWithinMeleeRange(me->GetVictim()))
                                DoCastVictim(SPELL_HAMSTRING);
                            events.ScheduleEvent(EVENT_HAMSTRING, urand(9 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                        case EVENT_LIGHTNING_BOLT:
                            DoCastVictim(SPELL_LIGHTNING_BOLT);
                            events.ScheduleEvent(EVENT_LIGHTNING_BOLT, urand(3.4 * IN_MILLISECONDS, 4.7 * IN_MILLISECONDS));
                            break;
                        case EVENT_CHECK_AVAILABLE_TARGET:
                            if (!me->FindNearestPlayer(100.0f) && (!me->FindNearestCreature(NPC_SALVAGED_SIEGE_ENGINE, 100.0f, true) || !me->FindNearestCreature(NPC_CHOPPER, 100.0f, true) || !me->FindNearestCreature(NPC_SALVAGED_DEMOLISHER, 100.0f, true)))
                            {
                                _EnterEvadeMode();
                                me->GetMotionMaster()->MoveTargetedHome();
                            }
                            events.ScheduleEvent(EVENT_CHECK_AVAILABLE_TARGET, 5*IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ulduar_steelforged_defenderAI>(creature);
        }
};

enum EventsSentry
{
    EVENT_FLAMING_RUNE = 1,
    EVENT_LAVA_BURST,
    EVENT_RUNED_FLAME_JETS
};

enum SpellsSentry
{
    SPELL_FLAMING_RUNE        = 64852,
    SPELL_LAVA_BURST          = 64870,
    SPELL_LAVA_BURST_25       = 64991,
    SPELL_RUNED_FLAME_JETS    = 64847,
    SPELL_RUNED_FLAME_JETS_25 = 64988
};

class npc_ulduar_runeforged_sentry : public CreatureScript
{
    public:
        npc_ulduar_runeforged_sentry() : CreatureScript("npc_ulduar_runeforged_sentry") { }

        struct npc_ulduar_runeforged_sentryAI : public ScriptedAI
        {
            npc_ulduar_runeforged_sentryAI(Creature* creature) : ScriptedAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FLAMING_RUNE, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_LAVA_BURST, urand(3 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RUNED_FLAME_JETS, urand(5.5 * IN_MILLISECONDS, 6.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CHECK_AVAILABLE_TARGET, 5 * IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_FLAMING_RUNE:
                            DoCastVictim(SPELL_FLAMING_RUNE);
                            events.ScheduleEvent(EVENT_FLAMING_RUNE, urand(18 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                        case EVENT_LAVA_BURST:
                            DoCastVictim(RAID_MODE(SPELL_LAVA_BURST, SPELL_LAVA_BURST_25));
                            events.ScheduleEvent(EVENT_LAVA_BURST, urand(11 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                        case EVENT_RUNED_FLAME_JETS:
                            DoCastVictim(RAID_MODE(SPELL_RUNED_FLAME_JETS, SPELL_RUNED_FLAME_JETS_25));
                            events.ScheduleEvent(EVENT_RUNED_FLAME_JETS, urand(15 * IN_MILLISECONDS, 15.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_CHECK_AVAILABLE_TARGET:
                            if (!me->FindNearestPlayer(100.0f) && (!me->FindNearestCreature(NPC_SALVAGED_SIEGE_ENGINE, 100.0f, true) || !me->FindNearestCreature(NPC_CHOPPER, 100.0f, true) || !me->FindNearestCreature(NPC_SALVAGED_DEMOLISHER, 100.0f, true)))
                            {
                                _EnterEvadeMode();
                                me->GetMotionMaster()->MoveTargetedHome();
                            }
                            events.ScheduleEvent(EVENT_CHECK_AVAILABLE_TARGET, 5*IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ulduar_runeforged_sentryAI>(creature);
        }
};

enum EventsTank
{
    EVENT_FLAME_CANNON_TANK = 1
};

enum SpellsTank
{
    SPELL_JUMP              = 64953,
    SPELL_FLAME_CANNON_TANK = 64692
};

class npc_ulduar_mechagnome_battletank : public CreatureScript
{
    public:
        npc_ulduar_mechagnome_battletank() : CreatureScript("npc_ulduar_mechagnome_battletank") { }

        struct npc_ulduar_mechagnome_battletankAI : public ScriptedAI
        {
            npc_ulduar_mechagnome_battletankAI(Creature* creature) : ScriptedAI(creature) { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCastVictim(SPELL_JUMP);
                events.ScheduleEvent(EVENT_FLAME_CANNON_TANK, urand(4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CHECK_AVAILABLE_TARGET, 5*IN_MILLISECONDS);
            }

            void Reset() override
            {
                events.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 event = events.ExecuteEvent())
                {
                    switch (event)
                    {
                        case EVENT_FLAME_CANNON_TANK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                                DoCast(target, SPELL_FLAME_CANNON_TANK);
                            events.ScheduleEvent(EVENT_FLAME_CANNON_TANK, urand(12 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case EVENT_CHECK_AVAILABLE_TARGET:
                            if (!me->FindNearestPlayer(100.0f) && (!me->FindNearestCreature(NPC_SALVAGED_SIEGE_ENGINE, 100.0f, true) || !me->FindNearestCreature(NPC_CHOPPER, 100.0f, true) || !me->FindNearestCreature(NPC_SALVAGED_DEMOLISHER, 100.0f, true)))
                            {
                                _EnterEvadeMode();
                                me->GetMotionMaster()->MoveTargetedHome();
                            }
                            events.ScheduleEvent(EVENT_CHECK_AVAILABLE_TARGET, 5*IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ulduar_mechagnome_battletankAI>(creature);
        }
};

enum StormTemperedKeeperEnums
{
    EVENT_FORKED_LIGHTNING      = 1,
    EVENT_SUMMON_CHARGED_SPHERE,
    EVENT_CHECK_COMPANION,
    EVENT_UPDATE_MOVEMENT,

    SPELL_FORKED_LIGHTNING      = 63541,
    SPELL_SUMMON_CHARGED_SPHERE = 63527,
    SPELL_SEPARATION_ANXIETY    = 63539,
    SPELL_VENGEFUL_SURGE        = 63630,

    SPELL_SUPERCHARGED          = 63528,

    NPC_STORM_TEMPERED_KEEPER_1 = 33699,
    NPC_STORM_TEMPERED_KEEPER_2 = 33722,
    NPC_CHARGED_SPHERE          = 33715,

    SAY_VENGEFUL_SURGE          = 0,
};

class npc_storm_tempered_keeper : public CreatureScript
{
    public:
        npc_storm_tempered_keeper() : CreatureScript("npc_storm_tempered_keeper") { }

        struct npc_storm_tempered_keeperAI : public ScriptedAI
        {
            npc_storm_tempered_keeperAI(Creature* creature) : ScriptedAI(creature)
            {
                if (me->IsAlive())
                    me->m_Events.Schedule(1000, [this]() { FindCompanion(); });
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustReachedHome() override
            {
                if (!GetCompanion())
                    SpawnCompanion();
            }

            void JustDied(Unit* killer) override
            {
                if (Creature* companion = GetCompanion())
                {
                    companion->AI()->Talk(SAY_VENGEFUL_SURGE);
                    companion->CastSpell(companion, SPELL_VENGEFUL_SURGE, true);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!companionGUID)
                    FindCompanion();

                DoZoneInCombat(me, 100.0f);

                Creature* companion = GetCompanion();
                if (!companion)
                    companion = SpawnCompanion();

                if (companion && !companion->IsInCombat())
                    DoZoneInCombat(companion, 100.0f);

                events.ScheduleEvent(EVENT_FORKED_LIGHTNING, urand(5000, 7000));
                events.ScheduleEvent(EVENT_SUMMON_CHARGED_SPHERE, IsPrimary() ? 16000 : 16000 + 16000);
                events.ScheduleEvent(EVENT_CHECK_COMPANION, 2000);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_VENGEFUL_SURGE)
                    events.CancelEvent(EVENT_SUMMON_CHARGED_SPHERE);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_CHARGED_SPHERE)
                    if (Creature* companion = GetCompanion())
                        summon->AI()->SetGUID(companion->GetGUID());
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
                        case EVENT_FORKED_LIGHTNING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, SPELL_FORKED_LIGHTNING);
                            events.ScheduleEvent(EVENT_FORKED_LIGHTNING, urand(15000, 18000));
                            break;
                        case EVENT_SUMMON_CHARGED_SPHERE:
                            DoCast(me, SPELL_SUMMON_CHARGED_SPHERE);
                            events.ScheduleEvent(EVENT_SUMMON_CHARGED_SPHERE, 40000);
                            break;
                        case EVENT_CHECK_COMPANION:
                            if (companionGUID && !me->HasAura(SPELL_SEPARATION_ANXIETY))
                            {
                                Creature* companion = GetCompanion(false);
                                if (!companion || !me->IsWithinDistInMap(companion, 50.0f))
                                    DoCast(me, SPELL_SEPARATION_ANXIETY);
                            }
                            events.ScheduleEvent(EVENT_CHECK_COMPANION, 2000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            ObjectGuid companionGUID = ObjectGuid::Empty;

            bool IsPrimary() const { return me->GetEntry() == NPC_STORM_TEMPERED_KEEPER_1; }
            bool IsSecondary() const { return me->GetEntry() == NPC_STORM_TEMPERED_KEEPER_2; }
            uint32 GetCompanionEntry() const { return IsPrimary() ? NPC_STORM_TEMPERED_KEEPER_2 : NPC_STORM_TEMPERED_KEEPER_1; }

            Creature* SpawnCompanion()
            {
                // Find position from a nearby keeper that isn't summoned but is a static DB spawn
                std::list<Creature*> keepers;
                GetCreatureListWithEntryInGrid(keepers, me, GetCompanionEntry(), 500.0f);
                keepers.sort(Trinity::ObjectDistanceOrderPred(me));
                for (auto&& keeper : keepers)
                {
                    if (!keeper->IsSummon())
                    {
                        float x, y, z, o;
                        keeper->GetRespawnPosition(x, y, z, &o);
                        if (Creature* summon = me->SummonCreature(GetCompanionEntry(), x, y, z, o)) // Summon is blizzlike, they don't respawn in case of dying
                        {
                            companionGUID = summon->GetGUID();
                            return summon;
                        }
                        break;
                    }
                }

                return nullptr;
            }

            Creature* GetCompanion(bool alive = true)
            {
                if (companionGUID)
                    if (Creature* companion = ObjectAccessor::GetCreature(*me, companionGUID))
                        if (alive ? companion->IsAlive() : companion->getDeathState() != DEAD) // Either find alive companion or not despawned one
                            return companion;

                return nullptr;
            }

            void FindCompanion()
            {
                if (companionGUID)
                    return;

                std::list<Creature*> keepers;
                GetCreatureListWithEntryInGrid(keepers, me, GetCompanionEntry(), 30.0f);
                keepers.sort(Trinity::ObjectDistanceOrderPred(me));
                for (auto&& keeper : keepers)
                {
                    if (keeper->IsAlive())
                    {
                        companionGUID = keeper->GetGUID();
                        return;
                    }
                }

                SpawnCompanion();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_storm_tempered_keeperAI>(creature);
        }
};

class npc_charged_sphere : public CreatureScript
{
    public:
        npc_charged_sphere() : CreatureScript("npc_charged_sphere") { }

        struct npc_charged_sphereAI : public ScriptedAI
        {
            npc_charged_sphereAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                targetPosition.Relocate(0.0f, 0.0f, 0.0f);
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                targetGUID = guid;
                events.ScheduleEvent(EVENT_UPDATE_MOVEMENT, 1);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_UPDATE_MOVEMENT)
                {
                    if (Creature* target = ObjectAccessor::GetCreature(*me, targetGUID))
                    {
                        if (target->IsAlive())
                        {
                            if (me->GetExactDist2d(target) < 2.0f)
                            {
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveIdle();
                                DoCastAOE(SPELL_SUPERCHARGED, true);
                                me->DespawnOrUnsummon(1500);
                            }
                            else
                            {
                                if (target->GetExactDist2d(&targetPosition) > 0.1f)
                                {
                                    targetPosition.Relocate(target);
                                    targetPosition.m_positionZ += 3.0f;
                                    me->GetMotionMaster()->MovePoint(0, targetPosition);
                                }
                                events.ScheduleEvent(EVENT_UPDATE_MOVEMENT, 1000);
                            }
                            return;
                        }
                    }

                    // If target wasn't found or isn't alive
                    me->DespawnOrUnsummon();
                }
            }

        private:
            EventMap events;
            ObjectGuid targetGUID = ObjectGuid::Empty;
            Position targetPosition;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_charged_sphereAI>(creature);
        }
};

class spell_summon_charged_sphere : public SpellScriptLoader
{
    public:
        spell_summon_charged_sphere() : SpellScriptLoader("spell_summon_charged_sphere") { }

        class spell_summon_charged_sphere_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_charged_sphere_SpellScript);

            void SelectDest(SpellDestination& dest)
            {
                dest._position.m_positionZ += 3.0f;
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_summon_charged_sphere_SpellScript::SelectDest, EFFECT_0, TARGET_DEST_CASTER);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_charged_sphere_SpellScript();
        }
};

enum ArachnopodEnums
{
    SPELL_MACHINE_GUN           = 64776,
    SPELL_FLAME_SPRAY           = 64717,
    SPELL_CHARGED_LEAP          = 64822,
    SPELL_CHARGED_LEAP_JUMP     = 64788,
    SPELL_ICE_TURRET            = 64966,
    SPELL_ICE_NOVA              = 66347,
    SPELL_VEHICLE_DAMAGED       = 63415,
    SPELL_EJECT_ALL_PASSENGERS  = 50630,
    SPELL_DAMAGED               = 64770,
    SPELL_TRIGGER_000           = 36294,

    FACTION_ARACHNOPOD_FRIENDLY = 1665,

    EVENT_MACHINE_GUN = 1,
    EVENT_FLAME_SPRAY,
    EVENT_CHARGED_LEAP,
    EVENT_ICE_TURRET,
};

class npc_arachnopod_destroyer : public CreatureScript
{
    public:
        npc_arachnopod_destroyer() : CreatureScript("npc_arachnopod_destroyer") { }

        struct npc_arachnopod_destroyerAI : public VehicleAI
        {
            npc_arachnopod_destroyerAI(Creature* creature) : VehicleAI(creature) { }

            void Reset() override
            {
                VehicleAI::Reset();
                events.Reset();
                events.ScheduleEvent(EVENT_MACHINE_GUN, urand(3500, 10000));
                events.ScheduleEvent(EVENT_FLAME_SPRAY, urand(18000, 32000));
                events.ScheduleEvent(EVENT_CHARGED_LEAP, urand(22000, 27000));
            }

            void JustAppeared() override
            {
                damaged = false;
                me->setRegeneratingHealth(true);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (damaged)
                    VehicleAI::MoveInLineOfSight(who);
                else
                    CreatureAI::MoveInLineOfSight(who);
            }

            void AttackStart(Unit* victim) override
            {
                if (damaged)
                    VehicleAI::AttackStart(victim);
                else
                    CreatureAI::AttackStart(victim);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!damaged)
                    DoCast(who, SPELL_MACHINE_GUN);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_TRIGGER_000)
                    me->Kill(me);
            }

            void UpdateAI(uint32 diff) override
            {
                VehicleAI::UpdateAI(diff);

                if (damaged)
                {
                    // Can be reset to default on exit vehicle
                    me->SetFaction(FACTION_ARACHNOPOD_FRIENDLY);
                    me->SetReactState(REACT_PASSIVE);
                }

                if (damaged || !UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(20) && !damaged)
                {
                    damaged = true;
                    me->SetHomePosition(*me);
                    EnterEvadeMode();
                    DoCast(me, SPELL_VEHICLE_DAMAGED, true);
                    DoCast(me, SPELL_EJECT_ALL_PASSENGERS, true);
                    DoCast(me, SPELL_DAMAGED, true);
                    me->setRegeneratingHealth(false);
                    me->SetFaction(FACTION_ARACHNOPOD_FRIENDLY);
                    me->SetReactState(REACT_PASSIVE);
                    me->CombatStop(true);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MACHINE_GUN:
                            DoCastVictim(SPELL_MACHINE_GUN);
                            events.ScheduleEvent(EVENT_MACHINE_GUN, urand(3500, 10000));
                            break;
                        case EVENT_FLAME_SPRAY:
                            DoCastVictim(SPELL_FLAME_SPRAY);
                            events.ScheduleEvent(EVENT_FLAME_SPRAY, urand(18000, 32000));
                            break;
                        case EVENT_CHARGED_LEAP:
                            DoCastAOE(SPELL_CHARGED_LEAP, true);
                            events.ScheduleEvent(EVENT_CHARGED_LEAP, urand(22000, 27000));
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }
            }

        private:
            EventMap events;
            bool damaged = false;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_arachnopod_destroyerAI>(creature);
        }
};

class npc_clockwork_mechanic : public CreatureScript
{
    public:
        npc_clockwork_mechanic() : CreatureScript("npc_clockwork_mechanic") { }

        struct npc_clockwork_mechanicAI : public ScriptedAI
        {
            npc_clockwork_mechanicAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ICE_TURRET, 1000);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                if (me->GetVehicle())
                    ScriptedAI::EnterEvadeMode();
                else
                    me->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* who) override
            {
                if (Creature* vehicle = me->GetVehicleCreatureBase())
                    vehicle->AI()->AttackStart(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetVehicle())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ICE_TURRET:
                            DoCast(me, SPELL_ICE_TURRET);
                            events.ScheduleEvent(EVENT_ICE_TURRET, 13300);
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_clockwork_mechanicAI>(creature);
        }
};

class npc_ice_turret : public CreatureScript
{
    public:
        npc_ice_turret() : CreatureScript("npc_ice_turret") { }

        struct npc_ice_turretAI : public NullCreatureAI
        {
            npc_ice_turretAI(Creature* creature) : NullCreatureAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, SPELL_ICE_NOVA);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ice_turretAI>(creature);
        }
};

// 64822 - Charged Leap
class spell_charged_leap : public SpellScriptLoader
{
    public:
        spell_charged_leap() : SpellScriptLoader("spell_charged_leap") { }

        class spell_charged_leap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_charged_leap_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                targets.remove_if([caster](WorldObject* target)
                {
                    if (Unit* unit = target->ToUnit())
                        return caster->GetVictim() == unit || caster->IsWithinCombatRange(unit, 10.0f) || !caster->IsWithinCombatRange(unit, 80.0f);
                    return true;
                });
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        caster->CastSpell(target, SPELL_CHARGED_LEAP_JUMP);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_charged_leap_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_charged_leap_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_charged_leap_SpellScript();
        }
};

class at_ulduar_event_areatrigger : public AreaTriggerScript
{
    public:
        at_ulduar_event_areatrigger() : AreaTriggerScript("at_ulduar_event_areatrigger") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                instance->SetData(DATA_AREATRIGGER_EVENT, trigger->ID);

            return true;
        }
};

void AddSC_ulduar_scripts()
{
    new npc_ironwork_cannon();
    new npc_ulduar_warmage();
    new npc_brann_bronzebeard();
    new npc_ulduar_engineer(); // event end
    new npc_ulduar_lorekeeper();
    new npc_high_explorer_dellorah();
    new npc_ulduar_steelforged_defender();
    new npc_ulduar_runeforged_sentry();
    new npc_ulduar_mechagnome_battletank();
    new npc_storm_tempered_keeper();
    new npc_charged_sphere();
    new spell_summon_charged_sphere();
    new npc_arachnopod_destroyer();
    new npc_clockwork_mechanic();
    new npc_ice_turret();
    new spell_charged_leap();
    new at_ulduar_event_areatrigger();
}
