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
#include "MoveSplineInit.h"
#include "Unit.h"
#include "Player.h"
#include "Creature.h"
#include "InstanceScript.h"
#include "Map.h"
#include "VehicleDefines.h"
#include "SpellInfo.h"
#include "siege_of_orgrimmar.h"

enum Yells
{
    ANN_SPLIT,  // Immerseus [Splits]!
    ANN_REFORM, // Immerseus [Reforms]!
    ANN_SWIRL,  // Immerseus begins to cast [Swirl]!
    TALK_WATER_DEFENDERS_FALLEN,
};

enum Spells
{
    // Misc - General.
    SPELL_ZERO_POWER            =  72242, // No Energy (Corruption) Regen.
    SPELL_SUBMERGE              = 121541, // Submerge visual (when casted makes boss submerge).
    SPELL_EMERGE                = 139832, // Submerged visual (when removed makes boss emerge).

    SPELL_SHA_BOLT_DUMMY        = 143290, // Dummy for SPELL_SHA_BOLT on eff 0 to cast it on all players in 500 yd.
    SPELL_SHA_BOLT              = 143293, // Triggers missile 143295 (summons NPC_SHA_BOLT).
    SPELL_SWIRL                 = 143309, // Applies Areatrigger 1018 (eff 1) on summoned NPC_SWIRL_TARGET - seems visual for water burst on boss. Eff 0 dummy aura.
    SPELL_SWIRL_EFF             = 143412, // On players coming into contact with waters near Immerseus. Periodic damage aura every 0.25 seconds.
    SPELL_SWIRL_SPRAY           = 143415, // Script effect (eff 0) for casting SPELL_SWIRL_ROOM on all triggers in room (NPC_SWIRL - "cracks in the ground").
    SPELL_SWIRL_SPRAY_AT        = 143410, // Applies Areatrigger 1024 on all NPC_SWIRL ground crack trigger mobs (visual).
    SPELL_SWIRL_SPRAY_EFF       = 143413, // On players coming into contact with waters on NPC_SWIRL "ground crack" trigger mobs. Periodic damage aura every 0.25 seconds.
    SPELL_CORROSIVE_BLAST       = 143436, // Damage + Shadow damage taken increase 45 sec.

    // Out of melee range version.
    SPELL_CORROSIVE_BLAST_OUM   = 143437, // Each sec.

    // Swelling Corruption - Heroic only!
    // Handled directly in the script, left here as reminder of spells connections.
    SPELL_SWELLING_CORRUPTION   = 143574, // Heroic only! Boss aura for checking player ability attacks (Summons Congealed Sha). Stacks = boss Corruption level. Dummy on eff 0 + 1.
    SPELL_SWELLING_CORRUPTION_A = 143578, // Announce for DBM
    SPELL_SHA_CORRUPTION_DMG    = 143579, // Periodic damage aura on players when they strike Immerseus with abilities. Stacks.
    SPELL_SHA_CORRUPTION_SUMM   = 143580, // Triggers SPELL_SWELLING_CORRUPTION_S missile when players strike Immerseus with abilities. Eff 1 on SPELL_SWELLING_CORUPTION.
    SPELL_SWELLING_CORRUPTION_S = 143581, // Summons NPC_CONGEALED_SHA.

    // Phase 2 - Split.

    // Split.
    SPELL_SPLIT                 = 143020, // On eff. 0 stun, eff. 1 Script Effect for add - summoning missiles.
    SPELL_REFORM                = 143469, // On eff. 0 Script Effect for setting Health and Corruption points.

    // Missiles and corresponding add - summoning spells.
    SPELL_SPLIT_SHA_MISSILE     = 143022, // Triggers SPELL_SPLIT_SHA.
    SPELL_SPLIT_SHA             = 143453, // Summons Sha Puddle at location.

    SPELL_SPLIT_CONTAM_MISSILE  = 143024, // Triggers SPELL_SPLIT_CONTAM.
    SPELL_SPLIT_CONTAM          = 143454, // Summons Contaminated Puddle at location.

    // When any Puddle reaches Immerseus, it triggers an eruption, inflicting:
    SPELL_ERRUPTING_SHA         = 143498, // 73125 to 76875 Shadow damage to all players for a Sha or Contaminated Puddle;
    SPELL_ERRUPTING_WATER       = 145377, // 29250 to 30750 Frost damage to all players for a Purified Puddle.

    // When the puddles are destroyed (Sha) / healed fully (Contaminated) before they reach Immerseus, the give to players:
    SPELL_SHA_RESIDUE           = 143459, // Sha Puddle. Applies Sha Residue to enemies within 10 yards. Increases damage dealt to Sha Puddles by 25%. Stacks.
    SPELL_PURIFIED_RESIDUE      = 143524, // Contaminated Puddle. Restores 25% of allies within 10 yards mana. Increases healing done by 75% (also 10 yd).
    SPELL_SHA_SLIME             = 143458,

    // Sha Pool - Heroic only!.
    SPELL_SHA_POOL_AURA         = 143462, // Triggers 143460 Periodic damage aura on eff 0, SPELL_SHA_POOL_CHECK_GROW script effect 100 yd on eff 1 (check players and grow / shrink).
    SPELL_SHA_POOL_CHECK_GROW   = 143461,

    // Phase 1 + Phase 2 (General).
    SPELL_SEEPING_SHA           = 143281, // Create Areatrigger 1016 (around Immerseus). 40 yards diameter, base water visual.
    SPELL_SEEPING_SHA_EFF       = 143286, // Coming into contact with the Seeping Sha that surrounds Immerseus inflicts 97500 to 102500 Shadow damage and knocks players back. 

    SPELL_BERSERK               =  64238, // Berserk, Enrage, Bye - Bye or, simply put, a wipe. :)

    SPELL_SHA_SPLASH            = 143298, // Creates Areatrigger 1017.
    SPELL_SHA_SPLASH_EFF        = 143297, // Periodic damage aura on player when splash occurs.

