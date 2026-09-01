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
#include "MoveSplineInit.h"
#include "shadopan_monastery.h"
#include "Vehicle.h"

Position SnowdriftPos[2] =
{
    {3680.56f, 3045.27f, 816.20f},// event place
    {3713.60f, 3091.87f, 817.31f} // fight place
};

Position ClonePos[3] =
{
    {3683.37f, 3087.65f, 815.70f, 5.67f},
    {3719.41f, 3062.31f, 815.70f, 1.79f},
    {3718.04f, 3097.97f, 817.40f, 3.46f}
};

const Position ChargePos = { 3692.12f, 3061.77f, 816.20f, 0.934f }; // Hack cuz he couldn`t find path with that next textures

#define SPELL_ROOT_FOR_EVER 31366

class boss_master_snowdrift : public CreatureScript
{
    public:
        boss_master_snowdrift() : CreatureScript("boss_master_snowdrift") { }

        enum eActions
        {
            ACTION_START_EVENT   = 0,
            ACTION_NOVOCE_DEFEAT = 1,
            ACTION_TALK_WAVE_1   = 2,
            ACTION_TALK_WAVE_2   = 3,
            ACTION_TALK_OUTRO    = 4,
            ACTION_TALK_WAVE_1_S = 5,
            ACTION_DISSAPEAR     = 6
        };

        enum eSpells
        {
            SPELL_BALL_OF_FIRE   = 113760,
            SPELL_FIST_OF_FURY   = 106853,
            SPELL_PARRY_STANCE   = 106454,
            SPELL_QUIVERING_PALM = 106422,
            SPELL_TORNADO_KICK   = 106434,
            SPELL_SMOKE_BOMB     = 110444,
            SPELL_HURL_CHI       = 106825,
            SPELL_CHASE_DOWN     = 118961,
            SPELL_TORNADO_SLAM   = 106330
        };

        enum eEvents
        {
            // Snowdrift
            EVENT_FIST_OF_FURY   = 1,
            EVENT_TORNADO_KICK   = 2,
            EVENT_PARRY_STANCE   = 3,
            EVENT_TORNADO_SLAM   = 4,
            EVENT_DISAPPEAR      = 5,
            EVENT_INTRO_1        = 6,
            EVENT_INTRO_2        = 7,
            EVENT_INTRO_3        = 8,
            EVENT_OUTRO_1        = 9,
            EVENT_OUTRO_2        = 10,
            EVENT_OUTRO_END      = 11,
            EVENT_CHECK_DEST     = 12,
            EVENT_CHECK_WIPE     = 13,
            EVENT_PHASE_2        = 14,
            EVENT_POSSESIONS     = 15,
            EVENT_FIST_END       = 16,
        };

        enum ePhases
        {
            PHASE_FIGHT_1 = 1,
            PHASE_FIGHT_2 = 2,
            PHASE_FIGHT_3 = 3,
            PHASE_FIGHT_4 = 4,
        };

        enum eTalks
        {
            TALK_INTRO_1     = 0,
            TALK_INTRO_2     = 1,
            TALK_WAWE1_START = 2,
            TALK_WAWE1_END   = 3,
            TALK_WAWE2_START = 4,
            TALK_WAWE2_END   = 5,
            TALK_OUTRO_1     = 6,
            TALK_OUTRO_2     = 7,
            TALK_AGGRO       = 8,
            TALK_DEATH       = 9,
            TALK_SLAY        = 10,
            TALK_PHASE_2     = 11,
            TALK_PHASE_3     = 12,
            TALK_STUDENTS    = 13,
            TALK_FISTS       = 14,
            TALK_TORNADO     = 15,
            TALK_PARRY       = 16,
            TALK_P_2_EMOTE   = 17,
            TALK_P_3_EMOTE   = 18
        };

        struct boss_master_snowdriftAI : public BossAI
        {
            boss_master_snowdriftAI(Creature* creature) : BossAI(creature, DATA_MASTER_SNOWDRIFT) { }

            EventMap nonCombatEvents;
            bool aggroDone, isBossSummoned, isFightWon;
            uint8 phase, hurlChiCnt;
            uint8 randIndex[3];
            ObjectGuid targetGUID;

            void InitializeAI() override
            {
                isFightWon = false;
                me->setActive(true);
                targetGUID = ObjectGuid::Empty;
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                for (uint8 i = 0; i < 3; ++i)
                    randIndex[i] = i;

                if (instance)
                {
                    if (instance->GetData(DATA_SNOWDRIFT_STATE) != DONE)
                    {
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->NearTeleportTo(SnowdriftPos[0].GetPositionX(), SnowdriftPos[0].GetPositionY(), SnowdriftPos[0].GetPositionZ(), 4.114f);
                    }

                    if (instance->GetBossState(DATA_MASTER_SNOWDRIFT) == DONE)
                    {
                        me->SetFaction(35);
                        me->SetReactState(REACT_PASSIVE);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        nonCombatEvents.ScheduleEvent(EVENT_POSSESIONS, 1 * IN_MILLISECONDS);

                        float x, y, z;
                        z = me->GetPositionZ();
                        GetPositionWithDistInOrientation(me, -5.0f, me->GetOrientation(), x, y);
                        me->NearTeleportTo(x, y, z, me->GetOrientation());

                        if (instance->GetBossState(DATA_TARAN_ZHU) == DONE)
                            me->SetVisible(false);
                    }
                }

                Reset();
            }

