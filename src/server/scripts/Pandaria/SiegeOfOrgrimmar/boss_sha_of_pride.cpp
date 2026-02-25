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
#include "MoveSplineInit.h"
#include "siege_of_orgrimmar.h"
#include "CreatureAI.h"

enum Yells
{
    // Norushen.
    SAY_NORUSHEN_INTRO,
    SAY_NORUSHEN_INTRO_1           = 1, // The corruption is amplifying. The fragments must be purged before it becomes too great.
    SAY_NORUSHEN_INTRO_2           = 2, // It did not matter. It comes. Steel your hearts and prepare your souls.
    SAY_NORUSHEN_DEATH             = 3, // You... must... contain... it...
    SAY_NORUSHEN_GIFT_OF_THE_TIT   = 3, // 0 - Be humble.; 1 - Free yourself of arrogance. .

    // Lorewalker Cho Intro.
    SAY_LOREWALKER_INTRO_1         = 0, // Taran Zhu!
    SAY_LOREWALKER_INTRO_2         = 1, // Don't speak, friend, I will take you to the healers.
    SAY_LOREWALKER_INTRO_3         = 2, // I will find help! Try to hold off the sha for as long as you can!
    SAY_LOREWALKER_OUTRO_1         = 3, // Heroes! You're alive. I brought help.

    // Taran Zhu Intro.
    SAY_TARAN_ZHU_INTRO_1          = 0, // *Cough cough* Nyyyhunnnggggg...
    SAY_TARAN_ZHU_INTRO_2          = 1, // Cho... the outsiders... THEY did this... we should never have let them in...

    // Sha of Pride.
    SAY_SHA_OF_PRIDE_INTRO_1       = 0, // Come, face me. Give in to your pride. Show me your "Greatness".
    SAY_SHA_OF_PRIDE_AGGRO         = 1, // So foolish...
    SAY_SHA_OF_PRIDE_UNLEASHED     = 2, // You've let your pride cloud your vision, Titan puppet! You can never contain ME!
    SAY_SHA_OF_PRIDE_KILL          = 3, // 0 - That one is WEAK.; 1 - That one is unworthy of your group.; 2 - Blame your companions! .
    SAY_SHA_OF_PRIDE_SELF_REFLECT  = 4, // 0 - You are better than your companions!; 1 - Your arrogance compels you... .
    SAY_SHA_OF_PRIDE_SWEL_PRIDE    = 5, // 0 - Succumb to your pride!; 1 - You should bow to no king or warchief. .
    SAY_SHA_OF_PRIDE_CORR_PRISON   = 6, // Your arrogance feeds me!
    SAY_SHA_OF_PRIDE_SWELLING_ANN  = 7,

    // Lady Jaina Proudmoore.
    SAY_JAINA_OUTRO_1              = 0, // So Hellscream's arrogance unleashed the last of the sha. I am not surprised.
    SAY_JAINA_OUTRO_2              = 1, // Gorehowl.
    SAY_JAINA_OUTRO_3              = 2, // News to no one, Regent Lord. King Wrynn's fleet is converging on Orgrimmar as we speak.
    SAY_JAINA_OUTRO_4              = 3, // I'm warning you, Lor'themar. The Alliance is besieging the city and we will destroy Hellscream. Your people had best stay out of our way.
    SAY_JAINA_OUTRO_5              = 4, // (Alliance) Lady Jaina Proudmoore says: Come heroes, through the portal! The Siege of Orgrimmar begins!

    // Lor'themar Theron.
    SAY_THERON_OUTRO_1             = 0, // Look here! He left his weapon behind.
    SAY_THERON_OUTRO_2             = 1, // This means he's completely unhinged.
    SAY_THERON_OUTRO_3             = 2, // Likewise. Sylvanas and I have both sent ships to support Vol'jin's revolution.
    SAY_THERON_OUTRO_4             = 3, // It is always a privilege to see you, Lady Proudmoore.
    SAY_THERON_OUTRO_5             = 4,  // (Horde) Lor'themar Theron says: Champions, come with me. It's time to settle grievances with our... "Warchief."

    SAY_LOREWALKER_OUTRO           = 18,
};

enum Spells
{
    // Norushen.
    SPELL_GIFT_OF_THE_TITANS_A     = 146595, // Trigger for all checks.
    SPELL_GIFT_OF_THE_TITANS_HEAL  = 144359, // Triggered by the aura.
    SPELL_GIFT_OF_THE_TITANS_DPS   = 146594, // Triggered by the aura. Same as 1.
    SPELL_GIFT_OF_THE_TITANS_D     = 144363, // Dummy for 8 yard checktriggered by 1 and 2 each sec.

    SPELL_POWER_OF_THE_TITANS      = 144364, // Buff for dmg + healing increase and threat decrease.

    // Final Gift
    SPELL_FINAL_GIFT               = 144854, // Sets Pride to 0 in 300y.

    // Sha of Pride.
    SPELL_BERSERK                  = 47008,
    SPELL_EMERGE_SUBMERGE          = 139832,
    SPELL_NON_REGENERATE_POWER     = 72242,
    SPELL_SHA_VORTEX_INTRO         = 146024, // Intro Aura - Visual + Trigger.
    SPELL_SHA_VORTEX_INTRO_SUMMON  = 149220, // Intro Aura - Dummy to summon a mob every 4 seconds.
    SPELL_SHA_INTRO_SPAWN_KB       = 149213, // Intro Knockback when Sha emerges.
    SPELL_GOREHOWL_VISUAL          = 146058, // Gorehowl in ground visual.

    // Pride
    SPELL_PRIDE_BAR                = 144343, // Enables Pride power.

    SPELL_REACHING_ATTACK          = 144774, // victim not in range
    SPELL_MARK_OF_ARROGANCE        = 144351,

    // Corrupted Prison
    SPELL_IMPRISON                 = 144563, // Cast time + Dummy (Effect 0). Boss main spell.
    SPELL_CORRUPTED_PRISON_1       = 144574, // First  prison victim. Teleport (Effect 0), Stun, Periodic dmg, Pacify. Needs script for Pride addition.
    SPELL_CORRUPTED_PRISON_2       = 144636, // Second prison victim. Teleport (Effect 0), Stun, Periodic dmg, Pacify. Needs script for Pride addition.
    SPELL_CORRUPTED_PRISON_3       = 144683, // Third  prison victim (25 man). Teleport (Effect 0), Stun, Periodic dmg, Pacify. Needs script for Pride addition.
    SPELL_CORRUPTED_PRISON_4       = 144684, // Fourth prison victim (25 man). Teleport (Effect 0), Stun, Periodic dmg, Pacify. Needs script for Pride addition.
    SPELL_CORRUPTED_PRISON_ACTIV   = 144615, // Activation knockback and damage (Effect 1). Needs script for Pride addition.

    SPELL_WOUNDED_PRIDE            = 144358, // Aura, Dummy Effect 0.

    // Self Reflection
    SPELL_SELF_REFLECTION          = 144800, // Periodic Dummy.
    SPELL_SELF_REFLECTION_VISUAL   = 144784, // Spawn Effect on NPC.
    SPELL_SELF_REFLECTION_EXPLODE  = 144788, // Damage. Needs script for Pride addition.

    // Unleashed
    SPELL_UNLEASHED                = 144832, // Triggers SPELL_UNLEASHED_DMG every 10 seconds.
    SPELL_UNLEASHED_DMG            = 144836, // Damage. Needs script for Pride addition.

