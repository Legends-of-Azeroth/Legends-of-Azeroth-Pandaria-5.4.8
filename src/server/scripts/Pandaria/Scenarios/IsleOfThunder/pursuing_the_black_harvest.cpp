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

#include "pursuing_the_black_harvest.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "LFGMgr.h"

enum Spells
{
    SPELL_SEARCHING_FOR_INTRUDERS  = 134110,
    SPELL_MULTI_SHOT               = 41187,
    SPELL_NETTED                   = 134111,
    SPELL_HEX                      = 134202,
    SPELL_LIGHTNING                = 42024,
    SPELL_BLACKOUT                 = 134112,
    SPELL_SOULRIFT                 = 134208,
    SPELL_SAP                      = 134205,
    SPELL_DEMONIC_GATEWAY_EFF      = 138649,
    SPELL_PLACE_EMPOWERED_SOULCORE = 138680,
    SPELL_SONIC_STRIKE             = 41168,
    SPELL_CLEAVE                   = 15284,
    SPELL_SHADOW_INFERNO           = 39645,
    SPELL_FEAR                     = 41150,
    SPELL_DARK_MENDING             = 16588,
    SPELL_SHADOW_BOLT              = 34344,
    SPELL_SHADOW_SHOCK             = 16583,
    SPELL_MELT_FLESH               = 37629,
    SPELL_FIREBOLT                 = 134245,
    SPELL_SOUL_BLAST               = 50992,
    SPELL_SHADOW_BOLT_2            = 12739,
    SPELL_ETERNAL_BANISHMENT       = 139186,
    SPELL_PURGE_XERATTH            = 139366,
    SPELL_CODEX_OF_XERATTH         = 101508,
    SPELL_SPELLFLAME               = 134234,
    SPELL_SPELLFLAME_EFF           = 134235,
    SPELL_HELLFIRE                 = 134225,

    // Kanrethad
    SPELL_SUMMON_PIT_LORD          = 138789,
    SPELL_DEMONIC_GRASP            = 139142,
    SPELL_AURA_OF_OMNIPOTENCE      = 138563,
    SPELL_CATACLYSM                = 138564,
    SPELL_EX_AGONY                 = 138560,
    SPELL_RAIN_OF_FIRE             = 138561,
    SPELL_BACKFIRE                 = 138619,
    SPELL_SUMMON_WILD_IMPS         = 138685,
    SPELL_BURNING_EMBERS           = 138557,
    SPELL_SOUL_SHARDS              = 138556,
    SPELL_CHAOS_BOLT               = 138559,
    SPELL_ANNIHILATE_DEMONS        = 139141,
    SPELL_SEED_OF_DESTRUCTION      = 138587,
    SPELL_CURSE_OF_ULTIMATE_DOON   = 138558,
    SPELL_SUMMON_FELHUNTERS        = 138751,
    SPELL_SOUL_FIRE                = 138554,

    // Green Fire
    SPELL_DRAIN_ENERGY             = 139200,
    SPELL_FEEL_ENERGY_SPIN         = 140164,
    SPELL_FEEL_ENERGY_GREEN_LIGHT  = 140136,
    SPELL_FEEL_ENERGY_GOLDEN_ORB   = 140137,
    SPELL_FEEL_ENERGY_GREEN_BALL   = 140160,
    SPELL_FEEL_ENERGY_BIG_BALL     = 140161,
    SPELL_FEEL_ENERGY_EXPLOSION    = 140163,
    SPELL_FEEL_ENERGY_VEHICLE      = 140116,
};

enum Events
{
    EVENT_MULTI_SHOT,
    EVENT_HEX,
    EVENT_LIGHTNING,
    EVENT_SONIC_STRIKE,
    EVENT_CLEAVE,
    EVENT_SHADOW_INFERNO,
    EVENT_FEAR,
    EVENT_DARK_MENDING,
    EVENT_SHADOW_BOLT,
    EVENT_SHADOW_SHOCK,
    EVENT_MELT_FLESH,
    EVENT_FIREBOLT,
    EVENT_SOUL_BLAST,
    EVENT_SPELLFLAME,
    EVENT_HELLFIRE,

    // Affliction
    EVENT_EX_AGONY,
    EVENT_BACKFIRE,
    EVENT_SEED_OF_DESTRUCTION,
    EVENT_CURSE_OF_ULTIMATE_DOOM,
    EVENT_AURA_OF_OMNIPOTENCE,

