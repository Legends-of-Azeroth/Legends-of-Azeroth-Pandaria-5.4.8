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
#include "CreatureTextMgr.h"
#include "MoveSplineInit.h"
#include "Weather.h"
#include "heart_of_fear.h"

enum Yells
{
    // Blade Lord Ta'yak
    SAY_PAST_INTRO       = 0,    // They approach? Good. Now, if only my students were here to see and learn from the coming display of martial prowess...
    SAY_AGGRO            = 1,    // On your guard, invaders. I, Ta'yak, Lord of Blades, will be your opponent.
    SAY_DEATH            = 2,    // I'd always hoped to cut someone like that someday, to hear that sound. But to have it happen to my own prothorax is ridiculous.
    SAY_SLAY             = 3,    // 0 - A perfect cut.; 1 - This is the technique of a Blade Lord.
    SAY_KILL_TRASH       = 4,    // 0 - Mere initiates.; 1 - They should have studied harder.; 2 - One technique alone is not enough.; 3 - They failed their test. Will you?
    SAY_STORM_UNLEASHED  = 5,    // Can you follow my blade?
    SAY_INTRO            = 6,
    ANN_UNSEEN           = 7,    // Blade Lord Ta'yak marks $n for [Unseen Strike]!
};

enum Spells
{
    // Intro
    SPELL_VISINTRO_TAYAK        = 128788,
    // Tempest Slash - Launches a Tornado towards a player location; upon reaching it, tornado spins around at the spot.
    SPELL_TEMP_SLASH_SUMM_V     = 122842,   // Summons Heart of Fear - Armsmaster Ta'yak Tempest Stalker (LTD)
    SPELL_TEMP_SLASH_AURA       = 122854,   // Visual + Periodic trigger aura for SPELL_TEMP_SLASH_DAMAGE.
    SPELL_TEMP_SLASH_DAMAGE     = 122853,   // Aura Damage + Knock back
    SPELL_TEMP_SLASH_DBM        = 122839,

    // Unseen Strike - Boss disappears, appears at a player, massive damage split between targets in 15 yards cone.
    SPELL_UNSEEN_STRIKE_TR      = 122949,   // Unattackable + Speed 200%. Triggers SPELL_UNSEEN_STRIKE_DMG after 5 secs, SPELL_UNSEEN_STRIKE_MKR on target, SPELL_UNSEEN_STRIKE_INV on self.
    SPELL_UNSEEN_STRIKE_MKR     = 123017,   // Target marker visual aura.

    // Wind Step - Teleports to a player, casts the bleed, teleports back.
    SPELL_WIND_STEP_TP          = 123175,   // Teleport. Triggers SPELL_WIND_STEP_DUMMY.
    SPELL_WIND_STEP_B_DMG       = 123180,   // Bleed damage for 8y targets.
    SPELL_WIND_STEP_TP_BACK     = 123460,   // Teleport back to the main target.
    SPELL_WIND_STEP_EFF         = 123459,

    // Intensify - Every 60 seconds Phase 1 / 10 seconds Phase 2 (But no melee).
    SPELL_INTENSIFY_BUFF        = 123471,
    SPELL_INTENSIFY_PERIODIC_60 = 123470,
    SPELL_INTENSIFY_PERIODIC_10 = 132254,

    // Overwhelming Assault.
    SPELL_OVERWHELMING_ASS      = 123474,

    // Blade tempest - Spins and pulls all players. Heroic ONLY. - Every 60 seconds.
    SPELL_BLADE_TEMPEST_AUR     = 125310, // Triggers SPELL_BLADE_TEMPEST_DMG each 0.5s, SPELL_BLADE_TEMPEST_AT.
    SPELL_BLADE_TEMPES_J_FC     = 125325, // Force Cast SPELL_BLADE_TEMPES_JUMP in 200 yards.
    SPELL_BLADE_TEMPEST_AT      = 125301, // AreaTrigger for ApplyMovementForce? Triggered by 125310
    SPELL_BLADE_TEMPEST_PULL    = 125327, // Grip All players to Tayak

    // Storm Unleashed - 20 % on one end, 10% on the opposite.
    SPELL_STORM_UNLEASHED_D     = 123814, // Boss Dummy Visual.
    SPELL_SU_AURA               = 123598, // Aura for the tornadoes, triggers SPELL_SU_RV_SE each 0.1 secs.
    SPELL_SU_RV                 = 123599, // Control Vehicle aura.
    SPELL_SU_DUMMY_VIS          = 124024, // Some dummy visual (for tornadoes?).
    SPELL_SU_DMG_AURA           = 124785, // Triggers SPELL_SU_DMG every 1 sec.
    SPELL_SU_WIND_GALE          = 123633,
    SPELL_SU_DUMMY_CRAP         = 123616, // Applies a dummy aura on a target.
    SPELL_SU_JUMP               = 123805, // triggered by tayak, pull players to center
    SPELL_SU_SUMM_VEH           = 124025, // summ vehicle stalker by each player
    SPELL_SU_VEH_DUMMY_VIS      = 124024, // small tornado anim on veh stalker.
    SPELL_SU_SUMM_WEST1_TORNADO = 123597,
    SPELL_SU_SUMM_WEST2_TORNADO = 123639,
    SPELL_SU_SUMM_WEST3_TORNADO = 123640,
    SPELL_SU_SUMM_EAST1_TORNADO = 123643,
    SPELL_SU_SUMM_EAST2_TORNADO = 123644,
    SPELL_SU_SUMM_EAST3_TORNADO = 123645,

    SPELL_TAYAK_BERSERK         = 26662,  // Enrage, 490 seconds, or 8:10 minutes.
    SPELL_GALE_FORCE_WINDS_T    = 123497,

    // Achievement Misc
    SPELL_BRAZIER_FIRE          = 128968,
    SPELL_FLAME_BEARER          = 128949, // required for click to brazier
};

enum Events
{
    // Blade Lord Ta'yak
    EVENT_TEMPEST_SLASH = 1,        // 10 seconds from pull. Every 15.5 seconds.
    EVENT_UNSEEN_STRIKE,            // 30.5 seconds from pull. Every 53 - 61 seconds.
    EVENT_UNSEEN_STRIKE_CHASE,
    EVENT_UNSEEN_STRIKE_RETURN,
    EVENT_UNSEEN_STRIKE_PREPARE,
    EVENT_TAYAK_WIND_STEP,          // 20.5 seconds from pull. Every 25 seconds.
    EVENT_WIND_STEP_RETURN,
    EVENT_OVERWHELMING_ASS,         // 15.5 seconds from pull. Every 20.5 seconds, delayable by up to 15 seconds.
    EVENT_INTENSIFY,

