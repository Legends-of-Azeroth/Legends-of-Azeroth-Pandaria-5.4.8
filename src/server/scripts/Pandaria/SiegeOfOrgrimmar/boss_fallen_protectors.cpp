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

#include "siege_of_orgrimmar.h"

enum Yells
{
    // Rook
    TALK_ROOK_AGGRO = 3,
    TALK_ROOK_DESPERATE_ANN,
    TALK_ROOK_DESPERATE,
    TALK_ROOK_BOND_OF_GOLDEN_LOTUS_ANN,
    TALK_ROOK_BOND_OF_GOLDEN_LOTUS,
    ROOK_SAY_CORRUPTION_KICK,

    // Sun
    TALK_SUN_DESPERATE_ANN = 1,
    TALK_SUN_DESPERATE,
    TALK_SUN_BOND_OF_GOLDEN_LOTUS,
    TALK_SUN_BOND_OF_GOLDEN_LOTUS_ANN,
    TALK_SUN_CALAMITY, 
    TALK_SUN_ANNOUNCE_CALAMITY,

    // He
    TALK_HE_DESPERATE_ANN = 0,
    TALK_HE_DESPERATE,
    TALK_HE_BOND_OF_GOLDEN_LOTUS_ANN,
    TALK_HE_BOND_OF_GOLDEN_LOTUS,
    TALK_HE_GOUGE_ANN,
    TALK_HE_ANNOUNCE_INSTANT_POISON,
    TALK_HE_ANNOUNCE_NOXIOUS_POISON,
};

enum Spells
{
    // General

    SPELL_BOND_OF_THE_GOLDEN_LOTUS = 143497,

    SPELL_ROOT_GENERIC             = 42716,
    SPELL_PROTECTORS_VISUAL        = 144942, // Not used - Black & White Dummy.
    SPELL_BERSERK                  = 144369,

    //============================================================================================================================================================================//

    // Rook Stonetoe
    SPELL_VENGEFUL_STRIKES         = 144396, // Stun, Damage, trigger 144397 each 0.5s.
    SPELL_VENGEFUL_STRIKES_VISUAL  = 145584,

    SPELL_CORRUPTED_BREW           = 143019, // Dummy on efect 0 to cast SPELL_CORRUPTED_BREW_MISSILE (1, 3, 2, 6 targets).
    SPELL_CORRUPTED_BREW_MISSILE   = 143021, // Triggers SPELL_CORRUPTED_BREW_DAMAGE.
    SPELL_CORRUPTED_BREW_DAMAGE    = 143023, // Damage and movement speed reduce in 5 yards.
    SPELL_CORRUPTED_BREW_OVERPOWER = 145605,
    SPELL_CORRUPTED_BREW_X1        = 145608, // depend of stack amount
    SPELL_CORRUPTED_BREW_X2        = 145609,
    SPELL_CORRUPTED_BREW_X3        = 145610,
    SPELL_CORRUPTED_BREW_x4        = 145611,
    SPELL_CORRUPTED_BREW_X5        = 145612,
    SPELL_CORRUPTED_BREW_X6        = 145615,
    SPELL_CORRUPTED_BREW_x7        = 145617,

    SPELL_CLASH                    = 143027, // Dummy on Effect 0 to cast SPELL_CLASH_ROOK + SPELL_CLASH_TARGET.
    SPELL_CLASH_BY_PLAYER          = 143030, // Player charge to Rook.
    SPELL_CLASH_BY_ROOK            = 143028, // Rook Jump to Player.

    SPELL_CORRUPTION_KICK          = 143007, // Immediately after Clash. Triggers damage and aura apply.
    SPELL_CORRUPTION_KICK_EFF      = 143010,

    SPELL_MISSERY_SORROW_GLOOM     = 143955, // Dummy aura, "Meditating" tooltip.

    //============================================================================================================================================================================//

    // He Softfoot
    SPELL_SHADOWSTEP               = 143048, // Triggers SPELL_HE_GARROTE and Teleport 2y behind target.
    SPELL_SHADOWSTEP_SELECTOR      = 143050,
    SPELL_HE_GARROTE               = 143198, // Damage every 2 seconds.

    SPELL_HE_GOUGE                 = 143330, // Dummy of Effect 0. Checks for SPELL_HE_GOUGE_DMG_STUN / SPELL_HE_GOUGE_KB.
    SPELL_HE_GOUGE_DMG_STUN        = 143301, // Damage and 6 seconds stun.
    SPELL_HE_GOUGE_KB              = 143331, // Knockback.

    SPELL_INSTANT_POISON           = 143210, // Applies Proc aura on melee atack for Instant Poison damage (Effect 1) and Script Effect for removal for SPELL_NOXIOUS_POISON on Effect 2.
    SPELL_INSTANCE_POISON_EFF      = 143224,

    SPELL_NOXIOUS_POISON           = 143225, // Applies Proc aura on melee atack for SPELL_NOXIOUS_POISON_MISSILE (Effect 1) and Script Effect for removal for SPELL_INSTANT_POISON on Effect 2.
    SPELL_NOXIOUS_POISON_MISSILE   = 143245, // Triggers SPELL_NOXIOUS_POISON_AT_MISS.
    SPELL_NOXIOUS_POISON_AT_MISS   = 143276, // Triggers SPELL_NOXIOUS_POISON_AT.
    SPELL_NOXIOUS_POISON_AT        = 143235, // Creates Areatrigger 1013 (Pool of Noxious Poison).
    SPELL_NOXIOUS_POISON_AURA      = 143239, // Player periodic damage trigger aura (From Areatrigger).

    SPELL_MARK_OF_ANGUISH          = 143822, // Dummy on Effect 0 for SPELL_MARK_OF_ANGUISH_MAIN aura apply on random target.
    SPELL_MARK_OF_ANGUISH_MAIN     = 143840, // Mechanic abilities, Periodic Dummy on Effect 1 for SPELL_MARK_OF_ANGUISH_DAMAGE, Root apply.
    SPELL_MARK_OF_ANGUISH_DAMAGE   = 144365, // Damage.
    SPELL_MARK_OF_ANGUISH_TRANSFER = 143842, // Mechanic Abilities button, Dummy on Effect 0 for SPELL_MARK_OF_ANGUISH_MAIN aura apply on selected friendly target.
    SPELL_MARK_OF_ANGUISH_VISUAL   = 143812, // Kneel boss visual, "Meditating" tooltip.
    SPELL_DEBILITATION             = 147383, // Possessing the Mark of Anguish debilitates targets, decreasing their armor by 50% for 2 min.
    SPELL_HE_FIXATE                = 143292, // in case if our target got gouge - fixate on anyone - until taunt or 30s duration.

    //============================================================================================================================================================================//

    // Sun Tenderheart
    SPELL_SHA_SHEAR                = 143423, // Triggers SPELL_SHA_SHEAR_DAMAGE each sec.
    SPELL_SHA_SHEAR_DAMAGE         = 143424, // Damage.

    SPELL_SHADOW_WORD_BANE         = 143446, // Cast time and Dumy on Effect 0 for targets apply of SPELL_SHADOW_WORD_BANE_HIT (2, 5, 4, 10 targets).
    SPELL_SHADOW_WORD_BANE_HIT     = 143434, // Dummy for Jump on Effect 0, Periodic Damage aura apply (Effect 1).
    SPELL_SHADOW_WORD_BANE_JUMP    = 143443, // Jump spell between players on tick. Dummy on Effect 0 to apply SPELL_SHADOW_WORD_BANE_HIT.
    SPELL_SHADOW_WORD_BANE_SELECT  = 143438,

    SPELL_CALAMITY                 = 143491, // Cast time and Dummy on Effect 0 for SPELL_CALAMITY_DAMAGE.
    SPELL_CALAMITY_DAMAGE          = 143493, // Damage health % (Effect 0) and Dummy (Effect 1) for 10% Heroic increase.
    SPELL_CALAMITY_OVERPOWER       = 143544,

    SPELL_DARK_MEDITATION          = 143546, // Creates Areatrigger 1032 (Dark Meditation) Effect 0, Periodic Dummy (Effect 1) for SPELL_DARK_MEDITATION_DAMAGE trigger every 0.5 seconds.
    SPELL_DARK_MEDITATION_DAMAGE   = 143559, // Damage.
    SPELL_DARK_MEDITATION_VISUAL   = 143843, // Visual area and Dummy Effect 0 (Unk).
    SPELL_DARK_MEDITATION_DMG_RED  = 143564, // Damage reduction aura on players inside the field.

    // ! On HEROIC difficulty, Sun Tenderheart periodically fires a bolt of dark energy !
    SPELL_MEDITATION_SPIKE_MISSILE = 143599, // Triggers SPELL_MEDITATION_SPIKE_DAMAGE.
    SPELL_MEDITATION_SPIKE_DAMAGE  = 143602, // Damage.

    // Embodied Misery
    SPELL_DEFILED_GROUND           = 143961, // Damage, Knockback and trigger SPELL_DEFILED_GROUND_AT (Effect 1).
    SPELL_DEFILED_GROUND_AT        = 143960, // Creates Areatrigger 1053 (Defiled Ground).
    SPELL_DEFILED_GROUND_AURA      = 143959, // Triggers Periodic Damage (Player aura).

    // Embodied Sorrow
    SPELL_INFERNO_STRIKE           = 143962, // Dummy for damage increase on Effect 0, Damage on effect 1. Damage split between targets in 8 yards.

    // Embodied Gloom 
    SPELL_CORRUPTION_SHOCK         = 143958, // Dummy on Effect 0 for SPELL_CORRUPTION_SHOCK_MISSILE (2, 5, 4, 10 targets).
    SPELL_CORRUPTION_SHOCK_MISSILE = 144020, // Triggers SPELL_CORRUPTION_SHOCK_DAMAGE at location.
    SPELL_CORRUPTION_SHOCK_DAMAGE  = 144018, // Damage.

