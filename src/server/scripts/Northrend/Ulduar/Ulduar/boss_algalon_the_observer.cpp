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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "PassiveAI.h"
#include "GameObjectAI.h"
#include "MapManager.h"
#include "Player.h"
#include "ulduar.h"

enum Texts
{
    SAY_BRANN_ALGALON_INTRO_1       = 0,
    SAY_BRANN_ALGALON_INTRO_2       = 1,
    SAY_BRANN_ALGALON_OUTRO         = 2,

    SAY_ALGALON_INTRO_1             = 0,
    SAY_ALGALON_INTRO_2             = 1,
    SAY_ALGALON_INTRO_3             = 2,
    SAY_ALGALON_START_TIMER         = 3,
    SAY_ALGALON_AGGRO               = 4,
    SAY_ALGALON_COLLAPSING_STAR     = 5,
    EMOTE_ALGALON_COLLAPSING_STAR   = 6,
    SAY_ALGALON_BIG_BANG            = 7,
    EMOTE_ALGALON_BIG_BANG          = 8,
    SAY_ALGALON_ASCEND              = 9,
    EMOTE_ALGALON_COSMIC_SMASH      = 10,
    SAY_ALGALON_PHASE_TWO           = 11,
    SAY_ALGALON_OUTRO_1             = 12,
    SAY_ALGALON_OUTRO_2             = 13,
    SAY_ALGALON_OUTRO_3             = 14,
    SAY_ALGALON_OUTRO_4             = 15,
    SAY_ALGALON_OUTRO_5             = 16,
    SAY_ALGALON_DESPAWN_1           = 17,
    SAY_ALGALON_DESPAWN_2           = 18,
    SAY_ALGALON_DESPAWN_3           = 19,
    SAY_ALGALON_KILL                = 20,
};

enum Spells
{
    // Algalon the Observer
    SPELL_ARRIVAL                       = 64997,
    SPELL_RIDE_THE_LIGHTNING            = 64986,
    SPELL_SUMMON_AZEROTH                = 64994,
    SPELL_REORIGINATION                 = 64996,
    SPELL_SUPERMASSIVE_FAIL             = 65311,
    SPELL_QUANTUM_STRIKE                = 64395,
    SPELL_PHASE_PUNCH                   = 64412,
    SPELL_BIG_BANG                      = 64443,
    SPELL_ASCEND_TO_THE_HEAVENS         = 64487,
    SPELL_COSMIC_SMASH                  = 62301,
    SPELL_COSMIC_SMASH_TRIGGERED        = 62304,
    SPELL_COSMIC_SMASH_VISUAL_STATE     = 62300,
    SPELL_SELF_STUN                     = 65256,
    SPELL_KILL_CREDIT                   = 65184,
    SPELL_TELEPORT                      = 62940,
    SPELL_BERSERK                       = 47008,

    // Algalon Stalker
    SPELL_TRIGGER_3_ADDS                = 62266,    // Triggers Living Constellation

    // Living Constellation
    SPELL_ARCANE_BARRAGE                = 64599,

    // Collapsing Star
    SPELL_COLLAPSE                      = 62018,
    SPELL_BLACK_HOLE_SPAWN_VISUAL       = 62003,
    SPELL_SUMMON_BLACK_HOLE             = 62189,

    // Black Hole
    SPELL_BLACK_HOLE_TRIGGER            = 62185,
    SPELL_BLACK_HOLE_PHASE              = 62168,
    SPELL_BLACK_HOLE_DAMAGE             = 62169,
    SPELL_CONSTELLATION_PHASE_TRIGGER   = 65508,
    SPELL_CONSTELLATION_PHASE_EFFECT    = 65509,
    SPELL_BLACK_HOLE_EXPLOSION          = 64122,
    SPELL_SUMMON_VOID_ZONE_VISUAL       = 64470,
    SPELL_VOID_ZONE_VISUAL              = 64469,
    SPELL_BLACK_HOLE_CREDIT             = 65312,

    // Worm Hole
    SPELL_WORM_HOLE_TRIGGER             = 65251,
    SPELL_WORM_HOLE_PHASE               = 65250,
    SPELL_SUMMON_UNLEASHED_DARK_MATTER  = 64450,
};

uint32 const PhasePunchAlphaId[5] = {64435, 64434, 64428, 64421, 64417};

enum Events
{
    // Celestial Planetarium Access
    EVENT_DESPAWN_CONSOLE           = 1,

    // brann Bronzebeard
    EVENT_BRANN_MOVE_INTRO          = 2,
    EVENT_SUMMON_ALGALON            = 3,
    EVENT_BRANN_INTRO_FINISH        = 41,
    EVENT_BRANN_OUTRO_1             = 4,
    EVENT_BRANN_OUTRO_2             = 5,

    // Algalon the Observer
    EVENT_INTRO_1                   = 6,
    EVENT_INTRO_2                   = 7,
    EVENT_INTRO_3                   = 8,
    EVENT_INTRO_FINISH              = 9,
    EVENT_START_COMBAT              = 10,
    EVENT_INTRO_TIMER_DONE          = 11,
    EVENT_QUANTUM_STRIKE            = 12,
    EVENT_PHASE_PUNCH               = 13,
    EVENT_SUMMON_COLLAPSING_STAR    = 14,
    EVENT_BIG_BANG                  = 15,
    EVENT_RESUME_UPDATING           = 16,
    EVENT_ASCEND_TO_THE_HEAVENS     = 17,
    EVENT_EVADE                     = 18,
    EVENT_COSMIC_SMASH              = 19,
    EVENT_UNLOCK_YELL               = 20,
    EVENT_OUTRO_START               = 21,
    EVENT_OUTRO_1                   = 22,
    EVENT_OUTRO_2                   = 23,
    EVENT_OUTRO_3                   = 24,
    EVENT_OUTRO_4                   = 25,
    EVENT_OUTRO_5                   = 26,
    EVENT_OUTRO_6                   = 27,
    EVENT_OUTRO_7                   = 28,
    EVENT_OUTRO_8                   = 29,
    EVENT_OUTRO_9                   = 30,
    EVENT_OUTRO_10                  = 31,
    EVENT_OUTRO_11                  = 32,
    EVENT_OUTRO_12                  = 33,
    EVENT_OUTRO_13                  = 34,
    EVENT_OUTRO_14                  = 35,
    EVENT_DESPAWN_ALGALON_1         = 36,
    EVENT_DESPAWN_ALGALON_2         = 37,
    EVENT_DESPAWN_ALGALON_3         = 38,
    EVENT_DESPAWN_ALGALON_4         = 40,
    EVENT_BERSERK                   = 41,

