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

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "ulduar.h"
#include "Unit.h"
#include "Vehicle.h"
#include "PassiveAI.h"

#include <limits>
#include <map>
#include <mutex>

enum Yells
{
    // Mimiron
    SAY_AGGRO                                   = 0,
    SAY_HARDMODE_ON                             = 1,
    SAY_MKII_ACTIVATE                           = 2,
    SAY_MKII_SLAY                               = 3,
    SAY_MKII_DEATH                              = 4,
    SAY_VX001_ACTIVATE                          = 5,
    SAY_VX001_SLAY                              = 6,
    SAY_VX001_DEATH                             = 7,
    SAY_AERIAL_ACTIVATE                         = 8,
    SAY_AERIAL_SLAY                             = 9,
    SAY_AERIAL_DEATH                            = 10,
    SAY_V07TRON_ACTIVATE                        = 11,
    SAY_V07TRON_SLAY                            = 12,
    SAY_V07TRON_DEATH                           = 13,
    SAY_BERSERK                                 = 14,

    // Leviathan MK II
    EMOTE_PLASMA_BLAST                          = 0,

    // Computer
    SAY_SELF_DESTRUCT_START                     = 0,
    SAY_SELF_DESTRUCT_6                         = 1,
    SAY_SELF_DESTRUCT_5                         = 2,
    SAY_SELF_DESTRUCT_4                         = 3,
    SAY_SELF_DESTRUCT_3                         = 4,
    SAY_SELF_DESTRUCT_2                         = 5,
    SAY_SELF_DESTRUCT_1                         = 6,
    SAY_SELF_DESTRUCT_0                         = 7,
    SAY_SELF_DESTRUCT_7                         = 8,
    SAY_SELF_DESTRUCT_8                         = 9,
    SAY_SELF_DESTRUCT_9                         = 10,
    SAY_SELF_DESTRUCT_10                        = 11,
    SAY_SELF_DESTRUCT_END                       = 12,
};

enum Spells
{
    SPELL_JETPACK                               = 63341, // Unused
    SPELL_EMERGENCY_MODE                        = 64582,
    SPELL_SELF_REPAIR                           = 64383,

    // Leviathan MK II
    SPELL_MINES_SPAWN                           = 65347,
    SPELL_FLAME_SUPPRESSANT_MK                  = 64570,
    SPELL_NAPALM_SHELL                          = 63666,
    SPELL_PLASMA_BLAST                          = 62997,
    SPELL_PROXIMITY_MINES                       = 63027,
    SPELL_SHOCK_BLAST                           = 63631,
    SPELL_EXPLOSION                             = 66351,
    SPELL_EXPLOSION_25                          = 63009,
    SPELL_DEATH_GRIP                            = 49560,     // used at MK turret's immunity

    // VX 001
    SPELL_FROSTBOMB                             = 64623,    // 64627 ?
    SPELL_FROST_BOMB_VISUAL                     = 64624,
    SPELL_FROST_BOMB_EXPLOSION_10               = 64626,
    SPELL_FROST_BOMB_EXPLOSION_25               = 65333,

    SPELL_P3WX2_LASER_BARRAGE                   = 63293,    // Cast through SPELL_SPINNING_UP
    SPELL_P3WX2_LASER_BARRAGE_LEVIATHAN         = 66490,    // Root aura on Leviathan Mk II
    SPELL_RAPID_BURST                           = 63382,
    SPELL_RAPID_BURST_LEFT_10                   = 63387,
    SPELL_RAPID_BURST_RIGHT_10                  = 64019,
    SPELL_RAPID_BURST_LEFT_25                   = 64531,
    SPELL_RAPID_BURST_RIGHT_25                  = 64532,
    SPELL_ROCKET_STRIKE                         = 63036,
    SPELL_ROCKET_STRIKE_AURA                    = 64064,
    SPELL_ROCKET_STRIKE_DMG                     = 63041,
    SPELL_SPINNING_UP                           = 63414,
    SPELL_HEAT_WAVE                             = 63677,
    SPELL_HAND_PULSE                            = 64348,
    SPELL_SELF_STUN                             = 14821,
    SPELL_BOMB_BOT                              = 63811,
    SPELL_SUMMON_SCRAP_BOT_JUNK_BOT             = 63819,
    SPELL_SUMMON_SCRAP_BOT_TRIGGER_JUNK_BOT     = 63820,
    SPELL_SUMMON_SCRAP_BOT_AURA_JUNK_BOT        = 64398,
    SPELL_SUMMON_SCRAP_BOT_TRIGGER_ASSAULT_BOT  = 64425,
    SPELL_SUMMON_SCRAP_BOT_AURA_ASSAULT_BOT     = 64426,
    SPELL_SUMMON_SCRAP_BOT_ASSAULT_BOT          = 64427,
    SPELL_SUMMON_FIRE_BOT_TRIGGER               = 64620,
    SPELL_SUMMON_FIRE_BOT_AURA                  = 64621,
    SPELL_SUMMON_FIRE_BOT                       = 64622,

    // Aerial Command Unit
    SPELL_PLASMA_BALL                           = 63689,
    SPELL_MAGNETIC_CORE                         = 64436,
    SPELL_MAGNETIC_CORE_VISUAL                  = 64438,
    SPELL_BOOM_BOT                              = 63767,
    SPELL_BOOM_BOT_PERIODIC                     = 63801,
    SPELL_MAGNETIC_FIELD                        = 64668,
    SPELL_HOVER                                 = 57764, // Set Hover position
    SPELL_BERSERK                               = 47008,

    // Additonal spells
    SPELL_DEAFENING_SIREN                       = 64616,

    SPELL_SELF_DESTRUCTION                      = 64610,
    SPELL_SELF_DESTRUCTION_VISUAL               = 64613,
    SPELL_FLAME_SUPPRESSANT_VX001               = 65192,
    SPELL_SUMMON_FLAMES_INITIAL                 = 64563,
    SPELL_FLAME                                 = 64561,
    SPELL_NOT_SO_FRIENDLY_FIRE                  = 65040,

    SPELL_WATER_SPRAY                           = 64619,
    SPELL_RIDE_VEHICLE                          = 46598,
};

enum Events
{
    // Mimiron
    EVENT_CHECK_TARGET              = 1,
    EVENT_ENRAGE                    = 2,
    EVENT_FLAME                     = 3,
    EVENT_STEP_1                    = 4,
    EVENT_STEP_2                    = 5,
    EVENT_STEP_3                    = 6,
    EVENT_STEP_4                    = 7,
    EVENT_STEP_5                    = 8,
    EVENT_STEP_6                    = 9,
    EVENT_STEP_7                    = 10,
    EVENT_STEP_8                    = 11,
    EVENT_STEP_9                    = 12,
    EVENT_CHECK_BOTALIVE            = 13,

    // Leviathan MK II
    EVENT_PROXIMITY_MINE            = 14,
    EVENT_NAPALM_SHELL              = 15,
    EVENT_PLASMA_BLAST              = 16,
    EVENT_SHOCK_BLAST               = 17,
    EVENT_FLAME_SUPPRESSANT         = 18,

    // VX-001
    EVENT_RAPID_BURST               = 19,
    EVENT_LASER_BARRAGE             = 20,
    EVENT_LASER_BARRAGE_END         = 21,
    EVENT_ROCKET_STRIKE             = 22,
    EVENT_HEAT_WAVE                 = 23,
    EVENT_HAND_PULSE                = 24,
    EVENT_FROST_BOMB                = 25,
    EVENT_FLAME_SUPPRESSANT_VX001   = 26,

    // Aerial unit
    EVENT_PLASMA_BALL               = 27,
    EVENT_REACTIVATE_AERIAL         = 28,
    EVENT_SUMMON_JUNK_BOT           = 29,
    EVENT_SUMMON_ASSAULT_BOT        = 30,
    EVENT_SUMMON_BOOM_BOT           = 31,
    EVENT_SUMMON_FIRE_BOT           = 32,

    // Computer
    EVENT_SELF_DESTRUCT_MIMIRON_YELL,
    EVENT_SELF_DESTRUCT_10,
    EVENT_SELF_DESTRUCT_9,
    EVENT_SELF_DESTRUCT_8,
    EVENT_SELF_DESTRUCT_7,
    EVENT_SELF_DESTRUCT_6,
    EVENT_SELF_DESTRUCT_5,
    EVENT_SELF_DESTRUCT_4,
    EVENT_SELF_DESTRUCT_3,
    EVENT_SELF_DESTRUCT_2,
    EVENT_SELF_DESTRUCT_1,
};

enum Phases
{
    // Mimiron
    PHASE_IDLE                              = 0,
    PHASE_INTRO                             = 1,
    PHASE_COMBAT                            = 2,
    PHASE_VX001_ACTIVATION                  = 3,
    PHASE_AERIAL_ACTIVATION                 = 4,
    PHASE_V0L7R0N_ACTIVATION                = 5,

    // Leviathan MK II
    PHASE_LEVIATHAN_SOLO__GLOBAL_1          = 6,
    PHASE_LEVIATHAN_ASSEMBLED__GLOBAL_4     = 7,

    // VX-001
    PHASE_VX001_SOLO__GLOBAL_2              = 8,
    PHASE_VX001_ASSEMBLED__GLOBAL_4         = 9,

    // Aerial unit
    PHASE_AERIAL_SOLO__GLOBAL_3             = 10,
    PHASE_AERIAL_ASSEMBLED__GLOBAL_4        = 11,
};

enum Actions
{
    DO_START_ENCOUNTER                          = 1,
    DO_ACTIVATE_VX001                           = 2,
    DO_START_VX001                              = 3,
    DO_ACTIVATE_AERIAL                          = 4,
    DO_START_AERIAL                             = 5,
    DO_DISABLE_AERIAL                           = 6,
    DO_ACTIVATE_V0L7R0N                         = 7,
    DO_LEVIATHAN_ASSEMBLED                      = 8,
    DO_LEVIATHAN_SELF_REPAIR_START              = 9,
    DO_LEVIATHAN_SELF_REPAIR_END                = 10,
    DO_VX001_ASSEMBLED                          = 11,
    DO_VX001_SELF_REPAIR_START                  = 12,
    DO_VX001_SELF_REPAIR_END                    = 13,
    DO_AERIAL_ASSEMBLED                         = 14,
    DO_AERIAL_SELF_REPAIR_START                 = 15,
    DO_AERIAL_SELF_REPAIR_END                   = 16,
    DO_ENTER_ENRAGE                             = 17,
    DO_ACTIVATE_HARD_MODE                       = 18,
    DO_INCREASE_FLAME_COUNT                     = 19,
    DO_DECREASE_FLAME_COUNT                     = 20,
    DATA_GET_HARD_MODE                          = 21,
    DATA_FLAME_COUNT                            = 22
};

