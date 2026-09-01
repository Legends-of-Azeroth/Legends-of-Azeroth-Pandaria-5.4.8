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
#include "blackwing_descent.h"
#include "MoveSplineInit.h"
#include "VMapFactory.h"
#include "VMapManager2.h"

enum ScriptTexts
{
    // Nefarian
    SAY_AVOID_WIPE                  = 0,
    SAY_AGGRO                       = 1,
    SAY_AIR_PHASE                   = 2,
    SAY_AIR_PHASE_2                 = 3,
    SAY_FINAL_PHASE                 = 4,
    SAY_SHADOWFLAME                 = 5,
    SAY_KILL                        = 6,
    SAY_DEATH                       = 7,
    SAY_ELECTRICITY_ANNOUNCE        = 8,

    // Lord Victor Nefarius
    SAY_INTRO_1                     = 0,
    SAY_INTRO_2                     = 1,
    SAY_INTRO_3                     = 2,

    // Onyxia
    SAY_ELECTRICITY_WARNING         = 0,
};

enum Spells
{
    SPELL_CHILDREN_OF_DEATHWING_NEF     = 80787,
    SPELL_CHILDREN_OF_DEATHWING_ONY     = 80785,
    SPELL_MAGMA                         = 81114,

    // Nefarian
    SPELL_ELECTROCUTE                   = 81198,
    SPELL_ELECTROCUTE_DMG               = 81272,
    SPELL_SHADOWFLAME_BREATH            = 77826,
    SPELL_ANIMATE_BONES                 = 78122,
    SPELL_HAIL_OF_BONES                 = 78679,
    SPELL_TAIL_LASH                     = 77827,
    SPELL_SHADOWBLAZE                   = 81007,
    SPELL_SHADOWBLAZE_SPARK             = 81031,
    SPELL_SHADOWFLAME_BARRAGE           = 78621,
    SPELL_H_EXPLOSIVE_CINDERS_PERIODIC  = 79339,
    SPELL_H_EXPLOSIVE_CINDERS_SUMM_DMG  = 79347,
    SPELL_SHADOW_COWARDICE              = 79353,
    SPELL_BERSERK_NEF                   = 26662,
    SPELL_INTRO_1_TRANSFORM_VISUAL      = 78205,
    SPELL_INTRO_2_STALKER_TRANSFORM     = 78750,
    SPELL_INTRO_3_SHRINK_AURA           = 78753,
    SPELL_INTRO_4_LIFT_OFF_ANIM_KIT     = 78692,
    SPELL_INTRO_5A_START_FIGHT_PROC     = 78730,
    SPELL_HITTING_YA                    = 94132,

    // Heroic Dominion Mechanic
    SPELL_DOMINION                      = 79318,
    SPELL_DOMINION_IMMUNITY             = 95900,
    SPELL_DOMINION_TARGET               = 94211,
    SPELL_DOMINION_PORTAL_BEAM          = 81709,
    SPELL_DOMINION_PORTAL_TRIGGER       = 81752,
    SPELL_SUMMON_DOMINION_STALKER_N     = 81665,
    SPELL_SUMMON_DOMINION_STALKER_E     = 81745,
    SPELL_SUMMON_DOMINION_STALKER_S     = 81746,
    SPELL_SUMMON_DOMINION_STALKER_W     = 81747,
    SPELL_SIPHON_POWER                  = 79319,
    SPELL_STOLEN_POWER                  = 80627,
    SPELL_FREE_YOUR_MIND                = 79323,

    // Onyxia
    SPELL_ONYXIA_ELECTRICAL_CHARGE      = 78949,
    SPELL_ONYXIA_ELECTRICAL_CHARGE_INC  = 95793,
    SPELL_ONYXIA_ELECTRICAL_CHARGE_DMG  = 78999,
    SPELL_LIGHTNING_DISCHARGE           = 78090,
    SPELL_LIGHTNING_DISCHARGE_TRIGGERED = 77836,
    SPELL_LIGHTNING_DISCHARGE_F_L       = 77939,
    SPELL_LIGHTNING_DISCHARGE_B_L       = 77942,
    SPELL_LIGHTNING_DISCHARGE_F_R       = 77943,
    SPELL_LIGHTNING_DISCHARGE_B_R       = 77944,
    SPELL_LIGHTNING_DISCHARGE_F_L_VIS   = 81435,
    SPELL_LIGHTNING_DISCHARGE_B_L_VIS   = 81436,
    SPELL_LIGHTNING_DISCHARGE_F_R_VIS   = 81437,
    SPELL_LIGHTNING_DISCHARGE_B_R_VIS   = 81438,

    // Anim Bone Warriors
    SPELL_NO_REGEN                      = 78725,
    SPELL_NO_REGEN2                     = 72242,
    SPELL_EMPOWER                       = 79329, 
    SPELL_HURL_BONE                     = 81586,
    SPELL_DIE_VISUAL                    = 70628,

    // Chromatic Prototype
    SPELL_BLAST_NOVA                    = 80734,

    // Shadowblaze Flashpoint
    SPELL_FIREBRUSH_AURA                = 79396,
    SPELL_BRUSHFIRE_SUMMON              = 79405,

    // Invisible Stalker
    SPELL_CHAINS_A_STEEL                = 81159,
    SPELL_CHAINS_B_STEEL                = 81174,
    SPELL_CHAINS_C_STEEL                = 81176,
    SPELL_CHAINS_A_LIGHTNING            = 81158,
    SPELL_CHAINS_B_LIGHTNING            = 81175,
    SPELL_CHAINS_C_LIGHTNING            = 81177,
    SPELL_AURA_OF_DREAD                 = 78495,
};

enum Phases
{
    PHASE_NULL = 0,
    PHASE_INTRO,
    PHASE_GROUND,
    PHASE_FLIGHT,
    PHASE_FINAL
};

enum Events
{
    // Intro for Lord Victor Nefarius
    EVENT_NEFARIUS_INTRO_1 = 1,
    EVENT_NEFARIUS_INTRO_2,
    EVENT_NEFARIUS_INTRO_3,
    EVENT_SUMMON_ONYXIA,
    EVENT_SUMMON_NEFARIAN,
    EVENT_TRANSFORM,
    EVENT_HIDE_NEFARIUS,
    EVENT_LAVA_CHECK,

    // Intro for Nefarian
    EVENT_NEFARIAN_INTRO_1,
    EVENT_NEFARIAN_INTRO_2,
    EVENT_NEFARIAN_INTRO_START_ANIM,
    EVENT_NEFARIAN_INTRO_GROW,
    EVENT_NEFARIAN_INTRO_STOP_VISUAL,
    EVENT_NEFARIAN_INTRO_NAG,
    EVENT_INITIAL_DESCEND,
    EVENT_BERSERK,
    EVENT_NEXT_FLIGHT_WAYPOINT,

    // Ground phase
    EVENT_SHADOWFLAME_BREATH,
    EVENT_SHADOW_COWARDICE,
    EVENT_TAIL_LASH,
    EVENT_LIFTOFF_START,
    EVENT_LIFTOFF_MOVE,
    EVENT_DOMINION,
    EVENT_ELECTROCUTE,

    // Air phase
    EVENT_SHADOWFLAME_BARRAGE,
    EVENT_EXPLOSIVE_CINDERS,
    EVENT_SUMMON_CHROMATIC,
    EVENT_LAND,
    EVENT_FORCED_TIMED_LANDING,

    // Animated Bone Warrior
    EVENT_HURL_BONE,

    // Chromatic Prototype
    EVENT_BLAST_NOVA,

    // Shadowblaze Flashpoint
    EVENT_SHADOWBLAZE_SPAWN,

