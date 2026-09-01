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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ahnkahet.h"

enum Yells
{
    TEXT_AGGRO                                    = 0,
    TEXT_SACRIFICE_1                              = 1,
    TEXT_SACRIFICE_2                              = 2,
    TEXT_SLAY                                     = 3,
    TEXT_DEATH                                    = 4,
    TEXT_PREACHING                                = 5,
};

enum Spells
{
    SPELL_SPHERE_VISUAL                           = 56075,
    SPELL_GIFT_OF_THE_HERALD                      = 56219,
    SPELL_CYCLONE_STRIKE                          = 56855, // Self
    SPELL_CYCLONE_STRIKE_H                        = 60030,
    SPELL_LIGHTNING_BOLT                          = 56891, // 40Y
    SPELL_LIGHTNING_BOLT_H                        = 60032, // 40Y
    SPELL_THUNDERSHOCK                            = 56926, // 30Y
    SPELL_THUNDERSHOCK_H                          = 60029  // 30Y
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

enum Npcs
{
    NPC_JEDOGA_CONTROLLER                         = 30181,
    NPC_TWILIGHT_VOLUNTEER                        = 30385,
};

enum Misc
{
    GO_CIRCLE                                     = 194394,
    VOLUNTEER_COUNT                               = 29,
    WORLD_STATE_VOLUNTEER_WORK                    = 6362,
};

static Locations VolunteerLoc[]=
{
    // 29 Volunteers
    { 365.68f, -735.95f, -16.17f, 1.607f }, // Right, first line
    { 367.12f, -736.13f, -16.17f, 1.607f },
    { 369.03f, -736.06f, -16.17f, 1.607f },
    { 371.66f, -735.97f, -16.17f, 1.607f },
    { 373.47f, -735.63f, -16.17f, 1.607f },

    { 365.45f, -739.03f, -16.00f, 1.607f }, // Right, second line
    { 367.56f, -738.62f, -16.00f, 1.607f },
    { 369.62f, -738.22f, -16.17f, 1.607f },
    { 371.66f, -737.82f, -16.06f, 1.607f },
    { 373.75f, -737.41f, -16.00f, 1.607f },

    { 400.99f, -705.41f, -16.00f, 2.491f }, // Center, from right
    { 398.07f, -710.02f, -16.00f, 2.491f },
    { 395.34f, -713.76f, -16.00f, 2.491f },
    { 393.42f, -716.39f, -16.00f, 2.491f },
    { 391.48f, -718.94f, -16.00f, 2.491f },
    { 388.80f, -722.46f, -16.00f, 2.491f },
    { 386.19f, -725.89f, -16.00f, 2.491f },
    { 383.61f, -729.29f, -16.00f, 2.491f },
    { 380.37f, -733.55f, -16.00f, 2.491f },

    { 402.72f, -700.79f, -16.00f, 3.046f }, // Left, first line
    { 402.63f, -698.86f, -16.18f, 3.149f },
    { 402.62f, -697.10f, -16.17f, 3.149f },
    { 402.61f, -695.50f, -16.17f, 3.059f },
    { 402.20f, -693.39f, -16.17f, 3.159f },

    { 405.31f, -701.29f, -16.00f, 2.924f }, // Left, second line
    { 405.46f, -699.25f, -16.00f, 3.198f },
    { 405.40f, -697.19f, -16.00f, 3.150f },
    { 405.35f, -695.30f, -16.00f, 3.150f },
    { 405.29f, -693.26f, -16.00f, 3.150f }
};

const Position JedogaPosition[2] =
{
    { 372.330994f, -705.278015f, -0.624178f,  5.427970f },
    { 372.330994f, -705.278015f, -16.179716f, 5.427970f }
};

class boss_jedoga_shadowseeker : public CreatureScript
{
    public:
        boss_jedoga_shadowseeker() : CreatureScript("boss_jedoga_shadowseeker") { }

        struct boss_jedoga_shadowseekerAI : public ScriptedAI
        {
            boss_jedoga_shadowseekerAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                bFirstTime = true;
                bPreDone = false;
            }

            InstanceScript* instance;

            uint32 uiCycloneTimer;
            uint32 uiBoltTimer;
            uint32 uiThunderTimer;
            uint32 uiHealthAmountModifier;

            bool bPreDone;
            bool bOpFerok;
            bool bOnGround;
            bool bOpFerokFail;
            bool bCanDown;
            bool volunteerWork;

