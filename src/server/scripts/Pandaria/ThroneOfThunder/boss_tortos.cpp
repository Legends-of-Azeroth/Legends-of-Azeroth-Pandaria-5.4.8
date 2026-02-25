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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "Spell.h"
#include "Vehicle.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CreatureTextMgr.h"
#include "Unit.h"
#include "Player.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "Map.h"
#include "VehicleDefines.h"
#include "SpellInfo.h"
#include "throne_of_thunder.h"

enum Yells
{
    // Boss
    ANN_TURTLES                 = 0,      // Tortos lets out a booming call, attracting nearby turtles.
    ANN_FURIOUS_BREATH                    // Tortos prepares to unleash a [Furious Stone Breath]!
};

enum Spells
{
    // Boss
    SPELL_ZERO_POWER            = 72242,  // No Regen

    SPELL_KICK_SHELL_A          = 134030, // Boss aura for adding mechanic abilities to players in 130y radius.

    SPELL_CALL_OF_TORTOS        = 136294, // Dummy on eff 0 for summoning 3 turtles.
    SPELL_FURIOUS_STONE_BREATH  = 133939, // Triggers damage each 500 ms + prevents Fury regen for duration.
    SPELL_GROWING_FURY          = 136010, // When no players are in melee range. Adds 10 Fury.
    SPELL_SNAPPING_BITE         = 135251, // On tank, main ability.

    SPELL_QUAKE_STOMP           = 134920, // Massive AOE damage. Interruptible. Triggers SPELL_ROCKFALL_STOMP.
    SPELL_ROCKFALL_STOMP        = 134915, // 8 second aura triggering SPELL_ROCKFALL_STOMP_S_TRIG each 500 ms.
    SPELL_ROCKFALL_STOMP_S_TRIG = 140431, // Dummy on eff 0 for SPELL_ROCKFALL_SUMMON on random player.

    SPELL_ROCKFALL_AURA         = 134849, // Triggers SPELL_ROCKFALL_AURA_S_TRIG each 10 seconds. Permanent boss aura in fight.
    SPELL_ROCKFALL_AURA_S_TRIG  = 134364, // Dummy on eff 0 for SPELL_ROCKFALL_SUMMON on random player.
    SPELL_ROCKFALL_SUMMON       = 134365, // Summons NPC_ROCKFALL_TORTOS.

    SPELL_SUMMON_BATS           = 136686, // Summons 8 Vampiric Cave Bats.

    // Adds

    // Whirl Turtle
    SPELL_SPINNING_SHELL_VISUAL = 133974, // Spin + aura visual.
    SPELL_SPINNING_SHELL_DUMMY  = 140443, // Speed decrease + periodic dummy on effect 1 for SPELL_SPINNING_SHELL_DMG.
    SPELL_SPINNING_SHELL_DMG    = 134011, // Damage and knockback.

    SPELL_SHELL_BLOCK           = 133971, // Damage immune and kickable state aura.

    SPELL_KICK_SHELL_TRIGGER    = 134031, // Spell from mechanic abilities button. Sends turtles forward fast. Needs turtle aura SPELL_SHELL_BLOCK to become usable.
    SPELL_KICK_SHELL_STUN       = 134073, // Unused.

    SPELL_SHELL_CONCUSSION      = 134092, // When kicked, aura triggering SPELL_SHELL_CONCUSSION_INT and SPELL_SHELL_CONCUSSION_D_IN each 300 ms.
    SPELL_SHELL_CONCUSSION_INT  = 134091, // Spell casting interruption for 3 seconds in 8y.
    SPELL_SHELL_CONCUSSION_D_IN = 136431, // Damage taken increase by 50% in 8y.

    // Vampiric Cave Bat
    SPELL_DRAIN_THE_WEAK_A      = 135103, // Triggers SPELL_DRAIN_THE_WEAK_DMG if target is below 35% health and drains 50x damage dealt.
    SPELL_DRAIN_THE_WEAK_DMG    = 135101, // 25% weapon damage.
    SPELL_DRAIN_THE_WEAK_HEAL   = 135102,

    // Rockfall
    SPELL_ROCKFALL              = 134475, // Visual on ground and triggers 134539 missile drop + damage after 5 seconds.
    SPELL_ROCKFALL_LARGE_AOE    = 134476,

