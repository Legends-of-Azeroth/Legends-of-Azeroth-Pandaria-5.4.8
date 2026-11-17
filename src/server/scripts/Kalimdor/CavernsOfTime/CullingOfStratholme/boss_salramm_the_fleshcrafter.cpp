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
SDName: Boss salramm
SDCategory:
Script Data End */

#include "ScriptPCH.h"
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CURSE_OF_TWISTED_FLESH                = 58845,
    SPELL_EXPLODE_GHOUL                         = 52480,
    SPELL_EXPLODE_GHOUL_H                       = 58825,
    SPELL_SHADOW_BOLT                           = 57725,
    SPELL_SHADOW_BOLT_H                         = 58827,
    SPELL_STEAL_FLESH                           = 52708,
    SPELL_STEAL_FLESH_VICTIM                    = 52711,
    SPELL_STEAL_FLESH_SELF                      = 52712,
    SPELL_SUMMON_GHOULS                         = 52451
};

enum Entry
{
    NPC_GHOUL_MINION                            = 27733
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SPAWN                                   = 1,
    SAY_SLAY                                    = 2,
    SAY_DEATH                                   = 3,
    SAY_EXPLODE_GHOUL                           = 4,
    SAY_STEAL_FLESH                             = 5,
    SAY_SUMMON_GHOULS                           = 6,
};

class boss_salramm : public CreatureScript
{
    public:
        boss_salramm() : CreatureScript("boss_salramm") { }

        struct boss_salrammAI : public ScriptedAI
        {
            boss_salrammAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                if (instance)
                    Talk(SAY_SPAWN);
            }

            uint32 uiCurseFleshTimer;
            uint32 uiExplodeGhoulTimer;
            uint32 uiShadowBoltTimer;
            uint32 uiStealFleshTimer;
            uint32 uiSummonGhoulsTimer;
            uint32 uiRespawnZombiesTimer;

            InstanceScript* instance;

            void Reset() override
            {
                 uiCurseFleshTimer = 30000;  // 30s DBM
                 uiExplodeGhoulTimer = urand(25000, 28000); // approx 6 sec after summon ghouls
                 uiShadowBoltTimer = urand(8000, 12000); // approx 10s
                 uiStealFleshTimer = 12345;
                 uiSummonGhoulsTimer = urand(19000, 24000); // on a video approx 24s after aggro
                 uiRespawnZombiesTimer = 200000;

                 if (instance)
                     instance->SetData(DATA_SALRAMM_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                     instance->SetData(DATA_SALRAMM_EVENT, IN_PROGRESS);
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_STEAL_FLESH)
                {
                    DoCast(target, SPELL_STEAL_FLESH_VICTIM, true);
                    DoCast(me, SPELL_STEAL_FLESH_SELF, true);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_SALRAMM_EVENT, DONE);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetCorpseDelay(0);  //instant despawn corpse (needed?)
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                // Curse of twisted flesh timer
                if (IsHeroic())
                    if (uiCurseFleshTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50, true))
                            DoCast(target, SPELL_CURSE_OF_TWISTED_FLESH);
                        uiCurseFleshTimer = 37000;
                    } else uiCurseFleshTimer -= diff;

                // Shadow bolt timer
                if (uiShadowBoltTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT, SPELL_SHADOW_BOLT_H));
                    uiShadowBoltTimer = urand(8000, 12000);
                } else uiShadowBoltTimer -= diff;

                // Steal Flesh timer
                if (uiStealFleshTimer <= diff)
                {
                    Talk(SAY_STEAL_FLESH);
                    if (Unit* random_target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(random_target, SPELL_STEAL_FLESH);
                    uiStealFleshTimer = 10000;
                } else uiStealFleshTimer -= diff;

                // Summon ghouls timer
                if (uiSummonGhoulsTimer <= diff)
                {
                    Talk(SAY_SUMMON_GHOULS);
                    if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 20, false))
                        DoCast(pTarget, SPELL_SUMMON_GHOULS);
                    uiSummonGhoulsTimer = 10000;
                    uiExplodeGhoulTimer = urand(8000, 10000);

                } else uiSummonGhoulsTimer -= diff;

                if (uiExplodeGhoulTimer <= diff)
                {
                    Talk(SAY_EXPLODE_GHOUL);
                    DoCast(DUNGEON_MODE(SPELL_EXPLODE_GHOUL, SPELL_EXPLODE_GHOUL_H));
                    uiExplodeGhoulTimer = urand(6000, 8000);
                } else uiExplodeGhoulTimer -= diff;

                // Respawn risen zombies at town
                if (uiRespawnZombiesTimer && uiRespawnZombiesTimer <= diff)
                {
                    instance->SetData(DATA_TRANSFORM_CITIZENS, SPECIAL);
                    uiRespawnZombiesTimer = 0;
                }
                else uiRespawnZombiesTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_salrammAI (creature);
        }
};

void AddSC_boss_salramm()
{
    new boss_salramm();
}
