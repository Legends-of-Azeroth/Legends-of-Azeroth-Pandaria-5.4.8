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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "temple_of_the_jade_serpent.h"

enum eEvents
{
    EVENT_INTRO_0    = 1,
    EVENT_INTRO_1    = 2,
    EVENT_INTRO_2    = 3,
    EVENT_INTRO_3    = 4,
    EVENT_SUN_0      = 5,
    EVENT_SUN_1      = 6,
    EVENT_SUN_2      = 7,
    EVENT_SUN_3      = 8,
    EVENT_SUN_4      = 9,
    EVENT_STRIFE_0   = 10,
    EVENT_STRIFE_1   = 11,
    EVENT_STRIFE_2   = 12,
    EVENT_STRIFE_3   = 13,
    EVENT_STRIFE_4   = 14,
    EVENT_CHECK_WIPE = 15
};

enum eMisc
{
    SPELL_CHALLENGE_ENCOUNTER_CREDIT = 126948, // it`s for challenge..
    CREDIT_STONESTEP                 = 56843,
    QUEST_RESTORE_JADE_PURITY        = 31355,
};

enum eTalks
{
    EVENT_TALK_INTRO_0             = 0,
    EVENT_TALK_INTRO_1             = 1,
    EVENT_TALK_INTRO_2             = 2,
    EVENT_TALK_INTRO_3             = 3,
    EVENT_TALK_ZAO_APPEARS_0       = 4,
    EVENT_TALK_ZAO_APPEARS_1       = 5,
    EVENT_TALK_ZAO_APPEARS_2       = 6,
    EVENT_TALK_ZAO_APPEARS_4       = 7,
    EVENT_TALK_ZAO_ENTERS_COMBAT_0 = 8,
    EVENT_TALK_STRIFE_0            = 9,
    EVENT_TALK_STRIFE_1            = 10,
    EVENT_TALK_STRIFE_2            = 11,
    EVENT_TALK_STRIFE_3            = 12,
    EVENT_TALK_STRIFE_4            = 13
};

enum eActions
{
    ACTION_ZAO   = 0,
    ACTION_TRIAL = 1,
    ACTION_RESET = 2
};

class boss_lorewalker_stonestep : public CreatureScript
{
    public:
        boss_lorewalker_stonestep() : CreatureScript("boss_lorewalker_stonestep") { }

        struct boss_lorewalker_stonestep_AI : public BossAI
        {
            boss_lorewalker_stonestep_AI(Creature* creature) : BossAI(creature, DATA_LOREWALKER) { }

            bool event;