    SPELL_CONGEALING_AURA       = 143538, // Check health every 500 ms (Periodic Dummy eff 0) and add SPELL_CONGEALING stacks.
    SPELL_CONGEALING            = 143540, // 5% Size mod. Movement speed reduced by 10%. Speed wanes as it increases in health (1 stack / 10% hp).
    SPELL_PURIFIED              = 143523, // Applied when it's healed to full (Dummy visual aura).
    SPELL_IMMERSEUS_KILL_CREDIT = 145889,

    // Achievement
    SPELL_CRY_CRY_CRY           = 148251,
};

enum Events
{
    // First phase (Normal).
    EVENT_SHA_BOLT              = 1,
    EVENT_SWIRL,
    EVENT_CORROSIVE_BLAST,
    EVENT_CORROSIVE_BLAST_OUM, // Melee check.
    EVENT_SWELLING_CORRUPTION,

    // Second phase (Split).
    EVENT_SPLIT,
    EVENT_PUDDLES_MOVE_CENTER,

    // Both phases.
    EVENT_SEEPING_SHA,
    EVENT_BERSERK,

    EVENT_SHA_BOLT_ROOM_CHECK,

    EVENT_SWIRL_ROOM_CHECK,

    EVENT_MOVE_CIRCLE,
    EVENT_SWIRL_TARGET_CHECK,

    EVENT_MOVE_CENTER,
    EVENT_CHECK_WIPE,
};

enum Actions
{
    ACTION_PUDDLES_MOVE_CENTER = 1,
    ACTION_STOP_SPLASH_CHECK,
    ACTION_RESET_SWIRLS,
};

enum Phases
{
    PHASE_IMMERSEUS_NORMAL       = 1,
    PHASE_IMMERSEUS_SPLIT        = 2
};

#define MAX_SPLIT_PUDDLES        25       // Maximum number of puddles summoned each Split phase.

class boss_immerseus : public CreatureScript
{
    public:
        boss_immerseus() : CreatureScript("boss_immerseus") { }

        struct boss_immerseusAI : public BossAI
        {
            boss_immerseusAI(Creature* creature) : BossAI(creature, DATA_IMMERSEUS), vehicle(creature->GetVehicleKit())
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                hasDefendersFallen = false;
                me->setActive(true);
                ASSERT(vehicle);                // Purple energy bar. Vehicle Id 2116.
            }

            Vehicle* vehicle;
            EventMap berserkerEvents;
            uint8 phase;
            uint8 corruptionToRemove;           // Sha Puddles killed / Contaminated Puddles healed in Split phase each increase it by 1. Used to set boss Corruption @ Phase 1 RR.
            int8 shaPuddlesToSummon;           // For Split phase. 1 / each 4 Corruption points boss has.
            int8 contaminatedPuddlesToSummon;  // For Split phase. 1 / each 4 Corruption points boss lacks.
            uint8 puddleDoneCount;
            uint32 tearsSlainCount;
            ObjectGuid targetGUID;
            bool secondPhaseInProgress;         // Used as an UpdateAI diff check.
            bool hasDefendersFallen;

            void Reset() override
            {
                // Encounter has Done
                if (instance && instance->GetBossState(DATA_IMMERSEUS) == DONE)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                events.Reset();
                berserkerEvents.Reset();
                scheduler.CancelAll();
                summons.DespawnAll();

                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 100);