            void Reset() override
            {
                if (instance)
                {
                    if (instance->GetData(DATA_SNOWDRIFT_STATE) == DONE && instance->GetBossState(DATA_MASTER_SNOWDRIFT) != DONE)
                    {
                        _Reset();
                        events.Reset();
                        nonCombatEvents.Reset();
                        me->SetReactState(REACT_AGGRESSIVE);
                    }
                }

                me->setRegeneratingHealth(true);
                me->SetHealth(me->GetMaxHealth());
                phase = PHASE_FIGHT_1;
                isBossSummoned = false;
                aggroDone = false;
                hurlChiCnt = 0;
                targetGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (!aggroDone)
                {
                    aggroDone = true;
                    Talk(TALK_AGGRO);
                    events.ScheduleEvent(EVENT_FIST_OF_FURY, 15 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_TORNADO_KICK, 30 * IN_MILLISECONDS);

                    if (instance)
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                        instance->SetData(DATA_MASTER_SNOWDRIFT, IN_PROGRESS);
                    }

                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += 2500, 10, [this]()
                    {
                        if (me->IsInCombat())
                            _JustEngagedWith();
                    });
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_MASTER_SNOWDRIFT, FAIL);
                    instance->SetData(DATA_MASTER_SNOWDRIFT, FAIL);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_EVENT:
                        nonCombatEvents.ScheduleEvent(EVENT_INTRO_1, 1 * IN_MILLISECONDS);
                        break;
                    case ACTION_NOVOCE_DEFEAT:
                        Talk(TALK_STUDENTS);
                        break;
                    case ACTION_TALK_WAVE_1:
                        Talk(TALK_WAWE1_END);
                        break;
                    case ACTION_TALK_WAVE_1_S:
                        Talk(TALK_WAWE1_START);
                        break;
                    case ACTION_TALK_OUTRO:
                        Talk(TALK_WAWE2_END);
                        nonCombatEvents.ScheduleEvent(EVENT_OUTRO_1, 6.35 * IN_MILLISECONDS);
                        break;
                    case ACTION_TALK_WAVE_2:
                        Talk(TALK_WAWE2_START);
                        break;
                    case ACTION_DISSAPEAR:
                        ++hurlChiCnt;
                        isBossSummoned = false;
                        if (hurlChiCnt < 3)
                        {
                            me->SetVisible(false);
                            me->CastSpell(me, SPELL_SMOKE_BOMB, true);   
                            summons.DespawnEntry(NPC_SNOWDRIFT_CLONE);
                            nonCombatEvents.ScheduleEvent(EVENT_DISAPPEAR, 2 * IN_MILLISECONDS);

                            if (instance)
                                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                        }
                        else
                        {
                            phase = PHASE_FIGHT_3;

                            Talk(TALK_PHASE_3);        
                            Talk(TALK_P_3_EMOTE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DoZoneInCombat();
                            me->setRegeneratingHealth(true);                  
                            events.ScheduleEvent(urand(EVENT_PARRY_STANCE, EVENT_TORNADO_SLAM), urand(12, 15) * IN_MILLISECONDS);
                            nonCombatEvents.Reset();

                            std::list<Creature*> clonesList;
                            me->GetCreatureListWithEntryInGrid(clonesList, NPC_SNOWDRIFT_CLONE, 150.0f);

                            for (auto&& clone : clonesList)
                            {
                                if (clone->IsAIEnabled)
                                    clone->AI()->DoAction(0); // ACTION_BALL_OF_FIRE
                            }

                            clonesList.clear();
                        }
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (phase == PHASE_FIGHT_4)
                {
                    damage = 0;
                    return;
                }

                if (HealthBelowPct(51) && phase == PHASE_FIGHT_1)
                {
                    phase = PHASE_FIGHT_2;
                    Talk(TALK_PHASE_2);
                    events.Reset();
                    me->StopMoving();
                    me->CombatStop(true);
                    me->setRegeneratingHealth(false);
                    me->SetReactState(REACT_PASSIVE);
                    nonCombatEvents.ScheduleEvent(EVENT_PHASE_2, 7 * IN_MILLISECONDS);
                }

                if (damage >= me->GetHealth() && phase != PHASE_FIGHT_4)
                {
                    damage = 0;
                    phase = PHASE_FIGHT_4;
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

                    if (!isFightWon)
                    {
                        isFightWon = true;

                        _JustDied();
                        Talk(TALK_DEATH);
                        me->StopMoving();
                        me->SetFaction(35);
                        me->RemoveAllAuras();
                        me->CombatStop(true);
                        me->DeleteThreatList();
                        me->SetReactState(REACT_PASSIVE);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        events.Reset();
                        nonCombatEvents.Reset();

                        if (instance)
                        {
                            instance->SetData(DATA_MASTER_SNOWDRIFT, DONE);
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                            if (!instance->instance->IsChallengeDungeon())
                                for (auto&& itr : instance->instance->GetPlayers())
                                    itr.GetSource()->ModifyCurrency(395, DUNGEON_MODE(70, 125) * CURRENCY_PRECISION);
                        }
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                    nonCombatEvents.ScheduleEvent(EVENT_OUTRO_END, me->GetSplineDuration());
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_POSSESIONS:
                            if (auto const possesions = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_POSSESSIONS)))
                                possesions->RemoveFromWorld();
                            break;
                        case EVENT_INTRO_1:
                            Talk(TALK_INTRO_1);
                            nonCombatEvents.ScheduleEvent(EVENT_INTRO_2, 7.4 * IN_MILLISECONDS);
                            break;
                        case EVENT_INTRO_2:
                            Talk(TALK_INTRO_2);
                            nonCombatEvents.ScheduleEvent(EVENT_INTRO_3, 10.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_INTRO_3:
                            if (instance)
                                if (Creature* referee = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_PANDAREN_REFEREE)))
                                    referee->AI()->DoAction(ACTION_START_EVENT);
                            break;
                        case EVENT_OUTRO_1:
                            Talk(TALK_OUTRO_1);
                            nonCombatEvents.ScheduleEvent(EVENT_OUTRO_2, 7.3 * IN_MILLISECONDS);
                            break;
                        case EVENT_OUTRO_2:
                            {
                                if (instance)
                                    instance->SetData(DATA_SNOWDRIFT_STATE, DONE);

                                Talk(TALK_OUTRO_2);
                                me->GetMotionMaster()->MoveCharge(SnowdriftPos[1].GetPositionX(), SnowdriftPos[1].GetPositionY(), SnowdriftPos[1].GetPositionZ(), 25.0f, EVENT_CHARGE);
                            }
                            break;
                        case EVENT_OUTRO_END:
                            me->SetFacingTo(4.114f);
                            me->SetReactState(REACT_AGGRESSIVE);                       
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            if (instance)
                            {
                                instance->SetBossState(DATA_MASTER_SNOWDRIFT, NOT_STARTED);
                                instance->SetData(DATA_MASTER_SNOWDRIFT, NOT_STARTED);
                            }
                            break;
                        case EVENT_DISAPPEAR:
                            {
                               srand(time(0));
                               std::random_device rd;
                               std::mt19937 g(rd());
                               std::shuffle(randIndex, randIndex + 3, g);                               
                               for (int i = 0; i < 3; ++i)
                               {
                                   if (!isBossSummoned)
                                   {
                                       isBossSummoned = true;
                                       me->NearTeleportTo(ClonePos[randIndex[i]].GetPositionX(), ClonePos[randIndex[i]].GetPositionY(), ClonePos[randIndex[i]].GetPositionZ(), ClonePos[randIndex[i]].GetOrientation());
                                       me->SetVisible(true);
                                       me->CastSpell(me, SPELL_HURL_CHI, true);
                                       me->CastSpell(me, SPELL_SMOKE_BOMB, true);

                                       if (instance)
                                           instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                                   }
                                   else
                                   {
                                       if (auto const clone = me->SummonCreature(NPC_SNOWDRIFT_CLONE, ClonePos[randIndex[i]].GetPositionX(), ClonePos[randIndex[i]].GetPositionY(), ClonePos[randIndex[i]].GetPositionZ(), ClonePos[randIndex[i]].GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN))
                                       {
                                           clone->SetReactState(REACT_PASSIVE);
                                           clone->CastSpell(clone, SPELL_SMOKE_BOMB, true);
                                       }
                                   }
                               }
                            }
                            break;
                        case EVENT_CHECK_WIPE:
                            if (instance)
                            {
                                if (instance->IsWipe())
                                    EnterEvadeMode();
                                else
                                    nonCombatEvents.ScheduleEvent(EVENT_CHECK_WIPE, 0.5 * IN_MILLISECONDS);
                            }
                            break;
                        case EVENT_PHASE_2:
                            Talk(TALK_P_2_EMOTE);
                            me->CastSpell(me, SPELL_SMOKE_BOMB, true);
                            me->SetVisible(false);
                            events.Reset();
                            nonCombatEvents.ScheduleEvent(EVENT_DISAPPEAR, 2 * IN_MILLISECONDS);
                            nonCombatEvents.ScheduleEvent(EVENT_CHECK_WIPE, 3 * IN_MILLISECONDS);
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
                        case EVENT_TORNADO_KICK:
                            Talk(TALK_TORNADO);
                            DoCast(me, SPELL_TORNADO_KICK);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_TORNADO_KICK, 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_FIST_OF_FURY:
                            Talk(TALK_FISTS);
                            if (Unit* victim = me->GetVictim())
                            {
                                targetGUID = victim->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(victim), SPELL_FIST_OF_FURY);
                            }
                            events.ScheduleEvent(EVENT_FIST_OF_FURY, 15 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_FIST_END, 6500);
                            break;
                        case EVENT_PARRY_STANCE:
                            Talk(TALK_PARRY);
                            me->CastSpell((Unit*)NULL, SPELL_PARRY_STANCE, false);
                            events.ScheduleEvent(EVENT_TORNADO_SLAM, urand(12, 15) * IN_MILLISECONDS);
                            break;
                        case EVENT_TORNADO_SLAM:
                            if (auto const target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                                me->CastSpell(target, SPELL_CHASE_DOWN, false);
                            me->SetReactState(REACT_PASSIVE);
                            events.ScheduleEvent(EVENT_CHECK_DEST, 0.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_CHECK_DEST:
                            if (me->GetVictim() && me->GetVictim()->HasAuraTypeWithCaster(SPELL_AURA_FIXATE_TARGET, me->GetGUID())) // SPELL_AURA_FIXATE_TARGET
                            {
                                if (me->GetDistance2d(me->GetVictim()) <= 1.5f)
                                {
                                    me->CastSpell(me->GetVictim(), SPELL_TORNADO_SLAM, false);
                                    me->SetReactState(REACT_AGGRESSIVE);
                                    events.ScheduleEvent(EVENT_PARRY_STANCE, urand(13, 15) * IN_MILLISECONDS);
                                }
                                else
                                    events.ScheduleEvent(EVENT_CHECK_DEST, 0.5 * IN_MILLISECONDS);
                            }
                            else
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                events.ScheduleEvent(EVENT_PARRY_STANCE, urand(13, 15) * IN_MILLISECONDS);
                            }
                            break;
                        case EVENT_FIST_END:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                    }
                }

                if (phase != PHASE_FIGHT_2)
                {
                    DoMeleeAttackIfReady();
                    EnterEvadeIfOutOfCombatArea(diff, 41.0f);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_master_snowdriftAI(creature);
        }
};