    // ! On HEROIC difficulty, Corruption Chain replaces Corruption Shock !
    SPELL_CORRUPTION_CHAIN         = 145653, // Cast time and dummy on Effect 0 for SPELL_CORRUPTION_CHAIN_DAMAGE.
    SPELL_CORRUPTION_CHAIN_DAMAGE  = 145631, // Damage and chain.

    // All three above.
    SPELL_SHARED_TORMENT           = 145655, // All three NPC's share health.

    // Embodied Anguish
    SPELL_SHADOW_WEAKNESS_AURA     = 144079, // Proc aura for SPELL_SHADOW_WEAKNESS.
    SPELL_SHADOW_WEAKNESS          = 144176,
    SPELL_SHADOW_WEAKNESS_TRANSFER = 144081, // Dummy on Effect 0 to apply a stack of SPELL_SHADOW_WEAKNESS on all players when using SPELL_MARK_OF_ANGUISH_TRANSFER.

    // Manifest Emotions � Embodied Despair and Embodied Desperation focus their negative emotions, creating Sha manifestations which attack players. 
    // Any damage taken by these manifested emotions will also be suffered by the creature that spawned them.

    // Embodied Despair
    SPELL_MANIFEST_DESPAIR         = 143746, // Summons NPC_DESPAIR_SPAWN.

    // Embodied Desperation
    SPELL_MANIFEST_DESPERATION     = 144504,  // Summons NPC_DESPERATION_SPAWN.

    // Achievement
    SPELL_GO_LONG                  = 145538,
};

enum Events
{
    // Rook Stonetoe
    EVENT_ROOK_VENGEFUL_STRIKES    = 1,      //  7 seconds after combat start. Every 21 seconds.
    EVENT_ROOK_CORRUPTED_BREW,               // 18 seconds after combat start. Every 17-25 seconds.
    EVENT_ROOK_CLASH,                        // 45 seconds after combat start. Every 46 seconds.
    EVENT_ROOK_CORRUPTION_KICK,              // After Clash.
    EVENT_ROOK_DESPERATE_MEASURES,

    // He Softfoot
    EVENT_HE_GARROTE,                        // 15 seconds after combat start. Every 30-46 (Heroic 20-26) seconds.
    EVENT_HE_GOUGE,                          // 23 seconds after combat start. Every 30-41 seconds.
    EVENT_HE_POISON_DAGGERS,
    EVENT_HE_DESPERATE_MEASURES,

    // Sun Tenderheart
    EVENT_SUN_SHA_SHEAR,
    EVENT_SUN_SHADOW_WORD_BANE,              // 15 seconds after combat start. Every 17-25 (Heroic 13-20) seconds.
    EVENT_SUN_CALAMITY,                      // 31 seconds after combat start. Every 40-50 seconds.
    EVENT_SUN_DESPERATE_MEASURES,
    EVENT_SUN_MEDITATION_SPIKE,              // HEROIC only!

    // General
    EVENT_CHECK_BOND_OF_THE_GOLDEN_LOTUS,
    EVENT_BERSERK,                           // 900 seconds Normal. 600 seconds Heroic.

    // Embodied Misery
    EVENT_DEFILED_GROUND,                    // Every 10.5 seconds.

    // Embodied Sorrow
    EVENT_INFERNO_STRIKE,                    // Every 9.5 seconds.

    // Embodied Gloom 
    EVENT_CORRUPTION_SHOCK_CHAIN,            // Shock -> Chain HEROIC!

    // Embodied Despair
    EVENT_MANIFEST_DESPAIR,

    // Embodied Desperation
    EVENT_MANIFEST_DESPERATION
};

enum Actions
{
    ACTION_EVENT_COMPLETE = 1,
    ACTION_NEW_MARK,
    ACTION_REMOVE_CLASH,
};

const std::vector<uint32> protectorsList =
{
    NPC_ROOK_STONETOE,
    NPC_HE_SOFTFOOT,
    NPC_SUN_TENDERHEART,
};

const std::map<uint32, Position> invRookDespairType =
{
    { NPC_EMBODIED_SORROW, { 1229.10f, 1034.04f, 418.06f, 3.26f } },
    { NPC_EMBODIED_MISERY, { 1197.40f, 1014.59f, 418.06f, 0.17f } },
    { NPC_EMBODIED_GLOOM,  { 1191.78f, 1046.75f, 417.99f, 0.17f } },
};

const std::map<uint32, uint32> invCorruptedBrewType =
{
    { 0, SPELL_CORRUPTED_BREW_MISSILE },
    { 1, SPELL_CORRUPTED_BREW_X1      },
    { 2, SPELL_CORRUPTED_BREW_X2      },
    { 3, SPELL_CORRUPTED_BREW_X3      },
    { 4, SPELL_CORRUPTED_BREW_x4      },
    { 5, SPELL_CORRUPTED_BREW_X5      },
    { 6, SPELL_CORRUPTED_BREW_X6      },
    { 7, SPELL_CORRUPTED_BREW_x7      },
};

// Rook Stonetoe 71475 - Acts as controller!
class boss_rook_stonetoe : public CreatureScript
{
    public:
        boss_rook_stonetoe() : CreatureScript("boss_rook_stonetoe") { }

        struct boss_rook_stonetoeAI : public BossAI
        {
            boss_rook_stonetoeAI(Creature* creature) : BossAI(creature, DATA_FALLEN_PROTECTORS) 
            {
                eventComplete = false;
            }

            bool doneDesperateMeasuresPhase, doneDesperateMeasuresPhase2;
            bool lotusScheduled;
            bool eventComplete;
            bool hasEvade;
            bool allowAchiev;
            ObjectGuid targetGUID;
            uint32 corruptedBrewCounter;
            uint32 markSummaryDistance;
            std::set<uint64> markSenderGUIDs;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                scheduler.CancelAll();
                me->SetReactState(REACT_DEFENSIVE);

                doneDesperateMeasuresPhase  = false;
                doneDesperateMeasuresPhase2 = false;

                lotusScheduled = false;
                hasEvade       = false;
                allowAchiev    = false;
                targetGUID = ObjectGuid::Empty;
                corruptedBrewCounter = 0;
                markSummaryDistance  = 0;
                markSenderGUIDs.clear();

                if (instance)
                    instance->SetData(DATA_FALLEN_PROTECTORS, NOT_STARTED);

                _Reset();

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetReactState(REACT_AGGRESSIVE);

                if (Creature* heSoftfoot = me->FindNearestCreature(NPC_HE_SOFTFOOT, 300.0f, true))
                    if (!heSoftfoot->IsInCombat())
                        heSoftfoot->AI()->DoZoneInCombat();
                if (Creature* sunTenderheart = me->FindNearestCreature(NPC_SUN_TENDERHEART, 300.0f, true))
                    if (!sunTenderheart->IsInCombat())
                        sunTenderheart->AI()->DoZoneInCombat();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(TALK_ROOK_AGGRO);

                events.ScheduleEvent(EVENT_ROOK_VENGEFUL_STRIKES, 7000);
                events.ScheduleEvent(EVENT_ROOK_CORRUPTED_BREW, 18000);
                events.ScheduleEvent(EVENT_ROOK_CLASH, 45000);

                events.ScheduleEvent(EVENT_BERSERK, (!IsHeroic() ? 15 : 10) * MINUTE * IN_MILLISECONDS);

                if (instance)
                    instance->SetData(DATA_FALLEN_PROTECTORS, IN_PROGRESS);

                _JustEngagedWith();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (lotusScheduled)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && !lotusScheduled)
                {
                    damage = 0;
                    me->SetHealth(1);
                    me->InterruptNonMeleeSpells(true);
                    Talk(TALK_ROOK_BOND_OF_GOLDEN_LOTUS);
                    Talk(TALK_ROOK_BOND_OF_GOLDEN_LOTUS_ANN);
                    DoCast(me, SPELL_BOND_OF_THE_GOLDEN_LOTUS);
                    lotusScheduled = true;
                    events.Reset();
                    return;
                }