                DoCast(me, SPELL_ZERO_POWER);
                me->SetReactState(REACT_DEFENSIVE);

                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetAnimTier(AnimTier::Fly);
                me->SetDisableGravity(true);
                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();
                me->NearTeleportTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());

                phase = PHASE_IMMERSEUS_NORMAL;

                corruptionToRemove = 0;
                secondPhaseInProgress = false;

                shaPuddlesToSummon = 0;
                contaminatedPuddlesToSummon = 0;
                targetGUID = ObjectGuid::Empty;
                puddleDoneCount = 0;
                tearsSlainCount = 0;

                me->GetMap()->SetWorldState(WORLDSTATE_NO_MORE_TEARS, 0);

                if (instance)
                    instance->SetData(DATA_IMMERSEUS, NOT_STARTED);

                _Reset();

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_AQUEOUS_DEFENDERS_FALLEN) == DONE)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_SEEPING_SHA, true);
                events.ScheduleEvent(EVENT_SEEPING_SHA,       100);
                events.ScheduleEvent(EVENT_SHA_BOLT,         5000);
                events.ScheduleEvent(EVENT_CORROSIVE_BLAST, 10000);
                events.ScheduleEvent(EVENT_SWIRL,           20000);

                if (me->GetMap()->IsHeroic())
                    events.ScheduleEvent(EVENT_SWELLING_CORRUPTION, 9900);

                events.ScheduleEvent(EVENT_CORROSIVE_BLAST_OUM, 4000); // Melee check (tank).
                berserkerEvents.ScheduleEvent(EVENT_BERSERK, 605000); // 6 minutes, 5 seconds according to logs.
                berserkerEvents.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                _JustEngagedWith();

                // Disable Pre-Event
                if (Creature* oozeController = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_OOZE_CONTROLLER) : ObjectGuid::Empty))
                    oozeController->AI()->DoAction(ACTION_START_INTRO);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_NO_MORE_TEARS && ++tearsSlainCount > 9)
                    me->GetMap()->SetWorldState(WORLDSTATE_NO_MORE_TEARS, 1);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetHealth() <= damage)
                {
                    damage = 0;
                    return;
                }

                if (phase == PHASE_IMMERSEUS_SPLIT)
                {
                    damage = 0;
                    return;
                }

                // Handle Split phase entrance and event ending.
                if (phase == PHASE_IMMERSEUS_NORMAL && damage >= me->GetHealth())
                {
                    damage = 0;

                    if (me->GetPower(POWER_ENERGY) > 10) // Split phase.
                        ChangePhase(PHASE_IMMERSEUS_SPLIT);
                    else // Boss is done.
                        HandleFinishEncounter();
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_AQUEOUS_DEFENDERS && !hasDefendersFallen)
                {
                    hasDefendersFallen = true;
                    Talk(TALK_WATER_DEFENDERS_FALLEN);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                }
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_IMMERSEUS) == DONE)
                    return;

                // Handle Emerge / Submerge mechanics.
                if (spell->Id == SPELL_SUBMERGE)
                {
                    me->AddAura(SPELL_EMERGE, me);
                    me->RemoveAurasDueToSpell(SPELL_SUBMERGE);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);

                if (summon->GetEntry() != NPC_SHA_PUDDLE && summon->GetEntry() != NPC_CONTAMINATED_PUDDLE)
                    return;

                uint8 puddlesCount = Is25ManRaid() ? MAX_SPLIT_PUDDLES : MAX_SPLIT_PUDDLES - 5;
                if (++puddleDoneCount >= puddlesCount && secondPhaseInProgress)
                {
                    secondPhaseInProgress = false;
                    Talk(ANN_REFORM);
                    DoCast(me, SPELL_REFORM);
                }
            }

            // Used to despawn all summons having a specific entry.
            void DespawnSummon(uint32 entry)
            {
                std::list<Creature*> summonsList;
                GetCreatureListWithEntryInGrid(summonsList, me, entry, 200.0f);

                for (auto&& itr : summonsList)
                    itr->DespawnOrUnsummon();
            }

            // Used to move and despawn all Sha Bolt Puddles.
            void MoveAndDespawnPuddles()
            {
                std::list<Creature*> shaPuddlesList;
                GetCreatureListWithEntryInGrid(shaPuddlesList, me, NPC_SHA_BOLT, 200.0f);

                for (auto&& itr : shaPuddlesList)
                    itr->AI()->DoAction(ACTION_START_INTRO);
            }

            // Used as an internal function for calling Boss Phase changes.
            void ChangePhase(uint8 bossPhase)
            {
                // Just preventing processing of any unwanted calls.
                if (bossPhase > PHASE_IMMERSEUS_SPLIT)
                    return;

                switch (bossPhase)
                {
                    case PHASE_IMMERSEUS_NORMAL: // When all adds are killed / healed or reached boss.
                    {
                        phase = PHASE_IMMERSEUS_NORMAL;
                        int32 powersRemaining = me->GetPower(POWER_ENERGY) - corruptionToRemove;
                        
                        if (IsHeroic())
                            summons.DespawnEntry(NPC_SHA_POOL);

                        // Finish Encounter
                        if (powersRemaining < 10)
                            HandleFinishEncounter();
                        else
                        {
                            if (uint32 healthRemaining = me->GetMaxHealth() * (0.01 * powersRemaining))
                                me->SetHealth(healthRemaining < 1 ? 1 : healthRemaining); // Works like percentage calculation (MaxHealth * (100% - (1% * KilledHealedAdds))).

                            me->SetPower(POWER_ENERGY, powersRemaining);

                            events.ScheduleEvent(EVENT_SHA_BOLT, 5000);
                            events.ScheduleEvent(EVENT_CORROSIVE_BLAST, 10000);
                            events.ScheduleEvent(EVENT_SWIRL, 20000);
                            events.ScheduleEvent(EVENT_CORROSIVE_BLAST_OUM, 4000);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SWELLING_CORRUPTION, 9900);

                            if (instance)
                                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SEEPING_SHA_EFF);
                        }

                        me->RemoveAurasDueToSpell(SPELL_SPLIT);
                        me->RemoveAurasDueToSpell(SPELL_EMERGE); // Emerge.
                        me->RemoveAurasDueToSpell(SPELL_SHA_POOL_AURA);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        corruptionToRemove = 0;
                        shaPuddlesToSummon = 0;
                        contaminatedPuddlesToSummon = 0;
                        break;
                    }
                    case PHASE_IMMERSEUS_SPLIT: // When boss "dies" (reaches < 1 health).
                        phase = PHASE_IMMERSEUS_SPLIT;
                        puddleDoneCount = 0;
                        MoveAndDespawnPuddles();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        events.CancelEvent(EVENT_SHA_BOLT);
                        events.CancelEvent(EVENT_CORROSIVE_BLAST);
                        events.CancelEvent(EVENT_SWIRL);
                        events.CancelEvent(EVENT_CORROSIVE_BLAST_OUM);

                        if (IsHeroic())
                        {
                            me->RemoveAurasDueToSpell(SPELL_SWELLING_CORRUPTION);
                            events.CancelEvent(EVENT_SWELLING_CORRUPTION);

                            // controller of pool
                            if (Creature* shaPool = me->SummonCreature(NPC_SHA_POOL, me->GetPositionX(), me->GetPositionY(), 246.835f, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                shaPool->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                shaPool->SetDisplayId(49056); // not sure, but his hitbox should`be same like an encounter
                                shaPool->CastSpell(shaPool, SPELL_SPLIT, true);
                                shaPool->CastSpell(shaPool, SPELL_SHA_POOL_AURA, true);
                                shaPool->SetObjectScale(1.0f);
                            }
                        }

                        if (instance)
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SEEPING_SHA_EFF);

                        events.ScheduleEvent(EVENT_SPLIT, 200);
                        break;

                    default: break;
                }
            }

            // Used as an Add counter function for SPELL_SPLIT calculations.
            void CalculateShaContaminatedPuddles()
            {
                // Generate some local variables, calculate the correct boss ones and make any necessary extra checks.
                uint8 myEnergy = me->GetPower(POWER_ENERGY);
                uint8 myEnergyMissing = 100 - myEnergy;

                shaPuddlesToSummon = myEnergy / 4; // Select the lowest number.

                contaminatedPuddlesToSummon = myEnergyMissing > 20 ? myEnergyMissing / 4 : 5; // 5 by default, doesn`t matter losed state

                uint8 totalAddsToSummon = shaPuddlesToSummon + contaminatedPuddlesToSummon;

                uint8 maxSplitCount = Is25ManRaid() ? MAX_SPLIT_PUDDLES : MAX_SPLIT_PUDDLES - 5;
                // Far fetched to believe we get here, but worth to check.
                if (totalAddsToSummon > maxSplitCount)
                {
                    if (shaPuddlesToSummon > contaminatedPuddlesToSummon)
                        shaPuddlesToSummon -= totalAddsToSummon - maxSplitCount;
                    else
                        contaminatedPuddlesToSummon -= totalAddsToSummon - maxSplitCount;
                }
                else if (totalAddsToSummon < maxSplitCount)
                {
                    if (shaPuddlesToSummon > contaminatedPuddlesToSummon)
                        contaminatedPuddlesToSummon += maxSplitCount - totalAddsToSummon;
                    else
                        shaPuddlesToSummon += maxSplitCount - totalAddsToSummon;
                }
            }

            void HandleFinishEncounter()
            {
                if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                {
                    player->RewardPersonalLootAndCurrency(me);
                    JustDied(player);
                    me->GetMap()->ToInstanceMap()->PermBindAllPlayers(player);

                    if (Group* group = player->GetGroup())
                        group->UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_IMMERSEUS_KILL_CREDIT, 0, 0, me, me);
                }

                me->SetFaction(35);
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->SetFullHealth();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_IMMERSEUS_KILL_CREDIT, me);
                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_IMMERSEUS_KILL_CREDIT);

                // Cho Event
                me->SummonCreature(NPC_LOREWALKER_CHO, choPastImmerseusEvent[0], TEMPSUMMON_MANUAL_DESPAWN);

                if (!instance->GetData(DATA_LFR))
                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(IsHeroic() ? GO_TEARS_OF_THE_VALE_HC : GO_TEARS_OF_THE_VALE) : ObjectGuid::Empty))
                        instance->DoRespawnGameObject(go->GetGUID(), 7 * DAY);
            }

            void KilledUnit(Unit* victim) override { }

            void EnterEvadeMode() override
            {
                DespawnSummon(NPC_SHA_BOLT);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->AddUnitState(UNIT_STATE_EVADE);

                berserkerEvents.Reset();
                scheduler.CancelAll();
                me->RemoveAllAuras();
                Reset();
                me->DeleteThreatList();
                me->CombatStop(true);
                me->GetMotionMaster()->MovementExpired();
                me->GetMotionMaster()->MoveTargetedHome();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SEEPING_SHA_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHA_SPLASH_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORROSIVE_BLAST);
                    instance->SetData(DATA_IMMERSEUS, FAIL);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove.
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                _EnterEvadeMode();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void JustReachedHome() override
            {
                // Prevent calling this if boss is not in Evade mode.
                if (!me->HasUnitState(UNIT_STATE_EVADE))
                    return;

                me->ClearUnitState(UNIT_STATE_EVADE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                DoCast(me, SPELL_ZERO_POWER);
                me->SetPower(POWER_ENERGY, 100);

                _JustReachedHome();
            }

            void JustDied(Unit* killer) override
            {
                summons.DespawnAll();
                DespawnSummon(NPC_SHA_BOLT);
                berserkerEvents.Reset();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SEEPING_SHA_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHA_SPLASH_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORROSIVE_BLAST);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove.
                    instance->SetData(DATA_IMMERSEUS, DONE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                _JustDied();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                berserkerEvents.Update(diff);
                events.Update(diff);

                while (uint32 eventId = berserkerEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_CHECK_WIPE:
                            if (instance && instance->IsWipe(100.0f, me))
                            {
                                EnterEvadeMode();
                                break;
                            }

                            berserkerEvents.ScheduleEvent(EVENT_CHECK_WIPE, 1 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        // Phase 1 - Tears of the Vale.

                        case EVENT_SHA_BOLT:
                        {
                            if (phase == PHASE_IMMERSEUS_NORMAL)
                                DoCast(me, SPELL_SHA_BOLT_DUMMY);

                            events.ScheduleEvent(EVENT_SHA_BOLT, urand(11000, 15000)); // 6 - 20 seconds in logs.
                            break;
                        }

                        case EVENT_SWIRL:
                        {
                            if (phase == PHASE_IMMERSEUS_NORMAL)
                            {
                                Talk(ANN_SWIRL);
                                

                                if (Unit* target = me->GetVictim())
                                    targetGUID = target->GetGUID();
                                
                                scheduler  // Should wait a few seconds on the postision before casting spell 
                                    .Schedule(Milliseconds(2000), [this](TaskContext context)
                                {
                                    DoCast(me, SPELL_SWIRL_SPRAY, true);
                                });

                                float x, y;
                                float sOri = Position::NormalizeOrientation(frand(0.0f, 2 * M_PI));
                                GetPositionWithDistInOrientation(me, 65.0f, sOri, x, y);

                                if (Creature* swirlTarget = me->SummonCreature(NPC_SWIRL_TARGET, x, y, 246.83f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 13100))
                                {
                                    me->PrepareChanneledCast(sOri, SPELL_SWIRL);
                                    me->SetGuidValue(UNIT_FIELD_CHANNEL_OBJECT, swirlTarget->GetGUID());
                                    me->SetTarget(swirlTarget->GetGUID());
                                }

                                scheduler
                                    .Schedule(Milliseconds(13500), [this](TaskContext context)
                                {
                                    me->RemoveChanneledCast(targetGUID);
                                });
                            }
                            events.ScheduleEvent(EVENT_SWIRL, urand(46500, 50500)); // 48.5 seconds in logs.
                            break;
                        }

                        case EVENT_CORROSIVE_BLAST:
                        {
                            if (phase == PHASE_IMMERSEUS_NORMAL)
                                if (Unit* mainTarget = me->GetVictim())
                                    DoCast(mainTarget, SPELL_CORROSIVE_BLAST);

                            events.ScheduleEvent(EVENT_CORROSIVE_BLAST, urand(33000, 37000)); // 35 seconds in logs.
                            break;
                        }

                        case EVENT_SWELLING_CORRUPTION:
                        {
                            DoCast(me, SPELL_SWELLING_CORRUPTION_A);
                            events.ScheduleEvent(EVENT_SWELLING_CORRUPTION, 75000); // 75 seconds in logs.
                            break;
                        }

                        // Phase 2 - Split.

                        case EVENT_SPLIT:
                        {
                            Talk(ANN_SPLIT);
                            CalculateShaContaminatedPuddles();
                            DoCast(me, SPELL_SPLIT);
                            secondPhaseInProgress = true;
                            break;
                        }
                        // Phase 1 + Phase 2 (General).
                        case EVENT_CORROSIVE_BLAST_OUM:
                            if (phase == PHASE_IMMERSEUS_NORMAL)
                                if ((me->GetVictim() && me->GetVictim()->GetExactDist2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50.0f))
                                    DoCast(me->GetVictim(), SPELL_CORROSIVE_BLAST, true);

                            events.ScheduleEvent(EVENT_CORROSIVE_BLAST_OUM, 2000);
                            break;

                        default: break;
                    }
                }

                if (phase == PHASE_IMMERSEUS_NORMAL)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_immerseusAI(creature);
        }
};