enum Npcs
{
    NPC_COMPUTER                                = 34143,
    NPC_ROCKET                                  = 34050,
    NPC_BURST_TARGET                            = 34211,
    NPC_JUNK_BOT                                = 33855,
    NPC_ASSAULT_BOT                             = 34057,
    NPC_BOMB_BOT                                = 33836,
    NPC_FIRE_BOT                                = 34147,
    NPC_FLAMES_INITIAL                          = 34363,
    NPC_FLAME_SPREAD                            = 34121,
    NPC_FROST_BOMB                              = 34149,
    NPC_MKII_TURRET                             = 34071,
    NPC_PROXIMITY_MINE                          = 34362,
    NPC_MIMIRON_FOCUS                           = 33835
};

// Achievements
#define ACHIEVEMENT_SET_UP_US_THE_BOMB          RAID_MODE(2989, 3237)
// TODO:
// Achiev 2989: Criterias 10543, 10544, 10545
// Achiev 3237: Criterias 10546, 10547, 10548
// Which criteria is reponsible for which step ?
// 26491 - 38080 - 45033 <- what do these numbers mean ?

enum BombIndices // Scripts are present, but criteria-id <-> script is missing
{
    DATA_AVOIDED_PROXIMITY_MINES     = 30,
    DATA_AVOIDED_ROCKET_STRIKES,
    DATA_AVOIDED_BOOM_BOT_EXPLOSION
};

Position const turretPos            = {2744.64f, 2569.459f, 364.397f, 0.0f};

#define FLAME_CAP 200 // find a good (blizzlike!) value

struct EqualHelper // Helper for Mimirons map that stores the members repair states
{
    EqualHelper(bool& base) : __base(base) { }
    void operator()(std::pair<uint32, bool> value)
    {
        __base = __base && value.second;
    }

private:
    bool& __base;
};

class boss_mimiron : public CreatureScript
{
    public:
        boss_mimiron() : CreatureScript("boss_mimiron") { }

        struct boss_mimironAI : public BossAI
        {
            boss_mimironAI(Creature* creature) : BossAI(creature, BOSS_MIMIRON)
            {
                me->SetReactState(REACT_PASSIVE);
                _gotEncounterFinished = false;
            }

            void DespawnCreatures(uint32 entry, float distance)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, distance);

                if (creatures.empty())
                    return;

