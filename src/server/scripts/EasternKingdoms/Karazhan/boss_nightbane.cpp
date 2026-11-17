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
#include "karazhan.h"

enum Spells
{
    // phase 1
    SPELL_BELLOWING_ROAR        = 39427,
    SPELL_CHARRED_EARTH         = 30129,
    SPELL_DISTRACTING_ASH       = 30130,
    SPELL_SMOLDERING_BREATH     = 30210,
    SPELL_TAIL_SWEEP            = 25653,
    // phase 2
    SPELL_RAIN_OF_BONES         = 37098,
    SPELL_SMOKING_BLAST         = 37057,
    SPELL_FIREBALL_BARRAGE      = 30282,
    SPELL_SEARING_CINDERS       = 30127,
    SPELL_SUMMON_SKELETON       = 30170,
};

enum Says
{
    EMOTE_SUMMON                = 0,
    YELL_AGGRO                  = 1,
    YELL_FLY_PHASE              = 2,
    YELL_LAND_PHASE             = 3,
    EMOTE_BREATH                = 4,
};

#define QUEST_NIGHTBANE             9644

Position IntroWay[8] =
{
    { -11053.37f, -1794.48f, 149.00f },
    { -11141.07f, -1841.40f, 125.00f },
    { -11187.28f, -1890.23f, 125.00f },
    { -11189.20f, -1931.25f, 125.00f },
    { -11153.76f, -1948.93f, 125.00f },
    { -11128.73f, -1929.75f, 125.00f },
    { -11140.00f, -1915.00f, 122.00f },
    { -11163.00f, -1903.00f, 91.473f },
};

class boss_nightbane : public CreatureScript
{
    public:
        boss_nightbane() : CreatureScript("boss_nightbane") { }

        struct boss_nightbaneAI : public ScriptedAI
        {
            boss_nightbaneAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
                Intro = true;
            }

            InstanceScript* instance;

            uint32 BellowingRoarTimer;
            uint32 CharredEarthTimer;
            uint32 DistractingAshTimer;
            uint32 SmolderingBreathTimer;
            uint32 TailSweepTimer;
            uint32 RainofBonesTimer;
            uint32 SmokingBlastTimer;
            uint32 FireballBarrageTimer;
            uint32 SearingCindersTimer;

            uint32 Phase;
            uint8 MovePhase;
            uint8 FlyCount;
            uint32 FlyTimer;

            bool RainBones;
            bool Skeletons;
            bool Intro;
            bool Flying;
            bool Movement;

            void Reset() override
            {
                BellowingRoarTimer = 30000;
                CharredEarthTimer = 15000;
                DistractingAshTimer = 20000;
                SmolderingBreathTimer = 10000;
                TailSweepTimer = 12000;
                RainofBonesTimer = 10000;
                SmokingBlastTimer = 20000;
                FireballBarrageTimer = 13000;
                SearingCindersTimer = 14000;

                Phase = 1;
                FlyCount = 0;
                MovePhase = 0;
                Movement = false;
                Flying = false;

                me->SetSpeed(MOVE_RUN, 2.0f);
                me->SetFlying(true);
                me->SetWalk(false);
                me->OverrideInhabitType(INHABIT_AIR);
                me->setActive(true);
                me->SetStandState(UNIT_STAND_STATE_STAND);

                if (instance)
                {
                    if (instance->GetData(TYPE_NIGHTBANE) == DONE || instance->GetData(TYPE_NIGHTBANE) == IN_PROGRESS)
                        me->DespawnOrUnsummon();
                    if (instance->GetData(TYPE_NIGHTBANE) != DONE)
                        instance->SetData(TYPE_NIGHTBANE, NOT_STARTED);
                }

                HandleTerraceDoors(true);
                me->SetReactState(REACT_PASSIVE);

                if (!Intro)
                {
                    me->SetHomePosition(IntroWay[7]);
                    me->GetMotionMaster()->MoveTargetedHome();
                }
                else
                    Talk(EMOTE_SUMMON);
            }

            void HandleTerraceDoors(bool open)
            {
                if (instance)
                {
                    instance->HandleGameObject(instance->GetGuidData(DATA_MASTERS_TERRACE_DOOR_1), open);
                    instance->HandleGameObject(instance->GetGuidData(DATA_MASTERS_TERRACE_DOOR_2), open);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SetData(TYPE_NIGHTBANE, IN_PROGRESS);
                HandleTerraceDoors(false);
                Talk(YELL_AGGRO);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetData(TYPE_NIGHTBANE, DONE);

                HandleTerraceDoors(true);
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE || id == 0)
                    return;

                if (id == 9)
                {
                    // EffectMovementGenerator will start chase movement before this point
                    me->StopMoving();
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                    Talk(EMOTE_BREATH);
                    Phase = 2;
                    return;
                }

                if (id == 8)
                {
                    if (Phase == 2)
                    {
                        Flying = false;
                        Phase = 1;
                        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
                    }

                    DoResetThreat();
                    me->SetFlying(false);
                    me->ResetInhabitTypeOverride();
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNK_6);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetInCombatWithZone();
                    if (me->GetVictim())
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    return;
                }