class npc_snowdrift_clone : public CreatureScript
{
    public:
        npc_snowdrift_clone() : CreatureScript("npc_snowdrift_clone") { }

        enum eSpells
        {
            SPELL_COPY_HEALTH  = 122074,
            SPELL_BALL_OF_FIRE = 113760
        };

        enum eEvents
        {
            EVENT_BALL_OF_FIRE = 1
        };

        enum eActions
        {
            ACTION_BALL_OF_FIRE = 0
        };

        struct npc_snowdrift_cloneAI : public ScriptedAI
        {
            npc_snowdrift_cloneAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                ASSERT(me->IsSummon());
                me->SetReactState(REACT_PASSIVE);
                me->setRegeneratingHealth(false);
                me->CastSpell((Unit*)NULL, SPELL_COPY_HEALTH, false);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_BALL_OF_FIRE)
                    events.ScheduleEvent(EVENT_BALL_OF_FIRE, 1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_BALL_OF_FIRE)
                {
                    me->CastSpell(me, SPELL_BALL_OF_FIRE, false);
                    events.ScheduleEvent(EVENT_BALL_OF_FIRE, 2 * IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_snowdrift_cloneAI(creature);
        }
};

Position InitiateSpawnPos[5] =
{
    {3708.56f, 3039.60f, 816.28f},
    {3699.19f, 3049.62f, 816.28f},
    {3688.03f, 3055.68f, 816.28f},
    {3675.10f, 3066.98f, 816.28f},
    {3668.95f, 3070.81f, 816.28f}
};

Position MinibossSpawnPos[2] =
{
    {3683.72f, 3053.94f, 816.28f},
    {3687.77f, 3051.30f, 816.28f}
};

class npc_spm_pandaren_referee : public CreatureScript
{
    public:
        npc_spm_pandaren_referee() : CreatureScript("npc_spm_pandaren_referee") { }