                for (auto&& creature : creatures)
                    if (creature)
                        creature->DespawnOrUnsummon();
            }

            void Reset() override
            {
                _gotEncounterFinished = _gotEncounterFinished || (instance && instance->GetBossState(BOSS_MIMIRON) == DONE);
                if (_gotEncounterFinished)
                    return;

                _Reset();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_USE_STANDING);
                me->SetVisible(true);
                me->ExitVehicle();
                me->GetMotionMaster()->MoveTargetedHome();

                instance->SetData(DATA_MIMIRON_ELEVATOR, GO_STATE_ACTIVE);
                instance->DoUpdateWorldState(WORLD_STATE_MIMIRON, 0);

                _phase = PHASE_IDLE;
                events.SetPhase(_phase);
                _flameCount = 0;
                _gotHardMode = false;
                me->GetMap()->SetWorldState(WORLDSTATE_FIRE_FIGHTER, 0);
                _enraged = false;

                me->GetMap()->SetWorldState(WORLDSTATE_SET_UP_US_BOMB_MINE, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_SET_UP_US_BOMB_ROCKET, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_SET_UP_US_BOMB_BOT, 1);

                events.ScheduleEvent(EVENT_CHECK_TARGET, 7*IN_MILLISECONDS);

                DespawnCreatures(NPC_FLAMES_INITIAL, 100.0f);
                DespawnCreatures(NPC_PROXIMITY_MINE, 100.0f);
                DespawnCreatures(NPC_ROCKET, 100.0f);
                DespawnCreatures(NPC_JUNK_BOT, 100.0f);
                DespawnCreatures(NPC_ASSAULT_BOT, 100.0f);
                DespawnCreatures(NPC_BOMB_BOT, 100.0f);
                DespawnCreatures(NPC_FIRE_BOT, 100.0f);

                for (uint8 data = DATA_LEVIATHAN_MK_II; data <= DATA_AERIAL_UNIT; ++data)
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(data)))
                        if (creature->IsAlive())
                        {
                            creature->ExitVehicle();
                            creature->AI()->EnterEvadeMode();
                        }

                if (GameObject* go = me->FindNearestGameObject(GO_BIG_RED_BUTTON, 200))
                {
                    go->SetGoState(GO_STATE_READY);
                    go->SetLootState(GO_JUST_DEACTIVATED);
                    go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }

                _isSelfRepairing[DATA_LEVIATHAN_MK_II] = false;
                _isSelfRepairing[DATA_VX_001] = false;
                _isSelfRepairing[DATA_AERIAL_UNIT] = false;

                _avoidedProximityMines = true;
                _avoidedRocketStrikes = true;
                _avoidedBoomBotExplosion = true;
            }

            void BotAliveCheck()
            {
                if (_phase != PHASE_COMBAT)
                    return;

                if (_mapMutex.try_lock())
                {
                    bool res = true;
                    // Check if there is still a false value.
                    std::for_each(_isSelfRepairing.begin(), _isSelfRepairing.end(), EqualHelper(res));
                    _mapMutex.unlock();
                    if (res)
                    {
                        // We're down, baby.
                        Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LEVIATHAN_MK_II));
                        Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001));
                        Creature* AerialUnit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_AERIAL_UNIT));
                        if (Leviathan && VX_001 && AerialUnit)
                        {
                            Leviathan->DisappearAndDie();
                            VX_001->DisappearAndDie();
                            AerialUnit->DisappearAndDie();
                            DespawnCreatures(NPC_FLAMES_INITIAL, 100.0f);
                            DespawnCreatures(NPC_PROXIMITY_MINE, 100.0f);
                            DespawnCreatures(NPC_ROCKET, 100);
                            me->ExitVehicle();
                            EncounterPostProgress();
                        }
                    }
                }
            }

            void EncounterPostProgress()
            {
                if (_gotEncounterFinished)
                    return;

                instance->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_LEVIATHAN_MKII, nullptr);

                me->SetReactState(REACT_PASSIVE);
                me->RemoveAllAuras();
                me->AttackStop();
                me->SetFaction(35);
                me->DeleteThreatList();
                me->CombatStop(true);

                if (_gotHardMode)
                {
                    instance->SetData(DATA_MIMIRON_HARDMODE, true);
                    ComputerTalk(SAY_SELF_DESTRUCT_END);
                    me->m_Events.Schedule(11000, [this]() { Talk(SAY_V07TRON_DEATH); });
                }
                else
                    Talk(SAY_V07TRON_DEATH);

                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, NPC_LEVIATHAN_MKII, 1);

                _JustDied();
                me->DespawnOrUnsummon(30*IN_MILLISECONDS);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _gotEncounterFinished = _gotEncounterFinished || (instance && instance->GetBossState(BOSS_MIMIRON) == DONE);
                if (_gotEncounterFinished)
                    return;

                _JustEngagedWith();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                _phase = PHASE_INTRO;
                events.SetPhase(_phase);
                if (_gotHardMode)
                {
                    events.ScheduleEvent(EVENT_FLAME, 5*IN_MILLISECONDS);

                    uint32 delay = 5000;
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_10, delay);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_MIMIRON_YELL, delay + 1000);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_9, delay + 1*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_8, delay + 2*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_7, delay + 3*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_6, delay + 4*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_5, delay + 5*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_4, delay + 6*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_3, delay + 7*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_2, delay + 8*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_1, delay + 9*MINUTE*IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_ENRAGE, delay + 10*MINUTE*IN_MILLISECONDS); // Enrage in 10 (hard mode) min
                }
                else
                    events.ScheduleEvent(EVENT_ENRAGE, 15*MINUTE*IN_MILLISECONDS); // Enrage in 15 min

                events.ScheduleEvent(EVENT_STEP_1, 0.1*IN_MILLISECONDS, 0, _phase);

                if (GameObject* go = me->FindNearestGameObject(GO_BIG_RED_BUTTON, 200.0f))
                    go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/) override
            {
                EncounterPostProgress();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_TARGET:
                            // prevent mimiron staying infight with leviathan introduced in rev #b40bf69
                            // TODO: find out why this happens
                            if (!SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            {
                                EnterEvadeMode();
                                return;
                            }
                            events.ScheduleEvent(EVENT_CHECK_TARGET, 7*IN_MILLISECONDS);
                            return;
                        case EVENT_ENRAGE:
                            Talk(SAY_BERSERK);
                            for (uint8 data = DATA_LEVIATHAN_MK_II; data <= DATA_AERIAL_UNIT; ++data)
                                if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(data)))
                                    creature->AI()->DoAction(DO_ENTER_ENRAGE);
                            _enraged = true;
                            if (_gotHardMode)
                            {
                                ComputerTalk(SAY_SELF_DESTRUCT_0);
                                DoCast(me, SPELL_SELF_DESTRUCTION, true);
                                DoCast(me, SPELL_SELF_DESTRUCTION_VISUAL, true);
                            }
                            return;
                        case EVENT_SELF_DESTRUCT_MIMIRON_YELL: Talk(SAY_HARDMODE_ON); break;
                        case EVENT_SELF_DESTRUCT_10: ComputerTalk(SAY_SELF_DESTRUCT_10); break;
                        case EVENT_SELF_DESTRUCT_9: ComputerTalk(SAY_SELF_DESTRUCT_9); break;
                        case EVENT_SELF_DESTRUCT_8: ComputerTalk(SAY_SELF_DESTRUCT_8); break;
                        case EVENT_SELF_DESTRUCT_7: ComputerTalk(SAY_SELF_DESTRUCT_7); break;
                        case EVENT_SELF_DESTRUCT_6: ComputerTalk(SAY_SELF_DESTRUCT_6); break;
                        case EVENT_SELF_DESTRUCT_5: ComputerTalk(SAY_SELF_DESTRUCT_5); break;
                        case EVENT_SELF_DESTRUCT_4: ComputerTalk(SAY_SELF_DESTRUCT_4); break;
                        case EVENT_SELF_DESTRUCT_3: ComputerTalk(SAY_SELF_DESTRUCT_3); break;
                        case EVENT_SELF_DESTRUCT_2: ComputerTalk(SAY_SELF_DESTRUCT_2); break;
                        case EVENT_SELF_DESTRUCT_1: ComputerTalk(SAY_SELF_DESTRUCT_1); break;
                        case EVENT_FLAME:
                            for (uint8 i = 0; i < 3; ++i)
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                    DoCast(target, SPELL_SUMMON_FLAMES_INITIAL, true);
                            events.ScheduleEvent(EVENT_FLAME, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_STEP_1:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    if (_gotHardMode)
                                    {
                                        ComputerTalk(SAY_SELF_DESTRUCT_START);
                                        events.ScheduleEvent(EVENT_STEP_2, 16*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    }
                                    else
                                    {
                                        Talk(SAY_AGGRO);
                                        events.ScheduleEvent(EVENT_STEP_2, 10*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    }
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    Talk(SAY_MKII_DEATH);
                                    events.ScheduleEvent(EVENT_STEP_2, 10*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                case PHASE_AERIAL_ACTIVATION:
                                    me->ChangeSeat(4);
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                                    events.ScheduleEvent(EVENT_STEP_2, 2.5*IN_MILLISECONDS, 0, PHASE_AERIAL_ACTIVATION);
                                    break;
                                case PHASE_V0L7R0N_ACTIVATION:
                                    if (instance)
                                    {
                                        //me->SetVisible(true);
                                        if (Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LEVIATHAN_MK_II)))
                                            Leviathan->GetMotionMaster()->MovePoint(0, 2744.65f, 2569.46f, 364.397f);
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                        {
                                            me->EnterVehicle(VX_001, 1);
                                            me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                                            Talk(SAY_AERIAL_DEATH);
                                        }
                                    }
                                    events.ScheduleEvent(EVENT_STEP_2, 5*IN_MILLISECONDS, 0, PHASE_V0L7R0N_ACTIVATION);
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_2:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    if (instance)
                                        if (Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LEVIATHAN_MK_II)))
                                            me->EnterVehicle(Leviathan, 4);
                                    events.ScheduleEvent(EVENT_STEP_3, 2*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    me->ChangeSeat(1);
                                    events.ScheduleEvent(EVENT_STEP_3, 2*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                case PHASE_AERIAL_ACTIVATION:
                                    Talk(SAY_VX001_DEATH);
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                                    events.ScheduleEvent(EVENT_STEP_3, 5*IN_MILLISECONDS, 0, PHASE_AERIAL_ACTIVATION);
                                    break;
                                case PHASE_V0L7R0N_ACTIVATION:
                                    if (instance)
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                            if (Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LEVIATHAN_MK_II)))
                                            {
                                                VX_001->SetStandState(UNIT_STAND_STATE_STAND);
                                                VX_001->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_CUSTOM_SPELL_01);
                                                VX_001->EnterVehicle(Leviathan, 7);
                                            }
                                    events.ScheduleEvent(EVENT_STEP_3, 2*IN_MILLISECONDS, 0, PHASE_V0L7R0N_ACTIVATION);
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_3:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    me->ChangeSeat(2);
                                    events.ScheduleEvent(EVENT_STEP_4, 2*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    if (instance)
                                        instance->SetData(DATA_MIMIRON_ELEVATOR, GO_STATE_READY);
                                    events.ScheduleEvent(EVENT_STEP_4, 15*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                case PHASE_AERIAL_ACTIVATION:
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                                    if (instance)
                                    {
                                        if (Creature* AerialUnit = me->SummonCreature(NPC_AERIAL_COMMAND_UNIT, 2744.65f, 2569.46f, 380.0f, 3.14159f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                                        {
                                            AerialUnit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                            AerialUnit->SetVisible(true);
                                        }
                                    }
                                    events.ScheduleEvent(EVENT_STEP_4, 5*IN_MILLISECONDS, 0, PHASE_AERIAL_ACTIVATION);
                                    break;
                                case PHASE_V0L7R0N_ACTIVATION:
                                    if (instance)
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                            if (Creature* AerialUnit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_AERIAL_UNIT)))
                                            {
                                                AerialUnit->SetFlying(false);
                                                AerialUnit->EnterVehicle(VX_001, 3);
                                                instance->DoUpdateWorldState(WORLD_STATE_MIMIRON, 1);
                                                Talk(SAY_V07TRON_ACTIVATE);
                                            }
                                    events.ScheduleEvent(EVENT_STEP_4, 10*IN_MILLISECONDS, 0, PHASE_V0L7R0N_ACTIVATION);
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_4:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    me->ChangeSeat(5);
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                                    events.ScheduleEvent(EVENT_STEP_5, 2.5*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    if (instance)
                                    {
                                        if (Creature* VX_001 = me->SummonCreature(NPC_VX_001, 2744.65f, 2569.46f, 364.397f, 3.14159f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                                        {
                                            instance->SetData(DATA_MIMIRON_ELEVATOR, GO_STATE_ACTIVE_ALTERNATIVE);
                                            VX_001->SetVisible(true);
                                            VX_001->SetFaction(35);
                                            VX_001->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                            for (uint8 n = 5; n < 7; ++n)
                                            {
                                                if (Creature* Rocket = VX_001->SummonCreature(NPC_ROCKET, VX_001->GetPositionX(), VX_001->GetPositionY(), VX_001->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                                                {
                                                    Rocket->SetFaction(14);
                                                    Rocket->SetReactState(REACT_PASSIVE);
                                                    Rocket->EnterVehicle(VX_001, n);
                                                }
                                            }
                                        }
                                    }
                                    events.ScheduleEvent(EVENT_STEP_5, 8*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                case PHASE_AERIAL_ACTIVATION:
                                    me->ExitVehicle();
                                    me->GetMotionMaster()->MoveJump(2745.06f, 2569.36f, 379.90f, 10, 15);
                                    // entering the vehicle makes Mimiron leave combat and restart the encounter
                                    if (Creature* AerialUnit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_AERIAL_UNIT)))
                                        me->EnterVehicle(AerialUnit, 0);
                                    events.ScheduleEvent(EVENT_STEP_5, 2*IN_MILLISECONDS, 0, PHASE_AERIAL_ACTIVATION);
                                    break;
                                case PHASE_V0L7R0N_ACTIVATION:
                                    if (instance)
                                    {
                                        if (Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LEVIATHAN_MK_II)))
                                            Leviathan->AI()->DoAction(DO_LEVIATHAN_ASSEMBLED);
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                            VX_001->AI()->DoAction(DO_VX001_ASSEMBLED);
                                        if (Creature* AerialUnit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_AERIAL_UNIT)))
                                            AerialUnit->AI()->DoAction(DO_AERIAL_ASSEMBLED);
                                        _phase = PHASE_COMBAT;
                                        events.SetPhase(_phase);
                                    }
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_5:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    Talk(SAY_MKII_ACTIVATE);
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                                    events.ScheduleEvent(EVENT_STEP_6, 6*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    if (instance)
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                            me->EnterVehicle(VX_001, 0);
                                    events.ScheduleEvent(EVENT_STEP_6, 3.5*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                case PHASE_AERIAL_ACTIVATION:
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                                    Talk(SAY_AERIAL_ACTIVATE);
                                    events.ScheduleEvent(EVENT_STEP_6, 8*IN_MILLISECONDS, 0, PHASE_AERIAL_ACTIVATION);
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_6:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    me->ChangeSeat(6);
                                    events.ScheduleEvent(EVENT_STEP_7, 2*IN_MILLISECONDS, 0, PHASE_INTRO);
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                                    Talk(SAY_VX001_ACTIVATE);
                                    events.ScheduleEvent(EVENT_STEP_7, 10*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                case PHASE_AERIAL_ACTIVATION:
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                                    //me->SetVisible(false);
                                    if (instance)
                                    {
                                        if (Creature* AerialUnit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_AERIAL_UNIT)))
                                        {
                                            AerialUnit->AI()->DoAction(DO_START_AERIAL);
                                            AerialUnit->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                            _phase = PHASE_COMBAT;
                                            events.SetPhase(_phase);
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_7:
                            switch (_phase)
                            {
                                case PHASE_INTRO:
                                    if (instance)
                                    {
                                        if (Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LEVIATHAN_MK_II)))
                                        {
                                            me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                                            Leviathan->AI()->DoAction(DO_START_ENCOUNTER);
                                            _phase = PHASE_COMBAT;
                                            events.SetPhase(_phase);
                                        }
                                    }
                                    break;
                                case PHASE_VX001_ACTIVATION:
                                    me->ChangeSeat(1);
                                    me->SetUInt32Value(UNIT_FIELD_NPC_EMOTESTATE, EMOTE_STATE_SIT);
                                    events.ScheduleEvent(EVENT_STEP_8, 2*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_8:
                            switch (_phase)
                            {
                                case PHASE_VX001_ACTIVATION:
                                    if (instance)
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                            VX_001->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                                    events.ScheduleEvent(EVENT_STEP_9, 3.5*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                                    break;
                                default:
                                    break;
                            }
                            return;
                        case EVENT_STEP_9:
                            switch (_phase)
                            {
                                case PHASE_VX001_ACTIVATION:
                                    if (instance)
                                        if (Creature* VX_001 = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VX_001)))
                                        {
                                            VX_001->AddAura(SPELL_HOVER, VX_001); // Hover
                                            VX_001->AI()->DoAction(DO_START_VX001);
                                            _phase = PHASE_COMBAT;
                                            events.SetPhase(_phase);
                                        }
                                    break;
                                default:
                                    break;
                            }
                            return;
                        default:
                            return;
                    }
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_GET_HARD_MODE:
                        return _gotHardMode ? true : false;
                    case DATA_FLAME_COUNT:
                        return _flameCount;
                    case DATA_AVOIDED_ROCKET_STRIKES:
                        return _avoidedRocketStrikes ? true : false;
                    case DATA_AVOIDED_PROXIMITY_MINES:
                        return _avoidedProximityMines ? true : false;
                    case DATA_AVOIDED_BOOM_BOT_EXPLOSION:
                        return _avoidedBoomBotExplosion ? true : false;
                    default:
                        break;
                }
                return 0;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case DO_ACTIVATE_VX001:
                        _phase = PHASE_VX001_ACTIVATION;
                        events.SetPhase(PHASE_VX001_ACTIVATION);
                        events.ScheduleEvent(EVENT_STEP_1, 0.1*IN_MILLISECONDS, 0, PHASE_VX001_ACTIVATION);
                        break;
                    case DO_ACTIVATE_AERIAL:
                        _phase = PHASE_AERIAL_ACTIVATION;
                        events.SetPhase(PHASE_AERIAL_ACTIVATION);
                        events.ScheduleEvent(EVENT_STEP_1, 5*IN_MILLISECONDS, 0, PHASE_AERIAL_ACTIVATION);
                        break;
                    case DO_ACTIVATE_V0L7R0N:
                        me->SetVisible(true);
                        _phase = PHASE_V0L7R0N_ACTIVATION;
                        events.SetPhase(PHASE_V0L7R0N_ACTIVATION);
                        events.ScheduleEvent(EVENT_STEP_1, 1*IN_MILLISECONDS, 0, PHASE_V0L7R0N_ACTIVATION);
                        break;
                    case DO_ACTIVATE_HARD_MODE:     // Cannot be done infight, since the button gets locked on JustEngagedWith() with Mimiron.
                        me->GetMap()->SetWorldState(WORLDSTATE_FIRE_FIGHTER, 1);
                        _gotHardMode = true;
                        DoZoneInCombat();
                        break;
                    case DO_INCREASE_FLAME_COUNT:
                        ++_flameCount;
                        break;
                    case DO_DECREASE_FLAME_COUNT:
                        if (_flameCount)
                            --_flameCount;
                        break;
                    // Repair stuff
                    case DO_LEVIATHAN_SELF_REPAIR_START:
                        if (_mapMutex.try_lock())
                        {
                            _isSelfRepairing[DATA_LEVIATHAN_MK_II] = true;
                            _mapMutex.unlock();
                        }                        
                        BotAliveCheck();
                        break;
                    case DO_LEVIATHAN_SELF_REPAIR_END:
                        if (_mapMutex.try_lock())
                        {
                            _isSelfRepairing[DATA_LEVIATHAN_MK_II] = false;
                            _mapMutex.unlock();
                        } 
                        break;
                    case DO_VX001_SELF_REPAIR_START:
                        if (_mapMutex.try_lock())
                        {
                            _isSelfRepairing[DATA_VX_001] = true;
                            _mapMutex.unlock();
                        }                         
                        BotAliveCheck();
                        break;
                    case DO_VX001_SELF_REPAIR_END:
                        if (_mapMutex.try_lock())
                        {
                            _isSelfRepairing[DATA_VX_001] = false;
                            _mapMutex.unlock();
                        }                         
                        break;
                    case DO_AERIAL_SELF_REPAIR_START:
                        if (_mapMutex.try_lock())
                        {
                            _isSelfRepairing[DATA_AERIAL_UNIT] = true;
                            _mapMutex.unlock();
                        }                        
                        BotAliveCheck();
                        break;
                    case DO_AERIAL_SELF_REPAIR_END:
                        if (_mapMutex.try_lock())
                        {
                            _isSelfRepairing[DATA_AERIAL_UNIT] = false;
                            _mapMutex.unlock();
                        }                         
                        break;
                    // Achiev
                    case DATA_AVOIDED_ROCKET_STRIKES:
                        _avoidedRocketStrikes = false;
                        me->GetMap()->SetWorldState(WORLDSTATE_SET_UP_US_BOMB_ROCKET, 0);
                        break;
                    case DATA_AVOIDED_PROXIMITY_MINES:
                        _avoidedProximityMines = false;
                        me->GetMap()->SetWorldState(WORLDSTATE_SET_UP_US_BOMB_MINE, 0);
                        break;
                    case DATA_AVOIDED_BOOM_BOT_EXPLOSION:
                        _avoidedBoomBotExplosion = false;
                        me->GetMap()->SetWorldState(WORLDSTATE_SET_UP_US_BOMB_BOT, 0);
                        break;
                    default:
                        break;
                }
            }

            private:
                std::mutex _mapMutex;
                std::map<uint32, bool> _isSelfRepairing;
                std::map<BombIndices, bool> _setUpUsTheBomb;
                Phases _phase;
                uint32 _flameCount;
                bool _gotHardMode;
                bool _enraged;
                bool _gotEncounterFinished;
                bool _avoidedRocketStrikes;
                bool _avoidedProximityMines;
                bool _avoidedBoomBotExplosion;

            void ComputerTalk(uint32 textId)
            {
                if (Creature* computer = me->FindNearestCreature(NPC_COMPUTER, 100.0f))
                    computer->AI()->Talk(textId);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_mimironAI>(creature);
        }
};