    EVENT_BLADE_TEMPEST,            // Every 60 seconds. Heroic only.

    EVENT_STORM_UNLEASHED,          // 20%
    EVENT_SUMMON_TORNADOES,

    EVENT_TAYAK_BERSERK,            // Enrage at 8 minutes, or, more precisely, 490 seconds.
};

enum TayakPhases
{
    PHASE_NORMAL          = 1,
    PHASE_STORM_UNLEASHED = 2
};

#define NPC_US_TORNADO 63278

enum GWStalkerActions
{
    ACTION_WIND      = 1,
    ACTION_STOP_WIND = 2,
    ACTION_TORNADO   = 3,
};

class boss_tayak : public CreatureScript
{
    public:
        boss_tayak() : CreatureScript("boss_tayak") { }

        struct boss_tayakAI : public BossAI
        {
            boss_tayakAI(Creature* creature) : BossAI(creature, DATA_TAYAK) { }

            ObjectGuid unseenTank, unseenTarget, currentTank, targetGUID;
            bool entranceDone, introDone, storm1Done, unseenReturn, evadeModeEnabled, m_rightSide;
            uint8 Phase;    // 0 - Phase 1 | 1 - TP Players | 2 - Players have been TP
            uint32 delay;
            uint32 braziersCount;
            std::list<ObjectGuid> playerGuids;
            uint32 PrevSideTornado, spellTornado;
            std::map<uint32, uint32> m_TornadoType;

            void InitializeAI() override 
            {
                entranceDone     = false;
                introDone        = false;
                Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);

                delay = 0;
                me->m_Events.Schedule(delay += 2500, 20, [this]()
                {
                    // If pre event has Done
                    if (instance->GetData(DATA_TAYAK) > DONE && me->IsAlive())
                    {
                        me->AI()->DoAction(ACTION_TAYAK_PREEVENT_DONE);
                        me->GetMotionMaster()->MoveTargetedHome();
                    }
                });
            }

            void Reset() override
            {
                _Reset();
                me->SetReactState(REACT_AGGRESSIVE);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                unseenTank       = ObjectGuid::Empty;
                PrevSideTornado  = 0;
                spellTornado     = 0;
                currentTank      = ObjectGuid::Empty;
                targetGUID       = ObjectGuid::Empty;
                braziersCount    = 0;
                storm1Done       = false;
                unseenReturn     = false;
                m_rightSide      = false;
                evadeModeEnabled = false;
                Phase            = PHASE_NORMAL;
                me->CastSpell(me, SPELL_VISINTRO_TAYAK, true);
                playerGuids.clear();
                ActivateGaleWinds(ACTION_STOP_WIND);
                me->SetSpeed(MOVE_WALK, 1.0f); // from db
                me->SetSpeed(MOVE_RUN, 1.14286f);

                // Init Tornado Type container
                m_TornadoType.insert(std::pair<uint32, uint32>(NPC_STORM_UNLEASHED_STALKER_WEST1, SPELL_SU_SUMM_WEST1_TORNADO));
                m_TornadoType.insert(std::pair<uint32, uint32>(NPC_STORM_UNLEASHED_STALKER_WEST2, SPELL_SU_SUMM_WEST2_TORNADO));
                m_TornadoType.insert(std::pair<uint32, uint32>(NPC_STORM_UNLEASHED_STALKER_WEST3, SPELL_SU_SUMM_WEST3_TORNADO));
                m_TornadoType.insert(std::pair<uint32, uint32>(NPC_STORM_UNLEASHED_STALKER_EAST1, SPELL_SU_SUMM_EAST1_TORNADO));
                m_TornadoType.insert(std::pair<uint32, uint32>(NPC_STORM_UNLEASHED_STALKER_EAST2, SPELL_SU_SUMM_EAST2_TORNADO));
                m_TornadoType.insert(std::pair<uint32, uint32>(NPC_STORM_UNLEASHED_STALKER_EAST3, SPELL_SU_SUMM_EAST3_TORNADO));

                // Movement Type
                me->OverrideInhabitType(INHABIT_GROUND);
                me->UpdateMovementFlags();

                me->GetMap()->SetWorldState(WORLDSTATE_CANDLE_IN_THE_WIND, 0);
            }

