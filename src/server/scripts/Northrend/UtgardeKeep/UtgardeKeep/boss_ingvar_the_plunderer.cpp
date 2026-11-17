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
SDName: Boss_Ingvar_The_Plunderer
SD%Complete: 95
SDComment: Some Problems with Annhylde Movement, Blizzlike Timers
SDCategory: Udgarde Keep
EndScriptData */

#include "ScriptPCH.h"
#include "utgarde_keep.h"

enum Yells
{
    // Yells Ingvar
    SAY_AGGRO_1                                 = 0,
    SAY_DEAD_1                                  = 1,
    SAY_KILL_1                                  = 2,

    SAY_AGGRO_2                                 = 0,
    SAY_DEAD_2                                  = 1,
    SAY_KILL_2                                  = 2,
};

enum Creatures
{
    NPC_INGVAR_HUMAN                            = 23954,
    NPC_ANNHYLDE_THE_CALLER                     = 24068,
    NPC_INGVAR_UNDEAD                           = 23980,
};

enum Spells
{
    // Ingvar Spells human form
    SPELL_CLEAVE                                = 42724,
    SPELL_SMASH                                 = 42669,
    SPELL_SMASH_H                               = 59706,
    SPELL_STAGGERING_ROAR                       = 42708,
    SPELL_STAGGERING_ROAR_H                     = 59708,
    SPELL_ENRAGE                                = 42705,
    SPELL_ENRAGE_H                              = 59707,

    SPELL_INGVAR_FEIGN_DEATH                    = 42795,
    SPELL_SUMMON_BANSHEE                        = 42912,
    SPELL_SCOURG_RESURRECTION                   = 42863, // Spawn resurrect effect around Ingvar

    // Ingvar Spells undead form
    SPELL_DARK_SMASH                            = 42723,
    SPELL_DREADFUL_ROAR                         = 42729,
    SPELL_DREADFUL_ROAR_H                       = 59734,
    SPELL_WOE_STRIKE                            = 42730,
    SPELL_WOE_STRIKE_H                          = 59735,

    NPC_THROW_TARGET                            = 23996,
    SPELL_SHADOW_AXE_SUMMON                     = 42749
};

class boss_ingvar_the_plunderer : public CreatureScript
{
    public:
        boss_ingvar_the_plunderer() : CreatureScript("boss_ingvar_the_plunderer") { }

        struct boss_ingvar_the_plundererAI : public ScriptedAI
        {
            boss_ingvar_the_plundererAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                bIsUndead = false;
            }

            InstanceScript* instance;

            bool bIsUndead;
            bool bEventInProgress;

            uint32 uiCleaveTimer;
            uint32 uiSmashTimer;
            uint32 uiEnrageTimer;
            uint32 uiRoarTimer;
            uint32 uiSpawnResTimer;

            void Reset() override
            {
                if (bIsUndead)
                    me->UpdateEntry(NPC_INGVAR_HUMAN);

                bIsUndead = false;
                bEventInProgress = false;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);

                uiCleaveTimer = 2000;
                uiSmashTimer = 14000;
                uiEnrageTimer = 10000;
                uiRoarTimer = 20000;

                uiSpawnResTimer = 3000;

