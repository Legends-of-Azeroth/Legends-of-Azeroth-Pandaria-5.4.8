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

#include <algorithm>
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "utgarde_pinnacle.h"

enum Spells
{
    SPELL_ARCING_SMASH                          = 48260,
    SPELL_IMPALE                                = 48261,
    SPELL_IMPALE_H                              = 59268,
    SPELL_WITHERING_ROAR                        = 48256,
    SPELL_WITHERING_ROAR_H                      = 59267,
    SPELL_FREEZE                                = 16245
};

enum OrbSpells
{
    SPELL_ORB_VISUAL                            = 48044,
    SPELL_ORB_CHANNEL                           = 48048,
    SPELL_WAKEUP_GORTOK                         = 47670,
    SPELL_AWAKEN_SUBBOSS                        = 47669,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2
};

enum Creatures
{
    NPC_STASIS_CONTROLLER                       = 26688
};

Position moveLocs[]=
{
    { 261.6f, -449.3f, 109.5f, 0.0f },
    { 263.3f, -454.0f, 109.5f, 0.0f },
    { 291.5f, -450.4f, 109.5f, 0.0f },
    { 291.5f, -454.0f, 109.5f, 0.0f },
    { 310.0f, -453.4f, 109.5f, 0.0f },
    { 238.6f, -460.7f, 109.5f, 0.0f }
};

enum Phase
{
    PHASE_FRENZIED_WORGEN,
    PHASE_RAVENOUS_FURLBORG,
    PHASE_MASSIVE_JORMUNGAR,
    PHASE_FEROCIOUS_RHINO,
    PHASE_GORTOK_PALEHOOF,
    PHASE_NONE
};

class boss_palehoof : public CreatureScript
{
    public:
        boss_palehoof() : CreatureScript("boss_palehoof") { }

        struct boss_palehoofAI : public ScriptedAI
        {
            boss_palehoofAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiArcingSmashTimer;
            uint32 uiImpaleTimer;
            uint32 uiWhiteringRoarTimer;
            uint32 uiWaitingTimer;
            Phase currentPhase;
            uint8 AddCount;
            //bool DoneAdds[4];
            Phase Sequence[4];

            InstanceScript* instance;

            void Reset() override
            {
                /// There is a good reason to store them like this, we are going to shuffle the order.
                for (uint32 i = PHASE_FRENZIED_WORGEN; i < PHASE_GORTOK_PALEHOOF; ++i)
                    Sequence[i] = Phase(i);

                /// This ensures a random order and only executes each phase once.
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(Sequence, Sequence + PHASE_GORTOK_PALEHOOF, g);                

                uiArcingSmashTimer = 7 * IN_MILLISECONDS;
                uiImpaleTimer = 10 * IN_MILLISECONDS;
                uiWhiteringRoarTimer = 15 * IN_MILLISECONDS;

                me->GetMotionMaster()->MoveTargetedHome();
            
                //me->SetReactState(REACT_DEFENSIVE);
     
                //for (uint32 i = 0; i < 4; i++)
                //    DoneAdds[i] = false;
                AddCount = 0;
     
                currentPhase = PHASE_NONE;

                if (instance)
                {
                    instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, NOT_STARTED);

                    //Creature* temp;
                    //if ((temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MOB_FRENZIED_WORGEN))) && !temp->IsAlive())
                    //{
                    //    temp->Respawn();
                    //    temp->AI()->EnterEvadeMode();
                    //}
                    //if ((temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MOB_FEROCIOUS_RHINO))) && !temp->IsAlive())
                    //{
                    //    temp->Respawn();
                    //    temp->AI()->EnterEvadeMode();
                    //}
                    //if ((temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MOB_MASSIVE_JORMUNGAR))) && !temp->IsAlive())
                    //{
                    //    temp->Respawn();
                    //    temp->AI()->EnterEvadeMode();
                    //}
                    //if ((temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_MOB_RAVENOUS_FURBOLG))) && !temp->IsAlive())
                    //{
                    //    temp->Respawn();
                    //    temp->AI()->EnterEvadeMode();
                    //}
                    //if (GameObject* pGo = instance->instance->GetGameObject(instance->GetGuidData(DATA_GORTOK_PALEHOOF_SPHERE)))
                    //{
                    //    pGo->SetGoState(GO_STATE_READY);
                    //    pGo->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    //}

                    Creature* temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_NPC_FRENZIED_WORGEN));
                    if (temp && !temp->IsAlive())
                        temp->Respawn();