    SPELL_SWELLING_PRIDE           = 144400, // Damage. Needs script for Pride addition.

    //============================= Swelling Pride effects - Start =============================//

    SPELL_BURSTING_PRIDE       = 144910, // Missile. Triggers SPELL_BURSTING_PRIDE.
    SPELL_BURSTING_PRIDE_EFF   = 144911, // Damage. Needs script for Pride addition.

    // Projection
    SPELL_PROJECTION_DUMMY         = 146822, // 6 second player aura.
    SPELL_PROJECTION_EXPLODE       = 145320, // Damage. Needs script for Pride addition.
    SPELL_PROJECTION_MISSLE        = 145066,
    SPELL_PROJECTION_NOT_EXPLODE   = 145526,

    // Aura of Pride
    SPELL_AURA_OF_PRIDE            = 146817, // Triggers SPELL_AURA_OF_PRIDE_DMG each sec.
    SPELL_AURA_OF_PRIDE_DMG        = 146818, // Damage. Needs script for Pride addition.

    // Overcome
    SPELL_OVERCOME                 = 144843, // Dmg / Healing increase, Morph.
    SPELL_OVERCOME_CHARMED         = 144863, // MC, max health increase.

    //============================= Swelling Pride effects - End   =============================//

    // Manifestation of Pride.
    SPELL_MOCKING_BLAST            = 144379, // Damage. Needs script for Pride addition.
    SPELL_LAST_WORD                = 144370, // 500y Dummy.
    SPELL_MANIFESTATION_SPAWN      = 144778,

    /*Heroic*/
    // Banishment
    SPELL_BANISHMENT               = 146823, // Main spell, Cast time, Dummy (Effect 0) for SPELL_BANISHMENT_AURA, Send Event (Effect 1).
    SPELL_BANISHMENT_AURA          = 145215, // Screen Effect, Phase, Move Forward force, Periodic Damage, Trigger spell (Effect 5).
    SPELL_BANISHMENT_AT            = 145217, // Creates 9 AreaTriggers.
    SPELL_BANISHMENT_SIZE          = 145684, // 75% size increase.
    SPELL_BANISHMENT_STUN          = 146623, // Stun.
    SPELL_BANISHMENT_TELEPORT      = 148705, // Teleport visual and effect.

    // Unstable Corruption
    SPELL_RIFT_OF_CORRUPTION_DUMMY = 147183, // Effect 0 - Periodic Dummy for Spawn Effect. The Sha's energy tears open Rifts of Corruption every 8 sec. 
    SPELL_RIFT_OF_CORRUPTION_VIS   = 147186, // NPC visual. Also 147210 & 147211.
    SPELL_RIFT_OF_CORRUPTION_SPAWN = 147199, // Spawn effect.
    SPELL_RIFT_OF_CORRUPTION_AT    = 147205,
    SPELL_BOLT_OF_CORRUPTION_DUMMY = 147389, // Effect 0 - Periodic Trigger. Each rift launches a bolt of corruption at a random player's location every 5 sec.
    SPELL_BOLT_OF_CORRUPTION_MIS   = 147391, // Missile, triggers SPELL_BOLT_OF_CORRUPTION. Triggered by above.
    SPELL_BOLT_OF_CORRUPTION       = 147198, // Damage.
    SPELL_COLLAPSING_RIFT          = 147388, // Players can close the rifts, causing them to explode, 250000 Shadow damage to all players within 8 yards.
    SPELL_WEAKENED_RESOLVE         = 147207, // Dummy (Effect 0), cannot "close" (kill) a rift for 1 minute.
};

enum Events
{
    // Intro - Controller: Norushen.
    EVENT_INTRO_1                  = 1,
    EVENT_INTRO_2,
    EVENT_INTRO_3,
    EVENT_INTRO_4,
    EVENT_INTRO_5,
    EVENT_INTRO_6,
    EVENT_INTRO_7,
    EVENT_INTRO_8,
    EVENT_INTRO_9,

    // Outro - Controller: Lady Jaina Proudmoore.
    EVENT_OUTRO_1,
    EVENT_OUTRO_2,
    EVENT_OUTRO_3,
    EVENT_OUTRO_4,
    EVENT_OUTRO_5,
    EVENT_OUTRO_6,
    EVENT_OUTRO_7,
    EVENT_OUTRO_8,
    EVENT_OUTRO_9,
    EVENT_OUTRO_10,
    EVENT_OUTRO_11,

    // Norushen.
    EVENT_REACH_SHA_ROOM,

    // Sha of Pride.
    EVENT_IMPRISON,
    EVENT_MARK_OF_ARROGANCE,
    EVENT_SELF_REFLECTION,
    EVENT_MANIFESTATION,
    EVENT_WOUNDED_PRIDE,
    EVENT_GAIN_ENERGY,
    EVENT_BERSERK,
    EVENT_SWELLING_PRIDE,
    EVENT_UNLEASHED,
    EVENT_MOCKING_BLAST,
    EVENT_GIFT_OF_THE_TITANS,
    EVENT_REACHING_ATTACK,
    EVENT_CHECK_EVADE,
    EVENT_SPAWN_RIFT,
};

enum Actions
{
    // Intro - Controller: Norushen.
    ACTION_START_SHA_PRIDE_INTRO  = 1,
    ACTION_CALCULATE_PRISON_VALUE,

    // Outro - Controller: Lady Jaina Proudmoore.
    ACTION_START_SHA_PRIDE_OUTRO,

    // Misc
    ACTION_SWELLING_PRIDE_EXECUTE,
    ACTION_UNLEASHED,
};

// Corrupted Prisons count, depending on difficulty.
enum PrisonsCount
{
    SP_PRISONS_COUNT_10MAN         = 2,
    SP_PRISONS_COUNT_25MAN         = 4
};

// Corrupted Prisons id.
enum Prisons
{
    PRISON_NORTH                   = 1,
    PRISON_SOUTH,
    PRISON_WEST,
    PRISON_EAST
};

// Tharan Zhu spawn position.
Position const tharanZhuSpawnPos   = { 780.412f, 1017.587f, 356.062f };

// Sha of Pride spawn position.
Position const shaPrideSpawnPos    = { 749.194f, 1112.641f, 357.314f };

// Norushen movement positions.
Position const NorushenMove[5]     =
{
    { 768.722f, 1015.379f, 356.073f }, // Stair before door.
    { 767.805f, 1019.000f, 357.101f }, // Up the stair.
    { 761.164f, 1049.889f, 357.151f }, // Front of entrance.
    { 760.357f, 1051.784f, 356.072f }, // Entrance.
    { 759.110f, 1060.828f, 356.072f }, // Encounter.
};

const std::map<uint32, std::array<uint32, 5>> invShaPrisonsType =
{
    { SPELL_CORRUPTED_PRISON_2, { GO_S_PRISON_FLOOR, GO_SOUTH_PRISON_A, GO_SOUTH_PRISON_B, GO_SOUTH_PRISON_C, 0  } }, // S-E
    { SPELL_CORRUPTED_PRISON_1, { GO_W_PRISON_FLOOR, GO_WEST_PRISON_A,  GO_WEST_PRISON_B,  GO_WEST_PRISON_C,  1  } }, // S-W
    { SPELL_CORRUPTED_PRISON_4, { GO_N_PRISON_FLOOR, GO_NORTH_PRISON_A, GO_NORTH_PRISON_B, GO_NORTH_PRISON_C, 2, } }, // N-W
    { SPELL_CORRUPTED_PRISON_3, { GO_E_PRISON_FLOOR, GO_EAST_PRISON_A,  GO_EAST_PRISON_B,  GO_EAST_PRISON_C,  3, } }, // N-E
};

