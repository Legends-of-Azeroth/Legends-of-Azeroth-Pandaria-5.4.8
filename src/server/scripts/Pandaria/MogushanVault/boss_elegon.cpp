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

#include "mogu_shan_vault.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"

enum eElegonPhase
{
    PHASE_1                         = 1,
    PHASE_2                         = 2,
    PHASE_3                         = 3,
    PHASE_FINAL                     = 4
};

enum eSpells
{
    // Elegon
    SPELL_APPARITION_VISUAL         = 127808,

    // Platform
    SPELL_TOUCH_OF_THE_TITANS       = 130287,
    SPELL_TOUCH_OF_TITANS_VISUAL    = 117874,
    SPELL_OVERCHARGED               = 117877,
    SPELL_OVERCHARGED_AURA          = 117878,

    // Phase 1
    SPELL_CELESTIAL_BREATH          = 117960,
    SPELL_MATERIALIZE_PROTECTOR     = 117958,
    SPELL_GRASPING_ENERGY_TENDRILS  = 127362,
    SPELL_GRASPING_ENERGY_GRIP      = 129724,
    SPELL_RADIATING_ENERGIES        = 118310,
    SPELL_RADIATING_ENERGIES_VISUAL = 118992,
    SPELL_RADIATING_OUTSIDE_VORTEX  = 118313,
    SPELL_RADIATING_INSIDE_VORTEX   = 122741,

    // Phase 2
    SPELL_FOCUS_POWER               = 119358,
    SPELL_DRAW_POWER_SHORT          = 119360,
    SPELL_DRAW_POWER_LONG           = 124967,
    SPELL_DRAW_POWER_SUMMONS        = 118018,
    SPELL_DRAW_POWER_DEBUFF         = 119387,

    // Phase 3
    SPELL_UNSTABLE_ENERGY           = 116994,
    SPELL_UNSTABLE_ENERGY_DAMAGE    = 116997,
    SPELL_PHASE_SHIFTED             = 118921,
    SPELL_OVERLOADED_MISSILE        = 116989,
    SPELL_OVERLOADED                = 117204,
    SPELL_ENERGY_CASCADE            = 122199,

    // Empyreal Focus
    SPELL_FOCUS_INACTIVE            = 127303,
    SPELL_FOCUS_ACTIVE              = 127305,
    SPELL_FOCUS_ACTIVATION_VISUAL   = 118794,
    SPELL_FOCUS_ACTIVATE_STATE      = 127305,
    SPELL_FOCUS_LIGHT_AREATRIGGER   = 116546,
    SPELL_FOCUS_LIGHT_EFF           = 116663,
    SPELL_FOCUS_LIGHT_CASTBAR       = 116598,
    SPELL_FOCUS_LIGHT_WALL_VISUAL   = 116604,

    // Celestial Protector
    SPELL_TOTAL_ANNIHILATION        = 129711,
    SPELL_TOTAL_ANNIHILATION_EFF    = 117914,
    SPELL_ARCING_ENERGY             = 117945,
    SPELL_STABILITY_FLUX            = 117911,
    SPELL_ECLIPSE_NO_PHASE          = 117885,
    SPELL_ECLIPSE_PHASE             = 117886,
    SPELL_CLOSED_CIRCUIT            = 117949,

    // Cosmic Spark
    SPELL_ICE_TRAP                  = 135382,

    // Energy Charge
    SPELL_CORE_CHECKER              = 118024,
    SPELL_CORE_BEAM                 = 118430,
    SPELL_DISCHARGE                 = 118299,
    SPELL_HIGH_ENERGY               = 118118,

    // Infinite Energy (Intro)
    SPELL_SPAWN_FLASH_1_PERIODIC    = 127785,
    SPELL_SPAWN_FLASH_2_PERIODIC    = 127783,
    SPELL_SPAWN_FLASH_3_PERIODIC    = 127781,

    // Heroic
    SPELL_DESTABILIZING_ENERGIES    = 132222,
    SPELL_CATASTROPHIC_ANOMALY      = 127341,
    SPELL_DESTABILIZE               = 132226,

    // Achievement
    SPELL_STRAIGHT_SIX              = 130284,
};

enum eEvents
{
    // Elegon
    EVENT_CHECK_UNIT_ON_PLATFORM    = 1,
    EVENT_CHECK_MELEE               = 2,
    EVENT_CELESTIAL_BREATH          = 3,
    EVENT_MATERIALIZE_PROTECTOR     = 4,

    EVENT_FOCUS_POWER               = 5,
    EVENT_DRAW_POWER                = 6,

    // Celestial Protector
    EVENT_ARCING_ENERGY             = 5,
    EVENT_TOUCH_OF_THE_TITANS       = 6,
    EVENT_KILLED                    = 7,

    // Cosmic Spark
    EVENT_ICE_TRAP                  = 8,

    EVENT_ENRAGE_HARD               = 9,
    EVENT_DESPAWN_PLATFORM          = 10,
    EVENT_AFTER_DESPAWN_PLATFORM    = 11,
    EVENT_END_OF_PHASE_3            = 12,
    EVENT_RADIATING_ENERGIES        = 13,
    EVENT_LAUNCH_COSMIC_SPARK       = 14,
    EVENT_CATASTROPHIC           = 15,
    EVENT_CATASTROPHIC_01              = 16,
};

enum elegonActions
{
    ACTION_RESET_DRAWING_POWER      = 1,
    ACTION_SPAWN_ENERGY_CHARGES     = 2,
    ACTION_DESPAWN_ENERGY_CHARGES   = 3,
    ACTION_EMPYREAL_FOCUS_KILLED    = 4,
    ACTION_3 = 5
};

enum eMovementPoints
{
    POINT_EMPYEREAN_FOCUS           = 1,
};

enum empyrealFocusActions
{
    ACTION_ACTIVATE_EMPYREAL_FOCUS  = 1,
    ACTION_RESET_EMPYREAL_FOCUS     = 2,
};

enum empyrealFocusEvents
{
    EVENT_ACTIVATE_EMPYREAL_FOCUS   = 1,
    EVENT_APPEAR_WALL_OF_LIGHTNING  = 2,
};

enum eTalk
{
    TALK_INTRO          = 0,
    TALK_INTRO_2        = 1,
    TALK_AGGRO          = 2,
    TALK_DEATH          = 3,
    TALK_A_TO_B_1       = 4,
    TALK_A_TO_B_2       = 5,
    TALK_B_TO_C_1       = 6,
    TALK_C_TO_A_1       = 7,
    TALK_C_TO_A_2       = 8,
    TALK_C_TO_A_3       = 9,
    TALK_ENRAGE_HARD    = 10,
    TALK_ENRAGE_SOFT    = 11,
    TALK_SLAY           = 12,
};

const std::map<uint64, uint32> ImperialFocusMatch =
{
    {552352, 3}, // South-East
    {552367, 4}, // South
    {552356, 5}, // South-West
    {552340, 2}, // North-East
    {552353, 1}, // North
    {552370, 0}, // North-West
};

Position middlePos         = { 4023.15f, 1907.60f, 358.872f, 0.0f };
Position infiniteEnergyPos = { 4023.45f, 1907.60f, 365.0f, 0.0f };

Position energyChargePos[6] =
{
    { 4029.58f, 1914.03f, 360.83f, 0.788954f }, // North-West
    { 4032.21f, 1907.66f, 360.83f, 0.002521f }, // North
    { 4029.58f, 1901.30f, 360.83f, 5.497790f }, // North-East
    { 4016.85f, 1901.30f, 360.83f, 3.923430f }, // South-East
    { 4014.21f, 1907.66f, 360.83f, 3.139080f }, // South
    { 4016.85f, 1914.03f, 360.83f, 2.356190f }  // South-West
};

Position empyrealFocusPosition[6] =
{
    { 4054.24f, 1938.86f, 358.872f, 0.785398f }, // North-West
    { 4067.13f, 1907.75f, 358.872f, 0.000000f }, // North
    { 4054.24f, 1876.64f, 358.872f, 5.49779f  }, // North-East
    { 3992.02f, 1876.64f, 358.872f, 3.92699f  }, // South-East
    { 3979.13f, 1907.75f, 358.872f, 3.14159f  }, // South
    { 3992.02f, 1938.86f, 358.872f, 2.35619f  }  // South-West
};