        enum eActions
        {
            ACTION_START_EVENT   = 0,
            ACTION_NOVICE_DONE   = 1,
            ACTION_MINIBOSS_DONE = 2,
            ACTION_NOVICE_DEFEAT = 3
        };

        enum eEvents
        {
            EVENT_INIT_EVENT   = 1,
            EVENT_CHECK_WIPE   = 2,
            EVENT_1_WAVE_END   = 3,
            EVENT_SUMMON_NOVIE = 4
        };

        struct npc_spm_pandaren_refereeAI : public ScriptedAI
        {
            npc_spm_pandaren_refereeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            uint8 eventPhase, novoiceSummoned;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();

                eventPhase = 0;
                novoiceSummoned = 1;

                if (instance && instance->GetData(DATA_SNOWDRIFT_STATE) != DONE)
                {
                    summons.DespawnAll();
                    if (instance)
                        instance->SetData(DATA_SNOWDRIFT_STATE, NOT_STARTED);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void DoEvent(uint8 eventId)
            {
                ++eventPhase;
                switch (eventPhase)
                {
                    case 1:
                    {
                        if (instance)
                            if (Creature* snowdrift = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MASTER_SNOWDRIFT)))
                                snowdrift->AI()->DoAction(5); //ACTION_TALK_WAVE_1_S

                        for (uint8 i = 0; i < 5; i++)
                        {
                            ++novoiceSummoned;
                            me->SummonCreature(NPC_NOVICE, InitiateSpawnPos[i].GetPositionX(), InitiateSpawnPos[i].GetPositionY(), InitiateSpawnPos[i].GetPositionZ());
                        }
                        events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                        break;
                    }
                    case 2:
                        if (instance)
                            if (auto const snowdrift = Unit::GetCreature(*me, instance->GetGuidData(DATA_MASTER_SNOWDRIFT)))
                                if (snowdrift->IsAIEnabled)
                                    snowdrift->AI()->DoAction(3); //ACTION_TALK_WAVE_2

                        me->SummonCreature(NPC_FLAGRANT_LOTUS, MinibossSpawnPos[0].GetPositionX(), MinibossSpawnPos[0].GetPositionY(), MinibossSpawnPos[0].GetPositionZ());
                        me->SummonCreature(NPC_FLYING_SNOW, MinibossSpawnPos[1].GetPositionX(), MinibossSpawnPos[1].GetPositionY(), MinibossSpawnPos[1].GetPositionZ());
                        break;
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                { 
                    case ACTION_START_EVENT:
                        DoEvent(eventPhase);
                        break;
                    case ACTION_NOVICE_DONE:
                        events.Reset();
                        events.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_1_WAVE_END, 8 * IN_MILLISECONDS);
                        if (instance)
                        {
                            if (auto const snowdrift = Unit::GetCreature(*me, instance->GetGuidData(DATA_MASTER_SNOWDRIFT)))
                               if (snowdrift->IsAIEnabled)
                                   snowdrift->AI()->DoAction(2); //ACTION_TALK_WAVE_1
                        }
                        break;
                    case ACTION_MINIBOSS_DONE:
                        if (instance)
                        {
                            events.Reset();

                            if (auto const snowdrift = Unit::GetCreature(*me, instance->GetGuidData(DATA_MASTER_SNOWDRIFT)))
                               if (snowdrift->IsAIEnabled)
                                   snowdrift->AI()->DoAction(4); //ACTION_TALK_OUTRO
                        }
                        break;
                    case ACTION_NOVICE_DEFEAT:
                        if (novoiceSummoned <= MAX_NOVICE)
                        {
                            ++novoiceSummoned;
                            events.ScheduleEvent(EVENT_SUMMON_NOVIE, urand(1, 3) * IN_MILLISECONDS);

                            switch (novoiceSummoned)
                            {
                                case 10:
                                case 15:
                                case 20:
                                    if (auto const snowdrift = Unit::GetCreature(*me, instance->GetGuidData(DATA_MASTER_SNOWDRIFT)))
                                        if (snowdrift->IsAIEnabled)
                                             snowdrift->AI()->DoAction(1); //ACTION_NOVOCE_DEFEAT
                                break;
                            }
                        }
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
                        case EVENT_INIT_EVENT:
                            DoEvent(eventPhase);
                            break;
                        case EVENT_CHECK_WIPE:
                            if (instance && instance->IsWipe())
                            {
                                eventPhase = 0;
                                novoiceSummoned = 1;
                                events.Reset();
                                summons.DespawnAll();
                                instance->SetData(DATA_SNOWDRIFT_STATE, FAIL);
                            }
                            else
                                events.ScheduleEvent(EVENT_CHECK_WIPE, 0.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_1_WAVE_END:
                            {
                                std::list<Creature*> noviceList;
                                GetCreatureListWithEntryInGrid(noviceList, me, NPC_NOVICE, 100.0f);

                                for (auto&& novice : noviceList)
                                {
                                    if (novice->IsAIEnabled)
                                        novice->AI()->DoAction(0);
                                }

                                noviceList.clear();
                            }
                            events.ScheduleEvent(EVENT_INIT_EVENT, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_SUMMON_NOVIE:
                             me->SummonCreature(NPC_NOVICE, InitiateSpawnPos[irand(0, 4)].GetPositionX(), InitiateSpawnPos[irand(0, 4)].GetPositionY(), InitiateSpawnPos[irand(0, 4)].GetPositionZ());
                            break;
                    }
                }
            }

        private:
            EventMap events;
            InstanceScript* instance;
            SummonList summons;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spm_pandaren_refereeAI(creature);
        }
};

class npc_snowdrift_novice : public CreatureScript
{
    public:
        npc_snowdrift_novice() : CreatureScript("npc_snowdrift_novice") { }