            void EnterEvadeMode() override
            {
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SetBossState(DATA_TAYAK, FAIL);

                    if (GameObject* entranceDoor = instance->instance->GetGameObject(instance->GetGuidData(GO_QUARTERS_DOOR_ENTRANCE)))
                        entranceDoor->SetGoState(GO_STATE_ACTIVE);

                    // Remove fire from candles and makes them unclickable
                    std::list<Creature*> candles;
                    GetCreatureListWithEntryInGrid(candles, me, NPC_BURNING_BRAZIER, 400.0f);
                    GetCreatureListWithEntryInGrid(candles, me, NPC_BRAZIER, 400.0f);

                    for (auto&& itr : candles)
                    {
                        itr->AI()->Reset();
                        itr->RemoveAurasDueToSpell(SPELL_BRAZIER_FIRE);
                        itr->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    }
                }

                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_BRAZIERS && ++braziersCount > 3)
                    me->GetMap()->SetWorldState(WORLDSTATE_CANDLE_IN_THE_WIND, 1);
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_TAYAK, FAIL);
            }

            void JustEngagedWith(Unit* who) override
            {
                if (!instance->CheckRequiredBosses(DATA_TAYAK, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    who->NearTeleportTo(EncountersEntrance[0].GetPositionX(), EncountersEntrance[0].GetPositionY(), EncountersEntrance[0].GetPositionZ(), EncountersEntrance[0].GetOrientation());
                    return;
                }
                // prevent type when tayak do it each time after unseen strike
                if (instance->GetBossState(DATA_TAYAK) == IN_PROGRESS)
                    return;

                _JustEngagedWith();
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_TEMPEST_SLASH, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_UNSEEN_STRIKE, 30.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_TAYAK_WIND_STEP, urand(19500, 21500));
                events.ScheduleEvent(EVENT_OVERWHELMING_ASS, urand(14500, 16500));
                events.ScheduleEvent(EVENT_INTENSIFY, 100);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_BLADE_TEMPEST, 60000);

                // 8:15 minutes Enrage timer
                events.ScheduleEvent(EVENT_TAYAK_BERSERK, 495000);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(DATA_TAYAK, IN_PROGRESS);

                    if (GameObject* entranceDoor = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(GO_QUARTERS_DOOR_ENTRANCE) : ObjectGuid::Empty))
                        entranceDoor->SetGoState(GO_STATE_READY);
                }

                me->RemoveAurasDueToSpell(SPELL_VISINTRO_TAYAK);
                evadeModeEnabled = true;
                me->setRegeneratingHealth(false);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    // Coming to center of the room
                    case 2:
                    {
                        // Beginning of phase 2, Ta'yak at room's center
                        me->CastSpell(me, SPELL_SU_JUMP, false);
                        events.ScheduleEvent(EVENT_INTENSIFY, 1500);
                        events.ScheduleEvent(EVENT_STORM_UNLEASHED, 1500);

                        // Movement Type
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->UpdateMovementFlags();

                        // Moving to the other side
                        me->SetSpeed(MOVE_RUN, 15.0f, true);
                        me->SetSpeed(MOVE_WALK, 15.0f, true);
                        me->SetSpeed(MOVE_FLIGHT, 15.0f, true);
                        break;
                    }
                    // Coming to an edge
                    case 3:
                    {
                        // Movement Type
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->UpdateMovementFlags();

                        // it`ll proc only in right side
                        HandleInvertTornadoes();
                        events.ScheduleEvent(EVENT_SUMMON_TORNADOES, 1000);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_TAYAK_TALK_TRASH:
                        Talk(SAY_KILL_TRASH);
                        break;
                    case ACTION_TAYAK_PREEVENT_DONE:
                    {
                        Talk(SAY_INTRO);
                        introDone = true;
                        me->RemoveAura(SPELL_VISINTRO_TAYAK);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_AGGRESSIVE);

                        if (instance && instance->GetData(DATA_TAYAK) != SPECIAL) // save state
                            instance->SetData(DATA_TAYAK, SPECIAL);
                        break;
                    }
                    case ACTION_TAYAK_ENTRANCE:
                        if (!entranceDone)
                        {
                            Talk(SAY_INTRO);
                            SendMoveToInstructorsPoints();
                            entranceDone = true;
                        }
                        break;
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_CURRENT_STORMID)
                    return Phase;
                else if (type == TYPE_TARGET_ID)
                    return storm1Done ? 1 : 0;
                return 0;
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                me->RemoveAllAuras();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_TAYAK, DONE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (GameObject* entranceDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_QUARTERS_DOOR_ENTRANCE)))
                        entranceDoor->SetGoState(GO_STATE_ACTIVE);
                }
                ActivateGaleWinds(ACTION_STOP_WIND);

                // Remove fire from candles and makes them unclickable
                std::list<Creature*> candles;
                GetCreatureListWithEntryInGrid(candles, me, NPC_BURNING_BRAZIER, 400.0f);
                GetCreatureListWithEntryInGrid(candles, me, NPC_BRAZIER, 400.0f);

                for (auto&& itr : candles)
                {
                    itr->AI()->Reset();
                    itr->RemoveAurasDueToSpell(SPELL_BRAZIER_FIRE);
                    itr->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                // Prevent switch if in us cast
                if (me->HasAura(SPELL_UNSEEN_STRIKE_TR))
                    return;

                // Storm Unleashed 20 - 10%.
                if (me->HealthBelowPct(20) && Phase == PHASE_NORMAL)
                {
                    // Set phase
                    Phase = PHASE_STORM_UNLEASHED;
                    ActivateGaleWinds();
                    DeActivateAliveTornado();

                    Talk(SAY_STORM_UNLEASHED);

                    // Cancel the P1 events except intensify
                    events.CancelEvent(EVENT_TEMPEST_SLASH);
                    events.CancelEvent(EVENT_UNSEEN_STRIKE);
                    events.CancelEvent(EVENT_TAYAK_WIND_STEP);
                    events.CancelEvent(EVENT_OVERWHELMING_ASS);
                    if (IsHeroic())
                        events.CancelEvent(EVENT_BLADE_TEMPEST);

                    me->SetReactState(REACT_PASSIVE);
                    // Movement Type
                    me->OverrideInhabitType(INHABIT_AIR);
                    me->UpdateMovementFlags();

                    // Teleport players and move to P2 first point.
                    me->SetSpeed(MOVE_RUN, 15.0f, true);
                    me->SetSpeed(MOVE_WALK, 15.0f, true);
                    me->SetSpeed(MOVE_FLIGHT, 15.0f, true);
                    me->GetMotionMaster()->MovePoint(2, PlayerTelePos);

                    DoCast(me, SPELL_STORM_UNLEASHED_D);
                    me->AddAura(SPELL_SU_DMG_AURA, me);

                    // Activate Candles for achiev
                    std::list<Creature*> candles;
                    GetCreatureListWithEntryInGrid(candles, me, NPC_BURNING_BRAZIER, 400.0f);
                    GetCreatureListWithEntryInGrid(candles, me, NPC_BRAZIER, 400.0f);

                    for (auto&& itr : candles)
                        itr->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                }

                // Storm Unleashed 10 - 0%.
                if (me->HealthBelowPct(10) && Phase == PHASE_STORM_UNLEASHED && !storm1Done)
                {
                    storm1Done = true;
                    m_rightSide = true;
                    PrevSideTornado = 0;
                    ActivateGaleWinds();
                    Talk(SAY_STORM_UNLEASHED);
                    events.CancelEvent(EVENT_SUMMON_TORNADOES);

                    // Move to P2 second point.
                    me->SetSpeed(MOVE_RUN, 15.0f, true);
                    me->SetSpeed(MOVE_WALK, 15.0f, true);
                    me->SetSpeed(MOVE_FLIGHT, 15.0f, true);
                    me->GetMotionMaster()->MovePoint(3, TayakStormPoints[1]);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (evadeModeEnabled && instance && instance->IsWipe()) // handle evade cuz we can`t use default [hello unseen strike]
                {
                    me->RemoveAllAuras();
                    ActivateGaleWinds(ACTION_STOP_WIND);
                    EnterEvadeMode();
                }

                if (entranceDone && !introDone && CheckTrash())
                    if (me->FindNearestPlayer(100.0f))
                        me->AI()->DoAction(ACTION_TAYAK_PREEVENT_DONE);

                if ((!UpdateVictim() && !unseenReturn) || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TEMPEST_SLASH:
                        {
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* TargetCaster = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                            {
                                DoCast(TargetCaster, SPELL_TEMP_SLASH_DBM);

                                if (Unit* vict = me->GetVictim())
                                    targetGUID = vict->GetGUID();

                                me->PrepareChanneledCast(me->GetAngle(TargetCaster), SPELL_TEMP_SLASH_SUMM_V);

                                delay = 0;
                                me->m_Events.Schedule(delay += 2100, 20, [this]()
                                {
                                    me->RemoveChanneledCast(targetGUID);
                                });
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                DoCast(target, SPELL_TEMP_SLASH_DBM);

                                if (Unit* vict = me->GetVictim())
                                    targetGUID = vict->GetGUID();

                                me->PrepareChanneledCast(me->GetAngle(TargetCaster), SPELL_TEMP_SLASH_SUMM_V);

                                delay = 0;
                                me->m_Events.Schedule(delay += 2100, 20, [this]()
                                {
                                    me->RemoveChanneledCast(targetGUID);
                                });
                            }
                            events.ScheduleEvent(EVENT_TEMPEST_SLASH, 15.5 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE:
                        {
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            // Blizzard pls don`t make anymore these spells with cone proc
                            unseenTank = me->GetVictim() ? me->GetVictim()->GetGUID() : ObjectGuid::Empty;
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                Talk(ANN_UNSEEN, target);
                                me->CastSpell(target, SPELL_UNSEEN_STRIKE_TR, true);
                                me->AddAura(SPELL_UNSEEN_STRIKE_MKR, target);
                                unseenTarget = target->GetGUID();
                                me->SetReactState(REACT_PASSIVE);
                                unseenReturn = true;
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, TankSpecTargetSelector()))
                            {
                                Talk(ANN_UNSEEN, target);
                                me->CastSpell(target, SPELL_UNSEEN_STRIKE_TR, true);
                                me->AddAura(SPELL_UNSEEN_STRIKE_MKR, target);
                                unseenTarget = target->GetGUID();
                                me->SetReactState(REACT_PASSIVE);
                                unseenReturn = true;
                            }
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_CHASE, 100);
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_PREPARE, 4.9 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_RETURN, 5500);
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE, urand(53000, 61000));
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE_CHASE:
                        {
                            if (Unit* target = ObjectAccessor::GetUnit(*me, unseenTarget))
                                me->GetMotionMaster()->MoveChase(target);
                            
                            events.ScheduleEvent(EVENT_UNSEEN_STRIKE_CHASE, urand(100, 200));
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE_PREPARE:
                        {
                            events.CancelEvent(EVENT_UNSEEN_STRIKE_CHASE);

                            if (Unit* target = ObjectAccessor::GetUnit(*me, unseenTarget))
                                me->PrepareChanneledCast(me->GetAngle(target));
                            break;
                        }
                        case EVENT_UNSEEN_STRIKE_RETURN:
                        {
                            me->RemoveChanneledCast(unseenTank);
                            events.CancelEvent(EVENT_UNSEEN_STRIKE_CHASE);
                            break;
                        }
                        case EVENT_TAYAK_WIND_STEP:
                        {
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* vict = me->GetVictim())
                                currentTank = vict->GetGUID();

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                                DoCast(target, SPELL_WIND_STEP_TP);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_WIND_STEP_TP);

                            events.ScheduleEvent(EVENT_WIND_STEP_RETURN, 1000);
                            events.ScheduleEvent(EVENT_TAYAK_WIND_STEP, urand(24000, 26000));
                            break;
                        }
                        case EVENT_WIND_STEP_RETURN:
                        {
                            if (Unit* vict = ObjectAccessor::GetUnit(*me, currentTank))
                            {
                                if (vict->IsAlive())
                                    DoCast(vict, SPELL_WIND_STEP_TP_BACK);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                                DoCast(target, SPELL_WIND_STEP_TP_BACK);
                            break;
                        }
                        case EVENT_OVERWHELMING_ASS:
                        {
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            // On tank
                            DoCastVictim(SPELL_OVERWHELMING_ASS);
                            events.ScheduleEvent(EVENT_OVERWHELMING_ASS, 20500);
                            break;
                        }
                        case EVENT_INTENSIFY:
                        {
                            switch (Phase)
                            {
                                case PHASE_NORMAL:
                                    DoCast(me, SPELL_INTENSIFY_PERIODIC_60);
                                    break;
                                case PHASE_STORM_UNLEASHED:
                                    me->RemoveAurasDueToSpell(SPELL_INTENSIFY_PERIODIC_60);
                                    DoCast(me, SPELL_INTENSIFY_PERIODIC_10);
                                    break;
                            }
                            break;
                        }
                        // Heroic
                        case EVENT_BLADE_TEMPEST:
                        {
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            // Pull all players
                            DoCast(me, SPELL_BLADE_TEMPES_J_FC);
                            DoCast(me, SPELL_BLADE_TEMPEST_AUR);
                            events.ScheduleEvent(EVENT_BLADE_TEMPEST, 60000);
                            break;
                        }
                        // P2
                        case EVENT_STORM_UNLEASHED:
                        {
                            me->GetMotionMaster()->MovePoint(3, TayakStormPoints[0]);
                            break;
                        }
                        case EVENT_SUMMON_TORNADOES:
                        {
                            // Summon the tornado
                            std::vector<uint32> m_left = { NPC_STORM_UNLEASHED_STALKER_WEST1, NPC_STORM_UNLEASHED_STALKER_WEST2, NPC_STORM_UNLEASHED_STALKER_WEST3 };
                            std::vector<uint32> m_right = { NPC_STORM_UNLEASHED_STALKER_EAST1, NPC_STORM_UNLEASHED_STALKER_EAST2, NPC_STORM_UNLEASHED_STALKER_EAST3 };

                            // Combo of tornado shouldn`t be repeatable. It mean that if we`d choise WEST1 stalker at last time, we couldn`t choise it again
                            if (PrevSideTornado)
                                m_rightSide ? m_right.erase(std::find(m_right.begin(), m_right.end(), PrevSideTornado)) : m_left.erase(std::find(m_left.begin(), m_left.end(), PrevSideTornado));

                            // Select new tornado stalker from updated container
                            PrevSideTornado = m_rightSide ? Trinity::Containers::SelectRandomContainerElement(m_right) : Trinity::Containers::SelectRandomContainerElement(m_left);

                            // Get spell for selected tornado stalker
                            spellTornado = m_TornadoType.find(PrevSideTornado)->second;

                            if (Creature* m_sTornado = GetClosestCreatureWithEntry(me, PrevSideTornado, VISIBLE_RANGE, true))
                                m_sTornado->CastSpell(m_sTornado, spellTornado, false);

                            events.ScheduleEvent(EVENT_SUMMON_TORNADOES, 2 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_TAYAK_BERSERK: // Nasty if you're here!
                        {
                            me->AddAura(SPELL_TAYAK_BERSERK, me);
                            break;
                        }
                        default:
                            break;
                    }
                }
                // No melee in P2
                if (Phase == PHASE_NORMAL)
                    DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

            private:
                bool CheckTrash()
                {
                    Creature* GaleSlicer = GetClosestCreatureWithEntry(me, NPC_SETTHIK_GALESLICER, 150.0f, true);
                    Creature* Silentwing = GetClosestCreatureWithEntry(me, NPC_KORTHIK_SILENTWING, 150.0f, true);
                    Creature* Swarmer = GetClosestCreatureWithEntry(me, NPC_KORTHIK_SWARMER, 150.0f, true);
                    Creature* Tempest = GetClosestCreatureWithEntry(me, NPC_SETTHIK_TEMPEST, 150.0f, true);

                    if (GaleSlicer || Silentwing || Swarmer || Tempest)
                        return false;

                    return true;
                }

                void ActivateGaleWinds(uint32 action = ACTION_WIND)
                {
                    std::list<Creature*> GWStalkersList;
                    GetCreatureListWithEntryInGrid(GWStalkersList, me, NPC_GALE_WINDS_STALKER, 300.0f);

                    for (auto&& stalker : GWStalkersList)
                        stalker->AI()->DoAction(action);
                }

                void SetAggro()
                {
                    if (Unit* victim = SelectTarget(SELECT_TARGET_TOPAGGRO))
                    {
                        AttackStart(victim);
                        me->SetInCombatWith(victim);
                    }
                }
                void SendMoveToInstructorsPoints()
                {
                    std::list <Creature*> Instructors;
                    GetCreatureListWithEntryInGrid(Instructors, me, NPC_INSTRUCTOR_KLITHAK, 50.0f);
                    GetCreatureListWithEntryInGrid(Instructors, me, NPC_INSTRUCTOR_MALTIK, 50.0f);
                    GetCreatureListWithEntryInGrid(Instructors, me, NPC_INSTRUCTOR_TAKTHOK, 50.0f);
                    GetCreatureListWithEntryInGrid(Instructors, me, NPC_INSTRUCTOR_ZARIK, 50.0f);

                    for (auto&& itr : Instructors)
                        if (itr->IsAlive())
                            itr->AI()->DoAction(ACTION_INSTRUCTORS_MOVE);
                }

                void DeActivateAliveTornado()
                {
                    std::list<Creature*> TornadoList;
                    GetCreatureListWithEntryInGrid(TornadoList, me, NPC_TEMPEST_STALKER, 200.0f);

                    for (auto&& itr : TornadoList)
                        itr->DespawnOrUnsummon();
                }

                void HandleInvertTornadoes()
                {
                    std::list<Creature*> m_vTornado;
                    GetCreatureListWithEntryInGrid(m_vTornado, me, NPC_US_TORNADO, VISIBLE_RANGE);

                    for (auto&& itr : m_vTornado)
                        itr->AI()->DoAction(ACTION_TORNADO);
                }

                bool HandleRescheduleEventsIfCastAny(uint32 eventId)
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_UNSEEN_STRIKE_TR))
                    {
                        events.RescheduleEvent(eventId, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                        return true;
                    }

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_tayakAI(creature);
        }
};