    // Destruction
    EVENT_CATACLYSM,
    EVENT_RAIN_OF_FIRE,
    EVENT_CHAOS_BOLT,
    EVENT_SOUL_FIRE,
    EVENT_SUMMON,
};

enum Phases
{
    PHASE_DESTRUCTION,
    PHASE_AFFLICTION,
};

// Akama 68137
class npc_black_harvest_akama : public CreatureScript
{
    public:
        npc_black_harvest_akama() : CreatureScript("npc_black_harvest_akama") { }
    
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
    
            if (action == GOSSIP_ACTION_INFO_DEF + 1 && creature->GetInstanceScript())
                creature->GetInstanceScript()->SetData(STEP_SEARCH_FOR_SIGNS, DONE);
    
            player->CLOSE_GOSSIP_MENU();
    
            return true;
        }
    
        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());
    
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show me a path!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_black_harvest_akamaAI : public ScriptedAI
        {
            npc_black_harvest_akamaAI(Creature* creature) : ScriptedAI(creature) { }
    
            TaskScheduler scheduler;
            InstanceScript* instance;
            bool hasTriggered;
            uint32 delay;
            float x, y;
    
            void Reset() override
            {
                me->SetSpeed(MOVE_RUN, 1.8f);
                me->SetVisible(false);
                hasTriggered = false;
                delay = 0;
                x = 0.0f; y = 0.0f;
                instance = me->GetInstanceScript();
            }
    
            void MoveInLineOfSight(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER && !hasTriggered)
                {
                    hasTriggered = true;
    
                    if (instance)
                        instance->SetData(STEP_ENTER_THE_BLACK_TEMPLE, DONE);
    
                    // Move to Sap Position
                    Movement::MoveSplineInit init(me);
    
                    for (auto&& itr : akamaScrollPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));
    
                    init.SetUncompressed();
                    init.Launch();
                }

                if (who->GetTypeId() == TYPEID_PLAYER && instance && instance->GetData(STEP_DEFEAT_ESSENCE_OF_ORDER) == DONE && instance->GetData(STEP_ESCAPE_SHRINE_OF_LOST_SOULS) != DONE)
                {
                    me->StopMoving();
                    Talk(TALK_SPECIAL_8);
                    instance->SetData(STEP_ESCAPE_SHRINE_OF_LOST_SOULS, DONE);

                    // Move Away
                    Movement::MoveSplineInit init(me);

                    for (auto&& itr : akamaLastPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.SetUncompressed();
                    init.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        me->DespawnOrUnsummon();
                    });
                }
            }
    
            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                            me->GetMotionMaster()->MovePoint(0, target->GetPositionX() + frand(-2.5, 2.5f), target->GetPositionY() + frand(-2.0f, 2.0f), target->GetPositionZ());

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                        {
                            if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                                DoCast(target, SPELL_SAP, true);

                            delay = 4 * IN_MILLISECONDS;

                            scheduler
                                .Schedule(Milliseconds(delay), [this](TaskContext context)
                            {
                                if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                                    me->SetFacingToObject(target);

                                me->SetVisible(true);
                                Talk(TALK_INTRO);
                            });

                            scheduler
                                .Schedule(Milliseconds(delay += 4 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                Talk(TALK_SPECIAL_1);
                            });

                            scheduler
                                .Schedule(Milliseconds(delay += 5 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                                    target->CastSpell(target, SPELL_TRUSTED_BY_ASHTONGUE, true);

                                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            });
                        });
                        break;
                    case ACTION_MOVE_TO_SANCTUM:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Talk(TALK_SPECIAL_2);

                        scheduler
                            .Schedule(Milliseconds(8200), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_3);

                            // Move to Sanctum Position
                            Movement::MoveSplineInit init(me);

                            for (auto&& itr : akamaSanctumPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetUncompressed();
                            init.SetWalk(true);
                            init.Launch();

                            scheduler
                                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                            {
                                me->StopMoving();

                                if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                                    Talk(TALK_SPECIAL_4, target);

                                if (Creature* shaman = me->FindNearestCreature(NPC_ASHTONGUE_SHAMAN, 50.0f, true))
                                {
                                    GetPositionWithDistInOrientation(shaman, 4.5f, Position::NormalizeOrientation(shaman->GetOrientation() + M_PI / 2), x, y);
                                    shaman->GetMotionMaster()->MovePoint(0, x, y, shaman->GetPositionZ());
                                }

                                scheduler
                                    .Schedule(Milliseconds(6500), [this](TaskContext context)
                                {
                                    // Move to Sanctum Inner
                                    Movement::MoveSplineInit init(me);

                                    for (auto&& itr : akamaPostSanctumPath)
                                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                                    init.SetUncompressed();
                                    init.SetWalk(true);
                                    init.Launch();

                                    delay = 0;

                                    scheduler
                                        .Schedule(Milliseconds((int32)(0.3f * me->GetSplineDuration())), [this](TaskContext context)
                                    {
                                        Talk(TALK_SPECIAL_5);
                                    });

                                    scheduler
                                        .Schedule(Milliseconds(delay += me->GetSplineDuration()), [this](TaskContext context)
                                    {
                                        if (instance)
                                            instance->SetData(STEP_FOLLOW_AKAMA, DONE);

                                        Talk(TALK_SPECIAL_6);
                                    });

                                    scheduler
                                        .Schedule(Milliseconds(delay += 6800), [this](TaskContext context)
                                    {
                                        Talk(TALK_SPECIAL_7);
                                    });
                                });
                            });
                        });
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
            return new npc_black_harvest_akamaAI(creature);
        }
};

