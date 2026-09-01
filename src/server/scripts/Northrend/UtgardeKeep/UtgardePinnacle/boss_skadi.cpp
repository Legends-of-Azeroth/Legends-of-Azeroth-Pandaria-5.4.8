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
#include "SpellAuras.h"
#include "SpellScript.h"
#include "utgarde_pinnacle.h"
#include "GridNotifiers.h"
#include "Player.h"
#include "MoveSplineInit.h"

enum Spells
{
    // Skadi Spells
    SPELL_CRUSH                                   = 50234,
    SPELL_POISONED_SPEAR                          = 50255,
    SPELL_WHIRLWIND                               = 50228,
    SPELL_SKADI_TELEPORT                          = 61790,
    SPELL_LAUNCH_HARPOON                          = 48642,
    SPELL_SUMMON_GAUNLET_MOBS_PERIODIC            = 59275,
    SPELL_FREEZING_CLOUD                          = 47579,
    SPELL_RIDE_GRAUF                              = 61791,
    SPELL_FREEZING_CLOUD_RIGHT_PERIODIC           = 47592,
    SPELL_FREEZING_CLOUD_LEFT_PERIODIC            = 47590,
    SPELL_FREEZING_CLOUD_RIGHT                    = 47593,
    SPELL_FREEZING_CLOUD_LEFT                     = 47563,
    SPELL_FREEZING_CLOUD_RIGHT_AREA               = 47594,
    SPELL_FREEZING_CLOUD_LEFT_AREA                = 47574,
    SPELL_SUMMON_YMIRJAR_WARRIOR_W                = 48631,
    SPELL_SUMMON_YMIRJAR_WARRIOR_E                = 48632,
    SPELL_SUMMON_YMIRJAR_HARPOONER_W              = 48633,
    SPELL_SUMMON_YMIRJAR_HARPOONER_E              = 48634,
    SPELL_SUMMON_YMIRJAR_WITCH_DOCTOR_W           = 48635,
    SPELL_SUMMON_YMIRJAR_WITCH_DOCTOR_E           = 48636,
    SPELL_UTGARDE_PINNACLE_GAUNTLET_EFFECT        = 47547,
    SPELL_UTGARDE_PINNACLE_GUANTLET_RESET_CHECK   = 49308,
    // Trash
    SPELL_HAMSTRING                               = 48639,
    SPELL_STRIKE                                  = 48640,
    SPELL_SHADOW_BOLT                             = 49084,
    SPELL_SHRINK                                  = 49089,
    SPELL_NET                                     = 49092,
    SPELL_THROW                                   = 49091,
    SPELL_SUMMON_HARPOON                          = 56789,
};

enum Yells
{
    SAY_AGGRO                           = 0,
    SAY_KILL                            = 1,
    SAY_DEATH                           = 2,
    SAY_DRAKE_HIT_1                     = 3,
    SAY_DRAKE_HIT_2                     = 4,
    SAY_DRAKE_DEATH                     = 5,
    //SAY_DRAKE_BREATH                    = 6 // Unused on retail, texts missing from broadcast_text
};

enum Emotes
{
    EMOTE_BREATH                        = 0,
    EMOTE_ON_RANGE
};

enum Data
{
    DATA_LOVE_TO_SKADI                   = 0,
    FIRST_WAVE_MAX_WARRIORS              = 10,
    FIRST_WAVE_SIZE                      = 13,
    GRAUF_PATH_INITIAL                   = 2689300,
    GRAUF_PATH_RIGHT                     = 2689301,
    GRAUF_PATH_LEFT                      = 2689302,
};

enum Points
{
    POINT_0                              = 0,
    POINT_1                              = 1,
    POINT_9                              = 9,
    POINT_11                             = 11,
    POINT_LEFT                           = 21,
    POINT_RIGHT                          = 22
};

enum Actions
{
    ACTION_START_ENCOUNTER               = 0,
    ACTION_FLAME,
    ACTION_GAUNTLET_END,
    ACTION_HARPOON_HIT,
};

enum CombatPhase
{
    PHASE_FLYING                         = 0,
    PHASE_GROUND,
    PHASE_NONE,
};