            void InitializeAI() override
            {
                if (instance->GetBossState(DATA_LOREWALKER) == DONE)
                {
                    event = true;
                    me->CastSpell(me, SPELL_MEDITATION, false);
                    me->SetReactState(REACT_PASSIVE);
                }
                else
                {
                    _Reset();
                    event = false;
                    me->GetMotionMaster()->MoveTargetedHome();
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ZAO:
                        events.ScheduleEvent(EVENT_SUN_0, 0.5 * IN_MILLISECONDS);
                        break;
                    case ACTION_TRIAL:
                        events.ScheduleEvent(EVENT_STRIFE_0, 0.5 * IN_MILLISECONDS);                   
                        break;
                    case ACTION_RESET:
                        events.Reset();
                        break;
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!event && who->GetTypeId() == TYPEID_PLAYER)
                {
                    event = true;
                    events.ScheduleEvent(EVENT_INTRO_0, 500);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_WIPE:
                            if (instance)
                                if (instance->IsWipe())
                                    instance->SetData(DATA_LOREWALKER, FAIL);

                            events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_INTRO_0:
                            Talk(EVENT_TALK_INTRO_0);
                            events.ScheduleEvent(EVENT_INTRO_1, 8019);
                            break;
                        case EVENT_INTRO_1:
                            Talk(EVENT_TALK_INTRO_1);
                            events.ScheduleEvent(EVENT_INTRO_2, 16162);
                            break;
                        case EVENT_INTRO_2:
                            Talk(EVENT_TALK_INTRO_2); 
                            events.ScheduleEvent(EVENT_INTRO_3, 9578);
                            break;
                        case EVENT_INTRO_3:
                            Talk(EVENT_TALK_INTRO_3);
                            break;
                        case EVENT_SUN_0:
                            me->RemoveAura(SPELL_ROOT_SELF);
                            Talk(EVENT_TALK_ZAO_APPEARS_0);
                            events.ScheduleEvent(EVENT_SUN_1, 9641);
                            break;
                        case EVENT_SUN_1:
                            Talk(EVENT_TALK_ZAO_APPEARS_1);
                            me->GetMotionMaster()->MovePoint(0, 838.033f, -2480.518f, 176.744f);
                            events.ScheduleEvent(EVENT_SUN_2, 12220);
                            break;
                        case EVENT_SUN_2:
                            Talk(EVENT_TALK_ZAO_APPEARS_2);
                            me->GetMotionMaster()->MovePoint(0, 834.643f, -2490.361f, 179.897f);
                            events.ScheduleEvent(EVENT_SUN_3, 8280);
                            break;
                        case EVENT_SUN_3:
                            me->SetFacingTo(1.239f);
                            if (instance)
                            {
                                instance->SetData(DATA_SUN, DONE);
                                if (Creature* zao = Unit::GetCreature(*me, instance->GetGuidData(DATA_ZAO)))
                                   if (zao->IsAIEnabled)
                                      zao->AI()->DoAction(1);
                            }
                            events.ScheduleEvent(EVENT_SUN_4, 4214);
                            break;
                        case EVENT_SUN_4:
                            me->CastSpell(me, SPELL_MEDITATION, false);
                            Talk(EVENT_TALK_ZAO_APPEARS_4);
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_STRIFE_0:
                            me->RemoveAura(SPELL_ROOT_SELF);
                            Talk(EVENT_TALK_STRIFE_0);
                            events.ScheduleEvent(EVENT_STRIFE_1, 8790);
                            break;
                        case EVENT_STRIFE_1:
                            Talk(EVENT_TALK_STRIFE_1);
                            me->GetMotionMaster()->MovePoint(0, 838.033f, -2480.518f, 176.744f);
                            events.ScheduleEvent(EVENT_STRIFE_2, 10780);
                            break;
                        case EVENT_STRIFE_2:
                            Talk(EVENT_TALK_STRIFE_2);
                            me->GetMotionMaster()->MovePoint(0, 834.643f, -2490.361f, 179.897f);
                            events.ScheduleEvent(EVENT_STRIFE_3, 9790);
                            break;
                        case EVENT_STRIFE_3:
                           {
                               Talk(EVENT_TALK_STRIFE_3);
                               me->SetFacingTo(1.239f);
                               events.ScheduleEvent(EVENT_STRIFE_4, 6480);

                              if (Creature* peril = me->FindNearestCreature(59726, 60.0f))
                              {
                                  peril->SetReactState(REACT_AGGRESSIVE);
                                  peril->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

                                 if (instance)
                                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, peril);

                                 if (Creature* osong = me->SummonCreature(NPC_OSONG, 842.752f, -2468.911f, 174.959f))
                                    osong->AI()->AttackStart(peril);
                              }

                              if (Creature* strife = me->FindNearestCreature(59051, 60.0f))
                              {
                                 strife->SetReactState(REACT_AGGRESSIVE);
                                 strife->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);

                                 if (instance)
                                     instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, strife);
                               }
                            }
                            break;
                        case EVENT_STRIFE_4:
                            me->CastSpell(me, SPELL_MEDITATION, false);
                            Talk(EVENT_TALK_STRIFE_4);
                            events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lorewalker_stonestep_AI(creature);
        }
};

class npc_sun : public CreatureScript
{
    public:
        npc_sun() : CreatureScript("npc_sun") { }