    // Final phase
    EVENT_SHADOWBLAZE,

    // Onyxia
    EVENT_LIGHTNING_DISCHARGE,
};

enum Npc
{
    NPC_NEFARIAN_INTRO                  = 41379,
    NPC_CHROMATIC_PROTO                 = 41948,
    NPC_ANIM_BONE_WARR                  = 41918, 
    MOB_SHADOWBLAZE                     = 42596,
    NPC_INVISIBLE_STALKER               = 42844,
    NPC_INVISIBLE_STALKER_FLOATING      = 42098,
    NPC_NEFARIANS_LIGHTNING_MACHINE     = 51089,
};

enum Actions
{
    ACTION_RESET = 0,
    ACTION_START_ENCOUNTER,
    ACTION_NEFARIAN_START_INTRO,
    ACTION_ONYXIA_AGGROED,
    ACTION_ONYXIA_DEAD,
    ACTION_CHROMATIC_PROTOTYPE_DEAD,
    ACTION_KILL_ANIMATED_WARRIOR,
};

enum Points
{
    POINT_FLIGHT = 1,
    POINT_DESCEND,
    POINT_ASCEND,
};

// Nefarian flight before Onyxia enters combat
#define NEFARIAN_FLIGHT_WAYPOINT_COUNT 16
Position const nefarianFlightWaypoints[NEFARIAN_FLIGHT_WAYPOINT_COUNT] =
{
    { -174.641f, -225.222f, 92.3193f },
    { -169.394f, -250.802f, 91.7818f },
    { -154.976f, -272.201f, 92.1151f },
    { -132.934f, -286.891f, 92.2541f },
    { -106.795f, -292.264f, 93.0691f },
    { -81.6632f, -287.300f, 93.8192f },
    { -59.4271f, -272.554f, 94.2915f },
    { -44.6042f, -251.019f, 94.4025f },
    { -39.8698f, -225.021f, 93.9301f },
    { -44.6285f, -198.873f, 94.1525f },
    { -59.6580f, -176.899f, 94.8191f },
    { -81.1875f, -162.474f, 94.7080f },
    { -107.082f, -157.372f, 94.2081f },
    { -132.807f, -162.351f, 93.4026f },
    { -154.677f, -177.123f, 92.9582f },
    { -169.151f, -199.177f, 92.2359f },
};

// Chromatic Prototype spawn positions
Position ChromaticPositions[3] =
{
    { -86.7713f, -190.62083f, 3.05710f, 0.00f },
    { -87.0204f, -258.60060f, 3.05750f, 0.00f },
    { -148.177f, -224.47300f, 3.05815f, 0.00f },
};

// Stalkers that cast chain visuals on Onyxia during intro
const Position onyxiaChainStalkers[3] =
{
    { -141.333f, -224.625f, 1.51210f, 0.00000f },
    { -90.3562f, -253.471f, 1.46139f, 2.11185f },
    { -90.2205f, -195.607f, 1.50508f, 4.11898f },
};
const Spells onyxiaChainSpells[6] =
{
    SPELL_CHAINS_A_STEEL,
    SPELL_CHAINS_B_STEEL,
    SPELL_CHAINS_C_STEEL,
    SPELL_CHAINS_A_LIGHTNING,
    SPELL_CHAINS_B_LIGHTNING,
    SPELL_CHAINS_C_LIGHTNING,
};

void SetOnElevator(Unit* unit, bool useTransport = true)
{
    if (InstanceScript* instance = unit->GetInstanceScript())
    {
        if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
        {
            unit->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
            if (useTransport)
            {
                float x, y, z, o;
                unit->GetPosition(x, y, z, o);
                // TODO: port fixes for elevators
                //elevator->CalculatePassengerOffset(x, y, z, &o);
                unit->m_movementInfo.transport.guid = elevator->GetGUID();
                unit->m_movementInfo.transport.pos.Relocate(x, y, z, o);
                unit->UpdateObjectVisibility();
                Movement::MoveSplineInit init(unit);
                init.MoveTo(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), false, true);
                init.Launch();
            }
            else if (unit->m_movementInfo.transport.guid)
            {
                // Silently update position, client will continue to show the unit on top of the elevator
                unit->m_movementInfo.transport.guid = ObjectGuid::Empty;
                unit->m_movementInfo.transport.seat = -1;
                unit->m_movementInfo.transport.pos.Relocate(0, 0, 0, 0);
                unit->UpdatePosition(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ() + 13.90172f, elevator->GetOrientation());
            }
        }
    }
}

class boss_bd_nefarian : public CreatureScript
{
    public:
        boss_bd_nefarian() : CreatureScript("boss_bd_nefarian") { }

        struct boss_bd_nefarianAI : public BossAI
        {
            boss_bd_nefarianAI(Creature* creature) : BossAI(creature, DATA_NEFARIAN)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
                waypointFlightActive = false;
                nextFlightWaypointIndex = 0;
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                healthPct   = 90;
                phase       = PHASE_NULL;
                said        = false;
                secondPhase = false;
                finalPhase  = false;
                me->GetMap()->SetWorldState(WORLDSTATE_KEEPING_IT_IN_THE_FAMILY, 0);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                DoCast(me, SPELL_HITTING_YA, true);