// Sha Bolt 71544
struct npc_sha_splash_bolt_immerseus : public ScriptedAI
{
    npc_sha_splash_bolt_immerseus(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        Reset();
        DoCast(me, SPELL_SHA_SPLASH, true);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetWalk(false);
            me->SetSpeed(MOVE_RUN, 0.65f);

            if (Creature* immersius = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IMMERSEUS) : ObjectGuid::Empty))
                me->GetMotionMaster()->MovePoint(0, *immersius);

            me->DespawnOrUnsummon(me->GetSplineDuration());
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Swirl 71548
struct npc_swirl_immerseus : public ScriptedAI
{
    npc_swirl_immerseus(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;

    void Reset() override
    {
        scheduler.CancelAll();
        me->RemoveAurasDueToSpell(SPELL_SWIRL_SPRAY_AT);
        x = me->GetPositionX(); y = me->GetPositionY();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                DoCast(me, SPELL_SWIRL_SPRAY_AT);
            
                scheduler
                    .Schedule(Milliseconds(100), [this](TaskContext context)
                {
                    me->GetMotionMaster()->MovePoint(0, x + frand(-5.0f, 5.0f), y + frand(-10.0f, 10.0f), me->GetPositionZ());
                    context.Repeat(Milliseconds(me->GetSplineDuration()));
                });
                break;
            case ACTION_RESET_SWIRLS:
                me->GetMotionMaster()->Clear();
                Reset();
                me->GetMotionMaster()->MoveTargetedHome();
                break;
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Swirl Target 71550
struct npc_swirl_target_immerseus : public ScriptedAI
{
    npc_swirl_target_immerseus(Creature* creature) : ScriptedAI(creature) { }

    float x, y, dist;
    bool clockWise;

    void Reset() override
    {
        me->SetSpeed(MOVE_RUN, 2.5f);
        me->SetSpeed(MOVE_WALK, 2.5f);
        clockWise = IsHeroic() ? 1 : urand(0, 1);

        dist = me->GetExactDist2d(&ImmerseusCenterPos);

        if (Creature* immerseus = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IMMERSEUS) : ObjectGuid::Empty))
        {
            Movement::MoveSplineInit init(me);
            for (uint8 i = 1; i < 41; ++i)
            {
                float step = clockWise ? - (i * M_PI / 20) : (i * M_PI / 20);
                x = ImmerseusCenterPos.GetPositionX() + (dist * cos(Position::NormalizeOrientation(immerseus->GetAngle(me) + step)));
                y = ImmerseusCenterPos.GetPositionY() + (dist * sin(Position::NormalizeOrientation(immerseus->GetAngle(me) + step)));

                init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
            }

            init.SetCyclic();
            init.Launch();
        }
    }
};

// Sha Puddle 71603
struct npc_sha_puddle_immerseus : public ScriptedAI
{
    npc_sha_puddle_immerseus(Creature* creature) : ScriptedAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
    }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    bool reachedBoss;