    // Living Constellation
    EVENT_ARCANE_BARRAGE            = 39,
};

enum Actions
{
    ACTION_START_INTRO      = 0,
    ACTION_FINISH_INTRO     = 1,
    ACTION_ACTIVATE_STAR    = 2,
    ACTION_BIG_BANG         = 3,
    ACTION_ASCEND           = 4,
    ACTION_OUTRO            = 5,
};

enum Points
{
    POINT_BRANN_INTRO           = 0,
    MAX_BRANN_WAYPOINTS_INTRO   = 10,
    POINT_BRANN_OUTRO           = 10,
    POINT_BRANN_OUTRO_END       = 11,

    POINT_ALGALON_LAND          = 1,
    POINT_ALGALON_OUTRO         = 2,
};

enum EncounterPhases
{
    PHASE_NORMAL             = 0,
    PHASE_ROLE_PLAY          = 1,
    PHASE_BIG_BANG           = 2,

    PHASE_MASK_NO_UPDATE     = (1 << PHASE_ROLE_PLAY) | (1 << PHASE_BIG_BANG),
    PHASE_MASK_NO_CAST_CHECK = 1 << PHASE_ROLE_PLAY,
};

enum AchievmentInfo
{
    EVENT_ID_SUPERMASSIVE_START = 21697,
    DATA_HAS_FED_ON_TEARS       = 30043005,
};


Position const BrannIntroSpawnPos = { 1676.277f, -162.5308f, 427.3326f, 3.235537f };
Position const BrannIntroWaypoint[MAX_BRANN_WAYPOINTS_INTRO] =
{
    { 1642.482f, -164.0812f, 427.2602f, 0.0f },
    { 1635.000f, -169.5145f, 427.2523f, 0.0f },
    { 1632.814f, -173.9334f, 427.2621f, 0.0f },
    { 1632.676f, -190.5927f, 425.8831f, 0.0f },
    { 1631.497f, -214.2221f, 418.1152f, 0.0f },
    { 1624.717f, -224.6876f, 418.1152f, 0.0f },
    { 1631.497f, -214.2221f, 418.1152f, 0.0f },
    { 1632.676f, -190.5927f, 425.8831f, 0.0f },
    { 1632.814f, -173.9334f, 427.2621f, 0.0f },
    { 1635.000f, -169.5145f, 427.2523f, 0.0f },
};
Position const AlgalonSummonPos = { 1632.531f, -304.8516f, 450.1123f, 1.530165f };
Position const AlgalonLandPos   = { 1632.668f, -302.7656f, 417.3211f, 1.530165f };

#define LIVING_CONSTELLATION_COUNT 11
Position const ConstellationPos[LIVING_CONSTELLATION_COUNT] =
{
    { 1625.208f, -267.2771f, 446.4296f, 5.044002f },
    { 1658.279f, -262.5490f, 441.9073f, 4.188790f },
    { 1678.677f, -276.3280f, 427.7531f, 3.979351f },
    { 1593.389f, -299.4325f, 432.4636f, 6.073746f },
    { 1685.613f, -300.1219f, 443.2366f, 3.385939f },
    { 1591.706f, -263.8201f, 441.4153f, 5.253441f },
    { 1668.317f, -324.7676f, 457.9394f, 3.211406f },
    { 1592.242f, -325.5323f, 446.9508f, 0.226893f },
    { 1635.821f, -363.3442f, 424.3459f, 1.466077f },
    { 1672.188f, -357.2484f, 436.7337f, 2.338741f },
    { 1615.800f, -348.0065f, 442.9586f, 1.134464f },
};

#define COLLAPSING_STAR_COUNT 4
Position const CollapsingStarPos[COLLAPSING_STAR_COUNT] =
{
    { 1649.438f, -319.8127f, 418.3941f, 1.082104f },
    { 1647.005f, -288.6790f, 417.3955f, 3.490659f },
    { 1622.451f, -321.1563f, 417.6188f, 4.677482f },
    { 1615.060f, -291.6816f, 417.7796f, 3.490659f },
};
Position const AlgalonOutroPos = { 1633.64f, -317.78f, 417.3211f, 0.0f };
Position const BrannOutroPos[3] =
{
    { 1632.023f, -243.7434f, 417.9118f, 0.0f },
    { 1631.986f, -297.7831f, 417.3210f, 0.0f },
    { 1633.832f, -216.2948f, 417.0463f, 0.0f },
};
Position const DarkMatterPos[8] =
{
    { 1647.01f, -288.679f, 417.396f, 3.49066f },
    { 1615.06f, -291.682f, 417.780f, 3.49066f },
    { 1615.06f, -291.682f, 417.780f, 3.49066f },
    { 1649.44f, -319.813f, 418.394f, 1.08210f },
    { 1622.45f, -321.156f, 417.619f, 4.67748f },
    { 1649.44f, -319.813f, 418.394f, 1.08210f },
    { 1622.45f, -321.156f, 417.619f, 4.67748f },
    { 1647.01f, -288.679f, 417.396f, 3.49066f }
};

class ActivateLivingConstellation : public BasicEvent
{
    public:
        ActivateLivingConstellation(Unit* owner) : _owner(owner), instance(owner->GetInstanceScript()) { }

        bool Execute(uint64 execTime, uint32 /*diff*/) override
        {
            if (!instance || instance->GetBossState(BOSS_ALGALON) != IN_PROGRESS)
                return true;    // delete event

            _owner->CastSpell((Unit*)nullptr, SPELL_TRIGGER_3_ADDS, TRIGGERED_FULL_MASK);
            _owner->m_Events.AddEvent(this, execTime + urand(45000, 50000));
            return false;
        }

    private:
        Unit* _owner;
        InstanceScript* instance;
};

class CosmicSmashDamageEvent : public BasicEvent
{
    public:
        CosmicSmashDamageEvent(Unit* caster) : _caster(caster) { }

        bool Execute(uint64 /*execTime*/, uint32 /*diff*/) override
        {
            _caster->CastSpell((Unit*)nullptr, SPELL_COSMIC_SMASH_TRIGGERED, TRIGGERED_FULL_MASK);
            return true;
        }

    private:
        Unit* _caster;
};

class SummonUnleashedDarkMatter : public BasicEvent
{
    public:
        SummonUnleashedDarkMatter(Unit* caster) : _caster(caster) { }