    // Humming Crystal - HEROIC only.
    SPELL_CRYSTAL_SHELL_AURA    = 137552, // Adds SPELL_CRYSTAL_SHELL_ABS, SPELL_CRYSTAL_SHELL_MOD_ABS to player attackers.
    SPELL_CRYSTAL_SHELL_ABS     = 137633, // Eff 0 absorb, eff 1 dummy for absorbing max 15% of player's hp.
    SPELL_CRYSTAL_SHELL_MOD_ABS = 137648, // Eff 0 mod absorb %, eff 1 dummy for adding player aura SPELL_CRYSTAL_SHELL_CAPPED on cap when absorbing max 75% player hp.
    SPELL_CRYSTAL_SHELL_CAPPED  = 140701,  // "Maximum capacity" dummy aura from Crystal Shield (at 5 stacks).

    // Misc
    SPELL_PLATFORM_DUMMY        = 82827,
    SPELL_TELEPORT_DEPTH        = 139852  // Tortos tele spell
};

enum Events
{
    // Boss
    EVENT_CALL_OF_TORTOS        = 1,
    EVENT_FURIOUS_STONE_BREATH,
    EVENT_RESET_CAST,
    EVENT_SNAPPING_BITE,
    EVENT_QUAKE_STOMP,
    EVENT_SUMMON_BATS,

    EVENT_GROWING_FURY,
    EVENT_REGEN_FURY_POWER,

    EVENT_BERSERK,

    // Whirl Turtle
    EVENT_SHELL_BLOCK,
    EVENT_KICKED
};

enum Timers
{
    // Boss
    TIMER_CALL_OF_TORTOS_F      = 21000,
    TIMER_CALL_OF_TORTOS_S      = 60500,

    TIMER_QUAKE_STOMP_F         = 27000,
    TIMER_QUAKE_STOMP_S         = 47000,

    TIMER_FURIOUS_STONE_BREATH  = 500,
    TIMER_RESET_CAST            = 6000,

    TIMER_SNAPPING_BITE_N       = 12000,
    TIMER_SNAPPING_BITE_H       = 8000,

    TIMER_CALL_BATS_F           = 57000,
    TIMER_CALL_BATS_S           = 50000,

    TIMER_GROWING_FURY          = 6000,
    TIMER_REGEN_FURY_POWER      = 450,

    TIMER_BERSERK_H             = 600000, // 10 minutes (Heroic).
    TIMER_BERSERK               = 780000  // 13 minutes.
};

enum Actions
{
    ACTION_SHELL_AVAILABLE,
    ACTION_SHELL_RESET,
    ACTION_SHELL_REMOVE,
};

class boss_tortos : public CreatureScript
{
    public:
        boss_tortos() : CreatureScript("boss_tortos") { }

        struct boss_tortosAI : public BossAI
        {
            boss_tortosAI(Creature* creature) : BossAI(creature, DATA_TORTOS), summons(me), vehicle(creature->GetVehicleKit())
            {
                me->setActive(true);
                ASSERT(vehicle);
            }

            Vehicle* vehicle;
            SummonList summons;
            EventMap energyRegen;
            bool breathScheduled;
            uint32 turtleHitCount;

            uint32 m_growingFuryCooldown;

            void Reset() override
            {
                me->SetReactState(REACT_DEFENSIVE);

                events.Reset();
                energyRegen.Reset();
                summons.DespawnAll();

                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                DoCast(me, SPELL_ZERO_POWER);
                me->SetPower(POWER_ENERGY, 0);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                breathScheduled = false;
                turtleHitCount = 0;

                if (instance)
                    instance->SetData(DATA_TORTOS, NOT_STARTED);

                _Reset();

                SetDeathCollision(me->getDeathState() == CORPSE);
                me->GetMap()->SetWorldState(WORLDSTATE_ONE_UP, 0);

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_SHELL_CONCUSSION_INT)
                {
                    if (breathScheduled)
                        me->SetPower(POWER_ENERGY, 0);
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_ONE_UP && ++turtleHitCount > 4)
                    me->GetMap()->SetWorldState(WORLDSTATE_ONE_UP, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                _JustEngagedWith();

                me->AddAura(SPELL_KICK_SHELL_A, me);
                me->AddAura(SPELL_ROCKFALL_AURA, me);

                events.ScheduleEvent(EVENT_CALL_OF_TORTOS, TIMER_CALL_OF_TORTOS_F);
                events.ScheduleEvent(EVENT_SNAPPING_BITE, IsHeroic() ? TIMER_SNAPPING_BITE_H : TIMER_SNAPPING_BITE_N);
                events.ScheduleEvent(EVENT_QUAKE_STOMP, TIMER_QUAKE_STOMP_F);
                events.ScheduleEvent(EVENT_SUMMON_BATS, TIMER_CALL_BATS_F);

                //events.ScheduleEvent(EVENT_GROWING_FURY, TIMER_GROWING_FURY);
                energyRegen.ScheduleEvent(EVENT_REGEN_FURY_POWER, TIMER_REGEN_FURY_POWER);
                energyRegen.ScheduleEvent(EVENT_BERSERK, TIMER_BERSERK);

                if (IsHeroic())
                    HandleSendActionToShell(ACTION_SHELL_AVAILABLE);

                m_growingFuryCooldown = 8000;
            }

