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
SDName: Zulfarrak
SD%Complete: 50
SDComment: Consider it temporary, no instance script made for this instance yet.
SDCategory: Zul'Farrak
EndScriptData */

/* ContentData
npc_sergeant_bly
npc_weegli_blastfuse
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "zulfarrak.h"
#include "Player.h"

/*######
## npc_sergeant_bly
######*/

enum blyAndCrewFactions
{
    FACTION_HOSTILE           = 14,
    FACTION_FRIENDLY          = 35,  //while in cages (so the trolls won't attack them while they're caged)
    FACTION_FREED             = 250  //after release (so they'll be hostile towards trolls)
};

enum blySays
{
    SAY_1 = 0,
    SAY_2 = 1
};

enum blySpells
{
    SPELL_SHIELD_BASH          = 11972,
    SPELL_REVENGE              = 12170
};

enum eActions
{
    ACTION_INTRO = 0,
};

#define GOSSIP_BLY                  "[PH] In that case, I will take my reward!"

class npc_sergeant_bly : public CreatureScript
{
    public:
        npc_sergeant_bly() : CreatureScript("npc_sergeant_bly") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->CLOSE_GOSSIP_MENU();
                CAST_AI(npc_sergeant_bly::npc_sergeant_blyAI, creature->AI())->PlayerGUID = player->GetGUID();
                creature->AI()->DoAction(0);
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->GetData(EVENT_PYRAMID) == PYRAMID_KILLED_ALL_TROLLS)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BLY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                    player->SEND_GOSSIP_MENU(1517, creature->GetGUID());
                }
                else
                    if (instance->GetData(EVENT_PYRAMID) == PYRAMID_NOT_STARTED)
                        player->SEND_GOSSIP_MENU(1515, creature->GetGUID());
                    else
                        player->SEND_GOSSIP_MENU(1516, creature->GetGUID());
                return true;
            }
            return false;
        }

        struct npc_sergeant_blyAI : public ScriptedAI
        {
            npc_sergeant_blyAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                postGossipStep = 0;
            }

            InstanceScript* instance;

            uint32 postGossipStep;
            uint32 Text_Timer;
            uint32 ShieldBash_Timer;
            uint32 Revenge_Timer;                                   //this is wrong, spell should never be used unless me->GetVictim() dodge, parry or block attack. Trinity support required.
            ObjectGuid PlayerGUID;

            void Reset() override
            {
                ShieldBash_Timer = 5000;
                Revenge_Timer = 8000;

                me->SetFaction(FACTION_FRIENDLY);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker->GetTypeId() != TYPEID_PLAYER && HealthBelowPct(40))
                    damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (postGossipStep > 0 && postGossipStep < 4)
                {
                    if (Text_Timer < diff)
                    {
                        switch (postGossipStep)
                        {
                        case 1:
                            //weegli doesn't fight - he goes & blows up the door
                            if (Creature* pWeegli = instance->instance->GetCreature(instance->GetGuidData(NPC_WEEGLI)))
                                pWeegli->AI()->DoAction(0);
                            Talk(SAY_1);
                            Text_Timer = 5000;
                            break;
                        case 2:
                            Talk(SAY_2);
                            Text_Timer = 5000;
                            break;
                        case 3:
                            me->SetFaction(FACTION_HOSTILE);
                            if (Player* target = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                                AttackStart(target);

                            if (instance)
                            {
                                switchFactionIfAlive(instance, NPC_RAVEN);
                                switchFactionIfAlive(instance, NPC_ORO);
                                switchFactionIfAlive(instance, NPC_MURTA);
                            }
                        }
                        postGossipStep++;
                    }
                    else Text_Timer -= diff;
                }

                if (!UpdateVictim())
                    return;

                if (ShieldBash_Timer <= diff)
                {
                    DoCastVictim(SPELL_SHIELD_BASH);
                    ShieldBash_Timer = 15000;
                }
                else
                    ShieldBash_Timer -= diff;

                if (Revenge_Timer <= diff)
                {
                    DoCastVictim(SPELL_REVENGE);
                    Revenge_Timer = 10000;
                }
                else
                    Revenge_Timer -= diff;

                DoMeleeAttackIfReady();
            }

            void DoAction(int32 /*param*/) override
            {
                postGossipStep = 1;
                Text_Timer = 0;
            }

            void switchFactionIfAlive(InstanceScript* instance, uint32 entry)
            {
                if (Creature* crew = instance->instance->GetCreature(instance->GetGuidData(entry)))
                    if (crew->IsAlive())
                        crew->SetFaction(FACTION_HOSTILE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sergeant_blyAI(creature);
        }
};

/*######
## npc_weegli_blastfuse
######*/

enum weegliSpells
{
    SPELL_BOMB                  = 8858,
    SPELL_GOBLIN_LAND_MINE      = 21688,
    SPELL_SHOOT                 = 6660,
    SPELL_WEEGLIS_BARREL        = 10772
};