        enum eEvents
        {
            EVENT_SUNFIRE_RAYS = 1
        };

        enum eSpells
        {
            SPELL_SUNFIRE_RAYS = 107223,
            SPELL_SUN          = 107349,
            SPELL_GROW_LOW     = 104921,
        };

        enum eActions
        {
            ACTION_SELECTABLE = 0
        };

        struct npc_sun_AI : public ScriptedAI
        {
            npc_sun_AI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->CastSpell(me, SPELL_SUN, false);
                me->CastSpell(me, SPELL_GROW_LOW, false);
                me->AddUnitState(UNIT_STATE_ROOT);
                me->SetDisableGravity(true);
                me->SetOrientation(1.239f);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_SELECTABLE:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        events.ScheduleEvent(EVENT_SUNFIRE_RAYS, 2 * IN_MILLISECONDS);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SUNFIRE_RAYS)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        me->CastSpell(target, SPELL_SUNFIRE_RAYS, true);

                    events.ScheduleEvent(EVENT_SUNFIRE_RAYS, urand(2,5) * IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sun_AI(creature);
        }
};

class npc_zao : public CreatureScript
{
    public:
        npc_zao() : CreatureScript("npc_zao") { }

        enum eEvents
        {
            EVENT_ZAO_ENTER_COMBAT_1 = 1,
            EVENT_ZAO_ATTACK         = 2,
            EVENT_ZAO_ENTERS_COMBAT  = 3
        };

        enum eSpells
        {
            SPELL_SHOOT_SUN         = 112084,
            SPELL_HELLFIRE_ARROW    = 113017,
            SPELL_SHA_CORRUPTION    = 120000
        };

        enum eTalks
        {
            TALK_INTRO = 0,
            TALK_AGGRO = 1
        };

        enum eActions
        {
            ACTION_TALK   = 1,
            ACTION_COMBAT = 2
        };