                        temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_NPC_FEROCIOUS_RHINO));
                    if (temp && !temp->IsAlive())
                        temp->Respawn();

                        temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_NPC_MASSIVE_JORMUNGAR));
                    if (temp && !temp->IsAlive())
                        temp->Respawn();

                        temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_NPC_RAVENOUS_FURBOLG));
                    if (temp && !temp->IsAlive())
                        temp->Respawn();

                        temp = Unit::GetCreature(*me, instance->GetGuidData(DATA_NPC_ORB));
                    if (temp)
                temp->DespawnOrUnsummon();

                        GameObject* go = instance->instance->GetGameObject(instance->GetGuidData(DATA_GORTOK_PALEHOOF_SPHERE));
                    if (go)
                    {
                        go->SetGoState(GO_STATE_READY);
                    go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                }
            }

            
            //void SetInCombat()
            //{
            //    Map *map = me->GetMap();
            //    if (!map->IsDungeon())
            //        return;
            //
            //    for (auto&& ref : map->GetPlayers())
            //    {
            //        if (Player* i_pl = ref.GetSource())
            //            if (!i_pl->IsGameMaster() && i_pl->IsAlive() && me->GetDistance(i_pl) <= 100)
            //            {
            //                me->SetInCombatWith(i_pl);
            //                i_pl->SetInCombatWith(me);
            //                me->AddThreat(i_pl, 1.0f);
            //            }
            //    }
            //}

            //void JustEngagedWith(Unit* who) override
            //{
            //    DoScriptText(SAY_AGGRO, me);
            //    SetInCombat();
            //}

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);
                Talk(SAY_AGGRO);
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }
     
            void UpdateAI(uint32 diff) override
            {
                if (currentPhase != PHASE_GORTOK_PALEHOOF)
                    return;

                if (!UpdateVictim())
                    return;
            
                Creature* temp = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_ORB) : ObjectGuid::Empty);
                if (temp && temp->IsAlive())
                    temp->DisappearAndDie();
            
                if (uiArcingSmashTimer <= diff)
                {
                    DoCast(me, SPELL_ARCING_SMASH);
                    uiArcingSmashTimer = urand(9 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
                } else uiArcingSmashTimer -= diff;
            
                if (uiImpaleTimer <= diff)
                {
                  if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                      DoCast(target, DUNGEON_MODE(SPELL_IMPALE, SPELL_IMPALE_H));
                  uiImpaleTimer = urand(9 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
                } else uiImpaleTimer -= diff;
            
                if (uiWhiteringRoarTimer <= diff)
                {
                    DoCast(me, DUNGEON_MODE(SPELL_WITHERING_ROAR, SPELL_WITHERING_ROAR_H));
                    uiWhiteringRoarTimer = urand(12 * IN_MILLISECONDS, 16 * IN_MILLISECONDS);
                } else uiWhiteringRoarTimer -= diff;
            
                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                if (instance)
                    instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, DONE);
                Creature* temp = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_ORB) : ObjectGuid::Empty);
                if (temp && temp->IsAlive())
                    temp->DisappearAndDie();
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void NextPhase()
            {
                if (currentPhase == PHASE_NONE)
                {
                    instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, IN_PROGRESS);
                    me->SummonCreature(NPC_STASIS_CONTROLLER, moveLocs[5].GetPositionX(), moveLocs[5].GetPositionY(), moveLocs[5].GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN);
                }
                Phase move = PHASE_NONE;
                if (AddCount >= DUNGEON_MODE(2, 4))
                    move = PHASE_GORTOK_PALEHOOF;
                else
                //{
                //    //select random not yet defeated add
                //    uint8 next = urand(0, 3);
                //    for (uint8 i = 0; i < 16; i++)
                //    {
                //        if (!DoneAdds[i % 4])
                //        {
                //            if (next == 0)
                //            {
                //                move = (Phase)(i % 4);
                //                break;
                //            }
                //            else if (next > 0)
                //                --next;
                //        }
                //    }
                //    ++AddCount;
                //    DoneAdds[move] = true;
                //    move = (Phase)(move % 4);
                //}
                move = Sequence[AddCount++];

                // send orb to summon spot
                Creature* pOrb = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_ORB) : ObjectGuid::Empty);
                if (pOrb && pOrb->IsAlive())
                {
                    if (currentPhase == PHASE_NONE)
                        pOrb->CastSpell(me, SPELL_ORB_VISUAL, true);
                    pOrb->GetMotionMaster()->MovePoint(move, moveLocs[move].GetPositionX(), moveLocs[move].GetPositionY(), moveLocs[move].GetPositionZ());
                }
                currentPhase = move;
            }

            void JustReachedHome() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_NOT_ATTACKABLE_1|UNIT_FLAG_IMMUNE_TO_PC);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_FREEZE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_palehoofAI(creature);
        }
};