enum
{
    WORLD_STATE_LOVES_TO_SKADI_ALL_THE_TIME = 6429,
    ACHIEV_LODI_DODI_WE_LOVES_THE_SKADI     = 17726,
};

Position const BreachPoint = { 0.0f, 0.0f, 0.0f, 2.670354f };
Position const SecondaryWavesInitialPoint = { 478.7434f, -505.5758f, 104.7237f };
Position const SecondaryWavesFinalPoint = { 318.177f, -503.8898f, 104.5326f };
Position const SpawnLoc = { 477.5808f, -484.5591f, 104.8221f, 4.677482f };
Position const GraufLoc = { 341.7411f, -516.9545f, 104.6695f, 3.124139f };
Position const BreathPointRight = { 496.434f,  -517.578f,  120.0f, 3.124139f };
Position const BreathPointLeft = { 500.243f,  -501.693f,  120.0f, 3.228859f };
Position const FirstWaveLocations[FIRST_WAVE_SIZE] =
{
    { 458.5323f, -516.2537f, 104.617f,  0.0f      },
    { 429.4242f, -517.5624f, 104.8936f, 0.0f      },
    { 394.4955f, -514.6144f, 104.7235f, 0.0f      },
    { 362.2862f, -515.8771f, 104.7539f, 0.0f      },
    { 333.5374f, -514.7942f, 104.4779f, 0.0f      },
    { 457.6105f, -508.8362f, 104.4013f, 0.0f      },
    { 427.4026f, -510.7716f, 104.8802f, 0.0f      },
    { 392.5114f, -507.9429f, 104.7433f, 0.0f      },
    { 362.9511f, -508.4115f, 104.7218f, 0.0f      },
    { 333.536f,  -506.0805f, 104.4258f, 0.0f      },
    { 478.31f,   -511.049f,  104.7242f, 3.263766f },
    { 482.25f,   -514.1273f, 104.7234f, 3.263766f },
    { 481.3883f, -507.1089f, 104.7241f, 3.263766f },
};

class boss_skadi : public CreatureScript
{
    public:
        boss_skadi() : CreatureScript("boss_skadi") { }

        struct boss_skadiAI : public BossAI
        {
            boss_skadiAI(Creature* creature) : BossAI(creature, DATA_SKADI_THE_RUTHLESS)
            {
                Initialize();
            }

            void Initialize()
            {
                firstWaveSummoned = false;
                harpoonHit = 0;
                graufDamaged = false;
                _phase = PHASE_NONE;
                scheduler.SetValidator([this]
                {
                    return !me->HasUnitState(UNIT_STATE_CASTING);
                });
            }

            void Reset() override
            {
                _Reset();
                Initialize();
                me->SetReactState(REACT_PASSIVE);
                if (Creature* grauf = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GRAUF)))
                    grauf->DespawnOrUnsummon();
                me->SummonCreature(NPC_GRAUF, GraufLoc);

                instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_LODI_DODI_WE_LOVES_THE_SKADI);
                me->GetMap()->SetWorldState(WORLD_STATE_LOVES_TO_SKADI_ALL_THE_TIME, 1);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                summons.DespawnAll();
                _DespawnAtEvade();
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_YMIRJAR_WARRIOR:
                    case NPC_YMIRJAR_WITCH_DOCTOR:
                    case NPC_YMIRJAR_HARPOONER:
                        if (firstWaveSummoned)
                            summon->GetMotionMaster()->MovePoint(POINT_1, SecondaryWavesInitialPoint);
                        break;
                    default:
                        break;
                }
                summons.Summon(summon);
            }

            void SpawnFirstWave()
            {
                for (uint8 i = 0; i < FIRST_WAVE_MAX_WARRIORS; i++)
                    if (Creature* summon = me->SummonCreature(NPC_YMIRJAR_WARRIOR, SpawnLoc, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms))
                        summon->GetMotionMaster()->MovePoint(POINT_0, FirstWaveLocations[i]);

                if (Creature* crea = me->SummonCreature(NPC_YMIRJAR_WITCH_DOCTOR, SpawnLoc, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms))
                    crea->GetMotionMaster()->MovePoint(POINT_0, FirstWaveLocations[10]);
                if (Creature* crea = me->SummonCreature(NPC_YMIRJAR_HARPOONER, SpawnLoc, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms))
                    crea->GetMotionMaster()->MovePoint(POINT_0, FirstWaveLocations[11]);
                if (Creature* crea = me->SummonCreature(NPC_YMIRJAR_HARPOONER, SpawnLoc, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000ms))
                    crea->GetMotionMaster()->MovePoint(POINT_0, FirstWaveLocations[12]);

                firstWaveSummoned = true;
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START_ENCOUNTER:
                        instance->SetBossState(DATA_SKADI_THE_RUTHLESS, IN_PROGRESS);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->setActive(true);
                        SpawnFirstWave();
                        Talk(SAY_AGGRO);
                        _phase = PHASE_FLYING;
                        instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEV_LODI_DODI_WE_LOVES_THE_SKADI);

                        scheduler
                            .Schedule(Seconds(6), [this](TaskContext resetCheck)
                            {
                                if (Creature* resetTrigger = me->FindNearestCreature(NPC_TRIGGER_RESET, 200.0f))
                                    resetTrigger->CastSpell(resetTrigger, SPELL_UTGARDE_PINNACLE_GUANTLET_RESET_CHECK, true);
                                resetCheck.Repeat();
                            })
                            .Schedule(Seconds(2), [this](TaskContext /*context*/)
                            {
                                if (Creature* grauf = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GRAUF)))
                                    DoCast(grauf, SPELL_RIDE_GRAUF);
                            });

                        if (Creature* summonTrigger = me->SummonCreature(NPC_WORLD_TRIGGER, SpawnLoc))
                            summonTrigger->CastSpell(summonTrigger, SPELL_SUMMON_GAUNLET_MOBS_PERIODIC, true);

                        if (Creature* combatTrigger = me->SummonCreature(NPC_COMBAT_TRIGGER, SpawnLoc))
                            combatTrigger->AI()->DoZoneInCombat();
                        break;
                    case ACTION_FLAME:
                        if (graufDamaged)
                            me->GetMap()->SetWorldState(WORLD_STATE_LOVES_TO_SKADI_ALL_THE_TIME, 0);
                        //Talk(SAY_DRAKE_BREATH);
                        break;
                    case ACTION_GAUNTLET_END:
                        me->ExitVehicle();
                        Talk(SAY_DRAKE_DEATH);
                        DoCast(me, SPELL_SKADI_TELEPORT, true);
                        summons.DespawnEntry(NPC_WORLD_TRIGGER);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        _phase = PHASE_GROUND;

                        scheduler
                            .Schedule(Seconds(8), [this](TaskContext crush)
                            {
                                DoCastVictim(SPELL_CRUSH);
                                crush.Repeat();
                            })
                            .Schedule(Seconds(11), [this](TaskContext poisonedSpear)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                    DoCast(target, SPELL_POISONED_SPEAR);
                                poisonedSpear.Repeat();
                            })
                            .Schedule(Seconds(23), [this](TaskContext whirlwind)
                            {
                                DoCast(SPELL_WHIRLWIND);
                                whirlwind.Repeat();
                            });
                        break;
                    case ACTION_HARPOON_HIT:
                        harpoonHit++;
                        graufDamaged = true;
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (_phase == PHASE_GROUND)
                {
                    if (!UpdateVictim())
                        return;

                    DoMeleeAttackIfReady();
                }
            }

        private:
            CombatPhase _phase;
            uint8 harpoonHit;
            bool graufDamaged;
            bool firstWaveSummoned;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_skadiAI>(creature);
        }
};

class npc_grauf : public CreatureScript
{
    public:
        npc_grauf() : CreatureScript("npc_grauf") { }

