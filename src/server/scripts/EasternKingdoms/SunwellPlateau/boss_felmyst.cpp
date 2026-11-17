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
SDName: Boss_Felmyst
SD%Complete: 0
SDComment:
EndScriptData */

#include "ScriptPCH.h"
#include "sunwell_plateau.h"

enum Texts
{
    ANNOUNCE_FOG    = 0,
    SAY_BIRTH       = 1,
    SAY_BREATH      = 2,
    SAY_TAKEOFF     = 3,
    SAY_KILL        = 4,
    SAY_BERSERK     = 5,
    SAY_DEATH       = 6,
};

enum Yells
{
    YELL_BIRTH                                    = 0,
    YELL_KILL                                     = 1,
    YELL_BREATH                                   = 2,
    YELL_TAKEOFF                                  = 3,
    YELL_BERSERK                                  = 4,
    YELL_DEATH                                    = 5,
  //YELL_KALECGOS                                 = 6, Not used. After felmyst's death spawned and say this
};

enum Spells
{
    //Aura
    AURA_SUNWELL_RADIANCE                         = 45769,
    AURA_NOXIOUS_FUMES                            = 47002,

    //Land phase
    SPELL_CLEAVE                                  = 19983,
    SPELL_CORROSION                               = 45866,
    SPELL_GAS_NOVA                                = 45855,
    SPELL_ENCAPSULATE_CHANNEL                     = 45661,
    // SPELL_ENCAPSULATE_EFFECT                      = 45665,
    // SPELL_ENCAPSULATE_AOE                         = 45662,

    //Flight phase
    SPELL_SUMMON_DEMONIC_VAPOR                    = 45391,   // fel to player, force cast 45392, 50000y selete target
    SPELL_SUMMON_DEMONIC_VAPOR_SUMMON             = 45392,   // player summon vapor, radius around caster, 5y,
    SPELL_DEMONIC_VAPOR_SPAWN_TRIGGER             = 45388,   // vapor to fel, force cast 45389
    SPELL_DEMONIC_VAPOR_BEAM_VISUAL               = 45389,   // fel to vapor, green beam channel
    SPELL_DEMONIC_VAPOR_PERIODIC                  = 45411,   // linked to 45389, vapor to self, trigger 45410 and 46931
    SPELL_DEMONIC_VAPOR_DAMAGE                    = 46931,   // vapor damage, 4000
    SPELL_SUMMON_DEMONIC_VAPOR_TRAIL              = 45410,   // vapor summon trail
    SPELL_DEMONIC_VAPOR_TRAIL_PERIODIC            = 45399,   // trail to self, trigger 45402
    SPELL_DEMONIC_VAPOR_TRAIL_DAMAGE              = 45402,   // trail damage, 2000 + 2000 dot
    SPELL_SUMMON_BLAZING_DEAD                     = 45400,   // summon blazing dead, 5min
    SPELL_BLAZING_DEAD_PASSIVE                    = 45415,
    SPELL_SPEED_BURST                             = 45495,   // fel to self, speed burst
    SPELL_FOG_OF_CORRUPTION                       = 45582,   // fog to self, trigger 45782
    SPELL_FOG_OF_CORRUPTION_FORCE_CAST            = 45782,   // fog to player, force cast 45714
    SPELL_FOG_OF_CORRUPTION_SCRIPT                = 45714,   // player let fel cast 45717, script effect
    SPELL_FOG_OF_CORRUPTION_CHARM                 = 45717,   // fel to player
    SPELL_FOG_OF_CORRUPTION_CHARM_2               = 45726,   // link to 45717
    SPELL_FOG_OF_CORRUPTION_CHARM_3               = 46411,   // link to 45717
    SPELL_SOUL_SEVER                              = 45917,
    SPELL_SOUL_SEVER_TARGETING                    = 45918,

    SPELL_TRANSFORM_TRIGGER                       = 44885,   // madrigosa to self, trigger 46350
    SPELL_TRANSFORM_VISUAL                        = 46350,   // 46411stun?
    SPELL_TRANSFORM_FELMYST                       = 45068,   // become fel
    SPELL_FELMYST_SUMMON                          = 45069,
    SPELL_TRIGGER_000_IF_SUMMONED_ONLY            = 45211,