// Heart of Fear - Trash Version Tempest Stalker (LTD): 64373.
class npc_tempest_slash_tornado : public CreatureScript
{
    public:
        npc_tempest_slash_tornado() : CreatureScript("npc_tempest_slash_tornado") { }

        enum Events
        {
            EVENT_MOVE   = 1,
            EVENT_CIRCLE = 2,
        };

        struct npc_tempest_slash_tornadoAI : public ScriptedAI
        {
            npc_tempest_slash_tornadoAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            EventMap events;
            InstanceScript* instance;
            float x, y;
            bool m_bCounterClockwise;

            void IsSummonedBy(Unit* summoner) override
            {
                events.Reset();
                me->SetReactState(REACT_PASSIVE);

                if (summoner)
                {
                    // Random orientation in front of Ta'yak
                    float ori = Position::NormalizeOrientation(summoner->GetOrientation() + frand(-M_PI / 12, M_PI / 12));
                    me->SetOrientation(ori);
                    me->SetFacingTo(ori);

                    // Applying auras and moving
                    me->SetInCombatWithZone();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                    me->AddAura(SPELL_TEMP_SLASH_AURA, me);     // Visual aura
                    me->AddAura(SPELL_TEMP_SLASH_DAMAGE, me);   // Damage aura

                    float x, y, z;
                    me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, frand(5.0f, 30.0f));
                    me->GetMotionMaster()->MovePoint(8, x, y, z);
                }
            }