            bool bFirstTime;

            void Reset() override
            {
                uiCycloneTimer = 3 * IN_MILLISECONDS;
                uiBoltTimer = 7 * IN_MILLISECONDS;
                uiThunderTimer = 12 * IN_MILLISECONDS;
                uiHealthAmountModifier = 1;

                bOpFerok = false;
                bOpFerokFail = false;
                bOnGround = false;
                bCanDown = false;
                volunteerWork = true;

                if (instance)
                {
                    if (!bFirstTime)
                        instance->SetData(DATA_JEDOGA_SHADOWSEEKER_EVENT, FAIL);

                    instance->SetData64(DATA_PL_JEDOGA_TARGET, 0);
                    instance->SetData64(DATA_ADD_JEDOGA_OPFER, 0);
                    instance->SetData(DATA_JEDOGA_RESET_INITIANDS, 0);

                    //lUnitList.clear();
                    //GetCreatureListWithEntryInGrid(lUnitList, me, NPC_TWILIGHT_VOLUNTEER, 100.0f);
                    //if (!lUnitList.empty())
                    //    for (auto&& creature : lUnitList)
                    //        creature->ForcedDespawn(); 
                }
                MoveUp();

                bFirstTime = false;

                me->GetMap()->SetWorldState(WORLD_STATE_VOLUNTEER_WORK, 1);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance || (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_JEDOGA_CONTROLLER))
                    return;

                Talk(TEXT_AGGRO);
                me->SetInCombatWithZone();
                instance->SetData(DATA_JEDOGA_SHADOWSEEKER_EVENT, IN_PROGRESS);

                //for (int i = 0; i < VOLUNTEER_COUNT; i++)
                //    if (Creature* pTemp = me->SummonCreature(NPC_TWILIGHT_VOLUNTEER, VolunteerLoc[i].x, VolunteerLoc[i].y, VolunteerLoc[i].z, VolunteerLoc[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0ms))
                //        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void AttackStart(Unit* who) override
            {
                if (!who || (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_JEDOGA_CONTROLLER))
                    return;

                ScriptedAI::AttackStart(who);
            }

            void KilledUnit(Unit* victim) override
            {
                if (!victim || victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(TEXT_SLAY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(TEXT_DEATH);

                if (instance)
                    instance->SetData(DATA_JEDOGA_SHADOWSEEKER_EVENT, DONE);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance || !who || (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_JEDOGA_CONTROLLER))
                    return;

                if (!bPreDone && who->GetTypeId() == TYPEID_PLAYER && me->GetDistance(who) < 100.0f)
                {
                    Talk(TEXT_PREACHING);
                    bPreDone = true;
                }

                if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS || !bOnGround)
                    return;

                //if (!me->GetVictim() && me->canCreatureAttack(who))
                if (!me->GetVictim() && who->isTargetableForAttack() && me->IsHostileTo(who) && who->isInAccessiblePlaceFor(me))
                {
                    float attackRadius = me->GetAttackDistance(who);
                    if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
                    {
                        if (!me->GetVictim())
                        {
                            who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                            AttackStart(who);
                        }
                        else if (me->GetMap()->IsDungeon())
                        {
                            who->SetInCombatWith(me);
                            me->AddThreat(who, 0.0f);
                        }
                    }
                }
            }

            void MoveDown()
            {
                if (!instance)
                    return;

                bOpFerokFail = false;

                instance->SetData(DATA_JEDOGA_TRIGGER_SWITCH, 0);
                me->GetMotionMaster()->MovePoint(1, JedogaPosition[1]);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);

                //GameObject* pCircle = GetClosestGameObjectWithEntry(me,GO_CIRCLE,50.0f);
                //if (pCircle && pCircle->isSpawned())
                //    pCircle->Delete();

                bOnGround = true;