    //Other
    SPELL_BERSERK                                 = 45078,
    SPELL_CLOUD_VISUAL                            = 45212,
    SPELL_CLOUD_SUMMON                            = 45884,
};

enum PhaseFelmyst
{
    PHASE_NONE,
    PHASE_GROUND,
    PHASE_FLIGHT,
};

enum EventFelmyst
{
    EVENT_NONE,
    EVENT_BERSERK,

    EVENT_CLEAVE,
    EVENT_CORROSION,
    EVENT_GAS_NOVA,
    EVENT_ENCAPSULATE,
    EVENT_FLIGHT,

    EVENT_FLIGHT_SEQUENCE,
    EVENT_SUMMON_DEAD,
    EVENT_SUMMON_FOG,
};

enum Points
{
    POINT_FLIGHT = 1,
    POINT_START,
    POINT_HOME,
};

class boss_felmyst : public CreatureScript
{
    public:
        boss_felmyst() : CreatureScript("boss_felmyst") { }

        struct boss_felmystAI : public ScriptedAI
        {
            boss_felmystAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            PhaseFelmyst phase;
            EventMap events;
            uint64 lastEncapsulateTarget = 0;
            uint32 flightCount;
            uint32 breathCount;
            uint32 outOfBoundsTime = 0;
            uint32 lastEncapsulateTimeout = 0;

            void Reset() override
            {
                phase = PHASE_NONE;

                events.Reset();

                flightCount = 0;

                me->SetFlying(true);
                me->OverrideInhabitType(INHABIT_AIR);

                me->setActive(false);

                std::list<Creature*> summons;
                GetCreatureListWithEntryInGrid(summons, me, NPC_VAPOR_TRAIL, 300.0f);
                for (auto&& summon : summons)
                    summon->DespawnOrUnsummon();

                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);

                DoCast(me, SPELL_TRANSFORM_FELMYST, true);
                DoCastAOE(SPELL_SOUL_SEVER_TARGETING, true);