const std::map<uint32, float> invShaPrisonsAngleType =
{
    { GO_SOUTH_PRISON_A, 5.69f },
    { GO_SOUTH_PRISON_B, 3.58f },
    { GO_SOUTH_PRISON_C, 1.50f },

    { GO_WEST_PRISON_A, 4.11f },
    { GO_WEST_PRISON_B, 6.20f },
    { GO_WEST_PRISON_C, 2.03f },

    { GO_NORTH_PRISON_A, 2.55f },
    { GO_NORTH_PRISON_B, 4.62f },
    { GO_NORTH_PRISON_C, 0.42f },

    { GO_EAST_PRISON_A, 5.19f },
    { GO_EAST_PRISON_B, 0.99f },
    { GO_EAST_PRISON_C, 3.04f },
};

// Norushen 71967.
struct boss_norushen_pride : public ScriptedAI
{
    boss_norushen_pride(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->GetEntry() == NPC_SHA_OF_PRIDE)
            return;

        Talk(SAY_NORUSHEN_INTRO);

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            me->GetMotionMaster()->MovePoint(0, norushenShaRoom);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                Talk(SAY_NORUSHEN_INTRO_1);
            });
        });
    }

    void Reset() override
    {
        events.Reset();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
            events.ScheduleEvent(EVENT_GIFT_OF_THE_TITANS, 8 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        events.Reset();
        Talk(SAY_NORUSHEN_DEATH);
        DoCast(me, SPELL_FINAL_GIFT, true);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_GIFT_OF_THE_TITANS)
            {
                DoCast(me, SPELL_GIFT_OF_THE_TITANS_A);
                events.ScheduleEvent(EVENT_GIFT_OF_THE_TITANS, 25.5 * IN_MILLISECONDS);
            }
            break;
        }
    }
};

struct lorewalkerAI : public CreatureAI
{
    lorewalkerAI(Creature* creature) : CreatureAI(creature) { }

    TaskScheduler scheduler;
    float x, y, z;

    void Reset() override { }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            z = me->GetPositionZ();

            uint32 delay = 1000;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(SAY_LOREWALKER_OUTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
            {
                me->SetWalk(true);
                GetPositionWithDistInOrientation(me, 50.0f, me->GetOrientation(), x, y);
                me->UpdateAllowedPositionZ(x, y, z);
                me->GetMotionMaster()->MovePoint(0, x, y, z);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Sha of Pride 71734.
class boss_sha_of_pride : public CreatureScript
{
    public:
        boss_sha_of_pride() : CreatureScript("boss_sha_of_pride") { }

        struct boss_sha_of_prideAI : public BossAI
        {
            boss_sha_of_prideAI(Creature* creature) : BossAI(creature, DATA_SHA_OF_PRIDE) 
            {
                me->setActive(true);
                SetCombatMovement(false);
                hasNorushenSpawned = false;
            }

            EventMap berserkerEvents;
            std::map<uint32, uint32> invPrisonSpellsType;
            uint32 tempPrisonData;
            uint32 energyGainTick;
            bool hasTriggered;
            bool hasNorushenSpawned;

            void Reset() override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_SHALLOW_YOUR_PRIDE, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_SHALLOW_YOUR_PRIDE_ADDITIONAL, 1);
                me->SetReactState(REACT_DEFENSIVE);

                _Reset();
                events.Reset();
                berserkerEvents.Reset();

                invPrisonSpellsType.clear();
                tempPrisonData = 0;

                invPrisonSpellsType.insert(std::pair<uint32, uint32>(SPELL_CORRUPTED_PRISON_1, 0));
                invPrisonSpellsType.insert(std::pair<uint32, uint32>(SPELL_CORRUPTED_PRISON_2, 0));
                invPrisonSpellsType.insert(std::pair<uint32, uint32>(SPELL_CORRUPTED_PRISON_3, 0));
                invPrisonSpellsType.insert(std::pair<uint32, uint32>(SPELL_CORRUPTED_PRISON_4, 0));

                DoCast(me, SPELL_NON_REGENERATE_POWER, true);
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                energyGainTick = 0;
                hasTriggered = false;

                if (instance && instance->GetData(DATA_SHA_OF_PRIDE_PRE_EVENT) != DONE)
                {
                    DoCast(me, SPELL_EMERGE_SUBMERGE, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                    // Missles
                    DoCast(me, SPELL_SHA_VORTEX_INTRO, true);

                    scheduler
                        .Schedule(Seconds(1), [this](TaskContext context)
                    {
                        if (!me->FindNearestCreature(NPC_LINGERING_CORRUPTION, 100.0f, true))
                        {
                            if (instance)
                                instance->SetData(DATA_SHA_OF_PRIDE_PRE_EVENT, DONE);

                            me->RemoveAurasDueToSpell(SPELL_SHA_VORTEX_INTRO);
                            me->RemoveAurasDueToSpell(SPELL_EMERGE_SUBMERGE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                            if (Creature* norushen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_NORUSHEN_2)))
                                norushen->AI()->Talk(SAY_NORUSHEN_INTRO_2);
                            else if (Creature* norushen = me->SummonCreature(NPC_NORUSHEN_2, norushenShaRoom, TEMPSUMMON_MANUAL_DESPAWN))// in case - norushen defeat - server crash
                                norushen->AI()->Talk(SAY_NORUSHEN_INTRO_2);

                            hasNorushenSpawned = true;

                            scheduler
                                .Schedule(Seconds(4), [this](TaskContext context)
                            {
                                Talk(SAY_SHA_OF_PRIDE_INTRO_1);
                            });
                            return;
                        }

                        context.Repeat(Seconds(1));
                    });
                }
                else if (!hasNorushenSpawned) // in case if prev event done
                {
                    hasNorushenSpawned = true;
                    me->SummonCreature(NPC_NORUSHEN_2, norushenShaRoom, TEMPSUMMON_MANUAL_DESPAWN);
                }

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    if (instance && instance->GetData(DATA_SHA_OF_PRIDE_PRE_EVENT) == DONE)
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_SHA_PRIDE_INTRO:
                        events.ScheduleEvent(EVENT_INTRO_1, 100);
                        break;
                    case ACTION_CALCULATE_PRISON_VALUE:
                        SetEmptyPrisonSpellId(); // no way to call in GetData() 
                        break;
                    case ACTION_SWELLING_PRIDE_EXECUTE:
                        HandleExecuteSwellingPride();
                        break;
                    case ACTION_UNLEASHED:
                        me->SetPower(POWER_ENERGY, 0);
                        events.CancelEvent(EVENT_IMPRISON);
                        events.CancelEvent(EVENT_SELF_REFLECTION);
                        events.CancelEvent(EVENT_MANIFESTATION);
                        
                        events.ScheduleEvent(EVENT_IMPRISON, 42 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SELF_REFLECTION, 16 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_MANIFESTATION, 51 * IN_MILLISECONDS);
                        berserkerEvents.ScheduleEvent(EVENT_GAIN_ENERGY, 1 * IN_MILLISECONDS);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_PRISON_ID)
                    return tempPrisonData;

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_PRISON_ID)
                    invPrisonSpellsType.find(data)->second = 0;
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(30) && !hasTriggered)
                {
                    Talk(SAY_SHA_OF_PRIDE_UNLEASHED);
                    hasTriggered = true;
                    berserkerEvents.CancelEvent(EVENT_GAIN_ENERGY);
                    events.ScheduleEvent(EVENT_UNLEASHED, 0.5 * IN_MILLISECONDS);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SHA_OF_PRIDE_KILL);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                Talk(SAY_SHA_OF_PRIDE_AGGRO);

