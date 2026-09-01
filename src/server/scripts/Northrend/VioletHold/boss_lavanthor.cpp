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
  SPELL_CAUTERIZING_FLAMES                      = 59466, //Only in heroic
  SPELL_FIREBOLT                                = 54235,
  SPELL_FIREBOLT_H                              = 59468,
  SPELL_FLAME_BREATH                            = 54282,
  SPELL_FLAME_BREATH_H                          = 59469,
  SPELL_LAVA_BURN                               = 54249,
  SPELL_LAVA_BURN_H                             = 59594
};

class boss_lavanthor : public CreatureScript
{
    public:
        boss_lavanthor() : CreatureScript("boss_lavanthor") { }

        struct boss_lavanthorAI : public ScriptedAI
        {
            boss_lavanthorAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiFireboltTimer;
            uint32 uiFlameBreathTimer;
            uint32 uiLavaBurnTimer;
            uint32 uiCauterizingFlamesTimer;

            InstanceScript* instance;

            void Reset() override
            {
                uiFireboltTimer = 1000;
                uiFlameBreathTimer = 5000;
                uiLavaBurnTimer = 10000;
                uiCauterizingFlamesTimer = 3000;
                if (instance)
                {
                    if (instance->GetData(DATA_WAVE_COUNT) == 6)
                        instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                        instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                {
                if (GameObject* pDoor = instance->instance->GetGameObject(instance->GetGuidData(DATA_LAVANTHOR_CELL)))
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

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiFlameBreathTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me->GetVictim(), SPELL_FLAME_BREATH);
                        uiFlameBreathTimer = urand(10000, 15000);
                    }
                } else uiFlameBreathTimer -= diff;

                if (uiLavaBurnTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_LAVA_BURN, SPELL_LAVA_BURN_H));
                        uiLavaBurnTimer = urand(10000, 20000);
                    }
                }

                if (IsHeroic())
                {
                    if (uiCauterizingFlamesTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_CAUTERIZING_FLAMES);
                        uiCauterizingFlamesTimer = urand(10000, 16000);
                    } else uiCauterizingFlamesTimer -= diff;
                }

                if (uiFireboltTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_FIREBOLT, SPELL_FIREBOLT_H));
                        uiFireboltTimer = urand(5000, 13000);
                    }
                } else uiFireboltTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
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
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lavanthorAI(creature);
        }
};

void AddSC_boss_lavanthor()
{
    new boss_lavanthor();
}