        struct npc_zao_AI : public ScriptedAI
        {
            npc_zao_AI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            bool isCorrupted;
            InstanceScript* instance;
            SummonList summons;
            EventMap events;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                isCorrupted = false;
                me->AddUnitState(UNIT_STATE_ROOT);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker->ToCreature() && (attacker->ToCreature()->GetEntry() == NPC_HAUNTING_SHA_2 || attacker->ToCreature()->GetEntry() == NPC_HAUNTING_SHA))
                {
                    attacker->ToCreature()->DespawnOrUnsummon();
                    isCorrupted = true;
                    me->SetFaction(14);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->CastSpell(me, SPELL_SHA_CORRUPTION, false);

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                summons.DespawnAll();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    instance->SetData(DATA_ZAO, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    for (auto&& itr : instance->instance->GetPlayers())
                    {
                        if (Player* player = itr.GetSource())
                            if (player->GetQuestStatus(QUEST_RESTORE_JADE_PURITY) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(CREDIT_STONESTEP);
                    }
                }

                summons.DespawnAll();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_COMBAT:
                        events.Reset();
                        events.ScheduleEvent(EVENT_ZAO_ENTER_COMBAT_1, 1 * IN_MILLISECONDS);
                        break;
                    case ACTION_TALK:                   
                        Talk(TALK_INTRO);
                        events.ScheduleEvent(EVENT_ZAO_ATTACK, 1 * IN_MILLISECONDS);                  
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ZAO_ENTER_COMBAT_1:
                            events.ScheduleEvent(EVENT_ZAO_ENTERS_COMBAT, 5.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ZAO_ENTERS_COMBAT:
                            Talk(TALK_AGGRO);
                            events.ScheduleEvent(EVENT_ZAO_ATTACK, 5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ZAO_ATTACK:
                            if (isCorrupted)
                            {                          
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                    me->CastSpell(target, SPELL_HELLFIRE_ARROW, true);

                                events.ScheduleEvent(EVENT_ZAO_ATTACK, 3000);
                            }
                            else
                            {
                                std::list<Creature*> suns;
                                me->GetCreatureListWithEntryInGrid(suns, NPC_ZAO_SUN, 100.0f);
                                for (auto&& creature : suns)
                                    DoCast(creature, SPELL_SHOOT_SUN);

                                events.ScheduleEvent(EVENT_ZAO_ATTACK, 3 * IN_MILLISECONDS);
                            }
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zao_AI(creature);
        }
};

class npc_haunting_sha : public CreatureScript
{
    public:
        npc_haunting_sha() : CreatureScript("npc_haunting_sha") { }

        enum eEvents
        {
            EVENT_HAUNTING_GAZE = 1
        };

        enum eSpells
        {
            SPELL_HAUNTING_GAZE = 114646
        };

        struct npc_haunting_sha_AI : public ScriptedAI
        {
            npc_haunting_sha_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* summoner) override
            {
                instance = me->GetInstanceScript();

                if (Creature* zao = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_ZAO) : ObjectGuid::Empty))
                    zao->AI()->JustSummoned(me);
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_HAUNTING_GAZE, 1 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == 0)
                {
                    if (instance)
                    {
                        if (Creature* zao = Unit::GetCreature(*me, instance->GetGuidData(DATA_ZAO)))
                        {
                            me->GetThreatManager().addThreat(zao, 1000000.f);
                            me->AI()->AttackStart(zao);
                        }
                    }
                }
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                    me->GetInstanceScript()->SetData(DATA_SUN_STATE, 1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->GetVictim())
                {
                    if (instance)
                    {
                        Map::PlayerList const& PlayerList = me->GetInstanceScript()->instance->GetPlayers();

                        if (!PlayerList.isEmpty())
                        {
                            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            {
                                if (Player* plr = i->GetSource())
                                    me->GetThreatManager().addThreat(plr, 1.0f);
                            }
                        }
                    }

                    AttackStart(SelectTarget(SELECT_TARGET_RANDOM));
                }

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_HAUNTING_GAZE)
                {
                    me->CastSpell(me->GetVictim(), SPELL_HAUNTING_GAZE, false);
                    events.ScheduleEvent(EVENT_HAUNTING_GAZE, 2 * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_haunting_sha_AI(creature);
        }
};

class npc_strife_peril : public CreatureScript
{
    public:
        npc_strife_peril() : CreatureScript("npc_strife_peril") { }

        enum eEvents
        {
            EVENT_AGONY       = 1
        };

        enum eSpells
        {
            SPELL_AGONY              = 114571,
            SPELL_SHA_CORRUPTION_SP  = 115086,
            SPELL_INTENSITY          = 113316
        };

        enum eActions
        {
            ACTION_IN_COMBAT = 1
        };

        struct npc_strife_peril_AI : public ScriptedAI
        {
            npc_strife_peril_AI(Creature* creature) : ScriptedAI(creature) { }

            bool isInfight;
            InstanceScript* instance;
            EventMap events;

            void InitializeAI() override
            {
                isInfight = false;
                me->SetFaction(14);
                instance = me->GetInstanceScript();  
                me->CastSpell(me, SPELL_SHA_CORRUPTION_SP, false);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker->GetTypeId() == TYPEID_UNIT && !attacker->ToCreature()->IsPet())
                {
                    if (me->GetHealth() < me->GetMaxHealth() || me->GetHealth() <= damage)
                        damage = 0;
                }
                else
                {
                    if (!me->HasAura(SPELL_INTENSITY) && !isInfight)
                    {
                        isInfight = true;
                        me->CastSpell(me, SPELL_INTENSITY, false);

                        if (me->GetEntry() == NPC_STRIFE)
                        {
                            if (Creature* peril = me->FindNearestCreature(NPC_PERIL, 50.0f))
                                if (peril->IsAIEnabled)
                                    peril->AI()->DoAction(ACTION_IN_COMBAT);
                        }
                        else
                        {
                            if (Creature* strife = me->FindNearestCreature(NPC_STRIFE, 50.0f))
                                if (strife->IsAIEnabled)
                                    strife->AI()->DoAction(ACTION_IN_COMBAT);
                        };
                    }      
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_IN_COMBAT)
                {
                    if (!me->HasAura(SPELL_INTENSITY) && !isInfight)
                    {
                        isInfight = true;
                        me->CastSpell(me, SPELL_INTENSITY, false);
                    }
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_AGONY, 1 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAura(SPELL_INTENSITY);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_TRIAL, DONE);

