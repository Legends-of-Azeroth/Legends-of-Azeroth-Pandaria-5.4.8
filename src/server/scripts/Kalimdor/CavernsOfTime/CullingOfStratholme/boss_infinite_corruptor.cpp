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
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_VOID_STIKE               = 60590,
    SPELL_CORRUPTING_BLIGHT        = 60588,
    SPELL_CORRUPTING_BLIGHT_REMOVE = 60589,
    SPELL_CHANNEL_VISUAL           = 31387,
};

enum Yells
{
    SAY_AGGRO                      = 0,
    SAY_DEATH                      = 1,
    SAY_FAIL                       = 2,
};

enum Npcs
{
    NPC_TIME_RIFT                  = 28409,
};

class boss_infinite_corruptor : public CreatureScript
{
    public:
        boss_infinite_corruptor() : CreatureScript("boss_infinite_corruptor") { }

        struct boss_infinite_corruptorAI : public ScriptedAI
        {
            boss_infinite_corruptorAI(Creature *creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
                timeRift = NULL;
                Reset();
            }

            InstanceScript* instance;
            uint32 uiStrikeTimer;
            uint32 uiBlightTimer;
            Creature* timeRift;

            void JustReachedHome() override
            {
                // Data for visual
                if (timeRift)
                    timeRift->DisappearAndDie();

                timeRift = me->SummonCreature(NPC_TIME_RIFT, me->GetPositionX() - 10.0f, me->GetPositionY(), me->GetPositionZ());

                // Visual Channeling
                if (timeRift)
                    me->CastSpell(timeRift, SPELL_CHANNEL_VISUAL, true);
            }

            void Reset() override
            {
                JustReachedHome();

                // Starting Timers
                uiStrikeTimer = urand(1000, 3000);
                uiBlightTimer = urand(5000, 8000);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCastAOE(SPELL_CORRUPTING_BLIGHT_REMOVE, true);
                Talk(SAY_DEATH);
                if (instance)
                    instance->SetData(DATA_INFINITE_EVENT, DONE);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_CORRUPTING_BLIGHT_REMOVE)
                    target->RemoveAurasDueToSpell(SPELL_CORRUPTING_BLIGHT);
            }

            void DoAction(int32 action) override
            {
                if (action == 0) // Called from InstanceScript
                {
                    Talk(SAY_FAIL);
                    me->DespawnOrUnsummon(1000);
                }
            }
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiStrikeTimer < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                       DoCast(target, SPELL_VOID_STIKE);
                    uiStrikeTimer = urand(3000, 8000);
                }
                else uiStrikeTimer -= diff;

                if (uiBlightTimer < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                       DoCast(target, SPELL_CORRUPTING_BLIGHT);
                    uiBlightTimer = urand(6000, 9000);
                }
                else uiBlightTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_infinite_corruptorAI(creature);
        }
};

void AddSC_boss_infinite_corruptor()
{
    new boss_infinite_corruptor();
}