                if (instance)
                {
                    instance->DoAddAuraOnPlayers(SPELL_PRIDE_BAR);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add.
                }

                events.ScheduleEvent(EVENT_MARK_OF_ARROGANCE, 33 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WOUNDED_PRIDE, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_REACHING_ATTACK, 2 * IN_MILLISECONDS); // range

                berserkerEvents.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
                berserkerEvents.ScheduleEvent(EVENT_CHECK_EVADE, 1 * IN_MILLISECONDS);
                HandleExecuteSwellingPride();

                if (Creature* norushen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_NORUSHEN_2) : ObjectGuid::Empty))
                    norushen->AI()->DoAction(ACTION_START_INTRO);

                if (IsHeroic())
                    berserkerEvents.ScheduleEvent(EVENT_SPAWN_RIFT, 10 * IN_MILLISECONDS);

            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_NORUSHEN_2)
                    return;

                summons.Summon(summon);
                summon->setActive(true);

                summon->SetInCombatWithZone();
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_SHA_OF_PRIDE, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PRIDE_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_1);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_3);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_4);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WOUNDED_PRIDE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ARROGANCE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_OF_PRIDE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCOME);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCOME_CHARMED);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* norushen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_NORUSHEN_2)))
                    {
                        norushen->AI()->Reset();
                        norushen->Respawn();
                    }
                }

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                summons.DespawnAll();
                berserkerEvents.Reset();
                HandleDeactivateTraps();
                _DespawnAtEvade();
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAllAreasTrigger();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me); // Remove.
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PRIDE_BAR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_1);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_3);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CORRUPTED_PRISON_4);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WOUNDED_PRIDE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_ARROGANCE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AURA_OF_PRIDE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCOME);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCOME_CHARMED);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SetData(DATA_GALAKRAS_PRE_EVENT, IN_PROGRESS);
                    instance->SetData(DATA_SHA_OF_PRIDE_PAST_EVENT, DONE);
                }

                berserkerEvents.Reset();
                HandleDeactivateTraps();

                if (!instance->GetData(DATA_LFR))
                    if (GameObject* go = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(IsHeroic() ? GO_VAULT_OF_FORBIDDEN_TREASURES_HC : GO_VAULT_OF_FORBIDDEN_TREASURES) : ObjectGuid::Empty))
                        instance->DoRespawnGameObject(go->GetGUID(), 7 * DAY);

                _JustDied();

                // it`s also found in MSV, but already have script (yes, it should be in msv too!)
                if (Creature* lorewalkerCho = me->SummonCreature(NPC_LOREWALKER_CHO_SHA_ROOM, norushenShaRoom, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    lorewalkerCho->SetAI(new lorewalkerAI(lorewalkerCho));
                    lorewalkerCho->AI()->DoAction(ACTION_START_INTRO);
                }

                me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkerEvents.Update(diff);

                while (uint32 eventId = berserkerEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_SPAWN_RIFT:
                            DoCast(me, SPELL_RIFT_OF_CORRUPTION_DUMMY, true);
                            break;
                        case EVENT_CHECK_EVADE:
                            if (instance && instance->IsWipe(68.0f, me))
                            {
                                EnterEvadeMode();
                                break;
                            }

                            berserkerEvents.ScheduleEvent(EVENT_CHECK_EVADE, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_GAIN_ENERGY:
                            if (energyGainTick > 2)
                                energyGainTick = 0;

                            uint32 gain = ++energyGainTick > 2 ? 2 : 1; // each third tick it gain 2 energy, in another - 1

                            if (me->GetPower(POWER_ENERGY) + gain > 99)
                            {
                                me->SetPower(POWER_ENERGY, 100);
                                events.ScheduleEvent(EVENT_SWELLING_PRIDE, 0.5 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) + gain);

                            berserkerEvents.ScheduleEvent(EVENT_GAIN_ENERGY, 1 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_IMPRISON:
                            Talk(SAY_SHA_OF_PRIDE_CORR_PRISON);
                            DoCast(me, SPELL_IMPRISON);
                            break;
                        case EVENT_MARK_OF_ARROGANCE:
                            DoCast(me, SPELL_MARK_OF_ARROGANCE);
                            events.ScheduleEvent(EVENT_MARK_OF_ARROGANCE, 20.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_SELF_REFLECTION:
                            Talk(SAY_SHA_OF_PRIDE_SELF_REFLECT);
                            DoCast(me, SPELL_SELF_REFLECTION);
                            break;
                        case EVENT_MANIFESTATION:
                        {
                            if (Is25ManRaid() || instance && instance->GetData(DATA_FLEX) && instance->instance->GetPlayersCountExceptGMs() > 17) // left-right side always
                            {
                                for (uint8 i = 1; i < 3; i++)
                                    me->SummonCreature(NPC_MANIFESTATION_OF_PRIDE, manifestationSpawnPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                            }
                            else // back side always
                                me->SummonCreature(NPC_MANIFESTATION_OF_PRIDE, manifestationSpawnPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                        }
                        case EVENT_WOUNDED_PRIDE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_WOUNDED_PRIDE);

                            events.ScheduleEvent(EVENT_WOUNDED_PRIDE, urand(16 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                            break;
                        case EVENT_SWELLING_PRIDE:
                            Talk(SAY_SHA_OF_PRIDE_SWEL_PRIDE);
                            Talk(SAY_SHA_OF_PRIDE_SWELLING_ANN);
                            DoCast(me, SPELL_SWELLING_PRIDE);
                            break;
                        case EVENT_UNLEASHED:
                            if (me->HasAura(SPELL_SELF_REFLECTION))
                            {
                                events.RescheduleEvent(EVENT_UNLEASHED, 1 * IN_MILLISECONDS); // issue with channeling spells
                                break;
                            }
                            DoCast(me, SPELL_UNLEASHED);
                            break;
                        case EVENT_REACHING_ATTACK:
                            if (me->GetVictim() && me->GetVictim()->GetExactDist2d(me) > 24.7f)
                                DoCast(me->GetVictim(), SPELL_REACHING_ATTACK);

                            events.ScheduleEvent(EVENT_REACHING_ATTACK, 2 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                void SetEmptyPrisonSpellId()
                {
                    // In 10 ppl mode always using I and III prisons
                    std::vector<uint32> choisePrison = { SPELL_CORRUPTED_PRISON_1, SPELL_CORRUPTED_PRISON_3};

                    // Whole Prisons Spell Id
                    if (Is25ManRaid() || instance && instance->GetData(DATA_FLEX))
                    {
                        choisePrison.push_back(SPELL_CORRUPTED_PRISON_2);
                        choisePrison.push_back(SPELL_CORRUPTED_PRISON_4);
                    }

                    for (auto&& itr : choisePrison)
                        if (invPrisonSpellsType.find(itr)->second != 0 && std::find(choisePrison.begin(), choisePrison.end(), itr) != choisePrison.end())
                            choisePrison.erase(std::find(choisePrison.begin(), choisePrison.end(), itr));

                    if (choisePrison.empty())
                    {
                        tempPrisonData = 0;
                        return;
                    }

                    uint32 prisonId = Trinity::Containers::SelectRandomContainerElement(choisePrison);
                    invPrisonSpellsType.find(prisonId)->second++;

                    tempPrisonData = prisonId;
                }

                void HandleDeactivateTraps()
                {
                    for (auto&& itr : invShaPrisonsAngleType)
                        if (GameObject* coneActive = me->FindNearestGameObject(itr.second, 100.0f))
                            coneActive->SetGoState(GO_STATE_ACTIVE);

                    for (auto&& itr : invShaPrisonsType)
                        if (GameObject* trap = me->FindNearestGameObject(itr.first, 100.0f))
                            trap->SetGoState(GO_STATE_READY);
                }

                void HandleExecuteSwellingPride()
                {
                    events.ScheduleEvent(EVENT_IMPRISON, 51 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SELF_REFLECTION, 25 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_MANIFESTATION, 60 * IN_MILLISECONDS);
                    berserkerEvents.ScheduleEvent(EVENT_GAIN_ENERGY, 1 * IN_MILLISECONDS);
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sha_of_prideAI(creature);
        }
};

// Manifestation of Pride 71946.
struct npc_manifestation_of_pride : public ScriptedAI
{
    npc_manifestation_of_pride(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;

    void Reset() override
    {
        DoCast(me, SPELL_MANIFESTATION_SPAWN);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            me->RemoveAurasDueToSpell(SPELL_MANIFESTATION_SPAWN);
            me->SetInCombatWithZone();
            events.ScheduleEvent(EVENT_MOCKING_BLAST, 0.5 * IN_MILLISECONDS);
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_LAST_WORD, true);
        me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
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
        events.RescheduleEvent(EVENT_MOCKING_BLAST, 6 * IN_MILLISECONDS);

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
            if (eventId == EVENT_MOCKING_BLAST)
            {
                me->StopMoving();
                me->GetMotionMaster()->MovementExpired();

                if (Unit* vict = me->GetVictim())
                {
                    if (me->GetExactDist2d(vict) > 95.0f)
                        DoStartMovement(vict);
                    else
                        DoStartNoMovement(vict);
                }

                DoCast(me, SPELL_MOCKING_BLAST);
                events.ScheduleEvent(EVENT_MOCKING_BLAST, urand(4 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Self Reflection 72172.
struct npc_self_reflection : public ScriptedAI
{
    npc_self_reflection(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        me->PrepareChanneledCast(me->GetOrientation());
        me->AddUnitState(UNIT_STATE_ROOT); // no grip/force until active
        DoCast(me, SPELL_SELF_REFLECTION_VISUAL, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (Creature* shaOfPride = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SHA_OF_PRIDE) : ObjectGuid::Empty))
            shaOfPride->AI()->JustSummoned(me);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->RemoveAurasDueToSpell(SPELL_SELF_REFLECTION_VISUAL);
            DoCast(me, SPELL_SELF_REFLECTION_EXPLODE, true);
            me->ClearUnitState(UNIT_STATE_ROOT);

            if (Player* target = me->FindNearestPlayer(150.0f))
                me->RemoveChanneledCast(target->GetGUID());
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Rift of Corruption 72846.
struct npc_rift_of_corruption : public ScriptedAI
{
    npc_rift_of_corruption(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetDisplayFromModel(0);
        DoCast(me, SPELL_RIFT_OF_CORRUPTION_VIS);
        DoCast(me, SPELL_BOLT_OF_CORRUPTION_DUMMY);
        DoCast(me, SPELL_RIFT_OF_CORRUPTION_AT);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }
};

// Shadow Prison 72017
struct npc_sha_of_pride_shadow_prison : public ScriptedAI
{
    npc_sha_of_pride_shadow_prison(Creature* creature) : ScriptedAI(creature) { }

    uint32 coneActive;

    void Reset() override
    {
        coneActive = 0;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_PRISON_CONE_ACTIVATED)
            return coneActive;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_PRISON_CONE_ACTIVATED)
            coneActive = data;
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Swelling Pride 144400
class spell_sha_of_pride_swelling_pride : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_swelling_pride);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->AI()->DoAction(ACTION_SWELLING_PRIDE_EXECUTE);

            // Trigger Additional effect of swelling pride
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, caster, 500.0f);

            for (auto&& itr : targets)
            {
                uint32 power = itr->GetPower(POWER_ALTERNATE_POWER) + 5;

                // Trigger Specific if pride higher than 25 or equal
                if (power >= 25 && power < 50) // Bursting Pride
                    caster->CastSpell(itr, SPELL_BURSTING_PRIDE, true);

                if (power >= 50 && power < 75) // Projection
                {
                    itr->CastSpell(itr, SPELL_PROJECTION_DUMMY, true);

                    // projection should`ve around
                    GetPositionWithDistInOrientation(itr, frand(5.0f, 10.0f), frand(0.0f, 2 * M_PI), x, y);
                    itr->CastSpell(x, y, itr->GetPositionZ(), SPELL_PROJECTION_MISSLE, true);
                }

                if (power >= 75 && power < 100) // Aura of Pride
                    caster->CastSpell(itr, SPELL_AURA_OF_PRIDE, true);

                if (power > 99 && !itr->HasAura(SPELL_OVERCOME_CHARMED)) // Overcome Charm
                    caster->CastSpell(itr, SPELL_OVERCOME_CHARMED, true);
                
                if (!itr->HasAura(SPELL_OVERCOME_CHARMED))
                    itr->SetUInt64Value(UNIT_FIELD_CHARMED_BY, 71734);

                    

                // Gain Power after effect
                if (!itr->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !itr->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                    itr->SetPower(POWER_ALTERNATE_POWER, power);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        AfterCast += SpellCastFn(spell_sha_of_pride_swelling_pride::HandleAfterCast);
    }
};

// Projection 145066
class spell_sha_of_pride_projection : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_projection);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (caster->GetExactDist2d(GetHitDest()) > 4.0f)
                caster->CastSpell(GetHitDest()->GetPositionX(), GetHitDest()->GetPositionY(), caster->GetPositionZ(), SPELL_PROJECTION_EXPLODE, true);
            else
                caster->CastSpell(caster, SPELL_PROJECTION_NOT_EXPLODE, true);
        }
    }

    void Register()
    {
        OnEffectHit += SpellEffectFn(spell_sha_of_pride_projection::HandleHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// Projection Explode 145320
class spell_sha_of_pride_projection_explode : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_projection_explode);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_projection_explode::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Aura of Pride 146818 
class spell_sha_of_pride_aura_of_pride : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_aura_of_pride);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_aura_of_pride::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Pride 144343
class spell_sha_of_pride_pride_bar : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_pride_bar);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // Handle Tooltip display.
            uint32 prideValue = owner->GetPower(POWER_ALTERNATE_POWER);

            if (prideValue > 99 && !owner->HasAura(SPELL_OVERCOME)) // only buff on reach 100 pride
                owner->CastSpell(owner, SPELL_OVERCOME, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_of_pride_pride_bar::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Gift of the Titans Link 144363
class spell_gift_of_the_titans_link : public SpellScript
{
    PrepareSpellScript(spell_gift_of_the_titans_link);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_POWER_OF_THE_TITANS))
                target->CastSpell(target, SPELL_POWER_OF_THE_TITANS, true);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && !target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL); });

        uint8 targetCount = GetCaster()->GetMap()->Is25ManRaid() ? 7 : 2; // less 1 than it available, cuz we not`ll target self

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetCount = 2; // min 2
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 13)
                targetCount++;

            if (totalPlayersCount > 16)
                targetCount++;

            if (totalPlayersCount > 17) // there healer new one healer got buff
                targetCount++;

            if (totalPlayersCount > 19)
                targetCount++;

            if (totalPlayersCount > 22)
                targetCount++;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount); // should be there for flex
        }

        if (targets.size() == targetCount)
            return;

        // not buff players with titans gift if not whole around 8m
        targets.clear();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_gift_of_the_titans_link::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gift_of_the_titans_link::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Mark of Arrogance 144351
class spell_sha_of_fear_mark_of_arrogance : public SpellScript
{
    PrepareSpellScript(spell_sha_of_fear_mark_of_arrogance);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets affected by titans gift if it possible
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 4 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 2(4)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_fear_mark_of_arrogance::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Mark of Arrogance 144351
class spell_sha_of_fear_mark_of_arrogance_aura : public AuraScript
{
    PrepareAuraScript(spell_sha_of_fear_mark_of_arrogance_aura);

    void OnDispel(DispelInfo* dispelInfo)
    {
        if (dispelInfo && dispelInfo->GetDispeller() && dispelInfo->GetDispeller()->ToPlayer() && !dispelInfo->GetDispeller()->ToPlayer()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !dispelInfo->GetDispeller()->ToPlayer()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
            dispelInfo->GetDispeller()->ToPlayer()->SetPower(POWER_ALTERNATE_POWER, dispelInfo->GetDispeller()->ToPlayer()->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register() override
    {
        AfterDispel += AuraDispelFn(spell_sha_of_fear_mark_of_arrogance_aura::OnDispel);
    }
};

// Wounded Pride 144358
class spell_sha_of_pride_wounded_pride : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_wounded_pride);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetActor() && eventInfo.GetActor()->GetEntry() == NPC_SHA_OF_PRIDE;
    }

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            if (!owner->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !owner->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                owner->SetPower(POWER_ALTERNATE_POWER, owner->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_sha_of_pride_wounded_pride::CheckProc);
        OnProc += AuraProcFn(spell_sha_of_pride_wounded_pride::HandleOnProc);
    }
};

// Mocking Blast 144379
class spell_sha_of_pride_mocking_blast : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_mocking_blast);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Player* target = GetHitPlayer())
        {
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
            {
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
                
                if (Creature* norushen = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_NORUSHEN_2) : ObjectGuid::Empty))
                    if (norushen->IsAlive())
                        target->GetMap()->SetWorldState(WORLDSTATE_SHALLOW_YOUR_PRIDE, 0);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks and victim affected by titans gift
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

        if (targets.size() >= 1) // doesn`t matter count - MaxAffect is 1
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_mocking_blast::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_mocking_blast::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class LastWordDistanceOrderPred
{
    public:
        LastWordDistanceOrderPred(const WorldObject* object, bool ascending = true) : m_object(object), m_ascending(ascending) {}
        bool operator() (const WorldObject* a, const WorldObject* b) const
        {
            return m_ascending ? a->GetDistance(m_object) < b->GetDistance(m_object) :
                a->GetDistance(m_object) > b->GetDistance(m_object);
        }
    private:
        const WorldObject* m_object;
        const bool m_ascending;
};

// Last Word 144370
class spell_sha_of_pride_last_word : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_last_word);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Player* target = GetHitPlayer())
        {
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
            {
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);

                if (Creature* norushen = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_NORUSHEN_2) : ObjectGuid::Empty))
                    if (norushen->IsAlive())
                        target->GetMap()->SetWorldState(WORLDSTATE_SHALLOW_YOUR_PRIDE, 0);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks and victim affected by titans gift
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

        if (targets.size() > 2) // MaxAffect is 2
        {
            targets.sort(LastWordDistanceOrderPred(GetCaster(), true));
            targets.resize(2);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (targets.size() > 2) // MaxAffect is 2
        {
            targets.sort(LastWordDistanceOrderPred(GetCaster(), true));
            targets.resize(2);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_last_word::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_last_word::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class SelfReflectionOrderPred
{
    public:
        SelfReflectionOrderPred() {}
        bool operator() (const Player* a, const Player* b) const
        {
            return a->GetPower(POWER_ALTERNATE_POWER) > b->GetPower(POWER_ALTERNATE_POWER);
        }
};

// Self-Reflection 144800
class spell_sha_of_pride_self_reflection : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_self_reflection);

    bool Load() override
    {
        tickCount = 0;
        return true;
    }

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, owner, 200.0f);

            if (targets.empty())
                return;

            targets.sort(SelfReflectionOrderPred()); // priority with high pride value

            if (targets.size() > 5)
                Trinity::Containers::RandomResizeList(targets, 5);

            for (auto&& itr : targets)
                itr->SummonCreature(NPC_REFLECTION, *itr, TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (++tickCount == 3) // summon Pride Reflection
            {
                std::list<Creature*> reflectionsList;
                GetCreatureListWithEntryInGrid(reflectionsList, owner, NPC_REFLECTION, 200.0f);

                for (auto&& itr : reflectionsList)
                    itr->AI()->DoAction(ACTION_START_INTRO);
            }
        }
    }

    private:
        uint32 tickCount;

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_sha_of_pride_self_reflection::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_of_pride_self_reflection::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Unleashed 144836
class spell_sha_of_pride_unleashed : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_unleashed);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_unleashed::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Imprison 144563
class spell_sha_of_pride_imprison : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_imprison);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->DoAction(ACTION_CALCULATE_PRISON_VALUE);

                uint32 prisonSpellId = caster->AI()->GetData(TYPE_PRISON_ID);
                uint32 prisonPosId = invShaPrisonsType.find(prisonSpellId)->second[4];

                // Cone Elements and Prison Activation
                auto key = invShaPrisonsType.find(prisonSpellId);

                if (GameObject* prison = caster->FindNearestGameObject(key->second[0], 200.0f))
                    prison->SetGoState(GO_STATE_ACTIVE);

                // In 10 ppl mode active are nearest of cone for tanks and farthest for any players
                std::list<uint32> choiseArcPrison = { key->second[1], key->second[2] };

                if (caster->GetMap()->Is25ManRaid() || caster->GetInstanceScript() && caster->GetInstanceScript()->GetData(DATA_FLEX) && caster->GetInstanceScript()->instance->GetPlayersCountExceptGMs() > 17)
                    choiseArcPrison.push_back(key->second[3]);

                for (auto&& itr : choiseArcPrison)
                    if (GameObject* arcPrison = caster->FindNearestGameObject(itr, 200.0f))
                        arcPrison->SetGoState(GO_STATE_READY);

                caster->CastSpell(target, prisonSpellId, true);

                // Interaction With Prison
                if (Creature* shaOfPride = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(DATA_SHA_OF_PRIDE) : ObjectGuid::Empty))
                    shaOfPride->CastSpell(pridePrisonsPos[prisonPosId].GetPositionX(), pridePrisonsPos[prisonPosId].GetPositionY(), pridePrisonsPos[prisonPosId].GetPositionZ(), SPELL_CORRUPTED_PRISON_ACTIV, true);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks, healers and victim affected by titans gift
        targets.remove_if(HealerSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

        uint8 targetCount = GetCaster()->GetMap()->Is25ManRaid() ? 4 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetCount++;

            if (totalPlayersCount > 20)
                targetCount++;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount); // should be there for flex
        }

        if (targets.size() >= targetCount) // doesn`t matter count - MaxAffect is 2(4)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetCount = 2; // min 2, max 4
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetCount++;

            if (totalPlayersCount > 20)
                targetCount++;

            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_imprison::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_imprison::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Corrupted Prison 144574, 144636, 144683, 144684
