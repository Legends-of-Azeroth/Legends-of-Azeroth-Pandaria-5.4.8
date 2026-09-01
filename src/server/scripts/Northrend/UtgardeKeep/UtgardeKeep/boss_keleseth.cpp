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

/* ScriptData
SDName: Boss_Prince_Keleseth
SD%Complete: 90
SDComment:  Needs Prince Movements, Needs adjustments to blizzlike timers, Needs Shadowbolt castbar, Needs right Ressurect Visual, Needs Some Heroic Spells
SDCategory: Utgarde Keep
EndScriptData */

#include "ScriptPCH.h"
#include "utgarde_keep.h"

enum ScriptTexts
{
    SAY_AGGRO                                = 0,
    SAY_FROST_TOMB                           = 1,
    SAY_FROST_TOMB_EMOTE                     = 2,
    SAY_SKELETONS                            = 3,
    SAY_KILL                                 = 4,
    SAY_DEATH                                = 5,
};

enum Spells
{
    SPELL_SHADOWBOLT                         = 43667,
    SPELL_SHADOWBOLT_HEROIC                  = 59389,
    SPELL_FROST_TOMB                         = 48400,
    SPELL_FROST_TOMB_SUMMON                  = 42714,
    SPELL_DECREPIFY                          = 42702,
    SPELL_SCOURGE_RESSURRECTION              = 42704,
};

enum Creatures
{
    NPC_FROSTTOMB                            = 23965,
    NPC_SKELETON                             = 23970,
};

enum Misc
{
    ACHIEVEMENT_ON_THE_ROCKS                 = 1919,
};

#define SKELETONSPAWN_Z                          42.8668f

float SkeletonSpawnPoint[5][5]=
{
    { 156.2559f, 259.2093f },
    { 156.2559f, 259.2093f },
    { 156.2559f, 259.2093f },
    { 156.2559f, 259.2093f },
    { 156.2559f, 259.2093f },
};

float AttackLoc[3] = { 197.636f, 194.046f, 40.8164f };

bool ShatterFrostTomb; // needed for achievement: On The Rocks(1919)

class npc_frost_tomb : public CreatureScript
{
    public:
        npc_frost_tomb() : CreatureScript("npc_frost_tomb") { }

        struct npc_frost_tombAI : public ScriptedAI
        {
            npc_frost_tombAI(Creature* creature) : ScriptedAI(creature)
            {
                FrostTombGUID = ObjectGuid::Empty;
            }

            ObjectGuid FrostTombGUID;

            void SetPrisoner(Unit* uPrisoner)
            {
                FrostTombGUID = uPrisoner->GetGUID();
            }