                if (instance)
                    instance->SetData(DATA_FELMYST_EVENT, NOT_STARTED);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, SPELL_TRIGGER_000_IF_SUMMONED_ONLY, true);
                me->SetVisible(false);
                me->SetFlying(false);
                me->ResetInhabitTypeOverride();
                me->SetStandState(UNIT_STAND_STATE_SLEEP);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetHomePosition(1489.438f, 651.615f, 44.65125f, 3.0f);
                uint32 delay = 0;
                me->m_Events.Schedule(delay += 9000, [this] { me->SetVisible(true); });
                me->m_Events.Schedule(delay += 3000, [this]
                {
                    if (Creature* cloud = me->FindNearestCreature(NPC_BRUTALLUS_DEATH_CLOUD, 100.0f))
                        cloud->DespawnOrUnsummon();
                });
                me->m_Events.Schedule(delay += 1000, [this] { me->SetStandState(UNIT_STAND_STATE_STAND); });
                me->m_Events.Schedule(delay += 2500, [this]
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                    Talk(SAY_BIRTH);
                });
                me->m_Events.Schedule(delay += 1100, [this]
                {
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_AIR);
                });
                me->m_Events.Schedule(delay += 7250, [this]
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->GetMotionMaster()->MoveTakeoff(POINT_HOME, me->GetHomePosition());
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BERSERK, 600000);

                me->setActive(true);
                DoZoneInCombat();
                DoCast(me, AURA_SUNWELL_RADIANCE, true);
                DoCast(me, AURA_NOXIOUS_FUMES, true);
                EnterPhase(PHASE_GROUND);
                if (me->GetVictim())
                {
                    Position pos;
                    me->SetFlying(true);
                    me->OverrideInhabitType(INHABIT_AIR);
                    me->GetVictim()->GetRandomContactPoint(me, pos.m_positionX, pos.m_positionY, pos.m_positionZ, CONTACT_DISTANCE, CONTACT_DISTANCE);
                    me->GetMotionMaster()->MovePoint(POINT_START, pos, false);
                }
                outOfBoundsTime = 0;

                if (instance)
                    instance->SetData(DATA_FELMYST_EVENT, IN_PROGRESS);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetFullHealth();
                me->OverrideInhabitType(INHABIT_AIR);
                ScriptedAI::EnterEvadeMode();
                me->m_Events.Schedule(50000, [this]
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                    me->SetVisible(true);
                });
            }

            void AttackStart(Unit* who) override
            {
                if (phase == PHASE_NONE && who)
                {
                    // Just attack and enter combat on initial pull, don't chase
                    me->Attack(who, true);
                    return;
                }
                if (phase != PHASE_FLIGHT)
                    ScriptedAI::AttackStart(who);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (phase != PHASE_FLIGHT)
                    ScriptedAI::MoveInLineOfSight(who);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_FELMYST_EVENT, DONE);
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                if (me->IsInCombat() && spell->Id == SPELL_FOG_OF_CORRUPTION_SCRIPT)
                    DoCast(caster, SPELL_FOG_OF_CORRUPTION_CHARM, true);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_SOUL_SEVER_TARGETING)
                    target->CastSpell(target, SPELL_SOUL_SEVER, true);
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (phase == PHASE_FLIGHT && (type == POINT_MOTION_TYPE || type == EFFECT_MOTION_TYPE) && point == POINT_FLIGHT)
                    events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 1);
                if (type == POINT_MOTION_TYPE && point == POINT_START)
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                    me->SetFlying(false);
                    me->ResetInhabitTypeOverride();
                    if (me->GetVictim())
                        me->m_Events.Schedule(1000, [this]() { me->GetMotionMaster()->MoveChase(me->GetVictim()); });
                    else
                        EnterEvadeMode();
                }
                if (type == EFFECT_MOTION_TYPE && point == POINT_HOME)
                    me->m_Events.Schedule(1000, [this]() { me->GetMotionMaster()->MovePath(me->GetEntry() * 100, true); });
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (phase != PHASE_GROUND && damage >= me->GetHealth())
                    damage = 0;
            }

            void EnterPhase(PhaseFelmyst nextPhase)
            {
                switch (nextPhase)
                {
                    case PHASE_GROUND:
                        me->RemoveAurasDueToSpell(SPELL_SPEED_BURST);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetFlying(false);
                        me->ResetInhabitTypeOverride();
                        events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 10000));
                        events.ScheduleEvent(EVENT_CORROSION, urand(10000, 20000));
                        events.ScheduleEvent(EVENT_GAS_NOVA, urand(15000, 20000));
                        events.ScheduleEvent(EVENT_ENCAPSULATE, urand(20000, 25000));
                        events.ScheduleEvent(EVENT_FLIGHT, 60000);
                        break;
                    case PHASE_FLIGHT:
                        me->SetReactState(REACT_PASSIVE);
                        me->InterruptNonMeleeSpells(false);
                        me->AttackStop();
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                        me->SetFlying(true);
                        me->OverrideInhabitType(INHABIT_AIR);
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 1000);
                        flightCount = 0;
                        breathCount = 0;
                        break;
                    default:
                        break;
                }
                phase = nextPhase;
            }

            void HandleFlightSequence()
            {
                switch (flightCount)
                {
                    case 0:
                    {
                        Position pos;
                        pos.Relocate(me);
                        pos.m_positionZ += 10.0f;
                        me->SetFlying(true);
                        me->OverrideInhabitType(INHABIT_AIR);
                        me->GetMotionMaster()->MoveTakeoff(POINT_FLIGHT, pos);
                        Talk(SAY_TAKEOFF);
                        //events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 2000);
                        break;
                    }
                    case 1:
                        me->GetMotionMaster()->MovePoint(POINT_FLIGHT, me->GetPositionX() + 1, me->GetPositionY(), me->GetPositionZ() + 10, false);
                        break;
                    case 2:
                        me->InterruptNonMeleeSpells(false);
                        DoCastAOE(SPELL_SUMMON_DEMONIC_VAPOR, true);
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 10000);
                        break;
                    case 3:
                        me->InterruptNonMeleeSpells(false);
                        DoCastAOE(SPELL_SUMMON_DEMONIC_VAPOR, true);
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 10000);
                        break;
                    case 4:
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 1);
                        break;
                    case 5: // Ready to start moving to Flight Trigger
                        if (Creature* trigger = GetClosestFlightTarget())
                            me->GetMotionMaster()->MovePoint(POINT_FLIGHT, *trigger, false);
                        break;
                    case 6: // Reached Flight Trigger
                        me->SetFacingTo(me->GetPositionY() < 611.0f ? M_PI / 2 : -M_PI / 2);
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 7000);
                        break;
                    case 7: // Turned around and ready to move to a random lane
                        me->GetMotionMaster()->MovePoint(POINT_FLIGHT, RAND(1452.0f, 1472.58f, 1495.0f), me->GetPositionY() < 611.0f ? 520.0f : 700.0f, 50.0f, false);
                        break;
                    case 8: // Reached start of lane
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 1000);
                        break;
                    case 9: // Just a bit of a delay
                        me->SetFacingTo(me->GetPositionY() < 611.0f ? M_PI / 2 : -M_PI / 2);
                        Talk(ANNOUNCE_FOG);
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 4000);
                        break;
                    case 10: // Turned towards the lane
                        DoCast(me, SPELL_SPEED_BURST, true);
                        me->GetMotionMaster()->MovePoint(POINT_FLIGHT, me->GetPositionX(), me->GetPositionY() < 611.0f ? 700.0f : 520.0f, 50.0f, false);
                        events.ScheduleEvent(EVENT_SUMMON_FOG, 500);
                        break;
                    case 11: // Reached the end of lane
                        me->RemoveAurasDueToSpell(SPELL_SPEED_BURST);
                        events.CancelEvent(EVENT_SUMMON_FOG);
                        ++breathCount;
                        events.ScheduleEvent(EVENT_FLIGHT_SEQUENCE, 1);
                        if (breathCount < 3)
                            flightCount = 4;
                        break;
                    case 12: // Ready to start flying back to combat area
                        me->GetMotionMaster()->MovePoint(POINT_FLIGHT, 1480.0f, me->GetPositionY() < 611.0f ? 560.0f : 650.0f, 40.0f, false);
                        break;
                    case 13: // Reached combat area
                    {
                        Position pos;
                        pos.Relocate(me);
                        me->UpdateGroundPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                        me->GetMotionMaster()->MoveLand(POINT_FLIGHT, pos);
                        break;
                    }
                    case 14: // Landed
                        EnterPhase(PHASE_GROUND);
                        if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            AttackStart(target);
                        else
                            EnterEvadeMode();
                        break;
                }
                ++flightCount;
            }

            bool CanAIAttack(Unit const* who) const override
            {
                return who->GetPositionZ() <= 42.0f || who->HasAura(SPELL_ENCAPSULATE_CHANNEL) || who->GetGUID() == lastEncapsulateTarget && getMSTime() < lastEncapsulateTimeout;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetPositionX() >= 1541.850952f && me->GetPositionY() >= 597.264587f ||
                    me->GetPositionX() >= 1519.0f && me->GetPositionY() <= 516.0f)
                {
                    outOfBoundsTime += diff;
                    if (outOfBoundsTime >= 5000)
                    {
                        EnterEvadeMode();
                        return;
                    }
                }
                else
                    outOfBoundsTime = 0;

                events.Update(diff);

                if (me->IsNonMeleeSpellCasted(false))
                    return;

                if (phase == PHASE_GROUND)
                {
                    switch (events.ExecuteEvent())
                    {
                        case EVENT_BERSERK:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK, true);
                            events.ScheduleEvent(EVENT_BERSERK, 10000);
                            break;
                        case EVENT_CLEAVE:
                            DoCast(me->GetVictim(), SPELL_CLEAVE, false);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(5000,10000));
                            break;
                        case EVENT_CORROSION:
                            DoCast(me->GetVictim(), SPELL_CORROSION, false);
                            events.ScheduleEvent(EVENT_CORROSION, urand(20000,30000));
                            break;
                        case EVENT_GAS_NOVA:
                            DoCast(me, SPELL_GAS_NOVA, false);
                            events.ScheduleEvent(EVENT_GAS_NOVA, urand(20000,25000));
                            break;
                        case EVENT_ENCAPSULATE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150, true))
                            {
                                lastEncapsulateTarget = target->GetGUID();
                                lastEncapsulateTimeout = getMSTime() + 15000;
                                DoCast(target, SPELL_ENCAPSULATE_CHANNEL, false);
                            }
                            events.ScheduleEvent(EVENT_ENCAPSULATE, urand(25000,30000));
                            break;
                        case EVENT_FLIGHT:
                            EnterPhase(PHASE_FLIGHT);
                            break;
                        default:
                            DoMeleeAttackIfReady();
                            break;
                    }
                }

                if (phase == PHASE_FLIGHT)
                {
                    switch (events.ExecuteEvent())
                    {
                        case EVENT_BERSERK:
                            Talk(SAY_BERSERK);
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_FLIGHT_SEQUENCE:
                            HandleFlightSequence();
                            break;
                        case EVENT_SUMMON_FOG:
                        {
                            std::list<Creature*> triggers;
                            GetCreatureListWithEntryInGrid(triggers, me, NPC_WORLD_TRIGGER_LARGE_AOI_NOT_IMMUNE_PC_NPC, 300.0f);
                            triggers.remove_if([this](Creature* trigger)
                            {
                                if (me->isInFront(trigger))
                                    return true;
                                if (me->GetPositionX() < 1460)
                                    return trigger->GetPositionX() > 1460;
                                if (me->GetPositionX() < 1483)
                                    return trigger->GetPositionX() < 1460 || trigger->GetPositionX() > 1483;
                                return trigger->GetPositionX() < 1483;
                            });
                            triggers.remove_if(Trinity::UnitAuraCheck(true, SPELL_FOG_OF_CORRUPTION));
                            for (auto&& trigger : triggers)
                                trigger->CastSpell(trigger, SPELL_FOG_OF_CORRUPTION, true);

                            events.ScheduleEvent(EVENT_SUMMON_FOG, 500);
                            break;
                        }
                    }
                }
            }

            Creature* GetClosestFlightTarget() const
            {
                Creature* left = me->FindNearestCreature(NPC_FLIGHT_LEFT, 300.0f);
                Creature* right = me->FindNearestCreature(NPC_FLIGHT_RIGHT, 300.0f);
                if (left && right)
                    return me->GetExactDist(left) < me->GetExactDist(right) ? left : right;
                return left ? left : right;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_felmystAI(creature);
        }
};