enum infiniteActions
{
    ACTION_INFINITE_GO_DOWN         = 0,
    ACTION_INFINITE_ACTIVATION_WEST = 1,
    ACTION_INFINITE_ACTIVATION      = 2,
    ACTION_INFINITE_ACTIVATION_EAST = 3,
    ACTION_INFINITE_FLASH_SPAWN     = 4,
    ACTION_INFINITE_SPAWN_PLATFORM  = 5,
    ACTION_INFINITE_SPAWN_BOSS      = 6,
    ACTION_INFINITE_LOOT            = 7,
};

enum AchievementWorldStates
{
    WORLD_STATE_STRAIGHT_SIX = 12649,
};


// Set values in reset of mob_empyreal_focus
ObjectGuid empyrealFocus[6] =
{
    ObjectGuid::Empty, // South-West
    ObjectGuid::Empty, // North-West
    ObjectGuid::Empty, // South
    ObjectGuid::Empty, // North
    ObjectGuid::Empty, // South-East
    ObjectGuid::Empty  // North-East
};
// Elegon - 60410
class boss_elegon : public CreatureScript
{
    public:
        boss_elegon() : CreatureScript("boss_elegon") { }

        struct boss_elegonAI : public BossAI
        {
            boss_elegonAI(Creature* creature) : BossAI(creature, DATA_ELEGON) { }

            uint8 phase;
            uint8 phase2WaveCount;
            float nextPhase1EndingHealthPct;
            uint8 successfulDrawingPower;
            uint8 energyChargeCounter;
            uint8 empyrealFocusKilled;
            uint32 empyrealFocusKilledInRow = 0;
            TimeValue empyrealFocusKilledLastTime;
            bool canGrasp;

            void InitializeAI() override
            {
                me->setActive(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_CAN_SWIM | UNIT_FLAG_NOT_SELECTABLE);
                me->SetDisplayId(11686);
                me->AddAura(SPELL_APPARITION_VISUAL, me);
                //hardcoded
                me->SetCanFly(true);
                me->SetPosition(4025.139f, 1907.31f, 370.f, me->GetOrientation());
                me->SetHomePosition(4025.139f, 1907.31f, 370.f, me->GetOrientation());

                phase = PHASE_1;
                phase2WaveCount = 0;
                nextPhase1EndingHealthPct = 85.0f;
                successfulDrawingPower = 0;
                energyChargeCounter = 0;
                empyrealFocusKilled = 0;
                canGrasp = false;
                SetCombatMovement(false);
            }

            void Reset() override
            {
                _Reset();

                me->AddAura(SPELL_APPARITION_VISUAL, me);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_CAN_SWIM | UNIT_FLAG_NOT_SELECTABLE);

                me->RemoveAurasDueToSpell(SPELL_UNSTABLE_ENERGY);
                me->RemoveAurasDueToSpell(SPELL_PHASE_SHIFTED);
                me->RemoveAurasDueToSpell(SPELL_OVERLOADED);
                me->RemoveAurasDueToSpell(SPELL_RADIATING_ENERGIES_VISUAL);

                phase                     = PHASE_1;
                phase2WaveCount           = 0;
                nextPhase1EndingHealthPct = 85.0f;
                successfulDrawingPower    = 0;
                energyChargeCounter       = 0;
                empyrealFocusKilled       = 0;
                empyrealFocusKilledInRow  = 0;
                empyrealFocusKilledLastTime = TimeValue::zero();
                canGrasp                  = false;

                if (instance)
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                me->GetMap()->SetWorldState(WORLD_STATE_STRAIGHT_SIX, 0);