        struct npc_graufAI : public ScriptedAI
        {
            npc_graufAI(Creature* creature) : ScriptedAI(creature)
            {
                _isFirstFly = true;
                _instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->setRegeneratingHealth(false);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* skadi = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SKADI_THE_RUTHLESS)))
                    skadi->AI()->DoAction(ACTION_GAUNTLET_END);

                me->DespawnOrUnsummon(6000);
            }

            void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
            {
                if (!apply)
                    return;

                Movement::MoveSplineInit init(who);
                init.DisableTransportPathTransformations();
                init.MoveTo(0.3320355f, 0.05355075f, 5.196949f, false);
                init.Launch();

                me->setActive(true);

                _scheduler.Schedule(Seconds(2) + Milliseconds(500), [this](TaskContext /*context*/)
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_AIR);
                });
                _scheduler.Schedule(Seconds(3) + Milliseconds(500), [this](TaskContext /*context*/)
                {
                    me->GetMotionMaster()->MovePath(GRAUF_PATH_INITIAL, false);
                });
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != WAYPOINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_9:
                        if (!_isFirstFly)
                            break;
                        _isFirstFly = false;
                        // no break
                    case POINT_11:
                        _scheduler
                            .Schedule(Milliseconds(1), [this](TaskContext /*context*/)
                            {
                                me->SetFacingTo(BreachPoint.GetOrientation());
                                Talk(EMOTE_ON_RANGE);
                            })
                            .Schedule(Seconds(10), [this](TaskContext /*context*/)
                            {
                                if (RAND(POINT_LEFT,POINT_RIGHT) == POINT_LEFT)
                                    me->GetMotionMaster()->MovePoint(POINT_LEFT, BreathPointLeft);
                                else
                                    me->GetMotionMaster()->MovePoint(POINT_RIGHT, BreathPointRight);
                            });
                        break;
                    case POINT_LEFT:
                        _scheduler
                            .Schedule(Milliseconds(1), [this](TaskContext /*context*/)
                            {
                                me->SetFacingTo(BreathPointLeft.GetOrientation());
                                Talk(EMOTE_BREATH);
                            })
                            .Schedule(Seconds(2), [this](TaskContext /*context*/)
                            {
                                me->GetMotionMaster()->MovePath(GRAUF_PATH_LEFT, false);
                                DoCast(SPELL_FREEZING_CLOUD_LEFT_PERIODIC);
                                if (Creature* skadi = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SKADI_THE_RUTHLESS)))
                                    skadi->AI()->DoAction(ACTION_FLAME);
                            })
                            .Schedule(Seconds(10), [this](TaskContext /*context*/)
                            {
                                me->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_LEFT_PERIODIC);
                            });
                        break;
                    case POINT_RIGHT:
                        _scheduler
                            .Schedule(Milliseconds(1), [this](TaskContext /*context*/)
                            {
                                me->SetFacingTo(BreathPointRight.GetOrientation());
                                Talk(EMOTE_BREATH);
                            })
                            .Schedule(Seconds(2), [this](TaskContext /*context*/)
                            {
                                me->GetMotionMaster()->MovePath(GRAUF_PATH_RIGHT, false);
                                DoCast(SPELL_FREEZING_CLOUD_RIGHT_PERIODIC);
                                if (Creature* skadi = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SKADI_THE_RUTHLESS)))
                                    skadi->AI()->DoAction(ACTION_FLAME);
                            })
                            .Schedule(Seconds(10), [this](TaskContext /*context*/)
                            {
                                me->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_RIGHT_PERIODIC);
                            });
                        break;
                default:
                    break;
                }
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_LAUNCH_HARPOON)
                    if (Creature* skadi = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SKADI_THE_RUTHLESS)))
                        skadi->AI()->DoAction(ACTION_HARPOON_HIT);
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FREEZING_CLOUD_LEFT:
                        me->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_LEFT_PERIODIC);
                        break;
                    case SPELL_FREEZING_CLOUD_RIGHT:
                        me->RemoveAurasDueToSpell(SPELL_FREEZING_CLOUD_RIGHT_PERIODIC);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                _scheduler.Update(diff);
            }

        private:
            TaskScheduler _scheduler;
            InstanceScript* _instance;
            bool _isFirstFly;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_graufAI>(creature);
        }
};