                if (UpdateVictim())
                {
                    AttackStart(me->GetVictim());
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                }
                else
                {
                    if (Unit* target = Unit::GetUnit(*me, instance->GetGuidData(DATA_PL_JEDOGA_TARGET)))
                    {
                        AttackStart(target);
                        instance->SetData(DATA_JEDOGA_RESET_INITIANDS, 0);
                        if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS)
                            JustEngagedWith(target);
                    }
                    else if (!me->IsInCombat())
                        EnterEvadeMode();
                }
            }

            void MoveUp()
            {
                if (!instance)
                    return;

                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                me->AttackStop();
                me->RemoveAllAuras();
                me->LoadCreaturesAddon();
                me->GetMotionMaster()->MovePoint(0, JedogaPosition[0]);

                GameObject* pCircle = GetClosestGameObjectWithEntry(me, GO_CIRCLE, 50.0f);
                if (pCircle && !pCircle->isSpawned())
                        pCircle->SetRespawnTime(1000);

                instance->SetData(DATA_JEDOGA_TRIGGER_SWITCH, 1);
                if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS)
                 OpferRufen();

                bOnGround = false;
            }

            void OpferRufen()
            {
                if (!instance)
                    return;

                ObjectGuid opfer = instance->GetGuidData(DATA_ADD_JEDOGA_INITIAND);

                if (opfer)
                {
                    Talk(TEXT_SACRIFICE_1);
                    instance->SetData64(DATA_ADD_JEDOGA_OPFER, opfer);
                } else
                    bCanDown = true;
            }

            void Opfern()
            {
                Talk(TEXT_SACRIFICE_2);

                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_GIFT_OF_THE_HERALD, false);

                bOpFerok = false;
                bCanDown = true;
                //volunteerWork = false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS && instance->GetData(DATA_ALL_INITIAND_DEAD))
                    MoveDown();

                if (bOpFerok && !bOnGround && !bCanDown) 
                    Opfern();

                if (bOpFerokFail && !bOnGround && !bCanDown)
                    bCanDown = true;

                if (bCanDown)
                {
                    MoveDown();
                    bCanDown = false;
                }

                if (bOnGround)
                {
                    if (!UpdateVictim())
                        return;

                    if (uiCycloneTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCast(me, DUNGEON_MODE(SPELL_CYCLONE_STRIKE, SPELL_CYCLONE_STRIKE_H), false);
                            uiCycloneTimer = urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS);
                        }
                    } else uiCycloneTimer -= diff;

                    if (uiBoltTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, -5, true))
                                me->CastSpell(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT, SPELL_LIGHTNING_BOLT_H), false);

                            uiBoltTimer = urand(8 * IN_MILLISECONDS,12 * IN_MILLISECONDS);
                        }
                    } else uiBoltTimer -= diff;

                    if (uiThunderTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                                me->CastSpell(target, DUNGEON_MODE(SPELL_THUNDERSHOCK, SPELL_THUNDERSHOCK_H), false);

                            uiThunderTimer = urand(15 * IN_MILLISECONDS,20 * IN_MILLISECONDS);
                        }
                    } else uiThunderTimer -= diff;

                    if (me->HealthBelowPct(100 - 25 * uiHealthAmountModifier))
                    {
                        ++uiHealthAmountModifier;
                        MoveUp();
                    }

                    EnterEvadeIfOutOfCombatArea(diff);
                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_jedoga_shadowseekerAI(creature);
        }
};

class npc_jedoga_initiand : public CreatureScript
{
    public:
        npc_jedoga_initiand() : CreatureScript("npc_jedoga_initiand") { }

        struct npc_jedoga_initiandAI : public ScriptedAI
        {
            npc_jedoga_initiandAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 bCheckTimer;
            bool bWalking;

            void Reset() override
            {
                if (!instance)
                    return;

                bWalking = false;
                bCheckTimer = 2 * IN_MILLISECONDS;

                if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS)
                {
                    me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                }
                else
                {
                    DoCast(me, SPELL_SPHERE_VISUAL, false);
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            void JustDied(Unit* killer) override
            {
                if (!killer || !instance)
                    return;

                if (bWalking)
                {
                    if (Creature* boss = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_JEDOGA_SHADOWSEEKER)))
                    {
                        if (!CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerok)
                            CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerokFail = true;

                        if (killer->GetTypeId() == TYPEID_PLAYER)
                            me->GetMap()->SetWorldState(WORLD_STATE_VOLUNTEER_WORK, 0);
                    }

                    instance->SetData64(DATA_ADD_JEDOGA_OPFER, 0);

                    bWalking = false;
                }
                if (killer->GetTypeId() == TYPEID_PLAYER)
                    instance->SetData64(DATA_PL_JEDOGA_TARGET, killer->GetGUID());
            }

            void JustEngagedWith(Unit* who) override
            {
                if ((instance && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS) || !who)
                    return;
            }