                RemoveSummons();
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_INTRO_2_STALKER_TRANSFORM);
                DoCast(me, SPELL_INTRO_3_SHRINK_AURA);
            }

            void RemoveSummons()
            {
                if (SummonList.empty())
                    return;

                for (std::list<ObjectGuid>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                    if (Creature* summon = ObjectAccessor::GetCreature(*me, *itr))
                        summon->DespawnOrUnsummon();

                SummonList.clear();
            }
            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                Reset();

                me->SetReactState(REACT_PASSIVE);
                me->RemoveAurasDueToSpell(SPELL_HAIL_OF_BONES);

                // Resume flight
                if (!waypointFlightActive)
                {
                    me->SetFlying(true);
                    nextFlightWaypointIndex = (GetClosestWaypoint() + 1) % NEFARIAN_FLIGHT_WAYPOINT_COUNT;
                    events.ScheduleEvent(EVENT_NEXT_FLIGHT_WAYPOINT, 1);
                }

                // Respawn Onyxia
                if (Creature* onyxia = me->FindNearestCreature(NPC_ONYXIA, 250))
                    onyxia->DespawnOrUnsummon();
                if (Creature* nefarius = me->FindNearestCreature(NPC_NEFARIAN_INTRO, 250))
                    nefarius->AI()->DoAction(ACTION_RESET);

                // Raise the elevator
                if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                    elevator->SetGoState(GO_STATE_READY);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                RemoveSummons();
                me->RemoveAllAuras();
                me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 8.489f, me->GetPositionZ());
                me->SetFlying(true);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                Talk(SAY_DEATH);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                uint32 pct = (uint32)(100.0f * (me->GetHealth() - damage) / me->GetMaxHealth());
                if (pct < healthPct && healthPct > 0)
                {
                    healthPct = (pct / 10) * 10;

                    Talk(SAY_ELECTRICITY_ANNOUNCE);
                    events.ScheduleEvent(EVENT_ELECTROCUTE, 5000);
                }
            }

            void EnterPhaseIntro()
            {
                phase = PHASE_INTRO;
                events.Reset();
                events.SetPhase(PHASE_INTRO);
                events.ScheduleEvent(EVENT_NEFARIAN_INTRO_1, 8000);
                events.ScheduleEvent(EVENT_NEFARIAN_INTRO_2, 11000);
                events.ScheduleEvent(EVENT_NEFARIAN_INTRO_START_ANIM, 250);
                events.ScheduleEvent(EVENT_NEFARIAN_INTRO_GROW, 500);
                events.ScheduleEvent(EVENT_NEFARIAN_INTRO_STOP_VISUAL, 3000);
                events.ScheduleEvent(EVENT_NEFARIAN_INTRO_NAG, 30000);
            }

            void EnterPhaseGround()
            {
                phase = PHASE_GROUND;
                events.Reset();
                events.SetPhase(PHASE_GROUND);
                events.ScheduleEvent(EVENT_SHADOWFLAME_BREATH,  5000,   PHASE_GROUND);
                events.ScheduleEvent(EVENT_SHADOW_COWARDICE,    20000,  PHASE_GROUND);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_DOMINION,        10000,  PHASE_GROUND);
            }

            void EnterPhaseAir()
            {
                phase = PHASE_FLIGHT;
                uint32 timeToForcedLanding = events.GetTimeUntilEvent(EVENT_FORCED_TIMED_LANDING);
                events.Reset();
                events.SetPhase(PHASE_FLIGHT);
                events.ScheduleEvent(EVENT_SHADOWFLAME_BARRAGE, 4000,   PHASE_FLIGHT);
                events.ScheduleEvent(EVENT_SUMMON_CHROMATIC,    10000,  PHASE_FLIGHT);
                events.ScheduleEvent(EVENT_FORCED_TIMED_LANDING, timeToForcedLanding, PHASE_FLIGHT);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_EXPLOSIVE_CINDERS, 7000, PHASE_FLIGHT);
            }

            void EnterPhaseFinal()
            {
                phase = PHASE_FINAL;
                events.Reset();
                events.SetPhase(PHASE_FINAL);
                events.ScheduleEvent(EVENT_TAIL_LASH,           4000, PHASE_FINAL);
                events.ScheduleEvent(EVENT_SHADOWFLAME_BREATH,  5000, PHASE_FINAL);
                events.ScheduleEvent(EVENT_SHADOWBLAZE,         9000, PHASE_FINAL);
            }

            void JustSummoned(Creature* summon) override
            {
                if (!me->IsInCombat())
                {
                    summon->DespawnOrUnsummon();
                    return;
                }

                SummonList.push_back(summon->GetGUID());
                switch (summon->GetEntry())
                {
                    case NPC_CHROMATIC_PROTO:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            summon->AI()->AttackStart(target);
                        break;
                    default:
                        if (summon->IsAIEnabled)
                            summon->AI()->DoZoneInCombat();
                        break;
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_NEFARIAN_START_INTRO:
                        DoCast(me, SPELL_INTRO_1_TRANSFORM_VISUAL);
                        me->RemoveAurasDueToSpell(SPELL_INTRO_2_STALKER_TRANSFORM);
                        DoCast(me, SPELL_INTRO_5A_START_FIGHT_PROC);
                        EnterPhaseIntro();
                        break;
                    case ACTION_ONYXIA_AGGROED:
                        Talk(SAY_AGGRO);
                        DoZoneInCombat();
                        RemoveSummons();
                        DoCast(me, SPELL_CHILDREN_OF_DEATHWING_NEF);
                        DoCast(me, SPELL_HAIL_OF_BONES);
                        events.CancelEvent(EVENT_NEFARIAN_INTRO_NAG);
                        events.ScheduleEvent(EVENT_INITIAL_DESCEND, 30000);
                        events.ScheduleEvent(EVENT_BERSERK, 630000);
                        break;
                    case ACTION_ONYXIA_DEAD:
                        me->AttackStop();
                        me->InterruptNonMeleeSpells(true);
                        me->GetMotionMaster()->MoveIdle();
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        events.Reset();
                        events.SetPhase(PHASE_GROUND);
                        events.ScheduleEvent(EVENT_LIFTOFF_START, 1000, PHASE_GROUND);
                        events.ScheduleEvent(EVENT_FORCED_TIMED_LANDING, 180000);
                        secondPhase = true;
                        waypointFlightActive = false;
                        break;
                    case ACTION_CHROMATIC_PROTOTYPE_DEAD:
                        if (!finalPhase && (Is25ManRaid() || !me->FindNearestCreature(NPC_CHROMATIC_PROTO, 150.0f)))
                        {
                            events.Reset();
                            events.SetPhase(PHASE_FLIGHT);
                            events.ScheduleEvent(EVENT_LAND, 15000, PHASE_FLIGHT);
                            finalPhase = true;
                            if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                                elevator->SetGoState(GO_STATE_READY);

                            std::list<Creature*> summons;
                            GetCreatureListWithEntryInGrid(summons, me, NPC_CHROMATIC_PROTO, 150.0f);
                            for (std::list<Creature*>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                if (Creature* summon = *itr)
                                    if (summon->IsAlive())
                                        SetOnElevator(summon, false);
                        }
                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT_DESCEND:
                        if (finalPhase)
                        {
                            if (me->GetHealthPct() < 50)
                                me->GetMap()->SetWorldState(WORLDSTATE_KEEPING_IT_IN_THE_FAMILY, 1);

                            Talk(SAY_FINAL_PHASE);
                            EnterPhaseFinal();
                        }
                        else
                        {
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                            EnterPhaseGround();
                        }
                        // Visual landing
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                        me->SetFlying(false);
                        // Combat related
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        if (me->GetVictim())
                        {
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            waypointFlightActive = false;
                        }
                        else
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                AttackStart(target);
                        break;
                    case POINT_ASCEND:
                        EnterPhaseAir();
                        break;
                    case POINT_FLIGHT:
                        // Fly to a next waypoint
                        events.ScheduleEvent(EVENT_NEXT_FLIGHT_WAYPOINT, 1);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                UpdateVictim();

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_NEFARIAN_INTRO_1:
                            me->SetFlying(true);
                            events.ScheduleEvent(EVENT_NEXT_FLIGHT_WAYPOINT, 1);
                            waypointFlightActive = false;
                            Talk(SAY_ELECTRICITY_ANNOUNCE);
                            break;
                        case EVENT_NEFARIAN_INTRO_2:
                            if (Creature* trigger = me->FindNearestCreature(NPC_NEFARIANS_LIGHTNING_MACHINE, 250))
                                trigger->CastSpell(trigger, SPELL_ELECTROCUTE, true);
                            if (Creature* onyxia = me->FindNearestCreature(NPC_ONYXIA, 250))
                            {
                                SetOnElevator(onyxia, false);
                                // Cast chain visual
                                std::list<Creature*> stalkers;
                                GetCreatureListWithEntryInGrid(stalkers, onyxia, NPC_INVISIBLE_STALKER_FLOATING, 100);
                                for (std::list<Creature*>::const_iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                                {
                                    if (Creature* stalker = *itr)
                                    {
                                        SetOnElevator(stalker, false);
                                        for (uint32 i = 0; i < 3; ++i)
                                        {
                                            if (stalker->GetExactDist2d(&onyxiaChainStalkers[i]) < 5)
                                            {
                                                stalker->InterruptNonMeleeSpells(true);
                                                stalker->CastSpell(onyxia, onyxiaChainSpells[3 + i]);
                                            }
                                        }
                                    }
                                }
                                // Make attackable
                                onyxia->SetReactState(REACT_AGGRESSIVE);
                                onyxia->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                                // Remove feign death
                                onyxia->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                                onyxia->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                                onyxia->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                                onyxia->ClearUnitState(UNIT_STATE_DIED);
                            }
                            break;
                        case EVENT_NEFARIAN_INTRO_START_ANIM:
                            DoCast(me, SPELL_INTRO_4_LIFT_OFF_ANIM_KIT);
                            break;
                        case EVENT_NEFARIAN_INTRO_GROW:
                            me->RemoveAurasDueToSpell(SPELL_INTRO_3_SHRINK_AURA);
                            break;
                        case EVENT_NEFARIAN_INTRO_STOP_VISUAL:
                            me->RemoveAurasDueToSpell(SPELL_INTRO_1_TRANSFORM_VISUAL);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            break;
                        case EVENT_NEFARIAN_INTRO_NAG:
                            Talk(SAY_AVOID_WIPE);
                            break;
                        case EVENT_BERSERK:
                            me->AddAura(SPELL_BERSERK_NEF, me);
                            break;
                        case EVENT_NEXT_FLIGHT_WAYPOINT:
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(POINT_FLIGHT, nefarianFlightWaypoints[nextFlightWaypointIndex++]);
                            waypointFlightActive = true;
                            nextFlightWaypointIndex %= NEFARIAN_FLIGHT_WAYPOINT_COUNT;
                            break;
                        case EVENT_INITIAL_DESCEND:
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(POINT_DESCEND, -100.123f, -221.522f, 6.489f); // Move down.
                            waypointFlightActive = false;
                            break;
                        case EVENT_SHADOWFLAME_BREATH:
                            DoCastVictim(SPELL_SHADOWFLAME_BREATH);
                            events.ScheduleEvent(EVENT_SHADOWFLAME_BREATH, urand(12000, 20000));
                            break;
                        case EVENT_SHADOW_COWARDICE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_SHADOW_COWARDICE);
                            events.ScheduleEvent(EVENT_SHADOW_COWARDICE, urand(9000, 10000));
                            break;
                        case EVENT_LIFTOFF_START:
                        {
                            Talk(SAY_AIR_PHASE_2);
                            me->AttackStop();
                            me->GetMotionMaster()->Clear();
                            me->SetReactState(REACT_PASSIVE);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                            events.ScheduleEvent(EVENT_LIFTOFF_MOVE, 1000);
                            // "Kill" all alive Animated Bone Warriors just in case
                            std::list<Creature*> creatures;
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_ANIM_BONE_WARR, 250);
                            for (std::list<Creature*>::iterator itr = creatures.begin(); itr != creatures.end(); ++itr)
                                if (Creature* creature = *itr)
                                    creature->AI()->DoAction(ACTION_KILL_ANIMATED_WARRIOR);
                            // Lower the elevator
                            if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                                elevator->SetGoState(GO_STATE_ACTIVE);
                            break;
                        }
                        case EVENT_LIFTOFF_MOVE:
                            me->SetFlying(true);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(POINT_ASCEND, -115.776f, -224.625f, 36.358f);
                            waypointFlightActive = false;
                            break;
                        case EVENT_DOMINION:
                            DoCast(me, SPELL_DOMINION_TARGET);
                            DoCast(me, SPELL_DOMINION);
                            events.ScheduleEvent(EVENT_DOMINION, 15000);
                            break;
                        case EVENT_ELECTROCUTE:
                            if (Creature* trigger = me->FindNearestCreature(NPC_NEFARIANS_LIGHTNING_MACHINE, 250))
                                trigger->CastSpell(trigger, SPELL_ELECTROCUTE, true);
                            DoCast(me, SPELL_ELECTROCUTE_DMG, true);
                            break;
                        case EVENT_SHADOWFLAME_BARRAGE:
                            if (!me->HasUnitState(UNIT_STATE_CASTING))
                                DoCast(me, SPELL_SHADOWFLAME_BARRAGE);
                            events.ScheduleEvent(EVENT_SHADOWFLAME_BARRAGE, 2000);
                            break;
                        case EVENT_EXPLOSIVE_CINDERS:
                            me->InterruptNonMeleeSpells(true);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 250, true))
                                DoCast(target, SPELL_H_EXPLOSIVE_CINDERS_PERIODIC);
                            events.ScheduleEvent(EVENT_EXPLOSIVE_CINDERS, 22000);
                            break;
                        case EVENT_SUMMON_CHROMATIC:
                            for (uint32 i = 0; i < 3; i++)
                                if (Creature* chromatic = me->SummonCreature(NPC_CHROMATIC_PROTO, ChromaticPositions[i], TEMPSUMMON_CORPSE_DESPAWN))
                                    SetOnElevator(chromatic);
                            break;
                        case EVENT_LAND:
                            me->AttackStop();
                            me->InterruptNonMeleeSpells(true);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(POINT_DESCEND, -100.123f, -221.522f, 6.489f);
                            waypointFlightActive = false;
                            break;
                        case EVENT_FORCED_TIMED_LANDING:
                            if (phase == PHASE_FLIGHT && !finalPhase)
                            {
                                events.Reset();
                                events.SetPhase(PHASE_FLIGHT);
                                events.ScheduleEvent(EVENT_LAND, 20000, PHASE_FLIGHT);
                                finalPhase = true;
                                if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                                    elevator->SetGoState(GO_STATE_READY);

                                std::list<Creature*> summons;
                                GetCreatureListWithEntryInGrid(summons, me, NPC_CHROMATIC_PROTO, 150.0f);
                                for (std::list<Creature*>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
                                    if (Creature* summon = *itr)
                                        if (summon->IsAlive())
                                            SetOnElevator(summon, false);
                            }
                            break;
                        case EVENT_TAIL_LASH:
                            DoCast(me, SPELL_TAIL_LASH);
                            events.ScheduleEvent(EVENT_TAIL_LASH, urand(10000, 20000));
                            break;
                        case EVENT_SHADOWBLAZE:
                            Talk(SAY_SHADOWFLAME);
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_SHADOWBLAZE_SPARK);
                            events.ScheduleEvent(EVENT_SHADOWBLAZE, urand(18000, 21000));
                            break;
                        default:
                            break;
                    }
                }
                if (phase == PHASE_GROUND || phase == PHASE_FINAL)
                    DoMeleeAttackIfReady();
            }

        private:
            Phases phase;
            bool said, secondPhase, finalPhase;

            uint8 healthPct;
            std::list<ObjectGuid> SummonList;

            bool waypointFlightActive;
            uint32 nextFlightWaypointIndex;

            uint32 GetClosestWaypoint()
            {
                uint32 waypoint = 0;
                float minDist = 1000;
                for (uint32 i = 0; i < NEFARIAN_FLIGHT_WAYPOINT_COUNT; ++i)
                {
                    float dist = me->GetExactDist(&nefarianFlightWaypoints[i]);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        waypoint = i;
                    }
                }
                return waypoint;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_bd_nefarianAI>(creature);
        }
};