struct npc_skadi_trashAI : public ScriptedAI
{
    npc_skadi_trashAI(Creature* creature) : ScriptedAI(creature)
    {
        _instance = me->GetInstanceScript();

        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustEngagedWith(Unit* who) override
    {
        CreatureAI::JustEngagedWith(who);
        ScheduleTasks();
    }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        if (Creature* skadi = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_SKADI_THE_RUTHLESS)))
            skadi->AI()->JustSummoned(me);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case POINT_0:
                me->HandleEmoteStateCommand(me->GetEntry() == NPC_YMIRJAR_WARRIOR ? EMOTE_STATE_READY1H : EMOTE_STATE_READY2HL);
                break;
            case POINT_1:
                _scheduler.Schedule(Seconds(1), [this](TaskContext /*context*/)
                {
                    me->GetMotionMaster()->MovePoint(POINT_9, SecondaryWavesFinalPoint);
                });
                break;
            case POINT_9:
                DoZoneInCombat(me, 200);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        UpdateVictim();

        _scheduler.Update(diff, [this]
        {
            DoMeleeAttackIfReady();
        });
    }

    virtual void ScheduleTasks() = 0;

protected:
    InstanceScript* _instance;
    TaskScheduler _scheduler;
};

class npc_ymirjar_warrior : public CreatureScript
{
    public:
        npc_ymirjar_warrior() : CreatureScript("npc_ymirjar_warrior") { }

        struct npc_ymirjar_warriorAI : public npc_skadi_trashAI
        {
            npc_ymirjar_warriorAI(Creature* creature) : npc_skadi_trashAI(creature) { }

            void ScheduleTasks() override
            {
                _scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                    {
                        DoCastVictim(SPELL_HAMSTRING);
                        context.Repeat(Seconds(11), Seconds(18));
                    })
                    .Schedule(Seconds(9), [this](TaskContext context)
                    {
                        DoCastVictim(SPELL_STRIKE);
                        context.Repeat(Seconds(10), Seconds(13));
                    });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ymirjar_warriorAI>(creature);
        }
};

class npc_ymirjar_witch_doctor : public CreatureScript
{
    public:
        npc_ymirjar_witch_doctor() : CreatureScript("npc_ymirjar_witch_doctor") { }

        struct npc_ymirjar_witch_doctorAI : public npc_skadi_trashAI
        {
            npc_ymirjar_witch_doctorAI(Creature* creature) : npc_skadi_trashAI(creature) { }

            void ScheduleTasks() override
            {
                _scheduler
                    .Schedule(Seconds(2), [this](TaskContext shadowBolt)
                    {
                        DoCastVictim(SPELL_SHADOW_BOLT);
                        shadowBolt.Repeat();
                    })
                    .Schedule(Seconds(20), Seconds(34), [this](TaskContext shrink)
                    {
                        DoCastVictim(SPELL_SHRINK);
                        shrink.Repeat();
                    });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ymirjar_witch_doctorAI>(creature);
        }
};

class npc_ymirjar_harpooner : public CreatureScript
{
    public:
        npc_ymirjar_harpooner() : CreatureScript("npc_ymirjar_harpooner") { }

        struct npc_ymirjar_harpoonerAI : public npc_skadi_trashAI
        {
            npc_ymirjar_harpoonerAI(Creature* creature) : npc_skadi_trashAI(creature) { }

            void ScheduleTasks() override
            {
                _scheduler
                    .Schedule(Seconds(13), [this](TaskContext net)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 30, true))
                            DoCast(target, SPELL_NET);
                        net.Repeat();
                    })
                    .Schedule(Seconds(2), [this](TaskContext castThrow)
                    {
                        DoCastVictim(SPELL_THROW);
                        castThrow.Repeat();
                    });
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(SPELL_SUMMON_HARPOON);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ymirjar_harpoonerAI>(creature);
        }
};

class spell_freezing_cloud_left : public SpellScriptLoader
{
    public:
        spell_freezing_cloud_left(char const* name, Targets target) : SpellScriptLoader(name), _target(target) { }