// Essence of Order 68151
struct npc_black_harvest_essence_of_order : public customCreatureAI
{
    npc_black_harvest_essence_of_order(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    bool hasTriggered;
    float faceOri;
    float dist;
    float x, y;

    void InitializeAI() override
    {
        hasTriggered = false;
        faceOri = 0.0f;
        dist = 4.0f;
        x = 0.0f; y = 0.0f;
        Reset();
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_SPECIAL_1);
        events.ScheduleEvent(EVENT_SPELLFLAME, 7 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HELLFIRE, 20 * IN_MILLISECONDS);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && !hasTriggered && me->GetInstanceScript() && me->GetInstanceScript()->GetData(STEP_FOLLOW_AKAMA) == DONE)
        {
            hasTriggered = true;

            if (Player* target = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                target->CastSpell(target, SPELL_MEMORY_OF_THE_RELICVARY, true);
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            
                Talk(TALK_INTRO);
                break;
            case ACTION_INIT_FLAME:
                dist = 4.0f;
                faceOri = me->GetOrientation();

                scheduler
                    .SetValidator([this] { return dist <= 40.0f; })
                    .Schedule(Milliseconds(250), [this](TaskContext context)
                {
                    GetPositionWithDistInOrientation(me, dist, faceOri, x, y);
                    me->CastSpell(x, y, me->GetPositionZ(), SPELL_SPELLFLAME_EFF, true);
                    dist += 4.0f;
                    context.Repeat(Milliseconds(250));
                });
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(TALK_SPECIAL_2);

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(STEP_DEFEAT_ESSENCE_OF_ORDER, DONE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);
        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_HELLFIRE, 30 * IN_MILLISECONDS, EVENT_HELLFIRE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_SPELLFLAME, urand(10.5 * IN_MILLISECONDS, 14 * IN_MILLISECONDS), EVENT_SPELLFLAME, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct demon_baseAI : public customCreatureAI
{
    demon_baseAI(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && me->GetEntry() == NPC_UNBOUND_SHIVARRA &&
            me->GetInstanceScript() && me->GetInstanceScript()->GetData(STEP_ESCAPE_SHRINE_OF_LOST_SOULS) == DONE && me->GetInstanceScript()->GetData(STEP_PLUNDER_THE_DEN_OF_MORTAL_DELIGHTS) != DONE)
            me->GetInstanceScript()->SetData(STEP_PLUNDER_THE_DEN_OF_MORTAL_DELIGHTS, DONE);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(65) && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetVisible(true);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        }
    }
};

// Unbound Centurion 68176
struct npc_black_harvest_unbound_centurion : public demon_baseAI
{
    npc_black_harvest_unbound_centurion(Creature* creature) : demon_baseAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SONIC_STRIKE, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CLEAVE, urand(1.5 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_SONIC_STRIKE, 10 * IN_MILLISECONDS, EVENT_SONIC_STRIKE, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_CLEAVE, urand(3.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS), EVENT_CLEAVE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Unbound Nightlord 68174
struct npc_black_harvest_unbound_nightlord : public demon_baseAI
{
    npc_black_harvest_unbound_nightlord(Creature* creature) : demon_baseAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHADOW_INFERNO, 9.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FEAR, urand(5.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_SHADOW_INFERNO, 11 * IN_MILLISECONDS, EVENT_SHADOW_INFERNO, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_FEAR, urand(5.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_FEAR, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Unbound Bonemender 68175
struct npc_black_harvest_unbound_bonemender : public demon_baseAI
{
    npc_black_harvest_unbound_bonemender(Creature* creature) : demon_baseAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DARK_MENDING, 9.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(1.5 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS));
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_UNBOUND_BONEMENDER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_UNBOUND_CENTURION, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_UNBOUND_NIGHTLORD, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_UNBOUND_SHIVARRA, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_UNBOUND_SUCCUBUS, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FREED_IMP, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
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
            ExecuteTargetEvent(SPELL_DARK_MENDING, 11 * IN_MILLISECONDS, EVENT_DARK_MENDING, eventId, PRIORITY_ALLY);
            ExecuteTargetEvent(SPELL_SHADOW_BOLT, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_SHADOW_BOLT, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Unbound Succubus 68205
struct npc_black_harvest_unbound_succubus : public demon_baseAI
{
    npc_black_harvest_unbound_succubus(Creature* creature) : demon_baseAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHADOW_SHOCK, 3.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_SHADOW_SHOCK, urand(5.5 * IN_MILLISECONDS, 8.5 * IN_MILLISECONDS), EVENT_SHADOW_SHOCK, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Unbound Shivarra 68206
struct npc_black_harvest_unbound_shivarra : public demon_baseAI
{
    npc_black_harvest_unbound_shivarra(Creature* creature) : demon_baseAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MELT_FLESH, 3.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_MELT_FLESH, urand(5.5 * IN_MILLISECONDS, 8.5 * IN_MILLISECONDS), EVENT_MELT_FLESH, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Freed Imp 68173
struct npc_black_harvest_freed_imp : public demon_baseAI
{
    npc_black_harvest_freed_imp(Creature* creature) : demon_baseAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FIREBOLT, 1 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_FIREBOLT, urand(1 * IN_MILLISECONDS, 1.5 * IN_MILLISECONDS), EVENT_FIREBOLT, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Suffering Soul Fragment 68139
struct npc_black_harvest_suffering_soul_fragment : public customCreatureAI
{
    npc_black_harvest_suffering_soul_fragment(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SOUL_BLAST, 9.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(1.5 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_SOUL_BLAST, 11 * IN_MILLISECONDS, EVENT_SOUL_BLAST, eventId);
            ExecuteTargetEvent(SPELL_SHADOW_BOLT_2, urand(2 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS), EVENT_SHADOW_BOLT, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Ashtongue Shaman 68129
struct npc_black_harvest_ashtongue_shaman : public customCreatureAI
{
    npc_black_harvest_ashtongue_shaman(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_SEARCHING_FOR_INTRUDERS);
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_HEX, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING, 3 * IN_MILLISECONDS);
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (!victim)
            return;

        if (!victim->HasAura(SPELL_BLACKOUT))
        {
            DoCast(victim, SPELL_BLACKOUT);
            EnterEvadeMode();
        }
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        me->GetMotionMaster()->MoveTargetedHome();
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
            ExecuteTargetEvent(SPELL_LIGHTNING, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS), EVENT_LIGHTNING, eventId);
            ExecuteTargetEvent(SPELL_HEX, 15 * IN_MILLISECONDS, EVENT_HEX, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Ashtongue Primalist 68096
struct npc_black_harvest_ashtongue_primalist : public customCreatureAI
{
    npc_black_harvest_ashtongue_primalist(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_SEARCHING_FOR_INTRUDERS);
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who, SPELL_NETTED, true);
        events.ScheduleEvent(EVENT_MULTI_SHOT, 1.5*IN_MILLISECONDS);
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (!victim)
            return;

        if (!victim->HasAura(SPELL_BLACKOUT))
        {
            DoCast(victim, SPELL_BLACKOUT);
            EnterEvadeMode();
        }
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        me->GetMotionMaster()->MoveTargetedHome();
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
            ExecuteTargetEvent(SPELL_MULTI_SHOT, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS), EVENT_SOUL_BLAST, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Ashtongue Worker 68098

// Kanrethad Ebonlocke 69964
struct npc_black_harvest_kanrethad_ebonlocke : public customCreatureAI
{
    npc_black_harvest_kanrethad_ebonlocke(Creature* creature) : customCreatureAI(creature) 
    {
        SetCombatMovement(false);
    }

    TaskScheduler scheduler;
    InstanceScript* instance;
    uint32 delay;
    bool hasTriggered;
    bool hasEnd;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
        me->GetMotionMaster()->MovePoint(0, kanrethadPath[0]);

        delay = me->GetSplineDuration();

        scheduler
            .Schedule(Milliseconds(delay), [this](TaskContext context)
        {
            Talk(TALK_INTRO);
        });

        scheduler
            .Schedule(Milliseconds(delay += 6 * IN_MILLISECONDS), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_1);
            me->GetMotionMaster()->MovePoint(0, kanrethadPath[1]);
        });

        scheduler
            .Schedule(Milliseconds(delay += 9800), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_2);
            
            if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                me->SetFacingTo(me->GetAngle(target));
        });

        scheduler
            .Schedule(Milliseconds(delay += 1500), [this](TaskContext context)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        });
    }

    void Reset() override
    {
        hasTriggered = false;
        hasEnd = false;
        events.Reset();
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (hasEnd)
            damage = 0;

        if (HealthBelowPct(50) && !hasTriggered)
        {
            hasTriggered = true;
            SetPhase(PHASE_AFFLICTION);
        }

        if (damage >= me->GetHealth() && !hasEnd)
        {
            damage = 0;
            events.Reset();
            me->SetHomePosition(*me);
            hasEnd = true;
            summons.DespawnAll();
            me->InterruptNonMeleeSpells(true);

            Talk(TALK_SPECIAL_5);

            if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                DoCast(target, SPELL_DEMONIC_GRASP);

            scheduler
                .Schedule(Milliseconds(4500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_6);

                if (Creature* jubeka = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_JUBEKA_SHADOWBREAKER) : ObjectGuid::Empty))
                    jubeka->AI()->DoAction(ACTION_START_INTRO);
            });
        }
    }

    void EnterEvadeMode() override
    {
        if (me->HasAura(SPELL_ETERNAL_BANISHMENT))
            return;

        ScriptedAI::EnterEvadeMode();

        summons.DespawnAll();

        me->GetMotionMaster()->MoveTargetedHome();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        delay = 0;

        scheduler
            .Schedule(Milliseconds(delay += 10500), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_3);
        });

        scheduler
            .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
        {
            // Start call demon here
            if (Creature* gateWay = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_DEMONIC_GATEWAY) : ObjectGuid::Empty))
                DoCast(gateWay, SPELL_SUMMON_PIT_LORD);
        });