enum RavenousSpells
{
    SPELL_CHAIN_LIGHTING                        = 48140,
    SPELL_CHAIN_LIGHTING_H                      = 59273,
    SPELL_CRAZED                                = 48139,
    SPELL_CRAZED_SCRIPT                         = 48146,
    SPELL_CRAZED_TAUNT                          = 48147,
    SPELL_TERRIFYING_ROAR                       = 48144
};

class npc_ravenous_furbolg : public CreatureScript
{
    public:
        npc_ravenous_furbolg() : CreatureScript("npc_ravenous_furbolg") { }

        struct npc_ravenous_furbolgAI : public ScriptedAI
        {
            npc_ravenous_furbolgAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiChainLightingTimer;
            uint32 uiCrazedTimer;
            uint32 uiTerrifyingRoarTimer;

            InstanceScript* instance;

            void Reset() override
            {
                uiChainLightingTimer = 5 * IN_MILLISECONDS;
                uiCrazedTimer = 10 * IN_MILLISECONDS;
                uiTerrifyingRoarTimer = 15 * IN_MILLISECONDS;

                //me->SetReactState(REACT_DEFENSIVE);
            
                me->GetMotionMaster()->MoveTargetedHome();
     
                if (instance)
                    if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT)==IN_PROGRESS)
                    {
                        Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
                        if (pPalehoof && pPalehoof->IsAlive())
                            CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->Reset();
                    }
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_CRAZED_SCRIPT)
                    target->CastSpell(me, SPELL_CRAZED_TAUNT, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiChainLightingTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, DUNGEON_MODE(SPELL_CHAIN_LIGHTING, SPELL_CHAIN_LIGHTING_H));
                    uiChainLightingTimer = 5 * IN_MILLISECONDS + rand() % 5 * IN_MILLISECONDS;
                } else uiChainLightingTimer -=  diff;

                if (uiCrazedTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_CRAZED);
                        uiCrazedTimer = 20 * IN_MILLISECONDS;
                    }
                } else uiCrazedTimer -=  diff;

                if (uiTerrifyingRoarTimer <= diff)
                {
                    DoCast(me, SPELL_TERRIFYING_ROAR);
                    uiTerrifyingRoarTimer = 15 * IN_MILLISECONDS + rand() % 5 * IN_MILLISECONDS;
                } else uiTerrifyingRoarTimer -=  diff;

                DoMeleeAttackIfReady();
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void JustDied(Unit* killer) override
            {
                if (instance)
                    if (Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty))
                        CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->NextPhase();
            }

            void JustReachedHome() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_FREEZE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ravenous_furbolgAI(creature);
        }
};

enum FrenziedSpells
{
    SPELL_MORTAL_WOUND                          = 48137,
    SPELL_MORTAL_WOUND_H                        = 59265,
    SPELL_ENRAGE_1                              = 48138,
    SPELL_ENRAGE_2                              = 48142
};

