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

/* Script Data Start
SDName: Boss epoch
SDCategory:
Script Data End */

#include "ScriptPCH.h"
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CURSE_OF_EXERTION                     = 52772,
    SPELL_TIME_WARP                             = 52766, // Time slows down, reducing attack, casting and movement speed by 70% for 6 sec.
    SPELL_TIME_STOP                             = 58848, // Stops time in a 50 yard sphere for 2 sec.
    SPELL_WOUNDING_STRIKE                       = 52771, // Used only on the tank
    SPELL_WOUNDING_STRIKE_H                     = 58830,
    SPELL_TIME_STEP                             = 52737, // not sure
    SPELL_TIME_STEP_H                           = 58829
};

enum Yells
{
    SAY_INTRO                                   = 0,
    SAY_AGGRO                                   = 1,
    SAY_TIME_WARP                               = 2,
    SAY_SLAY                                    = 3,
};

class boss_epoch : public CreatureScript
{
    public:
        boss_epoch() : CreatureScript("boss_epoch") { }

        struct boss_epochAI : public ScriptedAI
        {
            boss_epochAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint8 uiStepCount;

            uint32 uiWoundingStrikeTimer;
            uint32 uiTimeWarpTimer;
            uint32 uiTimeStopTimer;
            uint32 uiCurseOfExertionTimer;

            bool bTimeWarping;

            InstanceScript* instance;

            void Reset() override
            {
                uiCurseOfExertionTimer = 5000;
                uiTimeWarpTimer = 20000;
                uiTimeStopTimer = 25000;
                uiWoundingStrikeTimer = 7500;
                bTimeWarping = false;
                uiStepCount = 0;

                if (instance)
                    instance->SetData(DATA_EPOCH_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_EPOCH_EVENT, IN_PROGRESS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (bTimeWarping)
                {
                    if (uiTimeWarpTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, -5, true))
                            DoCast(target, DUNGEON_MODE(SPELL_TIME_STEP, SPELL_TIME_STEP_H));

                        uiTimeWarpTimer = 500;
                        ++uiStepCount;

                        if (uiStepCount >= 6)
                        {
                            bTimeWarping = false;
                            uiTimeWarpTimer = 25000;
                        }

                    } else uiTimeWarpTimer -= diff;

                    return;
                }

                if (uiCurseOfExertionTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, SPELL_CURSE_OF_EXERTION);
                    uiCurseOfExertionTimer = DUNGEON_MODE(15000, 10000);
                } else uiCurseOfExertionTimer -= diff;

                if (uiWoundingStrikeTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_WOUNDING_STRIKE, SPELL_WOUNDING_STRIKE_H));
                    uiWoundingStrikeTimer = urand(10000, 12000);
                } else uiWoundingStrikeTimer -= diff;

                if (IsHeroic())
                {
                    if (uiTimeStopTimer <= diff)
                    {
                        DoCastAOE(SPELL_TIME_STOP);
                        uiTimeStopTimer = urand(15000, 20000);
                    } else uiTimeStopTimer -= diff;
                }

                if (uiTimeWarpTimer <= diff)
                {
                    Talk(SAY_TIME_WARP);
                    DoCastAOE(SPELL_TIME_WARP);
                    uiTimeWarpTimer = 500;
                    bTimeWarping = true;
                    uiStepCount = 0;
                } else uiTimeWarpTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoPlaySoundToSet(me, 13416);

                if (instance)
                    instance->SetData(DATA_EPOCH_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_epochAI (creature);
        }
};

void AddSC_boss_epoch()
{
    new boss_epoch();
}