        scheduler
            .Schedule(Milliseconds(delay += 1500), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_4);
        });

        scheduler
            .Schedule(Milliseconds(delay += 15000), [this](TaskContext context)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

            if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                me->RemoveChanneledCast(target->GetGUID());

            // Events Here
            SetPhase(PHASE_DESTRUCTION);
        });
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
            summon->RemoveChanneledCast(target->GetGUID());

    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        clicker->CastSpell(me, SPELL_DRAIN_ENERGY, false);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_KANRETHAD_DEFEATED)
        {
            me->SetFaction(35);
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    void SetPhase(uint8 newPhase)
    {
        events.Reset();

        switch (newPhase)
        {
            case PHASE_DESTRUCTION:
                DoCast(me, SPELL_BURNING_EMBERS, true);
                events.ScheduleEvent(EVENT_CATACLYSM, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CHAOS_BOLT, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_RAIN_OF_FIRE, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SOUL_FIRE, 4 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SUMMON, 35 * IN_MILLISECONDS);
                break;
            case PHASE_AFFLICTION:
                me->RemoveAurasDueToSpell(SPELL_BURNING_EMBERS);
                DoCast(me, SPELL_SOUL_SHARDS, true);
                DoCast(me, SPELL_ANNIHILATE_DEMONS, true);
                events.ScheduleEvent(EVENT_EX_AGONY, 4 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_AURA_OF_OMNIPOTENCE, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SEED_OF_DESTRUCTION, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BACKFIRE, 7.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CURSE_OF_ULTIMATE_DOOM, 30 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SUMMON, 35 * IN_MILLISECONDS);
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (me->HasAura(SPELL_ETERNAL_BANISHMENT))
            return;

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_CATACLYSM, 40 * IN_MILLISECONDS, EVENT_CATACLYSM, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CHAOS_BOLT, urand(15 * IN_MILLISECONDS, 18 * IN_MILLISECONDS), EVENT_CHAOS_BOLT, eventId);
            ExecuteTargetEvent(SPELL_RAIN_OF_FIRE, urand(4 * IN_MILLISECONDS,4.5 * IN_MILLISECONDS), EVENT_RAIN_OF_FIRE, eventId);
            ExecuteTargetEvent(SPELL_SOUL_FIRE, 11 * IN_MILLISECONDS, EVENT_SOUL_FIRE, eventId);
            ExecuteTargetEvent(urand(0, 1) ? SPELL_SUMMON_FELHUNTERS : SPELL_SUMMON_WILD_IMPS, 50 * IN_MILLISECONDS, EVENT_SUMMON, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_EX_AGONY, 20 * IN_MILLISECONDS, EVENT_EX_AGONY, eventId);
            ExecuteTargetEvent(SPELL_AURA_OF_OMNIPOTENCE, 25 * IN_MILLISECONDS, EVENT_AURA_OF_OMNIPOTENCE, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_BACKFIRE, 20 * IN_MILLISECONDS, EVENT_BACKFIRE, eventId);
            ExecuteTargetEvent(SPELL_CURSE_OF_ULTIMATE_DOON, 0, EVENT_CURSE_OF_ULTIMATE_DOOM, eventId);
            ExecuteTargetEvent(SPELL_SEED_OF_DESTRUCTION, 12.5 * IN_MILLISECONDS, EVENT_SEED_OF_DESTRUCTION, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Black Temple Top Credit 68054
struct npc_black_harvest_black_temple_top_credit : public ScriptedAI
{
    npc_black_harvest_black_temple_top_credit(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* jubeka = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_JUBEKA_SHADOWBREAKER) : ObjectGuid::Empty))
            jubeka->AI()->Talk(TALK_SPECIAL_2);

        summonerGUID = summoner->GetGUID();
        if (summoner->GetTypeId() != TYPEID_PLAYER)
            return;

        summoner->ToPlayer()->LearnSpell(SPELL_CODEX_OF_XERATTH, false);
        summoner->CastSpell(summoner, SPELL_FEEL_ENERGY_VEHICLE, true);

        me->SetSpeed(MOVE_WALK, 0.1f);
        me->SetSpeed(MOVE_RUN,  0.1f);

        me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 20.0f);

        uint32 delay = 0;

        scheduler
            .Schedule(Milliseconds(delay += 500), [this](TaskContext context)
        {
            if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                owner->CastSpell(owner, SPELL_FEEL_ENERGY_SPIN, true);
        });

        scheduler
            .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
        {
            DoCast(me, SPELL_FEEL_ENERGY_GREEN_LIGHT);
        });

        scheduler
            .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
        {
            DoCast(me, SPELL_FEEL_ENERGY_GOLDEN_ORB);
        });

        scheduler
            .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
        {
            DoCast(me, SPELL_FEEL_ENERGY_GREEN_BALL);
        });

        scheduler
            .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
        {
            DoCast(me, SPELL_FEEL_ENERGY_BIG_BALL);
        });

        scheduler
            .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
        {
            DoCast(me, SPELL_FEEL_ENERGY_EXPLOSION);
        });

        scheduler
            .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
        {
            if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
                owner->ExitVehicle();


            me->DespawnOrUnsummon();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Demonic Soulwell 70052
struct npc_black_harvest_demonic_soulwell : public customCreatureAI
{
    npc_black_harvest_demonic_soulwell(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonGUID;
    bool hasTriggered;

    void Reset() override
    {
        events.Reset();
        summonGUID = ObjectGuid::Empty;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        clicker->CastSpell(me, SPELL_PLACE_EMPOWERED_SOULCORE, false);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

            if (Creature* gates = me->SummonCreature(NPC_DEMONIC_GATEWAY, DemonicGatewayPos, TEMPSUMMON_MANUAL_DESPAWN))
                summonGUID = gates->GetGUID();

            uint32 delay = 0;

            scheduler
                .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
            {
                if (Creature* gates = ObjectAccessor::GetCreature(*me, summonGUID))
                    gates->CastSpell(gates, SPELL_DEMONIC_GATEWAY_EFF, true);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
            {
                me->SummonCreature(NPC_KANRETHAD_EBONLOCKE, DemonicGatewayPos, TEMPSUMMON_MANUAL_DESPAWN);
            });
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && me->GetInstanceScript() && 
            me->GetInstanceScript()->GetData(STEP_PLUNDER_THE_DEN_OF_MORTAL_DELIGHTS) == DONE && me->GetInstanceScript()->GetData(STEP_HEAD_OF_THE_TEMPLE_SUMMIT) != DONE)
        {
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->GetInstanceScript()->SetData(STEP_HEAD_OF_THE_TEMPLE_SUMMIT, DONE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Jubeka Shadowbreaker 70166
struct npc_black_harvest_jubeka_shadowbreaker : public ScriptedAI
{
    npc_black_harvest_jubeka_shadowbreaker(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            uint32 delay = 0;

            me->SetVisible(true);

            if (Creature* kanrethad = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KANRETHAD_EBONLOCKE) : ObjectGuid::Empty))
                me->GetMotionMaster()->MovePoint(0, kanrethad->GetPositionX() + frand(-5.0f, 5.0f), kanrethad->GetPositionY() + frand(-5.0f, 5.0f), kanrethad->GetPositionZ());

            scheduler
                .Schedule(Milliseconds(2000), [this](TaskContext context)
            {
                if (Creature* kanrethad = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KANRETHAD_EBONLOCKE) : ObjectGuid::Empty))
                    kanrethad->AI()->Talk(TALK_SPECIAL_7);
            });

            scheduler
                .Schedule(Milliseconds(delay+=me->GetSplineDuration()), [this](TaskContext context)
            {
                Talk(TALK_INTRO);

                if (Creature* kanrethad = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KANRETHAD_EBONLOCKE) : ObjectGuid::Empty))
                    DoCast(kanrethad, SPELL_ETERNAL_BANISHMENT);

                if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    target->RemoveAurasDueToSpell(SPELL_DEMONIC_GRASP);
            });

            scheduler
                .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
            {
                Talk(TALK_INTRO);

                if (Creature* kanrethad = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KANRETHAD_EBONLOCKE) : ObjectGuid::Empty))
                    kanrethad->AI()->Talk(TALK_SPECIAL_8);
            });

            scheduler
                .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);

                if (instance)
                    instance->SetData(STEP_DEFEAT_KANRETHAD, DONE);

                if (Player* target = ObjectAccessor::GetPlayer(*me, instance ? instance->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    target->KilledMonsterCredit(NPC_HARVEST_CREDIT);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Illidari Scroll 216364
class go_black_harvest_illidari_scroll : public GameObjectScript
{
    public:
        go_black_harvest_illidari_scroll() : GameObjectScript("go_black_harvest_illidari_scroll") { }
    
        void OnLootStateChanged(GameObject* go, uint32 state, Unit* unit) override
        {
            if (state == GO_ACTIVATED && unit)
            {
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

                if (Creature* akama = ObjectAccessor::GetCreature(*unit, unit->GetInstanceScript() ? unit->GetInstanceScript()->GetGuidData(NPC_AKAMA) : ObjectGuid::Empty))
                    akama->AI()->DoAction(ACTION_START_INTRO);
            }
        }
};

// Jubekas Mark 216481
class go_black_harvest_jubekas_mark : public GameObjectScript
{
    public:
        go_black_harvest_jubekas_mark() : GameObjectScript("go_black_harvest_jubekas_mark") { }

        bool OnReportUse(Player* player, GameObject* go) override
        {
            if (player->GetQuestStatus(QUEST_SEEK_THE_SIGNAL) == QUEST_STATUS_INCOMPLETE)
            {
                player->KilledMonsterCredit(NPC_BLACK_TEMPLE_ENTRANCE);
                return true;
            }

            if (player->GetQuestStatus(QUEST_INFILTRATING_TO_BLACK_TEMPLE) == QUEST_STATUS_INCOMPLETE || (player->GetQuestStatus(QUEST_INFILTRATING_TO_BLACK_TEMPLE) == QUEST_STATUS_REWARDED && !player->HasSpell(SPELL_CODEX_OF_XERATTH)))
            {
                lfg::LfgDungeonSet scenario = { 594 };
                sLFGMgr->JoinLfg(player, lfg::PLAYER_ROLE_DAMAGE, scenario, "");
            }

            return true;
        }
};

// Place Empowered Soulcore 138680
class spell_black_harvest_place_empowered_soulcore : public AuraScript
{
    PrepareAuraScript(spell_black_harvest_place_empowered_soulcore);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* soulwell = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_DEMONIC_SOULWELL) : ObjectGuid::Empty))
                soulwell->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_black_harvest_place_empowered_soulcore::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Annihilate Demons 139141