class boss_leviathan_mk : public CreatureScript
{
    public:
        boss_leviathan_mk() : CreatureScript("boss_leviathan_mk") { }

        struct boss_leviathan_mkAI : public ScriptedAI
        {
            boss_leviathan_mkAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Reset();
            }

            void Reset() override
            {
                _events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetReactState(REACT_PASSIVE);
                me->ClearUnitState(UNIT_STATE_ROTATING);
                me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                _phase = PHASE_IDLE;
                _events.SetPhase(_phase);
                _gotMimironHardMode = false;
            }

            void JustReachedHome() override
            {
                Creature* findturret = me->FindNearestCreature(NPC_MKII_TURRET, 50.0f);
                if (!findturret)
                    if (Creature* turret = me->SummonCreature(NPC_MKII_TURRET, turretPos))
                    {
                        turret->EnterVehicle(me, 3);
                        turret->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                        turret->SetReactState(REACT_PASSIVE);
                    }
            }

            void KilledUnit(Unit* /*who*/) override
            {
                if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                {
                    if (_phase == PHASE_LEVIATHAN_SOLO__GLOBAL_1)
                    {
                        if (!urand(0,5))
                            Talk(SAY_MKII_SLAY, mimiron);
                    }
                    else
                        if (!urand(0,5))
                            Talk(SAY_V07TRON_SLAY, mimiron);
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (!caster || !spell)
                    return;

                if (spell->Id == SPELL_SELF_REPAIR)
                {
                    DoAction(DO_LEVIATHAN_SELF_REPAIR_END);
                    if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_LEVIATHAN_SELF_REPAIR_END);
                }
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    // Common stuff
                    damage = 0;

                    if (_phase == PHASE_IDLE)
                        return;

                    me->InterruptNonMeleeSpells(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    switch (_phase)
                    {
                        case PHASE_LEVIATHAN_SOLO__GLOBAL_1:
                            me->SetHealth(me->GetMaxHealth());
                            if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                Mimiron->AI()->DoAction(DO_ACTIVATE_VX001);
                            if (Creature* turret = me->GetVehicleKit()->GetPassenger(3)->ToCreature())
                            {
                                turret->Kill(turret, false);
                                turret->DespawnOrUnsummon(5000);
                            }
                            me->SetSpeed(MOVE_RUN, 1.5f, true);
                            me->GetMotionMaster()->MovePoint(0, 2790.11f, 2595.83f, 364.32f);
                            break;
                        case PHASE_LEVIATHAN_ASSEMBLED__GLOBAL_4:
                            me->SetStandState(UNIT_STAND_STATE_DEAD);
                            if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                Mimiron->AI()->DoAction(DO_LEVIATHAN_SELF_REPAIR_START);
                            DoCast(me, SPELL_SELF_REPAIR);
                            break;
                        default:
                            break;
                    }
                    _events.Reset(); // Wipe events, just for the case
                    _phase = PHASE_IDLE;
                    _events.SetPhase(_phase);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                    _gotMimironHardMode = Mimiron->AI()->GetData(DATA_GET_HARD_MODE);

                if (Creature* turret = me->GetVehicleKit()->GetPassenger(3)->ToCreature())
                {
                    turret->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                    turret->SetReactState(REACT_AGGRESSIVE);
                    turret->AI()->DoZoneInCombat();
                }

                _events.ScheduleEvent(EVENT_SHOCK_BLAST, 30*IN_MILLISECONDS, 0, PHASE_LEVIATHAN_SOLO__GLOBAL_1);
                _events.ScheduleEvent(EVENT_PROXIMITY_MINE, 0, 0, PHASE_LEVIATHAN_SOLO__GLOBAL_1);
                _events.ScheduleEvent(EVENT_PLASMA_BLAST, 10*IN_MILLISECONDS, 0, PHASE_LEVIATHAN_SOLO__GLOBAL_1);
                if (_gotMimironHardMode)
                {
                    DoCast(me, SPELL_EMERGENCY_MODE, true);
                    _events.ScheduleEvent(EVENT_FLAME_SUPPRESSANT, 1*MINUTE*IN_MILLISECONDS, 0, PHASE_LEVIATHAN_SOLO__GLOBAL_1);
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case DO_START_ENCOUNTER:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        _phase = PHASE_LEVIATHAN_SOLO__GLOBAL_1;
                        _events.SetPhase(_phase);
                        DoZoneInCombat();
                        break;
                    case DO_LEVIATHAN_ASSEMBLED:                            // Assemble and self-repair share some stuff, so the fallthrough is intended!
                        me->SetHealth((me->GetMaxHealth() >> 1));
                        // no break here
                    case DO_LEVIATHAN_SELF_REPAIR_END:
                        if (_gotMimironHardMode)
                            if (!me->HasAura(SPELL_EMERGENCY_MODE))
                                DoCast(me, SPELL_EMERGENCY_MODE, true);
                        _phase = PHASE_LEVIATHAN_ASSEMBLED__GLOBAL_4;
                        _events.SetPhase(_phase);
                        me->InterruptNonMeleeSpells(false);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                        if (!me->HasUnitState(UNIT_STATE_ROTATING))
                            me->SetReactState(REACT_AGGRESSIVE);
                        me->SetSpeed(MOVE_RUN, 1.0f, true);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        _events.RescheduleEvent(EVENT_PROXIMITY_MINE, 1*IN_MILLISECONDS, 0, PHASE_LEVIATHAN_ASSEMBLED__GLOBAL_4);
                        _events.RescheduleEvent(EVENT_SHOCK_BLAST, 3*IN_MILLISECONDS, 0, PHASE_LEVIATHAN_ASSEMBLED__GLOBAL_4);
                        break;
                    case DO_ENTER_ENRAGE:
                        DoCast(me, SPELL_BERSERK, true);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || _phase == PHASE_IDLE)
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PROXIMITY_MINE:
                            {
                                DoCast(SPELL_PROXIMITY_MINES);
                                _events.RescheduleEvent(EVENT_PROXIMITY_MINE, 35*IN_MILLISECONDS, _phase);
                            }
                            return;
                        case EVENT_PLASMA_BLAST:
                            Talk(EMOTE_PLASMA_BLAST);
                            DoCast(SPELL_PLASMA_BLAST);
                            _events.RescheduleEvent(EVENT_PLASMA_BLAST, urand(30*IN_MILLISECONDS, 35*IN_MILLISECONDS), 0, _phase);
                            _events.RescheduleEvent(EVENT_SHOCK_BLAST, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS), 0, _phase);
                            return;
                        case EVENT_SHOCK_BLAST:
                            DoCastAOE(SPELL_SHOCK_BLAST);
                            _events.RescheduleEvent(EVENT_SHOCK_BLAST, 35*IN_MILLISECONDS, 0, _phase);
                            return;
                        case EVENT_FLAME_SUPPRESSANT:
                            DoCastAOE(SPELL_FLAME_SUPPRESSANT_MK);
                            _events.RescheduleEvent(EVENT_FLAME_SUPPRESSANT, 1*MINUTE*IN_MILLISECONDS, 0, PHASE_LEVIATHAN_SOLO__GLOBAL_1);
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                Phases _phase;
                EventMap _events;
                bool _gotMimironHardMode;
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_leviathan_mkAI>(creature);
        }
};