        enum eEvents
        {
            EVENT_FIGHT_DONE     = 1,
            EVENT_KNEEL          = 2,
            EVENT_ATTACK         = 3,
            EVENT_FLYING_KICK    = 4,
            EVENT_HUNDRED_STRIKE = 5,
            EVENT_PALM_STRIKE    = 6,
            EVENT_PARRY_STANCE   = 7
        };

        enum eActions
        {
            ACTION_JUMP = 0
        };

        enum eSpells
        {
            SPELL_JUMP           = 106046,
            SPELL_FLYING_KICK    = 106439,
            SPELL_HUNDRED_STRIKE = 106428,
            SPELL_PALM_STRIKE    = 106421,
            SPELL_PARRY_STANCE   = 106447
        };

        struct npc_snowdrift_noviceAI : public ScriptedAI
        {
            npc_snowdrift_noviceAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;
            bool stillInFight;
            bool hasReward;
            bool allowReward;
            
            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                ASSERT(me->IsSummon());
                stillInFight = true;
                me->CastSpell(Trinity::Containers::SelectRandomContainerElement(m_NovicePos), SPELL_JUMP, false);
                events.ScheduleEvent(EVENT_ATTACK, me->GetSplineDuration());

                hasReward = false;
                allowReward = false;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_JUMP)
                {
                    events.Reset();
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    if (instance)
                    {
                        if (auto const targetGUID = instance->GetGuidData(DATA_RANDOM_SECOND_POS))
                        {
                            if (auto const target = ObjectAccessor::GetUnit(*me, targetGUID))
                            {
                                me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);
                                me->CastSpell(target, SPELL_JUMP, false);
                                events.ScheduleEvent(EVENT_KNEEL, me->GetSplineDuration() + 3.5f);
                            }
                        }
                    }
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (!stillInFight)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    stillInFight = false;
                    me->StopMoving();
                    me->SetFaction(35);
                    me->RemoveAllAuras();
                    me->CombatStop(true);
                    me->DeleteThreatList();
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    allowReward = true;

                    events.Reset();
                    events.ScheduleEvent(EVENT_JUMP, 2 * IN_MILLISECONDS);

                    if (auto const referee = Unit::GetCreature(*me, instance->GetGuidData(DATA_PANDAREN_REFEREE)))
                       if (referee->IsAIEnabled)
                            referee->AI()->DoAction(3); // ACTION_NOVICE_DEFEAT

                    if (instance)
                        instance->SetData(DATA_DEFEATED_NOVICE, 1);
                }
            }

            void ReceiveEmote(Player* player, uint32 uiTextEmote) override
            {
                if (uiTextEmote == TEXT_EMOTE_BOW && !hasReward && allowReward)
                {
                    hasReward = true;
                    DoCast(me, SPELL_RESPECT);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ATTACK:
                            DoZoneInCombat();
                            events.ScheduleEvent(EVENT_FLYING_KICK, urand(6, 8) * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_HUNDRED_STRIKE, urand(4, 6) * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_PALM_STRIKE, urand(7, 9) * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_PARRY_STANCE, urand(12, 14) * IN_MILLISECONDS);
                            break;
                        case EVENT_KNEEL:
                            me->SetFacingTo(me->GetAngle(3659.212f, 3015.495f));
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            break;
                        case EVENT_JUMP:
                            if (instance)
                            {
                                if (auto const targetGUID = instance->GetGuidData(DATA_RANDOM_FIRST_POS))
                                {
                                    if (auto const target = ObjectAccessor::GetUnit(*me, targetGUID))
                                    {
                                        me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);
                                        me->CastSpell(target, SPELL_JUMP, false);
                                        events.ScheduleEvent(EVENT_KNEEL, me->GetSplineDuration() + 2.0f);
                                    }
                                }
                            }
                            allowReward = false;
                            break;
                        case EVENT_FLYING_KICK:
                            me->CastSpell((Unit*)NULL, SPELL_FLYING_KICK, false);
                            events.ScheduleEvent(EVENT_FLYING_KICK, urand(12, 15) * IN_MILLISECONDS);
                            break;
                        case EVENT_HUNDRED_STRIKE:
                            me->CastSpell(me, SPELL_HUNDRED_STRIKE, false);
                            events.ScheduleEvent(EVENT_HUNDRED_STRIKE, urand(8, 12) * IN_MILLISECONDS);
                            break;
                        case EVENT_PALM_STRIKE:
                            me->CastSpell((Unit*)NULL, SPELL_PALM_STRIKE, false);
                            events.ScheduleEvent(EVENT_PALM_STRIKE, urand(16, 21) * IN_MILLISECONDS);
                            break;
                        case EVENT_PARRY_STANCE:
                            me->CastSpell((Unit*)NULL, SPELL_PARRY_STANCE, false);
                            events.ScheduleEvent(EVENT_PARRY_STANCE, urand(15, 19) * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_snowdrift_noviceAI(creature);
        }
};

class npc_snowdrift_miniboss : public CreatureScript
{
    public:
        npc_snowdrift_miniboss() : CreatureScript("npc_snowdrift_miniboss") { }

        enum eEvents
        {
            EVENT_MOVE_AWAY             = 1,
            EVENT_BALL_OF_FIRE          = 2,
            EVENT_FLYING_KICK           = 3,
            EVENT_CALL_STAFF            = 4,
            EVENT_RELEASE_STAFF         = 5,
            EVENT_WHIRLING_STEEL        = 6,
            EVENT_SUMMON_STAFF          = 7,
            EVENT_KNEEL                 = 8
        };

        enum eSpells
        {
            SPELL_EJECT_PASSENGERS = 68576,
            SPELL_WHIRLING_STEEL   = 106646,
            SPELL_JUMP             = 106046,
            SPELL_JADE_STAFF_PRO   = 106620,
            SPELL_BALL_OF_FIRE     = 113760,
            SPELL_FLYING_KICK      = 106439,
            SPELL_CALL_STAFF       = 106681,
            SPELL_RELEASE_STAFF    = 106680
        };

        enum eTalks
        {
            TALK_WHIRLING_STEEL = 0
        };

