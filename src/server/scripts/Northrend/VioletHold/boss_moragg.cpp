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
    SPELL_CORROSIVE_SALIVA                     = 54527,
    SPELL_OPTIC_LINK                           = 54396,
    SPELL_RAY_OF_SUFFERING                     = 54442,
    SPELL_RAY_OF_SUFFERING_H                   = 59524,
    SPELL_RAY_OF_SUFFERING_TIGGER              = 54417,
    SPELL_RAY_OF_PAIN                          = 54438,
    SPELL_RAY_OF_PAIN_H                        = 59523,
    SPELL_RAY_OF_PAIN_TIGGER                   = 54416,
};

class boss_moragg : public CreatureScript
{
    public:
        boss_moragg() : CreatureScript("boss_moragg") { }

        struct boss_moraggAI : public ScriptedAI
        {
            boss_moraggAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
            {
                _scheduler.SetValidator([this]
                {
                    return !me->HasUnitState(UNIT_STATE_CASTING);
                });
            }

            void Reset() override
            {
                _scheduler.CancelAll();

                if (_instance)
                {
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                        _instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                        _instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (_instance)
                {
                    if (GameObject* pDoor = _instance->instance->GetGameObject(_instance->GetGuidData(DATA_MORAGG_CELL)))
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

                DoCast(me, DUNGEON_MODE(SPELL_RAY_OF_PAIN, SPELL_RAY_OF_PAIN_H), true);
                DoCast(me, DUNGEON_MODE(SPELL_RAY_OF_SUFFERING, SPELL_RAY_OF_SUFFERING_H), true);

                _scheduler
                    .Schedule(Seconds(15), [this](TaskContext context)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                            DoCast(target, SPELL_OPTIC_LINK);
                        context.Repeat(Seconds(25));
                    })
                    .Schedule(Seconds(5), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), SPELL_CORROSIVE_SALIVA);
                        context.Repeat(Seconds(10));
                    });
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (_instance)
                    _instance->SetData(DATA_WIPE, 1);
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

                _scheduler.Update(diff, [this]
                {
                    DoMeleeAttackIfReady();
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                _scheduler.CancelAll();

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
            }

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_moraggAI(creature);
        }
};

void AddSC_boss_moragg()
{
    new boss_moragg();
}