        bool Execute(uint64 execTime, uint32 /*diff*/) override
        {
            _caster->CastSpell((Unit*)nullptr, SPELL_SUMMON_UNLEASHED_DARK_MATTER, TRIGGERED_FULL_MASK);
            _caster->m_Events.AddEvent(this, execTime + 30000);
            return false;
        }

    private:
        Unit* _caster;
};

class boss_algalon_the_observer : public CreatureScript
{
    public:
        boss_algalon_the_observer() : CreatureScript("boss_algalon_the_observer") { }

        struct boss_algalon_the_observerAI : public BossAI
        {
            boss_algalon_the_observerAI(Creature* creature) : BossAI(creature, BOSS_ALGALON)
            {
                _firstPull = true;
                _fedOnTears = false;
            }

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_PASSIVE);
                me->SetCanDualWield(true);
                me->SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, me->GetCreatureTemplate()->mindmg);
                me->SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, me->GetCreatureTemplate()->maxdmg);
                me->UpdateDamagePhysical(OFF_ATTACK);
                _phaseTwo = false;
                _fightWon = false;
                _hasYelled = false;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                {
                    _fedOnTears = true;
                    if (!_hasYelled && !_fightWon)
                    {
                        _hasYelled = true;
                        events.ScheduleEvent(EVENT_UNLOCK_YELL, 1000);
                        Talk(SAY_ALGALON_KILL);
                    }
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START_INTRO:
                    {
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_INSTANTLY_APPEAR_MODEL);
                        me->SetFlying(true);
                        DoCast(me, SPELL_ARRIVAL, true);
                        DoCast(me, SPELL_RIDE_THE_LIGHTNING, true);
                        me->GetMotionMaster()->MoveLand(POINT_ALGALON_LAND, AlgalonLandPos);
                        me->SetHomePosition(AlgalonLandPos);
                        events.Reset();
                        events.SetPhase(PHASE_ROLE_PLAY);
                        events.ScheduleEvent(EVENT_INTRO_1, 5000, 0, PHASE_ROLE_PLAY);
                        events.ScheduleEvent(EVENT_INTRO_2, 15000, 0, PHASE_ROLE_PLAY);
                        events.ScheduleEvent(EVENT_INTRO_3, 23000, 0, PHASE_ROLE_PLAY);
                        events.ScheduleEvent(EVENT_INTRO_FINISH, 43000, 0, PHASE_ROLE_PLAY);
                        break;
                    }
                    case ACTION_ASCEND:
                        events.SetPhase(PHASE_BIG_BANG);
                        events.CancelEvent(EVENT_RESUME_UPDATING);
                        events.RescheduleEvent(EVENT_ASCEND_TO_THE_HEAVENS, 1500);
                        break;
                    case EVENT_DESPAWN_ALGALON:
                    {
                        _fightWon = true; // To prevent receiving damage
                        instance->SetBossState(BOSS_ALGALON, FAIL);
                        me->SetReactState(REACT_PASSIVE);
                        me->InterruptNonMeleeSpells(false);
                        me->AttackStop();
                        me->StopMoving();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        events.Reset();
                        events.SetPhase(PHASE_ROLE_PLAY);
                        events.ScheduleEvent(EVENT_DESPAWN_ALGALON_1, 5000);
                        events.ScheduleEvent(EVENT_DESPAWN_ALGALON_2, 17000);
                        events.ScheduleEvent(EVENT_DESPAWN_ALGALON_3, 26000);
                        events.ScheduleEvent(EVENT_DESPAWN_ALGALON_4, 31000);
                        me->DespawnOrUnsummon(34000);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC);

                        summons.DespawnAll();

                        std::list<Creature*> darkmatters;
                        me->GetCreatureListWithEntryInGrid(darkmatters, 33089, 50.0f);
                        for (auto&& darkmatter : darkmatters)
                            if (darkmatter)
                                darkmatter->DespawnOrUnsummon();
                        break;
                    }
                    case ACTION_INIT_ALGALON:
                        _firstPull = false;
                        // no break;
                    case ACTION_INIT_ALGALON_FIRST:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                        DoCastAOE(SPELL_SUMMON_AZEROTH, true);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                return type == DATA_HAS_FED_ON_TEARS ? _fedOnTears : 1;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (_fightWon)
                    return;

                uint32 introDelay = 0;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC);
                events.Reset();
                events.SetPhase(PHASE_ROLE_PLAY);

                if (!_firstPull)
                {
                    Talk(SAY_ALGALON_AGGRO);
                    _JustEngagedWith();
                    introDelay = 8000;
                    summons.DespawnEntry(NPC_AZEROTH);
                }
                else
                {
                    _firstPull = false;
                    Talk(SAY_ALGALON_START_TIMER);

                    me->setActive(true);
                    DoZoneInCombat();
                    introDelay = 26000;
                    summons.DespawnEntry(NPC_AZEROTH);
                    instance->SetData(EVENT_DESPAWN_ALGALON, 0);
                    events.ScheduleEvent(EVENT_START_COMBAT, 18000);
                }

                events.ScheduleEvent(EVENT_INTRO_TIMER_DONE, introDelay);
                events.ScheduleEvent(EVENT_QUANTUM_STRIKE, 3500 + introDelay);
                events.ScheduleEvent(EVENT_PHASE_PUNCH, 15500 + introDelay);
                events.ScheduleEvent(EVENT_SUMMON_COLLAPSING_STAR, 18000 + introDelay);
                events.ScheduleEvent(EVENT_BIG_BANG, 90000 + introDelay);
                events.ScheduleEvent(EVENT_BERSERK, 360000 + introDelay);
                events.ScheduleEvent(EVENT_COSMIC_SMASH, 25000 + introDelay);

                std::list<Creature*> stalkers;
                me->GetCreatureListWithEntryInGrid(stalkers, NPC_ALGALON_STALKER, 200.0f);
                for (auto&& stalker : stalkers)
                    stalker->m_Events.KillAllEvents(true);
            }

            void MovementInform(uint32 movementType, uint32 point) override
            {
                if (movementType != POINT_MOTION_TYPE && movementType != EFFECT_MOTION_TYPE)
                    return;

                if (point == POINT_ALGALON_LAND)
                    me->SetFlying(false);
                else if (point == POINT_ALGALON_OUTRO)
                    me->SetFacingTo(1.605703f);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                switch (summon->GetEntry())
                {
                    case NPC_AZEROTH:
                        DoCastAOE(SPELL_REORIGINATION, true);
                        break;
                    case NPC_COLLAPSING_STAR:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->GetMotionMaster()->MoveRandom(20.0f);
                        summon->CastSpell(summon, SPELL_COLLAPSE, TRIGGERED_FULL_MASK);
                        break;
                    case NPC_BLACK_HOLE:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->CastSpell((Unit*)nullptr, SPELL_BLACK_HOLE_TRIGGER, TRIGGERED_FULL_MASK);
                        summon->CastSpell(summon, SPELL_CONSTELLATION_PHASE_TRIGGER, TRIGGERED_FULL_MASK);
                        summon->CastSpell((Unit*)nullptr, SPELL_BLACK_HOLE_EXPLOSION);
                        summon->CastSpell(summon, SPELL_SUMMON_VOID_ZONE_VISUAL, TRIGGERED_FULL_MASK);
                        break;
                    case NPC_ALGALON_VOID_ZONE_VISUAL_STALKER:
                        summon->CastSpell(summon, SPELL_VOID_ZONE_VISUAL, TRIGGERED_FULL_MASK);
                        break;
                    case NPC_ALGALON_STALKER_ASTEROID_TARGET_01:
                        summon->CastSpell(summon, SPELL_COSMIC_SMASH_VISUAL_STATE, TRIGGERED_FULL_MASK);
                        break;
                    case NPC_ALGALON_STALKER_ASTEROID_TARGET_02:
                        summon->m_Events.AddEvent(new CosmicSmashDamageEvent(summon), summon->m_Events.CalculateTime(3250));
                        break;
                    case NPC_WORM_HOLE:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->CastSpell(summon, SPELL_WORM_HOLE_TRIGGER, TRIGGERED_FULL_MASK);
                        summon->CastSpell(summon, SPELL_SUMMON_VOID_ZONE_VISUAL, TRIGGERED_FULL_MASK);
                        break;
                    case NPC_UNLEASHED_DARK_MATTER:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            if (summon->Attack(target, true))
                                summon->GetMotionMaster()->MoveChase(target);
                        break;
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                instance->SetBossState(BOSS_ALGALON, FAIL);
                BossAI::EnterEvadeMode();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                me->SetSheath(SHEATH_STATE_UNARMED);
                std::list<Creature*> darkmatters;
                me->GetCreatureListWithEntryInGrid(darkmatters, 33089, 50.0f);
                for (auto&& darkmatter : darkmatters)
                    darkmatter->DespawnOrUnsummon();
            }

            void JustReachedHome() override
            {
                BossAI::JustReachedHome();
                DoCastAOE(SPELL_SUMMON_AZEROTH, true);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (_fightWon)
                {
                    damage = 0;
                    return;
                }

                if (!_phaseTwo && me->HealthBelowPctDamaged(20, damage))
                {
                    _phaseTwo = true;
                    Talk(SAY_ALGALON_PHASE_TWO);
                    summons.DespawnEntry(NPC_LIVING_CONSTELLATION);
                    summons.DespawnEntry(NPC_COLLAPSING_STAR);
                    summons.DespawnEntry(NPC_BLACK_HOLE);
                    summons.DespawnEntry(NPC_ALGALON_VOID_ZONE_VISUAL_STALKER);
                    events.CancelEvent(EVENT_SUMMON_COLLAPSING_STAR);
                    std::list<Creature*> stalkers;
                    me->GetCreatureListWithEntryInGrid(stalkers, NPC_ALGALON_STALKER, 200.0f);
                    for (auto&& stalker : stalkers)
                        stalker->m_Events.KillAllEvents(true);
                    for (uint32 i = 0; i < COLLAPSING_STAR_COUNT; ++i)
                        if (Creature* wormHole = DoSummon(NPC_WORM_HOLE, CollapsingStarPos[i], 30s, TEMPSUMMON_MANUAL_DESPAWN))
                            wormHole->m_Events.AddEvent(new SummonUnleashedDarkMatter(wormHole), wormHole->m_Events.CalculateTime(i >= 2 ? 8000 : 6000));
                }
                else if ((int32(me->GetHealth()) - int32(damage)) < CalculatePct(float(me->GetMaxHealth()), 2.5f) && !_fightWon)
                {
                    _fightWon = true;
                    damage = 0;
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->SetFaction(35);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    DoCast(me, SPELL_SELF_STUN);
                    events.Reset();
                    summons.DespawnAll();
                    events.SetPhase(PHASE_ROLE_PLAY);
                    events.ScheduleEvent(EVENT_OUTRO_START, 1500);
                    events.ScheduleEvent(EVENT_OUTRO_1, 7200);
                    events.ScheduleEvent(EVENT_OUTRO_2, 8700);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                // This monstrosity of a code makes Algalon cast Ascend to the Heavens instead of evading if all players enter Black Holes
                if (!_fightWon && me->IsInCombat() && !events.IsInPhase(PHASE_BIG_BANG) && me->GetThreatManager().getOnlineContainer().empty() && !me->GetThreatManager().getOfflineContainer().empty())
                {
                    for (auto&& ref : me->GetThreatManager().getOfflineContainer().getThreatList())
                    {
                        if (ref->getUnitGuid().IsPlayer())
                        {
                            if (Unit* target = ref->getTarget())
                            {
                                if (target->FindMap() == me->GetMap() && !target->InSamePhase(me))
                                {
                                    me->InterruptNonMeleeSpells(false);
                                    events.Reset();
                                    events.SetPhase(PHASE_BIG_BANG);
                                    events.ScheduleEvent(EVENT_ASCEND_TO_THE_HEAVENS, 1);
                                    break;
                                }
                            }
                        }
                    }
                }

                if ((!(events.IsInPhase(PHASE_ROLE_PLAY) || events.IsInPhase(PHASE_BIG_BANG)) && !UpdateVictim()) || !CheckInRoom())
                    return;

                events.Update(diff);

                if (!events.IsInPhase(PHASE_ROLE_PLAY))
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_1:
                            me->RemoveAurasDueToSpell(SPELL_RIDE_THE_LIGHTNING);
                            Talk(SAY_ALGALON_INTRO_1);
                            break;
                        case EVENT_INTRO_2:
                            DoCastAOE(SPELL_SUMMON_AZEROTH, true);
                            Talk(SAY_ALGALON_INTRO_2);
                            break;
                        case EVENT_INTRO_3:
                            Talk(SAY_ALGALON_INTRO_3);
                            break;
                        case EVENT_INTRO_FINISH:
                            events.Reset();
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            break;
                        case EVENT_START_COMBAT:
                            instance->SetBossState(BOSS_ALGALON, IN_PROGRESS);
                            break;
                        case EVENT_INTRO_TIMER_DONE:
                        {
                            events.SetPhase(PHASE_NORMAL);
                            me->SetSheath(SHEATH_STATE_MELEE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC);
                            me->SetReactState(REACT_DEFENSIVE);
                            DoCastAOE(SPELL_SUPERMASSIVE_FAIL, true);
                            // Workaround for Creature::_IsTargetAcceptable returning false
                            // for creatures that start combat in REACT_PASSIVE and UNIT_FLAG_NOT_SELECTABLE
                            // causing them to immediately evade
                            if (!me->GetThreatManager().isThreatListEmpty())
                                AttackStart(me->GetThreatManager().getHostilTarget());
                            for (uint32 i = 0; i < LIVING_CONSTELLATION_COUNT; ++i)
                                if (Creature* summon = DoSummon(NPC_LIVING_CONSTELLATION, ConstellationPos[i], 0ms, TEMPSUMMON_DEAD_DESPAWN))
                                    summon->SetReactState(REACT_PASSIVE);
                            for (uint32 i = 0; i < 8; i++)
                                if (Creature* DarkMatter = me->SummonCreature(33089, DarkMatterPos[i], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    DarkMatter->SetPhaseMask(16, true);
                                    DarkMatter->GetMotionMaster()->MoveRandom(20.0f);
                                    DarkMatter->SetReactState(REACT_AGGRESSIVE);
                                }

                            std::list<Creature*> stalkers;
                            me->GetCreatureListWithEntryInGrid(stalkers, NPC_ALGALON_STALKER, 200.0f);
                            if (!stalkers.empty())
                            {
                                Unit* stalker = Trinity::Containers::SelectRandomContainerElement(stalkers);
                                stalker->m_Events.AddEvent(new ActivateLivingConstellation(stalker), stalker->m_Events.CalculateTime(urand(45000, 50000)));
                            }
                            break;
                        }
                        case EVENT_QUANTUM_STRIKE:
                            DoCastVictim(SPELL_QUANTUM_STRIKE);
                            events.ScheduleEvent(EVENT_QUANTUM_STRIKE, urand(3000, 5000));
                            break;
                        case EVENT_PHASE_PUNCH:
                            DoCastVictim(SPELL_PHASE_PUNCH);
                            events.ScheduleEvent(EVENT_PHASE_PUNCH, 15500);
                            break;
                        case EVENT_SUMMON_COLLAPSING_STAR:
                            Talk(SAY_ALGALON_COLLAPSING_STAR);
                            Talk(EMOTE_ALGALON_COLLAPSING_STAR);
                            for (uint32 i = 0; i < COLLAPSING_STAR_COUNT; ++i)
                                me->SummonCreature(NPC_COLLAPSING_STAR, CollapsingStarPos[i], TEMPSUMMON_CORPSE_DESPAWN);
                            events.ScheduleEvent(EVENT_SUMMON_COLLAPSING_STAR, 60000);
                            break;
                        case EVENT_BIG_BANG:
                        {
                            Talk(SAY_ALGALON_BIG_BANG);
                            Talk(EMOTE_ALGALON_BIG_BANG);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveIdle();
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            events.SetPhase(PHASE_BIG_BANG);
                            std::list<Creature*> constellations;
                            me->GetCreatureListWithEntryInGrid(constellations, NPC_LIVING_CONSTELLATION, 200.0f);
                            for (auto&& constellation : constellations)
                                constellation->AI()->DoAction(ACTION_BIG_BANG);
                            DoCastAOE(SPELL_BIG_BANG);
                            events.ScheduleEvent(EVENT_BIG_BANG, 90500);
                            events.ScheduleEvent(EVENT_RESUME_UPDATING, 12000);
                            break;
                        }
                        case EVENT_RESUME_UPDATING:
                            events.SetPhase(PHASE_NORMAL);
                            me->SetReactState(REACT_DEFENSIVE);
                            break;
                        case EVENT_ASCEND_TO_THE_HEAVENS:
                            Talk(SAY_ALGALON_ASCEND);
                            DoCastAOE(SPELL_ASCEND_TO_THE_HEAVENS);
                            events.ScheduleEvent(EVENT_EVADE, 2500);
                            break;
                        case EVENT_EVADE:
                            EnterEvadeMode();
                            break;
                        case EVENT_COSMIC_SMASH:
                            Talk(EMOTE_ALGALON_COSMIC_SMASH);
                            DoCastAOE(SPELL_COSMIC_SMASH);
                            events.ScheduleEvent(EVENT_COSMIC_SMASH, 25500);
                            break;
                        case EVENT_UNLOCK_YELL:
                            _hasYelled = false;
                            break;
                        case EVENT_OUTRO_START:
                            me->SetPhaseMask(PHASEMASK_ANYWHERE, false);
                            me->CastSpell(me, SPELL_KILL_CREDIT, true);
                            me->SetPhaseMask(PHASEMASK_NORMAL, false);
                            instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, me->GetEntry(), me);
                            instance->SetBossState(BOSS_ALGALON, DONE);
                            break;
                        case EVENT_OUTRO_1:
                            me->RemoveAllAuras();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_RENAME);
                            events.ScheduleEvent(EVENT_OUTRO_3, 1200);
                            events.ScheduleEvent(EVENT_OUTRO_4, 2400);
                            events.ScheduleEvent(EVENT_OUTRO_5, 8500);
                            events.ScheduleEvent(EVENT_OUTRO_6, 15500);
                            events.ScheduleEvent(EVENT_OUTRO_7, 55500);
                            events.ScheduleEvent(EVENT_OUTRO_8, 73500);
                            events.ScheduleEvent(EVENT_OUTRO_9, 85500);
                            events.ScheduleEvent(EVENT_OUTRO_10, 108500);
                            events.ScheduleEvent(EVENT_OUTRO_11, 123500);
                            break;
                        case EVENT_OUTRO_2:
                            me->GetMotionMaster()->MovePoint(POINT_ALGALON_OUTRO, AlgalonOutroPos);
                            break;
                        case EVENT_OUTRO_3:
                            break;
                        case EVENT_OUTRO_4:
                            DoCastAOE(SPELL_SUPERMASSIVE_FAIL);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            break;
                        case EVENT_OUTRO_5:
                            if (Creature* brann = DoSummon(NPC_BRANN_BRONZBEARD_ALG, BrannOutroPos[0], 131500ms, TEMPSUMMON_TIMED_DESPAWN))
                                brann->AI()->DoAction(ACTION_OUTRO);
                            break;
                        case EVENT_OUTRO_6:
                            Talk(SAY_ALGALON_OUTRO_1);
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            break;
                        case EVENT_OUTRO_7:
                            Talk(SAY_ALGALON_OUTRO_2);
                            break;
                        case EVENT_OUTRO_8:
                            Talk(SAY_ALGALON_OUTRO_3);
                            break;
                        case EVENT_OUTRO_9:
                            Talk(SAY_ALGALON_OUTRO_4);
                            break;
                        case EVENT_OUTRO_10:
                            Talk(SAY_ALGALON_OUTRO_5);
                            break;
                        case EVENT_OUTRO_11:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            DoCast(me, SPELL_TELEPORT);
                            me->DespawnOrUnsummon(1500);
                            break;
                        case EVENT_DESPAWN_ALGALON_1:
                            Talk(SAY_ALGALON_DESPAWN_1);
                            break;
                        case EVENT_DESPAWN_ALGALON_2:
                            Talk(SAY_ALGALON_DESPAWN_2);
                            break;
                        case EVENT_DESPAWN_ALGALON_3:
                            Talk(SAY_ALGALON_DESPAWN_3);
                            break;
                        case EVENT_DESPAWN_ALGALON_4:
                            DoCastAOE(SPELL_ASCEND_TO_THE_HEAVENS);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            bool _firstPull;
            bool _fedOnTears;
            bool _phaseTwo;
            bool _fightWon;
            bool _hasYelled;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_algalon_the_observerAI>(creature);
        }
};