                // Schedule Desperate Measures phase entrance.
                if (me->HealthBelowPct(67) && !doneDesperateMeasuresPhase || me->HealthBelowPct(34) && !doneDesperateMeasuresPhase2)
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    events.ScheduleEvent(EVENT_ROOK_DESPERATE_MEASURES, 500);
                    if (!doneDesperateMeasuresPhase)
                        doneDesperateMeasuresPhase = true;
                    else
                        doneDesperateMeasuresPhase2 = true;
                    return;
                }
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                markSenderGUIDs.insert(guid);

                if (markSenderGUIDs.size() > 4 && markSummaryDistance > 99 && !allowAchiev)
                    allowAchiev = true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_GO_LONG:
                        markSummaryDistance += data;
                        break;
                    case TYPE_GO_LONG_CLEAR:
                        markSummaryDistance = 0;
                        markSenderGUIDs.clear();
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void DespawnSummon(uint32 entry)
            {
                std::list<Creature*> summonsList;
                GetCreatureListWithEntryInGrid(summonsList, me, entry, 200.0f);

                for (auto&& itr : summonsList)
                    itr->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* victim) override
            {
                /*if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(ROOK_SAY_KILL);*/
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_HEALED_BY_BOND_OF_LOTUS:
                        lotusScheduled = false;
                        scheduler.CancelAll();

                        events.ScheduleEvent(EVENT_ROOK_VENGEFUL_STRIKES, 7000);
                        events.ScheduleEvent(EVENT_ROOK_CORRUPTED_BREW, 18000);
                        events.ScheduleEvent(EVENT_ROOK_CLASH, 45000);
                        break;
                    case ACTION_REMOVE_CLASH:
                        if (!me->HasAura(SPELL_MISSERY_SORROW_GLOOM))
                            me->RemoveChanneledCast(targetGUID);
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (hasEvade)
                    return;

                hasEvade = true;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
                me->RemoveAllAreasTrigger();
                scheduler.CancelAll();

                if (instance)
                {
                    instance->SetData(DATA_FALLEN_PROTECTORS, FAIL);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_BREW_DAMAGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTION_KICK_EFF);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                // Evade Encounter
                for (auto&& itr : protectorsList)
                    if (Creature* fallenProtector = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        fallenProtector->AI()->EnterEvadeMode();

                _EnterEvadeMode();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_BOND_OF_LOTUS)
                    return lotusScheduled ? 1 : 0;

                return 0;
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);

                _JustReachedHome();
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (allowAchiev)
                        instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_GO_LONG);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                if (hasProtectorsEncounterDone() && !eventComplete)
                {
                    eventComplete = true;
                    HandleFinishEncounter();
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ROOK_VENGEFUL_STRIKES:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_VENGEFUL_STRIKES);

                            events.ScheduleEvent(EVENT_ROOK_VENGEFUL_STRIKES, 21000);
                            break;

                        case EVENT_ROOK_CORRUPTED_BREW:
                            if (IsHeroic() && ++corruptedBrewCounter % 2 == 0)
                                DoCast(me, SPELL_CORRUPTED_BREW_OVERPOWER, true);

                            DoCast(me, SPELL_CORRUPTED_BREW);
                            events.ScheduleEvent(EVENT_ROOK_CORRUPTED_BREW, urand(17000, 25000));
                            break;

                        case EVENT_ROOK_CLASH:
                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            // Not use this spell during any Measures phase
                            if (isAnyMeasuresPhase())
                            {
                                events.RescheduleEvent(EVENT_ROOK_CLASH, 2 * IN_MILLISECONDS);
                                break;
                            }

                            DoCast(me, SPELL_CLASH);
                            events.ScheduleEvent(EVENT_ROOK_CORRUPTION_KICK, 1 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_ROOK_CLASH, 46000);
                            break;
                        case EVENT_ROOK_CORRUPTION_KICK:
                            Talk(ROOK_SAY_CORRUPTION_KICK);
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();

                            DoCast(me, SPELL_CORRUPTION_KICK);
                            break;

                        case EVENT_ROOK_DESPERATE_MEASURES:
                            Talk(TALK_ROOK_DESPERATE_ANN);
                            Talk(TALK_ROOK_DESPERATE);

                            me->RemoveAurasDueToSpell(SPELL_CORRUPTED_BREW_OVERPOWER);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                            // Cancel the normal events.
                            events.CancelEvent(EVENT_ROOK_VENGEFUL_STRIKES);
                            events.CancelEvent(EVENT_ROOK_CORRUPTED_BREW);
                            events.CancelEvent(EVENT_ROOK_CORRUPTION_KICK);

                            // On Normal difficulty Clash is not cast during this phase. Also the boss does not move.
                            events.CancelEvent(EVENT_ROOK_CLASH);
                            me->AddAura(SPELL_ROOT_GENERIC, me);

                            DoCast(me, SPELL_MISSERY_SORROW_GLOOM);

                            for (auto&& itr : invRookDespairType)
                                if (Creature* rookSpirits = me->SummonCreature(itr.first, fallenProtectorsCenterPos.GetPositionX() + frand(-2.0f, 2.0f), fallenProtectorsCenterPos.GetPositionY() + frand(-2.0f, 2.0f), 450.5f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                    rookSpirits->GetMotionMaster()->MoveJump(itr.second.GetPositionX(), itr.second.GetPositionY(), itr.second.GetPositionZ(), 15.0f, 15.0f, EVENT_JUMP);


                            scheduler
                                .Schedule(Seconds(2), [this](TaskContext context)
                            {
                                if (me->HasAura(SPELL_MISSERY_SORROW_GLOOM) && !isAnyEmbodiedAlive())
                                {
                                    me->RemoveAurasDueToSpell(SPELL_ROOT_GENERIC);
                                    me->RemoveAurasDueToSpell(SPELL_MISSERY_SORROW_GLOOM);

                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                                    // Reschedule the normal events.
                                    events.ScheduleEvent(EVENT_ROOK_VENGEFUL_STRIKES, 7000);
                                    events.ScheduleEvent(EVENT_ROOK_CORRUPTED_BREW, 18000);

                                    events.ScheduleEvent(EVENT_ROOK_CLASH, 45000);
                                }
                                else
                                    context.Repeat(Seconds(1));
                            });
                            break;

                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            if (Creature* heSoftfoot = me->FindNearestCreature(NPC_HE_SOFTFOOT, 300.0f, true))
                                heSoftfoot->CastSpell(heSoftfoot, SPELL_BERSERK, false);
                            if (Creature* sunTenderheart = me->FindNearestCreature(NPC_SUN_TENDERHEART, 300.0f, true))
                                sunTenderheart->CastSpell(sunTenderheart, SPELL_BERSERK, false);
                            break;
                    }
                }

                // Rook does not melee in Desperate Measures phases.
                if (!me->HasAura(SPELL_MISSERY_SORROW_GLOOM))
                    DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

            private:
                bool hasProtectorsEncounterDone()
                {
                    for (auto&& itr : protectorsList)
                        if (Creature* fallenProtector = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                            if (!fallenProtector->AI()->GetData(TYPE_BOND_OF_LOTUS))
                                return false;

                    return true;
                }

                void HandleFinishEncounter()
                {
                    if (Player* target = me->FindNearestPlayer(200.0f))
                    {
                        for (auto&& itr : protectorsList)
                            if (Creature* fallenProtector = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                                target->Kill(fallenProtector);

                        JustDied(target);
                    }

                    // Cho Past Event
                    if (Creature* lorewalkerCho = me->FindNearestCreature(NPC_LOREWALKER_CHO, 100.0f))
                        lorewalkerCho->AI()->DoAction(ACTION_CHO_PAST_PROTECTORS);
                }

                bool isAnyMeasuresPhase()
                {
                    // Doesn`t matter in HC
                    if (IsHeroic())
                        return false;

                    if (Creature* he = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_HE_SOFTFOOT) : ObjectGuid::Empty))
                        if (he->HasAura(SPELL_MARK_OF_ANGUISH_VISUAL))
                            return true;

                    if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                        if (sun->HasAura(SPELL_DARK_MEDITATION_VISUAL))
                            return true;

                    return false;
                }

                bool isAnyEmbodiedAlive()
                {
                    std::list<Creature*> embodiedList;

                    for (auto&& itr : invRookDespairType)
                        GetCreatureListWithEntryInGrid(embodiedList, me, itr.first, 300.0f);

                    // Remove Visual Triggers
                    embodiedList.remove_if([=](Creature* target) { return target && target->AI()->GetData(TYPE_EMBODIED_TRIGGER); });

                    for (auto&& itr : embodiedList)
                        if (itr->IsAlive())
                            return true;

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_rook_stonetoeAI(creature);
        }
};

// He Softfoot 71479.
class boss_he_softfoot : public CreatureScript
{
    public:
        boss_he_softfoot() : CreatureScript("boss_he_softfoot") { }

        struct boss_he_softfootAI : public BossAI
        {
            boss_he_softfootAI(Creature* creature) : BossAI(creature, DATA_FALLEN_PROTECTORS) { }

            bool doneDesperateMeasuresPhase, doneDesperateMeasuresPhase2;
            bool lotusScheduled;
            bool hasEvade;
            ObjectGuid noxGUID;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                scheduler.CancelAll();
                me->SetReactState(REACT_DEFENSIVE);

                doneDesperateMeasuresPhase  = false;
                doneDesperateMeasuresPhase2 = false;

                lotusScheduled = false;
                hasEvade = false;
                noxGUID = ObjectGuid::Empty;