class spell_sha_of_pride_corrupted_prison : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_corrupted_prison);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            auto key = invShaPrisonsType.find(GetSpellInfo()->Id);
            std::list<GameObject*> prisonsList;

            for (uint8 i = 1; i < 4; i++)
                if (GameObject* tempPrison = owner->FindNearestGameObject(key->second[i], 15.0f))
                    prisonsList.push_back(tempPrison);

            prisonsList.remove_if([=](GameObject* target) { return target && target->GetGoState() != GO_STATE_READY && target->GetGoState() != GO_STATE_ACTIVE_ALTERNATIVE; });

            std::list<Player*> targets;
            GetPlayerListInGrid(targets, owner, 10.0f);

            uint32 onConeTrap = 0;

            for (auto&& firstItr : prisonsList)
            {
                bool hasAnyInner = false;

                for (auto&& secondItr : targets)
                {
                    if (secondItr->GetGUID() != owner->GetGUID() && HasInRotatingArc(owner, M_PI / 3, invShaPrisonsAngleType.find(firstItr->GetEntry())->second, secondItr) && secondItr->GetExactDist2d(owner) >= 7.15f) // trigger and player in same arc
                    {
                        hasAnyInner = true;
                        onConeTrap++;

                        if (GameObject* prisonCone = owner->FindNearestGameObject(invShaPrisonsAngleType.find(firstItr->GetEntry())->first, 200.0f))
                            prisonCone->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                        break;
                    }
                }

                if (!hasAnyInner)
                    if (GameObject* prisonCone = owner->FindNearestGameObject(invShaPrisonsAngleType.find(firstItr->GetEntry())->first, 200.0f))
                        prisonCone->SetGoState(GO_STATE_READY);
            }

            // Open Lock
            uint8 lockCount = prisonsList.size();
            if (onConeTrap >= lockCount)
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToPlayer())
        {
            // Cone Elements and Prison Activation
            auto key = invShaPrisonsType.find(GetSpellInfo()->Id);

            if (GameObject* prison = owner->FindNearestGameObject(key->second[0], 200.0f))
                prison->SetGoState(GO_STATE_READY);

            std::list<uint32> choiseArcPrison = { key->second[1], key->second[2], key->second[3] };

            for (auto&& itr : choiseArcPrison)
                if (GameObject* arcPrison = owner->FindNearestGameObject(itr, 200.0f))
                    arcPrison->SetGoState(GO_STATE_ACTIVE);

            // Free Slot for this Prison
            if (Creature* shaOfPride = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_SHA_OF_PRIDE) : ObjectGuid::Empty))
                shaOfPride->AI()->SetData(TYPE_PRISON_ID, GetSpellInfo()->Id);
        }
    }

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        // tooltip is wrong - only 5 pride at apply, not per second
        if (Unit* owner = GetOwner()->ToUnit())
            owner->SetPower(POWER_ALTERNATE_POWER, owner->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    private:
        bool HasInRotatingArc(Unit* caster, float arc, float facing, const Position* obj, float border = 2.0f)
        {
            // move arc to range 0.. 2*pi
            arc = Position::NormalizeOrientation(arc);
    
            float angle = facing; // go inf
            float secondAngle = caster->GetAngle(obj);
    
            // move angle to range -pi ... +pi
            angle = Position::NormalizeOrientation(angle);
    
            float lborder = -1 * (arc / border);                        // in range -pi..0
            float rborder = (arc / border);                             // in range 0..pi

            if (secondAngle <= M_PI / 4 && angle >= (7 * M_PI / 4))
                secondAngle += 2 * M_PI;
            else if (angle <= M_PI / 4 && secondAngle >= (7 * M_PI / 4))
                angle += 2 * M_PI;

            return (secondAngle <= angle + rborder && secondAngle >= angle + lborder);
        }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_of_pride_corrupted_prison::OnTick, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectRemoveFn(spell_sha_of_pride_corrupted_prison::OnRemove, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_sha_of_pride_corrupted_prison::OnApply, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Corrupted Prison Activation 144615
class spell_sha_of_pride_corrupted_prison_activation : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_corrupted_prison_activation);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        // not knockback our targets from prison
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_CORRUPTED_PRISON_1) || target->ToPlayer()->HasAura(SPELL_CORRUPTED_PRISON_2) || target->ToPlayer()->HasAura(SPELL_CORRUPTED_PRISON_3) || target->ToPlayer()->HasAura(SPELL_CORRUPTED_PRISON_4)); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_corrupted_prison_activation::HandleOnEffectHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_corrupted_prison_activation::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Unleashed 144832
class spell_sha_of_pride_unleashed_apply : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_unleashed_apply);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_UNLEASHED);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_unleashed_apply::HandleOnEffectHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// Final Gift 144854
class spell_sha_of_pride_final_gift : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_final_gift);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            target->SetPower(POWER_ALTERNATE_POWER, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_final_gift::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// Self-Reflection Explode 144788
class spell_sha_of_pride_self_reflection_explode : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_self_reflection_explode);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_self_reflection_explode::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Gift of the Titans Dps 146594
class spell_sha_of_pride_gift_of_the_titans_dps_selector : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_gift_of_the_titans_dps_selector);

    std::list<WorldObject*> m_targets, copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(HealerSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

        uint8 minCount = GetCaster()->GetMap()->Is25ManRaid() ? 6 : 2;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            minCount = 2; // min 2, max 6
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 13)
                minCount++;

            if (totalPlayersCount > 16)
                minCount++;

            if (totalPlayersCount > 19)
                minCount++;

            if (totalPlayersCount > 22)
                minCount++;

            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount); // should be there for flex
        }

        if (targets.size() < minCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                minCount = 2; // min 2, max 6
                uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

                if (totalPlayersCount > 13)
                    minCount++;

                if (totalPlayersCount > 16)
                    minCount++;

                if (totalPlayersCount > 19)
                    minCount++;

                if (totalPlayersCount > 22)
                    minCount++;

                if (targets.size() > minCount)
                    Trinity::Containers::RandomResizeList(targets, minCount); // should be there for flex
            }

            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount);

            copyTargets = targets;
            return;
        }

        if (targets.size() > minCount)
            Trinity::Containers::RandomResizeList(targets, minCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_gift_of_the_titans_dps_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_gift_of_the_titans_dps_selector::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Gift of the Titans Healers 144359
class spell_sha_of_pride_gift_of_the_titans_healers_selector : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_gift_of_the_titans_healers_selector);

    std::list<WorldObject*> m_targets, copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(DpsSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

        uint8 minCount = GetCaster()->GetMap()->Is25ManRaid() ? 2 : 1;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            minCount = 1; // min 1, max 2
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 17)
                minCount++;

            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount); // should be there for flex
        }

        if (targets.size() < minCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->ToUnit()->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                minCount = 1; // min 1, max 2
                uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

                if (totalPlayersCount > 17)
                    minCount++;

                if (targets.size() > minCount)
                    Trinity::Containers::RandomResizeList(targets, minCount); // should be there for flex
            }

            if (targets.size() > minCount)
                Trinity::Containers::RandomResizeList(targets, minCount);

            copyTargets = targets;
            return;
        }

        if (targets.size() > minCount)
            Trinity::Containers::RandomResizeList(targets, minCount);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_gift_of_the_titans_healers_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sha_of_pride_gift_of_the_titans_healers_selector::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Overcome 144843