            void EnterEvadeMode() override { }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 8)
                {
                    me->StopMoving();
                    m_bCounterClockwise = urand(0, 1);
                    x = me->GetPositionX();
                    y = me->GetPositionY();

                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 1; i < 13; ++i)
                    {
                        float newX = x + 2.0f * cos(i * M_PI / 6);
                        float newY = m_bCounterClockwise ? y - 2.0f * sin(i * M_PI / 6) : y + 2.0f * sin(i * M_PI / 6);
                        init.Path().push_back(G3D::Vector3(newX, newY, me->GetPositionZ()));
                    }

                    init.SetUncompressed();
                    init.SetCyclic();
                    init.Launch();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tempest_slash_tornadoAI(creature);
        }
};

// Heart of Fear - Armsmaster Ta'yak - Storm Unleashed West 1 Tornado (LTD): 63278.
class npc_storm_unleashed_tornado : public CreatureScript
{
    public:
        npc_storm_unleashed_tornado() : CreatureScript("npc_storm_unleashed_tornado") { }

        struct npc_storm_unleashed_tornadoAI : public ScriptedAI
        {
            npc_storm_unleashed_tornadoAI(Creature* creature) : ScriptedAI(creature), vehicle(creature->GetVehicleKit())
            {
                //ASSERT(vehicle);
                instance = creature->GetInstanceScript();
            }