    void IsSummonedBy(Unit* summoner) override
    {
        reachedBoss = false;
        me->SetInCombatWithZone();
        me->SetReactState(REACT_PASSIVE);
        DoCast(me, SPELL_SHA_SLIME, true);
        summonerGUID = summoner->GetGUID();

        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (Creature* Immerseus = ObjectAccessor::GetCreature(*me, summonerGUID))
            {
                me->GetMotionMaster()->MoveFollow(Immerseus, 2.0f, me->GetAngle(Immerseus));

                if (me->GetExactDist2d(Immerseus) <= 20.0f && !reachedBoss)
                {
                    reachedBoss = true;
                    me->GetMotionMaster()->MovementExpired();
                    me->DespawnOrUnsummon(200);
                    Immerseus->CastSpell(Immerseus, SPELL_ERRUPTING_SHA, true);
                    return;
                }

                context.Repeat(Seconds(1));
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    void JustDied(Unit* killer) override
    {
        // Set boss corruption to remove to +1.
        if (Creature* Immerseus = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IMMERSEUS) : ObjectGuid::Empty))
            CAST_AI(boss_immerseus::boss_immerseusAI, Immerseus->AI())->corruptionToRemove += 1;

        // Cast the buffs.
        DoCast(me, SPELL_SHA_RESIDUE, true);
        me->DespawnOrUnsummon(0.2 * IN_MILLISECONDS);
    }
};