class boss_bd_onyxia : public CreatureScript
{
    public:
        boss_bd_onyxia() : CreatureScript("boss_bd_onyxia") { }

        struct boss_bd_onyxiaAI : public ScriptedAI
        {
            boss_bd_onyxiaAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            void Reset() override
            {
                events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetReactState(REACT_PASSIVE);
                me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
                warned = false;
                overloaded = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Creature* nefarian = me->FindNearestCreature(NPC_NEFARIAN, 250))
                    nefarian->AI()->DoAction(ACTION_ONYXIA_AGGROED);

                DoCast(me, SPELL_CHILDREN_OF_DEATHWING_ONY);
                DoCast(me, SPELL_ONYXIA_ELECTRICAL_CHARGE);
                DoCast(me, SPELL_ONYXIA_ELECTRICAL_CHARGE_INC);
                DoCast(me, SPELL_HITTING_YA, true);
                events.ScheduleEvent(EVENT_SHADOWFLAME_BREATH, urand(7000, 9000));
                events.ScheduleEvent(EVENT_LIGHTNING_DISCHARGE, urand(12000, 15000));
                events.ScheduleEvent(EVENT_TAIL_LASH, urand(4000, 6000));
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                me->DespawnOrUnsummon(1000);
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                if (Creature* nefarian = me->FindNearestCreature(NPC_NEFARIAN, 250))
                    nefarian->AI()->DoAction(ACTION_ONYXIA_DEAD);
                me->DespawnOrUnsummon(3000);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_ELECTROCUTE)
                    me->ModifyPower(POWER_ALTERNATE_POWER, 20);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (!warned && me->GetPower(POWER_ALTERNATE_POWER) >= 50)
                {
                    Talk(SAY_ELECTRICITY_WARNING);
                    warned = true;
                }
                if (!overloaded && me->GetPower(POWER_ALTERNATE_POWER) >= 100)
                {
                    DoCast(me, SPELL_ONYXIA_ELECTRICAL_CHARGE_DMG);
                    overloaded = true;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOWFLAME_BREATH:
                            DoCastVictim(SPELL_SHADOWFLAME_BREATH);
                            events.ScheduleEvent(EVENT_SHADOWFLAME_BREATH, urand(12000, 20000));
                            break;
                        case EVENT_LIGHTNING_DISCHARGE:
                            DoCast(me, SPELL_LIGHTNING_DISCHARGE);
                            events.ScheduleEvent(EVENT_LIGHTNING_DISCHARGE, urand(25000, 30000));
                            break;
                        case EVENT_TAIL_LASH:
                            DoCast(me, SPELL_TAIL_LASH);
                            events.ScheduleEvent(EVENT_TAIL_LASH, urand(10000, 20000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            bool warned, overloaded;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_bd_onyxiaAI>(creature);
        }
};

class npc_nefarian_intro : public CreatureScript
{
    public:
        npc_nefarian_intro() : CreatureScript("npc_nefarian_intro") { }

        struct npc_nefarian_introAI : public ScriptedAI
        {
            npc_nefarian_introAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            bool introDone;

            void Reset() override
            {
                if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                    elevator->SetGoState(GO_STATE_ACTIVE);

                events.Reset();
                events.ScheduleEvent(EVENT_LAVA_CHECK, 250);
                introDone = false;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (instance->GetBossState(DATA_NEFARIAN) == DONE)
                {
                    introDone = true;
                    events.ScheduleEvent(EVENT_HIDE_NEFARIUS, 1);
                }
            }

            void MoveInLineOfSight(Unit* /*who*/) override
            {
                // Intended to be empty
            }

            uint32 GetData(uint32 /*type*/) const override
            {
                if (introDone || !me->IsVisible())
                    return false;

                if (!instance->CheckRequiredBosses(DATA_NEFARIAN))
                    return false;

                if (instance->GetBossState(DATA_NEFARIAN) == DONE)
                    return false;

                return true;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_RESET:
                        if (Creature* onyxia = me->SummonCreature(NPC_ONYXIA, -107.213f, -224.62f, -7.413594f + 13.90172f, 3.122f))
                        {
                            onyxia->SetReactState(REACT_AGGRESSIVE);
                            onyxia->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        }
                        break;
                    case ACTION_START_ENCOUNTER:
                        if (GetData(0))
                        {
                            events.ScheduleEvent(EVENT_NEFARIUS_INTRO_1, 2000);
                            me->SetReactState(REACT_PASSIVE);
                            introDone = true;
                        }
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_NEFARIUS_INTRO_1:
                        {
                            Talk(SAY_INTRO_1);
                            if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                                elevator->SetGoState(GO_STATE_ACTIVE);
                            if (Creature* onyxia = me->SummonCreature(NPC_ONYXIA, -107.213f, -224.62f, -7.413594f, 3.122f))
                            {
                                // Put onyxia onto the elevator
                                SetOnElevator(onyxia);
                                // Feign death
                                onyxia->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                                onyxia->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                                onyxia->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                                onyxia->AddUnitState(UNIT_STATE_DIED);
                                // Put stalkers onto the elevator
                                std::list<Creature*> stalkers;
                                GetCreatureListWithEntryInGrid(stalkers, onyxia, NPC_INVISIBLE_STALKER, 50);
                                for (std::list<Creature*>::const_iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                                    if (Creature* stalker = *itr)
                                        stalker->NearTeleportTo(stalker->GetPositionX(), stalker->GetPositionY(), stalker->GetPositionZ() + 13.90172f, stalker->GetOrientation()); // Hack for proper spell targeting...
                                // Cast chain visual
                                stalkers.clear();
                                GetCreatureListWithEntryInGrid(stalkers, onyxia, NPC_INVISIBLE_STALKER_FLOATING, 50);
                                for (std::list<Creature*>::const_iterator itr = stalkers.begin(); itr != stalkers.end(); ++itr)
                                {
                                    if (Creature* stalker = *itr)
                                    {
                                        SetOnElevator(stalker);
                                        for (uint32 i = 0; i < 3; ++i)
                                            if (stalker->GetExactDist2d(&onyxiaChainStalkers[i]) < 5)
                                                stalker->CastSpell(onyxia, onyxiaChainSpells[i]);
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_NEFARIUS_INTRO_2, 20000);
                            break;
                        }
                        case EVENT_NEFARIUS_INTRO_2:
                            if (GameObject* elevator = instance->instance->GetGameObject(instance->GetGuidData(DATA_NEFARIAN_FLOOR)))
                                elevator->SetGoState(GO_STATE_READY);
                            Talk(SAY_INTRO_2);
                            events.ScheduleEvent(EVENT_NEFARIUS_INTRO_3, 10000);
                            break;
                        case EVENT_NEFARIUS_INTRO_3:
                            Talk(SAY_INTRO_3);
                            me->SummonCreature(NPC_NEFARIAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                            events.ScheduleEvent(EVENT_TRANSFORM, 7500);
                            events.ScheduleEvent(EVENT_HIDE_NEFARIUS, 10000);
                            break;
                        case EVENT_TRANSFORM:
                            if (Creature* nefarian = me->FindNearestCreature(NPC_NEFARIAN, 100))
                                nefarian->AI()->DoAction(ACTION_NEFARIAN_START_INTRO);
                            break;
                        case EVENT_HIDE_NEFARIUS:
                            me->SetVisible(false);
                            if (Creature* stalker = me->FindNearestCreature(NPC_INVISIBLE_STALKER_FLOATING, 10))
                                stalker->RemoveOwnedAura(SPELL_AURA_OF_DREAD);
                            break;
                        case EVENT_LAVA_CHECK:
                        {
                            // Hackish spellcast for being in lava - Player::UpdateUnderwaterState doesn't work due to issues with collision detection
                            Map::PlayerList const& players = instance->instance->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                            {
                                if (Player* player = itr->GetSource())
                                {
                                    if (player->IsAlive())
                                    {
                                        if (player->GetPositionZ() <= 1.2f)
                                        {
                                            if (!player->HasAura(SPELL_MAGMA) && !player->HasAura(31224) && !player->HasAura(39666) && !player->HasAura(65961)) // Exception for Cloak of Shadows
                                                player->CastSpell(player, SPELL_MAGMA, true);
                                        }
                                        else
                                            if (player->HasAura(SPELL_MAGMA))
                                                player->RemoveAurasDueToSpell(SPELL_MAGMA);
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_LAVA_CHECK, 250);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                damage = 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_nefarian_introAI>(creature);
        }
};

class npc_animated_bone_warrior : public CreatureScript
{
    public:
        npc_animated_bone_warrior() : CreatureScript("npc_animated_bone_warrior") { }

        struct npc_animated_bone_warriorAI : public ScriptedAI
        {
            npc_animated_bone_warriorAI(Creature* creature) : ScriptedAI(creature) 
            {
                ASSERT(creature->GetVehicleKit());
                creature->SetPower(POWER_ENERGY, 100);
                creature->SetMaxPower(POWER_ENERGY, 100);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_HURL_BONE, urand(4000, 9000));
                me->AddAura(SPELL_NO_REGEN, me);
                me->AddAura(SPELL_NO_REGEN2, me);
                DoCast(me, SPELL_ANIMATE_BONES);
                DoCast(me, SPELL_EMPOWER);
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_SHADOWBLAZE || spell->Id == SPELL_SHADOWFLAME_BREATH)
                {
                    SetOnElevator(me, false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveAurasDueToSpell(SPELL_DIE_VISUAL);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                    me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                    me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    DoCast(me, spell->Effects[spell->Id == SPELL_SHADOWBLAZE ? EFFECT_1 : EFFECT_2].BasePoints, true);
                    events.ScheduleEvent(EVENT_HURL_BONE, urand(4000, 9000));
                }
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_KILL_ANIMATED_WARRIOR && me->HasAura(SPELL_ANIMATE_BONES))
                {
                    events.CancelEvent(EVENT_HURL_BONE);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->InterruptNonMeleeSpells(true);
                    me->RemoveAura(SPELL_ANIMATE_BONES);
                    DoCast(me, SPELL_DIE_VISUAL, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                    me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
                    me->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    me->SetPower(POWER_ENERGY, 0);
                    SetOnElevator(me);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->GetVictim() && !me->HasAura(SPELL_ANIMATE_BONES))
                {
                    me->AttackStop();
                    return;
                }

                if (!UpdateVictim())
                    return;

                if (me->GetPower(POWER_ENERGY) < (IsHeroic() ? 2 : 3) && me->HasAura(SPELL_ANIMATE_BONES))
                    DoAction(ACTION_KILL_ANIMATED_WARRIOR);

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HURL_BONE:
                            DoCastVictim(SPELL_HURL_BONE);
                            events.ScheduleEvent(EVENT_HURL_BONE, urand(8000, 14000));
                            break;
                        default:
                            break;
                    }
                }

                if (me->HasAura(SPELL_ANIMATE_BONES))
                    DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon(3000);
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_animated_bone_warriorAI>(creature);
        }
};

class npc_chromatic_prototype : public CreatureScript
{
    public:
        npc_chromatic_prototype() : CreatureScript("npc_chromatic_prototype") { }

        struct npc_chromatic_prototypeAI : public ScriptedAI
        {
            npc_chromatic_prototypeAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                isCasting = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                isCasting = false;
                me->SetReactState(REACT_DEFENSIVE);
                events.ScheduleEvent(EVENT_BLAST_NOVA, 2000);
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
            }

            void Reset() override
            {
                events.Reset();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (isCasting && !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    isCasting = false;
                    events.ScheduleEvent(EVENT_BLAST_NOVA, Is25ManRaid() ? 5000 : 10000);
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLAST_NOVA:
                            isCasting = true;
                            if (!me->HasAura(SPELL_BLAST_NOVA))
                                DoCast(me, SPELL_BLAST_NOVA);
                            break;
                        default:
                            break;
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* nefarian = me->FindNearestCreature(NPC_NEFARIAN, 250))
                    nefarian->AI()->DoAction(ACTION_CHROMATIC_PROTOTYPE_DEAD);
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->DespawnOrUnsummon(5000);
            }

        private:
            EventMap events;
            bool isCasting;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chromatic_prototypeAI>(creature);
        }
};

class npc_shadowblaze_flashpoint : public CreatureScript
{
    public:
        npc_shadowblaze_flashpoint() : CreatureScript("npc_shadowblaze_flashpoint") { }

        struct npc_shadowblaze_flashpointAI : public ScriptedAI
        {
            npc_shadowblaze_flashpointAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                angle = summoner->GetAngle(me);
                distance = 0;
                events.Reset();
                events.ScheduleEvent(EVENT_SHADOWBLAZE_SPAWN, 1);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOWBLAZE_SPAWN:
                        {
                            float x, y, z;
                            me->GetPosition(x, y, z);
                            x += cosf(angle) * distance;
                            y += sinf(angle) * distance;
                            z = 8.489f;
                            me->CastSpell(x, y, z, SPELL_BRUSHFIRE_SUMMON, false);
                            distance += 7.5f;
                            if (distance > 50)
                                me->DespawnOrUnsummon();
                            else
                                events.ScheduleEvent(EVENT_SHADOWBLAZE_SPAWN, 1000);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap events;
            float angle, distance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_shadowblaze_flashpointAI>(creature);
        }
};

class npc_shadowblaze : public CreatureScript
{
    public:
        npc_shadowblaze() : CreatureScript("npc_shadowblaze") { }

        struct npc_shadowblazeAI : public ScriptedAI
        {
            npc_shadowblazeAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(me, SPELL_FIREBRUSH_AURA);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->HasAura(SPELL_FIREBRUSH_AURA))
                    me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_shadowblazeAI>(creature);
        }
};

class npc_dominion_stalker : public CreatureScript
{
    public:
        npc_dominion_stalker() : CreatureScript("npc_dominion_stalker") { }

        struct npc_dominion_stalkerAI : public ScriptedAI
        {
            npc_dominion_stalkerAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                DoCast(me, SPELL_DOMINION_PORTAL_TRIGGER);
                DoCast(me, SPELL_DOMINION_PORTAL_BEAM);
                summoner->SetWalk(true);
                summoner->GetMotionMaster()->Clear();
                Position pos = me->GetPosition();
                summoner->GetMotionMaster()->MovePoint(1, pos, false);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (TempSummon* summon = me->ToTempSummon())
                {
                    if (Unit* summoner = summon->GetSummoner())
                    {
                        if (!summoner->HasAura(SPELL_DOMINION))
                            me->DespawnOrUnsummon();
                        else if (me->GetExactDist2d(summoner) < 1)
                            summoner->Kill(summoner);
                    }
                    else
                        me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dominion_stalkerAI>(creature);
        }
};

class go_orb_of_culmination : public GameObjectScript
{
    public:
        go_orb_of_culmination() : GameObjectScript("go_orb_of_culmination") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (Creature* nefarius = go->FindNearestCreature(NPC_NEFARIAN_INTRO, 250))
                if (nefarius->AI()->GetData(0))
                    player->ADD_GOSSIP_ITEM_DB(11492, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(16139, go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                if (Creature* nefarius = go->FindNearestCreature(NPC_NEFARIAN_INTRO, 250))
                {
                    if (nefarius->AI()->GetData(0))
                    {
                        nefarius->AI()->DoAction(ACTION_START_ENCOUNTER);
                        if (Creature* stalker = go->FindNearestCreature(NPC_ORB_OF_CULMINATION_STALKER, 50))
                            stalker->SetVisible(false);
                        go->SetRespawnTime(WEEK);
                        player->CLOSE_GOSSIP_MENU();
                    }
                }
            }

            return true;
        }
};

class spell_onyxia_lightning_discharge_triggered : public SpellScriptLoader
{
    public:
        spell_onyxia_lightning_discharge_triggered() : SpellScriptLoader("spell_onyxia_lightning_discharge_triggered") { }

        class spell_onyxia_lightning_discharge_triggered_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_onyxia_lightning_discharge_triggered_SpellScript);

            void CastHandler()
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, SPELL_LIGHTNING_DISCHARGE_F_L);
                caster->CastSpell(caster, SPELL_LIGHTNING_DISCHARGE_B_L);
                caster->CastSpell(caster, SPELL_LIGHTNING_DISCHARGE_F_R);
                caster->CastSpell(caster, SPELL_LIGHTNING_DISCHARGE_B_R);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_onyxia_lightning_discharge_triggered_SpellScript::CastHandler);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_onyxia_lightning_discharge_triggered_SpellScript();
        }
};

class spell_onyxia_lightning_discharge : public SpellScriptLoader
{
    public:
        spell_onyxia_lightning_discharge() : SpellScriptLoader("spell_onyxia_lightning_discharge") { }

        class spell_onyxia_lightning_discharge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_onyxia_lightning_discharge_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                float angleFrom, angleTo;
                uint32 visSpell;
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_LIGHTNING_DISCHARGE_F_L: visSpell = SPELL_LIGHTNING_DISCHARGE_F_L_VIS; angleFrom = M_PI/6; angleTo = M_PI/2; break;
                    case SPELL_LIGHTNING_DISCHARGE_B_L: visSpell = SPELL_LIGHTNING_DISCHARGE_B_L_VIS; angleFrom = M_PI/2; angleTo = 5*M_PI/6; break;
                    case SPELL_LIGHTNING_DISCHARGE_F_R: visSpell = SPELL_LIGHTNING_DISCHARGE_F_R_VIS; angleFrom = 2*M_PI-M_PI/2; angleTo = 2*M_PI-M_PI/6; break;
                    case SPELL_LIGHTNING_DISCHARGE_B_R: visSpell = SPELL_LIGHTNING_DISCHARGE_B_R_VIS; angleFrom = 2*M_PI-5*M_PI/6; angleTo = 2*M_PI-M_PI/2; break;
                    default:
                        targets.clear();
                        return;
                }

                targets.remove_if(AngleCheck(GetCaster(), angleFrom, angleTo));

                if (targets.size() < 4)
                    for (uint32 i = 0; i < 4 - targets.size(); ++i)
                        GetCaster()->CastSpell(GetCaster(), visSpell, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_onyxia_lightning_discharge_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            class AngleCheck
            {
                public:
                    AngleCheck(Unit* caster, float from, float to) : _caster(caster), _from(from), _to(to) { }

                    bool operator()(WorldObject* unit) const
                    {
                        float angle = Position::NormalizeOrientation(_caster->GetRelativeAngle(unit));
                        return !(_from < angle && angle <= _to);
                    }

                private:
                    Unit* _caster;
                    float _from, _to;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_onyxia_lightning_discharge_SpellScript();
        }
};

class spell_onyxia_lightning_discharge_visual : public SpellScriptLoader
{
    public:
        spell_onyxia_lightning_discharge_visual() : SpellScriptLoader("spell_onyxia_lightning_discharge_visual") { }

        class spell_onyxia_lightning_discharge_visual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_onyxia_lightning_discharge_visual_SpellScript);

            void SetDest(SpellDestination& dest)
            {
                if (!GetCaster())
                    return;

                float angleFrom, angleTo;
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_LIGHTNING_DISCHARGE_F_L_VIS: angleFrom = M_PI/6; angleTo = M_PI/2; break;
                    case SPELL_LIGHTNING_DISCHARGE_B_L_VIS: angleFrom = M_PI/2; angleTo = 5*M_PI/6; break;
                    case SPELL_LIGHTNING_DISCHARGE_F_R_VIS: angleFrom = 2*M_PI-M_PI/2; angleTo = 2*M_PI-M_PI/6; break;
                    case SPELL_LIGHTNING_DISCHARGE_B_R_VIS: angleFrom = 2*M_PI-5*M_PI/6; angleTo = 2*M_PI-M_PI/2; break;
                    default: return;
                }

                float x, y, z;
                GetCaster()->GetPosition(x, y, z);

                // Elevator height correction
                z = 6.489f;

                // EFFECT_0 movement
                float angle = GetCaster()->GetOrientation() + angleFrom + (float)rand_norm() * (angleTo - angleFrom);
                float dist = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster(), GetSpell());
                x += cosf(angle) * dist;
                y += sinf(angle) * dist;

                // EFFECT_1 movement
                angle = GetCaster()->GetOrientation() + (float)rand_norm() * M_PI * 2;
                dist = (float)rand_norm() * GetSpellInfo()->Effects[EFFECT_1].CalcRadius(GetCaster(), GetSpell());
                x += cosf(angle) * dist;
                y += sinf(angle) * dist;

                Position newPos = { x, y, z, 0 };
                dest.Relocate(newPos);
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_onyxia_lightning_discharge_visual_SpellScript::SetDest, EFFECT_1, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_onyxia_lightning_discharge_visual_SpellScript();
        }
};

class spell_children_of_deathwing_aura : public SpellScriptLoader
{
    public:
        spell_children_of_deathwing_aura() : SpellScriptLoader("spell_children_of_deathwing_aura") { }