            Vehicle* vehicle;
            InstanceScript* instance;
            bool storm1;
            float x, y;
            ObjectGuid m_ownerGUID;
            uint32 m_passenger;

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner)
                    m_ownerGUID = summoner->GetGUID();
            }

            void Reset() override
            {
                storm1 = true;
                me->AddAura(SPELL_SU_AURA, me);         // Control vehicle aura.
                me->AddAura(SPELL_SU_DUMMY_VIS, me);    // Visual aura.
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);

                me->SetSpeed(MOVE_WALK, 1.35f, true);
                me->SetSpeed(MOVE_RUN, 1.35f, true);
                Movement::MoveSplineInit init(me);
                init.SetOrientationFixed(true);
                init.Launch();

                GetPositionWithDistInOrientation(me, 168.0f, me->GetOrientation(), x, y);
                me->GetMotionMaster()->MovePoint(2, x, y, me->GetPositionZ());
            }

            void Eject()
            {
                if (vehicle)
                {
                    if (Unit* passenger = vehicle->GetPassenger(0))
                    {
                        float ExAngle = urand(0, 1) ? 5.0f : -5.0f; // if not use this, then player will at vehicle till end of way

                        Position ExPos;
                        ExPos.m_positionX = me->GetPositionX() + ExAngle;
                        ExPos.m_positionY = me->GetPositionY() + frand(-4.0f, 4.0f);
                        ExPos.m_positionZ = me->GetPositionZ();
                        ExPos.SetOrientation(me->GetOrientation());
                        passenger->ExitVehicle(&ExPos);
                    }
                }

                me->RemoveAura(SPELL_SU_AURA);
                me->RemoveAura(SPELL_SU_DUMMY_VIS);
                me->DespawnOrUnsummon();
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_TARGET_ID)
                    return m_passenger;
                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_TARGET_ID)
                    m_passenger = data;
            }

            void DoAction(int32 actionId) override
            {
                // Invert our path if Tayak has change it
                if (actionId == ACTION_TORNADO)
                {
                    if (Creature* m_sTornado = ObjectAccessor::GetCreature(*me, m_ownerGUID))
                    {
                        me->GetMotionMaster()->Clear();
                        m_passenger = 0;
                        me->GetMotionMaster()->MovePoint(2, *m_sTornado);
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 2)
                    Eject();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!instance)
                {
                    Eject();
                    return;
                }

                // Removing storm from the 1st storm phase (20-10%) when switching on 2nd storm phase (10-0%)
                if (storm1)
                    if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_TAYAK) : ObjectGuid::Empty))
                        if (tayak->AI()->GetData(TYPE_TARGET_ID))
                            Eject();

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_storm_unleashed_tornadoAI(creature);
        }
};

// Gale Winds Stalker - 63292
class npc_gale_winds_stalker : public CreatureScript
{
    public:
        npc_gale_winds_stalker() : CreatureScript("npc_gale_winds_stalker") { }

        struct npc_gale_winds_stalkerAI : public ScriptedAI
        {
            npc_gale_winds_stalkerAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            bool firstWind, isActive;

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                isActive = false;
                firstWind = false;
                if (me->GetOrientation() < 3)
                    firstWind = true;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_WIND:
                    {
                        if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                        {
                            bool firstPos = tayak->AI()->GetData(TYPE_TARGET_ID);
                            if ((firstPos && firstWind) || (!firstPos && !firstWind))
                            {
                                DoCast(SPELL_SU_WIND_GALE);
                                tayak->CastSpell(me, 123616, false);
                            }
                            else
                                if (me->HasAura(SPELL_SU_WIND_GALE))
                                    me->RemoveAura(SPELL_SU_WIND_GALE);

                            if (!isActive)
                                isActive = true;
                        }
                        break;
                    }
                    case ACTION_STOP_WIND:
                    {
                        me->RemoveAurasDueToSpell(SPELL_SU_WIND_GALE);

                        if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                        {
                            Position pos = { tayak->GetPositionX(), tayak->GetPositionY(), tayak->GetPositionZ(), 0.0f };

                            std::list<Player*> playerList;
                            GetPlayerListInGrid(playerList, me, 300.0f);

                            for (auto&& player : playerList)
                                if (player->HasForcedMovement())
                                    player->SendApplyMovementForce(false, pos);

                            isActive = false;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                // Check force
                if (!instance || !isActive)
                    return;

                if (Creature* tayak = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                {
                    Position pos = { tayak->GetPositionX(), tayak->GetPositionY(), tayak->GetPositionZ(), 0.0f };

                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, me, 20.0f);

                    for (auto&& player : playerList)
                    {
                        // if player is in wind gale
                        if (player->GetPositionX() > -2109.51f || player->GetPositionX() < -2129.05f)
                        {
                            // Player doesn't have forcedMovement
                            if (player->IsAlive() && !player->HasForcedMovement())
                                player->SendApplyMovementForce(true, pos, -7.0f);
                            // Dead player has forcedMovement
                            else if (!player->IsAlive() && player->HasForcedMovement())
                                player->SendApplyMovementForce(false, pos);
                        }
                        // player not in wind gale
                        else if (player->HasForcedMovement())
                            player->SendApplyMovementForce(false, pos);
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_gale_winds_stalkerAI(creature);
        }
};

// Storm Unleashed Player Vehicle - 63567
class npc_su_player_vehicle : public CreatureScript
{
    public:
        npc_su_player_vehicle() : CreatureScript("npc_su_player_vehicle") { }

        struct npc_su_player_vehicleAI : public ScriptedAI
        {
            npc_su_player_vehicleAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetSpeed(MOVE_WALK, 1.5f);
                me->SetSpeed(MOVE_RUN, 1.5f);
                me->SetDisplayId(11686);
                me->CastSpell(me, SPELL_SU_VEH_DUMMY_VIS, false);

                if (summoner && summoner->IsAlive())
                    summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

                me->GetMotionMaster()->MovePoint(2, PlayerVehPos);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 2)
                {
                    if (Vehicle* m_veh = me->GetVehicle())
                        if (Unit* m_passenger = m_veh->GetPassenger(0))
                            m_passenger->ExitVehicle();

                    me->DespawnOrUnsummon();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_su_player_vehicleAI(creature);
        }
};

// Burning Brazier 65597
struct npc_burning_brazier : public ScriptedAI
{
    npc_burning_brazier(Creature* creature) : ScriptedAI(creature) { }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (clicker->HasAura(SPELL_FLAME_BEARER))
            return;

        clicker->CastSpell(clicker, SPELL_FLAME_BEARER, true);
    }

    void UpdateAI(uint32 diff) override { }
};

// Brazier 65593
struct npc_achiev_brazier : public ScriptedAI
{
    npc_achiev_brazier(Creature* creature) : ScriptedAI(creature) { }

    bool hasLaunched;

    void Reset() override
    {
        hasLaunched = false;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (!clicker->HasAura(SPELL_FLAME_BEARER))
            return;

        if (!me->HasAura(SPELL_BRAZIER_FIRE) && !hasLaunched)
        {
            hasLaunched = true;
            DoCast(me, SPELL_BRAZIER_FIRE, true);

            if (Creature* tayak = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_TAYAK) : ObjectGuid::Empty))
                tayak->AI()->SetData(TYPE_BRAZIERS, 1);
        }
    }