                    for (auto&& itr : instance->instance->GetPlayers())
                    {
                        if (Player* player = itr.GetSource())
                            if (player->GetQuestStatus(QUEST_RESTORE_JADE_PURITY) == QUEST_STATUS_INCOMPLETE)
                                player->KilledMonsterCredit(CREDIT_STONESTEP);
                    }

                    if (instance->GetData(DATA_LOREWALKER) != DONE)
                        me->SetLootRecipient(NULL);
                    else
                    {
                        if (!instance->instance->IsChallengeDungeon())
                            for (auto&& itr : instance->instance->GetPlayers())
                                itr.GetSource()->ModifyCurrency(395, DUNGEON_MODE(70, 125) * CURRENCY_PRECISION);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_AGONY)
                {
                    me->CastSpell(me->GetVictim(), SPELL_AGONY, false);
                    events.ScheduleEvent(EVENT_AGONY, 2 * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_strife_peril_AI(creature);
        }
};

class npc_nodding_tiger : public CreatureScript
{
    public:
        npc_nodding_tiger() : CreatureScript("npc_nodding_tiger") { }

        enum eEvents
        {
            EVENT_CLAW = 1
        };

        enum eSpells
        {
            SPELL_CLAW = 31289
        };

        struct npc_nodding_tiger_AI : public ScriptedAI
        {
            npc_nodding_tiger_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_CLAW, 2 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_CLAW)
                {
                    me->CastSpell((Unit*)NULL, SPELL_CLAW, false);
                    events.ScheduleEvent(EVENT_CLAW, 3 * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_nodding_tiger_AI(creature);
        }
};

class npc_golden_beetle : public CreatureScript
{
    public:
        npc_golden_beetle() : CreatureScript("npc_golden_beetle") { }

        enum eEvents
        {
            EVENT_SERRATED_SLASH = 1,
            EVENT_SHROUD_OF_GOLD = 2,
            EVENT_SLOW           = 3
        };

        enum eSpells
        {
            SPELL_SERRATED_SLASH = 128051,
            SPELL_SHROUD_OF_GOLD = 88023,
            SPELL_SLOW           = 31589
        };

        struct npc_golden_beetle_AI : public ScriptedAI
        {
            npc_golden_beetle_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_SERRATED_SLASH, 2 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SHROUD_OF_GOLD, 4 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SLOW, 6 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SERRATED_SLASH:
                            me->CastSpell(me->GetVictim(), SPELL_SERRATED_SLASH, false);
                            events.ScheduleEvent(EVENT_SERRATED_SLASH, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_SHROUD_OF_GOLD:
                            me->CastSpell(me->GetVictim(), SPELL_SHROUD_OF_GOLD, false);
                            events.ScheduleEvent(EVENT_SHROUD_OF_GOLD, 5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SLOW:
                            me->CastSpell(me->GetVictim(), SPELL_SLOW, false);
                            events.ScheduleEvent(EVENT_SLOW, 15 * IN_MILLISECONDS);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_golden_beetle_AI(creature);
        }
};

class npc_jiang_xiang : public CreatureScript
{
    public:
        npc_jiang_xiang() : CreatureScript("npc_jiang_xiang") { }

        enum eEvents
        {
            EVENT_AERIALISTS_KICK = 1,
            EVENT_THROW_TORCH     = 2
        };

        enum eSpells
        {
            SPELL_AERIALISTS_KICK = 114805,
            SPELL_THROW_TORCH     = 114803
        };