            void EnterEvadeMode() override
            {
                me->AddUnitState(UNIT_STATE_EVADE);

                me->RemoveAllAuras();
                Reset();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->GetMotionMaster()->MovementExpired();
                me->GetMotionMaster()->MoveTargetedHome();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHELL_CAPPED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHELL_ABS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHELL_MOD_ABS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                    instance->SetBossState(DATA_TORTOS, FAIL);
                }

                _EnterEvadeMode();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                HandleSendActionToShell(ACTION_SHELL_RESET);
                _DespawnAtEvade();
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);
                DoCast(me, SPELL_ZERO_POWER);
                me->SetPower(POWER_ENERGY, 0);

                _JustReachedHome();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void DoMeleeOrGrowingFury()
            {
                Unit* victim = me->GetVictim();
                //Make sure our attack is ready and we aren't currently casting before checking distance
                if (me->isAttackReady())
                {
                    if (me->IsWithinMeleeRange(victim))
                    {
                        me->AttackerStateUpdate(victim);
                        me->resetAttackTimer();
                        return;
                    }
                    else
                    {
                        ThreatContainer::StorageType threatList = me->GetThreatManager().getThreatList();

                        for (ThreatContainer::StorageType::const_iterator itr = threatList.cbegin(); itr != threatList.cend(); ++itr)
                        {
                            if (Unit* target = (*itr)->getTarget())
                            {
                                if (me->IsWithinMeleeRange(target))
                                {
                                    me->AttackerStateUpdate(target);
                                    me->resetAttackTimer();
                                    return;
                                }
                            }
                        }
                    }
                    
                    if (!m_growingFuryCooldown)
                    {
                        events.ScheduleEvent(EVENT_GROWING_FURY, 1500);
                        m_growingFuryCooldown = 3000;
                    }
                }
            }

            void JustDied(Unit* killer) override
            {
                summons.DespawnAll();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHELL_CAPPED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHELL_ABS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CRYSTAL_SHELL_MOD_ABS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                _JustDied();

                HandleSendActionToShell(ACTION_SHELL_REMOVE);

                SetDeathCollision(true);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim() || !CheckInRoom())
                    return;

                if (instance && instance->IsWipe())
                {
                    EnterEvadeMode();
                    return;
                }
                
                energyRegen.Update(diff);