                //hardcoded
                me->SetPosition(4025.139f, 1907.31f, 370.f, me->GetOrientation());
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_ELEGON, FAIL);
            }

            void JustEngagedWith(Unit* who) override
            {
                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 24.0f);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 24.0f);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(DATA_ELEGON, IN_PROGRESS);
                }

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_CAN_SWIM | UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveAurasDueToSpell(SPELL_APPARITION_VISUAL);

                Talk(TALK_AGGRO);

                events.ScheduleEvent(EVENT_CELESTIAL_BREATH, 8 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_MATERIALIZE_PROTECTOR, 13 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENRAGE_HARD, 570000); // 9min30
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    if (GameObject* energyPlatform = instance->instance->GetGameObject(instance->GetGuidData(GO_ENERGY_PLATFORM)))
                        energyPlatform->SetGoState(GO_STATE_READY);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOUCH_OF_THE_TITANS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOUCH_OF_TITANS_VISUAL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCHARGED_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCHARGED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DESTABILIZE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLOSED_CIRCUIT);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    std::list<Creature*> imperialList;
                    GetCreatureListWithEntryInGrid(imperialList, me, NPC_EMPYREAL_FOCUS, 200.0f);

                    for (auto&& it : imperialList)
                        it->AI()->DoAction(ACTION_RESET_EMPYREAL_FOCUS);

                    // Despawn de cosmic sparks restantes...
                    std::list<Creature*> cosmicSparkList;
                    GetCreatureListWithEntryInGrid(cosmicSparkList, me, 62618, 200.0f);
                    for (auto&& it : cosmicSparkList)
                        it->DespawnOrUnsummon();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_ELEGON, FAIL);
                }
                summons.DespawnAll();
                me->GetMotionMaster()->MoveTargetedHome();
                _DespawnAtEvade();
            }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (me->Attack(target, true))
                    DoStartNoMovement(target);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_RESET_DRAWING_POWER:
                    {
                        successfulDrawingPower = 0;
                        break;
                    }
                    case ACTION_SPAWN_ENERGY_CHARGES:
                    {
                        std::list<Creature*> imperialList;
                        GetCreatureListWithEntryInGrid(imperialList, me, NPC_EMPYREAL_FOCUS, 200.0f);

                        for (auto&& it : imperialList)
                        {
                            for (auto&& impItr : ImperialFocusMatch)
                            {
                                if (it->GetDBTableGUIDLow() == impItr.first)
                                {
                                    if (Creature* energyCharge = me->SummonCreature(NPC_ENERGY_CHARGE, energyChargePos[impItr.second], TEMPSUMMON_CORPSE_DESPAWN))
                                    {
                                        // Increase speed by 20% for each successful draw power
                                        for (int j = 0; j < (successfulDrawingPower - 1); j++)
                                            energyCharge->CastSpell(energyCharge, SPELL_HIGH_ENERGY, true);

                                        // Should be realised update in SpellAuraEffect for MOVE_WALK
                                        if (Aura* cEnergy = energyCharge->GetAura(SPELL_HIGH_ENERGY))
                                            if (uint32 eStacks = cEnergy->GetStackAmount())
                                                energyCharge->SetSpeed(MOVE_WALK, 1.0f + 0.2f * eStacks * 1);

                                        energyCharge->CastSpell(it, SPELL_CORE_BEAM, true);
                                        energyCharge->SetTarget(it->GetGUID());
                                        energyCharge->GetMotionMaster()->MovePoint(POINT_EMPYEREAN_FOCUS, empyrealFocusPosition[impItr.second]);
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case ACTION_DESPAWN_ENERGY_CHARGES:
                    {
                        std::list<Creature*> energyChargesList;
                        GetCreatureListWithEntryInGrid(energyChargesList, me, NPC_ENERGY_CHARGE, 100.0f);

                        if (phase == PHASE_2)
                        {
                            Talk(TALK_C_TO_A_1);
                            DoAction(ACTION_RESET_DRAWING_POWER);
                            events.ScheduleEvent(EVENT_DESPAWN_PLATFORM, 6000);
                            events.ScheduleEvent(EVENT_LAUNCH_COSMIC_SPARK, 6000);
                        }

                        me->AddAura(SPELL_UNSTABLE_ENERGY, me);
                        me->AddAura(SPELL_PHASE_SHIFTED, me);

                        phase = PHASE_3;

                        for (auto&& itr : energyChargesList)
                            itr->DespawnOrUnsummon(50);

                        break;
                    }
                    case ACTION_EMPYREAL_FOCUS_KILLED:
                    {
                        HandleStraitSix();

                        empyrealFocusKilled++;

                        if (empyrealFocusKilled < 6)
                            break;

                        empyrealFocusKilled = 0;

                        events.ScheduleEvent(EVENT_END_OF_PHASE_3, 6000);

                        Talk(TALK_A_TO_B_1);

                        break;
                    }
                    case ACTION_3:
                        events.ScheduleEvent(EVENT_CATASTROPHIC, 1000);
                        break;
                    default:
                        break;
                }
            }

            void HandleStraitSix()
            {
                auto now = TimeValue::Now();
                if (empyrealFocusKilledInRow == 0)
                    ++empyrealFocusKilledInRow;
                else
                {
                    if (empyrealFocusKilledLastTime + Seconds(10) >= now)
                    {
                        if (++empyrealFocusKilledInRow == 6)
                            me->GetMap()->SetWorldState(WORLD_STATE_STRAIGHT_SIX, 1);
                    }
                    else
                        empyrealFocusKilledInRow = 1;
                }

                empyrealFocusKilledLastTime = now;
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_ENERGY_CHARGE)
                    energyChargeCounter++;
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);

                if (summon->GetEntry() == NPC_ENERGY_CHARGE && energyChargeCounter > 0)
                    energyChargeCounter--;
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (phase == PHASE_1 && me->HealthBelowPctDamaged(nextPhase1EndingHealthPct, damage))
                {
                    phase = PHASE_2;
                    canGrasp = true;
                    events.CancelEvent(EVENT_CHECK_MELEE);
                    phase2WaveCount = 0;

                    Talk(TALK_B_TO_C_1);

                    if (nextPhase1EndingHealthPct == 85.0f)
                        nextPhase1EndingHealthPct = 50.0f;
                    else if (nextPhase1EndingHealthPct == 50.0f)
                    {
                        events.ScheduleEvent(EVENT_RADIATING_ENERGIES, 1000);
                        nextPhase1EndingHealthPct = 0.0f;
                    }

                    events.ScheduleEvent(EVENT_DRAW_POWER,             1000);
                }
                else if (damage > me->GetHealth())
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY|MOVEMENTFLAG_FLYING|MOVEMENTFLAG_CAN_FLY);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SetBossState(DATA_ELEGON, DONE);

                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    Map::PlayerList const& playerList = instance->instance->GetPlayers();
                    if (!playerList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                        {
                            if (Player* player = itr->GetSource())
                            {
                                if (player->IsGameMaster())
                                    continue;

                                if (player->IsAlive())
                                {
                                    player->CombatStop();
                                    player->CombatStopWithPets(true);
                                }
                            }
                        }
                    }
                }

                me->SetCanFly(false);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveLand(2, middlePos);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOUCH_OF_THE_TITANS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOUCH_OF_TITANS_VISUAL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCHARGED_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLOSED_CIRCUIT);
                }

                me->RemoveAurasDueToSpell(SPELL_RADIATING_ENERGIES_VISUAL);
                me->RemoveAurasDueToSpell(SPELL_OVERCHARGED_AURA);
                me->RemoveAurasDueToSpell(SPELL_OVERCHARGED);
                me->RemoveAurasDueToSpell(SPELL_TOUCH_OF_THE_TITANS);
                Talk(TALK_DEATH);

                if (Creature* infiniteEnergy = instance->instance->GetCreature(instance->GetGuidData(NPC_INFINITE_ENERGY)))
                    infiniteEnergy->AI()->DoAction(ACTION_INFINITE_LOOT);

                if (GameObject* door = instance->instance->GetGameObject(instance->GetGuidData(GO_ELEGON_DOOR_ENTRANCE)))
                    if (door->GetGoState() == GO_STATE_READY)
                        door->SetGoState(GO_STATE_ACTIVE);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (who->HasAura(SPELL_TOUCH_OF_THE_TITANS) || who->HasAura(SPELL_TOUCH_OF_TITANS_VISUAL) || who->HasAura(SPELL_OVERCHARGED_AURA))
                    return;

                if (me->GetExactDist2d(who) < 42.7f)
                {
                    if (Player* player = who->ToPlayer())
                    {
                        if (player->IsGameMaster())
                            return;

                        if (player->IsAlive())
                        {
                            player->CastSpell(player, SPELL_TOUCH_OF_THE_TITANS, true);
                            player->AddAura(SPELL_TOUCH_OF_TITANS_VISUAL, player);
                            player->CastSpell(player, SPELL_OVERCHARGED, true);
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (IsHeroic())
                    HandleRemoveDestabilizeEnergy();

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                            // Phase 1
                        case EVENT_CHECK_MELEE:
                        {
                            // First check victim if come
                            if (Unit* vict = me->GetVictim())
                            {
                                if (me->IsWithinMeleeRange(vict, 24.0f))
                                {
                                    canGrasp = false;
                                    break;
                                }
                                // Second check any player in 24.0f and try to set his on victim
                                else if (Player* pItr = me->FindNearestPlayer(24.0f))
                                {
                                    if (!me->IsWithinMeleeRange(pItr, 24.0f))
                                        HandleGrasp();

                                    me->AI()->AttackStart(pItr);
                                    me->SetInCombatWith(pItr);
                                    me->GetThreatManager().addThreat(pItr, 100.0f);
                                    canGrasp = false;
                                    break;
                                }
                                else
                                    HandleGrasp();
                            }
                            break;
                        }
                        case EVENT_CELESTIAL_BREATH:
                        {
                            if (phase == PHASE_1)
                                if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                {
                                    me->SetFacingTo(me->GetAngle(target));
                                    me->CastSpell(target, SPELL_CELESTIAL_BREATH, false);
                                }

                            events.ScheduleEvent(EVENT_CELESTIAL_BREATH, 18 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_MATERIALIZE_PROTECTOR:
                        {
                            if (phase == PHASE_1 && nextPhase1EndingHealthPct > 0.0f)
                                me->CastSpell(me, SPELL_MATERIALIZE_PROTECTOR, false);

                            events.ScheduleEvent(EVENT_MATERIALIZE_PROTECTOR, urand(30000, 32000));
                            break;
                        }
                        case EVENT_RADIATING_ENERGIES:
                        {
                            if (me->HasAura(SPELL_RADIATING_ENERGIES))
                                break;

                            if (phase == PHASE_1)
                            {
                                me->CastSpell(me, SPELL_RADIATING_ENERGIES, true);
                                me->CastSpell(me, SPELL_RADIATING_ENERGIES_VISUAL, true);
                                Talk(TALK_ENRAGE_SOFT);
                            }

                            events.ScheduleEvent(EVENT_RADIATING_ENERGIES, 1000);
                            break;
                        }
                        // Phase 2
                        case EVENT_DRAW_POWER:
                        {
                            if (phase == PHASE_2)
                            {
                                if (!successfulDrawingPower)
                                    me->CastSpell(me, SPELL_DRAW_POWER_LONG, false);
                                else
                                    me->CastSpell(me, SPELL_DRAW_POWER_SHORT, false);

                                successfulDrawingPower++;
                            }

                            events.ScheduleEvent(EVENT_FOCUS_POWER, 100); // We don't decount when casting
                            break;
                        }
                        case EVENT_FOCUS_POWER:
                        {
                            if (phase == PHASE_2)
                                if (energyChargeCounter)
                                    me->CastSpell(me, SPELL_FOCUS_POWER, false);

                            events.ScheduleEvent(EVENT_DRAW_POWER, 100); // We don't decount when casting
                            break;
                        }
                        // Phase 3
                        case EVENT_DESPAWN_PLATFORM:
                        {
                            if (phase == PHASE_3)
                            {
                                Talk(TALK_C_TO_A_2);
                                me->OverrideInhabitType(INHABIT_AIR);
                                me->UpdateMovementFlags();

                                if (instance)
                                {
                                    if (GameObject* energyPlatform = instance->instance->GetGameObject(instance->GetGuidData(GO_ENERGY_PLATFORM)))
                                        energyPlatform->SetGoState(GO_STATE_ACTIVE);

                                    std::list<GameObject*> titanCircles1;
                                    std::list<GameObject*> titanCircles2;
                                    std::list<GameObject*> titanCircles3;
                                    GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_1, 100.0f);
                                    GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_2, 100.0f);
                                    GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_3, 100.0f);

                                    for (auto&& titanCircle : titanCircles1)
                                        titanCircle->SetGoState(GO_STATE_ACTIVE);
                                    for (auto&& titanCircle : titanCircles2)
                                        titanCircle->SetGoState(GO_STATE_ACTIVE);
                                    for (auto&& titanCircle : titanCircles3)
                                        titanCircle->SetGoState(GO_STATE_ACTIVE);

                                    std::list<GameObject*> moguRune;

                                    for (uint32 entry = GO_MOGU_RUNE_FIRST; entry < GO_MOGU_RUNE_END; entry++)
                                    {
                                        GetGameObjectListWithEntryInGrid(moguRune, me, entry, 500.0f);

                                        for (auto&& itr : moguRune)
                                            itr->SetGoState(GO_STATE_ACTIVE);

                                        moguRune.clear();
                                    }
                                }
                            }

                            events.ScheduleEvent(EVENT_AFTER_DESPAWN_PLATFORM, 2000);

                            break;
                        }
                        case EVENT_AFTER_DESPAWN_PLATFORM:
                        {
                            if (phase == PHASE_3)
                                Talk(TALK_C_TO_A_3);

                            break;
                        }
                        case EVENT_END_OF_PHASE_3:
                        {
                            if (phase == PHASE_3)
                            {
                                Talk(TALK_A_TO_B_2);
                                me->OverrideInhabitType(INHABIT_GROUND);
                                me->UpdateMovementFlags();

                                if (instance)
                                {
                                    if (GameObject* energyPlatform = instance->instance->GetGameObject(instance->GetGuidData(GO_ENERGY_PLATFORM)))
                                        energyPlatform->SetGoState(GO_STATE_READY);

                                    std::list<GameObject*> titanCircles1;
                                    std::list<GameObject*> titanCircles2;
                                    std::list<GameObject*> titanCircles3;
                                    GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_1, 100.0f);
                                    GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_2, 100.0f);
                                    GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_3, 100.0f);

                                    for (auto&& titanCircle : titanCircles1)
                                        titanCircle->SetGoState(GO_STATE_READY);
                                    for (auto&& titanCircle : titanCircles2)
                                        titanCircle->SetGoState(GO_STATE_READY);
                                    for (auto&& titanCircle : titanCircles3)
                                        titanCircle->SetGoState(GO_STATE_READY);

                                    std::list<GameObject*> moguRune;

                                    for (uint32 entry = GO_MOGU_RUNE_FIRST; entry < GO_MOGU_RUNE_END; entry++)
                                    {
                                        GetGameObjectListWithEntryInGrid(moguRune, me, entry, 500.0f);

                                        for (auto&& itr : moguRune)
                                            itr->SetGoState(GO_STATE_READY);

                                        moguRune.clear();
                                    }
                                }

                                phase = PHASE_1;

                                me->RemoveAurasDueToSpell(SPELL_UNSTABLE_ENERGY);
                                me->RemoveAurasDueToSpell(SPELL_PHASE_SHIFTED);
                                me->RemoveAurasDueToSpell(SPELL_OVERLOADED);
                            }

                            break;
                        }
                        case EVENT_LAUNCH_COSMIC_SPARK:
                        {
                            if (phase == PHASE_3)
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                    me->CastSpell(target, SPELL_ENERGY_CASCADE, false);

                            events.ScheduleEvent(EVENT_LAUNCH_COSMIC_SPARK, 5000);
                            break;
                        }
                        // Others
                        case EVENT_ENRAGE_HARD:
                        {
                            me->CastSpell(me, SPELL_BERSERK, true);
                            events.ScheduleEvent(EVENT_CATASTROPHIC_01, 5000);
                            Talk(TALK_ENRAGE_HARD);
                            break;
                        }
                        case EVENT_CATASTROPHIC:
                             DoCast(127341);
                            break;
                        case EVENT_CATASTROPHIC_01:
                             DoCast(132256);
                            break;
                        default:
                            break;
                    }
                }

                if (Unit* vict = me->GetVictim())
                {
                    if (!me->IsWithinMeleeRange(vict, 24.0f) && phase == PHASE_1 && !canGrasp)
                    {
                        canGrasp = true;
                        events.ScheduleEvent(EVENT_CHECK_MELEE, urand(3500, 5000));
                    }
                }

                if (phase == PHASE_1)
                    DoMeleeAttackIfReady();
            }

            private:
                void HandleGrasp()
                {
                    if (phase != PHASE_1)
                        return;

                    me->CastSpell(me, SPELL_GRASPING_ENERGY_TENDRILS, false);
                    events.ScheduleEvent(EVENT_CHECK_MELEE, 500);
                }

                void HandleRemoveDestabilizeEnergy()
                {
                    std::list<Player*> pList;
                    GetPlayerListInGrid(pList, me, 200.0f);
                    pList.remove_if([=](Player* target) { return target && !target->HasAura(SPELL_DESTABILIZING_ENERGIES); });

                    for (auto&& itr : pList)
                        if (itr->IsAlive() && itr->GetHealthPct() >= 80.0f)
                            itr->RemoveAurasDueToSpell(SPELL_DESTABILIZING_ENERGIES);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_elegonAI(creature);
        }
};

