/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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

/*
 * Comment: Timer check pending
 */

#include "ScriptPCH.h"
#include "halls_of_lightning.h"

enum Spells
{
    SPELL_BALL_LIGHTNING                          = 52780,
    SPELL_BALL_LIGHTNING_H                        = 59800,
    SPELL_STATIC_OVERLOAD                         = 52658,
    SPELL_STATIC_OVERLOAD_H                       = 59795,

    SPELL_DISPERSE                                = 52770,
    SPELL_SUMMON_SPARK                            = 52746,
    SPELL_SPARK_DESPAWN                           = 52776,

    // Spark of Ionar
    SPELL_SPARK_VISUAL_TRIGGER                    = 52667,
    SPELL_SPARK_VISUAL_TRIGGER_H                  = 59833,
    SPELL_ARCING_BURN                             = 52671,
    SPELL_ARCING_BURN_H                           = 59834,
    ARC_WELD                                      = 59086
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_SPLIT                                     = 1,
    SAY_SLAY                                      = 2,
    SAY_DEATH                                     = 3,
};

enum IonarCreatures
{
    NPC_SPARK_OF_IONAR                            = 28926
};

enum Misc
{
    DATA_MAX_SPARKS                               = 5,
    DATA_MAX_SPARK_DISTANCE                       = 90, // Distance to boss - prevent runs through the whole instance
    DATA_POINT_CALLBACK                           = 0
};

class boss_ionar : public CreatureScript
{
    public:
        boss_ionar() : CreatureScript("boss_ionar") { }

        bool OnDummyEffect(Unit* /*pcaster*/, uint32 SpellId, SpellEffIndex EffIndex, Creature* Target) override
        {
            // always check spellid and effectindex
            if (SpellId == SPELL_DISPERSE && EffIndex == 0)
            {
                if (Target->GetEntry() != NPC_IONAR)
                    return true;

                for (uint8 i = 0; i < DATA_MAX_SPARKS; ++i)
                    Target->CastSpell(Target, SPELL_SUMMON_SPARK, true);

                Target->AttackStop();
                Target->SetVisible(false);
                Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);

                Target->GetMotionMaster()->Clear();
                Target->GetMotionMaster()->MoveIdle();

                // always return true when we are handling this spell and effect
                return true;
            }
            return false;
        }

        struct boss_ionarAI : public ScriptedAI
        {
            boss_ionarAI(Creature* creature) : ScriptedAI(creature), lSparkList(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            SummonList lSparkList;

            bool bIsSplitPhase;
            bool bHasDispersed;

            uint32 uiSplitTimer;

            uint32 uiStaticOverloadTimer;
            uint32 uiBallLightningTimer;

            uint32 uiDisperseHealth;

            void Reset() override
            {
                lSparkList.DespawnAll();

                bIsSplitPhase = true;
                bHasDispersed = false;

                uiSplitTimer = 25 * IN_MILLISECONDS;

                uiStaticOverloadTimer = urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS);
                uiBallLightningTimer = urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS);

                uiDisperseHealth = 45 + urand(0, 10);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);

                if (!me->IsVisible())
                    me->SetVisible(true);

                if (instance)
                    instance->SetData(TYPE_IONAR, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(TYPE_IONAR, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                lSparkList.DespawnAll();

                if (instance)
                    instance->SetData(TYPE_IONAR, DONE);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            // make sparks come back
            void CallBackSparks()
            {
                // should never be empty here, but check
                if (lSparkList.empty())
                    return;

                Position pos = me->GetPosition();

                for (auto&& guid : lSparkList)
                {
                    if (Creature* pSpark = Unit::GetCreature(*me, guid))
                    {
                        if (pSpark->IsAlive())
                        {
                            pSpark->SetSpeed(MOVE_RUN, 2.0f);
                            pSpark->GetMotionMaster()->Clear();
                            pSpark->GetMotionMaster()->MovePoint(DATA_POINT_CALLBACK, pos);
                        }
                        else
                            pSpark->DespawnOrUnsummon();
                    }
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!me->IsVisible())
                    damage = 0;
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_SPARK_OF_IONAR)
                {
                    lSparkList.Summon(summon);

                    summon->CastSpell(summon, DUNGEON_MODE(SPELL_SPARK_VISUAL_TRIGGER,SPELL_SPARK_VISUAL_TRIGGER_H), true);

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    {
                        summon->SetInCombatWith(target);
                        summon->GetMotionMaster()->Clear();
                        summon->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                    }
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_SPARK_OF_IONAR)
                    lSparkList.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                if (me->FindCurrentSpellBySpellId(SPELL_DISPERSE))
                    return;

                // Splitted
                if (!me->IsVisible())
                {
                    if (uiSplitTimer <= diff)
                    {
                        uiSplitTimer = 2500;

                        // Return sparks to where Ionar splitted
                        if (bIsSplitPhase)
                        {
                            CallBackSparks();
                            bIsSplitPhase = false;
                        }
                        // Lightning effect and restore Ionar
                        else if (lSparkList.empty())
                        {
                            me->SetVisible(true);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);

                            DoCast(me, SPELL_SPARK_DESPAWN, false);

                            uiSplitTimer = 25000;
                            bIsSplitPhase = true;

                            if (me->GetVictim())
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                        }
                    }
                    else
                        uiSplitTimer -= diff;

                    return;
                }

                if (uiStaticOverloadTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_STATIC_OVERLOAD);

                    uiStaticOverloadTimer = urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS);
                }
                else
                    uiStaticOverloadTimer -= diff;

                if (uiBallLightningTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_BALL_LIGHTNING);
                    uiBallLightningTimer = urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS);
                }
                else
                    uiBallLightningTimer -= diff;

                // Health check
                if (!bHasDispersed && HealthBelowPct(uiDisperseHealth))
                {
                    bHasDispersed = true;

                    Talk(SAY_SPLIT);

                    if (me->IsNonMeleeSpellCasted(false))
                        me->InterruptNonMeleeSpells(false);

                    DoCast(me, SPELL_DISPERSE);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ionarAI(creature);
        }
};