                while (uint32 eventId = energyRegen.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_REGEN_FURY_POWER:
                            me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) + 1);
                            energyRegen.ScheduleEvent(EVENT_REGEN_FURY_POWER, TIMER_REGEN_FURY_POWER);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_RESET_CAST:
                            breathScheduled = false;
                            break;
                    }
                }

                if (m_growingFuryCooldown)
                {
                    if (m_growingFuryCooldown <= diff)
                        m_growingFuryCooldown = 0;
                    else
                        m_growingFuryCooldown -= diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetPower(POWER_ENERGY) == 100 && !breathScheduled && !me->IsNonMeleeSpellCasted(true))
                {
                    events.ScheduleEvent(EVENT_FURIOUS_STONE_BREATH, TIMER_FURIOUS_STONE_BREATH);
                    breathScheduled = true;
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CALL_OF_TORTOS:
                            Talk(ANN_TURTLES);
                            DoCast(me, SPELL_CALL_OF_TORTOS);
                            events.ScheduleEvent(EVENT_CALL_OF_TORTOS, TIMER_CALL_OF_TORTOS_S);
                            break;
                        case EVENT_FURIOUS_STONE_BREATH:
                            Talk(ANN_FURIOUS_BREATH);
                            DoCast(me, SPELL_FURIOUS_STONE_BREATH);
                            energyRegen.RescheduleEvent(EVENT_REGEN_FURY_POWER, 5000);
                            energyRegen.ScheduleEvent(EVENT_RESET_CAST, 500);
                            break;
                        case EVENT_SNAPPING_BITE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_SNAPPING_BITE);
                            events.ScheduleEvent(EVENT_SNAPPING_BITE, IsHeroic() ? TIMER_SNAPPING_BITE_H : TIMER_SNAPPING_BITE_N);
                            break;
                        case EVENT_QUAKE_STOMP:
                            DoCast(me, SPELL_QUAKE_STOMP);
                            events.ScheduleEvent(EVENT_QUAKE_STOMP, TIMER_QUAKE_STOMP_S);
                            break;
                        case EVENT_SUMMON_BATS:
                            me->CastSpell(aBatPos[urand(0, 3)], SPELL_SUMMON_BATS, true);
                            events.ScheduleEvent(EVENT_SUMMON_BATS, TIMER_CALL_BATS_S);
                            break;
                        case EVENT_GROWING_FURY:
                            if (!me->IsWithinMeleeRange(me->GetVictim()))
                                DoCast(me, SPELL_GROWING_FURY);

                            events.ScheduleEvent(EVENT_GROWING_FURY, TIMER_GROWING_FURY);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeOrGrowingFury();
            }

            private:
                void CorpseRemoved(uint32&) override
                {
                    SetDeathCollision(false);
                }

                void SetDeathCollision(bool enabled)
                {
                    if (GameObject* deathCollision = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_TORTOS_COLLISION) : ObjectGuid::Empty))
                        deathCollision->SetPhaseMask(enabled ? me->GetPhaseMask() : 0, true);
                }

                void HandleSendActionToShell(int32 actionId)
                {
                    std::list<Creature*> shellsList;
                    GetCreatureListWithEntryInGrid(shellsList, me, NPC_HUMMING_CRYSTAL, 150.0f);

                    for (auto&& itr : shellsList)
                        itr->AI()->DoAction(actionId);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_tortosAI(creature);
        }
};

// Whirl Turtle 67966
class npc_whirl_turtle : public CreatureScript
{
    enum Events
    {
        EVENT_NONE,
        EVENT_MOVE,
        EVENT_INIT_MOVE
    };

    public:
        npc_whirl_turtle() : CreatureScript("npc_whirl_turtle") { }

        struct npc_whirl_turtleAI : public ScriptedAI
        {
            npc_whirl_turtleAI(Creature* creature) : ScriptedAI(creature) { }

            bool shellBlocked;
            EventMap events;
            InstanceScript* instance;

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                instance = me->GetInstanceScript();
                Reset();
                shellBlocked = false;
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_SPINNING_SHELL_VISUAL, true);
                DoCast(me, SPELL_SPINNING_SHELL_DUMMY, true);

                events.ScheduleEvent(EVENT_INIT_MOVE, 200);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if ((me->HealthBelowPct(5) || damage >= me->GetHealth()) && !shellBlocked)
                {
                    damage = 0;
                    me->RemoveAurasDueToSpell(SPELL_SPINNING_SHELL_VISUAL);
                    me->RemoveAurasDueToSpell(SPELL_SPINNING_SHELL_DUMMY);

                    me->AddAura(SPELL_SHELL_BLOCK, me);
                    me->GetMotionMaster()->MovementExpired();
                    me->GetMotionMaster()->MoveIdle();
                    shellBlocked = true;
                }
            }

            void Move()
            {
                me->GetMotionMaster()->MovePoint(2, tortosCenter.GetPositionX() + frand(-34.0f, 34.0f), tortosCenter.GetPositionY() + frand(-27.0f, 27.0f), tortosCenter.GetPositionZ());
                events.ScheduleEvent(EVENT_MOVE, me->GetSplineDuration());
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE || shellBlocked)
                    return;

                if (pointId == 4)
                {
                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, false);
                    events.ScheduleEvent(EVENT_MOVE, 200 + rand() % 500);
                    me->SetWalk(true);
                    me->SetSpeed(MOVE_WALK, 1.8f, true);
                }
                if (pointId == 2)
                    events.RescheduleEvent(EVENT_MOVE, 200);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_KICK_SHELL_TRIGGER)
                {
                    me->AddAura(SPELL_SHELL_CONCUSSION, me);

                    float x, y;
                    GetPositionWithDistInOrientation(caster, 80.0f, caster->GetOrientation(), x, y);
                    me->GetMotionMaster()->MoveCharge(x, y, -61.19f, 30.0f);
                    me->DespawnOrUnsummon(6000);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (shellBlocked)
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_MOVE:
                        Move();
                        break;
                    case EVENT_INIT_MOVE:
                        me->SetSpeed(MOVE_RUN, 8.2f);
                        me->SetWalk(false);
                        Position pos = me->GetRandomPoint(tortosCenter, 8.f);
                        me->GetMotionMaster()->MovePoint(4, pos);
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_whirl_turtleAI(creature);
        }
};

