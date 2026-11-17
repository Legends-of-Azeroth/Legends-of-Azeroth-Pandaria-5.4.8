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
    SPELL_ARCANE_VACUUM                         = 58694,
    SPELL_BLIZZARD                              = 58693,
    SPELL_BLIZZARD_H                            = 59369,
    SPELL_MANA_DESTRUCTION                      = 59374,
    SPELL_TAIL_SWEEP                            = 58690,
    SPELL_TAIL_SWEEP_H                          = 59283,
    SPELL_UNCONTROLLABLE_ENERGY                 = 58688,
    SPELL_UNCONTROLLABLE_ENERGY_H               = 59281,
    SPELL_TRANSFORM                             = 58668
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_DISRUPTION                              = 4,
    SAY_BREATH_ATTACK                           = 5,
    SAY_SPECIAL_ATTACK                          = 6,
};

class boss_cyanigosa : public CreatureScript
{
    public:
        boss_cyanigosa() : CreatureScript("boss_cyanigosa") { }

        struct boss_cyanigosaAI : public ScriptedAI
        {
            boss_cyanigosaAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
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
                    _instance->SetData(DATA_CYANIGOSA_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (_instance)
                    _instance->SetData(DATA_CYANIGOSA_EVENT, IN_PROGRESS);

                _scheduler
                    .Schedule(Seconds(10), [this](TaskContext context)
                    {
                        DoCastAOE(SPELL_ARCANE_VACUUM);
                        context.Repeat(Seconds(10));
                    })
                    .Schedule(Seconds(15), [this](TaskContext context)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 45, true))
                            DoCast(target, DUNGEON_MODE(SPELL_BLIZZARD, SPELL_BLIZZARD_H));
                        context.Repeat(Seconds(15));
                    })
                    .Schedule(Seconds(20), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_TAIL_SWEEP, SPELL_TAIL_SWEEP_H));
                        context.Repeat(Seconds(20));
                    })
                    .Schedule(Seconds(25), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_UNCONTROLLABLE_ENERGY, SPELL_UNCONTROLLABLE_ENERGY_H));
                        context.Repeat(Seconds(25));
                    });

                if (IsHeroic())
                    _scheduler.Schedule(Seconds(30), [this](TaskContext context)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                            DoCast(target, SPELL_MANA_DESTRUCTION);
                        context.Repeat(Seconds(30));
                    });
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_ARCANE_VACUUM)
                {
                    if (Player* player = target->ToPlayer())
                        player->TeleportTo(me->GetMapId(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0);
                }
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (_instance && _instance->GetData(DATA_REMOVE_NPC) == 1)
                {
                    me->DespawnOrUnsummon();
                    _instance->SetData(DATA_REMOVE_NPC, 0);
                }

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
                    _instance->SetData(DATA_CYANIGOSA_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_cyanigosaAI(creature);
        }
};

void AddSC_boss_cyanigosa()
{
    new boss_cyanigosa();
}