        struct npc_snowdrift_minibossAI : public ScriptedAI
        {
            npc_snowdrift_minibossAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            SummonList summons;
            EventMap events, nonCombatEvents;
            InstanceScript* instance;       
            bool stillInFight;

            void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
            {
                if (who->GetTypeId() != TYPEID_UNIT)
                    return;

                if (apply)
                {
                    who->ToCreature()->SetReactState(REACT_PASSIVE);
                    who->CombatStop(true);
                    who->AddUnitState(UNIT_STATE_ROOT);
                    who->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    who->CastSpell(me, SPELL_JADE_STAFF_PRO, false);

                    if (who->IsAIEnabled)
                        who->GetAI()->DoAction(2);
                }
                else
                {
                    who->ToCreature()->SetReactState(REACT_AGGRESSIVE);
                    who->ClearUnitState(UNIT_STATE_ROOT);
                    who->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->RemoveAura(SPELL_JADE_STAFF_PRO);

                    if (who->IsAIEnabled)
                        who->GetAI()->DoAction(1);
                }
            }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                ASSERT(me->IsSummon());
                stillInFight = true;
                me->CastSpell(Trinity::Containers::SelectRandomContainerElement(m_NovicePos), SPELL_JUMP, false);
                DoZoneInCombat();

                switch (me->GetEntry())
                {
                    case NPC_FLAGRANT_LOTUS:
                        nonCombatEvents.ScheduleEvent(EVENT_SUMMON_STAFF, 0.5 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BALL_OF_FIRE, urand(2, 4) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_FLYING_KICK, urand(3, 6) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_RELEASE_STAFF, urand(8, 12) * IN_MILLISECONDS);
                        break;
                    case NPC_FLYING_SNOW:
                        events.ScheduleEvent(EVENT_WHIRLING_STEEL, urand(4, 6) * IN_MILLISECONDS);
                        break;
                }
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (!stillInFight)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    stillInFight = false;
                    me->StopMoving();
                    me->SetFaction(35);
                    me->RemoveAllAuras();
                    me->CombatStop(true);
                    me->DeleteThreatList();
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);

                    events.Reset();
                    nonCombatEvents.ScheduleEvent(EVENT_MOVE_AWAY, 2 * IN_MILLISECONDS);

                    if (instance)
                        instance->SetData(DATA_DEFEATED_MINIBOSS, 1);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_AWAY:
                            {
                               if (ObjectGuid targetGUID = instance->GetGuidData(DATA_RANDOM_MINIBOSS_POS))
                               {
                                   if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                                   {
                                       me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_CANNOT_TURN);
                                       me->CastSpell(target, SPELL_JUMP, false);
                                       nonCombatEvents.ScheduleEvent(EVENT_KNEEL, me->GetSplineDuration() + 2.0f);
                                   }
                               }
                            }
                            break;
                        case EVENT_SUMMON_STAFF:
                            if (Creature* staff = me->SummonCreature(NPC_LOTUS_STAFF, *me))
                                staff->EnterVehicle(me);
                            break;
                        case EVENT_KNEEL:
                            me->SetFacingTo(me->GetAngle(3659.212f, 3015.495f));
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            break;
                    }
                }

                events.Update(diff);
                nonCombatEvents.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BALL_OF_FIRE:
                            if (auto const target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                                me->CastSpell(target, SPELL_BALL_OF_FIRE, false);
                            events.ScheduleEvent(EVENT_BALL_OF_FIRE, urand(16,24) * IN_MILLISECONDS);
                            break;
                        case EVENT_FLYING_KICK:
                            if (auto const target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                                me->CastSpell(target, SPELL_FLYING_KICK, false);
                            events.ScheduleEvent(EVENT_FLYING_KICK, urand(12,16) * IN_MILLISECONDS);
                            break;
                        case EVENT_CALL_STAFF:
                            me->CastSpell(me, SPELL_CALL_STAFF, false);
                            events.ScheduleEvent(EVENT_RELEASE_STAFF, urand(18,22) * IN_MILLISECONDS);
                            break;
                        case EVENT_RELEASE_STAFF:
                            me->CastSpell(me, SPELL_EJECT_PASSENGERS, TRIGGERED_FULL_MASK);
                            events.ScheduleEvent(EVENT_CALL_STAFF, urand(8,12) * IN_MILLISECONDS);
                            break;
                        case EVENT_WHIRLING_STEEL:
                            Talk(TALK_WHIRLING_STEEL);
                            me->CastSpell(me, SPELL_WHIRLING_STEEL, TRIGGERED_FULL_MASK);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_WHIRLING_STEEL, urand(20, 24) * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_snowdrift_minibossAI(creature);
        }
};

class npc_spm_jade_staff : public CreatureScript
{
    public:
        npc_spm_jade_staff() : CreatureScript("npc_spm_jade_staff") { }

        enum eActions
        {
            ACTION_START_COMBAT = 1,
            ACTION_STOP_COMBAT  = 2
        };

        enum eEvents
        {
            EVENT_STAFF_STRIKE = 1
        };

        enum eSpells
        {
            SPELL_STAFF_STRIKE = 106659
        };

        struct npc_spm_jade_staffAI : public ScriptedAI
        {
            npc_spm_jade_staffAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_COMBAT:
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 1); //UNIT_FIELD_BYTES_2
                        events.ScheduleEvent(EVENT_STAFF_STRIKE, urand(4, 6) * IN_MILLISECONDS);
                        break;
                    case ACTION_STOP_COMBAT:
                        events.Reset();
                        me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0); //UNIT_FIELD_BYTES_2
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_STAFF_STRIKE)
                {
                    me->CastSpell(me->GetVictim(), SPELL_STAFF_STRIKE, false);
                    events.ScheduleEvent(EVENT_STAFF_STRIKE, urand(6, 8) * IN_MILLISECONDS);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spm_jade_staffAI(creature);
        }
};

class npc_snowdrift_fireball : public CreatureScript
{
    public:
        npc_snowdrift_fireball() : CreatureScript("npc_snowdrift_fireball") { }

        enum eSpells
        {
            SPELL_BALL_OF_FIRE_BLUE = 106534,
            SPELL_BALL_OF_FIRE_RED  = 106535 
        };