                _Reset();

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });

                // Heroic
                if (IsHeroic())
                {
                    me->SetTotalAuraEffectValue(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, 1.35f);
                    me->UpdateCastingSpeed();
                    me->UpdateHasteRegen();
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetReactState(REACT_AGGRESSIVE);

                if (Creature* rookStonetoe = me->FindNearestCreature(NPC_ROOK_STONETOE, 300.0f, true))
                    if (!rookStonetoe->IsInCombat())
                        rookStonetoe->AI()->DoZoneInCombat();
                if (Creature* sunTenderheart = me->FindNearestCreature(NPC_SUN_TENDERHEART, 300.0f, true))
                    if (!sunTenderheart->IsInCombat())
                        sunTenderheart->AI()->DoZoneInCombat();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_HE_GARROTE, 15000);
                events.ScheduleEvent(EVENT_HE_GOUGE, 23000);
                events.ScheduleEvent(EVENT_HE_POISON_DAGGERS, 45000);

                _JustEngagedWith();
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                if (instance && spell->HasEffect(SPELL_EFFECT_ATTACK_ME))
                {
                    me->RemoveAurasDueToSpell(SPELL_HE_FIXATE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HE_FIXATE);
                }

            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                noxGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return noxGUID;
            }

            void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
            {
                if (victim && victim->GetGUID() != GetGUID(0))
                    SetGUID(victim->GetGUID(), 0);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (lotusScheduled)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && !lotusScheduled)
                {
                    damage = 0;
                    me->SetHealth(1);
                    me->InterruptNonMeleeSpells(true);
                    Talk(TALK_HE_BOND_OF_GOLDEN_LOTUS);
                    Talk(TALK_HE_BOND_OF_GOLDEN_LOTUS_ANN);
                    DoCast(me, SPELL_BOND_OF_THE_GOLDEN_LOTUS);
                    lotusScheduled = true;
                    events.Reset();
                    return;
                }

                // Schedule Desperate Measures phase entrance.
                if (me->HealthBelowPct(67) && !doneDesperateMeasuresPhase || me->HealthBelowPct(34) && !doneDesperateMeasuresPhase2)
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    events.ScheduleEvent(EVENT_HE_DESPERATE_MEASURES, 500);
                    if (!doneDesperateMeasuresPhase)
                        doneDesperateMeasuresPhase = true;
                    else
                        doneDesperateMeasuresPhase2 = true;
                    return;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void DespawnSummon(uint32 entry)
            {
                std::list<Creature*> summonsList;
                GetCreatureListWithEntryInGrid(summonsList, me, entry, 200.0f);

                for (auto&& itr : summonsList)
                    itr->DespawnOrUnsummon();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_HEALED_BY_BOND_OF_LOTUS:
                        lotusScheduled = false;

                        events.ScheduleEvent(EVENT_HE_GARROTE, 15000);
                        events.ScheduleEvent(EVENT_HE_GOUGE, 23000);
                        events.ScheduleEvent(EVENT_HE_POISON_DAGGERS, 45000);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_BOND_OF_LOTUS)
                    return lotusScheduled ? 1 : 0;

                return 0;
            }

            void EnterEvadeMode() override
            {
                if (hasEvade)
                    return;

                hasEvade = true;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                me->AddUnitState(UNIT_STATE_EVADE);

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();
                me->RemoveAllAreasTrigger();
                scheduler.CancelAll();

                summons.DespawnAll();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HE_GARROTE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HE_GOUGE_DMG_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ANGUISH_MAIN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ANGUISH_TRANSFER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WEAKNESS_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WEAKNESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEBILITATION);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                // Evade Encounter
                for (auto&& itr : protectorsList)
                    if (Creature* fallenProtector = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        fallenProtector->AI()->EnterEvadeMode();

                _EnterEvadeMode();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);

                _JustReachedHome();
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HE_GARROTE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HE_GOUGE_DMG_STUN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ANGUISH_MAIN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ANGUISH_TRANSFER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WEAKNESS_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WEAKNESS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DEBILITATION);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HE_GARROTE:
                            DoCast(me, SPELL_SHADOWSTEP_SELECTOR);
                            events.ScheduleEvent(EVENT_HE_GARROTE, IsHeroic() ? urand(20000, 26000) : urand(30000, 46000));
                            break;

                        case EVENT_HE_GOUGE:
                            Talk(TALK_HE_GOUGE_ANN, me->GetVictim());
                            DoCast(me->GetVictim(), SPELL_HE_GOUGE);
                            events.ScheduleEvent(EVENT_HE_GOUGE, urand(30000, 41000));
                            break;

                        case EVENT_HE_POISON_DAGGERS:
                            if (!me->HasAura(SPELL_INSTANT_POISON) && !me->HasAura(SPELL_NOXIOUS_POISON)) // First cast, no poison selected yet, select random one.
                            {
                                uint32 poisonChoice = urand(0, 1);
                                Talk(poisonChoice ? TALK_HE_ANNOUNCE_NOXIOUS_POISON : TALK_HE_ANNOUNCE_INSTANT_POISON);
                                DoCast(me, poisonChoice ? SPELL_NOXIOUS_POISON : SPELL_INSTANT_POISON);
                            }
                            else if (me->HasAura(SPELL_INSTANT_POISON))                                   // Has Instant Poison, select Noxious Poison.
                            {
                                Talk(TALK_HE_ANNOUNCE_NOXIOUS_POISON);
                                me->RemoveAurasDueToSpell(SPELL_INSTANT_POISON);
                                DoCast(me, SPELL_NOXIOUS_POISON);
                            }
                            else                                                                          // Has Noxious Poison, select Instant Poison.
                            {
                                Talk(TALK_HE_ANNOUNCE_INSTANT_POISON);
                                me->RemoveAurasDueToSpell(SPELL_NOXIOUS_POISON);
                                DoCast(me, SPELL_INSTANT_POISON);
                            }
                            events.ScheduleEvent(EVENT_HE_POISON_DAGGERS, 46000);
                            break;

                        case EVENT_HE_DESPERATE_MEASURES:
                            Talk(TALK_HE_DESPERATE);
                            Talk(TALK_HE_DESPERATE_ANN);

                            // Remove Garrote effect from players and Noxious Poison trigger aura from boss.
                            me->RemoveAurasDueToSpell(SPELL_NOXIOUS_POISON);
                            me->RemoveAllAreasTrigger();

                            if (instance)
                                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HE_GARROTE);

                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                            // Cancel the normal events.
                            events.CancelEvent(EVENT_HE_GARROTE);
                            events.CancelEvent(EVENT_HE_GOUGE);
                            events.CancelEvent(EVENT_HE_POISON_DAGGERS);
                            me->AddAura(SPELL_ROOT_GENERIC, me);

                            me->AddAura(SPELL_MARK_OF_ANGUISH_VISUAL, me);

                            // Achiev
                            if (Creature* rook = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROOK_STONETOE) : ObjectGuid::Empty))
                                rook->AI()->SetData(TYPE_GO_LONG_CLEAR, 1);

                            if (Creature* anguish = me->SummonCreature(NPC_EMBODIED_ANGUISH, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                anguish->CastSpell(anguish, SPELL_MARK_OF_ANGUISH, false);

                            scheduler
                                .Schedule(Seconds(1), [this](TaskContext context)
                            {
                                if (me->HasAura(SPELL_MARK_OF_ANGUISH_VISUAL) && !me->FindNearestCreature(NPC_EMBODIED_ANGUISH, 300.0f, true))
                                {
                                    me->RemoveAurasDueToSpell(SPELL_ROOT_GENERIC);
                                    me->RemoveAurasDueToSpell(SPELL_MARK_OF_ANGUISH_VISUAL);

                                    if (instance)
                                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WEAKNESS);

                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                                    // Reschedule the normal events.
                                    events.ScheduleEvent(EVENT_HE_GARROTE, 15000);
                                    events.ScheduleEvent(EVENT_HE_GOUGE, 23000);

                                    // On Normal difficulty needs reschedule.
                                    events.ScheduleEvent(EVENT_HE_POISON_DAGGERS, 45000);
                                }
                                else
                                    context.Repeat(Seconds(1));
                            });
                            break;
                    }
                }

                // He does not melee in Desperate Measures phases.
                if (!me->HasAura(SPELL_MARK_OF_ANGUISH_VISUAL))
                    DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_he_softfootAI(creature);
        }
};

// Sun Tenderheart 71480.
class boss_sun_tenderheart : public CreatureScript
{
    public:
        boss_sun_tenderheart() : CreatureScript("boss_sun_tenderheart") { }

        struct boss_sun_tenderheartAI : public BossAI
        {
            boss_sun_tenderheartAI(Creature* creature) : BossAI(creature, DATA_FALLEN_PROTECTORS) 
            {
                SetCombatMovement(false);
            }

            bool doneDesperateMeasuresPhase, doneDesperateMeasuresPhase2;
            bool lotusScheduled;
            ObjectGuid shadowWordTargetGUID;
            bool hasEvade;
            uint32 calamityPower;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                scheduler.CancelAll();
                me->SetReactState(REACT_DEFENSIVE);

                doneDesperateMeasuresPhase  = false;
                doneDesperateMeasuresPhase2 = false;

                lotusScheduled = false;
                hasEvade       = false;
                shadowWordTargetGUID = ObjectGuid::Empty;
                calamityPower        = 1;

                _Reset();

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                shadowWordTargetGUID = guid;
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return shadowWordTargetGUID;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->SetReactState(REACT_AGGRESSIVE);

                if (Creature* rookStonetoe = me->FindNearestCreature(NPC_ROOK_STONETOE, 300.0f, true))
                    if (!rookStonetoe->IsInCombat())
                        rookStonetoe->AI()->DoZoneInCombat();
                if (Creature* heSoftfoot = me->FindNearestCreature(NPC_HE_SOFTFOOT, 300.0f, true))
                    if (!heSoftfoot->IsInCombat())
                        heSoftfoot->AI()->DoZoneInCombat();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_SUN_SHA_SHEAR, 2000);
                events.ScheduleEvent(EVENT_SUN_SHADOW_WORD_BANE, 15000);
                events.ScheduleEvent(EVENT_SUN_CALAMITY, 31000);