class npc_frenzied_worgen : public CreatureScript
{
    public:
        npc_frenzied_worgen() : CreatureScript("npc_frenzied_worgen") { }

        struct npc_frenzied_worgenAI : public ScriptedAI
        {
            npc_frenzied_worgenAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiMortalWoundTimer;
            uint32 uiEnrage1Timer;
            bool uiEnraged;

            InstanceScript* instance;

            void Reset() override
            {
                uiMortalWoundTimer = 5 * IN_MILLISECONDS;
                uiEnrage1Timer = 10 * IN_MILLISECONDS;
                uiEnraged = false;

                //me->SetReactState(REACT_DEFENSIVE);
         
                me->GetMotionMaster()->MoveTargetedHome();
     
                if (instance)
                    if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT)==IN_PROGRESS)
                    {
                        Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
                        if (pPalehoof && pPalehoof->IsAlive())
                            CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->Reset();
                    }
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiMortalWoundTimer <= diff)
                {
                    DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_MORTAL_WOUND, SPELL_MORTAL_WOUND_H));
                    uiMortalWoundTimer = 5 * IN_MILLISECONDS + rand() % 4 * IN_MILLISECONDS;
                } else uiMortalWoundTimer -= diff;

                if (uiEnrage1Timer <= diff)
                {
                    DoCast(me, SPELL_ENRAGE_1);
                    uiEnrage1Timer = 15 * IN_MILLISECONDS;
                } else uiEnrage1Timer -= diff;

                if (!uiEnraged && (HealthBelowPct(33)))
                {
                    DoCast(me, SPELL_ENRAGE_2);
                    uiEnraged = true;
                }

                DoMeleeAttackIfReady();
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }

                if (instance)
                    instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    Creature* pPalehoof = Unit::GetCreature(*me, instance->GetGuidData(DATA_GORTOK_PALEHOOF));
                    if (pPalehoof)
                        CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->NextPhase();
                }
            }

            void JustReachedHome() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_FREEZE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_frenzied_worgenAI(creature);
        }
};

enum FerociousSpells
{
    SPELL_GORE                                  = 48130,
    SPELL_GORE_H                                = 59264,
    SPELL_GRIEVOUS_WOUND                        = 48105,
    SPELL_GRIEVOUS_WOUND_H                      = 59263,
    SPELL_STOMP                                 = 48131
};

class npc_ferocious_rhino : public CreatureScript
{
    public:
        npc_ferocious_rhino() : CreatureScript("npc_ferocious_rhino") { }

        struct npc_ferocious_rhinoAI : public ScriptedAI
        {
            npc_ferocious_rhinoAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiStompTimer;
            uint32 uiGoreTimer;
            uint32 uiGrievousWoundTimer;

            InstanceScript* instance;

            void Reset() override
            {
                uiStompTimer = 5 * IN_MILLISECONDS;
                uiGoreTimer = 10 * IN_MILLISECONDS;
                uiGrievousWoundTimer = 12 * IN_MILLISECONDS;
            
                //me->SetReactState(REACT_DEFENSIVE);

                me->GetMotionMaster()->MoveTargetedHome();
     
                if (instance)
                    if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
                    {
                        Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
                        if (pPalehoof && pPalehoof->IsAlive())
                            CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->Reset();
                    }
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiStompTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_STOMP);
                    uiStompTimer = 8 * IN_MILLISECONDS + rand() % 4 * IN_MILLISECONDS;
                } else uiStompTimer -= diff;

                if (uiGoreTimer <= diff)
                {
                    DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_GORE, SPELL_GORE_H));
                    uiGoreTimer = 12 * IN_MILLISECONDS + rand() % 3 * IN_MILLISECONDS;
                } else uiGoreTimer -= diff;

                if (uiGrievousWoundTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                        DoCast(target, DUNGEON_MODE(SPELL_GRIEVOUS_WOUND, SPELL_GRIEVOUS_WOUND_H));
                    uiGrievousWoundTimer = 12 * IN_MILLISECONDS + rand() % 3 * IN_MILLISECONDS;
                } else uiGrievousWoundTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
                    if (pPalehoof)
                        CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->NextPhase();
                }
            }

            void JustReachedHome() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_FREEZE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ferocious_rhinoAI(creature);
        }
};