        class spell_children_of_deathwing_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_children_of_deathwing_aura_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (GetCaster())
                    GetCaster()->CastSpell((Unit*)NULL, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_children_of_deathwing_aura_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_children_of_deathwing_aura_AuraScript();
        }
};

class spell_children_of_deathwing : public SpellScriptLoader
{
    public:
        spell_children_of_deathwing() : SpellScriptLoader("spell_children_of_deathwing") { }

        class spell_children_of_deathwing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_children_of_deathwing_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(TargetCheck(GetCaster()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_children_of_deathwing_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }

            class TargetCheck
            {
                public:
                    TargetCheck(Unit* caster) : _caster(caster) { }

                    bool operator()(WorldObject* unit) const
                    {
                        return unit == _caster || (unit->GetEntry() != NPC_NEFARIAN && unit->GetEntry() != NPC_ONYXIA);
                    }

                private:
                    Unit* _caster;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_children_of_deathwing_SpellScript();
        }
};

class spell_electrical_charge : public SpellScriptLoader
{
    public:
        spell_electrical_charge() : SpellScriptLoader("spell_electrical_charge") { }

        class spell_electrical_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_electrical_charge_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (GetCaster())
                    GetCaster()->ModifyPower(POWER_ALTERNATE_POWER, 1);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_electrical_charge_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_electrical_charge_AuraScript();
        }
};

class spell_shadowflame_barrage : public SpellScriptLoader
{
    public:
        spell_shadowflame_barrage() : SpellScriptLoader("spell_shadowflame_barrage") { }