// Empyreal Focus - 60776
class npc_empyreal_focus : public CreatureScript
{
    public:
        npc_empyreal_focus() : CreatureScript("npc_empyreal_focus") { }

        struct npc_empyreal_focusAI : public ScriptedAI
        {
            npc_empyreal_focusAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                instance = creature->GetInstanceScript();
            }

            EventMap events;
            bool activationDone;
            InstanceScript* instance;
            ObjectGuid targetfocusGUID;
            uint16 CheckTimer;
            bool active;

            void Reset() override
            {
                events.Reset();
                CheckTimer = 1000;
                activationDone = false;
                active = false;

                me->SetReactState(REACT_PASSIVE);
                me->AddAura(SPELL_FOCUS_INACTIVE, me);
                me->RemoveAurasDueToSpell(132257);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == 116598)
                    targetfocusGUID = caster->GetGUID();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ACTIVATE_EMPYREAL_FOCUS:
                    {
                        if (activationDone)
                            break;

                        events.ScheduleEvent(EVENT_ACTIVATE_EMPYREAL_FOCUS, 3000);

                        me->CastSpell(me, SPELL_FOCUS_ACTIVATION_VISUAL, true);

                        if (instance && !activationDone)
                            if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                                elegon->AI()->DoAction(ACTION_DESPAWN_ENERGY_CHARGES);

                            activationDone = true;

                        std::list<Creature*> imperialList;
                        GetCreatureListWithEntryInGrid(imperialList, me, NPC_EMPYREAL_FOCUS, 200.0f);

                        for (auto&& it : imperialList)
                            it->AI()->DoAction(ACTION_ACTIVATE_EMPYREAL_FOCUS);

                        break;
                    }
                    case ACTION_RESET_EMPYREAL_FOCUS:
                    {
                        me->RemoveAurasDueToSpell(SPELL_FOCUS_ACTIVATE_STATE);
                        me->RemoveAurasDueToSpell(SPELL_FOCUS_LIGHT_WALL_VISUAL);
                        me->RemoveAurasDueToSpell(SPELL_FOCUS_LIGHT_CASTBAR);

                        me->SetHealth(me->GetMaxHealth());

                        if (AreaTrigger* lightningWall = me->GetAreaTrigger(SPELL_FOCUS_LIGHT_AREATRIGGER))
                            lightningWall->SetDuration(0);

                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        activationDone = false;
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (me->GetHealth() < damage)
                {
                    damage = 0;

                    activationDone = false;
                    active = true;
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    //me->RemoveAurasDueToSpell(SPELL_FOCUS_ACTIVATE_STATE);
                    me->RemoveAurasDueToSpell(SPELL_FOCUS_LIGHT_WALL_VISUAL);
                    me->RemoveAurasDueToSpell(SPELL_FOCUS_LIGHT_CASTBAR);
                    DoCast(me, 132257, true);
                    me->SetHealth(me->GetMaxHealth());

                    if (AreaTrigger* lightningWall = me->GetAreaTrigger(SPELL_FOCUS_LIGHT_AREATRIGGER))
                        lightningWall->SetDuration(0);

                    if (instance)
                    {
                        if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                        {
                            me->CastSpell(elegon, SPELL_OVERLOADED_MISSILE, false);
                            elegon->AI()->DoAction(ACTION_EMPYREAL_FOCUS_KILLED);
                        }
                    }
                }
            }