enum weegliSays
{
    SAY_WEEGLI_OHNO             = 0,
    SAY_WEEGLI_OK_I_GO          = 1
};

#define GOSSIP_WEEGLI               "[PH] Please blow up the door."

class npc_weegli_blastfuse : public CreatureScript
{
    public:
        npc_weegli_blastfuse() : CreatureScript("npc_weegli_blastfuse") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->CLOSE_GOSSIP_MENU();
                //here we make him run to door, set the charge and run away off to nowhere
                creature->AI()->DoAction(0);
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                switch (instance->GetData(EVENT_PYRAMID))
                {
                    case PYRAMID_KILLED_ALL_TROLLS:
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WEEGLI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                        player->SEND_GOSSIP_MENU(1514, creature->GetGUID());  //if event can proceed to end
                        break;
                    case PYRAMID_NOT_STARTED:
                        player->SEND_GOSSIP_MENU(1511, creature->GetGUID());  //if event not started
                        break;
                    default:
                        player->SEND_GOSSIP_MENU(1513, creature->GetGUID());  //if event are in progress
                }
                return true;
            }
            return false;
        }

        struct npc_weegli_blastfuseAI : public ScriptedAI
        {
            npc_weegli_blastfuseAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                destroyingDoor = false;
                Bomb_Timer = 10000;
                LandMine_Timer = 30000;
            }

            uint32 Bomb_Timer;
            uint32 LandMine_Timer;
            bool destroyingDoor;
            InstanceScript* instance;

            void Reset() override
            {
                /*if (instance)
                    instance->SetData(0, NOT_STARTED);*/
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker->GetTypeId() != TYPEID_PLAYER && HealthBelowPct(40))
                    damage = 0;
            }

            void AttackStart(Unit* victim) override
            {
                AttackStartCaster(victim, 10);//keep back & toss bombs/shoot
            }

            void JustDied(Unit* /*killer*/) override
            {
                /*if (instance)
                    instance->SetData(0, DONE);*/
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (Bomb_Timer < diff)
                {
                    DoCastVictim(SPELL_BOMB);
                    Bomb_Timer = 10000;
                }
                else
                    Bomb_Timer -= diff;

                if (me->isAttackReady() && !me->IsWithinMeleeRange(me->GetVictim()))
                {
                    DoCastVictim(SPELL_SHOOT);
                    me->SetSheath(SHEATH_STATE_RANGED);
                }
                else
                {
                    me->SetSheath(SHEATH_STATE_MELEE);
                    DoMeleeAttackIfReady();
                }
            }

            void MovementInform(uint32 /*type*/, uint32 /*id*/) override
            {
                if (instance)
                {
                    if (instance->GetData(EVENT_PYRAMID) == PYRAMID_CAGES_OPEN)
                    {
                        instance->SetData(EVENT_PYRAMID, PYRAMID_ARRIVED_AT_STAIR);
                        Talk(SAY_WEEGLI_OHNO);
                        me->SetHomePosition(1882.69f, 1272.28f, 41.87f, 0);
                    }
                    else
                        if (destroyingDoor)
                        {
                            instance->DoUseDoorOrButton(instance->GetGuidData(GO_END_DOOR));
                            /// @todo leave the area...
                            me->DespawnOrUnsummon();
                        };
                }
            }

            void DoAction(int32 /*param*/) override
            {
                DestroyDoor();
            }

            void DestroyDoor()
            {
                if (me->IsAlive())
                {
                    me->SetFaction(FACTION_FRIENDLY);
                    me->GetMotionMaster()->MovePoint(0, 1858.57f, 1146.35f, 14.745f);
                    me->SetHomePosition(1858.57f, 1146.35f, 14.745f, 3.85f); // in case he gets interrupted
                    Talk(SAY_WEEGLI_OK_I_GO);
                    destroyingDoor = true;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_weegli_blastfuseAI(creature);
        }
};

/*######
## npc_sandfury_executioner 7274
######*/

class npc_sandfury_executioner : public CreatureScript
{
    public:
        npc_sandfury_executioner() : CreatureScript("npc_sandfury_executioner") { }

        enum iSpells
        {
            SPELL_CLEAVE = 15496,
        };

        enum iTalks
        {
            TALK_AGGRO = 0,
            TALK_SLAY  = 1,
        };

        enum iEvents
        {
            EVENT_CLEAVE = 1,
        };