            void AttackStart(Unit* who) override
            {
                if ((instance && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS) || !who)
                    return;

                ScriptedAI::AttackStart(who);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if ((instance && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS) || !who)
                    return;

                ScriptedAI::MoveInLineOfSight(who);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || !instance)
                    return;

                switch (pointId)
                {
                    case 1:
                        {
                            Creature* boss = me->GetMap()->GetCreature(instance->GetGuidData(DATA_JEDOGA_SHADOWSEEKER));
                            if (boss)
                            {
                                CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerok = true;
                                CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerokFail = false;

                                // sacrifice health transfer
                                if ((boss->GetHealth() + me->GetHealth()) >= boss->GetMaxHealth())
                                  boss->SetFullHealth();
                                else
                                  boss->SetHealth(boss->GetHealth() + me->GetHealth());

                                me->Kill(me);
                            }
                        }
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (instance && bCheckTimer <= diff)
                {
                    if (me->GetGUID() == instance->GetGuidData(DATA_ADD_JEDOGA_OPFER) && !bWalking)
                    {
                        me->SetMaxHealth(DUNGEON_MODE(25705, 58648)); // TODO: implement npc entry 30385
                        me->SetHealth(DUNGEON_MODE(25705, 58648));
                        me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                        float distance = me->GetDistance(JedogaPosition[1]);

                        if (distance < 9.0f)
                            me->SetSpeed(MOVE_WALK, 0.5f, true);
                        else if (distance < 15.0f)
                            me->SetSpeed(MOVE_WALK, 0.75f, true);
                        else if (distance < 20.0f)
                            me->SetSpeed(MOVE_WALK, 1.0f, true);

                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MovePoint(1, JedogaPosition[1]);
                        bWalking = true;
                    }
                    if (!bWalking)
                    {
                        if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS && me->HasAura(SPELL_SPHERE_VISUAL))
                        {
                            me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        }
                        if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS && !me->HasAura(SPELL_SPHERE_VISUAL))
                        {
                            DoCast(me, SPELL_SPHERE_VISUAL, false);
                            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
                            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        }
                    }
                    bCheckTimer = 2 * IN_MILLISECONDS;
                } else bCheckTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jedoga_initiandAI(creature);
        }
};

enum AufseherSpell
{
  SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_1            = 60342,
  SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_2            = 56312
};

class npc_jedogas_aufseher_trigger : public CreatureScript
{
    public:
        npc_jedogas_aufseher_trigger() : CreatureScript("npc_jedogas_aufseher_trigger") { }

        struct npc_jedogas_aufseher_triggerAI : public ScriptedAI
        {
            npc_jedogas_aufseher_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); //database?
                instance = creature->GetInstanceScript();
                bRemoved = false;
                bRemoved2 = false;
                bCasted = false;
                bCasted2 = false;
                SetCombatMovement(false);
            }

            InstanceScript* instance;
            bool bRemoved;
            bool bRemoved2;
            bool bCasted;
            bool bCasted2;

            void Reset() override { }

            void JustEngagedWith(Unit* /*who*/) override { }

            void AttackStart(Unit* /*who*/) override { }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!instance)
                    return;

                if (!bRemoved && me->GetPositionX() > 440.0f)
                {
                    if (instance->GetData(DATA_PRINCE_TALDARAM_EVENT) == DONE)
                    {
                        me->InterruptNonMeleeSpells(true);
                        bRemoved = true;
                        return;
                    }
                    if (!bCasted)
                    {
                        DoCast(me, SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_1, false);
                        bCasted = true;
                    }
                }
                if (!bRemoved2 && me->GetPositionX() < 440.0f)
                {
                    if (!bCasted2 && instance->GetData(DATA_JEDOGA_TRIGGER_SWITCH))
                    {
                        DoCast(me, SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_2, false);
                        bCasted2 = true;
                    }
                    if (bCasted2 && !instance->GetData(DATA_JEDOGA_TRIGGER_SWITCH))
                    {
                        me->InterruptNonMeleeSpells(true);
                        bCasted2 = false;
                    }
                    if (!bRemoved2 && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == DONE)
                    {
                        me->InterruptNonMeleeSpells(true);
                        bRemoved2 = true;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jedogas_aufseher_triggerAI(creature);
        }
};

void AddSC_boss_jedoga_shadowseeker()
{
    new boss_jedoga_shadowseeker();
    new npc_jedoga_initiand();
    new npc_jedogas_aufseher_trigger();
}