            void DeactivateFocus()
            {
                Creature* focus = me->GetCreature(*me, targetfocusGUID);
                if (!focus)
                    return;

                if (me->HasAura(132257) && focus->HasAura(132257))
                {
                    active = false;
                    focus->RemoveAurasDueToSpell(132257);
                    me->RemoveAurasDueToSpell(132257);
                    focus->RemoveAurasDueToSpell(SPELL_FOCUS_ACTIVATE_STATE);
                    me->RemoveAurasDueToSpell(SPELL_FOCUS_ACTIVATE_STATE);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (CheckTimer <= diff)
                {
                    if (active)
                        DeactivateFocus();
                    CheckTimer = 1000;
                }
                else
                    CheckTimer -= diff;

                switch (events.ExecuteEvent())
                {
                    case EVENT_ACTIVATE_EMPYREAL_FOCUS:
                        me->CastSpell(me, SPELL_FOCUS_ACTIVATE_STATE, true);
                        events.ScheduleEvent(EVENT_APPEAR_WALL_OF_LIGHTNING, 3000);
                        break;
                    case EVENT_APPEAR_WALL_OF_LIGHTNING:
                    {
                        me->CastSpell(me, SPELL_FOCUS_LIGHT_AREATRIGGER, true);                        
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                        Position pos = me->GetPosition();
                        for (int i = 0; i < 6; i++)
                        {
                            if (pos.GetPositionX() - empyrealFocusPosition[i].GetPositionX() <= 4.0f &&
                                pos.GetPositionX() - empyrealFocusPosition[i].GetPositionX() >= -4.0f &&
                                pos.GetPositionY() - empyrealFocusPosition[i].GetPositionY() <= 4.0f &&
                                pos.GetPositionY() - empyrealFocusPosition[i].GetPositionY() >= -4.0f)
                            {
                                switch (i)
                                {
                                case 0:
                                    empyrealFocus[1] = me->GetGUID();//����
                                    break;
                                case 1:
                                    empyrealFocus[3] = me->GetGUID();//����
                                    break;
                                case 2:
                                    empyrealFocus[5] = me->GetGUID();//����
                                    break;
                                case 3:
                                    empyrealFocus[4] = me->GetGUID();//����
                                    break;
                                case 4:
                                    empyrealFocus[2] = me->GetGUID();//�ϱ�
                                    break;
                                case 5:
                                    empyrealFocus[0] = me->GetGUID();  //����
                                    break;
                                }

                                break;
                            }
                        }        

                        if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                        {
                            if (Creature* empyrealfocus = instance->instance->GetCreature(empyrealFocus[0]))
                            {
                                if (Creature* empyrealfocus05 = instance->instance->GetCreature(empyrealFocus[5]))
                                {
                                    empyrealfocus->CastSpell(empyrealfocus05, 116598, true);
                                    empyrealfocus05->CastSpell(empyrealfocus, 116598, true);
                                }
                            }
                            if (Creature* empyrealfocus01 = instance->instance->GetCreature(empyrealFocus[1]))
                            {
                                if (Creature* empyrealfocus04 = instance->instance->GetCreature(empyrealFocus[4]))
                                {
                                    empyrealfocus01->CastSpell(empyrealfocus04, 116598, true);
                                    empyrealfocus04->CastSpell(empyrealfocus01, 116598, true);
                                }
                            }
                            if (Creature* empyrealfocus02 = instance->instance->GetCreature(empyrealFocus[2]))
                            {
                                if (Creature* empyrealfocus03 = instance->instance->GetCreature(empyrealFocus[3]))
                                {
                                    empyrealfocus02->CastSpell(empyrealfocus03, 116598, true);
                                    empyrealfocus03->CastSpell(empyrealfocus02, 116598, true);
                                }
                            }
                        }                        
                        
                    }
                        break;
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_empyreal_focusAI(creature);
        }
};

// Celestial Protector - 60793
class npc_celestial_protector : public CreatureScript
{
    public:
        npc_celestial_protector() : CreatureScript("npc_celestial_protector") { }

        struct npc_celestial_protectorAI : public ScriptedAI
        {
            npc_celestial_protectorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool stabilityFluxCasted;
            bool totalAnnihilationCasted;

            void IsSummonedBy(Unit* summoner) override
            {
                stabilityFluxCasted     = false;
                totalAnnihilationCasted = false;
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

                if (Player* player = me->FindNearestPlayer(80.0f))
                {
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MoveChase(player);
                    me->Attack(player, true);
                }
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARCING_ENERGY, 10000);
                events.ScheduleEvent(EVENT_CHECK_UNIT_ON_PLATFORM, 1000);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!stabilityFluxCasted)
                {
                    if (me->HealthBelowPctDamaged(25, damage))
                    {
                        me->CastSpell(me, SPELL_STABILITY_FLUX, false);
                        stabilityFluxCasted = true;
                    }
                }

                if (me->GetHealthPct() <= 2.0f)
                {
                    damage = 0;

                    if (!totalAnnihilationCasted)
                    {
                        uint32 delay = 0;
                        me->PrepareChanneledCast(me->GetOrientation());
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_CAN_SWIM);
                        me->m_Events.Schedule(delay += 500, 1, [this]()
                        {
                            DoCast(me, SPELL_TOTAL_ANNIHILATION);
                        });

                        totalAnnihilationCasted = true;
                        events.Reset();
                        events.ScheduleEvent(EVENT_KILLED , 5500);
                    }
                }
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
                        case EVENT_CHECK_UNIT_ON_PLATFORM:
                        {
                            if (me->GetExactDist2d(&middlePos) <= 42.7f)
                            {
                                if (me->GetHealthPct() > 25.0f)
                                {
                                    me->AddAura(SPELL_ECLIPSE_PHASE, me);
                                    me->RemoveAurasDueToSpell(SPELL_ECLIPSE_NO_PHASE);
                                }
                                else
                                {
                                    me->RemoveAurasDueToSpell(SPELL_ECLIPSE_NO_PHASE);
                                    me->RemoveAurasDueToSpell(SPELL_ECLIPSE_PHASE);
                                }

                                me->CastSpell(me, SPELL_TOUCH_OF_THE_TITANS, true);
                            }
                            else
                            {
                                if (me->GetHealthPct() > 25.0f)
                                {
                                    me->AddAura(SPELL_ECLIPSE_NO_PHASE, me);
                                    me->RemoveAurasDueToSpell(SPELL_ECLIPSE_PHASE);
                                }
                                else
                                {
                                    me->RemoveAurasDueToSpell(SPELL_ECLIPSE_NO_PHASE);
                                    me->RemoveAurasDueToSpell(SPELL_ECLIPSE_PHASE);
                                }

                                me->RemoveAurasDueToSpell(SPELL_TOUCH_OF_THE_TITANS);
                            }
                            events.ScheduleEvent(EVENT_CHECK_UNIT_ON_PLATFORM, 1000);
                            break;
                        }
                        case EVENT_ARCING_ENERGY:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_ARCING_ENERGY, false);
                            events.ScheduleEvent(EVENT_ARCING_ENERGY,      30000);
                            break;
                        }
                        case EVENT_KILLED:
                            me->Kill(me);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_celestial_protectorAI(creature);
        }
};