// Vampiric Cave Bat 68497
class npc_vampiric_cave_bat : public CreatureScript
{
    public:
        npc_vampiric_cave_bat() : CreatureScript("npc_vampiric_cave_bat") { }

        struct npc_vampiric_cave_batAI : public ScriptedAI
        {
            npc_vampiric_cave_batAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                Reset();
                DoZoneInCombat(me, 130.0f);
            }

            void Reset() override
            {
                me->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_CAN_FLY);
                me->AddAura(SPELL_DRAIN_THE_WEAK_A, me);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_vampiric_cave_batAI(creature);
        }
};

// Rockfall 68219
class npc_rockfall_tortos : public CreatureScript
{
    public:
        npc_rockfall_tortos() : CreatureScript("npc_rockfall_tortos") { }

        struct npc_rockfall_tortosAI : public ScriptedAI
        {
            npc_rockfall_tortosAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                Reset();
                me->SetDisplayFromModel(0);
                DoCast(me, SPELL_ROCKFALL, true);
                me->DespawnOrUnsummon(10000);
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void UpdateAI(uint32 diff) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_rockfall_tortosAI(creature);
        }
};

// Humming Crystal 69639 - HEROIC only
struct npc_humming_crystal : public ScriptedAI
{
    npc_humming_crystal(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        me->SetReactState(REACT_PASSIVE);

        DoCast(me, SPELL_CRYSTAL_SHELL_AURA, true);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_SHELL_AVAILABLE:
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                break;
            case ACTION_SHELL_RESET:
                EnterEvadeMode();
                break;
            case ACTION_SHELL_REMOVE:
                me->DisappearAndDie();
                break;
        }
    }

    void UpdateAI(uint32 diff) override { }
};

// Call of Tortos 136294
class spell_call_of_tortos : public SpellScriptLoader
{
    public:
        spell_call_of_tortos() : SpellScriptLoader("spell_call_of_tortos") { }

        class spell_call_of_tortos_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_call_of_tortos_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    for (uint8 i = 0; i < 3; i++)
                        caster->SummonCreature(NPC_WHIRL_TURTLE, aTurtlePos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000 + rand() % 5000);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_call_of_tortos_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_call_of_tortos_SpellScript();
        }
};

// Rockfall 140431, 134364
class spell_rockfall_trigger_tortos : public SpellScriptLoader
{
    public:
        spell_rockfall_trigger_tortos() : SpellScriptLoader("spell_rockfall_trigger_tortos") { }

        class spell_rockfall_trigger_tortos_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rockfall_trigger_tortos_SpellScript);

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetHitUnit())
                        caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rockfall_trigger_tortos_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_rockfall_trigger_tortos_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rockfall_trigger_tortos_SpellScript();
        }
};

class spell_rockfall_damage : public SpellScriptLoader
{
    public:
        spell_rockfall_damage() : SpellScriptLoader("spell_rockfall_damage") { }

        class spell_rockfall_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rockfall_damage_SpellScript);

            void HandleOnCast()
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_ROCKFALL_LARGE_AOE, true);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_rockfall_damage_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rockfall_damage_SpellScript();
        }
};

class spell_rockfall_aoe_damage : public SpellScriptLoader
{
    public:
        spell_rockfall_aoe_damage() : SpellScriptLoader("spell_rockfall_aoe_damage") { }