                if (instance)
                    instance->SetData(DATA_INGVAR_EVENT, NOT_STARTED);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && !bIsUndead)
                {
                    //DoCast(me, SPELL_INGVAR_FEIGN_DEATH, true);  // Dont work ???
                    // visuel hack
                    me->SetHealth(0);
                    me->InterruptNonMeleeSpells(true);
                    me->RemoveAllAuras();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->GetMotionMaster()->MovementExpired(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    // visuel hack end

                    bEventInProgress = true;
                    bIsUndead = true;

                    Talk(SAY_DEAD_1);
                }

                if (bEventInProgress)
                {
                    damage = 0;
                }
            }

            void StartZombiePhase()
            {
                bIsUndead = true;
                bEventInProgress = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->UpdateEntry(NPC_INGVAR_UNDEAD);
                me->SetInCombatWith(me->GetVictim());
                me->GetMotionMaster()->MoveChase(me->GetVictim());

                Talk(SAY_AGGRO_2);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO_1);

                if (instance)
                    instance->SetData(DATA_INGVAR_EVENT, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEAD_2);

                if (instance)
                    instance->SetData(DATA_INGVAR_EVENT, DONE);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                if (bIsUndead)
                    Talk(SAY_KILL_1);
                else
                    Talk(SAY_KILL_2);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (bEventInProgress)
                {
                    if (uiSpawnResTimer)
                    {
                        if (uiSpawnResTimer <= diff)
                        {
                            DoCast(me, SPELL_SUMMON_BANSHEE); // Summons directly on caster position
                            // DoCast(me, SPELL_SCOURG_RESURRECTION, true); // Not needed ?
                            uiSpawnResTimer = 0;
                        } else uiSpawnResTimer -= diff;
                    }

                    return;
                }

                if (uiCleaveTimer <= diff)
                {
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        if (bIsUndead)
                            DoCast(me->GetVictim(), SPELL_WOE_STRIKE);
                        else
                            DoCast(me->GetVictim(), SPELL_CLEAVE);
                        uiCleaveTimer = rand() % 5000 + 2000;
                    }
                } else uiCleaveTimer -= diff;

                if (uiSmashTimer <= diff)
                {
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        if (bIsUndead)
                            DoCast(me->GetVictim(), SPELL_DARK_SMASH);
                        else
                            DoCast(me->GetVictim(), SPELL_SMASH);
                        uiSmashTimer = urand(7000, 10000);
                    }
                } else uiSmashTimer -= diff;

                if (!bIsUndead)
                {
                    if (uiEnrageTimer <= diff)
                    {
                        DoCast(me, SPELL_ENRAGE);
                        uiEnrageTimer = 10000;
                    } else uiEnrageTimer -= diff;
                } else // In Undead form used to summon weapon
                {
                    if (uiEnrageTimer <= diff)
                    {
                        if (!me->HasUnitState(UNIT_STATE_CASTING))
                        {
                            // Spawn target for Axe
                            Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO, 1);
                            if (target)
                            {
                                me->SummonCreature(NPC_THROW_TARGET,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN,2000ms);

                                DoCast(me, SPELL_SHADOW_AXE_SUMMON);
                            }
                            uiEnrageTimer = 30000;
                        }
                    } else uiEnrageTimer -= diff;
                }

                if (uiRoarTimer <= diff)
                {
                    if (!me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        if (bIsUndead)
                            DoCast(me, SPELL_DREADFUL_ROAR);
                        else
                            DoCast(me, SPELL_STAGGERING_ROAR);
                        uiRoarTimer = 20000;
                    }
                } else uiRoarTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_ingvar_the_plundererAI(creature);
        }
};

enum eYells
{
    SAY_RESSURECT                               = 0,
};

enum eSpells
{
    // Spells for Annhylde
    SPELL_SCOURG_RESURRECTION_HEAL              = 42704, // Heal Max + DummyAura
    SPELL_SCOURG_RESURRECTION_BEAM              = 42857, // Channeling Beam of Annhylde
    SPELL_SCOURG_RESURRECTION_DUMMY             = 42862, // Some Emote Dummy?
    SPELL_INGVAR_TRANSFORM                      = 42796
};

class npc_annhylde_the_caller : public CreatureScript
{
    public:
        npc_annhylde_the_caller() : CreatureScript("npc_annhylde_the_caller") { }

        struct npc_annhylde_the_callerAI : public ScriptedAI
        {
            npc_annhylde_the_callerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            float x,y,z;
            InstanceScript* instance;
            uint32 uiResurectTimer;
            uint32 uiResurectPhase;