        struct npc_jiang_xiang_AI : public ScriptedAI
        {
            npc_jiang_xiang_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_AERIALISTS_KICK, 2 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_THROW_TORCH, 4 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AERIALISTS_KICK:
                            me->CastSpell(me->GetVictim(), SPELL_AERIALISTS_KICK, false);
                            events.ScheduleEvent(EVENT_AERIALISTS_KICK, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_THROW_TORCH:
                            me->CastSpell(me->GetVictim(), SPELL_THROW_TORCH, false);
                            events.ScheduleEvent(EVENT_THROW_TORCH, 5 * IN_MILLISECONDS);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jiang_xiang_AI(creature);
        }
};

class npc_songbird_queen : public CreatureScript
{
    public:
        npc_songbird_queen() : CreatureScript("npc_songbird_queen") { }

        enum eSpells
        {
            SPELL_SONGBIRD_SERENADE = 114826
        };

        enum eEvents
        {
            EVENT_SONGBIRD_SERENADE = 1
        };

        struct npc_songbird_queen_AI : public ScriptedAI
        {
            npc_songbird_queen_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_SONGBIRD_SERENADE, 2 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SONGBIRD_SERENADE)
                {
                    me->CastSpell(me->GetVictim(), SPELL_SONGBIRD_SERENADE, false);
                    events.ScheduleEvent(EVENT_SONGBIRD_SERENADE, 10 * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_songbird_queen_AI(creature);
        }
};

class npc_corrupted_scroll : public CreatureScript
{
    public:
        npc_corrupted_scroll() : CreatureScript("npc_corrupted_scroll") { }

        enum eSpells
        {
            SPELL_SCROLL_FLOOR = 107350,
            SPELL_JADE_ENERGY_2 = 111452,
            SPELL_GROW_LOW = 104921,

            SPELL_SHA_BURNING = 111588,
            SPELL_SHA_EXPLOSION = 111579,
            SPELL_DEATH = 98391
        };

        struct npc_corrupted_scrollAI : public ScriptedAI
        {
            npc_corrupted_scrollAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->setRegeneratingHealth(false);
                me->CastSpell(me, SPELL_SCROLL_FLOOR, false);
                me->CastSpell(me, SPELL_JADE_ENERGY_2, false);
                me->CastSpell(me, SPELL_GROW_LOW, false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAllAuras();
                me->CastSpell(me, SPELL_SHA_BURNING, false);
                me->CastSpell(me, SPELL_SHA_EXPLOSION, false);
                me->CastSpell(me, SPELL_DEATH, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_corrupted_scrollAI(creature);
        }
};

class npc_talking_fish : public CreatureScript
{
    public:
        npc_talking_fish() : CreatureScript("npc_talking_fish") { }

        enum eTalks
        {
            TALK_0 = 0,
            TALK_1 = 1,
            TALK_2 = 2,
            TALK_3 = 3,
        };

        enum eEvents
        {
            EVENT_BUBBLE_SPRAY = 1,
            EVENT_TALK_2 = 2,
            EVENT_TALK_3 = 3,
            EVENT_TALK_4 = 4
        };

        enum eSpells
        {
            SPELL_BUBBLE_SPRAY = 114811
        };

        struct npc_talking_fish_AI : public ScriptedAI
        {
            npc_talking_fish_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void JustEngagedWith(Unit* who) override
            {
                Talk(TALK_0);
                events.ScheduleEvent(EVENT_BUBBLE_SPRAY, 2000);
                events.ScheduleEvent(EVENT_TALK_2, 3 * IN_MILLISECONDS);
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
                        case EVENT_BUBBLE_SPRAY:
                            me->CastSpell(me->GetVictim(), SPELL_BUBBLE_SPRAY, false);
                            events.ScheduleEvent(EVENT_BUBBLE_SPRAY, 10000);
                            break;
                        case EVENT_TALK_2:
                            Talk(TALK_1);
                            events.ScheduleEvent(EVENT_TALK_3, 3.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_TALK_3:
                            Talk(TALK_2);
                            events.ScheduleEvent(EVENT_TALK_4, 3 * IN_MILLISECONDS);
                            break;
                        case EVENT_TALK_4:
                            Talk(TALK_3);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_talking_fish_AI(creature);
        }
};

class spell_tjs_intensity : public SpellScriptLoader
{
    public:
        spell_tjs_intensity() : SpellScriptLoader("spell_tjs_intensity") { }