class spell_sha_of_pride_overcome : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_overcome);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetInstanceScript() && GetOwner()->ToUnit()->GetInstanceScript()->GetBossState(DATA_SHA_OF_PRIDE) != IN_PROGRESS)
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_sha_of_pride_overcome::OnUpdate, EFFECT_0, SPELL_AURA_TRANSFORM);
    }
};

// Unstable Corruption 147389
class spell_sha_of_pride_unstable_corruption : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_unstable_corruption);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // No spell selector for these
            std::list<Player*> targets, tempTargets;
            GetPlayerListInGrid(targets, owner, 200.0f);
            tempTargets = targets;

            // Try not select players with gift of titans
            targets.remove_if(TankSpecTargetSelector());
            targets.remove_if([=](Player* target) { return target && (target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL)); });

            if (targets.empty())
            {
                targets.clear();

                for (auto&& itr : tempTargets)
                    targets.push_back(itr);

                if (targets.empty()) // no available targets
                    return;

                if (Player* riftTarget = Trinity::Containers::SelectRandomContainerElement(targets))
                    owner->CastSpell(riftTarget, SPELL_BOLT_OF_CORRUPTION_MIS, false);

                return;
            }

            if (Player* riftTarget = Trinity::Containers::SelectRandomContainerElement(targets))
                owner->CastSpell(riftTarget, SPELL_BOLT_OF_CORRUPTION_MIS, false);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_of_pride_unstable_corruption::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Unstable Corruption Eff 147198