            void Reset() override
            {
                me->SetFlying(true);
                me->SetSpeed(MOVE_SWIM , 1.0f);
                me->SetSpeed(MOVE_RUN , 1.0f);
                me->SetSpeed(MOVE_WALK , 1.0f);

                me->GetPosition(x, y, z);
                DoTeleportTo(x + 1, y, z + 30);

                Unit* ingvar = Unit::GetUnit(*me, instance ? instance->GetGuidData(DATA_INGVAR) : ObjectGuid::Empty);
                if (ingvar)
                {
                    me->GetMotionMaster()->MovePoint(1, x, y, z + 15);

                    Talk(SAY_RESSURECT);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                Unit* ingvar = Unit::GetUnit(*me, instance ? instance->GetGuidData(DATA_INGVAR) : ObjectGuid::Empty);
                if (ingvar)
                {
                    switch (pointId)
                    {
                        case 1:
                            ingvar->RemoveAura(SPELL_SUMMON_BANSHEE);
                            ingvar->CastSpell(ingvar,SPELL_SCOURG_RESURRECTION_DUMMY,true);
                            DoCast(ingvar, SPELL_SCOURG_RESURRECTION_BEAM);
                            uiResurectTimer = 8000;
                            uiResurectPhase = 1;
                            break;
                        case 2:
                            me->SetVisible(false);
                            me->DealDamage(me,me->GetHealth());
                            me->RemoveCorpse();
                            break;
                    }
                }
            }

            void AttackStart(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (uiResurectTimer)
                {
                    if (uiResurectTimer <= diff)
                    {
                        if (uiResurectPhase == 1)
                        {
                            Unit* ingvar = Unit::GetUnit(*me, instance ? instance->GetGuidData(DATA_INGVAR) : ObjectGuid::Empty);
                            if (ingvar)
                            {
                                ingvar->SetStandState(UNIT_STAND_STATE_STAND);
                                ingvar->CastSpell(ingvar,SPELL_SCOURG_RESURRECTION_HEAL,false);
                            }
                            uiResurectTimer = 3000;
                            uiResurectPhase = 2;
                        }
                        else if (uiResurectPhase == 2)
                        {
                            if (Creature* ingvar = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_INGVAR) : ObjectGuid::Empty))
                            {
                                ingvar->RemoveAurasDueToSpell(SPELL_SCOURG_RESURRECTION_DUMMY);

                                if (boss_ingvar_the_plunderer::boss_ingvar_the_plundererAI* pAI = CAST_AI(boss_ingvar_the_plunderer::boss_ingvar_the_plundererAI, ingvar->AI()))
                                    if (ingvar->GetVictim())
                                       pAI->StartZombiePhase();

                                me->GetMotionMaster()->MovePoint(2,x+1,y,z+30);
                                ++uiResurectPhase;
                                uiResurectTimer = 0;
                            }
                        }
                    } else uiResurectTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_annhylde_the_callerAI(creature);
        }
};

enum eShadowAxe
{
    SPELL_SHADOW_AXE_DAMAGE                     = 42750,
    SPELL_SHADOW_AXE_DAMAGE_H                   = 59719
};

class npc_ingvar_throw_dummy : public CreatureScript
{
    public:
        npc_ingvar_throw_dummy() : CreatureScript("npc_ingvar_throw_dummy") { }

        struct npc_ingvar_throw_dummyAI : public ScriptedAI
        {
            npc_ingvar_throw_dummyAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 uiDespawnTimer;

            void Reset() override
            {
                Unit* target = me->FindNearestCreature(NPC_THROW_TARGET,50);
                if (target)
                {
                    DoCast(me, SPELL_SHADOW_AXE_DAMAGE);
                    float x,y,z;
                    target->GetPosition(x,y,z);
                    me->GetMotionMaster()->MovePoint(0,x,y,z);
                }
                uiDespawnTimer = 7000;
            }

            void AttackStart(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (uiDespawnTimer <= diff)
                {
                    me->DealDamage(me,me->GetHealth());
                    me->RemoveCorpse();
                    uiDespawnTimer = 0;
                } else uiDespawnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ingvar_throw_dummyAI(creature);
        }
};


void AddSC_boss_ingvar_the_plunderer()
{
    new boss_ingvar_the_plunderer();
    new npc_annhylde_the_caller();
    new npc_ingvar_throw_dummy();
}