        class spell_rockfall_aoe_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rockfall_aoe_damage_SpellScript);

            void SelectTargets(std::list<WorldObject*>&targets)
            {
                targets.remove_if([=](WorldObject* target) { return target && (!target->ToPlayer() || target->ToPlayer() && target->ToPlayer()->IsGameMaster()); });
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rockfall_aoe_damage_SpellScript::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rockfall_aoe_damage_SpellScript();
        }
};

class spinningShellPredicate
{
    private:
        Unit* caster;
    public:
        spinningShellPredicate(Unit* _caster) : caster(_caster) { }

        bool operator()(WorldObject* target) const
        {
            if (target->GetExactDist2d(caster) < 5.1f && target->GetPositionZ() <= (caster->GetPositionZ() + 1.f))
                return false;

            return true;
        }
};

// Spinning Shell 140443
class spell_spinning_shell : public AuraScript
{
    PrepareAuraScript(spell_spinning_shell)

    void OnPeriodic(AuraEffect const* aurEff)
    {
        if (GetCaster() && !GetCaster()->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
        {
            PreventDefaultAction();
            Remove(AURA_REMOVE_BY_EXPIRE);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spinning_shell::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Shell Concussion 136431
class spell_shell_concussion : public SpellScript
{
    PrepareSpellScript(spell_shell_concussion);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* turtle = GetHitCreature())
        {
            if (turtle->GetEntry() != NPC_WHIRL_TURTLE)
                return;

            if (Creature* tortos = ObjectAccessor::GetCreature(*turtle, turtle->GetInstanceScript() ? turtle->GetInstanceScript()->GetGuidData(DATA_TORTOS) : ObjectGuid::Empty))
                tortos->AI()->SetData(TYPE_ONE_UP, 1);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([this](WorldObject* target) -> bool {return target->GetEntry() != NPC_TORTOS && target->GetEntry() != NPC_VAMPIRIC_CAVE_BAT && target->GetEntry() != NPC_WHIRL_TURTLE; });
        targets.remove_if([this](WorldObject* target) -> bool {return target->ToUnit() && target->ToUnit()->HasAura(SPELL_SHELL_CONCUSSION_D_IN); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_shell_concussion::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shell_concussion::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Drain of the Weak 135103
class spell_drain_the_weak : public AuraScript
{
    PrepareAuraScript(spell_drain_the_weak);

    void HandleProc(ProcEventInfo& procInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Unit* target = procInfo.GetActionTarget())
            {
                if ((int32)target->GetHealth() < sSpellMgr->GetSpellInfo(SPELL_DRAIN_THE_WEAK_A, target->GetMap()->GetDifficulty())->Effects[0].BasePoints)
                {
                    owner->CastSpell(target, SPELL_DRAIN_THE_WEAK_DMG, true);
                    
                    // Heal
                    if (const SpellInfo* sInfo = sSpellMgr->GetSpellInfo(SPELL_DRAIN_THE_WEAK_HEAL, target->GetMap()->GetDifficulty()))
                    {
                        int32 bp = sInfo->Effects[EFFECT_0].BasePoints * (int32)procInfo.GetDamageInfo()->GetDamage();
                        owner->CastCustomSpell(owner, SPELL_DRAIN_THE_WEAK_HEAL, &bp, 0, 0, true);
                    }
                }
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_drain_the_weak::HandleProc);
    }
};

// Crystall Shell Proc 137552
class spell_crystall_shell_proc : public AuraScript
{
    PrepareAuraScript(spell_crystall_shell_proc);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* victim = eventInfo.GetProcTarget())
        {
            if (victim->GetTypeId() != TYPEID_PLAYER)
                return;

            GetUnitOwner()->CastSpell(victim, SPELL_CRYSTAL_SHELL_MOD_ABS, true);

            if (!victim->HasAura(SPELL_CRYSTAL_SHELL_ABS))
            {
                int32 bp = (int32)(victim->GetMaxHealth() * 0.15f);
                GetUnitOwner()->CastCustomSpell(victim, SPELL_CRYSTAL_SHELL_ABS, &bp, 0, 0, true);
            }
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_crystall_shell_proc::HandleOnProc);
    }
};

// Crystall Shell Mod Abs 137648
class spell_crystal_shell_mod_abs : public AuraScript
{
    PrepareAuraScript(spell_crystal_shell_mod_abs);

    void OnAbsorb(AuraEffect* aurEff, uint32& heal, uint32& absorb)
    {
        if (Unit* owner = GetUnitOwner())
        {
            // Prevent if encounter is done
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_TORTOS) == DONE)
                return;

            if (Aura* shell = owner->GetAura(SPELL_CRYSTAL_SHELL_ABS))
            {
                int32 maxAbsorb = int32(owner->GetMaxHealth() * 0.75);
                int32 amountDiff = shell->GetEffect(EFFECT_0)->GetAmount() + absorb;

                // Max Absorb by heal is 75% of max HP
                if (amountDiff >= maxAbsorb)
                {
                    amountDiff = maxAbsorb;
                    owner->CastSpell(owner, SPELL_CRYSTAL_SHELL_CAPPED, true);
                }
                else
                    owner->RemoveAurasDueToSpell(SPELL_CRYSTAL_SHELL_CAPPED);

                shell->GetEffect(EFFECT_0)->ChangeAmount(amountDiff);
                shell->SetNeedClientUpdateForTargets();
                shell->RefreshDuration();
                RefreshDuration();
            }
        }
    }

    void Register() override
    {
        OnEffectAbsorbHeal += AuraEffectAbsorbHealFn(spell_crystal_shell_mod_abs::OnAbsorb, EFFECT_0);
    }
};

// Quake Stomp 134920
class spell_tortos_quake_stomp : public SpellScriptLoader
{
    public:
        spell_tortos_quake_stomp() : SpellScriptLoader("spell_tortos_quake_stomp") { }

        class spell_tortos_quake_stomp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tortos_quake_stomp_SpellScript);

            void CalculateDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                uint32 maxHealth = target->GetMaxHealth();

                float multiplier = (sSpellMgr->GetSpellInfo(SPELL_QUAKE_STOMP, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints) * 0.01f;

                SetHitDamage(maxHealth * multiplier);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tortos_quake_stomp_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tortos_quake_stomp_SpellScript();
        }
};

class spell_furious_stone_breath : public SpellScriptLoader
{
    public:
        spell_furious_stone_breath() : SpellScriptLoader("spell_furious_stone_breath") { }