class npc_living_constellation : public CreatureScript
{
    public:
        npc_living_constellation() : CreatureScript("npc_living_constellation") { }

        struct npc_living_constellationAI : public ScriptedAI
        {
            npc_living_constellationAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_ARCANE_BARRAGE, 2500);
                _isActive = false;
            }

            void KilledUnit(Unit* victim) override
            {
                if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                    algalon->AI()->KilledUnit(victim);
            }

            uint32 GetData(uint32 /*type*/) const override
            {
                return _isActive ? 1 : 0;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVATE_STAR:
                        if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                        {
                            if (Unit* target = algalon->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(algalon)))
                            {
                                me->SetReactState(REACT_AGGRESSIVE);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                AttackStart(target);
                                DoZoneInCombat();
                                _isActive = true;
                            }
                        }
                        break;
                    case ACTION_BIG_BANG:
                        _events.SetPhase(PHASE_BIG_BANG);
                        _events.DelayEvents(9500);
                        _events.ScheduleEvent(EVENT_RESUME_UPDATING, 9500);
                        break;
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id != SPELL_CONSTELLATION_PHASE_EFFECT || caster->GetTypeId() != TYPEID_UNIT)
                    return;

                me->DespawnOrUnsummon(0);
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, EVENT_ID_SUPERMASSIVE_START);
                caster->CastSpell((Unit*)nullptr, SPELL_BLACK_HOLE_CREDIT, TRIGGERED_FULL_MASK);
                if (Creature* voidZone = GetClosestCreatureWithEntry(caster, NPC_ALGALON_VOID_ZONE_VISUAL_STALKER, 10))
                    voidZone->DespawnOrUnsummon(1);
                caster->ToCreature()->DespawnOrUnsummon(1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!(_events.IsInPhase(PHASE_ROLE_PLAY) || _events.IsInPhase(PHASE_BIG_BANG)) && !UpdateVictim())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BARRAGE:
                            DoCastAOE(SPELL_ARCANE_BARRAGE);
                            me->ClearUnitState(UNIT_STATE_CASTING); // Allow movement during cast
                            _events.ScheduleEvent(EVENT_ARCANE_BARRAGE, 2500);
                            break;
                        case EVENT_RESUME_UPDATING:
                            _events.SetPhase(0);
                            break;
                    }
                }
            }

        private:
            EventMap _events;
            bool _isActive;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_living_constellationAI>(creature);
        }
};