// Cosmic Spark - 62618
class npc_cosmic_spark : public CreatureScript
{
    public:
        npc_cosmic_spark() : CreatureScript("npc_cosmic_spark") { }

        struct npc_cosmic_sparkAI : public ScriptedAI
        {
            npc_cosmic_sparkAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_AGGRESSIVE);
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_CHECK_UNIT_ON_PLATFORM, 1000);
                events.ScheduleEvent(EVENT_ICE_TRAP, 10000);

                std::list<Player*> plrList;
                me->GetPlayerListInGrid(plrList, 170.0f);
                if (!plrList.empty())
                {
                    plrList.sort(Trinity::ObjectDistanceOrderPred(me));
                    plrList.resize(1);
                    if (Unit* target = plrList.front())
                    {
                        me->AddThreat(target, 150.0f);
                        AttackStart(target);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_UNIT_ON_PLATFORM:
                            if (me->GetExactDist2d(&middlePos) <= 42.7f)
                                me->CastSpell(me, SPELL_TOUCH_OF_THE_TITANS, true);
                            else
                                me->RemoveAurasDueToSpell(SPELL_TOUCH_OF_THE_TITANS);
                            events.ScheduleEvent(EVENT_CHECK_UNIT_ON_PLATFORM, 1000);
                            break;
                        case EVENT_ICE_TRAP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_ICE_TRAP, false);
                            events.ScheduleEvent(EVENT_ICE_TRAP,      60000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_cosmic_sparkAI(creature);
        }
};

enum energyChargeActions
{
    ACTION_ENERGIZE_EMPYREAL_FOCUS  = 1,
};

// Energy Charge - 60913
class npc_energy_charge : public CreatureScript
{
    public:
        npc_energy_charge() : CreatureScript("npc_energy_charge") { }

        struct npc_energy_chargeAI : public ScriptedAI
        {
            npc_energy_chargeAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
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
                me->SetReactState(REACT_PASSIVE);
                me->AddAura(SPELL_CORE_CHECKER, me);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetWalk(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->CastSpell(me, SPELL_DISCHARGE, true);
            }

            void MovementInform(uint32 /*type*/, uint32 pointId) override
            {
                switch (pointId)
                {
                    case POINT_EMPYEREAN_FOCUS:
                    {
                        if (Unit* focus = ObjectAccessor::GetUnit(*me, me->GetGuidValue(UNIT_FIELD_TARGET)))
                        {
                            Position pos = focus->GetPosition();

                            me->GetMotionMaster()->MovePoint(POINT_EMPYEREAN_FOCUS, pos);
                        }
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
                    case ACTION_ENERGIZE_EMPYREAL_FOCUS:
                    {
                        if (Unit* focus = ObjectAccessor::GetUnit(*me, me->GetGuidValue(UNIT_FIELD_TARGET)))
                            if (focus->GetAI())
                                focus->GetAI()->DoAction(ACTION_ACTIVATE_EMPYREAL_FOCUS);

                            me->RemoveAurasDueToSpell(SPELL_CORE_CHECKER);
                        me->DespawnOrUnsummon();

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_energy_chargeAI(creature);
        }
};

enum infiniteEvents
{
    EVENT_ACTIVATION_WEST   = 1,
    EVENT_ACTIVATION        = 2,
    EVENT_ACTIVATION_EAST   = 3,
    EVENT_FLASH_SPAWN       = 4,
    EVENT_BOSS_ACTIVATION   = 5,
    EVENT_BOSS_INTRO_2      = 6,
};

// Infinite Energy - 65293
class npc_infinite_energy : public CreatureScript
{
    public:
        npc_infinite_energy() : CreatureScript("npc_infinite_energy"){ }

        struct npc_infinite_energyAI : public ScriptedAI
        {
            npc_infinite_energyAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;

            void Reset() override
            {
                events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INFINITE_GO_DOWN:
                    {
                        me->GetMotionMaster()->MovePoint(0, infiniteEnergyPos);
                        events.ScheduleEvent(EVENT_ACTIVATION_WEST, 3000);
                        break;
                    }
                    case ACTION_INFINITE_ACTIVATION_WEST:
                    {
                        std::list<Creature*> imperialList;
                        GetCreatureListWithEntryInGrid(imperialList, me, NPC_EMPYREAL_FOCUS, 200.0f);

                        for (auto&& it : imperialList)
                            if (ImperialFocusMatch.find(it->GetDBTableGUIDLow())->second == 0 || ImperialFocusMatch.find(it->GetDBTableGUIDLow())->second == 5)
                                it->RemoveAurasDueToSpell(SPELL_FOCUS_INACTIVE);

                        events.ScheduleEvent(EVENT_ACTIVATION, 1000);
                        break;
                    }
                    case ACTION_INFINITE_ACTIVATION:
                    {
                        std::list<Creature*> imperialList;
                        GetCreatureListWithEntryInGrid(imperialList, me, NPC_EMPYREAL_FOCUS, 200.0f);

                        for (auto&& it : imperialList)
                            if (ImperialFocusMatch.find(it->GetDBTableGUIDLow())->second == 4 || ImperialFocusMatch.find(it->GetDBTableGUIDLow())->second == 1)
                                it->RemoveAurasDueToSpell(SPELL_FOCUS_INACTIVE);

                        events.ScheduleEvent(EVENT_ACTIVATION_EAST, 1000);
                        break;
                    }
                    case ACTION_INFINITE_ACTIVATION_EAST:
                    {
                        std::list<Creature*> imperialList;
                        GetCreatureListWithEntryInGrid(imperialList, me, NPC_EMPYREAL_FOCUS, 200.0f);

                        for (auto&& it : imperialList)
                            if (ImperialFocusMatch.find(it->GetDBTableGUIDLow())->second == 3 || ImperialFocusMatch.find(it->GetDBTableGUIDLow())->second == 2)
                                it->RemoveAurasDueToSpell(SPELL_FOCUS_INACTIVE);

                        events.ScheduleEvent(EVENT_FLASH_SPAWN, 1000);
                        break;
                    }
                    case ACTION_INFINITE_FLASH_SPAWN:
                    {
                        me->CastSpell(me, SPELL_SPAWN_FLASH_1_PERIODIC, true);
                        break;
                    }
                    case ACTION_INFINITE_SPAWN_PLATFORM:
                    {
                        if (instance)
                        {
                            if (GameObject* energyPlatform = instance->instance->GetGameObject(instance->GetGuidData(GO_ENERGY_PLATFORM)))
                                energyPlatform->SetGoState(GO_STATE_READY);

                            std::list<GameObject*> titanCircles1;
                            std::list<GameObject*> titanCircles2;
                            std::list<GameObject*> titanCircles3;
                            GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_1, 100.0f);
                            GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_2, 100.0f);
                            GetGameObjectListWithEntryInGrid(titanCircles1, me, GO_ENERGY_TITAN_CIRCLE_3, 100.0f);

                            for (auto&& titanCircle : titanCircles1)
                                titanCircle->SetGoState(GO_STATE_READY);
                            for (auto&& titanCircle : titanCircles2)
                                titanCircle->SetGoState(GO_STATE_READY);
                            for (auto&& titanCircle : titanCircles3)
                                titanCircle->SetGoState(GO_STATE_READY);

                            std::list<GameObject*> moguRune;

                            for (uint32 entry = GO_MOGU_RUNE_FIRST; entry < GO_MOGU_RUNE_END; entry++)
                            {
                                GetGameObjectListWithEntryInGrid(moguRune, me, entry, 500.0f);

                                for (auto&& itr : moguRune)
                                    itr->SetGoState(GO_STATE_READY);

                                moguRune.clear();
                            }
                        }
                        break;
                    }
                    case ACTION_INFINITE_SPAWN_BOSS:
                    {
                        if (instance)
                        {
                            if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                            {
                                elegon->RestoreDisplayId();
                                elegon->AI()->Talk(TALK_INTRO);
                                events.ScheduleEvent(EVENT_BOSS_INTRO_2,    5000);
                                events.ScheduleEvent(EVENT_BOSS_ACTIVATION, 8000);
                            }
                        }
                        break;
                    }
                    case ACTION_INFINITE_LOOT:
                    {
                        // Loots chest
                        if (IsHeroic())
                            me->SummonGameObject(GO_ELEGON_CHEST_HEROIC, middlePos.GetPositionX(), middlePos.GetPositionY(), middlePos.GetPositionZ(), 0.0f, { }, 0);
                        else
                            me->SummonGameObject(GO_ELEGON_CHEST, middlePos.GetPositionX(), middlePos.GetPositionY(), middlePos.GetPositionZ(), 0.0f, { }, 0);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_ACTIVATION_WEST:
                    {
                        me->AI()->DoAction(ACTION_INFINITE_ACTIVATION_WEST);
                        break;
                    }
                    case EVENT_ACTIVATION:
                    {
                        me->AI()->DoAction(ACTION_INFINITE_ACTIVATION);
                        break;
                    }
                    case EVENT_ACTIVATION_EAST:
                    {
                        me->AI()->DoAction(ACTION_INFINITE_ACTIVATION_EAST);
                        break;
                    }
                    case EVENT_FLASH_SPAWN:
                    {
                        me->AI()->DoAction(ACTION_INFINITE_FLASH_SPAWN);
                        break;
                    }
                    case EVENT_BOSS_ACTIVATION:
                    {
                        if (instance)
                        {
                            if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                            {
                                elegon->OverrideInhabitType(INHABIT_GROUND);
                                elegon->UpdateMovementFlags();
                                elegon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_CAN_SWIM | UNIT_FLAG_NOT_SELECTABLE);
                            }
                        }

                            break;
                    }
                    case EVENT_BOSS_INTRO_2:
                    {
                        if (instance)
                            if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                                elegon->AI()->Talk(TALK_INTRO_2);
                            break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_infinite_energyAI(creature);
        }
};

// Celestial Control Console - 211650
class go_celestial_control_console : public GameObjectScript
{
    public:
        go_celestial_control_console() : GameObjectScript("go_celestial_control_console") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (!instance->CheckRequiredBosses(DATA_ELEGON, player) && !player->IsGameMaster())
                    return false;

                if (Creature* infiniteEnergy = instance->instance->GetCreature(instance->GetGuidData(NPC_INFINITE_ENERGY)))
                    infiniteEnergy->AI()->DoAction(ACTION_INFINITE_GO_DOWN);

                if (GameObject* titanDisk = instance->instance->GetGameObject(instance->GetGuidData(GO_ENERGY_TITAN_DISK)))
                    titanDisk->SetGoState(GO_STATE_READY);

                go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }

            return false;
        }
};

// Spawn Flash 1 Periodic - 127785
class spell_spawn_flash_1_periodic : public SpellScriptLoader
    {
    public:
        spell_spawn_flash_1_periodic() : SpellScriptLoader("spell_spawn_flash_1_periodic") { }