        class spell_shadowflame_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shadowflame_barrage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                std::list<WorldObject*> targetsW(targets.begin(), targets.end());
                std::list<WorldObject*> targetsE(targets.begin(), targets.end());
                std::list<WorldObject*> targetsS(targets.begin(), targets.end());
                targetsW.remove_if(DistanceCheck(ChromaticPositions[0], 10));
                targetsE.remove_if(DistanceCheck(ChromaticPositions[1], 10));
                targetsS.remove_if(DistanceCheck(ChromaticPositions[2], 10));
                Trinity::Containers::RandomResizeList(targetsW, 1);
                Trinity::Containers::RandomResizeList(targetsE, 1);
                Trinity::Containers::RandomResizeList(targetsS, 1);
                targets.clear();
                if (targetsW.size())
                    targets.push_back(*targetsW.begin());
                if (targetsE.size())
                    targets.push_back(*targetsE.begin());
                if (targetsS.size())
                    targets.push_back(*targetsS.begin());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shadowflame_barrage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            class DistanceCheck
            {
                public:
                    DistanceCheck(Position pos, float dist) : _pos(pos), _dist(dist) { }

                    bool operator()(WorldObject* unit) const
                    {
                        return unit->GetExactDist2d(&_pos) > _dist;
                    }

                private:
                    Position _pos;
                    float _dist;
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shadowflame_barrage_SpellScript();
        }
};

class spell_explosive_cinders : public SpellScriptLoader
{
    public:
        spell_explosive_cinders() : SpellScriptLoader("spell_explosive_cinders") { }