class npc_spark_of_ionar : public CreatureScript
{
    public:
        npc_spark_of_ionar() : CreatureScript("npc_spark_of_ionar") { }

        struct npc_spark_of_ionarAI : public ScriptedAI
        {
            npc_spark_of_ionarAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiCheckTimer;
            uint32 uiArcingBurnTimer;
            uint32 uiArcWeldTimer;

            void Reset() override
            {
                uiCheckTimer = 2 * IN_MILLISECONDS;
                uiArcingBurnTimer = 1000;
                uiArcWeldTimer = 50;
                me->SetReactState(REACT_PASSIVE);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || !instance)
                    return;

                if (pointId == DATA_POINT_CALLBACK)
                    me->DespawnOrUnsummon();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                // Despawn if the encounter is not running
                if (instance && instance->GetData(TYPE_IONAR) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (uiArcWeldTimer <= diff)
                {
                    DoCast(me, ARC_WELD); //Creates the walking effect
                    uiArcWeldTimer = 50;
                }
                else
                    uiArcWeldTimer -= diff;

                if (uiArcingBurnTimer <= diff)
                {
                    DoCast(me, DUNGEON_MODE(SPELL_ARCING_BURN,SPELL_ARCING_BURN_H));
                    uiArcingBurnTimer = 1000;
                }
                else
                    uiArcingBurnTimer -= diff;

                // Prevent them to follow players through the whole instance
                if (uiCheckTimer <= diff)
                {
                    if (instance)
                    {
                        Creature* pIonar = instance->instance->GetCreature(instance->GetGuidData(DATA_IONAR));
                        if (pIonar && pIonar->IsAlive())
                        {
                            if (me->GetDistance(pIonar) > DATA_MAX_SPARK_DISTANCE)
                            {
                                Position pos = pIonar->GetPosition();

                                me->SetSpeed(MOVE_RUN, 2.0f);
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MovePoint(DATA_POINT_CALLBACK, pos);
                            }
                        }
                        else
                            me->DespawnOrUnsummon();
                    }
                    uiCheckTimer = 2 * IN_MILLISECONDS;
                }
                else
                    uiCheckTimer -= diff;

                // No melee attack at all!
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spark_of_ionarAI(creature);
        }
};

void AddSC_boss_ionar()
{
    new boss_ionar();
    new npc_spark_of_ionar();
}