        class spell_freezing_cloud_left_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_freezing_cloud_left_SpellScript);

        public:
            spell_freezing_cloud_left_SpellScript(Targets target) : _target(target) { }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([](WorldObject* obj) { return obj->GetPositionY() < -511.0f; });
                if (_target == TARGET_UNIT_CONE_ENTRY)
                {
                    // Pick the closest target for initial spell
                    targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));

                    if (targets.empty())
                        return;

                    targets.resize(1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_freezing_cloud_left_SpellScript::FilterTargets, EFFECT_0, _target);
            }

        private:
            Targets _target;
        };

        class spell_freezing_cloud_left_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_freezing_cloud_left_AuraScript);

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_FREEZING_CLOUD, true);
                GetTarget()->CastSpell(GetTarget(), SPELL_FREEZING_CLOUD_LEFT_AREA, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_freezing_cloud_left_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_freezing_cloud_left_SpellScript(_target);
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_freezing_cloud_left_AuraScript();
        }

    private:
        Targets _target;
};

class spell_freezing_cloud_right : public SpellScriptLoader
{
    public:
        spell_freezing_cloud_right(char const* name, Targets target) : SpellScriptLoader(name), _target(target) { }

        class spell_freezing_cloud_right_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_freezing_cloud_right_SpellScript);

        public:
            spell_freezing_cloud_right_SpellScript(Targets target) : _target(target) { }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if([](WorldObject* obj) { return obj->GetPositionY() > -511.0f; });
                if (_target == TARGET_UNIT_CONE_ENTRY)
                {
                    // Pick the closest target for initial spell
                    targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
                    targets.resize(1);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_freezing_cloud_right_SpellScript::FilterTargets, EFFECT_0, _target);
            }

        private:
            Targets _target;
        };

        class spell_freezing_cloud_right_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_freezing_cloud_right_AuraScript);

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_FREEZING_CLOUD, true);
                GetTarget()->CastSpell(GetTarget(), SPELL_FREEZING_CLOUD_RIGHT_AREA, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_freezing_cloud_right_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_freezing_cloud_right_SpellScript(_target);
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_freezing_cloud_right_AuraScript();
        }

    private:
        Targets _target;
};

class spell_skadi_reset_check : public SpellScriptLoader
{
    public:
        spell_skadi_reset_check() : SpellScriptLoader("spell_skadi_reset_check") { }

        class spell_skadi_reset_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skadi_reset_check_SpellScript);

            void CountTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(false, SPELL_UTGARDE_PINNACLE_GAUNTLET_EFFECT));
                _targetCount = targets.size();
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (_targetCount)
                    return;

                Creature* target = GetHitCreature();
                if (!target)
                    return;

                if (InstanceScript* instance = target->GetInstanceScript())
                    if (instance->GetBossState(DATA_SKADI_THE_RUTHLESS) == IN_PROGRESS)
                        target->AI()->EnterEvadeMode();
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_skadi_reset_check_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_skadi_reset_check_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

        private:
            uint32 _targetCount = 0;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skadi_reset_check_SpellScript();
        }
};

class spell_skadi_launch_harpoon : public SpellScriptLoader
{
    public:
        spell_skadi_launch_harpoon() : SpellScriptLoader("spell_skadi_launch_harpoon") { }

        class spell_skadi_launch_harpoon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skadi_launch_harpoon_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (targets.size() >= 2)
                    targets.remove_if([](WorldObject* obj) { return obj->GetEntry() != NPC_GRAUF; });
            }

            void HandleDamageCalc()
            {
                if (Unit* target = GetHitUnit())
                {
                    if (target->GetEntry() != NPC_GRAUF)
                    {
                        PreventDefaultEffect(EFFECT_0);
                        return;
                    }

                    SetHitDamage(target->CountPctFromMaxHealth(35));
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_skadi_launch_harpoon_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CONE_ENTRY);
                OnHit += SpellHitFn(spell_skadi_launch_harpoon_SpellScript::HandleDamageCalc);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skadi_launch_harpoon_SpellScript();
        }
};

class spell_skadi_poisoned_spear : public SpellScriptLoader
{
    public:
        spell_skadi_poisoned_spear() : SpellScriptLoader("spell_skadi_poisoned_spear") { }

        class spell_skadi_poisoned_spear_left_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_skadi_poisoned_spear_left_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->CastSpell(GetHitUnit(), GetEffectValue(), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_skadi_poisoned_spear_left_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_skadi_poisoned_spear_left_SpellScript();
        }
};

class spell_summon_gauntlet_mobs_periodic : public SpellScriptLoader
{
    public:
        spell_summon_gauntlet_mobs_periodic() : SpellScriptLoader("spell_summon_gauntlet_mobs_periodic") { }