class npc_collapsing_star : public CreatureScript
{
    public:
        npc_collapsing_star() : CreatureScript("npc_collapsing_star") { }

        struct npc_collapsing_starAI : public ScriptedAI
        {
            npc_collapsing_starAI(Creature* creature) : ScriptedAI(creature)
            {
                _dying = false;
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void UpdateAI(uint32 /*diff*/) override { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoZoneInCombat();
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() != NPC_BLACK_HOLE)
                    return;

                if (TempSummon* summ = me->ToTempSummon())
                    if (Creature* algalon = ObjectAccessor::GetCreature(*me, summ->GetSummonerGUID()))
                        algalon->AI()->JustSummoned(summon);

                DoZoneInCombat(summon);

                me->DespawnOrUnsummon(1);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (_dying)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth())
                {
                    _dying = true;
                    damage = 0;
                    DoCast(me, SPELL_BLACK_HOLE_SPAWN_VISUAL, true);
                    DoCast(me, SPELL_SUMMON_BLACK_HOLE, true);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                    algalon->AI()->KilledUnit(victim);
            }

            bool _dying;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_collapsing_starAI>(creature);
        }
};

class npc_black_hole : public CreatureScript
{
    public:
        npc_black_hole() : CreatureScript("npc_black_hole") { }

        struct npc_black_holeAI : public ScriptedAI
        {
            npc_black_holeAI(Creature* creature) : ScriptedAI(creature) { }