// Levi's turret
class boss_leviathan_mk_turret : public CreatureScript
{
    public:
        boss_leviathan_mk_turret() : CreatureScript("boss_leviathan_mk_turret") { }

        struct boss_leviathan_mk_turretAI : public ScriptedAI
        {
            boss_leviathan_mk_turretAI(Creature* c) : ScriptedAI(c) { }

            void InitializeAI() override
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_DEATH_GRIP, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                Reset();
            }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                _napalmShellTimer = urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS);
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                damage = 0;
            }

            // prefer ranged targets
            Unit* GetNapalmShellTarget()
            {
                if (Map* map = me->GetMap())
                {
                    std::list<Player*> playerList;
                    for (auto&& itr : map->GetPlayers())
                    {
                        if (Player* player = itr.GetSource())
                        {
                            if (player->isDead() || player->IsGameMaster())
                                continue;

                            float Distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            if (Distance < 15.0f || Distance > 100.0f)
                                continue;

                            playerList.push_back(player);
                        }
                    }
                    if (playerList.empty())
                        return SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true);
                    else
                        return Trinity::Containers::SelectRandomContainerElement(playerList);
                }
                else
                    return 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (_napalmShellTimer <= diff)
                {
                    if (Unit* shellTarget = GetNapalmShellTarget())
                        DoCast(shellTarget, SPELL_NAPALM_SHELL);
                    _napalmShellTimer = urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS);
                }
                else
                    _napalmShellTimer -= diff;
            }

            private:
                uint32 _napalmShellTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_leviathan_mk_turretAI>(creature);
        }
};

// Npc that triggers the mine explosion
// Bomb will explode after 35 seconds or if a potential target is within half a meter
class npc_proximity_mine : public CreatureScript
{
    public:
        npc_proximity_mine() : CreatureScript("npc_proximity_mine") { }

        struct npc_proximity_mineAI : public ScriptedAI
        {
            npc_proximity_mineAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                _boomTimer = 35*IN_MILLISECONDS;
                _boomLocked = false;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (!target || !spell)
                    return;

                if (target->ToPlayer())
                    if (spell->Id == SPELL_EXPLOSION || spell->Id == SPELL_EXPLOSION_25)
                        if (InstanceScript* instance = me->GetInstanceScript())
                            if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                mimiron->AI()->DoAction(DATA_AVOIDED_PROXIMITY_MINES);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who)
                    return;

                if (Player* player = who->ToPlayer())
                    if (!player->IsGameMaster())
                        if (!_boomLocked && me->GetDistance2d(player) < 2.0f)
                        {
                            DoCastAOE(SPELL_EXPLOSION);
                            _boomLocked = true;
                            me->DespawnOrUnsummon(1*IN_MILLISECONDS);
                        }
            }

            void UpdateAI(uint32 diff) override
            {
                if (_boomTimer <= diff)
                {
                    if (!_boomLocked)
                    {
                        DoCastAOE(SPELL_EXPLOSION);
                        me->DespawnOrUnsummon(0.2*IN_MILLISECONDS);
                        _boomLocked = true;
                    }
                }
                else
                    _boomTimer -= diff;
            }

            private:
                uint32 _boomTimer;
                bool _boomLocked;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_proximity_mineAI>(creature);
        }
};

class spell_proximity_mines : public SpellScriptLoader // Spell 63027
{
    public:
        spell_proximity_mines() : SpellScriptLoader("spell_proximity_mines") { }

        class spell_proximity_mines_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_proximity_mines_SpellScript);

            void HandleDummyTick(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                {
                    for (uint8 i = 0; i < 10; i++)
                        caster->CastSpell(caster, SPELL_MINES_SPAWN, true);
                }
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_proximity_mines_SpellScript::HandleDummyTick, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_proximity_mines_SpellScript();
        }
};

class boss_vx_001 : public CreatureScript
{
    public:
        boss_vx_001() : CreatureScript("boss_vx_001") { }

        struct boss_vx_001AI : public ScriptedAI
        {
            boss_vx_001AI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Reset();
            }

            void Reset() override
            {
                _spinning = false;
                _direction = false;
                _spinTimer = 0.25*IN_MILLISECONDS;

                _events.Reset();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_DISABLE_MOVE);
                //me->SetStandState(UNIT_STAND_STATE_STAND);
                me->ClearUnitState(UNIT_STATE_STUNNED);
                me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                _phase = PHASE_IDLE;
                _events.SetPhase(_phase);
                _mimironHardMode = false;
            }

            void KilledUnit(Unit* /*who*/) override
            {
                if (instance)
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                    {
                        if (_phase == PHASE_VX001_SOLO__GLOBAL_2)
                        {
                            if (!urand(0,5))
                                Talk(SAY_VX001_SLAY, mimiron);
                        }
                        else
                            if (!urand(0,5))
                                Talk(SAY_V07TRON_SLAY, mimiron);
                    }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                    _mimironHardMode = Mimiron->AI()->GetData(DATA_GET_HARD_MODE);

                if (_mimironHardMode)
                {
                    DoCast(me, SPELL_EMERGENCY_MODE, true);
                    _events.ScheduleEvent(EVENT_FROST_BOMB, 15*IN_MILLISECONDS, 0, PHASE_VX001_SOLO__GLOBAL_2);
                    _events.ScheduleEvent(EVENT_FLAME_SUPPRESSANT_VX001, 1*IN_MILLISECONDS, 0, PHASE_VX001_SOLO__GLOBAL_2);
                }

                _events.ScheduleEvent(EVENT_RAPID_BURST, 2.5*IN_MILLISECONDS, 0, PHASE_VX001_SOLO__GLOBAL_2);
                _events.ScheduleEvent(EVENT_LASER_BARRAGE, urand(35*IN_MILLISECONDS, 40*IN_MILLISECONDS), 0, PHASE_VX001_SOLO__GLOBAL_2);
                _events.ScheduleEvent(EVENT_ROCKET_STRIKE, 20*IN_MILLISECONDS, 0, PHASE_VX001_SOLO__GLOBAL_2);
                _events.ScheduleEvent(EVENT_HEAT_WAVE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS), 0, PHASE_VX001_SOLO__GLOBAL_2);
            }

            void AttackStart(Unit* victim) override
            {
                // Do nothing, don't change target
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case DO_START_VX001:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                        _phase = PHASE_VX001_SOLO__GLOBAL_2;
                        _events.SetPhase(_phase);
                        me->SetFaction(14);
                        me->SetInCombatWithZone();
                        break;
                    case DO_VX001_ASSEMBLED:                                // Reassemble and heal share some stuff, fallthrough is intended
                        me->SetHealth( (me->GetMaxHealth() >> 1) );
                    case DO_VX001_SELF_REPAIR_END:
                        if (_mimironHardMode)
                            if (!me->HasAura(SPELL_EMERGENCY_MODE))
                                DoCast(me, SPELL_EMERGENCY_MODE, true);
                        _phase = PHASE_VX001_ASSEMBLED__GLOBAL_4;
                        _events.SetPhase(_phase);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        _events.RescheduleEvent(EVENT_LASER_BARRAGE, urand(35*IN_MILLISECONDS, 40*IN_MILLISECONDS), 0, PHASE_VX001_ASSEMBLED__GLOBAL_4);
                        _events.RescheduleEvent(EVENT_ROCKET_STRIKE, 20*IN_MILLISECONDS, 0, PHASE_VX001_ASSEMBLED__GLOBAL_4);
                        _events.RescheduleEvent(EVENT_HAND_PULSE, 5*IN_MILLISECONDS, 0, PHASE_VX001_ASSEMBLED__GLOBAL_4);
                        if (_mimironHardMode)
                            _events.RescheduleEvent(EVENT_FROST_BOMB, 15*IN_MILLISECONDS, 0, PHASE_VX001_ASSEMBLED__GLOBAL_4);
                        break;
                    case DO_ENTER_ENRAGE:
                        DoCast(me, SPELL_BERSERK, true);
                        break;
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;

                    if (_phase == PHASE_IDLE)
                        return;

                    _spinning = false;
                    me->InterruptNonMeleeSpells(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->AttackStop();
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    switch (_phase)
                    {
                        case PHASE_VX001_SOLO__GLOBAL_2:
                            me->SetHealth(me->GetMaxHealth());
                            if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                Mimiron->AI()->DoAction(DO_ACTIVATE_AERIAL);
                            break;
                        case PHASE_VX001_ASSEMBLED__GLOBAL_4:
                            if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                Mimiron->AI()->DoAction(DO_VX001_SELF_REPAIR_START);
                            DoCast(me, SPELL_SELF_REPAIR);
                            break;
                        default:
                            break;
                    }
                    _events.Reset();
                    _phase = PHASE_IDLE;
                    _events.SetPhase(_phase);
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (!caster || !spell)
                    return;

                if (spell->Id == SPELL_SELF_REPAIR)
                {
                    DoAction(DO_VX001_SELF_REPAIR_END);
                    if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_VX001_SELF_REPAIR_END);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FROSTBOMB:
                        me->SummonCreature(NPC_FROST_BOMB, *target, TEMPSUMMON_TIMED_DESPAWN, 11*IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || _phase == PHASE_IDLE)
                    return;

                _events.Update(diff);

                if (_spinning)
                {
                    if (Creature* leviathan = me->GetVehicleCreatureBase())
                    {
                        me->m_movementInfo.transport.pos.SetOrientation(me->GetOrientation() - leviathan->GetOrientation());
                        me->SendMovementFlagUpdate();
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RAPID_BURST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                if (Creature* BurstTarget = me->SummonCreature(NPC_BURST_TARGET, *target, TEMPSUMMON_TIMED_DESPAWN, 3.1*IN_MILLISECONDS))
                                {
                                    DoCast(BurstTarget, SPELL_RAPID_BURST);
                                    me->SetFacingToObject(target);
                                }
                            _events.RescheduleEvent(EVENT_RAPID_BURST, 2500, 0, PHASE_VX001_SOLO__GLOBAL_2);
                            return;
                        case EVENT_LASER_BARRAGE:
                        {
                            if (Creature* leviathan = me->GetVehicleCreatureBase())
                            {
                                leviathan->AddUnitState(UNIT_STATE_ROTATING);
                                leviathan->SetReactState(REACT_PASSIVE);
                                leviathan->SetFacingTo(leviathan->GetOrientation());
                                leviathan->SetTarget(ObjectGuid::Empty);
                            }
                            _direction = RAND(true, false);
                            _spinning = true;
                            DoCast(SPELL_SPINNING_UP); // The triggered spells should be casted on another target, seem to be self-casted 63274,66490
                            _events.DelayEvents(15*IN_MILLISECONDS);
                            _events.RescheduleEvent(EVENT_LASER_BARRAGE, 1*MINUTE*IN_MILLISECONDS, 0, _phase);
                            _events.RescheduleEvent(EVENT_LASER_BARRAGE_END, 14*IN_MILLISECONDS, 0, _phase);
                            return;
                        }
                        case EVENT_LASER_BARRAGE_END:
                            me->m_movementInfo.transport.pos.SetOrientation(0);
                            me->SendMovementFlagUpdate();
                            if (Creature* leviathan = me->GetVehicleCreatureBase())
                            {
                                leviathan->ClearUnitState(UNIT_STATE_ROTATING);
                                if (!leviathan->FindCurrentSpellBySpellId(SPELL_SELF_REPAIR))
                                    leviathan->SetReactState(REACT_AGGRESSIVE);
                                if (leviathan->GetVictim())
                                    leviathan->SetTarget(leviathan->GetVictim()->GetGUID());
                            }
                            if (me->GetVictim())
                                AttackStart(me->GetVictim());
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                AttackStart(target);
                            _spinning = false;
                            return;
                        case EVENT_ROCKET_STRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            {
                                if (Creature* pTemp = me->SummonCreature(NPC_MIMIRON_FOCUS, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                                {
                                    pTemp->SetReactState(REACT_PASSIVE);
                                    pTemp->SetDisplayFromModel(1);
                                    pTemp->GetMotionMaster()->MoveIdle();
                                    pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                    pTemp->CombatStop();
                                    pTemp->CastSpell(pTemp, SPELL_ROCKET_STRIKE_AURA, true);
                                    pTemp->DespawnOrUnsummon(6000);
                                }
                                if (Unit* missile = me->GetVehicleKit()->GetPassenger(5))
                                    missile->CastSpell(target, SPELL_ROCKET_STRIKE);
                            }
                            if (_phase == PHASE_VX001_ASSEMBLED__GLOBAL_4)
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                {
                                    if (Creature* pTemp = me->SummonCreature(NPC_MIMIRON_FOCUS, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                                    {
                                        pTemp->SetReactState(REACT_PASSIVE);
                                        pTemp->SetDisplayFromModel(1);
                                        pTemp->GetMotionMaster()->MoveIdle();
                                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                        pTemp->CombatStop();
                                        pTemp->CastSpell(pTemp, SPELL_ROCKET_STRIKE_AURA, true);
                                        pTemp->DespawnOrUnsummon(6000);
                                    }
                                    if (Unit* missile = me->GetVehicleKit()->GetPassenger(6))
                                        missile->CastSpell(target, SPELL_ROCKET_STRIKE);
                                }
                            _events.RescheduleEvent(EVENT_ROCKET_STRIKE, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS), 0, _phase);
                            return;
                        case EVENT_HEAT_WAVE:
                            DoCastAOE(SPELL_HEAT_WAVE);
                            _events.RescheduleEvent(EVENT_HEAT_WAVE, 10*IN_MILLISECONDS, 0, PHASE_VX001_SOLO__GLOBAL_2);
                            return;
                        case EVENT_HAND_PULSE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_HAND_PULSE);
                            _events.RescheduleEvent(EVENT_HAND_PULSE, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS), 0 , PHASE_VX001_ASSEMBLED__GLOBAL_4);
                            return;
                        case EVENT_FROST_BOMB:
                            if (me->FindNearestCreature(NPC_FLAME_SPREAD, 100.0f))
                            {
                                DoCast(SPELL_FROSTBOMB);
                                _events.RescheduleEvent(EVENT_FROST_BOMB, 45*IN_MILLISECONDS, 0, _phase);
                            }
                            else
                                _events.RescheduleEvent(EVENT_FROST_BOMB, 5*IN_MILLISECONDS, 0, _phase);
                            return;
                        case EVENT_FLAME_SUPPRESSANT_VX001:
                            DoCastAOE(SPELL_FLAME_SUPPRESSANT_VX001);
                            _events.RescheduleEvent(EVENT_FLAME_SUPPRESSANT_VX001, 10*IN_MILLISECONDS, 0, PHASE_VX001_SOLO__GLOBAL_2);
                            return;
                        default:
                            return;
                    }
                }
            }

            private:
                Phases _phase;
                EventMap _events;
                InstanceScript* instance;
                bool _mimironHardMode;
                bool _spinning;
                bool _direction;
                uint32 _spinTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_vx_001AI>(creature);
        }
};