class npc_felmyst_vapor : public CreatureScript
{
    public:
        npc_felmyst_vapor() : CreatureScript("npc_felmyst_vapor") { }

        struct npc_felmyst_vaporAI : public ScriptedAI
        {
            npc_felmyst_vaporAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetSpeed(MOVE_RUN, 0.8f);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->GetMotionMaster()->MoveFollow(summoner, 0.01f, 0.0f);
                DoCast(me, SPELL_DEMONIC_VAPOR_PERIODIC, true);
                DoCast(me, SPELL_DEMONIC_VAPOR_SPAWN_TRIGGER, true);
            }

            void Reset() override { }
            void JustEngagedWith(Unit* /*who*/) override { }
            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_felmyst_vaporAI(creature);
        }
};

class npc_felmyst_trail : public CreatureScript
{
    public:
        npc_felmyst_trail() : CreatureScript("npc_felmyst_trail") { }

        struct npc_felmyst_trailAI : public ScriptedAI
        {
            npc_felmyst_trailAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 0.01f); // core bug
                DoCast(me, SPELL_DEMONIC_VAPOR_TRAIL_PERIODIC, true);
                me->m_Events.Schedule(6000, [this]() { DoCast(me, SPELL_SUMMON_BLAZING_DEAD, true); });
            }

            void JustSummoned(Creature* summon) override
            {
                DoZoneInCombat(summon);
                summon->CastSpell(summon, SPELL_BLAZING_DEAD_PASSIVE, true);
            }

            void Reset() override { }
            void JustEngagedWith(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void MoveInLineOfSight(Unit* /*who*/) override { }
            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_felmyst_trailAI(creature);
        }
};

class spell_fog_of_corruption : public SpellScriptLoader
{
    public:
        spell_fog_of_corruption() : SpellScriptLoader("spell_fog_of_corruption") { }

        class spell_fog_of_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fog_of_corruption_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->CastSpell(target, SPELL_FOG_OF_CORRUPTION_CHARM_2, true);
                    target->CastSpell(target, SPELL_FOG_OF_CORRUPTION_CHARM_3, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_fog_of_corruption_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_fog_of_corruption_SpellScript();
        }
};

void AddSC_boss_felmyst()
{
    new boss_felmyst();
    new npc_felmyst_vapor();
    new npc_felmyst_trail();
    new spell_fog_of_corruption();
}