            void MoveInLineOfSight(Unit* /*who*/) override { }
            void AttackStart(Unit* /*who*/) override { }
            void UpdateAI(uint32 /*diff*/) override { }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (target && (spell->Id == SPELL_BLACK_HOLE_PHASE || spell->Id == SPELL_WORM_HOLE_PHASE))
                {
                    if (me->GetEntry() == NPC_BLACK_HOLE && !target->HasAura(SPELL_BLACK_HOLE_DAMAGE))
                        target->CastSpell(target, SPELL_BLACK_HOLE_DAMAGE, true, nullptr, nullptr, me->GetGUID());

                    if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                    {
                        algalon->GetThreatManager().getOnlineContainer().modifyThreatPercent(target, -100);
                        algalon->GetThreatManager().getOfflineContainer().modifyThreatPercent(target, -100); // SpellHitTarget is called after effects are handle, hence the target was already moved to offline container due to being in unreachable phase
                    }
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                    algalon->AI()->KilledUnit(victim);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_black_holeAI>(creature);
        }
};

class npc_brann_bronzebeard_algalon : public CreatureScript
{
    public:
        npc_brann_bronzebeard_algalon() : CreatureScript("npc_brann_bronzebeard_algalon") { }

        struct npc_brann_bronzebeard_algalonAI : public ScriptedAI
        {
            npc_brann_bronzebeard_algalonAI(Creature* creature) : ScriptedAI(creature) { }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START_INTRO:
                        _currentPoint = 0;
                        _events.Reset();
                        me->SetWalk(false);
                        _events.ScheduleEvent(EVENT_BRANN_MOVE_INTRO, 1);
                        break;
                    case ACTION_FINISH_INTRO:
                        Talk(SAY_BRANN_ALGALON_INTRO_2);
                        _events.ScheduleEvent(EVENT_BRANN_MOVE_INTRO, 1);
                        break;
                    case ACTION_OUTRO:
                        me->GetMotionMaster()->MovePoint(POINT_BRANN_OUTRO, BrannOutroPos[1]);
                        _events.ScheduleEvent(EVENT_BRANN_OUTRO_1, 89500);
                        _events.ScheduleEvent(EVENT_BRANN_OUTRO_2, 116500);
                        break;
                }
            }