        class spell_summon_gauntlet_mobs_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_summon_gauntlet_mobs_periodic_AuraScript);

            void CastTheNextTwoSpells()
            {
                for (uint8 i = 0; i < 2; ++i)
                {
                    uint32 spellId = SummonSpellsList.front();
                    GetTarget()->CastSpell((Unit*)nullptr, spellId, true);
                    SummonSpellsList.push_back(spellId);
                    SummonSpellsList.pop_front();
                }
            }
            void PushBackTheNextTwoSpells()
            {
                for (uint8 j = 0; j < 2; ++j)
                {
                    SummonSpellsList.push_back(SummonSpellsList.front());
                    SummonSpellsList.pop_front();
                }
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (RAND(0, 1))
                {
                    CastTheNextTwoSpells();
                    PushBackTheNextTwoSpells();
                }
                else
                {
                    PushBackTheNextTwoSpells();
                    CastTheNextTwoSpells();
                }
            }
        private:
            std::deque<uint32> SummonSpellsList =
            { {
                SPELL_SUMMON_YMIRJAR_WARRIOR_E,
                SPELL_SUMMON_YMIRJAR_HARPOONER_W,
                SPELL_SUMMON_YMIRJAR_WARRIOR_W,
                SPELL_SUMMON_YMIRJAR_HARPOONER_E,
                SPELL_SUMMON_YMIRJAR_WARRIOR_W,
                SPELL_SUMMON_YMIRJAR_WITCH_DOCTOR_E,
                SPELL_SUMMON_YMIRJAR_WARRIOR_E,
                SPELL_SUMMON_YMIRJAR_WITCH_DOCTOR_W
            } };

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_summon_gauntlet_mobs_periodic_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_summon_gauntlet_mobs_periodic_AuraScript();
        }
};

class spell_summon_harpoon : public SpellScriptLoader
{
    public:
        spell_summon_harpoon() : SpellScriptLoader("spell_summon_harpoon") { }

        class spell_summon_harpoon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_harpoon_SpellScript);

            void SelectDest(SpellDestination& pos)
            {
                _position = pos;
            }

            void CopyDest(SpellDestination& pos)
            {
                pos = _position;
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_summon_harpoon_SpellScript::SelectDest, EFFECT_0, TARGET_DEST_CASTER_RADIUS);
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_summon_harpoon_SpellScript::CopyDest, EFFECT_1, TARGET_DEST_CASTER_RADIUS);
            }

        private:
            SpellDestination _position;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_harpoon_SpellScript();
        }
};

class at_skadi_gaunlet : public AreaTriggerScript
{
    public:
        at_skadi_gaunlet() : AreaTriggerScript("at_skadi_gaunlet") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_SKADI_THE_RUTHLESS) == NOT_STARTED)
                    if (Creature* skadi = ObjectAccessor::GetCreature(*player, instance->GetGuidData(DATA_SKADI_THE_RUTHLESS)))
                    {
                        skadi->AI()->DoAction(ACTION_START_ENCOUNTER);
                        return true;
                    }
            }

            return true;
        }
};

void AddSC_boss_skadi()
{
    new boss_skadi();
    new npc_grauf();
    new npc_ymirjar_warrior();
    new npc_ymirjar_witch_doctor();
    new npc_ymirjar_harpooner();
    new spell_freezing_cloud_left("spell_freezing_cloud_left", TARGET_UNIT_CONE_ENTRY);
    new spell_freezing_cloud_left("spell_freezing_cloud_area_left", TARGET_UNIT_SRC_AREA_ENTRY);
    new spell_freezing_cloud_right("spell_freezing_cloud_right", TARGET_UNIT_CONE_ENTRY);
    new spell_freezing_cloud_right("spell_freezing_cloud_area_right", TARGET_UNIT_SRC_AREA_ENTRY);
    new spell_skadi_reset_check();
    new spell_skadi_launch_harpoon();
    new spell_skadi_poisoned_spear();
    new spell_summon_gauntlet_mobs_periodic();
    new spell_summon_harpoon();
    new at_skadi_gaunlet();
}