enum MassiveSpells
{
    SPELL_ACID_SPIT                             = 48132,
    SPELL_ACID_SPLATTER                         = 48136,
    SPELL_ACID_SPLATTER_H                       = 59272,
    SPELL_POISON_BREATH                         = 48133,
    SPELL_POISON_BREATH_H                       = 59271
};

enum MassiveAdds
{
    NPC_JORMUNGAR_WORM                          = 27228
};

class npc_massive_jormungar : public CreatureScript
{
    public:
        npc_massive_jormungar() : CreatureScript("npc_massive_jormungar") { }

        struct npc_massive_jormungarAI : public ScriptedAI
        {
            npc_massive_jormungarAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiAcidSpitTimer;
            uint32 uiAcidSplatterTimer;
            uint32 uiPoisonBreathTimer;

            InstanceScript* instance;

            void Reset() override
            {
                uiAcidSpitTimer = 5 * IN_MILLISECONDS;
                uiAcidSplatterTimer = 12 * IN_MILLISECONDS;
                uiPoisonBreathTimer = 15 * IN_MILLISECONDS;

                me->GetMotionMaster()->MoveTargetedHome();
                //me->SetReactState(REACT_DEFENSIVE);

                if (instance)
                    if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
                    {
                        Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
                        if (pPalehoof && pPalehoof->IsAlive())
                            CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->Reset();
                    }
            }

            void JustEngagedWith(Unit* who) override
            {
                me->GetMotionMaster()->MoveChase(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiAcidSpitTimer <= diff)
                {
                    DoCast(me->GetVictim(), SPELL_ACID_SPIT);
                    uiAcidSpitTimer = 5 * IN_MILLISECONDS + rand() % 3 * IN_MILLISECONDS;
                } else uiAcidSpitTimer -= diff;

                if (uiAcidSplatterTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_ACID_SPLATTER, SPELL_ACID_SPLATTER_H)); //for 2 seconds only?
                        uiAcidSplatterTimer = 25 * IN_MILLISECONDS + rand() % 5 * IN_MILLISECONDS;
                        for (uint8 i = 0; i < 6; ++i)
                            DoSpawnCreature(NPC_JORMUNGAR_WORM, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000ms);
                    }
                } else uiAcidSplatterTimer -= diff;

                if (uiPoisonBreathTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            DoCast(target, DUNGEON_MODE(SPELL_POISON_BREATH, SPELL_POISON_BREATH_H));
                        uiPoisonBreathTimer = 15 * IN_MILLISECONDS + rand() % 5 * IN_MILLISECONDS;
                    }
                } else uiPoisonBreathTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    Creature* pPalehoof = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
                    if (pPalehoof)
                        CAST_AI(boss_palehoof::boss_palehoofAI,pPalehoof->AI())->NextPhase();
                }
            }

            void JustReachedHome() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_FREEZE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_massive_jormungarAI(creature);
        }
};

class npc_palehoof_orb : public CreatureScript
{
    public:
        npc_palehoof_orb() : CreatureScript("npc_palehoof_orb") { }

        struct npc_palehoof_orbAI : public ScriptedAI
        {
            npc_palehoof_orbAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 SummonTimer;
            Phase currentPhase;

            void Reset() override
            {
                currentPhase=PHASE_NONE;
                SummonTimer=5 * IN_MILLISECONDS;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_ATTACKABLE_1|UNIT_FLAG_IMMUNE_TO_PC);
                me->SetDisplayId(11686);
                me->SetFlying(true);
                me->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
                me->SetSpeed(MOVE_RUN, 0.5f);
                //me->SetReactState(REACT_PASSIVE);
            }

            /*void SetInCombat()
            {
                Map *map = me->GetMap();
                if (!map->IsDungeon())
                    return;

                for (auto&& ref : map->GetPlayers())
                {
                    if (Player* i_pl = ref.GetSource())
                        if (!i_pl->IsGameMaster() && i_pl->IsAlive() && me->GetDistance(i_pl) <= 100)
                        {
                            me->SetInCombatWith(i_pl);
                            i_pl->SetInCombatWith(me);
                            me->AddThreat(i_pl, 1.0f);
                        }
                }
            }*/