            void MovementInform(uint32 movementType, uint32 point) override
            {
                if (movementType != POINT_MOTION_TYPE)
                    return;

                uint32 delay = 1;
                _currentPoint = point + 1;
                switch (point)
                {
                    case 2:
                        delay = 8000;
                        me->SetWalk(true);
                        break;
                    case 5:
                        me->SetWalk(false);
                        Talk(SAY_BRANN_ALGALON_INTRO_1);
                        _events.ScheduleEvent(EVENT_SUMMON_ALGALON, 7500);
                        return;
                    case 9:
                        me->DespawnOrUnsummon(1);
                        return;
                    case POINT_BRANN_OUTRO:
                    case POINT_BRANN_OUTRO_END:
                        return;
                }

                _events.ScheduleEvent(EVENT_BRANN_MOVE_INTRO, delay);
            }

            void UpdateAI(uint32 diff) override
            {
                UpdateVictim();

                if (_events.Empty())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BRANN_MOVE_INTRO:
                            if (_currentPoint < MAX_BRANN_WAYPOINTS_INTRO)
                                me->GetMotionMaster()->MovePoint(_currentPoint, BrannIntroWaypoint[_currentPoint]);
                            break;
                        case EVENT_SUMMON_ALGALON:
                            if (Creature* algalon = me->GetMap()->SummonCreature(NPC_ALGALON, AlgalonSummonPos))
                                algalon->AI()->DoAction(ACTION_START_INTRO);
                            _events.ScheduleEvent(EVENT_BRANN_INTRO_FINISH, 23000 + 12500);
                            break;
                        case EVENT_BRANN_INTRO_FINISH:
                            DoAction(ACTION_FINISH_INTRO);
                            break;
                        case EVENT_BRANN_OUTRO_1:
                            Talk(SAY_BRANN_ALGALON_OUTRO);
                            break;
                        case EVENT_BRANN_OUTRO_2:
                            me->GetMotionMaster()->MovePoint(POINT_BRANN_OUTRO_END, BrannOutroPos[2]);
                            break;
                    }
                }
            }

        private:
            EventMap _events;
            uint32 _currentPoint;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_brann_bronzebeard_algalonAI>(creature);
        }
};

class npc_dark_matter : public CreatureScript
{
    public:
        npc_dark_matter() : CreatureScript("npc_dark_matter") { }

        struct npc_dark_matterAI : public ScriptedAI
        {
            npc_dark_matterAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner && summoner->GetEntry() == NPC_WORM_HOLE)
                    if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                        algalon->AI()->JustSummoned(me);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* victim) override
            {
                if (Creature* algalon = me->FindNearestCreature(NPC_ALGALON, 200.0f))
                    algalon->AI()->KilledUnit(victim);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dark_matterAI>(creature);
        }
};

class go_celestial_planetarium_access : public GameObjectScript
{
    public:
        go_celestial_planetarium_access() : GameObjectScript("go_celestial_planetarium_access") { }

        struct go_celestial_planetarium_accessAI : public GameObjectAI
        {
            go_celestial_planetarium_accessAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {
                if (!isUse)
                    return true;

                if (me->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE))
                    return false;
                if (me->FindNearestCreature(NPC_BRANN_BRONZBEARD_ALG, 50.0f))
                    return false;

                // Start Algalon event
                me->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                _events.ScheduleEvent(EVENT_DESPAWN_CONSOLE, 5000);
                if (Creature* brann = me->SummonCreature(NPC_BRANN_BRONZBEARD_ALG, BrannIntroSpawnPos))
                    brann->AI()->DoAction(ACTION_START_INTRO);

                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    instance->SetData(DATA_ALGALON_SUMMON_STATE, 1);
                    if (GameObject* sigil = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_SIGILDOOR_01)))
                        sigil->SetGoState(GO_STATE_ACTIVE);

                    if (GameObject* sigil = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_SIGILDOOR_02)))
                        sigil->SetGoState(GO_STATE_ACTIVE);
                }

                return false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (_events.Empty())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESPAWN_CONSOLE:
                            me->Delete();
                            break;
                    }
                }
            }

            EventMap _events;
        };

        GameObjectAI* GetAI(GameObject* go) const
        {
            return GetInstanceAI<go_celestial_planetarium_accessAI>(go);
        }
};

class spell_algalon_phase_punch : public SpellScriptLoader
{
    public:
        spell_algalon_phase_punch() : SpellScriptLoader("spell_algalon_phase_punch") { }

        class spell_algalon_phase_punch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_algalon_phase_punch_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (GetStackAmount() != 1)
                    GetTarget()->RemoveAurasDueToSpell(PhasePunchAlphaId[GetStackAmount() - 2]);
                GetTarget()->CastSpell(GetTarget(), PhasePunchAlphaId[GetStackAmount() - 1], TRIGGERED_FULL_MASK);
                if (GetStackAmount() == 5)
                    Remove(AURA_REMOVE_BY_DEFAULT);
            }

            void OnRemove(AuraEffect const*, AuraEffectHandleModes)
            {
                if (GetStackAmount() != 5)
                    GetTarget()->RemoveAurasDueToSpell(PhasePunchAlphaId[GetStackAmount() - 1]);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_algalon_phase_punch_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectRemove += AuraEffectRemoveFn(spell_algalon_phase_punch_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_algalon_phase_punch_AuraScript();
        }
};