    void UpdateAI(uint32 diff) override { }
};

// Wind Step: 123175
class spell_tayak_wind_step : public SpellScriptLoader
{
    public:
        spell_tayak_wind_step() : SpellScriptLoader("spell_tayak_wind_step") { }

        class spell_tayak_wind_stepSpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tayak_wind_stepSpellScript);

            void HandleTeleportEffect(SpellEffIndex effIndex)
            {
                if (Unit* caster = GetCaster())
                {
                    ObjectGuid casterGUID = caster->GetGUID();
                    uint32 delay = 0;
                    caster->m_Events.Schedule(delay += 200, 20, [caster, casterGUID]()
                    {
                        if (Unit* m_caster = ObjectAccessor::GetUnit(*caster, casterGUID))
                            m_caster->CastSpell(m_caster, SPELL_WIND_STEP_EFF, false);
                    });
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tayak_wind_stepSpellScript::HandleTeleportEffect, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tayak_wind_stepSpellScript();
        }
};

// Wind Step Effect: 123459
class spell_tayak_wind_step_eff : public SpellScriptLoader
{
    public:
        spell_tayak_wind_step_eff() : SpellScriptLoader("spell_tayak_wind_step_eff") { }

        class spell_tayak_wind_step_eff_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tayak_wind_step_eff_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetOwner()->ToUnit())
                    caster->AddAura(SPELL_WIND_STEP_B_DMG, caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_tayak_wind_step_eff_AuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tayak_wind_step_eff_AuraScript();
        }
};

// 122982 - Unseen Strike
class spell_unseen_strike_aura : public SpellScriptLoader
{
    public:
        spell_unseen_strike_aura() : SpellScriptLoader("spell_unseen_strike") { }

        class spell_unseen_strike_auraAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_unseen_strike_auraAuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetTarget())
                        caster->AddAura(SPELL_UNSEEN_STRIKE_TR, target);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_unseen_strike_auraAuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_unseen_strike_auraAuraScript();
        }
};

// 122994 - Unseen Strike
class spell_unseen_strike_dmg : public SpellScriptLoader
{
    public:
        spell_unseen_strike_dmg() : SpellScriptLoader("spell_unseen_strike_dmg") { }

        class spell_unseen_strike_dmgSpellScript : public SpellScript
        {
            PrepareSpellScript(spell_unseen_strike_dmgSpellScript);

            int32 numTargets;

            void CountTargets(std::list<WorldObject*>& targets)
            {
                numTargets = targets.size();
            }

            void DealDamages(SpellEffIndex effIndex)
            {
                if (numTargets)
                    SetHitDamage(GetSpellInfo()->Effects[effIndex].BasePoints / numTargets);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_unseen_strike_dmgSpellScript::CountTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
                OnEffectHitTarget += SpellEffectFn(spell_unseen_strike_dmgSpellScript::DealDamages, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_unseen_strike_dmgSpellScript();
        }
};

// Storm Unleashed: 124783.
class spell_tayak_storm_unleashed_dmg : public SpellScriptLoader
{
    public:
        spell_tayak_storm_unleashed_dmg() : SpellScriptLoader("spell_tayak_storm_unleashed_dmg") { }

        class spell_tayak_storm_unleashed_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tayak_storm_unleashed_dmg_SpellScript);

            void HandleRecalculateDamage(SpellEffIndex /*effIndex*/)
            {
                uint32 m_damage = GetHitDamage();

                if (GetHitUnit() && GetHitUnit()->IsOnVehicle())
                    SetHitDamage(int32(m_damage + m_damage * 0.25f)); // if player caught in storm, then inc dmg on 25% from SU
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_tayak_storm_unleashed_dmg_SpellScript::HandleRecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_tayak_storm_unleashed_dmg_SpellScript();
        }
};

// 122853 - Tempest Slash (damage aura)
class spell_tempest_slash : public SpellScriptLoader
{
    public:
        spell_tempest_slash() : SpellScriptLoader("spell_tempest_slash") { }

        class spell_tempest_slash_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tempest_slash_AuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_TEMP_SLASH_DAMAGE, caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_tempest_slash_AuraScript::Apply, EFFECT_2, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tempest_slash_AuraScript();
        }
};

// 123814 - Storm unleashed - Boss visual effect
class spell_tayak_su_visual : public SpellScriptLoader
{
    public:
        spell_tayak_su_visual() : SpellScriptLoader("spell_tayak_su_visual") { }

        class spell_tayak_su_visualAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tayak_su_visualAuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_STORM_UNLEASHED_D, caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_tayak_su_visualAuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_tayak_su_visualAuraScript();
        }
};

// 124024 - Storm unleashed dummy visual
class spell_su_dummy_visual : public SpellScriptLoader
{
    public:
        spell_su_dummy_visual() : SpellScriptLoader("spell_su_dummy_visual") { }

        class spell_su_dummy_visualAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_su_dummy_visualAuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_SU_DUMMY_VIS, caster);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_su_dummy_visualAuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScrpt() const
        {
            return new spell_su_dummy_visualAuraScript();
        }
};

// 123633 - Gale Winds
class spell_gale_winds : public SpellScriptLoader
{
    public:
        spell_gale_winds() : SpellScriptLoader("spell_gale_winds") { }

        class spell_gale_windsAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gale_windsAuraScript);

            void Apply(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                    caster->AddAura(SPELL_GALE_FORCE_WINDS_T, caster);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gale_windsAuraScript::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_gale_windsAuraScript();
        }
};