            void Reset() override
            {
                FrostTombGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void AttackStart(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void JustDied(Unit* killer) override
            {
                if (killer->GetGUID() != me->GetGUID())
                    ShatterFrostTomb = true;

                if (FrostTombGUID)
                {
                    Unit* FrostTomb = Unit::GetUnit(*me, FrostTombGUID);
                    if (FrostTomb)
                        FrostTomb->RemoveAurasDueToSpell(SPELL_FROST_TOMB);
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                Unit* temp = Unit::GetUnit(*me, FrostTombGUID);
                if ((temp && temp->IsAlive() && !temp->HasAura(SPELL_FROST_TOMB)) || !temp)
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_frost_tombAI(creature);
        }
};

class boss_keleseth : public CreatureScript
{
    public:
        boss_keleseth() : CreatureScript("boss_keleseth") { }

        struct boss_kelesethAI : public ScriptedAI
        {
            boss_kelesethAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 FrostTombTimer;
            uint32 SummonSkeletonsTimer;
            uint32 RespawnSkeletonsTimer;
            uint32 ShadowboltTimer;
            ObjectGuid SkeletonGUID[5];
            bool Skeletons;
            bool RespawnSkeletons;

            void Reset() override
            {
                ShadowboltTimer = 0;
                Skeletons = false;

                ShatterFrostTomb = false;

                ResetTimer();

                if (instance)
                    instance->SetData(DATA_PRINCEKELESETH_EVENT, NOT_STARTED);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (IsHeroic() && !ShatterFrostTomb)
                {
                    AchievementEntry const* AchievOnTheRocks = sAchievementStore.LookupEntry(ACHIEVEMENT_ON_THE_ROCKS);
                    if (AchievOnTheRocks)
                    {
                        Map* map = me->GetMap();
                        if (map && map->IsDungeon())
                        {
                            for (auto&& ref : map->GetPlayers())
                                ref.GetSource()->CompletedAchievement(AchievOnTheRocks);
                        }
                    }
                }

                if (instance)
                    instance->SetData(DATA_PRINCEKELESETH_EVENT, DONE);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                DoZoneInCombat();

                if (instance)
                    instance->SetData(DATA_PRINCEKELESETH_EVENT, IN_PROGRESS);
            }

            void ResetTimer(uint32 inc = 0)
            {
                SummonSkeletonsTimer = 5000 + inc;
                FrostTombTimer = 28000 + inc;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (ShadowboltTimer <= diff)
                {
                    Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0);
                    if (target && target->IsAlive() && target->GetTypeId() == TYPEID_PLAYER)
                        me->CastSpell(target, DUNGEON_MODE(SPELL_SHADOWBOLT, SPELL_SHADOWBOLT_HEROIC), true);
                    ShadowboltTimer = 10000;
                } else ShadowboltTimer -= diff;

                if (!Skeletons)
                {
                    if ((SummonSkeletonsTimer <= diff))
                    {
                        Creature* Skeleton;
                        Talk(SAY_SKELETONS);
                        for (uint8 i = 0; i < 5; ++i)
                        {
                            Skeleton = me->SummonCreature(NPC_SKELETON, SkeletonSpawnPoint[i][0], SkeletonSpawnPoint[i][1] , SKELETONSPAWN_Z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000ms);
                            if (Skeleton)
                            {
                                Skeleton->SetWalk(false);
                                Skeleton->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY() , me->GetPositionZ());
                                Skeleton->AddThreat(me->GetVictim(), 0.0f);
                                DoZoneInCombat(Skeleton);
                            }
                        }
                        Skeletons = true;
                    } else SummonSkeletonsTimer -= diff;
                }

                if (FrostTombTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        if (target->IsAlive())
                        {
                            //DoCast(target, SPELL_FROST_TOMB_SUMMON, true);
                            if (Creature* pChains = me->SummonCreature(NPC_FROSTTOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000ms))
                            {
                                CAST_AI(npc_frost_tomb::npc_frost_tombAI, pChains->AI())->SetPrisoner(target);
                                pChains->CastSpell(target, SPELL_FROST_TOMB, true);

                                Talk(SAY_FROST_TOMB);
                                Talk(SAY_FROST_TOMB_EMOTE, target);
                            }
                        }
                    FrostTombTimer = 15000;
                } else FrostTombTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_kelesethAI(creature);
        }
};

class npc_vrykul_skeleton : public CreatureScript
{
    public:
        npc_vrykul_skeleton() : CreatureScript("npc_vrykul_skeleton") { }

        struct npc_vrykul_skeletonAI : public ScriptedAI
        {
            npc_vrykul_skeletonAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 Respawn_Time;
            ObjectGuid Target_Guid;
            uint32 Decrepify_Timer;

            bool isDead;

            void Reset() override
            {
                Respawn_Time = 12000;
                Decrepify_Timer = urand(10000, 20000);
                isDead = false;
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            
            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker->GetGUID() == me->GetGUID())
                    return;

                if (damage >= me->GetHealth())
                {
                    PretendToDie();
                    damage = 0;
                }
            }

            void PretendToDie()
            {
                isDead = true;
                me->InterruptNonMeleeSpells(true);
                me->RemoveAllAuras();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->GetMotionMaster()->MovementExpired(false);
                me->GetMotionMaster()->MoveIdle();
                me->SetStandState(UNIT_STAND_STATE_DEAD);
            };

            void Resurrect()
            {
                isDead = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_SCOURGE_RESSURRECTION, true);

                if (me->GetVictim())
                {
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                    me->AI()->AttackStart(me->GetVictim());
                }
                else
                    me->GetMotionMaster()->Initialize();
            };

            void UpdateAI(uint32 diff) override
            {
                if (instance && instance->GetData(DATA_PRINCEKELESETH_EVENT) == IN_PROGRESS)
                {
                    if (isDead)
                    {
                        if (Respawn_Time <= diff)
                        {
                            Resurrect();
                            Respawn_Time = 12000;
                        } else Respawn_Time -= diff;
                    }
                    else
                    {
                        if (!UpdateVictim())
                            return;

                        if (Decrepify_Timer <= diff)
                        {
                            DoCast(me->GetVictim(), SPELL_DECREPIFY);
                            Decrepify_Timer = 30000;
                        } else Decrepify_Timer -= diff;

                        DoMeleeAttackIfReady();
                    }
                }
                else
                {
                    if (me->IsAlive())
                        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_vrykul_skeletonAI(creature);
        }
};

void AddSC_boss_keleseth()
{
    new boss_keleseth();
    new npc_frost_tomb();
    new npc_vrykul_skeleton();
}
