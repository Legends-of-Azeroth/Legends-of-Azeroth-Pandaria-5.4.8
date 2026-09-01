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

#include "ScriptPCH.h"
#include "ahnkahet.h"

enum Spells
{
    SPELL_BLOODTHIRST                             = 55968, // Trigger Spell + add aura
    SPELL_CONJURE_FLAME_SPHERE                    = 55931,
    SPELL_FLAME_SPHERE_SUMMON_1                   = 55895, // 1x 30106
    SPELL_FLAME_SPHERE_SUMMON_1_H                 = 59511, // 1x 31686
    SPELL_FLAME_SPHERE_SUMMON_2_H                 = 59512, // 1x 31687
    SPELL_FLAME_SPHERE_SPAWN_EFFECT               = 55891,
    SPELL_FLAME_SPHERE_VISUAL                     = 55928,
    SPELL_FLAME_SPHERE_PERIODIC                   = 55926,
    SPELL_FLAME_SPHERE_PERIODIC_H                 = 59508,
    SPELL_FLAME_SPHERE_DEATH_EFFECT               = 55947,
    SPELL_BEAM_VISUAL                             = 60342,
    SPELL_EMBRACE_OF_THE_VAMPYR                   = 55959,
    SPELL_EMBRACE_OF_THE_VAMPYR_H                 = 59513,
    SPELL_VANISH                                  = 55964,
    NPC_FLAME_SPHERE                              = 30106,
    NPC_FLAME_SPHERE_1_H                          = 31686,
    NPC_FLAME_SPHERE_2_H                          = 31687
};

enum Misc
{
    DATA_EMBRACE_DMG                              = 20000,
    DATA_EMBRACE_DMG_H                            = 40000,
    DATA_SPHERE_DISTANCE                          = 60,
};

#define DATA_SPHERE_ANGLE_OFFSET 2.1f
#define DATA_GROUND_POSITION_Z 11.4f

enum Yells
{
    SAY_SPHERE                                    = 0,
    SAY_WARNING                                   = 1,
    SAY_AGGRO                                     = 2,
    SAY_SLAY                                      = 3,
    SAY_DEATH                                     = 4,
    SAY_FEED                                      = 5,
    SAY_VANISH                                    = 6
};

enum CombatPhase
{
    NORMAL,
    CASTING_FLAME_SPHERES,
    JUST_VANISHED,
    VANISHED,
    FEEDING
};

const Position HomePosition = { 528.691f,-845.629f,11.308f, 0.68f };

class boss_taldaram : public CreatureScript
{
    public:
        boss_taldaram() : CreatureScript("boss_taldaram") { }

        struct boss_taldaramAI : public ScriptedAI
        {
            boss_taldaramAI(Creature* creature) : ScriptedAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
                // SPELL_BLOODTHIRST should trigger effect 1 on self
                // TODO: move to core
                // below may need another adjustment
                //TempSpell = GET_SPELL(DUNGEON_MODE(SPELL_FLAME_SPHERE_PERIODIC, SPELL_FLAME_SPHERE_PERIODIC_H));
                //if (TempSpell)
                //    TempSpell->EffectAmplitude[0] = 500;
            }

            bool warning = false;
            uint32 uiBloodthirstTimer;
            uint32 uiVanishTimer;
            uint32 uiWaitTimer;
            uint32 uiEmbraceTimer;
            uint32 uiEmbraceTakenDamage;
            uint32 uiFlamesphereTimer;
            uint32 uiPhaseTimer;

            ObjectGuid uiEmbraceTarget;

            CombatPhase Phase;

            InstanceScript* instance;
            SummonList Summons;

            void JustSummoned(Creature* summon) override
            {
                Summons.Summon(summon);
            }

            void SummonedCreateDespawn(Creature* summon)
            {
                Summons.Despawn(summon);
            }