        class spell_explosive_cinders_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_explosive_cinders_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_explosive_cinders_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_explosive_cinders_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_explosive_cinders_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTargetApplication()->GetTarget())
                    target->CastSpell(target, SPELL_H_EXPLOSIVE_CINDERS_SUMM_DMG, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_explosive_cinders_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_explosive_cinders_SpellScript();
        }
        AuraScript* GetAuraScript() const override
        {
            return new spell_explosive_cinders_AuraScript();
        }
};

class spell_dominion_target : public SpellScriptLoader
{
    public:
        spell_dominion_target() : SpellScriptLoader("spell_dominion_target") { }

        class spell_dominion_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dominion_target_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (GetCaster())
                    Trinity::Containers::RandomResizeList(targets, GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, SPELL_DOMINION_IMMUNITY, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dominion_target_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_dominion_target_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dominion_target_SpellScript();
        }
};

class spell_dominion : public SpellScriptLoader
{
    public:
        spell_dominion() : SpellScriptLoader("spell_dominion") { }

        class spell_dominion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dominion_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(false, SPELL_DOMINION_IMMUNITY));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dominion_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dominion_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_dominion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dominion_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* owner = GetTargetApplication()->GetTarget();
                if (!owner)
                    return;

                float angles[4] = { 0, 3 * M_PI / 4, M_PI, M_PI / 2 };
                uint32 spells[4] = { SPELL_SUMMON_DOMINION_STALKER_N, SPELL_SUMMON_DOMINION_STALKER_E, SPELL_SUMMON_DOMINION_STALKER_S, SPELL_SUMMON_DOMINION_STALKER_W };

                float ox, oy, oz, oo;
                owner->GetPosition(ox, oy, oz, oo);

                float farthestDistance = 0;
                uint32 farthestIndex = -1;
                for (uint32 i = 0; i < 4; ++i)
                {
                    float angle = oo + angles[i];
                    float distance = sSpellMgr->GetSpellInfo(spells[i])->Effects[EFFECT_0].CalcRadius(GetCaster(), NULL);

                    float rx, ry, rz;
                    if (VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(owner->GetMapId(), ox, oy, oz+2, ox + cosf(angle) * distance, oy + sinf(angle) * distance, oz+2, rx, ry, rz, -0.5f))
                        distance = sqrtf((ox - rx) * (ox - rx) + (oy - ry) * (oy - ry));

                    if (distance > farthestDistance)
                    {
                        farthestDistance = distance;
                        farthestIndex = i;
                    }
                }

                if (farthestDistance > 0)
                    owner->CastSpell(owner, spells[farthestIndex], true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetTargetApplication()->GetTarget())
                {
                    owner->StopMoving();
                    owner->SetWalk(false);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_dominion_AuraScript::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_dominion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dominion_SpellScript();
        }
        AuraScript* GetAuraScript() const override
        {
            return new spell_dominion_AuraScript();
        }
};