                _JustEngagedWith();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (lotusScheduled)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && !lotusScheduled)
                {
                    damage = 0;
                    me->SetHealth(1);
                    me->InterruptNonMeleeSpells(true);
                    Talk(TALK_SUN_BOND_OF_GOLDEN_LOTUS);
                    Talk(TALK_SUN_BOND_OF_GOLDEN_LOTUS_ANN);
                    DoCast(me, SPELL_BOND_OF_THE_GOLDEN_LOTUS);
                    lotusScheduled = true;
                    events.Reset();
                    return;
                }

                // Schedule Desperate Measures phase entrance.
                if (me->HealthBelowPct(67) && !doneDesperateMeasuresPhase || me->HealthBelowPct(34) && !doneDesperateMeasuresPhase2)
                {
                    if (me->HasUnitState(UNIT_STATE_CASTING) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL) && me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_CALAMITY)
                        return;

                    events.ScheduleEvent(EVENT_SUN_DESPERATE_MEASURES, 500);
                    if (!doneDesperateMeasuresPhase)
                        doneDesperateMeasuresPhase = true;
                    else
                        doneDesperateMeasuresPhase2 = true;
                    return;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void DespawnSummon(uint32 entry)
            {
                std::list<Creature*> summonsList;
                GetCreatureListWithEntryInGrid(summonsList, me, entry, 200.0f);

                for (auto&& itr : summonsList)
                    itr->DespawnOrUnsummon();
            }

            void KilledUnit(Unit* victim) override
            {
                /*if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SUN_SAY_KILL);*/
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_BOND_OF_LOTUS:
                        return lotusScheduled ? 1 : 0;
                    case TYPE_CALAMITY_OVERPOWER:
                        return calamityPower;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_CALAMITY_OVERPOWER)
                    calamityPower = data;
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                events.RescheduleEvent(EVENT_SUN_SHA_SHEAR, 6 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_HEALED_BY_BOND_OF_LOTUS:
                        lotusScheduled = false;

                        events.ScheduleEvent(EVENT_SUN_SHA_SHEAR, 2000);
                        events.ScheduleEvent(EVENT_SUN_SHADOW_WORD_BANE, 15000);
                        events.ScheduleEvent(EVENT_SUN_CALAMITY, 31000);
                        break;
                }
            };

            void EnterEvadeMode() override
            {
                if (hasEvade)
                    return;

                hasEvade = true;

                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                DespawnSummon(NPC_DESPAIR_SPAWN);
                DespawnSummon(NPC_DESPERATION_SPAWN);

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                summons.DespawnAll();
                me->RemoveAllAreasTrigger();
                scheduler.CancelAll();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WORD_BANE_HIT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WORD_BANE_JUMP);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                // Evade Encounter
                for (auto&& itr : protectorsList)
                    if (Creature* fallenProtector = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        fallenProtector->AI()->EnterEvadeMode();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void JustReachedHome() override
            {
                me->ClearUnitState(UNIT_STATE_EVADE);

                _JustReachedHome();
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WORD_BANE_HIT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WORD_BANE_JUMP);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUN_SHA_SHEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                DoCast(target, SPELL_SHA_SHEAR);

                            events.ScheduleEvent(EVENT_SUN_SHA_SHEAR, 3000);
                            break;

                        case EVENT_SUN_SHADOW_WORD_BANE:
                            DoCast(me, SPELL_SHADOW_WORD_BANE);
                            events.ScheduleEvent(EVENT_SUN_SHADOW_WORD_BANE, IsHeroic() ? urand(13000, 20000) : urand(18000, 25000));
                            break;

                        case EVENT_SUN_CALAMITY:
                            Talk(TALK_SUN_CALAMITY);
                            Talk(TALK_SUN_ANNOUNCE_CALAMITY);

                            // Not use this spell during any Measures phase
                            if (isAnyMeasuresPhase())
                            {
                                events.RescheduleEvent(EVENT_SUN_CALAMITY, 2 * IN_MILLISECONDS);
                                break;
                            }

                            // Remove Shadow Word : Bane effect from players.
                            if (instance)
                                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHADOW_WORD_BANE_HIT);

                            DoCast(me, SPELL_CALAMITY);
                            events.ScheduleEvent(EVENT_SUN_CALAMITY, 40 * IN_MILLISECONDS);
                            break;

                        case EVENT_SUN_DESPERATE_MEASURES:
                        {
                            Talk(TALK_SUN_DESPERATE_ANN);

                            me->RemoveAurasDueToSpell(SPELL_CALAMITY_OVERPOWER);
                            calamityPower = 1;
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                            // Cancel the normal events.
                            events.CancelEvent(EVENT_SUN_SHA_SHEAR);
                            events.CancelEvent(EVENT_SUN_SHADOW_WORD_BANE);

                            // On Normal difficulty Calamity is not cast during this phase.
                            events.CancelEvent(EVENT_SUN_CALAMITY);
                            me->AddAura(SPELL_ROOT_GENERIC, me);

                            if (!IsHeroic())
                            {
                                // Remove He`s poisons while it casting
                                if (Creature* he = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_HE_SOFTFOOT) : ObjectGuid::Empty))
                                {
                                    he->RemoveAurasDueToSpell(SPELL_INSTANT_POISON);
                                    he->RemoveAurasDueToSpell(SPELL_NOXIOUS_POISON);
                                }
                            }

                            me->AddAura(SPELL_DARK_MEDITATION_VISUAL, me);

                            DoCast(me, SPELL_DARK_MEDITATION, true);

                            float sOri = urand(0.0f, 2 * M_PI);
                            float x, y;
                            GetPositionWithDistInOrientation(me, 26.5f, sOri, x, y);

                            // Despair
                            if (Creature* despair = me->SummonCreature(NPC_EMBODIED_DESPAIR, fallenProtectorsCenterPos.GetPositionX() + frand(-2.0f, 2.0f), fallenProtectorsCenterPos.GetPositionY() + frand(-2.0f, 2.0f), 450.5f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                despair->GetMotionMaster()->MoveJump(x, y, 418.1f, 15.0f, 15.0f, EVENT_JUMP);

                            // Desperation always in inverse side
                            GetPositionWithDistInOrientation(me, 26.5f, Position::NormalizeOrientation(sOri - M_PI), x, y);

                            if (Creature* despair = me->SummonCreature(NPC_EMBODIED_DESPERATION, fallenProtectorsCenterPos.GetPositionX() + frand(-2.0f, 2.0f), fallenProtectorsCenterPos.GetPositionY() + frand(-2.0f, 2.0f), 450.5f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 1000))
                                despair->GetMotionMaster()->MoveJump(x, y, 418.1f, 15.0f, 15.0f, EVENT_JUMP);

                            scheduler
                                .Schedule(Seconds(1), [this](TaskContext context)
                            {
                                if (!me->FindNearestCreature(NPC_EMBODIED_DESPAIR, 300.0f, true) && !me->FindNearestCreature(NPC_EMBODIED_DESPERATION, 300.0f, true) && me->HasAura(SPELL_DARK_MEDITATION_VISUAL))
                                {
                                    me->RemoveAurasDueToSpell(SPELL_ROOT_GENERIC);

                                    me->RemoveAurasDueToSpell(SPELL_DARK_MEDITATION_VISUAL);
                                    me->RemoveAurasDueToSpell(SPELL_DARK_MEDITATION);
                                    me->RemoveAllAreasTrigger();

                                    // Remove Meditative Field effect from players.
                                    if (instance)
                                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DARK_MEDITATION_DMG_RED);

                                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                                    // Reschedule the normal events.
                                    events.ScheduleEvent(EVENT_SUN_SHA_SHEAR, 2000);
                                    events.ScheduleEvent(EVENT_SUN_SHADOW_WORD_BANE, 15000);

                                    // On Normal difficulty Calamity needs reschedule.
                                    events.ScheduleEvent(EVENT_SUN_CALAMITY, 31000);
                                }
                                else
                                    context.Repeat(Seconds(1));
                            });
                            break;
                        }
                    }
                }

                // No melee, no tank needed.
            }

            private:
                bool isAnyMeasuresPhase()
                {
                    // Doesn`t matter in HC
                    if (IsHeroic())
                        return false;

                    if (Creature* he = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_HE_SOFTFOOT) : ObjectGuid::Empty))
                        if (he->HasAura(SPELL_MARK_OF_ANGUISH_VISUAL))
                            return true;

                    if (Creature* rook = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_ROOK_STONETOE) : ObjectGuid::Empty))
                        if (rook->HasAura(SPELL_MISSERY_SORROW_GLOOM))
                            return true;

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sun_tenderheartAI(creature);
        }
};

// Embodied Misery 71476.
struct npc_embodied_misery : public ScriptedAI
{
    npc_embodied_misery(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    uint32 summonerEntry;
    uint32 triggerData;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerEntry = summoner->GetEntry();
    }

    void Reset() override 
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        triggerData = 0;

        // On Heroic they share health.
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (summonerEntry == NPC_ROOK_STONETOE)
                DoCast(me, SPELL_SHARED_TORMENT);
            else
            {
                triggerData = 1;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        });
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_EMBODIED_TRIGGER)
            return triggerData;

        return 0;
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);

            for (auto&& itr:me->GetInstanceScript()->instance->GetPlayers())
            {
                if (Player* player = itr.GetSource())
                {
                    if (player->IsGameMaster())
                        continue;

                    if (player->IsAlive())
                    {
                        me->SetInCombatState(player);
                        player->SetInCombatWith(me);
                        me->AddThreat(player, 0.0f);
                    }
                }
            }

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            events.ScheduleEvent(EVENT_DEFILED_GROUND, 8 * IN_MILLISECONDS);

            if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                me->SetFacingTo(me->GetAngle(sun));
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DEFILED_GROUND:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_DEFILED_GROUND);

                    events.ScheduleEvent(EVENT_DEFILED_GROUND, 10500);
                    break;

                default: break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Embodied Sorrow 71481.
struct npc_embodied_sorrow : public ScriptedAI
{
    npc_embodied_sorrow(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    uint32 summonerEntry;
    uint32 triggerData;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerEntry = summoner->GetEntry();
    }

    void Reset() override 
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
        SetCombatMovement(false);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        triggerData = 0;

        // On Heroic they share health.
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (summonerEntry == NPC_ROOK_STONETOE)
                DoCast(me, SPELL_SHARED_TORMENT);
            else
            {
                triggerData = 1;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        });
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_EMBODIED_TRIGGER)
            return triggerData;

        return 0;
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);

            for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
            {
                if (Player* player = itr.GetSource())
                {
                    if (player->IsGameMaster())
                        continue;

                    if (player->IsAlive())
                    {
                        me->SetInCombatState(player);
                        player->SetInCombatWith(me);
                        me->AddThreat(player, 0.0f);
                    }
                }
            }

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            events.ScheduleEvent(EVENT_INFERNO_STRIKE, 6 * IN_MILLISECONDS);

            if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                me->SetFacingTo(me->GetAngle(sun));
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_INFERNO_STRIKE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        DoCast(target, SPELL_INFERNO_STRIKE);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                        DoCast(target, SPELL_INFERNO_STRIKE);

                    events.ScheduleEvent(EVENT_INFERNO_STRIKE, 9500);
                    break;

                default: break;
            }
        }
    }
};

// Embodied Gloom 71477.
struct npc_embodied_gloom : public ScriptedAI
{
    npc_embodied_gloom(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    uint32 summonerEntry;
    uint32 triggerData;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerEntry = summoner->GetEntry();
    }

    void Reset() override 
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        triggerData = 0;