class NotVictimFilter
{
    public:
        NotVictimFilter(Unit* caster) : _victim(caster->GetVictim())
        {
        }

        bool operator()(WorldObject* target)
        {
            return target != _victim;
        }

    private:
        Unit* _victim;
};

class spell_algalon_arcane_barrage : public SpellScriptLoader
{
    public:
        spell_algalon_arcane_barrage() : SpellScriptLoader("spell_algalon_arcane_barrage") { }

        class spell_algalon_arcane_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_algalon_arcane_barrage_SpellScript);

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                targets.remove_if(NotVictimFilter(GetCaster()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_algalon_arcane_barrage_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_algalon_arcane_barrage_SpellScript();
        }
};

class ActiveConstellationFilter
{
    public:
        bool operator()(WorldObject* target) const
        {
            return target->ToUnit() && target->ToUnit()->GetAI() && target->ToUnit()->GetAI()->GetData(0);
        }
};

class spell_algalon_trigger_3_adds : public SpellScriptLoader
{
    public:
        spell_algalon_trigger_3_adds() : SpellScriptLoader("spell_algalon_trigger_3_adds") { }

        class spell_algalon_trigger_3_adds_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_algalon_trigger_3_adds_SpellScript);

            void SelectTarget(std::list<WorldObject*>& targets)
            {
                targets.remove_if(ActiveConstellationFilter());
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Creature* target = GetHitCreature();
                if (!target)
                    return;

                target->AI()->DoAction(ACTION_ACTIVATE_STAR);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_algalon_trigger_3_adds_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_algalon_trigger_3_adds_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_algalon_trigger_3_adds_SpellScript();
        }
};

class spell_algalon_collapse : public SpellScriptLoader
{
    public:
        spell_algalon_collapse() : SpellScriptLoader("spell_algalon_collapse") { }

        class spell_algalon_collapse_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_algalon_collapse_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                GetTarget()->DealDamage(GetTarget(), GetTarget()->CountPctFromMaxHealth(1), nullptr, NODAMAGE);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_algalon_collapse_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_algalon_collapse_AuraScript();
        }
};

class spell_algalon_big_bang : public SpellScriptLoader
{
    public:
        spell_algalon_big_bang() : SpellScriptLoader("spell_algalon_big_bang") { }

        class spell_algalon_big_bang_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_algalon_big_bang_SpellScript);

            bool Load() override
            {
                _targetCount = 0;
                return true;
            }

            void CountTargets(std::list<WorldObject*>& targets)
            {
                _targetCount = targets.size();
            }

            void CheckTargets()
            {
                if (!_targetCount)
                    GetCaster()->GetAI()->DoAction(ACTION_ASCEND);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_algalon_big_bang_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterCast += SpellCastFn(spell_algalon_big_bang_SpellScript::CheckTargets);
            }

            uint32 _targetCount;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_algalon_big_bang_SpellScript();
        }
};

class spell_algalon_remove_phase : public SpellScriptLoader
{
    public:
        spell_algalon_remove_phase() : SpellScriptLoader("spell_algalon_remove_phase") { }

        class spell_algalon_remove_phase_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_algalon_remove_phase_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                GetTarget()->RemoveAurasByType(SPELL_AURA_PHASE);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_algalon_remove_phase_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_algalon_remove_phase_AuraScript();
        }
};

class spell_algalon_cosmic_smash : public SpellScriptLoader
{
    public:
        spell_algalon_cosmic_smash() : SpellScriptLoader("spell_algalon_cosmic_smash") { }

        class spell_algalon_cosmic_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_algalon_cosmic_smash_SpellScript);

            void ModDestHeight(SpellEffIndex /*effIndex*/)
            {
                Position offset = {0.0f, 0.0f, 65.0f, 0.0f};
                const_cast<WorldLocation*>(GetExplTargetDest())->RelocateOffset(offset);
                GetHitDest()->RelocateOffset(offset);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_algalon_cosmic_smash_SpellScript::ModDestHeight, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_algalon_cosmic_smash_SpellScript();
        }
};

class spell_algalon_cosmic_smash_damage : public SpellScriptLoader
{
    public:
        spell_algalon_cosmic_smash_damage() : SpellScriptLoader("spell_algalon_cosmic_smash_damage") { }

        class spell_algalon_cosmic_smash_damage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_algalon_cosmic_smash_damage_SpellScript);

            void RecalculateDamage()
            {
                if (!GetExplTargetDest() || !GetHitUnit())
                    return;

                float distance = GetHitUnit()->GetDistance2d(GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY());
                if (distance > 6.0f)
                    SetHitDamage(int32(float(GetHitDamage()) / distance) * 2);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_algalon_cosmic_smash_damage_SpellScript::RecalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_algalon_cosmic_smash_damage_SpellScript();
        }
};

class spell_algalon_supermassive_fail : public SpellScriptLoader
{
    public:
        spell_algalon_supermassive_fail() : SpellScriptLoader("spell_algalon_supermassive_fail") { }

        class spell_algalon_supermassive_fail_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_algalon_supermassive_fail_SpellScript);

            void RecalculateDamage()
            {
                if (!GetHitPlayer())
                    return;

                GetHitPlayer()->ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, ACHIEVEMENT_CRITERIA_CONDITION_NO_SPELL_HIT, GetSpellInfo()->Id, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_algalon_supermassive_fail_SpellScript::RecalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_algalon_supermassive_fail_SpellScript();
        }
};

void AddSC_boss_algalon_the_observer()
{
    new boss_algalon_the_observer();
    new npc_living_constellation();
    new npc_collapsing_star();
    new npc_black_hole();
    new npc_brann_bronzebeard_algalon();
    new npc_dark_matter();
    new go_celestial_planetarium_access();
    new spell_algalon_phase_punch();
    new spell_algalon_arcane_barrage();
    new spell_algalon_trigger_3_adds();
    new spell_algalon_collapse();
    new spell_algalon_big_bang();
    new spell_algalon_remove_phase();
    new spell_algalon_cosmic_smash();
    new spell_algalon_cosmic_smash_damage();
    new spell_algalon_supermassive_fail();
}