class npc_rocket_strike : public CreatureScript
{
    public:
        npc_rocket_strike() : CreatureScript("npc_rocket_strike") { }

        struct npc_rocket_strikeAI : public ScriptedAI
        {
            npc_rocket_strikeAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->DespawnOrUnsummon(1);
                Reset();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_rocket_strikeAI>(creature);
        }
};

class spell_rapid_burst : public SpellScriptLoader
{
    public:
        spell_rapid_burst() : SpellScriptLoader("spell_rapid_burst") { }

        class spell_rapid_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rapid_burst_AuraScript);

            void HandleDummyTick(AuraEffect const* aurEff)
            {
                if (!GetTarget())
                    return;

                if (Unit* caster = GetCaster())
                {
                    caster->RemoveAurasDueToSpell(SPELL_HOVER);

                    switch (caster->GetMap()->GetDifficulty())
                    {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            caster->CastSpell(GetTarget(), (left = !left) ? SPELL_RAPID_BURST_LEFT_10 : SPELL_RAPID_BURST_RIGHT_10, true, nullptr, aurEff);
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            caster->CastSpell(GetTarget(), (left = !left) ? SPELL_RAPID_BURST_LEFT_25 : SPELL_RAPID_BURST_RIGHT_25, true, nullptr, aurEff);
                            break;
                        default:
                            break;
                    }

                    if (!aurEff->GetBase()->GetDuration())
                        caster->CastSpell(caster, SPELL_HOVER, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rapid_burst_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

        private:
            bool left = false;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_rapid_burst_AuraScript();
        }
};

class boss_aerial_unit : public CreatureScript
{
    public:
        boss_aerial_unit() : CreatureScript("boss_aerial_unit") { }

        struct boss_aerial_unitAI : public ScriptedAI
        {
            boss_aerial_unitAI(Creature* creature) : ScriptedAI(creature), _summons(me), _phase(PHASE_IDLE) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlying(true);
                Reset();
            }

            void Reset() override
            {
                _events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_PASSIVE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetVisible(false);
                me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                me->SetFlying(true);
                _phase = PHASE_IDLE;
                _events.SetPhase(PHASE_IDLE);
                _summons.DespawnAll();
                _gotMimironHardMode = false;
            }

            void KilledUnit(Unit* /*who*/) override
            {
                if (instance)
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                    {
                        if (_phase == PHASE_AERIAL_SOLO__GLOBAL_3)
                        {
                            if (!urand(0,5))
                                Talk(SAY_AERIAL_SLAY, mimiron);
                        }
                        else
                            if (!urand(0,5))
                                Talk(SAY_V07TRON_SLAY, mimiron);
                    }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (!caster || !spell)
                    return;

                if (spell->Id == SPELL_SELF_REPAIR)
                {
                    DoAction(DO_AERIAL_SELF_REPAIR_END);
                    if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_AERIAL_SELF_REPAIR_END);
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                uint32 aura = 0;
                switch (spell->Id)
                {
                    case SPELL_SUMMON_SCRAP_BOT_TRIGGER_JUNK_BOT:    aura = SPELL_SUMMON_SCRAP_BOT_AURA_JUNK_BOT;    break;
                    case SPELL_SUMMON_SCRAP_BOT_TRIGGER_ASSAULT_BOT: aura = SPELL_SUMMON_SCRAP_BOT_AURA_ASSAULT_BOT; break;
                    case SPELL_SUMMON_FIRE_BOT_TRIGGER:              aura = SPELL_SUMMON_FIRE_BOT_AURA;              break;
                    default:
                        return;
                }
                target->CastSpell(target, aura, true);
                for (uint32 id = 194740; id <= 194748; ++id)
                {
                    if (GameObject* go = target->FindNearestGameObject(id, 5.0f))
                    {
                        go->UseDoorOrButton();
                        break;
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                    _gotMimironHardMode = Mimiron->AI()->GetData(DATA_GET_HARD_MODE);

                if (_gotMimironHardMode)
                    DoCast(me, SPELL_EMERGENCY_MODE, true);
                
                _events.ScheduleEvent(EVENT_PLASMA_BALL, 1*IN_MILLISECONDS);
                _events.ScheduleEvent(EVENT_SUMMON_JUNK_BOT,    10 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                _events.ScheduleEvent(EVENT_SUMMON_FIRE_BOT,     2 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                _events.ScheduleEvent(EVENT_SUMMON_ASSAULT_BOT,  7 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                _events.ScheduleEvent(EVENT_SUMMON_BOOM_BOT,    11 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
            }

            void AttackStart(Unit* target) override
            {
                AttackStartCaster(target, 100.0f);
            }

            void DoAction(int32 action) override
            {
                Position destination;
                switch (action)
                {
                    case DO_START_AERIAL:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_IMMUNE_TO_PC);
                        me->SetReactState(REACT_AGGRESSIVE);
                        _phase = PHASE_AERIAL_SOLO__GLOBAL_3;
                        _events.SetPhase(PHASE_AERIAL_SOLO__GLOBAL_3);
                        DoZoneInCombat();
                        break;
                    case DO_DISABLE_AERIAL:
                        if (_phase == PHASE_AERIAL_SOLO__GLOBAL_3)
                        {
                            me->CastStop();
                            me->SetReactState(REACT_PASSIVE);
                            me->GetMotionMaster()->Clear();
                            // check if the aura works correctly (+50% damage from all sources)
                            DoCast(me, SPELL_MAGNETIC_CORE);
                            DoCast(me, SPELL_MAGNETIC_CORE_VISUAL);
                            // Move to floor.
                            destination = me->GetPosition();
                            destination.m_positionZ = 368.965f;
                            me->GetMotionMaster()->MoveLand(1, destination, 5.0f);  // Check if MoveLand is ok here, a flying unit should have a landing animation, but... just 4 the case
                            _events.DelayEvents(20*IN_MILLISECONDS);
                            _events.ScheduleEvent(EVENT_REACTIVATE_AERIAL, 20*IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                        }
                        break;
                    case DO_AERIAL_ASSEMBLED:
                        me->SetHealth( (me->GetMaxHealth() >> 1) );               // Once again, re-assemble and repairing share some stuff, so the fallthrough is intended!                        
                    case DO_AERIAL_SELF_REPAIR_END:
                        if (_gotMimironHardMode)
                            if (!me->HasAura(SPELL_EMERGENCY_MODE))
                                DoCast(me, SPELL_EMERGENCY_MODE, true);
                        _phase = PHASE_AERIAL_ASSEMBLED__GLOBAL_4;
                        _events.SetPhase(_phase);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        _events.RescheduleEvent(EVENT_PLASMA_BALL, 2*IN_MILLISECONDS);
                        break;
                    case DO_ENTER_ENRAGE:
                        DoCast(me, SPELL_BERSERK, true);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim() || _phase == PHASE_IDLE)
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    Position destination;
                    switch (eventId)
                    {
                        case EVENT_PLASMA_BALL:
                            if (_phase == PHASE_AERIAL_SOLO__GLOBAL_3 && me->GetVictim())
                            {
                                float x = me->GetVictim()->GetPositionX();
                                float y = me->GetVictim()->GetPositionY();
                                float z = me->GetVictim()->GetPositionZ();
                                if (me->IsWithinDist3d(x, y, z, 30.0f))
                                {
                                    me->GetMotionMaster()->Initialize();
                                    DoCastVictim(SPELL_PLASMA_BALL);
                                }
                                else
                                    me->GetMotionMaster()->MovePoint(0, x, y, 380.04f);
                            }
                            else if (_phase == PHASE_AERIAL_ASSEMBLED__GLOBAL_4 && me->GetVictim())
                            {
                                if (me->GetVictim()->IsWithinDist3d(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 30.0f))
                                    DoCastVictim(SPELL_PLASMA_BALL);
                                else if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST, 0, 0.0f, true))
                                    DoCast(target, SPELL_PLASMA_BALL);
                            }
                            _events.RescheduleEvent(EVENT_PLASMA_BALL, urand(3000, 5000));
                            return;
                        case EVENT_REACTIVATE_AERIAL:
                            me->RemoveAurasDueToSpell(SPELL_MAGNETIC_CORE_VISUAL);
                            destination = me->GetPosition();
                            destination.m_positionZ = 380.04f;
                            // FIXME find correct speed
                            me->GetMotionMaster()->MoveTakeoff(1, destination, 5.0f); // Check if MoveTakeoff is ok here, a flying unit should have a landing animation, but... just 4 the case
                            me->SetReactState(REACT_AGGRESSIVE);
                            return;
                        case EVENT_SUMMON_JUNK_BOT:
                            me->CastCustomSpell(SPELL_SUMMON_SCRAP_BOT_TRIGGER_JUNK_BOT, SPELLVALUE_MAX_TARGETS, 1, me, true);
                            _events.RescheduleEvent(EVENT_SUMMON_JUNK_BOT, urand(10 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                            return;
                        case EVENT_SUMMON_ASSAULT_BOT:
                            me->CastCustomSpell(SPELL_SUMMON_SCRAP_BOT_TRIGGER_ASSAULT_BOT, SPELLVALUE_MAX_TARGETS, 1, me, true);
                            _events.RescheduleEvent(EVENT_SUMMON_ASSAULT_BOT, 30 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                            return;
                        case EVENT_SUMMON_BOOM_BOT:
                            DoCast(me, SPELL_BOMB_BOT, true);
                            _events.RescheduleEvent(EVENT_SUMMON_BOOM_BOT, 15 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                            return;
                        case EVENT_SUMMON_FIRE_BOT:
                            if (_gotMimironHardMode)
                                me->CastCustomSpell(SPELL_SUMMON_FIRE_BOT_TRIGGER, SPELLVALUE_MAX_TARGETS, 3, me, true);
                            _events.RescheduleEvent(EVENT_SUMMON_FIRE_BOT, 45 * IN_MILLISECONDS, 0, PHASE_AERIAL_SOLO__GLOBAL_3);
                            return;
                        default:
                            return;
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                _summons.Summon(summon);

                if (summon->GetEntry() == NPC_BOMB_BOT || summon->GetEntry() == NPC_FIRE_BOT)
                    return;

                DoZoneInCombat(summon, 100.0f);
                if (_gotMimironHardMode)
                    summon->CastSpell(summon, SPELL_EMERGENCY_MODE, true);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;

                    if (_phase == PHASE_IDLE)
                        return;

                    me->InterruptNonMeleeSpells(true);
                    me->GetMotionMaster()->Clear(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    switch (_phase)
                    {
                        case PHASE_AERIAL_SOLO__GLOBAL_3:
                            me->SetHealth(me->GetMaxHealth()); 
                            if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                Mimiron->AI()->DoAction(DO_ACTIVATE_V0L7R0N);
                            break;
                        case PHASE_AERIAL_ASSEMBLED__GLOBAL_4:
                            if (Creature* Mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                Mimiron->AI()->DoAction(DO_AERIAL_SELF_REPAIR_START);
                            DoCast(me, SPELL_SELF_REPAIR);
                            break;
                        default:
                            break;
                    }
                    _events.Reset();
                    _phase = PHASE_IDLE;
                    _events.SetPhase(PHASE_IDLE);
                }
            }

            private:
                Phases _phase;
                EventMap _events;
                SummonList _summons;
                InstanceScript* instance;
                bool _gotMimironHardMode;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_aerial_unitAI>(creature);
        }
};

class npc_magnetic_core : public CreatureScript
{
    public:
        npc_magnetic_core() : CreatureScript("npc_magnetic_core") { }

        struct npc_magnetic_coreAI : public ScriptedAI
        {
            npc_magnetic_coreAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->DespawnOrUnsummon(21*IN_MILLISECONDS);
                if (Creature* AerialUnit = me->FindNearestCreature(NPC_AERIAL_COMMAND_UNIT, 100.0f, true))
                {
                    AerialUnit->AI()->DoAction(DO_DISABLE_AERIAL); // Causes the NPC to land
                    // me->GetMotionMaster()->MoveFall();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_magnetic_coreAI>(creature);
        }
};

class npc_junk_bot : public CreatureScript
{
    public:
        npc_junk_bot() : CreatureScript("npc_junk_bot") { }

        struct npc_junk_botAI : public ScriptedAI
        {
            npc_junk_botAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* summoner) override
            {
                if (me->GetInstanceScript()->GetBossState(BOSS_MIMIRON) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                DoCast(me, 65256, true); // Self Stun (1 sec)
                if (Creature* aerialCommandUnit = me->FindNearestCreature(NPC_AERIAL_COMMAND_UNIT, 100.0f, true))
                    aerialCommandUnit->AI()->JustSummoned(me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_junk_botAI>(creature);
        }
};

class npc_assault_bot : public CreatureScript
{
    public:
        npc_assault_bot() : CreatureScript("npc_assault_bot") { }

        struct npc_assault_botAI : public ScriptedAI
        {
            npc_assault_botAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _fieldTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (me->GetInstanceScript()->GetBossState(BOSS_MIMIRON) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                DoCast(me, 65256, true); // Self Stun (1 sec)
                if (Creature* aerialCommandUnit = me->FindNearestCreature(NPC_AERIAL_COMMAND_UNIT, 100.0f, true))
                    aerialCommandUnit->AI()->JustSummoned(me);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (_fieldTimer <= diff)
                {
                    DoCastVictim(SPELL_MAGNETIC_FIELD);
                    _fieldTimer = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                }
                else
                    _fieldTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                // Achievement Not-So-Friendly Fire
                if (spell->Id == SPELL_ROCKET_STRIKE_DMG)
                    if (Player* player = me->SelectNearestPlayer(100.0f))
                        player->CastSpell(player, SPELL_NOT_SO_FRIENDLY_FIRE, true);
            }

            private:
                uint32 _fieldTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_assault_botAI>(creature);
        }
};

class npc_emergency_bot : public CreatureScript
{
    public:
        npc_emergency_bot() : CreatureScript("npc_emergency_bot") { }

        struct npc_emergency_botAI : public ScriptedAI
        {
            npc_emergency_botAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                me->SetFaction(14);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_DEATH_GRIP, true);
                me->SetReactState(REACT_PASSIVE);
                Reset();
            }

            void Reset() override
            {
                me->GetMotionMaster()->MoveRandom(15);
                _sprayTimer = 5*IN_MILLISECONDS;

                if (Is25ManRaid())
                    DoCast(me, SPELL_DEAFENING_SIREN, true);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (me->GetInstanceScript()->GetBossState(BOSS_MIMIRON) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                DoCast(me, 65256, true); // Self Stun (1 sec)
                if (Creature* aerialCommandUnit = me->FindNearestCreature(NPC_AERIAL_COMMAND_UNIT, 100.0f, true))
                    aerialCommandUnit->AI()->JustSummoned(me);
            }

            void UpdateAI(uint32 diff) override
            {
                if (_sprayTimer <= diff)
                {
                    DoCast(SPELL_WATER_SPRAY);
                    _sprayTimer = 10*IN_MILLISECONDS;
                }
                else
                    _sprayTimer -= diff;
            }

            private:
                uint32 _sprayTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_emergency_botAI>(creature);
        }
};

class npc_mimiron_bomb_bot : public CreatureScript
{
    public:
        npc_mimiron_bomb_bot() : CreatureScript("npc_mimiron_bomb_bot") { }

        struct npc_mimiron_bomb_botAI : public ScriptedAI
        {
            npc_mimiron_bomb_botAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->SetReactState(REACT_PASSIVE);
                _despawn = false;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (me->GetInstanceScript()->GetBossState(BOSS_MIMIRON) != IN_PROGRESS)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (Creature* aerialCommandUnit = me->FindNearestCreature(NPC_AERIAL_COMMAND_UNIT, 100.0f, true))
                    aerialCommandUnit->AI()->JustSummoned(me);

                me->m_Events.Schedule(3000, [this]()
                {
                    Position pos;
                    pos.Relocate(me);
                    //me->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ, 0, 100.0f);
                    me->UpdateAllowedPositionZ(pos.GetPositionX(), pos.GetPositionY(), pos.m_positionZ);
                    me->UpdatePosition(pos);
                    DoCast(me, SPELL_BOOM_BOT, true);
                    DoZoneInCombat(me, 100.0f);
                    if (Unit* target = me->FindNearestPlayer(100.0f))
                    {
                        AttackStart(target);
                        me->AddThreat(target, 100000000.0f);
                        me->GetMotionMaster()->MoveFollow(target, 0.01f, 0.0f);
                    }
                });
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (!target || !spell)
                    return;

                if (target->ToPlayer())
                    if (spell->Id == SPELL_BOOM_BOT || spell->Id == SPELL_BOOM_BOT_PERIODIC)
                        if (InstanceScript* instance = me->GetInstanceScript())
                            if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_MIMIRON)))
                                mimiron->AI()->DoAction(DATA_AVOIDED_BOOM_BOT_EXPLOSION);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_BOOM_BOT_PERIODIC)
                    me->DespawnOrUnsummon(1*IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_BOOM_BOT, true);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;

                // suicide has procflag PROC_FLAG_DONE_MELEE_AUTO_ATTACK, they have to melee, even tho the spell is delayed if the npc misses
                DoMeleeAttackIfReady();
            }

            private:
                bool _despawn;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mimiron_bomb_botAI>(creature);
        }
};

class go_not_push_button : public GameObjectScript
{
    public:
        go_not_push_button() : GameObjectScript("go_mimiron_hard_mode_activator") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (go->HasFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE))
                    return false;

                if ((instance->GetBossState(BOSS_MIMIRON) != IN_PROGRESS && instance->GetBossState(BOSS_MIMIRON) != DONE) && player)
                    if (Creature* mimiron = ObjectAccessor::GetCreature((*player), instance->GetGuidData(BOSS_MIMIRON)))
                    {
                        mimiron->AI()->DoAction(DO_ACTIVATE_HARD_MODE);
                        go->UseDoorOrButton();
                        return true;
                    }
            }
            return false;
        }
};

class npc_mimiron_flame_trigger : public CreatureScript
{
    public:
        npc_mimiron_flame_trigger() : CreatureScript("npc_mimiron_flame_trigger") { }

        struct npc_mimiron_flame_triggerAI : public ScriptedAI
        {
            npc_mimiron_flame_triggerAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                Reset();
            }

            void Reset() override
            {
                _flameTimer = 2*IN_MILLISECONDS;
                me->SetInCombatWithZone();
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FLAME_SUPPRESSANT_MK:
                    case SPELL_FLAME_SUPPRESSANT_VX001:
                    case SPELL_FROST_BOMB_EXPLOSION_10:
                    case SPELL_FROST_BOMB_EXPLOSION_25:
                    case SPELL_WATER_SPRAY:
                        _flameTimer = 1*IN_MILLISECONDS;
                        me->DespawnOrUnsummon(0.5*IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* /*summon*/) override
            {
                if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                    mimiron->AI()->DoAction(DO_INCREASE_FLAME_COUNT);
            }

            void UpdateAI(uint32 diff) override
            {
                if (_flameTimer <= diff)
                {
                    // check if flame cap is reached
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                        if (mimiron->AI()->GetData(DATA_FLAME_COUNT) >= FLAME_CAP)
                        {
                            me->DespawnOrUnsummon();
                            return;
                        }

                    DoZoneInCombat(me, 100.0f);

                    if (Player* nearest = me->SelectNearestPlayer(100.0f))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveFollow(nearest, 0.0f, 0.0f);
                    }

                    if (GetClosestCreatureWithEntry(me, NPC_FLAME_SPREAD, 1.0f))
                        _flameTimer = 1*IN_MILLISECONDS;
                    else
                    {
                        me->SummonCreature(NPC_FLAME_SPREAD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                        _flameTimer = 4*IN_MILLISECONDS;
                    }
                }
                else
                    _flameTimer -= diff;
            }

            private:
                InstanceScript* instance;
                uint32 _flameTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mimiron_flame_triggerAI>(creature);
        }
};

class npc_mimiron_flame_spread : public CreatureScript
{
    public:
        npc_mimiron_flame_spread() : CreatureScript("npc_mimiron_flame_spread") { }

        struct npc_mimiron_flame_spreadAI : public ScriptedAI
        {
            npc_mimiron_flame_spreadAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_PASSIVE);
                Reset();
            }

            void Reset() override
            {
                DoCast(me, SPELL_FLAME, true);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                switch (spell->Id)
                {
                    case SPELL_FLAME_SUPPRESSANT_MK:
                    case SPELL_FLAME_SUPPRESSANT_VX001:
                    case SPELL_FROST_BOMB_EXPLOSION_10:
                    case SPELL_FROST_BOMB_EXPLOSION_25:
                    case SPELL_WATER_SPRAY:
                        if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(BOSS_MIMIRON) : ObjectGuid::Empty))
                            mimiron->AI()->DoAction(DO_DECREASE_FLAME_COUNT);
                        me->DespawnOrUnsummon(0.5*IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (instance && instance->GetBossState(BOSS_MIMIRON) != IN_PROGRESS)
                    me->DespawnOrUnsummon();
            }

            private:
                InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mimiron_flame_spreadAI>(creature);
        }
};

class npc_frost_bomb : public CreatureScript
{
    public:
        npc_frost_bomb() : CreatureScript("npc_frost_bomb") { }

        struct npc_frost_bombAI : public ScriptedAI
        {
            npc_frost_bombAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                Reset();
            }

            void Reset() override
            {
                DoCast(me, SPELL_FROST_BOMB_VISUAL, true);
                _frostTimer = 10*IN_MILLISECONDS;
            }

            void UpdateAI(uint32 diff) override
            {
                if (_frostTimer <= diff)
                {
                    DoCast(me, RAID_MODE(SPELL_FROST_BOMB_EXPLOSION_10, SPELL_FROST_BOMB_EXPLOSION_25), true);
                    _frostTimer = 10*IN_MILLISECONDS;
                }
                else
                    _frostTimer -= diff;
            }

            private:
                uint32 _frostTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_frost_bombAI>(creature);
        }
};

class npc_mimiron_db_target : public CreatureScript
{
    public:
        npc_mimiron_db_target() : CreatureScript("npc_mimiron_db_target") { }

        struct npc_mimiron_db_targetAI : public PassiveAI
        {
            npc_mimiron_db_targetAI(Creature* creature) : PassiveAI(creature) { }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                damage = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override { }
            void EnterEvadeMode() override { }
            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_mimiron_db_targetAI>(creature);
        }
};

class spell_frost_bomb : public SpellScriptLoader
{
    public:
        spell_frost_bomb() : SpellScriptLoader("spell_frost_bomb") { }

        class spell_frost_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frost_bomb_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (WorldObject* _target = Trinity::Containers::SelectRandomContainerElement(targets))
                {
                    targets.clear();
                    targets.push_back(_target);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_frost_bomb_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_frost_bomb_SpellScript();
        }
};

class spell_rocket_strike : public SpellScriptLoader
{
    public:
        spell_rocket_strike() : SpellScriptLoader("spell_rocket_strike") { }

        class spell_rocket_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rocket_strike_SpellScript);

            void HandleHitTarget(SpellEffIndex /*effIndex*/)
            {
                if (GetHitPlayer())
                    if (Unit* caster = GetCaster())
                        if (InstanceScript* instance = caster->GetInstanceScript())
                            if (Creature* mimiron = ObjectAccessor::GetCreature(*caster, instance->GetGuidData(BOSS_MIMIRON)))
                                mimiron->AI()->DoAction(DATA_AVOIDED_ROCKET_STRIKES);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_rocket_strike_SpellScript::HandleHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_rocket_strike_SpellScript();
        }
};