        // On Heroic they share health.
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (summonerEntry == NPC_ROOK_STONETOE)
                DoCast(me, SPELL_SHARED_TORMENT);
            else
            {
                triggerData = 1;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        });
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_EMBODIED_TRIGGER)
            return triggerData;

        return 0;
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);

            for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
            {
                if (Player* player = itr.GetSource())
                {
                    if (player->IsGameMaster())
                        continue;

                    if (player->IsAlive())
                    {
                        me->SetInCombatState(player);
                        player->SetInCombatWith(me);
                        me->AddThreat(player, 0.0f);
                    }
                }
            }

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

            events.ScheduleEvent(EVENT_CORRUPTION_SHOCK_CHAIN, 1.5 * IN_MILLISECONDS);

            if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                me->SetFacingTo(me->GetAngle(sun));
        }
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        events.RescheduleEvent(EVENT_CORRUPTION_SHOCK_CHAIN, 6 * IN_MILLISECONDS);

        if (Unit* vict = me->GetVictim())
            me->GetMotionMaster()->MoveChase(vict);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CORRUPTION_SHOCK_CHAIN:
                    me->StopMoving();

                    if (Unit* vict = me->GetVictim())
                    {
                        if (me->GetExactDist2d(vict) > 50.0f)
                            DoStartMovement(vict);
                        else
                            DoStartNoMovement(vict);
                    }

                    if (!IsHeroic()) // Corruption Shock on Normal.
                        DoCast(me, SPELL_CORRUPTION_SHOCK);
                    else // Corruption Chain on Heroic.
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                            DoCast(target, SPELL_CORRUPTION_CHAIN);
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                            DoCast(target, SPELL_CORRUPTION_CHAIN);
                    }
                    events.ScheduleEvent(EVENT_CORRUPTION_SHOCK_CHAIN, 3 * IN_MILLISECONDS);
                    break;
                
                default: break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Embodied Anguish 71478.
struct npc_embodied_anguish : public ScriptedAI
{
    npc_embodied_anguish(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid markTargetGUID;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_SHADOW_WEAKNESS_AURA);

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        markTargetGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return markTargetGUID;
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        // Add a stack of Shadow Weakness to the victim on melee attack.
        if (damageType == DIRECT_DAMAGE)
            me->AddAura(SPELL_SHADOW_WEAKNESS, victim);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_NEW_MARK)
        {
            if (Unit* oldTarget = ObjectAccessor::GetUnit(*me, markTargetGUID))
                oldTarget->RemoveAurasDueToSpell(SPELL_MARK_OF_ANGUISH_MAIN);

            me->InterruptNonMeleeSpells(true, SPELL_MARK_OF_ANGUISH_MAIN);
            DoCast(me, SPELL_SHADOW_WEAKNESS_TRANSFER, true);

            scheduler
                .Schedule(Milliseconds(500), [this](TaskContext context)
            {
                DoCast(me, SPELL_MARK_OF_ANGUISH, true);
            });
        }
    }

    void JustDied(Unit* killer) override
    {
        if (me->GetInstanceScript())
        {
            me->GetInstanceScript()->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ANGUISH_MAIN);
            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictimWithGaze() || me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoMeleeAttackIfReady();
    }
};

// Embodied Despair 71474.
struct npc_embodied_despair : public ScriptedAI
{
    npc_embodied_despair(Creature* creature) : ScriptedAI(creature), summons(me)
    {
        SetCombatMovement(false);
    }

    SummonList summons;
    EventMap events;

    void Reset() override 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        events.Reset();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            events.ScheduleEvent(EVENT_MANIFEST_DESPAIR, 0.5 * IN_MILLISECONDS);
            me->SummonCreature(NPC_DESPAIR_SPAWN, *me, TEMPSUMMON_MANUAL_DESPAWN);

            if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                me->SetFacingTo(me->GetAngle(sun));

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetInCombatWithZone();
    }

    void JustDied(Unit* killer) override
    {
        summons.DespawnAll();

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MANIFEST_DESPAIR:
                    DoCast(me, SPELL_MANIFEST_DESPAIR);
                    events.ScheduleEvent(EVENT_MANIFEST_DESPAIR, urand(5000, 7000));
                    break;

                default: break;
            }
        }

        // No melee.
    }
};

// Embodied Desperation 71482.
struct npc_embodied_desperation : public ScriptedAI
{
    npc_embodied_desperation(Creature* creature) : ScriptedAI(creature), summons(me)
    {
        SetCombatMovement(false);
    }

    SummonList summons;
    EventMap events;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        events.Reset();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            events.ScheduleEvent(EVENT_MANIFEST_DESPERATION, 0.5 * IN_MILLISECONDS);
            me->SummonCreature(NPC_DESPERATION_SPAWN, *me, TEMPSUMMON_MANUAL_DESPAWN);

            if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                me->SetFacingTo(me->GetAngle(sun));

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetInCombatWithZone();
    }

    void JustDied(Unit* killer) override
    {
        summons.DespawnAll();

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MANIFEST_DESPERATION:
                    DoCast(me, SPELL_MANIFEST_DESPERATION);
                    events.ScheduleEvent(EVENT_MANIFEST_DESPERATION, urand(5000, 7000));
                    break;
                
                default: break;
            }
        }

        // No melee.
    }
};

// Despair Spawn 71712.
struct npc_despair_spawn : public ScriptedAI
{
    npc_despair_spawn(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (Creature* despair = ObjectAccessor::GetCreature(*me, summonerGUID))
            if (damage < despair->GetHealth())
                despair->ModifyHealth(-int32(damage));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoMeleeAttackIfReady();
    }
};

// Desperation Spawn 71993.
struct npc_desperation_spawn : public ScriptedAI
{
    npc_desperation_spawn(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (Creature* desperation = ObjectAccessor::GetCreature(*me, summonerGUID))
            if (damage < desperation->GetHealth())
                desperation->ModifyHealth(-int32(damage));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoMeleeAttackIfReady();
    }
};