// 123600 - Storm unleashed
class spell_su_dummy : public SpellScriptLoader
{
    public:
        spell_su_dummy() : SpellScriptLoader("spell_su_dummy") { }

        class spell_su_dummyAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_su_dummyAuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<Creature*> galeList;
                    GetCreatureListWithEntryInGrid(galeList, caster, NPC_GALE_WINDS_STALKER, 15.0f);

                    for (auto&& gale : galeList)
                        caster->AddAura(SPELL_SU_DUMMY_CRAP, gale);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_su_dummyAuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_su_dummyAuraScript();
        }
};

// 123616 - Storm unleashed
class spell_su_dumaura : public SpellScriptLoader
{
    public:
        spell_su_dumaura() : SpellScriptLoader("spell_su_dumaura") { }

        class spell_su_dumauraAuraScript : public AuraScript
        {
            PrepareAuraScript(spell_su_dumauraAuraScript);

            void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    std::list<Player*> playerList;
                    GetPlayerListInGrid(playerList, caster, 40.0f);

                    for (auto&& player : playerList)
                        caster->AddAura(SPELL_SU_DUMMY_CRAP, player);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_su_dumauraAuraScript::Apply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_su_dumauraAuraScript();
        }
};

// 124258 - Storm unleashed vehicle players
class spell_su_vehicle_apply : public SpellScriptLoader
{
    public:
        spell_su_vehicle_apply() : SpellScriptLoader("spell_su_vehicle_apply") { }

        class spell_su_vehicle_apply_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_su_vehicle_apply_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                uint32 stormLowId = caster->GetAI()->GetData(TYPE_TARGET_ID);

                // Not pull same target again
                if (uint32 stormLowId = caster->GetAI()->GetData(TYPE_TARGET_ID))
                    if (stormLowId == target->GetGUID().GetCounter())
                        return;

                // Pull only if we haven`t anyone already in us
                if (!target->IsOnVehicle() && caster->GetVehicleKit() && !caster->GetVehicleKit()->GetPassenger(0))
                {
                    target->CastSpell(caster, SPELL_SU_RV, true);
                    caster->GetAI()->SetData(TYPE_TARGET_ID, target->GetGUID().GetCounter());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_su_vehicle_apply_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_su_vehicle_apply_SpellScript();
        }
};

// Blade Tempest Jump 125327
class spell_blade_templest_jump : public SpellScriptLoader
{
    public:
        spell_blade_templest_jump() : SpellScriptLoader("spell_blade_templest_jump") { }

        class spell_blade_templest_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blade_templest_jump_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLADE_TEMPEST_PULL))
                    return false;
                return true;
            }

            void HandlePosition(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Unit* caster = GetCaster())
                    if (InstanceScript* instance = caster->GetInstanceScript())
                        if (Unit* target = ObjectAccessor::GetUnit(*caster, instance->GetGuidData(DATA_TAYAK)))
                            if (caster->GetExactDist2d(target) > 2.0f)
                                caster->GetMotionMaster()->MoveJump(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 35.0f, 15.0f);

                FinishCast(SPELL_CAST_OK);
            }

            void Register() override
            {
                OnEffectLaunch += SpellEffectFn(spell_blade_templest_jump_SpellScript::HandlePosition, EFFECT_0, SPELL_EFFECT_JUMP);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blade_templest_jump_SpellScript;
        }
};

// Storm Unleashed Jump 124033
class spell_su_jump : public SpellScriptLoader
{
    public:
        spell_su_jump() : SpellScriptLoader("spell_su_jump") { }

        class spell_su_jump_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_su_jump_SpellScript);

            void HandlePosition(SpellEffIndex effIndex)
            {
                if (Unit* m_caster = GetCaster())
                    m_caster->CastSpell(m_caster, SPELL_SU_SUMM_VEH, false);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_su_jump_SpellScript::HandlePosition, EFFECT_0, SPELL_EFFECT_JUMP);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_su_jump_SpellScript;
        }
};


// AreaTrigger 8261
class AreaTrigger_at_tayak_room : public AreaTriggerScript
{
    public:
        AreaTrigger_at_tayak_room() : AreaTriggerScript("AreaTrigger_at_tayak_room") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Unit* tayak = ObjectAccessor::GetUnit(*player, instance->GetGuidData(DATA_TAYAK)))
                    if (tayak->ToCreature() && tayak->ToCreature()->AI() && instance->GetData(DATA_TAYAK) < DONE)
                        tayak->ToCreature()->AI()->DoAction(ACTION_TAYAK_ENTRANCE);

            return false;
        }
};

// 381. Summoned by 125301 - Blade Tempest
class sat_tayak_blade_tempest : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                if (target->IsAlive() && !target->HasForcedMovement())
                    target->SendApplyMovementForce(true, *caster, 6.8f);
                else if (!target->IsAlive() && target->HasForcedMovement())
                    target->SendApplyMovementForce(false, *caster);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                if (target->HasForcedMovement())
                    target->SendApplyMovementForce(false, *caster);
    }
};

void AddSC_boss_tayak()
{
    new boss_tayak();                       // 62543
    new npc_tempest_slash_tornado();        // 64573
    new npc_storm_unleashed_tornado();      // 63278
    new npc_gale_winds_stalker();           // 63292
    new npc_su_player_vehicle();            // 63567
    new creature_script<npc_burning_brazier>("npc_burning_brazier");
    new creature_script<npc_achiev_brazier>("npc_achiev_brazier");

    new spell_tayak_wind_step();            // 123175
    new spell_tayak_wind_step_eff();        // 123459
    new spell_tayak_storm_unleashed_dmg();  // 124783
    new spell_tempest_slash();              // 122853
    new spell_unseen_strike_aura();         // 122982
    new spell_unseen_strike_dmg();          // 122994
    new spell_tayak_su_visual();            // 123814
    new spell_su_dummy_visual();            // 124024
    new spell_gale_winds();                 // 123633
    new spell_su_dummy();                   // 123600
    new spell_su_dumaura();                 // 123616
    new spell_su_vehicle_apply();           // 124258
    new spell_blade_templest_jump();        // 125327
    new spell_su_jump();                    // 124033
    new AreaTrigger_at_tayak_room();        // 8261
    new atrigger_script<sat_tayak_blade_tempest>("sat_tayak_blade_tempest");
}