        enum eEvents
        {
            EVENT_INITIALIZE = 1
        };

        struct npc_snowdrift_fireballAI : public ScriptedAI
        {
            npc_snowdrift_fireballAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                events.ScheduleEvent(EVENT_INITIALIZE, 0.1 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_INITIALIZE)
                {
                    me->SetReactState(REACT_PASSIVE);
                    float x, y;
                    GetPositionWithDistInOrientation(me, 100.0f, me->GetOrientation(), x, y);
                    me->CastSpell(me, urand(SPELL_BALL_OF_FIRE_BLUE, SPELL_BALL_OF_FIRE_RED), false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_snowdrift_fireballAI(creature);
        }
};

class AreaTrigger_at_before_snowdrift : public AreaTriggerScript
{
    public:
        AreaTrigger_at_before_snowdrift() : AreaTriggerScript("AreaTrigger_at_before_snowdrift") { }

        enum eActions
        {
            ACTION_START_EVENT = 0
        };

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (auto const instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_SNOWDRIFT_STATE) != IN_PROGRESS && instance->GetData(DATA_SNOWDRIFT_STATE) != DONE)
                {
                    instance->SetData(DATA_SNOWDRIFT_STATE, IN_PROGRESS);
                    if (auto const snowdrift = Unit::GetCreature(*player, instance->GetGuidData(DATA_MASTER_SNOWDRIFT)))
                        if (snowdrift->IsAIEnabled)
                            snowdrift->AI()->DoAction(ACTION_START_EVENT);
                }
            }

            return false;
        }
};

class spell_spm_swirling_steel : public SpellScriptLoader
{
    public:
        spell_spm_swirling_steel() : SpellScriptLoader("spell_spm_swirling_steel") { }

        SpellScript* GetSpellScript() const override
        {
            return new spell_spm_swirling_steel_SpellScript();
        }

        class spell_spm_swirling_steel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spm_swirling_steel_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!targets.empty())
                    Trinity::Containers::RandomResizeList(targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spm_swirling_steel_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };
};

class BallOfFireTargetSelector
{
    public:
        bool operator()(WorldObject* object) const
        {
            if (object->GetEntry() == NPC_SNOWDRIFT_CLONE)
                return false;

            return true;
        }
};

class spell_snowdrift_ball_of_fire : public SpellScriptLoader
{
    public:
        spell_snowdrift_ball_of_fire() : SpellScriptLoader("spell_snowdrift_ball_of_fire") { }

        enum eSpells
        {
            SPELL_BALL_OF_FIRE = 113760
        };

        class spell_snowdrift_ball_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_snowdrift_ball_of_fire_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (BallOfFireTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_snowdrift_ball_of_fire_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        class spell_snowdrift_ball_of_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snowdrift_ball_of_fire_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto const owner = GetOwner()->ToCreature();

                if (owner)
                {
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, owner, 150.0f);
                    if (!playerList.empty())
                    {
                        for (std::list<Player*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                        {
                            if (auto const player = (*itr)->ToPlayer())
                            {
                                if (player->isInFront(owner, M_PI))
                                {
                                    owner->SetFacingToObject(player);
                                    owner->SetOrientation(owner->GetAngle(player));
                                    break;
                                }                            
                            }
                        }
                    }

                    owner->CastSpell(owner, SPELL_BALL_OF_FIRE, true);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_snowdrift_ball_of_fire_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_snowdrift_ball_of_fire_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_snowdrift_ball_of_fire_AuraScript();
        }
};

class spell_snowdrift_ball_of_fire_aura : public SpellScriptLoader
{
    public:
        spell_snowdrift_ball_of_fire_aura() : SpellScriptLoader("spell_snowdrift_ball_of_fire_aura") { }

        enum eSpells
        {
            SPELL_BALL_OF_FIRE = 113760
        };

        class spell_snowdrift_ball_of_fire_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snowdrift_ball_of_fire_aura_AuraScript);

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                auto const owner = GetOwner()->ToCreature();

                if (owner)
                {
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, owner, 150.0f);
                    if (!playerList.empty())
                    {
                        for (std::list<Player*>::iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                        {
                            if (auto const player = (*itr)->ToPlayer())
                            {
                                if (player->isInFront(owner, M_PI))
                                {
                                    owner->SetFacingToObject(player);
                                    owner->SetOrientation(owner->GetAngle(player));
                                    break;
                                }
                            }
                        }
                    }

                    owner->CastSpell(owner, SPELL_BALL_OF_FIRE, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_snowdrift_ball_of_fire_aura_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_snowdrift_ball_of_fire_aura_AuraScript();
        }
};

class spell_snowdrift_hurl_chi : public SpellScriptLoader
{
    public:
        spell_snowdrift_hurl_chi() : SpellScriptLoader("spell_snowdrift_hurl_chi") { }

        class spell_snowdrift_hurl_chi_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snowdrift_hurl_chi_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (auto const caster = GetCaster())
                   if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                       if (caster->IsAIEnabled)
                           caster->GetAI()->DoAction(6);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_snowdrift_hurl_chi_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_snowdrift_hurl_chi_AuraScript();
        }
};

class spell_snowdrift_tornado_slam : public SpellScriptLoader
{
    public:
        spell_snowdrift_tornado_slam() : SpellScriptLoader("spell_snowdrift_tornado_slam") { }

        enum eSpells
        {
            SPELL_TORNADO_SLAM_EFFECT = 106345,
            SPELL_CHASE_DOWN          = 118961,
        };

        class spell_snowdrift_tornado_slam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_snowdrift_tornado_slam_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                auto const target = GetHitUnit();
                auto const caster = GetCaster();
                if (!target || !caster)
                    return;

                caster->RemoveAura(SPELL_CHASE_DOWN);
                target->CastSpell((Unit*)NULL, SPELL_TORNADO_SLAM_EFFECT, false);
                target->AddAura(SPELL_ROOT_FOR_EVER, target);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_snowdrift_tornado_slam_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_snowdrift_tornado_slam_SpellScript();
        }
};

class spell_snowdrift_tornado_slam_effect : public SpellScriptLoader
{
    public:
        spell_snowdrift_tornado_slam_effect() : SpellScriptLoader("spell_snowdrift_tornado_slam_effect") { }