// Golden Lotus Trigger 71711, 71684, 71686
struct npc_golden_lotus_trigger : public ScriptedAI
{
    npc_golden_lotus_trigger(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->SetDisplayFromModel(1);

        scheduler
            .Schedule(Milliseconds(2500), [this](TaskContext context)
        {
            if (Creature* embodiedSpirit = me->SummonCreature(goldenLotusCorrupedType.find(me->GetEntry())->second, *me, TEMPSUMMON_MANUAL_DESPAWN))
                embodiedSpirit->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

            if (Creature* sun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
            {
                float dist = me->GetExactDist2d(sun);
                float x, y;

                // spin round sun traj
                Movement::MoveSplineInit init(me);
                for (uint8 i = 1; i < 25; ++i)
                {
                    x = sun->GetPositionX() + dist * cos(Position::NormalizeOrientation(me->GetAngle(sun->GetPositionX(), sun->GetPositionY()) + (i * M_PI / 12) - M_PI));
                    y = sun->GetPositionY() + dist * sin(Position::NormalizeOrientation(me->GetAngle(sun->GetPositionX(), sun->GetPositionY()) + (i * M_PI / 12) - M_PI));
                    init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
                }

                init.SetUncompressed();
                init.SetWalk(true);
                init.SetCyclic();
                init.Launch();
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Corrupted Brew (Boss cast) 143019.
class spell_rook_corrupted_brew : public SpellScript
{
    PrepareSpellScript(spell_rook_corrupted_brew);

    std::list<WorldObject*> m_targets;

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            if (GetCaster()->GetMap()->IsHeroic())
            {
                uint32 power = GetCaster()->GetAura(SPELL_CORRUPTED_BREW_OVERPOWER) ? GetCaster()->GetAura(SPELL_CORRUPTED_BREW_OVERPOWER)->GetStackAmount() : 0;
                
                if (power > 7)
                    power = 7;

                uint32 spellId = invCorruptedBrewType.find(power)->second;
                GetCaster()->CastSpell(target, spellId, true);

            }
            else
                GetCaster()->CastSpell(target, SPELL_CORRUPTED_BREW_MISSILE, true);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 1(3)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rook_corrupted_brew::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rook_corrupted_brew::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class clashDistanceOrderPred
{
    public:
        clashDistanceOrderPred(const WorldObject* object, bool ascending = true) : m_object(object), m_ascending(ascending) {}
        bool operator() (const WorldObject* a, const WorldObject* b) const
        {
            return m_ascending ? a->GetDistance(m_object) < b->GetDistance(m_object) :
                a->GetDistance(m_object) > b->GetDistance(m_object);
        }
    private:
        const WorldObject* m_object;
        const bool m_ascending;
};

// Clash (Boss cast) 143027.
class spell_rook_clash : public SpellScript
{
    PrepareSpellScript(spell_rook_clash);

    std::list<WorldObject*> m_targets;

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                float dist = caster->GetExactDist2d(target);

                // lower not calculate
                if (dist < 2.0f)
                {
                    caster->CastSpell(target, SPELL_CLASH_BY_ROOK, true);
                    target->CastSpell(caster, SPELL_CLASH_BY_PLAYER, true);
                }
                else // calculate middle
                {
                    GetPositionWithDistInOrientation(caster, dist / 2.0f, caster->GetAngle(target), x, y);
                    caster->CastSpell(x, y, caster->GetPositionZ(), SPELL_CLASH_BY_ROOK, true);
                    target->CastSpell(x, y, target->GetPositionZ(), SPELL_CLASH_BY_PLAYER, true);
                }
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());

        if (!targets.empty())
        {
            targets.sort(clashDistanceOrderPred(GetCaster(), false));
            targets.resize(1);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rook_clash::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rook_clash::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Gouge (Boss Cast) 143330.
class spell_he_gouge : public SpellScript
{
    PrepareSpellScript(spell_he_gouge);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        // If the tank is facing the boss, incapacitate him. Else, just knock him back a bit.
        if (target->isInFront(caster, 2.5f))
        {
            caster->CastSpell(target, SPELL_HE_GOUGE_DMG_STUN, true);
            
            if (caster->ToCreature())
            {
                if (Unit* fixateTarget = caster->ToCreature()->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                    caster->CastSpell(fixateTarget, SPELL_HE_FIXATE, true);
                else if (Unit* fixateTarget = caster->ToCreature()->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true, -SPELL_HE_GOUGE_DMG_STUN))
                    caster->CastSpell(fixateTarget, SPELL_HE_FIXATE, true);
            }

        }
        else
            caster->CastSpell(target, SPELL_HE_GOUGE_KB, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_he_gouge::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Instant Poison 143210.
class spell_he_instant_poison : public SpellScript
{
    PrepareSpellScript(spell_he_instant_poison);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        caster->RemoveAurasDueToSpell(SPELL_NOXIOUS_POISON);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_he_instant_poison::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Noxious Poison 143225.
class spell_he_noxious_poison : public SpellScript
{
    PrepareSpellScript(spell_he_noxious_poison);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (!caster)
            return;

        caster->RemoveAurasDueToSpell(SPELL_INSTANT_POISON);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_he_noxious_poison::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Mark of Anguish (Boss cast) 143822.
class spell_he_mark_of_anguish : public SpellScript
{
    PrepareSpellScript(spell_he_mark_of_anguish);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
                caster->AI()->AttackStart(target);
                caster->SetReactState(REACT_PASSIVE);
            }
        }
    }

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.size() >= 1) // doesn`t matter count - MaxAffect is 2(5)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_he_mark_of_anguish::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_he_mark_of_anguish::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Mark of Anguish (Main target spell) 143840.
class spell_he_mark_of_anguish_main : public AuraScript
{
    PrepareAuraScript(spell_he_mark_of_anguish_main);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (!caster || !target)
            return;

        if (caster->ToCreature())
            caster->ToCreature()->AI()->SetGUID(target->GetGUID());

        target->CastSpell(target, SPELL_DEBILITATION, true);

        // Make the Embodied Anguish attack this target.
        if (caster->ToCreature())
        {
            caster->ToCreature()->AI()->AttackStart(target);
            caster->ToCreature()->ClearUnitState(UNIT_STATE_CASTING);
        }
    }

    void OnTick(AuraEffect const* aurEff)
    {
        Unit* target = GetTarget();

        if (!target)
            return;

        target->CastSpell(target, SPELL_MARK_OF_ANGUISH_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_he_mark_of_anguish_main::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_he_mark_of_anguish_main::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Mark of Anguish (Transfer spell) 143842.
class spell_he_mark_of_anguish_transfer : public SpellScript
{
    PrepareSpellScript(spell_he_mark_of_anguish_transfer);

    void HandleDummyLaunchTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        if (Creature* anguish = caster->FindNearestCreature(NPC_EMBODIED_ANGUISH, 300.0f, true))
        {
            anguish->AddAura(SPELL_MARK_OF_ANGUISH_MAIN, target);
            anguish->CastSpell(anguish, SPELL_SHADOW_WEAKNESS_TRANSFER, true);
            anguish->AI()->AttackStart(target);
            anguish->SetReactState(REACT_PASSIVE);
        }

        // Achiev
        if (Creature* rook = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_ROOK_STONETOE) : ObjectGuid::Empty))
        {
            rook->AI()->SetData(TYPE_GO_LONG, (uint32)(caster->GetExactDist2d(target)));
            rook->AI()->SetGUID(caster->GetGUID());
        }

        caster->RemoveAurasDueToSpell(SPELL_MARK_OF_ANGUISH_MAIN);
    }

    void Register() override
    {
        OnEffectLaunchTarget += SpellEffectFn(spell_he_mark_of_anguish_transfer::HandleDummyLaunchTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Sha Shear (Damage spell) 143424.
class spell_sun_sha_shear_eff : public SpellScript
{
    PrepareSpellScript(spell_sun_sha_shear_eff);

    void CalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (AuraEffect const* shearAurEff = target->GetAuraEffect(SPELL_SHA_SHEAR, EFFECT_0))
                SetHitDamage(int32(GetHitDamage() + ((GetHitDamage() * shearAurEff->GetTickNumber()) / 4))); // Increases by 25% each tick.
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sun_sha_shear_eff::CalculateDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Sha Shear 143423
class spell_sun_sha_shear : public SpellScript
{
    PrepareSpellScript(spell_sun_sha_shear);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.size() >= 1) // doesn`t matter count - MaxAffect is 1
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sun_sha_shear::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Shadow Word : Bane (Boss cast) 143446.
class spell_sun_shadow_word_bane : public SpellScript
{
    PrepareSpellScript(spell_sun_shadow_word_bane);

    std::list<WorldObject*> m_targets;

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_SHADOW_WORD_BANE_HIT, true);
                caster->AI()->SetGUID(target->GetGUID());
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());

        uint8 targetCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetCount++;

            if (totalPlayersCount > 19)
                targetCount++;

            if (totalPlayersCount > 24)
                targetCount++;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount); // should be there for flex
        }

        if (targets.size() >= targetCount) // doesn`t matter count - MaxAffect is 2(5)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetCount++;

            if (totalPlayersCount > 19)
                targetCount++;

            if (totalPlayersCount > 24)
                targetCount++;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sun_shadow_word_bane::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sun_shadow_word_bane::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Shadow Word : Bane (Main target spell) 143434.
class spell_sun_shadow_word_bane_main : public AuraScript
{
    PrepareAuraScript(spell_sun_shadow_word_bane_main)

    void OnPeriodic(AuraEffect const* aurEff)
    {
        // not jump if not main target
        if (GetCaster() && GetCaster()->GetEntry() != NPC_SUN_TENDERHEART)
            return;

        if (Unit* owner = GetOwner()->ToUnit())
            // First 3 ticks, it jumps to another player not having the main spell.
            if (aurEff->GetTickNumber() <= 3)
                if (Creature* sun = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                    sun->CastSpell(owner, SPELL_SHADOW_WORD_BANE_SELECT, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sun_shadow_word_bane_main::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Shadow Word Bane Jump Selector 143438
class spell_shadow_word_bane_jump_selector : public SpellScript
{
    PrepareSpellScript(spell_shadow_word_bane_jump_selector);

    std::list<WorldObject*> m_targets;

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                // Prevent Hit Effect for main target cuz it`s only area selector
                if (target->HasAura(SPELL_SHADOW_WORD_BANE_HIT))
                    return;

                caster->CastSpell(target, SPELL_SHADOW_WORD_BANE_JUMP, true);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_SHADOW_WORD_BANE_HIT); });

        if (targets.size() >= 1) // doesn`t matter count - MaxAffect is 1
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_SHADOW_WORD_BANE_HIT); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_shadow_word_bane_jump_selector::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shadow_word_bane_jump_selector::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Shadow Word : Bane (Jump spell) 143443.
class spell_sun_shadow_word_bane_jump : public SpellScript
{
    PrepareSpellScript(spell_sun_shadow_word_bane_jump);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->AddAura(SPELL_SHADOW_WORD_BANE_HIT, target);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sun_shadow_word_bane_jump::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Calamity 143491.
class spell_sun_calamity : public SpellScript
{
    PrepareSpellScript(spell_sun_calamity);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            if (GetCaster()->GetMap()->IsHeroic() && GetCaster()->ToCreature())
            {
                int32 bp = 30 + (int32)((GetCaster()->ToCreature()->AI()->GetData(TYPE_CALAMITY_OVERPOWER) - 1) * 10);
                GetCaster()->CastCustomSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, &bp, NULL, NULL, true);
            }
            else
                GetCaster()->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);

            target->RemoveAurasDueToSpell(SPELL_SHADOW_WORD_BANE_HIT);
        }
    }

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (!caster->GetMap()->IsHeroic())
                return;

            int32 bp = 30 + (int32)(caster->AI()->GetData(TYPE_CALAMITY_OVERPOWER) * 10);
            GetCaster()->CastCustomSpell(caster, SPELL_CALAMITY_OVERPOWER, &bp, NULL, NULL, true);
            caster->AI()->SetData(TYPE_CALAMITY_OVERPOWER, caster->AI()->GetData(TYPE_CALAMITY_OVERPOWER) + 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sun_calamity::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(spell_sun_calamity::HandleAfterCast);
    }
};

// Dark Meditation 143546.
class spell_sun_dark_meditation : public AuraScript
{
    PrepareAuraScript(spell_sun_dark_meditation);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_DARK_MEDITATION_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sun_dark_meditation::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Corruption Shock (Boss cast) 143958.
class spell_embodied_gloom_corruption_shock : public SpellScript
{
    PrepareSpellScript(spell_embodied_gloom_corruption_shock);

    std::list<WorldObject*> m_targets;

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 24)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 2(5)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (totalPlayersCount > 24)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_embodied_gloom_corruption_shock::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_embodied_gloom_corruption_shock::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Corruption Chain (Boss cast) 145653.
class spell_embodied_gloom_corruption_chain : public SpellScript
{
    PrepareSpellScript(spell_embodied_gloom_corruption_chain);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (!caster || !target)
            return;

        caster->CastSpell(target, SPELL_CORRUPTION_CHAIN_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_embodied_gloom_corruption_chain::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Shadow Weakness (Transfer spell) 144081.
class spell_embodied_anguish_shadow_weakness_transfer : public SpellScript
{
    PrepareSpellScript(spell_embodied_anguish_shadow_weakness_transfer);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_embodied_anguish_shadow_weakness_transfer::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_embodied_anguish_shadow_weakness_transfer::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Shadowstep Selector 143050
class spell_he_shadowstep_selector : public SpellScript
{
    PrepareSpellScript(spell_he_shadowstep_selector);

    std::list<WorldObject*> m_targets;

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_SHADOWSTEP, true);

                // Apply poison effect by hit
                if (caster->GetMap()->IsHeroic())
                {
                    if (caster->HasAura(SPELL_INSTANT_POISON))
                        caster->CastSpell(target, SPELL_INSTANCE_POISON_EFF, true);
                    else if (caster->HasAura(SPELL_NOXIOUS_POISON))
                    {
                        caster->AI()->SetGUID(target->GetGUID());
                        caster->CastSpell(target, SPELL_NOXIOUS_POISON_MISSILE, true);
                    }
                }
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_HE_GARROTE); });

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 1(3)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_HE_GARROTE); });

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_he_shadowstep_selector::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_he_shadowstep_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Mark of Anguish Eff 144365
class spell_he_mark_of_anguish_eff : public SpellScript
{
    PrepareSpellScript(spell_he_mark_of_anguish_eff);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            float mod = target->GetAura(SPELL_SHADOW_WEAKNESS) ? ((target->GetAura(SPELL_SHADOW_WEAKNESS)->GetStackAmount() - 1) * sSpellMgr->GetSpellInfo(SPELL_SHADOW_WEAKNESS, GetCaster()->GetMap()->GetDifficulty())->Effects[EFFECT_0].BasePoints) / 100.0f : 0;
            int32 recalculatedEff = (int32)(GetHitDamage() + GetHitDamage() * mod);
            SetHitDamage(recalculatedEff);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_he_mark_of_anguish_eff::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Inferno Strike 143962
class spell_sorrow_inferno_strike : public SpellScript
{
    PrepareSpellScript(spell_sorrow_inferno_strike);

    uint8 targetsCount;
    uint32 damage;

    bool Load() override
    {
        targetsCount = 1;

        return true;
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage((GetHitDamage() * sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, GetCaster()->GetMap()->GetDifficulty())->Effects[0].BasePoints) / targetsCount); // why blizzard?
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targetsCount = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sorrow_inferno_strike::RecalculateDamage, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sorrow_inferno_strike::SelectTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Bond of the Golden Lotus 143497
class spell_bond_of_the_golden_lotus : public SpellScript
{
    PrepareSpellScript(spell_bond_of_the_golden_lotus);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_HEALED_BY_BOND_OF_LOTUS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bond_of_the_golden_lotus::HandleDummy, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
    }
};

// Noxious Poison Missle 143245
class spell_noxious_poison_missle : public SpellScript
{
    PrepareSpellScript(spell_noxious_poison_missle);

    void SelectTargets(SpellDestination& dest)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* noxTarget = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
            {
                Position newPos = { noxTarget->GetPositionX(), noxTarget->GetPositionY(), dest._position.GetPositionZ(), 0 };
                dest.Relocate(newPos);
            }
        }
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_noxious_poison_missle::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Instant Poison 143210
class spell_he_instance_poison : public AuraScript
{
    PrepareAuraScript(spell_he_instance_poison);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Unit* target = eventInfo.GetProcTarget())
                owner->CastSpell(target, SPELL_INSTANCE_POISON_EFF, true);
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_he_instance_poison::HandleOnProc);
    }
};

// Shared Torment 145655
class spell_fallen_protectors_shared_torment : public SpellScript
{
    PrepareSpellScript(spell_fallen_protectors_shared_torment);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_EMBODIED_SORROW && target->GetEntry() != NPC_EMBODIED_GLOOM && target->GetEntry() != NPC_EMBODIED_MISERY; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_fallen_protectors_shared_torment::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Debilitation 147383
class spell_fallen_protectors_debilitation : public AuraScript
{
    PrepareAuraScript(spell_fallen_protectors_debilitation);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetMap()->IsHeroic())
            SetDuration(4 * MINUTE * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_fallen_protectors_debilitation::OnApply, EFFECT_0, SPELL_AURA_MOD_RESISTANCE_PCT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Fixate 143292
class spell_he_fixate : public AuraScript
{
    PrepareAuraScript(spell_he_fixate);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (Unit* target = GetOwner()->ToUnit())
            {
                GetCaster()->ToCreature()->AI()->AttackStart(target);
                GetCaster()->ToCreature()->SetReactState(REACT_PASSIVE);
                GetCaster()->ToCreature()->ClearUnitState(UNIT_STATE_CASTING);
            }
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->SetReactState(REACT_AGGRESSIVE);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_he_fixate::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_he_fixate::OnRemove, EFFECT_3, SPELL_AURA_MOD_SPEED_NOT_STACK, AURA_EFFECT_HANDLE_REAL);
    }
};

// Corruptiong Chain Eff 145631
class spell_rook_corruptiong_chain_eff : public SpellScript
{
    PrepareSpellScript(spell_rook_corruptiong_chain_eff);

    void HandleDummy(SpellEffIndex effIndex)
    {
        if (GetHitUnit() && GetHitUnit()->GetTypeId() != TYPEID_PLAYER)
            SetHitDamage(0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_rook_corruptiong_chain_eff::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Vengeful Strikes 144397
class spell_rook_vengeful_strikes : public SpellScript
{
    PrepareSpellScript(spell_rook_vengeful_strikes);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_rook_vengeful_strikes::FilterTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_104);
    }
};

// Corruption Kick 143007
class spell_rook_corruption_kick : public AuraScript
{
    PrepareAuraScript(spell_rook_corruption_kick);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_REMOVE_CLASH);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_rook_corruption_kick::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Vengeful Strikes 144396
class spell_rook_vengeful_strikes_periodic : public AuraScript
{
    PrepareAuraScript(spell_rook_vengeful_strikes_periodic);

    bool Load() override
    {
        tickCount = 0;
        return true;
    }

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            if (++tickCount % 2 == 0)
                caster->CastSpell(caster, SPELL_VENGEFUL_STRIKES_VISUAL, true); // just additional visual effect each 1s
        }
    }

    private:
        uint8 tickCount;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_rook_vengeful_strikes_periodic::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 1032. Summoned by 143546 - Dark Meditation
class sat_dark_meditation : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_DARK_MEDITATION_DMG_RED, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_DARK_MEDITATION_DMG_RED);
    }
};