        class spell_spawn_flash_1_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spawn_flash_1_periodic_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
                    target->CastSpell(target, SPELL_SPAWN_FLASH_2_PERIODIC, true);
                    target->SetDisplayId(11686); // Invisible
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_spawn_flash_1_periodic_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_spawn_flash_1_periodic_AuraScript();
        }
};

// Spawn Flash 2 Periodic - 127783
class spell_spawn_flash_2_periodic : public SpellScriptLoader
{
    public:
        spell_spawn_flash_2_periodic() : SpellScriptLoader("spell_spawn_flash_2_periodic") { }

        class spell_spawn_flash_2_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spawn_flash_2_periodic_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
                    target->CastSpell(target, SPELL_SPAWN_FLASH_3_PERIODIC, true);

                    if (target->GetAI())
                        target->GetAI()->DoAction(ACTION_INFINITE_SPAWN_PLATFORM);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_spawn_flash_2_periodic_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_spawn_flash_2_periodic_AuraScript();
        }
};

// Spawn Flash 3 Periodic - 127781
class spell_spawn_flash_3_periodic : public SpellScriptLoader
{
    public:
        spell_spawn_flash_3_periodic() : SpellScriptLoader("spell_spawn_flash_3_periodic") { }

        class spell_spawn_flash_3_periodic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spawn_flash_3_periodic_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                    if (target->GetAI())
                        target->GetAI()->DoAction(ACTION_INFINITE_SPAWN_BOSS);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_spawn_flash_3_periodic_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_spawn_flash_3_periodic_AuraScript();
        }
};

// Touch of Titans - 117874
class spell_touch_of_titans : public SpellScriptLoader
{
    public:
        spell_touch_of_titans() : SpellScriptLoader("spell_touch_of_titans") { }

        class spell_touch_of_titans_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_touch_of_titans_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (!GetTarget())
                    return;

                if (Player* player = GetTarget()->ToPlayer())
                {
                    if (player->GetExactDist2d(&middlePos) > 42.7f)
                    {
                        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_THE_TITANS);
                        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_TITANS_VISUAL);
                        player->RemoveAurasDueToSpell(SPELL_OVERCHARGED_AURA);
                        player->RemoveAurasDueToSpell(SPELL_OVERCHARGED);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
                    target->RemoveAurasDueToSpell(SPELL_TOUCH_OF_THE_TITANS);
                    target->RemoveAurasDueToSpell(SPELL_TOUCH_OF_TITANS_VISUAL);
                    target->RemoveAurasDueToSpell(SPELL_OVERCHARGED_AURA);
                    target->RemoveAurasDueToSpell(SPELL_OVERCHARGED);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_touch_of_titans_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectRemove += AuraEffectRemoveFn(spell_touch_of_titans_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_touch_of_titans_AuraScript();
        }
};

// Radiating Energies - 118313 (outside) or Radiating Energies - 122741 (inside)
class spell_radiating_energies : public SpellScriptLoader
{
    public:
        spell_radiating_energies() : SpellScriptLoader("spell_radiating_energies") { }

        class spell_radiating_energies_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_radiating_energies_SpellScript);

            void CorrectRange(std::list<WorldObject*>& targets)
            {
                targets.clear();

                float const maxDist = GetSpellInfo()->Id == SPELL_RADIATING_INSIDE_VORTEX ? 36.0f : 200.0f;
                float const minDist = GetSpellInfo()->Id == SPELL_RADIATING_OUTSIDE_VORTEX ? 36.0f : 0.0f;

                Map::PlayerList const &players = GetCaster()->GetMap()->GetPlayers();
                for (auto itr = players.begin(); itr != players.end(); ++itr)
                {
                    if (auto const player = itr->GetSource())
                    {
                        auto const dist2d = player->GetExactDist2d(GetCaster()->GetPositionX(), GetCaster()->GetPositionY());
                        if (dist2d <= maxDist && dist2d >= minDist)
                            targets.push_back(player);
                    }
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_radiating_energies_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_radiating_energies_SpellScript();
        }
};

// Draw Power - 119360 and Draw Power - 124967
class spell_draw_power : public SpellScriptLoader
{
    public:
        spell_draw_power() : SpellScriptLoader("spell_draw_power") { }

        class spell_draw_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_draw_power_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* elegon = GetCaster())
                {
                    elegon->GetAI()->DoAction(ACTION_SPAWN_ENERGY_CHARGES);
                    elegon->AddAura(SPELL_DRAW_POWER_DEBUFF, elegon);
                }
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_draw_power_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_draw_power_AuraScript();
        }
};

// Core Checker - 118024
class spell_core_checker : public SpellScriptLoader
{
    public:
        spell_core_checker() : SpellScriptLoader("spell_core_checker") { }

        class spell_core_checker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_core_checker_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* energyCharge = GetTarget())
                {
                    std::list<Creature*> focus;
                    GetCreatureListWithEntryInGrid(focus, energyCharge, NPC_EMPYREAL_FOCUS, 1.0f);

                    if (!focus.empty())
                        if (energyCharge->GetAI())
                            energyCharge->GetAI()->DoAction(ACTION_ENERGIZE_EMPYREAL_FOCUS);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_core_checker_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_core_checker_AuraScript();
        }
};

