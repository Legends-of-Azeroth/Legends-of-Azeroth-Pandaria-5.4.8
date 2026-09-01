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
    SAY_SHIELD                                  = 4, // unused
    SAY_WHISPER                                 = 5, // unused
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
            boss_zuramatAI(Creature* creature) : ScriptedAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            SummonList Summons;

            uint32 SpellVoidShiftTimer;
            uint32 SpellSummonVoidTimer;
            uint32 SpellShroudOfDarknessTimer;

            void Reset() override
            {
                Summons.DespawnAll();
                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }

                SpellShroudOfDarknessTimer = 22000;
                SpellVoidShiftTimer = 15000;
                SpellSummonVoidTimer = 12000;
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

                if (instance)
                {
                    if (GameObject* pDoor = instance->instance->GetGameObject(instance->GetGuidData(DATA_ZURAMAT_CELL)))
                        if (pDoor->GetGoState() == GO_STATE_READY)
                        {
                            EnterEvadeMode();
                            return;
                        }
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (instance)
                    instance->SetData(DATA_WIPE, 1);
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (SpellSummonVoidTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SUMMON_VOID_SENTRY, false);
                    SpellSummonVoidTimer = 20000;
                } else SpellSummonVoidTimer -=diff;

                if (SpellVoidShiftTimer <= diff)
                {
                     if (Unit* pUnit = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(pUnit, SPELL_VOID_SHIFT);
                    SpellVoidShiftTimer = 20000;
                } else SpellVoidShiftTimer -=diff;

                if (SpellShroudOfDarknessTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_SHROUD_OF_DARKNESS);
                    SpellShroudOfDarknessTimer = 20000;
                } else SpellShroudOfDarknessTimer -=diff;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    {
                        if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                        instance->SetData(DATA_WAVE_COUNT, 7);
                    }
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    {
                        if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
                        instance->SetData(DATA_WAVE_COUNT, 13);
                    }
                }

                Summons.DespawnAll();
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

            void JustSummoned(Creature* summon) override
            {
                Summons.Summon(summon);
            }
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
            npc_void_sentryAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_SUMMON_VOID_SENTRY_BALL, true);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void JustDied(Unit* killer) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_A_VOID_DANCE, 0);
                summons.DespawnAll();
            }

            void Unsummoned() override
            {
                summons.DespawnAll();
            }

            void UpdateAI(uint32 /*diff*/) override { }

        private:
            SummonList summons;
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

            void DamageTaken(Unit* attacker, uint32& damage) override
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