// Contaminated Puddle 71604
struct npc_contaminated_puddle_immerseus : public ScriptedAI
{
    npc_contaminated_puddle_immerseus(Creature* creature) : ScriptedAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_INCREASE_HEALTH, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
    }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    bool reachedBoss;
    bool hasCleared;

    void IsSummonedBy(Unit* summoner) override
    {
        reachedBoss = false;
        hasCleared  = false;
        me->SetInCombatWithZone();
        me->SetHealth(me->CountPctFromMaxHealth(1));
        DoCast(me, SPELL_CONGEALING_AURA, true);
        me->SetReactState(REACT_PASSIVE);

        summonerGUID = summoner->GetGUID();

        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (Creature* Immerseus = ObjectAccessor::GetCreature(*me, summonerGUID))
            {
                me->GetMotionMaster()->MoveFollow(Immerseus, 2.0f, me->GetAngle(Immerseus));

                if (me->GetExactDist2d(Immerseus) <= 20.0f && !reachedBoss)
                {
                    reachedBoss = true;
                    me->GetMotionMaster()->MovementExpired();
                    me->DespawnOrUnsummon(0.2 * IN_MILLISECONDS);

                    Immerseus->CastSpell(Immerseus, me->HasAura(SPELL_PURIFIED) ? SPELL_ERRUPTING_WATER : SPELL_ERRUPTING_SHA, true);

                    return;
                }

                context.Repeat(Seconds(1));
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Tears of the Vale 73638
struct npc_tears_of_the_vale : public ScriptedAI
{
    npc_tears_of_the_vale(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        if (Creature* immerseus = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IMMERSEUS) : ObjectGuid::Empty))
            immerseus->AI()->JustSummoned(me);

        scheduler
            .Schedule(Seconds(3), Seconds(8), [this](TaskContext context)
        {
            DoCast(me, SPELL_CRY_CRY_CRY);
            context.Repeat(Seconds(8), Seconds(14));
        });
    }

    void JustDied(Unit* killer) override
    {
        if (Creature* immerseus = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IMMERSEUS) : ObjectGuid::Empty))
            immerseus->AI()->SetData(TYPE_NO_MORE_TEARS, 1);
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Sha Bolt 143290
class spell_immerseus_sha_bolt : public SpellScript
{
    PrepareSpellScript(spell_immerseus_sha_bolt);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_sha_bolt::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Swirl Spray 143415
class spell_swirl_spray : public SpellScript
{
    PrepareSpellScript(spell_swirl_spray);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_START_INTRO);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SWIRL; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_swirl_spray::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_swirl_spray::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Swirl Areatrigger 143410
class spell_immersius_swirl_at_aura : public AuraScript
{
    PrepareAuraScript(spell_immersius_swirl_at_aura);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_RESET_SWIRLS);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_immersius_swirl_at_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_AREA_TIRGGER, AURA_EFFECT_HANDLE_REAL);
    }
};

// Split 143020
class spell_immerseus_split : public SpellScript
{
    PrepareSpellScript(spell_immerseus_split);

    void PreventStun()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        // No need to stun the boss.
        PreventHitDefaultEffect(EFFECT_0);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        if (!caster->ToCreature())
            return;

        if (caster->ToCreature()->GetEntry() != NPC_IMMERSEUS)
            return;

        caster->SendClearTarget(); // couldn`t call UNIT_FLAG_NOT_SELECTABLE cuz it`ll break DBM

        // Trigger the mob summon missiles (85 yard radius).
        uint8 shaPuddles = CAST_AI(boss_immerseus::boss_immerseusAI, caster->ToCreature()->AI())->shaPuddlesToSummon;
        uint8 conPuddles = CAST_AI(boss_immerseus::boss_immerseusAI, caster->ToCreature()->AI())->contaminatedPuddlesToSummon;

        for (uint8 i = 0; i < shaPuddles; i++)
            caster->CastSpell(caster, SPELL_SPLIT_SHA_MISSILE, true);

        for (uint8 i = 0; i < conPuddles; i++)
            caster->CastSpell(caster, SPELL_SPLIT_CONTAM_MISSILE, true);