class spell_sha_of_pride_unstable_corruption_eff : public SpellScript
{
    PrepareSpellScript(spell_sha_of_pride_unstable_corruption_eff);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            if (!target->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) && !target->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                target->SetPower(POWER_ALTERNATE_POWER, target->GetPower(POWER_ALTERNATE_POWER) + 5);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_sha_of_pride_unstable_corruption_eff::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Rift of Corruption Spawn 147183
class spell_sha_of_pride_rift_of_corruption : public AuraScript
{
    PrepareAuraScript(spell_sha_of_pride_rift_of_corruption);

    void OnTick(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            GetPositionWithDistInOrientation(owner, frand(45.0f, 65.0f), frand(0.0f, 2 * M_PI), x, y);
            float z = owner->GetPositionZ();
            owner->UpdateAllowedPositionZ(x, y, z);
            owner->SummonCreature(NPC_RIFT_OF_CORRUPTION, x, y, z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_of_pride_rift_of_corruption::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 1344. Summoned by 147205 - Rift of Corruption
class sat_sha_of_pride_rift_of_corruption : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && !object->ToPlayer()->HasAura(SPELL_WEAKENED_RESOLVE) && object->ToPlayer()->GetPower(POWER_ALTERNATE_POWER) != 100;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* itr = target->ToPlayer())
            {
                caster->RemoveAllAuras();
                caster->CastSpell(caster, SPELL_COLLAPSING_RIFT, true);
                caster->CastSpell(itr, SPELL_WEAKENED_RESOLVE, true);

                if (itr->HasAura(SPELL_GIFT_OF_THE_TITANS_DPS) || itr->HasAura(SPELL_GIFT_OF_THE_TITANS_HEAL))
                    return;

                itr->SetPower(POWER_ALTERNATE_POWER, itr->GetPower(POWER_ALTERNATE_POWER) + 5);
            }
        }
    }
};