// 1013. Summoned by 143235 - Noxious Poison
class sat_noxious_poison : public IAreaTriggerAura
{
    void OnInit() override
    {
        if (GetAreaTrigger() && GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_FALLEN_PROTECTORS) != IN_PROGRESS)
            GetAreaTrigger()->SetDuration(0);
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_NOXIOUS_POISON_AURA, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_NOXIOUS_POISON_AURA);
    }
};

// 1053. Summoned by 143960 - Defiled Ground
class sat_rook_defiled_ground : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetAreaTrigger() && GetAreaTrigger()->HasInArc(M_PI / 3, triggering->ToPlayer());
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_DEFILED_GROUND_AURA, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_DEFILED_GROUND_AURA);
    }
};

void AddSC_fallen_protectors()
{
    new boss_rook_stonetoe();
    new boss_he_softfoot();
    new boss_sun_tenderheart();
    new creature_script<npc_embodied_misery>("npc_embodied_misery");
    new creature_script<npc_embodied_sorrow>("npc_embodied_sorrow");
    new creature_script<npc_embodied_gloom>("npc_embodied_gloom");
    new creature_script<npc_embodied_anguish>("npc_embodied_anguish");
    new creature_script<npc_embodied_despair>("npc_embodied_despair");
    new creature_script<npc_embodied_desperation>("npc_embodied_desperation");
    new creature_script<npc_despair_spawn>("npc_despair_spawn");
    new creature_script<npc_desperation_spawn>("npc_desperation_spawn");
    new creature_script<npc_golden_lotus_trigger>("npc_golden_lotus_trigger");

    new spell_script<spell_rook_corrupted_brew>("spell_rook_corrupted_brew");
    new spell_script<spell_rook_clash>("spell_rook_clash");
    new spell_script<spell_he_gouge>("spell_he_gouge");
    new spell_script<spell_he_instant_poison>("spell_he_instant_poison");
    new spell_script<spell_he_noxious_poison>("spell_he_noxious_poison");
    new spell_script<spell_he_mark_of_anguish>("spell_he_mark_of_anguish");
    new aura_script<spell_he_mark_of_anguish_main>("spell_he_mark_of_anguish_main");
    new spell_script<spell_he_mark_of_anguish_transfer>("spell_he_mark_of_anguish_transfer");
    new spell_script<spell_sun_sha_shear_eff>("spell_sun_sha_shear_eff");
    new spell_script<spell_sun_sha_shear>("spell_sun_sha_shear");
    new spell_script<spell_sun_shadow_word_bane>("spell_sun_shadow_word_bane");
    new aura_script<spell_sun_shadow_word_bane_main>("spell_sun_shadow_word_bane_main");
    new spell_script<spell_sun_shadow_word_bane_jump>("spell_sun_shadow_word_bane_jump");
    new spell_script<spell_shadow_word_bane_jump_selector>("spell_shadow_word_bane_jump_selector");
    new spell_script<spell_sun_calamity>("spell_sun_calamity");
    new aura_script<spell_sun_dark_meditation>("spell_sun_dark_meditation");
    new spell_script<spell_embodied_gloom_corruption_shock>("spell_embodied_gloom_corruption_shock");
    new spell_script<spell_embodied_gloom_corruption_chain>("spell_embodied_gloom_corruption_chain");
    new spell_script<spell_embodied_anguish_shadow_weakness_transfer>("spell_embodied_anguish_shadow_weakness_transfer");
    new spell_script<spell_he_shadowstep_selector>("spell_he_shadowstep_selector");
    new spell_script<spell_he_mark_of_anguish_eff>("spell_he_mark_of_anguish_eff");
    new spell_script<spell_sorrow_inferno_strike>("spell_sorrow_inferno_strike");
    new spell_script<spell_bond_of_the_golden_lotus>("spell_bond_of_the_golden_lotus");
    new spell_script<spell_noxious_poison_missle>("spell_noxious_poison_missle");
    new aura_script<spell_he_instance_poison>("spell_he_instance_poison");
    new spell_script<spell_fallen_protectors_shared_torment>("spell_fallen_protectors_shared_torment");
    new aura_script<spell_fallen_protectors_debilitation>("spell_fallen_protectors_debilitation");
    new aura_script<spell_he_fixate>("spell_he_fixate");
    new spell_script<spell_rook_corruptiong_chain_eff>("spell_rook_corruptiong_chain_eff");
    new spell_script<spell_rook_vengeful_strikes>("spell_rook_vengeful_strikes");
    new aura_script<spell_rook_corruption_kick>("spell_rook_corruption_kick");
    new aura_script<spell_rook_vengeful_strikes_periodic>("spell_rook_vengeful_strikes_periodic");
    new atrigger_script<sat_dark_meditation>("sat_dark_meditation");
    new atrigger_script<sat_noxious_poison>("sat_noxious_poison");
    new atrigger_script<sat_rook_defiled_ground>("sat_rook_defiled_ground");
}