struct PlayerTargetSelector
{
    bool operator ()(WorldObject const* object) const
    {
        if (object->GetTypeId() == TYPEID_PLAYER)
            return false;

        return true;
    }
};

// Grasping Energy Tendrils - 127362
class spell_grasping_energy_tendrils : public SpellScriptLoader
{
    public:
        spell_grasping_energy_tendrils() : SpellScriptLoader("spell_grasping_energy_tendrils") { }

        class spell_grasping_energy_tendrils_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_grasping_energy_tendrils_SpellScript);

            std::list<WorldObject*> m_targets;

            void HandleOnHit()
            {
                if (Unit* elegon = GetCaster())
                {
                    if (Unit* target = GetHitUnit())
                    {
                        SpellInfo const* m_spellInfo = sSpellMgr->GetSpellInfo(SPELL_GRASPING_ENERGY_GRIP);
                        if (!m_spellInfo)
                            return;

                        // Init dest coordinates
                        float x, y, z;
                        elegon->GetPosition(x, y, z);

                        float dist = target->GetExactDist2d(x, y);

                        float speedZ, speedXY;
                        if (m_spellInfo->Effects[0].MiscValue)
                            speedZ = float(m_spellInfo->Effects[0].MiscValue)/10;
                        else if (m_spellInfo->Effects[0].MiscValueB)
                            speedZ = float(m_spellInfo->Effects[0].MiscValueB)/10;
                        else
                            speedZ = 10.0f;

                        speedXY = dist * 10.0f / speedZ;

                        target->GetMotionMaster()->CustomJump(x, y, z, speedXY, speedZ);
                    }
                }
            }

            void CorrectRange(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PlayerTargetSelector());
                m_targets = targets;
            }

            void CopyTargets(std::list<WorldObject*>& targets)
            {
                targets = m_targets;
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_grasping_energy_tendrils_SpellScript::HandleOnHit);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grasping_energy_tendrils_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_grasping_energy_tendrils_SpellScript::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_grasping_energy_tendrils_SpellScript();
        }
};

class TotalAnnihilationPredicate 
{
    public:
        TotalAnnihilationPredicate(Unit* const m_caster) : _caster(m_caster) { }

        bool operator()(WorldObject* object)
        {
            if (_caster->HasAura(SPELL_TOUCH_OF_THE_TITANS))
                return object && object->ToPlayer() && !object->ToPlayer()->HasAura(SPELL_TOUCH_OF_THE_TITANS);

            return object && object->ToPlayer() && object->ToPlayer()->HasAura(SPELL_TOUCH_OF_THE_TITANS);
        }

    private:
        Unit const* _caster;
};

// Destabilizing Energies - 132222
class spell_destabilizing_energies : public SpellScript
{
    PrepareSpellScript(spell_destabilizing_energies);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if(TotalAnnihilationPredicate(caster));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_destabilizing_energies::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Total Annihilation - 129711
class spell_total_annihilation : public SpellScript
{
    PrepareSpellScript(spell_total_annihilation);

    uint32 targetCount;

    bool Load() override
    {
        targetCount = 0;
        return true;
    }

    void CountTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if(TotalAnnihilationPredicate(caster));

        targetCount = targets.size();
    }

    void CheckTargets()
    {
        if (Unit* caster = GetCaster())
        {
            if (!caster->GetInstanceScript()->instance->IsHeroic())
                return;

            caster->CastSpell(caster, SPELL_DESTABILIZING_ENERGIES, true);
            uint8 diffic = caster->GetMap()->GetDifficulty();

            // In heroic we should handle cast this spell
            caster->CastSpell(caster, SPELL_TOTAL_ANNIHILATION_EFF, true);

            if (!targetCount)
                caster->CastSpell(caster, SPELL_CATASTROPHIC_ANOMALY, false);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_total_annihilation::CountTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        AfterCast += SpellCastFn(spell_total_annihilation::CheckTargets);
    }
};

// Total Annihilation Damage Effect 117914
class spell_total_annihilation_eff : public SpellScript
{
    PrepareSpellScript(spell_total_annihilation_eff);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
        {
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (!instance->instance->IsHeroic())
                    return;

            targets.remove_if(TotalAnnihilationPredicate(caster));
        }
    }

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (!instance->instance->IsHeroic())
                    return;

            if (Unit* target = GetHitUnit())
            {
                if (target->HasAura(SPELL_DESTABILIZE))
                {
                    caster->Kill(target);
                    return;
                }

                target->CastSpell(target, SPELL_DESTABILIZE, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_total_annihilation_eff::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_total_annihilation_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Unstable Energy - 116994
class spell_unstable_energy : public SpellScriptLoader
{
    public:
        spell_unstable_energy() : SpellScriptLoader("spell_unstable_energy") { }

        class spell_unstable_energy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_unstable_energy_AuraScript);

            void OnTick(AuraEffect const* /*aurEff*/)
            {
                if (Unit* elegon = GetTarget())
                    elegon->CastSpell(elegon, SPELL_UNSTABLE_ENERGY_DAMAGE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_unstable_energy_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_unstable_energy_AuraScript();
        }
};

// Areatrigger 8580
// Players should die from AT, not drop damage, cuz for example priest could use levitation and don`t die
class AreaTrigger_under_elegon_platform : public AreaTriggerScript
{
    public:
        AreaTrigger_under_elegon_platform() : AreaTriggerScript("AreaTrigger_under_elegon_platform") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->Kill(player);
            return false;
        }
};

// 306. Summoned by 116546 - Energy Conduit
class sat_mv_energy_conduit : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetAreaTrigger() && triggering->ToPlayer()->IsInAxe(GetCaster(), GetAreaTrigger(), 2.0f);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_FOCUS_LIGHT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_FOCUS_LIGHT_EFF);
    }
};

class spell_stability_flux : public SpellScriptLoader
{
public:
    spell_stability_flux() : SpellScriptLoader("spell_stability_flux") { }

    class spell_stability_flux_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_stability_flux_SpellScript);

        void RecalculateDamage(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetHitUnit())
            {
                if (!GetCaster()->GetInstanceScript()->instance->IsHeroic())
                    SetHitDamage(GetHitDamage());
                else
                {
                    InstanceScript* instance = GetCaster()->GetInstanceScript();
                    if (Creature* elegon = instance->instance->GetCreature(instance->GetGuidData(NPC_ELEGON)))
                        if (elegon->GetExactDist2d(GetCaster()) < 42.7f)
                        {
                            if (target->HasAura(117878))
                                SetHitDamage(GetHitDamage());                                
                            else
                                SetHitDamage(0);
                        }
                        else
                        {
                            if (target->HasAura(117878))
                                SetHitDamage(0);
                            else
                                SetHitDamage(GetHitDamage());
                        }
                }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_stability_flux_SpellScript::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_stability_flux_SpellScript();
    }
};

void AddSC_boss_elegon()
{
    new boss_elegon();
    new npc_empyreal_focus();
    new npc_celestial_protector();
    new npc_cosmic_spark();
    new npc_energy_charge();
    new npc_infinite_energy();
    new go_celestial_control_console();
    new spell_spawn_flash_1_periodic();
    new spell_spawn_flash_2_periodic();
    new spell_spawn_flash_3_periodic();
    new spell_touch_of_titans();
    new spell_radiating_energies();
    new spell_draw_power();
    new spell_core_checker();
    new spell_grasping_energy_tendrils();
    new spell_unstable_energy();
    new spell_stability_flux();
    new spell_script<spell_destabilizing_energies>("spell_destabilizing_energies");
    new spell_script<spell_total_annihilation>("spell_total_annihilation");
    new spell_script<spell_total_annihilation_eff>("spell_total_annihilation_eff");
    new AreaTrigger_under_elegon_platform();
    new atrigger_script<sat_mv_energy_conduit>("sat_mv_energy_conduit");
}