class spell_summon_dominion_stalker : public SpellScriptLoader
{
    public:
        spell_summon_dominion_stalker(char const* name, Targets target) : SpellScriptLoader(name), _target(target) { }

        class spell_summon_dominion_stalker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_dominion_stalker_SpellScript);

            void SetDest(SpellDestination& dest)
            {
                if (!GetCaster())
                    return;

                float angle;
                float distance = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster(), GetSpell());
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_SUMMON_DOMINION_STALKER_N: angle = 0; break;
                    case SPELL_SUMMON_DOMINION_STALKER_E: angle = 3 * M_PI / 4; break;
                    case SPELL_SUMMON_DOMINION_STALKER_S: angle = M_PI; break;
                    case SPELL_SUMMON_DOMINION_STALKER_W: angle = M_PI / 2; break;
                    default: return;
                }

                float x, y, z, o;
                GetCaster()->GetPosition(x, y, z, o);

                angle += o;

                float rx, ry, rz;
                if (VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(GetCaster()->GetMapId(), x, y, z+2, x + cosf(angle) * distance, y + sinf(angle) * distance, z+2, rx, ry, rz, -0.5f))
                    distance = sqrtf((x - rx) * (x - rx) + (y - ry) * (y - ry));

                x += cosf(angle) * distance;
                y += sinf(angle) * distance;

                Position newPos = { x, y, z, o };
                dest.Relocate(newPos);
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_summon_dominion_stalker_SpellScript::SetDest, EFFECT_0, _target);
            }

            Targets _target;

        public:
            spell_summon_dominion_stalker_SpellScript(Targets target) : SpellScript(), _target(target) { }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_summon_dominion_stalker_SpellScript(_target);
        }

    private:
        Targets _target;
};

class spell_free_your_mind : public SpellScriptLoader
{
    public:
        spell_free_your_mind() : SpellScriptLoader("spell_free_your_mind") { }

        class spell_free_your_mind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_free_your_mind_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->RemoveAurasDueToSpell(GetSpellInfo()->Effects[EFFECT_0].BasePoints);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_free_your_mind_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_free_your_mind_AuraScript();
        }
};

class spell_siphon_power : public SpellScriptLoader
{
    public:
        spell_siphon_power() : SpellScriptLoader("spell_siphon_power") { }

        class spell_siphon_power_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_siphon_power_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    for (uint32 i = 0; i < 10; ++i)
                        target->CastSpell(target, SPELL_STOLEN_POWER, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_siphon_power_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_siphon_power_SpellScript();
        }
};

class spell_dragon_orb : public SpellScriptLoader
{
    public:
        spell_dragon_orb() : SpellScriptLoader("spell_dragon_orb") { }

        class spell_dragon_orb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dragon_orb_SpellScript);

            void SetDest(SpellDestination& dest)
            {
                if (!GetCaster())
                    return;

                float angle = (float)rand_norm() * M_PI * 2;
                float distance = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster(), GetSpell()) * 2;

                float x, y, z, o;
                GetCaster()->GetPosition(x, y, z, o);

                x += cosf(angle) * distance;
                y += sinf(angle) * distance;
                z += 6 + (float)rand_norm() * 2;

                Position newPos = { x, y, z, o };
                dest.Relocate(newPos);
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_dragon_orb_SpellScript::SetDest, EFFECT_0, TARGET_DEST_CASTER_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dragon_orb_SpellScript();
        }
};

void AddSC_boss_bwd_nefarian()
{
    new boss_bd_nefarian();
    new boss_bd_onyxia();
    new npc_nefarian_intro();
    new npc_animated_bone_warrior();
    new npc_chromatic_prototype();
    new npc_shadowblaze_flashpoint();
    new npc_shadowblaze();
    new npc_dominion_stalker();
    new go_orb_of_culmination();
    new spell_onyxia_lightning_discharge_triggered();
    new spell_onyxia_lightning_discharge();
    new spell_onyxia_lightning_discharge_visual();
    new spell_children_of_deathwing_aura();
    new spell_children_of_deathwing();
    new spell_electrical_charge();
    new spell_shadowflame_barrage();
    new spell_explosive_cinders();
    new spell_dominion_target();
    new spell_dominion();
    new spell_summon_dominion_stalker("spell_summon_dominion_stalker_n", TARGET_DEST_CASTER_FRONT);
    new spell_summon_dominion_stalker("spell_summon_dominion_stalker_e", TARGET_DEST_CASTER_RIGHT);
    new spell_summon_dominion_stalker("spell_summon_dominion_stalker_s", TARGET_DEST_CASTER_BACK);
    new spell_summon_dominion_stalker("spell_summon_dominion_stalker_w", TARGET_DEST_CASTER_LEFT);
    new spell_free_your_mind();
    new spell_siphon_power();
    new spell_dragon_orb();
}