        // Handle Submerging.
        caster->CastSpell(caster, SPELL_SUBMERGE, false);
    }

    void Register() override
    {
        BeforeHit += SpellHitFn(spell_immerseus_split::PreventStun);
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_split::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Reform 143469
class spell_immerseus_reform : public SpellScript
{
    PrepareSpellScript(spell_immerseus_reform);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            CAST_AI(boss_immerseus::boss_immerseusAI, caster->ToCreature()->AI())->ChangePhase(PHASE_IMMERSEUS_NORMAL);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_reform::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Congealing 143538
class spell_immerseus_congealing : public AuraScript
{
    PrepareAuraScript(spell_immerseus_congealing);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (!caster || !target)
            return;

        // Calculate correct stacks count (+1 for every 10% health healed).
        uint8 stackCount = floor(caster->GetHealthPct() / 10);

        // Should not add anything if stacks should be 0. 
        if (stackCount < 1)
            return;

        // Add the aura and set the stack count.
        if (stackCount >= 1 && !caster->GetAura(SPELL_CONGEALING))
        {
            if (Aura* congealing = caster->AddAura(SPELL_CONGEALING, caster))
                congealing->SetStackAmount(stackCount);
        }
        else
        {
            if (Aura* congealing = caster->GetAura(SPELL_CONGEALING))
                congealing->SetStackAmount(stackCount);
        }

        // Handle Purified aura addition and cast the buffs.
        if (stackCount == 10 && caster->HealthAbovePct(99) && !caster->HasAura(SPELL_PURIFIED))
        {
            caster->CastSpell(caster, SPELL_PURIFIED, true);

            // Set boss corruption to remove to +1.
            if (Creature* Immerseus = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_IMMERSEUS) : ObjectGuid::Empty))
                CAST_AI(boss_immerseus::boss_immerseusAI, Immerseus->AI())->corruptionToRemove += 1;

            caster->CastSpell(caster, SPELL_PURIFIED_RESIDUE, true);
            caster->RemoveAurasDueToSpell(GetSpellInfo()->Id);
            caster->RemoveAurasDueToSpell(SPELL_CONGEALING);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_immerseus_congealing::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Swirl 143309
class spell_immerseus_swirl_aura : public AuraScript
{
    PrepareAuraScript(spell_immerseus_swirl_aura);

    void OnUpdate(uint32 diff)
    {
        if (GetCaster() && GetCaster()->GetTarget())
            if (Unit* owner = ObjectAccessor::GetUnit(*GetCaster(), GetCaster()->GetTarget()))
                GetCaster()->UpdateOrientation(GetCaster()->GetAngle(owner)); // Handle update server-side orientation
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_immerseus_swirl_aura::OnUpdate);
    }
};

// Split Sha Missle 143453, 143454
class spell_immerseus_split_sha_missle_eff : public SpellScript
{
    PrepareSpellScript(spell_immerseus_split_sha_missle_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_IMMERSEUS) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_immerseus_split_sha_missle_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Sha Splash Eff 143297
class spell_immerseus_sha_splash_eff : public AuraScript
{
    PrepareAuraScript(spell_immerseus_sha_splash_eff);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        // why 5s duration?
        if (Unit* owner = GetOwner()->ToUnit())
            RefreshDuration();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_immerseus_sha_splash_eff::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Swelling Corruption Announce 143578
class spell_immerseus_swelling_corruption_ann : public SpellScript
{
    PrepareSpellScript(spell_immerseus_swelling_corruption_ann);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_SWELLING_CORRUPTION, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_immerseus_swelling_corruption_ann::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Swelling Corruption 143574
class spell_immersius_swelling_corruption : public AuraScript
{
    PrepareAuraScript(spell_immersius_swelling_corruption);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            uint8 stacks = owner->GetPower(POWER_ENERGY) % 2 == 0 ? owner->GetPower(POWER_ENERGY) / 2 : (owner->GetPower(POWER_ENERGY) - 1) / 2;
            SetStackAmount(stacks);
        }
    }

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Unit* target = eventInfo.GetActor())
            {
                owner->CastSpell(target, SPELL_SHA_CORRUPTION_DMG, true);
                owner->CastSpell(owner, SPELL_SHA_CORRUPTION_SUMM, true);

                // remove stack per hit
                if (GetStackAmount() > 1)
                    SetStackAmount(GetStackAmount() - 1);
                else
                    owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
            }
        }
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.TriggeredBySpell()->m_targets.GetUnitTarget() == eventInfo.GetActionTarget();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_immersius_swelling_corruption::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_immersius_swelling_corruption::HandleOnProc);
        DoCheckProc += AuraCheckProcFn(spell_immersius_swelling_corruption::CheckProc);
    }
};

// Swelling Corruption Summ 143581
class spell_immerseus_swelling_corruption_summ : public SpellScript
{
    PrepareSpellScript(spell_immerseus_swelling_corruption_summ);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_IMMERSEUS) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_immerseus_swelling_corruption_summ::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Sha Pool Puddles Ping 143461
class spell_immerseus_sha_pool_puddles_ping : public SpellScript
{
    PrepareSpellScript(spell_immerseus_sha_pool_puddles_ping);

    std::list<uint64> affectedPuddles;

    bool Load() override
    {
        affectedPuddles.clear();
        return true;
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* target = GetHitCreature())
            {
                affectedPuddles.push_back(target->GetGUID());
                caster->SetObjectScale(caster->GetObjectScale() + 0.12f);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SHA_PUDDLE && target->GetEntry() != NPC_CONTAMINATED_PUDDLE; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && std::find(affectedPuddles.begin(), affectedPuddles.end(), target->ToCreature()->GetGUID()) != affectedPuddles.end(); });

        // Range Sort
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && GetCaster() && GetCaster()->GetExactDist2d(target->ToUnit()) > (GetCaster()->GetObjectScale() * 16.0f) / 2.0f; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_sha_pool_puddles_ping::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_sha_pool_puddles_ping::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Sha Pool 143462
class spell_immerseus_sha_pool : public AuraScript
{
    PrepareAuraScript(spell_immerseus_sha_pool);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->SetObjectScale(owner->GetObjectScale() + 0.12f);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_immerseus_sha_pool::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Sha Pool Players Ping 143460
class spell_immerseus_sha_pool_players_ping : public SpellScript
{
    PrepareSpellScript(spell_immerseus_sha_pool_players_ping);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->SetObjectScale(1.0f); // set to default
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Range Sort
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && GetCaster()->GetExactDist2d(target->ToPlayer()) > (GetCaster()->GetObjectScale() * 31.0f) / 2.0f; });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_sha_pool_players_ping::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_immerseus_sha_pool_players_ping::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Sha Pool Players Ping 143460
class spell_immerseus_sha_pool_players_ping_aura : public AuraScript
{
    PrepareAuraScript(spell_immerseus_sha_pool_players_ping_aura);

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (GetStackAmount() < 2)
                return;

            int32 calculatedAmount = 15000 * std::pow(6, GetStackAmount() - 1);
            damage = calculatedAmount;
        }
    }

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (GetStackAmount() < 2)
                return;

            uint32 calculatedAmount = 15000 * std::pow(6, GetStackAmount() - 1);
            amount = calculatedAmount;
        }

        canBeRecalculated = true;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_immerseus_sha_pool_players_ping_aura::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_immerseus_sha_pool_players_ping_aura::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Seeping Sha Eff 143286
class spell_immerseus_seeping_sha_eff : public SpellScript
{
    PrepareSpellScript(spell_immerseus_seeping_sha_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetMap()->IsHeroic() && GetCaster()->HasAura(SPELL_SPLIT))
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_seeping_sha_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_immerseus_seeping_sha_eff::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
    }
};

