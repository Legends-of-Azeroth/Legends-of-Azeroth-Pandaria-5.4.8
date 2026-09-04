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
#include "violet_hold.h"

enum Spells
{
    SPELL_VOID_SHIFT                            = 54361,
    SPELL_VOID_SHIFT_H                          = 59743,
    SPELL_SHROUD_OF_DARKNESS                    = 54524,
    SPELL_SHROUD_OF_DARKNESS_H                  = 59745,
    SPELL_SUMMON_VOID_SENTRY                    = 54369,
    SPELL_SUMMON_VOID_SENTRY_BALL               = 58650,

    SPELL_EXHAUSTION                            = 44867,
    SPELL_VOID_SHIFTED                          = 54343,
    SPELL_ZURAMAT_ADD_2                         = 54342,
    SPELL_ZURAMAT_ADD_2_H                       = 59747,
    SPELL_SHADOW_BOLT_VOLLEY                    = 57942,
};

enum ZuramatCreatures
{
    NPC_VOID_SENTRY                             = 29364
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_SHIELD                                  = 4,
    SAY_WHISPER                                 = 5,
};

enum eActions
{
    ACTION_VOID_DEAD,
};

class boss_zuramat : public CreatureScript
{
    public:
        boss_zuramat() : CreatureScript("boss_zuramat") { }

        struct boss_zuramatAI : public ScriptedAI
        {
            boss_zuramatAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()), _summons(me)
            {
                _scheduler.SetValidator([this]
                {
                    return !me->HasUnitState(UNIT_STATE_CASTING);
                });
            }

            void Reset() override
            {
                _scheduler.CancelAll();
                _summons.DespawnAll();

                if (_instance)
                {
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                        _instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                        _instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }

                me->GetMap()->SetWorldState(WORLDSTATE_A_VOID_DANCE, 1);
            }

            void AttackStart(Unit* who) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (_instance)
                {
                    if (GameObject* pDoor = _instance->instance->GetGameObject(_instance->GetGuidData(DATA_ZURAMAT_CELL)))
                        if (pDoor->GetGoState() == GO_STATE_READY)
                        {
                            EnterEvadeMode();
                            return;
                        }
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                        _instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                        _instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
                }

                _scheduler
                    .Schedule(Seconds(4), [this](TaskContext context)
                    {
                        DoCast(me, SPELL_SUMMON_VOID_SENTRY, true);
                        context.Repeat(Seconds(7), Seconds(10));
                    })
                    .Schedule(Seconds(9), [this](TaskContext context)
                    {
                        if (Unit* pUnit = SelectTarget(SELECT_TARGET_RANDOM, 0, 60, true))
                            DoCast(pUnit, DUNGEON_MODE(SPELL_VOID_SHIFT, SPELL_VOID_SHIFT_H));
                        context.Repeat(Seconds(15));
                    })
                    .Schedule(Seconds(18), Seconds(20), [this](TaskContext context)
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_SHROUD_OF_DARKNESS, SPELL_SHROUD_OF_DARKNESS_H));
                        context.Repeat(Seconds(18), Seconds(20));
                    });
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (_instance)
                    _instance->SetData(DATA_WIPE, 1);
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _scheduler.Update(diff, [this]
                {
                    DoMeleeAttackIfReady();
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                _scheduler.CancelAll();
                Talk(SAY_DEATH);

                if (_instance)
                {
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                    {
                        if (IsHeroic() && _instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        _instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                        _instance->SetData(DATA_WAVE_COUNT, 7);
                    }
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                    {
                        if (IsHeroic() && _instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        _instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
                        _instance->SetData(DATA_WAVE_COUNT, 13);
                    }
                }

                _summons.DespawnAll();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

            void JustSummoned(Creature* summon) override
            {
                _summons.Summon(summon);
            }

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
            SummonList _summons;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_zuramatAI(creature);
        }
};

class npc_void_sentry : public CreatureScript
{
    public:
        npc_void_sentry() : CreatureScript("npc_void_sentry") { }

        struct npc_void_sentryAI : public ScriptedAI
        {
            npc_void_sentryAI(Creature* creature) : ScriptedAI(creature), _summons(me) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_SUMMON_VOID_SENTRY_BALL, true);
            }

            void JustSummoned(Creature* summon) override
            {
                _summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_A_VOID_DANCE, 0);
                _summons.DespawnAll();
            }

            void Unsummoned() override
            {
                _summons.DespawnAll();
            }

            void UpdateAI(uint32 /*diff*/) override { }

        private:
            SummonList _summons;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_void_sentryAI(creature);
        }
};

class npc_void_sentry_ball : public CreatureScript
{
    public:
        npc_void_sentry_ball() : CreatureScript("npc_void_sentry_ball") { }

        struct npc_void_sentry_ballAI : public ScriptedAI
        {
            npc_void_sentry_ballAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_void_sentry_ballAI(creature);
        }
};

void AddSC_boss_zuramat()
{
    new boss_zuramat();
    new npc_void_sentry();
    new npc_void_sentry_ball();
}