        struct npc_sandfury_executionerAI : public ScriptedAI
        {
            npc_sandfury_executionerAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_AGGRO);
                events.ScheduleEvent(EVENT_CLEAVE, 8 * IN_MILLISECONDS);
            }

            void initBlyCrewMember(InstanceScript* instance, uint32 entry, float x, float y, float z)
            {
                if (Creature* crew = ObjectAccessor::GetCreature(*me, instance->GetGuidData(entry)))
                {
                    crew->SetReactState(REACT_AGGRESSIVE);
                    crew->SetWalk(true);
                    crew->SetHomePosition(x, y, z, 0);
                    crew->GetMotionMaster()->MovePoint(1, x, y, z);
                    crew->SetFaction(FACTION_FREED);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    instance->SetData(EVENT_PYRAMID, PYRAMID_CAGES_OPEN);
                    //set bly & co to aggressive & start moving to top of stairs
                    initBlyCrewMember(instance, NPC_BLY, 1884.99f, 1263, 41.52f);
                    initBlyCrewMember(instance, NPC_RAVEN, 1882.5f, 1263, 41.52f);
                    initBlyCrewMember(instance, NPC_ORO, 1886.47f, 1270.68f, 41.68f);
                    initBlyCrewMember(instance, NPC_WEEGLI, 1890, 1263, 41.52f);
                    initBlyCrewMember(instance, NPC_MURTA, 1891.19f, 1272.03f, 41.60f);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_CLEAVE)
                    {
                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_CLEAVE);

                        events.ScheduleEvent(EVENT_CLEAVE, urand(13 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sandfury_executionerAI(creature);
        }
};

/*######
## npc_gahzrilla 7273
######*/

class npc_gahzrilla : public CreatureScript
{
    public:
        npc_gahzrilla() : CreatureScript("npc_gahzrilla") { }

        enum iSpells
        {
            SPELL_FREEZE_SOLID   = 11836,
            SPELL_GAHZRILLA_SLAM = 11902,
            SPELL_ICICLE         = 11131,
        };

        enum iEvents
        {
            EVENT_SOLID  = 1,
            EVENT_SLAM   = 2,
            EVENT_ICICLE = 3,
            EVENT_INIT   = 4,
        };

        struct npc_gahzrillaAI : public ScriptedAI
        {
            npc_gahzrillaAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events, nonCombatEvents;

            void InitializeAI() override
            {
                me->setActive(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SOLID, urand(10 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SLAM, urand(16 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICICLE, urand(3 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTRO)
                {
                    Movement::MoveSplineInit init(me);
                    for (auto itr : GahzrillaPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();
                    nonCombatEvents.ScheduleEvent(EVENT_INIT, me->GetSplineDuration());
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_INIT)
                    {
                        me->SetHomePosition(*me);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
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
                        case EVENT_SLAM:
                            DoCastAOE(SPELL_GAHZRILLA_SLAM);
                            events.ScheduleEvent(EVENT_SLAM, urand(28 * IN_MILLISECONDS, 31 * IN_MILLISECONDS));
                            break;
                        case EVENT_ICICLE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_ICICLE);

                            events.ScheduleEvent(EVENT_ICICLE, urand(3 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            break;
                        case EVENT_SOLID:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_FREEZE_SOLID);
                            else if (Unit* target=me->GetVictim())
                                DoCast(target, SPELL_FREEZE_SOLID);

                            events.ScheduleEvent(EVENT_SOLID, urand(21 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gahzrillaAI(creature);
        }
};

enum ShallowGrave
{
    NPC_ZOMBIE          = 7286,
    NPC_DEAD_HERO       = 7276,
    CHANCE_ZOMBIE       = 65,
    CHANCE_DEAD_HERO    = 10
};

class go_shallow_grave : public GameObjectScript
{
    public:
        go_shallow_grave() : GameObjectScript("go_shallow_grave") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            // randomly summon a zombie or dead hero the first time a grave is used
            if (go->GetUseCount() == 0)
            {
                uint32 randomchance = urand(0, 100);
                if (randomchance < CHANCE_ZOMBIE)
                    go->SummonCreature(NPC_ZOMBIE, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                else
                    if ((randomchance - CHANCE_ZOMBIE) < CHANCE_DEAD_HERO)
                        go->SummonCreature(NPC_DEAD_HERO, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
            }
            go->AddUse();
            return false;
        }
};

/*######
## go_gong_of_zulfarrak 141832
######*/
class go_gong_of_zulfarrak : public GameObjectScript
{
    public:
        go_gong_of_zulfarrak() : GameObjectScript("go_gong_of_zulfarrak") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

            if (TempSummon* Gahzrilla = go->SummonCreature(NPC_GAHZRILLA, GahzrillaPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                Gahzrilla->AI()->DoAction(ACTION_INTRO);

            return false;
        }
};

/*######
## at_zumrah
######*/

enum zumrahConsts
{
    ZUMRAH_ID = 7271,
    ZUMRAH_HOSTILE_FACTION = 37
};

class at_zumrah : public AreaTriggerScript
{
    public:
        at_zumrah() : AreaTriggerScript("at_zumrah") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/) override
        {
            Creature* pZumrah = player->FindNearestCreature(ZUMRAH_ID, 30.0f);

            if (!pZumrah)
                return false;

            pZumrah->SetFaction(ZUMRAH_HOSTILE_FACTION);
            return true;
        }
};

void AddSC_zulfarrak()
{
    new npc_sergeant_bly();
    new npc_weegli_blastfuse();
    new npc_sandfury_executioner();
    new npc_gahzrilla();
    new go_shallow_grave();
    new go_gong_of_zulfarrak();
    new at_zumrah();
}