        class spell_furious_stone_breath_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_furious_stone_breath_AuraScript);

            void HandleOnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Creature* creature = GetCaster()->ToCreature())
                    creature->SetPower(POWER_ENERGY, 0);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_furious_stone_breath_AuraScript::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_furious_stone_breath_AuraScript();
        }
};

class spell_teleport_all : public SpellScriptLoader
{
    public:
        spell_teleport_all() : SpellScriptLoader("spell_teleport_all") { }

        class spell_teleport_all_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_teleport_all_AuraScript);

            void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* Owner = GetOwner()->ToUnit())
                {
                    Owner->SetControlled(false, UNIT_STATE_STUNNED);
                    Owner->NearTeleportTo(6041.22f, 5085.77f, -42.f, 4.81f);

                    Owner->RemoveAurasDueToSpell(130);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_teleport_all_AuraScript::HandleAuraEffectRemove, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_teleport_all_AuraScript();
        }
};

class spell_waterspout_aura : public SpellScriptLoader
{
    public:
        spell_waterspout_aura() : SpellScriptLoader("spell_waterspout_aura") { }

        class spell_waterspout_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_waterspout_aura_AuraScript);

            void HandleOnPeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();

                if (Unit* pOwner = GetOwner()->ToUnit())
                    pOwner->CastSpell(pOwner, 139165, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_waterspout_aura_AuraScript::HandleOnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_waterspout_aura_AuraScript();
        }
};