class spell_black_harvest_annihilate_demons : public SpellScript
{
    PrepareSpellScript(spell_black_harvest_annihilate_demons);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_KANRETHAD_WILD_IMP && target->GetEntry() != NPC_KANRETHAD_FEELHUNTER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_black_harvest_annihilate_demons::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Eternal Banishment 139186
class spell_black_harvest_eternal_banishment : public AuraScript
{
    PrepareAuraScript(spell_black_harvest_eternal_banishment);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_KANRETHAD_DEFEATED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_black_harvest_eternal_banishment::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Drain Fel Energy 139200
class spell_black_harvest_drain_fel_energy : public AuraScript
{
    PrepareAuraScript(spell_black_harvest_drain_fel_energy);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        if (Unit* caster = GetCaster())
            caster->SummonCreature(NPC_HARVEST_CREDIT, felStalkerPos, TEMPSUMMON_MANUAL_DESPAWN);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_black_harvest_drain_fel_energy::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Blackout 134112
class spell_black_harvest_blackout : public AuraScript
{
    PrepareAuraScript(spell_black_harvest_blackout);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->NearTeleportTo(entrancePos.GetPositionX(), entrancePos.GetPositionY(), entrancePos.GetPositionZ(), entrancePos.GetOrientation());
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_black_harvest_blackout::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Spellflame 134234
class spell_black_harvest_spellflame : public SpellScript
{
    PrepareSpellScript(spell_black_harvest_spellflame);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_INIT_FLAME);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_black_harvest_spellflame::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

class scene_memory_of_relicvary : public SceneScript
{
    public:
        scene_memory_of_relicvary() : SceneScript("scene_memory_of_relicvary") { }
    