class spell_p3wx2_laser_barrage : public SpellScriptLoader
{
    public:
        spell_p3wx2_laser_barrage() : SpellScriptLoader("spell_p3wx2_laser_barrage") { }

        class spell_p3wx2_laser_barrage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_p3wx2_laser_barrage_SpellScript);

            void SelectDest(SpellDestination& dest)
            {
                if (Unit* caster = GetCaster())
                    dest._position.m_positionZ = caster->GetPositionZ();
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_p3wx2_laser_barrage_SpellScript::SelectDest, EFFECT_0, TARGET_DEST_CASTER_FRONT);
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_p3wx2_laser_barrage_SpellScript::SelectDest, EFFECT_1, m_scriptSpellId == 63297 ? TARGET_DEST_DEST_LEFT : TARGET_DEST_DEST_RIGHT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_p3wx2_laser_barrage_SpellScript();
        }
};

class achievement_set_up_us_the_bomb__proximity_mines : public AchievementCriteriaScript
{
    public:
        achievement_set_up_us_the_bomb__proximity_mines(const char* name) : AchievementCriteriaScript(name) { }

        bool OnCheck(Player* player, Unit* /*target*/)
        {
            if (player)
                if (InstanceScript* instance = player->GetInstanceScript())
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*player, instance->GetGuidData(BOSS_MIMIRON)))
                        return (mimiron->AI()->GetData(DATA_AVOIDED_PROXIMITY_MINES));
            return false;
        }
};