// Knock Away 140688
class spell_tortos_knock_away : public SpellScript
{
    PrepareSpellScript(spell_tortos_knock_away);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([this](WorldObject* target) -> bool { return target->ToUnit() && target->ToUnit()->GetPositionY() > 4935.0f; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tortos_knock_away::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// spell_crystal_shell_aura 137633
class spell_crystal_shell_aura : public AuraScript
{
    PrepareAuraScript(spell_crystal_shell_aura)

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_CRYSTAL_SHELL_MOD_ABS);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_crystal_shell_aura::HandleRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// Areatrigger 8937
class AreaTrigger_at_tortos_intro : public AreaTriggerScript
{
    public:
        AreaTrigger_at_tortos_intro() : AreaTriggerScript("at_tortos_intro") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (Creature* introLeiShen = ObjectAccessor::GetCreature(*player, instance ? instance->GetGuidData(NPC_LEI_SHEN_TRIGGER) : ObjectGuid::Empty))
                {
                    if (instance->GetData(DATA_TORTOS_EVENT) == DONE && !player->HasAura(SPELL_TELEPORT_DEPTH))
                    {
                        player->AddAura(SPELL_TELEPORT_DEPTH, player);
                        return true;
                    }

                    introLeiShen->AI()->DoAction(ACTION_START_INTRO);
                }
            }

            return true;
        }
};

class sat_waterspout : public SpellAreaTriggerScript
{
    enum eSpells : uint32
    {
        SPELL_WATERSPOUT_TRIGGER        = 139158,
        SPELL_WATERSPOUT_SPOUT          = 139159,
    };

    public:
        sat_waterspout() : SpellAreaTriggerScript("sat_waterspout") { }

        class sat_waterspout_IAreaTriggerAura : public IAreaTriggerAura
        {
            bool CheckTriggering(WorldObject* triggering)
            {
                Player* player = triggering->ToPlayer();

                if (!player)
                    return false;
                
                if (player->HasAura(SPELL_WATERSPOUT_TRIGGER))
                    return false;

                return player->IsAlive();
            }

            void OnTriggeringApply(WorldObject* triggering)
            {
                Unit* auraHolder = m_target->ToUnit();

                if (!auraHolder)
                    return;

                if (!auraHolder->HasAura(SPELL_WATERSPOUT_SPOUT))
                    auraHolder->AddAura(SPELL_WATERSPOUT_SPOUT, auraHolder);
            }
        };

        IAreaTrigger* GetInterface() const override
        {
            return new sat_waterspout_IAreaTriggerAura();
        }
};

class sat_shell_spin : public SpellAreaTriggerScript
{
    public:
        sat_shell_spin() : SpellAreaTriggerScript("sat_shell_spin") { }

        class sat_shell_spin_IAreaTriggerAura : public IAreaTriggerAura
        {
            bool CheckTriggering(WorldObject* triggering)
            {
                Player* player = triggering->ToPlayer();

                if (!player)
                    return false;

                if (player->HasAura(SPELL_SPINNING_SHELL_DUMMY))
                    return false;

                return player->IsAlive();
            }

            void OnTriggeringApply(WorldObject* triggering)
            {
                if (Unit* target = triggering->ToUnit())
                {
                    target->CastSpell(target, SPELL_SPINNING_SHELL_DMG, true, 0, 0, m_caster->GetGUID());
                    target->AddAura(SPELL_SPINNING_SHELL_DUMMY, target);
                }
            }
        };

        IAreaTrigger* GetInterface() const override
        {
            return new sat_shell_spin_IAreaTriggerAura();
        }
};

void AddSC_boss_tortos()
{
    new boss_tortos();
    new npc_whirl_turtle();
    new npc_vampiric_cave_bat();
    new npc_rockfall_tortos();
    new creature_script<npc_humming_crystal>("npc_humming_crystal");
    new spell_call_of_tortos();
    new spell_rockfall_trigger_tortos();
    new spell_rockfall_damage();
    new spell_rockfall_aoe_damage();
    new aura_script<spell_spinning_shell>("spell_spinning_shell");
    new spell_script<spell_shell_concussion>("spell_shell_concussion");
    new aura_script<spell_drain_the_weak>("spell_drain_the_weak");
    new aura_script<spell_crystall_shell_proc>("spell_crystall_shell_proc");
    new aura_script<spell_crystal_shell_mod_abs>("spell_crystal_shell_mod_abs");
    new spell_tortos_quake_stomp();
    new spell_furious_stone_breath();
    new spell_teleport_all();
    new spell_waterspout_aura();
    new spell_script<spell_tortos_knock_away>("spell_tortos_knock_away");
    new aura_script<spell_crystal_shell_aura>("spell_crystal_shell_aura");
    new AreaTrigger_at_tortos_intro();
    new sat_waterspout();
    new sat_shell_spin();
}