        enum eSpells
        {
            SPELL_EJECT_PASS = 68576
        };

        class spell_snowdrift_tornado_slam_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_snowdrift_tornado_slam_effect_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                auto const caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, SPELL_EJECT_PASS, TRIGGERED_FULL_MASK);
                caster->RemoveAura(SPELL_ROOT_FOR_EVER);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_snowdrift_tornado_slam_effect_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_snowdrift_tornado_slam_effect_SpellScript();
        }
};

class spell_snowdrift_parry_stance : public SpellScriptLoader
{
    public:
        spell_snowdrift_parry_stance() : SpellScriptLoader("spell_snowdrift_parry_stance") { }

        enum eSpells
        {
            SPELL_FLYING_KICK    = 106439,
            SPELL_QUIVERING_PALM = 106422
        };

        class spell_snowdrift_parry_stance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snowdrift_parry_stance_AuraScript);

            void HanleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                auto const caster = eventInfo.GetActionTarget();
                auto const target = eventInfo.GetActor();
                if (!caster || !target)
                    return;

                if (auto const aura = caster->GetAura(aurEff->GetId()))
                {
                    if (aura->GetDuration() > 1.5 * IN_MILLISECONDS)
                        caster->CastSpell(target, SPELL_QUIVERING_PALM, true);
                    else
                        caster->CastSpell(target, SPELL_FLYING_KICK, true);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_snowdrift_parry_stance_AuraScript::HanleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };
         
        AuraScript* GetAuraScript() const override
        {
            return new spell_snowdrift_parry_stance_AuraScript();
        }
};

class spell_snowdrift_copy_health : public SpellScriptLoader
{
    public:
        spell_snowdrift_copy_health() : SpellScriptLoader("spell_snowdrift_copy_health") { }

        class spell_snowdrift_copy_health_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_snowdrift_copy_health_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                auto const target = GetHitUnit();
                auto const caster = GetCaster();
                if (!target || !caster)
                    return;

                caster->SetHealth(caster->CountPctFromMaxHealth(target->GetHealthPct()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_snowdrift_copy_health_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_snowdrift_copy_health_SpellScript();
        }
};

class BallOfFireDamageTargetSelector
{
    public:
        explicit BallOfFireDamageTargetSelector(Unit* _caster) : caster(_caster) { }

        bool operator() (WorldObject* unit) const
        {
            if (unit->GetDistance(caster) <= 0.5 && unit->GetTypeId() == TYPEID_PLAYER && !unit->ToPlayer()->HasAura(SPELL_BALL_OF_FIRE_AURA_BLUE) && !unit->ToPlayer()->HasAura(SPELL_BALL_OF_FIRE_AURA_RED))
                return false;

            return true;
        }

    private:
        Unit* caster;

        enum eSpells
        {
            SPELL_BALL_OF_FIRE_AURA_BLUE = 115324,
            SPELL_BALL_OF_FIRE_AURA_RED  = 115325
        };
};

class spell_snowdrift_ball_of_fire_damage : public SpellScriptLoader
{
    public:
        spell_snowdrift_ball_of_fire_damage() : SpellScriptLoader("spell_snowdrift_ball_of_fire_damage") { }

        class spell_snowdrift_ball_of_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_snowdrift_ball_of_fire_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (BallOfFireDamageTargetSelector(GetCaster()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_snowdrift_ball_of_fire_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_snowdrift_ball_of_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snowdrift_ball_of_fire_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                auto const caster = GetCaster();
                auto const owner = GetOwner()->ToUnit();
                if (!caster || !owner)
                    return;

                caster->CastSpell(owner, aurEff->GetSpellInfo()->Effects[0].BasePoints, TRIGGERED_FULL_MASK);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_snowdrift_ball_of_fire_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_snowdrift_ball_of_fire_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_snowdrift_ball_of_fire_AuraScript();
        }
};

class spell_snowdrift_fists_of_fury : public SpellScriptLoader
{
    public:
        spell_snowdrift_fists_of_fury() : SpellScriptLoader("spell_snowdrift_fists_of_fury") { }

        class spell_snowdrift_fists_of_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_snowdrift_fists_of_fury_SpellScript);

            void HandleBeforeCast()
            {
                auto const caster = GetCaster();
                if (!caster)
                    return;

                if (caster->ToCreature())
                    caster->ToCreature()->SetReactState(REACT_PASSIVE);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_snowdrift_fists_of_fury_SpellScript::HandleBeforeCast);
            }
        };

        class spell_snowdrift_fists_of_fury_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snowdrift_fists_of_fury_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto const owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                owner->SetReactState(REACT_AGGRESSIVE);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_snowdrift_fists_of_fury_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_snowdrift_fists_of_fury_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_snowdrift_fists_of_fury_AuraScript();
        }
};

class spell_spm_parry_stance : public SpellScriptLoader
{
    public:
        spell_spm_parry_stance() : SpellScriptLoader("spell_spm_parry_stance") { }

        class spell_spm_parry_stance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spm_parry_stance_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto const owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                if (owner->IsAIEnabled)
                    owner->AI()->DoZoneInCombat();
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_spm_parry_stance_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_PACIFY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_spm_parry_stance_AuraScript();
        }
};

void AddSC_boss_master_snowdrift()
{
    new boss_master_snowdrift();
    new npc_snowdrift_clone();
    new npc_spm_pandaren_referee();
    new npc_snowdrift_novice();
    new npc_snowdrift_miniboss();
    new npc_spm_jade_staff();
    new npc_snowdrift_fireball();
    new spell_spm_swirling_steel();
    new spell_snowdrift_ball_of_fire();
    new spell_snowdrift_ball_of_fire_aura();
    new spell_snowdrift_hurl_chi();
    new spell_snowdrift_tornado_slam();
    new spell_snowdrift_tornado_slam_effect();
    new spell_snowdrift_parry_stance();
    new spell_snowdrift_copy_health();
    new spell_snowdrift_ball_of_fire_damage();
    new spell_snowdrift_fists_of_fury();
    new spell_spm_parry_stance();
    new AreaTrigger_at_before_snowdrift();
}