class achievement_set_up_us_the_bomb__rocket_strikes : public AchievementCriteriaScript
{
    public:
        achievement_set_up_us_the_bomb__rocket_strikes(const char* name) : AchievementCriteriaScript(name) { }

        bool OnCheck(Player* player, Unit* /*target*/)
        {
            if (player)
                if (InstanceScript* instance = player->GetInstanceScript())
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*player, instance->GetGuidData(BOSS_MIMIRON)))
                        return (mimiron->AI()->GetData(DATA_AVOIDED_ROCKET_STRIKES));
            return false;
        }
};

class achievement_set_up_us_the_bomb__boombot_explosion : public AchievementCriteriaScript
{
    public:
        achievement_set_up_us_the_bomb__boombot_explosion(const char* name) : AchievementCriteriaScript(name) { }

        bool OnCheck(Player* player, Unit* /*target*/)
        {
            if (player)
                if (InstanceScript* instance = player->GetInstanceScript())
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*player, instance->GetGuidData(BOSS_MIMIRON)))
                        return (mimiron->AI()->GetData(DATA_AVOIDED_BOOM_BOT_EXPLOSION));
            return false;
        }
};

void AddSC_boss_mimiron()
{
    new boss_mimiron();
    new boss_leviathan_mk();
    new boss_leviathan_mk_turret();
    new boss_vx_001();
    new boss_aerial_unit();

    new npc_proximity_mine();
    new npc_rocket_strike();
    new npc_magnetic_core();
    new npc_junk_bot();
    new npc_assault_bot();
    new npc_emergency_bot();
    new npc_mimiron_bomb_bot();
    new npc_mimiron_flame_trigger();
    new npc_mimiron_flame_spread();
    new npc_frost_bomb();
    new npc_mimiron_db_target();

    new spell_rapid_burst();
    new spell_proximity_mines();
    new spell_frost_bomb();
    new spell_rocket_strike();
    new spell_p3wx2_laser_barrage();

    new go_not_push_button();
}