// Split 143020
class spell_immersius_split_aura : public AuraScript
{
    PrepareAuraScript(spell_immersius_split_aura);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetOwner() && GetOwner()->GetEntry() != NPC_IMMERSEUS)
            SetDuration(3 * MINUTE * IN_MILLISECONDS);
    }

    void HandleOnRemove(AuraEffect const* aureff, AuraEffectHandleModes /*mode*/)
    {
        if (aureff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (owner->GetEntry() != NPC_IMMERSEUS)
                return;

            // Remove not absorbed puddles & transform some of there
            std::list<Creature*> puddles;
            GetCreatureListWithEntryInGrid(puddles, owner, NPC_SHA_PUDDLE, 300.0f);
            GetCreatureListWithEntryInGrid(puddles, owner, NPC_CONTAMINATED_PUDDLE, 300.0f);

            puddles.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

            for (auto&& itr : puddles)
            {
                if (itr->GetEntry() == NPC_SHA_PUDDLE)
                    itr->SummonCreature(NPC_TEARS_OF_THE_VALE, *itr, TEMPSUMMON_MANUAL_DESPAWN);

                itr->DespawnOrUnsummon();
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_immersius_split_aura::HandleOnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_immersius_split_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Sha Corruption Eff 143579
class spell_immerseus_sha_corruption_eff : public AuraScript
{
    PrepareAuraScript(spell_immerseus_sha_corruption_eff);

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (GetStackAmount() < 2)
                return;

            int32 calculatedAmount = 2500 * std::pow(3, GetStackAmount() - 1);
            damage = calculatedAmount;
        }
    }

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (GetStackAmount() < 2)
                return;

            uint32 calculatedAmount = 2500 * std::pow(3, GetStackAmount() - 1);
            amount = calculatedAmount;
        }

        canBeRecalculated = true;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_immerseus_sha_corruption_eff::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_immerseus_sha_corruption_eff::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 1017. Summoned by 143298 - Sha Splash
class sat_sha_splash : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnInit() override
    {
        GetAreaTrigger()->AttachToTarget();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_SHA_SPLASH_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SHA_SPLASH_EFF);
    }
};

// 1024. Summoned by 143410 - Swirl
class sat_immersius_swirl : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnInit() override
    {
        GetAreaTrigger()->AttachToTarget();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_SWIRL_SPRAY_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SWIRL_SPRAY_EFF);
    }
};

// 1016. Summoned by 143281 - Seeping Sha
class sat_immersius_seeping_sha : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_SEEPING_SHA_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SEEPING_SHA_EFF);
    }
};

// 1018. Summoned by 143309 - Swirl
class sat_immersius_main_swirl : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->HasInArc(M_PI / 6, triggering->ToPlayer(), 3.0f) && triggering->ToPlayer()->GetPositionZ() < 248.0f;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_SWIRL_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SWIRL_EFF);
    }
};

void AddSC_immerseus()
{
    new boss_immerseus();
    new creature_script<npc_sha_splash_bolt_immerseus>("npc_sha_splash_bolt_immerseus");
    new creature_script<npc_swirl_immerseus>("npc_swirl_immerseus");
    new creature_script<npc_swirl_target_immerseus>("npc_swirl_target_immerseus");
    new creature_script<npc_sha_puddle_immerseus>("npc_sha_puddle_immerseus");
    new creature_script<npc_contaminated_puddle_immerseus>("npc_contaminated_puddle_immerseus");
    new creature_script<npc_tears_of_the_vale>("npc_tears_of_the_vale");

    new spell_script<spell_immerseus_sha_bolt>("spell_immerseus_sha_bolt");
    new spell_script<spell_swirl_spray>("spell_swirl_spray");
    new aura_script<spell_immersius_swirl_at_aura>("spell_immersius_swirl_at_aura");
    new spell_script<spell_immerseus_split>("spell_immerseus_split");
    new spell_script<spell_immerseus_reform>("spell_immerseus_reform");
    new aura_script<spell_immerseus_congealing>("spell_immerseus_congealing");
    new aura_script<spell_immerseus_swirl_aura>("spell_immerseus_swirl_aura");
    new spell_script<spell_immerseus_split_sha_missle_eff>("spell_immerseus_split_sha_missle_eff");
    new aura_script<spell_immerseus_sha_splash_eff>("spell_immerseus_sha_splash_eff");
    new spell_script<spell_immerseus_swelling_corruption_ann>("spell_immerseus_swelling_corruption_ann");
    new aura_script<spell_immersius_swelling_corruption>("spell_immersius_swelling_corruption");
    new spell_script<spell_immerseus_swelling_corruption_summ>("spell_immerseus_swelling_corruption_summ");
    new spell_script<spell_immerseus_sha_pool_puddles_ping>("spell_immerseus_sha_pool_puddles_ping");
    new aura_script<spell_immerseus_sha_pool>("spell_immerseus_sha_pool");
    new spell_script<spell_immerseus_sha_pool_players_ping>("spell_immerseus_sha_pool_players_ping");
    new aura_script<spell_immerseus_sha_pool_players_ping_aura>("spell_immerseus_sha_pool_players_ping_aura");
    new spell_script<spell_immerseus_seeping_sha_eff>("spell_immerseus_seeping_sha_eff");
    new aura_script<spell_immersius_split_aura>("spell_immersius_split_aura");
    new aura_script<spell_immerseus_sha_corruption_eff>("spell_immerseus_sha_corruption_eff");
    new atrigger_script<sat_sha_splash>("sat_sha_splash");
    new atrigger_script<sat_immersius_swirl>("sat_immersius_swirl");
    new atrigger_script<sat_immersius_seeping_sha>("sat_immersius_seeping_sha");
    new atrigger_script<sat_immersius_main_swirl>("sat_immersius_main_swirl");
}