void AddSC_sha_of_pride()
{
    new creature_script<boss_norushen_pride>("boss_norushen_pride");
    new boss_sha_of_pride();

    new creature_script<npc_manifestation_of_pride>("npc_manifestation_of_pride");
    new creature_script<npc_self_reflection>("npc_self_reflection");
    new creature_script<npc_rift_of_corruption>("npc_rift_of_corruption");
    new creature_script<npc_sha_of_pride_shadow_prison>("npc_sha_of_pride_shadow_prison");

    new spell_script<spell_sha_of_pride_swelling_pride>("spell_sha_of_pride_swelling_pride");
    new spell_script<spell_sha_of_pride_projection>("spell_sha_of_pride_projection");
    new spell_script<spell_sha_of_pride_projection_explode>("spell_sha_of_pride_projection_explode");
    new spell_script<spell_sha_of_pride_aura_of_pride>("spell_sha_of_pride_aura_of_pride");
    new aura_script<spell_sha_of_pride_pride_bar>("spell_sha_of_pride_pride_bar");
    new spell_script<spell_gift_of_the_titans_link>("spell_gift_of_the_titans_link");
    new spell_script<spell_sha_of_fear_mark_of_arrogance>("spell_sha_of_fear_mark_of_arrogance");
    new aura_script<spell_sha_of_fear_mark_of_arrogance_aura>("spell_sha_of_fear_mark_of_arrogance_aura");
    new aura_script<spell_sha_of_pride_wounded_pride>("spell_sha_of_pride_wounded_pride");
    new spell_script<spell_sha_of_pride_mocking_blast>("spell_sha_of_pride_mocking_blast");
    new spell_script<spell_sha_of_pride_last_word>("spell_sha_of_pride_last_word");
    new aura_script<spell_sha_of_pride_self_reflection>("spell_sha_of_pride_self_reflection");
    new spell_script<spell_sha_of_pride_unleashed>("spell_sha_of_pride_unleashed");
    new spell_script<spell_sha_of_pride_imprison>("spell_sha_of_pride_imprison");
    new aura_script<spell_sha_of_pride_corrupted_prison>("spell_sha_of_pride_corrupted_prison");
    new spell_script<spell_sha_of_pride_corrupted_prison_activation>("spell_sha_of_pride_corrupted_prison_activation");
    new spell_script<spell_sha_of_pride_unleashed_apply>("spell_sha_of_pride_unleashed_apply");
    new spell_script<spell_sha_of_pride_final_gift>("spell_sha_of_pride_final_gift");
    new spell_script<spell_sha_of_pride_self_reflection_explode>("spell_sha_of_pride_self_reflection_explode");
    new spell_script<spell_sha_of_pride_gift_of_the_titans_dps_selector>("spell_sha_of_pride_gift_of_the_titans_dps_selector");
    new spell_script<spell_sha_of_pride_gift_of_the_titans_healers_selector>("spell_sha_of_pride_gift_of_the_titans_healers_selector");
    new aura_script<spell_sha_of_pride_overcome>("spell_sha_of_pride_overcome");
    new aura_script<spell_sha_of_pride_unstable_corruption>("spell_sha_of_pride_unstable_corruption");
    new spell_script<spell_sha_of_pride_unstable_corruption_eff>("spell_sha_of_pride_unstable_corruption_eff");
    new aura_script<spell_sha_of_pride_rift_of_corruption>("spell_sha_of_pride_rift_of_corruption");
    new atrigger_script<sat_sha_of_pride_rift_of_corruption>("sat_sha_of_pride_rift_of_corruption");
}