            void Reset() override
            {
                Summons.DespawnAll();
                uiBloodthirstTimer = 10 * IN_MILLISECONDS;
                uiVanishTimer = urand(25 * IN_MILLISECONDS,35 * IN_MILLISECONDS);
                uiEmbraceTimer = 20 * IN_MILLISECONDS;
                uiFlamesphereTimer = 5 * IN_MILLISECONDS;
                uiEmbraceTakenDamage = 0;
                Phase = NORMAL;
                uiPhaseTimer = 0;
                uiEmbraceTarget = ObjectGuid::Empty;

                if (instance)
                instance->SetData(DATA_PRINCE_TALDARAM_EVENT, NOT_STARTED);

                if (!CheckSpheres())
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetData(DATA_PRINCE_TALDARAM_EVENT, IN_PROGRESS);
                me->RemoveAurasDueToSpell(SPELL_BEAM_VISUAL);
                Talk(SAY_AGGRO);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                                        
                if (uiPhaseTimer <= diff)
                {
                    switch (Phase)
                    {
                        case CASTING_FLAME_SPHERES:
                        {
                            Creature* pSpheres[3];

                            //DoCast(me, SPELL_FLAME_SPHERE_SUMMON_1);
                            pSpheres[0] = DoSpawnCreature(NPC_FLAME_SPHERE, 0, 0, 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10s);
                            Unit* pSphereTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                            if (pSphereTarget && pSpheres[0])
                            {
                                float angle,x,y;
                                angle = pSpheres[0]->GetAngle(pSphereTarget);
                                x = pSpheres[0]->GetPositionX() + DATA_SPHERE_DISTANCE * cos(angle);
                                y = pSpheres[0]->GetPositionY() + DATA_SPHERE_DISTANCE * sin(angle);
                                pSpheres[0]->GetMotionMaster()->MovePoint(0, x, y, pSpheres[0]->GetPositionZ());
                            }
                            if (IsHeroic())
                            {
                                //DoCast(me, SPELL_FLAME_SPHERE_SUMMON_1_H);
                                pSpheres[1] = DoSpawnCreature(NPC_FLAME_SPHERE_1_H, 0, 0, 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10s);
                                //DoCast(me, SPELL_FLAME_SPHERE_SUMMON_2_H);
                                pSpheres[2] = DoSpawnCreature(NPC_FLAME_SPHERE_2_H, 0, 0, 5, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10s);
                                if (pSphereTarget && pSpheres[1] && pSpheres[2])
                                {
                                    float angle,x,y;
                                    angle = pSpheres[1]->GetAngle(pSphereTarget) + DATA_SPHERE_ANGLE_OFFSET;
                                    x = pSpheres[1]->GetPositionX() + DATA_SPHERE_DISTANCE / 2 * cos(angle);
                                    y = pSpheres[1]->GetPositionY() + DATA_SPHERE_DISTANCE / 2 * sin(angle);
                                    pSpheres[1]->GetMotionMaster()->MovePoint(0, x, y, pSpheres[1]->GetPositionZ(), false);
                                    angle = pSpheres[2]->GetAngle(pSphereTarget) - DATA_SPHERE_ANGLE_OFFSET;
                                    x = pSpheres[2]->GetPositionX() + DATA_SPHERE_DISTANCE / 2 * cos(angle);
                                    y = pSpheres[2]->GetPositionY() + DATA_SPHERE_DISTANCE / 2 * sin(angle);
                                    pSpheres[2]->GetMotionMaster()->MovePoint(0, x, y, pSpheres[2]->GetPositionZ(), false);
                                }
                            }

                            Phase = NORMAL;
                            uiPhaseTimer = 0;
                            break;
                        }
                        case JUST_VANISHED:
                            if (Unit* pEmbraceTarget = GetEmbraceTarget())
                            {
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveChase(pEmbraceTarget);
                            }
                            Phase = VANISHED;
                            uiPhaseTimer = 1300;
                            break;
                        case VANISHED:
                            me->SetVisible(true);
                            if (Unit* pEmbraceTarget = GetEmbraceTarget())
                                DoCast(pEmbraceTarget, DUNGEON_MODE(SPELL_EMBRACE_OF_THE_VAMPYR, SPELL_EMBRACE_OF_THE_VAMPYR_H));
                            Talk(SAY_FEED);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            Phase = FEEDING;
                            uiPhaseTimer = 20 * IN_MILLISECONDS;
                            break;
                        case FEEDING:
                            Phase = NORMAL;
                            uiPhaseTimer = 0;
                            uiEmbraceTarget = ObjectGuid::Empty;
                            break;
                        case NORMAL:
                            if (uiBloodthirstTimer <= diff)
                            {
                                DoCast(me->GetVictim(), SPELL_BLOODTHIRST);
                                uiBloodthirstTimer = 10 * IN_MILLISECONDS;
                            } else uiBloodthirstTimer -= diff;

                            if (uiFlamesphereTimer <= diff)
                            {
                                DoCastVictim(SPELL_CONJURE_FLAME_SPHERE);
                                Phase = CASTING_FLAME_SPHERES;
                                uiPhaseTimer = 1;
                                uiFlamesphereTimer = 15 * IN_MILLISECONDS;
                            } else uiFlamesphereTimer -= diff;

                            if (uiVanishTimer <= diff)
                            {
                                // Count alive players
                                Unit* target = NULL;
                                std::vector<Unit*> target_list;
                                for (auto&& threat : me->GetThreatManager().getThreatList())
                                {
                                    target = Unit::GetUnit(*me, threat->getUnitGuid());
                                    // exclude pets & totems
                                    if (target && target->GetTypeId() == TYPEID_PLAYER && target->IsAlive())
                                        target_list.push_back(target);
                                    target = NULL;
                                }
                                // He only vanishes if there are 3 or more alive players
                                if (target_list.size() > 2)
                                {
                                    Talk(SAY_VANISH);
                                    //DoCast(me, SPELL_VANISH); // causes health reset
                                    me->SetVisible(false);
                                    me->SetInCombatWithZone();
                                    Phase = JUST_VANISHED;
                                    uiPhaseTimer = 500;
                                    if (Unit* pEmbraceTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                        uiEmbraceTarget = pEmbraceTarget->GetGUID();

                                }
                                uiVanishTimer = urand(25 * IN_MILLISECONDS,35 * IN_MILLISECONDS);
                            } else uiVanishTimer -= diff;

                            EnterEvadeIfOutOfCombatArea(diff);
                            DoMeleeAttackIfReady();
                        break;
                    }
                } else uiPhaseTimer -= diff;
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                Unit* pEmbraceTarget = GetEmbraceTarget();

                if (Phase == FEEDING && pEmbraceTarget && pEmbraceTarget->IsAlive())
                {
                  uiEmbraceTakenDamage += damage;
                  if (uiEmbraceTakenDamage > (uint32) DUNGEON_MODE(DATA_EMBRACE_DMG, DATA_EMBRACE_DMG_H))
                  {
                      Phase = NORMAL;
                      uiPhaseTimer = 0;
                      uiEmbraceTarget = ObjectGuid::Empty;
                      me->CastStop();
                  }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                Summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_PRINCE_TALDARAM_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Unit* pEmbraceTarget = GetEmbraceTarget();
                if (Phase == FEEDING && pEmbraceTarget && victim == pEmbraceTarget)
                {
                    Phase = NORMAL;
                    uiPhaseTimer = 0;
                    uiEmbraceTarget = ObjectGuid::Empty;
                }
                Talk(SAY_SLAY);
            }

            bool CheckSpheres()
            {
                if (!instance)
                    return false;

                ObjectGuid uiSphereGuids[2];
                uiSphereGuids[0] = instance->GetGuidData(DATA_SPHERE1);
                uiSphereGuids[1] = instance->GetGuidData(DATA_SPHERE2);

                for (uint8 i=0; i < 2; ++i)
                {
                    GameObject* pSpheres = instance->instance->GetGameObject(uiSphereGuids[i]);
                    if (!pSpheres)
                        return false;
                    if (pSpheres->GetGoState() != GO_STATE_ACTIVE)
                        return false;
                }
                RemovePrison();
                return true;
            }

            Unit* GetEmbraceTarget()
            {
                if (!uiEmbraceTarget)
                    return NULL;

                return Unit::GetUnit(*me, uiEmbraceTarget);
            }

            void RemovePrison()
            {
                if (!instance)
                    return;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveAurasDueToSpell(SPELL_BEAM_VISUAL);
                me->SetWalk(false);
                me->SetHomePosition(HomePosition);
                me->GetMotionMaster()->MoveLand(0, HomePosition);
                if (!warning)
                {
                    Talk(SAY_WARNING);
                    warning = true;
                }
                ObjectGuid prison_GUID = instance->GetGuidData(DATA_PRINCE_TALDARAM_PLATFORM);
                instance->HandleGameObject(prison_GUID,true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_taldaramAI(creature);
        }
};

class npc_taldaram_flamesphere : public CreatureScript
{
    public:
        npc_taldaram_flamesphere() : CreatureScript("npc_taldaram_flamesphere") { }

        struct npc_taldaram_flamesphereAI : public ScriptedAI
        {
            npc_taldaram_flamesphereAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 uiDespawnTimer;
            InstanceScript* instance;

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlying(true);
                me->SetDisplayFromModel(0);
                me->SetFaction(16);
                me->SetObjectScale(1.0f);
                DoCast(me, SPELL_FLAME_SPHERE_VISUAL);
                DoCast(me, SPELL_FLAME_SPHERE_SPAWN_EFFECT);
                DoCast(me, DUNGEON_MODE(SPELL_FLAME_SPHERE_PERIODIC, SPELL_FLAME_SPHERE_PERIODIC_H));
                uiDespawnTimer = 10 * IN_MILLISECONDS;
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_FLAME_SPHERE_DEATH_EFFECT);
            }

            void UpdateAI(uint32 diff) override
            {
                if (uiDespawnTimer <= diff)
                    me->DisappearAndDie();
                else
                    uiDespawnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_taldaram_flamesphereAI(creature);
        }
};

class go_prince_taldaram_sphere : public GameObjectScript
{
    public:
        go_prince_taldaram_sphere() : GameObjectScript("go_prince_taldaram_sphere") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go)
        {
            InstanceScript* instance = go->GetInstanceScript();

            Creature* pPrinceTaldaram = Unit::GetCreature(*go, instance ? instance->GetGuidData(DATA_PRINCE_TALDARAM) : ObjectGuid::Empty);
            if (pPrinceTaldaram && pPrinceTaldaram->IsAlive())
            {
                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                go->SetGoState(GO_STATE_ACTIVE);

                if (instance)
                {
                    switch (go->GetEntry())
                    {
                        case GO_PRINCE_TALDARAM_SPHERE_1:
                            instance->SetData(DATA_SPHERE1_EVENT, IN_PROGRESS);
                            instance->SetData64(DATA_SPHERE1, go->GetGUID());
                            pPrinceTaldaram->AI()->Talk(SAY_SPHERE);
                            break;
                        case GO_PRINCE_TALDARAM_SPHERE_2:
                            instance->SetData(DATA_SPHERE2_EVENT, IN_PROGRESS);
                            instance->SetData64(DATA_SPHERE2, go->GetGUID());
                            pPrinceTaldaram->AI()->Talk(SAY_SPHERE);
                            break;
                    }
                }

                 CAST_AI(boss_taldaram::boss_taldaramAI, pPrinceTaldaram->AI())->CheckSpheres();
             }
            return true;
        }
};

void AddSC_boss_taldaram()
{
    new boss_taldaram();
    new npc_taldaram_flamesphere();
    new go_prince_taldaram_sphere();
}