                ++MovePhase;
                Movement = true;
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetInCombatWithZone();
            }

            void TakeOff()
            {
                Talk(YELL_FLY_PHASE);
                me->SetReactState(REACT_PASSIVE);
                me->InterruptSpell(CURRENT_GENERIC_SPELL);
                me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                me->SetFlying(true);
                me->OverrideInhabitType(INHABIT_AIR);
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveTakeoff(8 + 1, IntroWay[2]);

                FlyTimer = urand(45000, 60000);
                ++FlyCount;

                RainofBonesTimer = 5000;
                RainBones = false;
                Skeletons = false;

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
             }

            void UpdateAI(uint32 diff) override
            {
                if (Intro && !Movement)
                {
                    Intro = false;
                    Movement = true;
                }

                if (Movement)
                {
                    Movement = false;
                    if (MovePhase == 7)
                        me->GetMotionMaster()->MoveLand(MovePhase + 1, IntroWay[MovePhase]);
                    else
                        me->GetMotionMaster()->MovePoint(MovePhase + 1, IntroWay[MovePhase]);
                }

                if (!UpdateVictim())
                    return;

                if (Flying)
                    return;

                // Phase 1 "GROUND FIGHT"
                if (Phase == 1)
                {
                    if (BellowingRoarTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_BELLOWING_ROAR);
                        BellowingRoarTimer = urand(30000, 40000);
                    } else BellowingRoarTimer -= diff;

                    if (SmolderingBreathTimer <= diff)
                    {
                        DoCast(me->GetVictim(), SPELL_SMOLDERING_BREATH);
                        SmolderingBreathTimer = 20000;
                    } else SmolderingBreathTimer -= diff;

                    if (CharredEarthTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            DoCast(target, SPELL_CHARRED_EARTH);
                        CharredEarthTimer = 20000;
                    } else CharredEarthTimer -= diff;

                    if (TailSweepTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            if (!me->HasInArc(M_PI, target))
                                DoCast(target, SPELL_TAIL_SWEEP);
                        TailSweepTimer = 15000;
                    } else TailSweepTimer -= diff;

                    if (SearingCindersTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            DoCast(target, SPELL_SEARING_CINDERS);
                        SearingCindersTimer = 10000;
                    } else SearingCindersTimer -= diff;

                    uint32 Prozent = uint32(me->GetHealthPct());

                    if (Prozent < 75 && FlyCount == 0) // first take off 75%
                        TakeOff();

                    if (Prozent < 50 && FlyCount == 1) // secound take off 50%
                        TakeOff();

                    if (Prozent < 25 && FlyCount == 2) // third take off 25%
                        TakeOff();

                    DoMeleeAttackIfReady();
                }

                // Phase 2 "FLYING FIGHT"
                if (Phase == 2)
                {
                    if (!RainBones)
                    {
                        if (!Skeletons)
                        {
                            for (uint8 i = 0; i <= 3; ++i)
                            {
                                DoCast(me->GetVictim(), SPELL_SUMMON_SKELETON);
                                Skeletons = true;
                            }
                        }

                        if (RainofBonesTimer < diff && !RainBones) // only once at the beginning of phase 2
                        {
                            DoCast(me->GetVictim(), SPELL_RAIN_OF_BONES);
                            RainBones = true;
                            SmokingBlastTimer = 20000;
                        } else RainofBonesTimer -= diff;

                        if (DistractingAshTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                                DoCast(target, SPELL_DISTRACTING_ASH);
                            DistractingAshTimer = 2000;  // TODO: find correct timer
                        } else DistractingAshTimer -= diff;
                    }

                    if (RainBones)
                    {
                        if (SmokingBlastTimer <= diff)
                         {
                            DoCast(me->GetVictim(), SPELL_SMOKING_BLAST);
                            SmokingBlastTimer = 1500; // TODO: find correct timer
                         } else SmokingBlastTimer -= diff;
                    }

                    if (FireballBarrageTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0))
                            DoCast(target, SPELL_FIREBALL_BARRAGE);
                        FireballBarrageTimer = 20000;
                    } else FireballBarrageTimer -= diff;

                    if (FlyTimer <= diff) //landing
                    {
                        Talk(YELL_LAND_PHASE); // check random 2txt
                        Movement = true;
                        MovePhase = 3;
                        Flying = true;
                    } else FlyTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_nightbaneAI(creature);
        }
};

class go_blackened_urn : public GameObjectScript
{
    public:
        go_blackened_urn() : GameObjectScript("go_blackened_urn") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            InstanceScript* instance = go->GetInstanceScript();
            if (!instance)
                return false;

            if (instance->GetData(TYPE_NIGHTBANE) != NOT_STARTED)
                return false;

            // TODO: check if can summon without quest on 335
            if (player->GetQuestStatus(QUEST_NIGHTBANE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_NIGHTBANE) == QUEST_STATUS_REWARDED)
            {
                go->SummonCreature(17225, -11003.7f, -1760.19f, 140.25f, 0.29f);
                instance->SetData(TYPE_NIGHTBANE, SPECIAL);
            }
            return false;
        }
};

void AddSC_boss_nightbane()
{
    new boss_nightbane();
    new go_blackened_urn();
}