        enum eSpells
        {
            SPELL_ULTIMATE_POWER = 113309,
            SPELL_INTENSITY      = 113315,
            SPELL_DISSIPATION    = 113379 
        };

        class spell_tjs_intensity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tjs_intensity_AuraScript);

            bool canProc;
            uint8 intensityStack, prevIntensityStack;

            bool Load() override
            {
                if (Unit* owner = GetOwner()->ToUnit())
                {
                    if (Aura *aura = owner->GetAura(SPELL_INTENSITY))
                        intensityStack = aura->GetStackAmount();
                }
                prevIntensityStack = 0;
                return true;
            }

            bool HandleCheckProc(ProcEventInfo &eventInfo)
            {
                if (!canProc)
                    return false;

                auto const caster = eventInfo.GetActionTarget();
                if (caster && caster->HasAura(SPELL_ULTIMATE_POWER))
                    return false;

                auto const target = eventInfo.GetActor();
                if (!target || target->GetTypeId() != TYPEID_PLAYER)
                    return false;

                return true;
            }

            void HanleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                {
                    if (Aura *aura = owner->GetAura(SPELL_INTENSITY))
                    {
                        intensityStack = aura->GetStackAmount();
                        if (aura->GetStackAmount() == 10)
                        {                      
                            owner->RemoveAura(SPELL_INTENSITY);
                            owner->CastSpell(owner, SPELL_ULTIMATE_POWER, false);                       
                        }
                    }

                    if (owner->HasAura(SPELL_DISSIPATION))
                        owner->RemoveAura(SPELL_DISSIPATION);
                }

                canProc = false;
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (canProc)
                {
                    if (Unit* owner = GetOwner()->ToUnit())
                    {
                        if (owner->HasAura(SPELL_INTENSITY))
                        {
                            if (Aura *aura = owner->GetAura(SPELL_INTENSITY))
                            {
                                if (intensityStack <= prevIntensityStack)
                                {
                                    aura->ModStackAmount(-1);
                                    intensityStack--;
                                    prevIntensityStack--;
                                }
                                else
                                    prevIntensityStack = aura->GetStackAmount();
                            }
                        }
                        else
                        {
                            if (!owner->HasAura(SPELL_ULTIMATE_POWER))
                                owner->CastSpell(owner, SPELL_DISSIPATION, false);
                        }
                    }
                }

                if (!canProc)
                    canProc = true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetOwner()->ToUnit())
                {
                    if (owner->HasAura(SPELL_INTENSITY))
                        owner->RemoveAura(SPELL_INTENSITY);

                    if (owner->HasAura(SPELL_DISSIPATION))
                        owner->RemoveAura(SPELL_DISSIPATION);

                    if (owner->HasAura(SPELL_ULTIMATE_POWER))
                        owner->RemoveAura(SPELL_ULTIMATE_POWER);
                }
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_tjs_intensity_AuraScript::HandleCheckProc);
                OnEffectProc += AuraEffectProcFn(spell_tjs_intensity_AuraScript::HanleOnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_tjs_intensity_AuraScript::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tjs_intensity_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tjs_intensity_AuraScript();
        }
};

void AddSC_boss_lorewalker_stonestep()
{
    new boss_lorewalker_stonestep();
    new npc_sun();
    new npc_zao();
    new npc_haunting_sha();
    new npc_strife_peril();

    new npc_corrupted_scroll();
    new npc_nodding_tiger();
    new npc_golden_beetle();
    new npc_jiang_xiang();
    new npc_songbird_queen();
    new npc_talking_fish();
    new spell_tjs_intensity();
}