            void UpdateAI(uint32 diff) override
            {
                if (currentPhase == PHASE_NONE)
                    return;

                if (SummonTimer <= diff)
                {
                    if (currentPhase < 5 && currentPhase >= 0)
                    {
                       Creature* pNext = NULL;
                       switch (currentPhase)
                       {
                           case PHASE_FRENZIED_WORGEN:   pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_FRENZIED_WORGEN)   : ObjectGuid::Empty); break;
                            case PHASE_RAVENOUS_FURLBORG: pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_RAVENOUS_FURBOLG)  : ObjectGuid::Empty); break;
                            case PHASE_MASSIVE_JORMUNGAR: pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_MASSIVE_JORMUNGAR) : ObjectGuid::Empty); break;
                            case PHASE_FEROCIOUS_RHINO:   pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_FEROCIOUS_RHINO)   : ObjectGuid::Empty); break;
                            case PHASE_GORTOK_PALEHOOF:   pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF)       : ObjectGuid::Empty); break;
                            default: break;
                       }

                       if (pNext)
                       {
                            pNext->RemoveAurasDueToSpell(SPELL_FREEZE);
                            pNext->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                            pNext->SetStandState(UNIT_STAND_STATE_STAND);
                            //me->CastSpell(pNext, SPELL_AWAKEN_SUBBOSS, true);
                            pNext->SetInCombatWithZone();
                            pNext->Attack(pNext->SelectNearestTarget(100), true);

                       }
                       currentPhase = PHASE_NONE;
                    }
                } else SummonTimer -= diff;
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId > 4)
                    return;

                Creature* pNext = NULL;
                me->SetSpeed(MOVE_RUN, 3.0f);

                switch (pointId)
                {
                    case PHASE_FRENZIED_WORGEN:   pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_FRENZIED_WORGEN)   : ObjectGuid::Empty); break;
                    case PHASE_RAVENOUS_FURLBORG: pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_RAVENOUS_FURBOLG)  : ObjectGuid::Empty); break;
                    case PHASE_MASSIVE_JORMUNGAR: pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_MASSIVE_JORMUNGAR) : ObjectGuid::Empty); break;
                    case PHASE_FEROCIOUS_RHINO:   pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_NPC_FEROCIOUS_RHINO)   : ObjectGuid::Empty); break;
                    case PHASE_GORTOK_PALEHOOF:   pNext = Unit::GetCreature(*me, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF)       : ObjectGuid::Empty); break;
                    default: break;
                }

                if (pNext)
                    DoCast(pNext, SPELL_ORB_CHANNEL, false);

                //DoCast(pNext, SPELL_AWAKEN_SUBBOSS, false);
                currentPhase = (Phase)pointId;
                SummonTimer = 5 * IN_MILLISECONDS;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_palehoof_orbAI(creature);
        }
};


class go_palehoof_sphere : public GameObjectScript
{
    public:
        go_palehoof_sphere() : GameObjectScript("go_palehoof_sphere") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();

            Creature* pPalehoof = Unit::GetCreature(*go, instance ? instance->GetGuidData(DATA_GORTOK_PALEHOOF) : ObjectGuid::Empty);
            if (pPalehoof && pPalehoof->IsAlive())
            {
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                go->SetGoState(GO_STATE_ACTIVE);

                if (InstanceScript* instance = go->GetInstanceScript())
                    if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
                        return true;

                CAST_AI(boss_palehoof::boss_palehoofAI, pPalehoof->AI())->NextPhase();
            }
            return true;
        }
};


void AddSC_boss_palehoof()
{
    new boss_palehoof();
    new npc_ravenous_furbolg();
    new npc_frenzied_worgen();
    new npc_ferocious_rhino();
    new npc_massive_jormungar();
    new npc_palehoof_orb();
    new go_palehoof_sphere();
}