        void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            CreditScene(player);
        }
    
        void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            CreditScene(player);
        }
    
        void CreditScene(Player* player)
        {
            if (player->GetInstanceScript())
                player->GetInstanceScript()->SetData(STEP_UNCOVER_COUNCILS_PLAN, DONE);
        }
};

void AddSC_pursuing_the_black_harvest()
{
    new npc_black_harvest_akama();
    new creature_script<npc_black_harvest_essence_of_order>("npc_black_harvest_essence_of_order");
    new creature_script<npc_black_harvest_unbound_centurion>("npc_black_harvest_unbound_centurion");
    new creature_script<npc_black_harvest_unbound_nightlord>("npc_black_harvest_unbound_nightlord");
    new creature_script<npc_black_harvest_unbound_bonemender>("npc_black_harvest_unbound_bonemender");
    new creature_script<npc_black_harvest_unbound_succubus>("npc_black_harvest_unbound_succubus");
    new creature_script<npc_black_harvest_unbound_shivarra>("npc_black_harvest_unbound_shivarra");
    new creature_script<npc_black_harvest_freed_imp>("npc_black_harvest_freed_imp");
    new creature_script<npc_black_harvest_suffering_soul_fragment>("npc_black_harvest_suffering_soul_fragment");
    new creature_script<npc_black_harvest_ashtongue_shaman>("npc_black_harvest_ashtongue_shaman");
    new creature_script<npc_black_harvest_ashtongue_primalist>("npc_black_harvest_ashtongue_primalist");
    new creature_script<npc_black_harvest_kanrethad_ebonlocke>("npc_black_harvest_kanrethad_ebonlocke");
    new creature_script<npc_black_harvest_black_temple_top_credit>("npc_black_harvest_black_temple_top_credit");
    new creature_script<npc_black_harvest_demonic_soulwell>("npc_black_harvest_demonic_soulwell");
    new creature_script<npc_black_harvest_jubeka_shadowbreaker>("npc_black_harvest_jubeka_shadowbreaker");
    new go_black_harvest_illidari_scroll();
    new go_black_harvest_jubekas_mark();

    new aura_script<spell_black_harvest_place_empowered_soulcore>("spell_black_harvest_place_empowered_soulcore");
    new spell_script<spell_black_harvest_annihilate_demons>("spell_black_harvest_annihilate_demons");
    new aura_script<spell_black_harvest_eternal_banishment>("spell_black_harvest_eternal_banishment");
    new aura_script<spell_black_harvest_drain_fel_energy>("spell_black_harvest_drain_fel_energy");
    new aura_script<spell_black_harvest_blackout>("spell_black_harvest_blackout");
    new spell_script<spell_black_harvest_spellflame>("spell_black_harvest_spellflame");
    new scene_memory_of_relicvary();
}