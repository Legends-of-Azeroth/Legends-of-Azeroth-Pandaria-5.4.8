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

#include "GameObjectAI.h"
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

enum Spells
{
    // Ji-Kun.

    SPELL_INFECTED_TALONS_AURA          = 140094, // Boss check aura on successful melee ability.
    SPELL_INFECTED_TALONS_DAMAGE        = 140092, // Player debuff.
    SPELL_TALON_RAKE                    = 134366, // Damage + increase.
    SPELL_QUILLS                        = 134380, // Triggers SPELL_QUILLS_DAMAGE damage and SPELL_QUILLS_PERIODIC (Effect 1).
    SPELL_QUILLS_DAMAGE                 = 134381, // Quills damage.
    SPELL_QUILLS_PERIODIC               = 138639, // Periodic dummy - cast of SPELL_QUILLS_VISUAL.
    SPELL_QUILLS_VISUAL                 = 138650, // Quills visual.
    SPELL_DOWN_DRAFT                    = 134370, // Areatrigger aura.
    SPELL_CAW                           = 138923, // Script Effect for SPELL_CAW_MISSILE (Effect 0).
    SPELL_CAW_MISSILE                   = 138926, // Triggers SPELL_CAW_DAMAGE.
    SPELL_CAW_DAMAGE                    = 134375, // Damage in 8 yards.


    /*Nest*/
    SPELL_DROP_FEATHER                  = 134338, // 140016 Summons lootable feather.
    SPELL_DAEDELIAN_WINGS               = 134339, // 140014 LFR version (No CD trigger). Flight spell + SPELL_LESSON_OF_ICARUS trigger. Stacks added / aura removed using this spell.
    SPELL_LESSON_OF_ICARUS              = 140571, // Cooldown 1 minute Dummy (Effect 0) - Cannot pickup wings.
    SPELL_FLIGHT_ACTION_BUTTON          = 133755, // Action button spell on Daedelian Wings. Removes 1 stack and makes player fly.
    SPELL_FEED_YOUNG                    = 137528, // Triggers SPELL_REGURGITATE each 1 sec on Normal / 2.5 sec on Heroic.
    SPELL_REGURGITATE                   = 134385, // Script effect (Effect 0) for summoning NPC_FEED_POOL / NPC_FEED_HATCHLINGS.
    SPELL_FEED_POOL_EFF                 = 138319, // Periodic damage.
    SPELL_FEED_POOL_VISUAL              = 138854, // Green visual, on platform.
    SPELL_FEED_POOL_VISUAL_HATCHLING    = 139284, // Yellow visual, on hatchlings in nest.
    SPELL_SUMMON_POOL                   = 134259, // Main platform, summons NPC_POOL_OF_FEED.
    SPELL_SUMMON_POOL_HATCHLINGS        = 139285, // Hatchling nest, summons NPC_POOL_OF_FEED_HATCHLING
    SPELL_SLIMED                        = 134256, // On players intercepting Ji-Kun's food globules. Periodic damage and Script Effect (Effect 1) for SPELL_SLIMED_DMG_INCREASE.
    SPELL_SLIMED_DMG_INCREASE           = 138309, // 10% damage increase from next Slimed.
    SPELL_PRIMAL_NUTRIMENT              = 140741, // On players intercepting Ji-Kun's food globules. Eff 0 + 1 Dummy A. H / D incr., SPELL_PRIMAL_NUTRIMENT_INCREASE trigger on Eff 2.
    SPELL_PRIMAL_NUTRIMENT_INCREASE     = 112879, // 30% Healing and 100% Damage increase.
    SPELL_HATCHLING_CHEEP               = 139296, // Triggers SPELL_HATCHLING_CHEEP_DAMAGE in 10 yards.
    SPELL_HATCHLING_CHEEP_DAMAGE        = 139298,
    SPELL_CHEEP_AOE                     = 140129,
    SPELL_CHEEP_SELECTOR                = 140227,
    SPELL_INCUBATION_LOW_NEST           = 134347, // 10 sec incubation
    SPELL_INCUBATION_HIGH_NEST          = 134335, // 20 sec incubation
    SPELL_SUMMON_YOUNG_HATCHLING        = 134336,
    SPELL_SUMMON_JUVENILE               = 138905,
    SPELL_HATCHLING_EVOLUTION           = 134322,
    SPELL_HATCHLING_EATING              = 134321,
    SPELL_FLEDGLING_LAY_EGG             = 134367, // Summons NPC_FLEDGLING_EGG_JIKUN.
    SPELL_FLEDGLING_CHEEP               = 140570, // Needs target limitation.
    SPELL_JUVENILE_CHEEP                = 140227, // Script effect (Effect 0) for SPELL_JUVENILE_CHEEP_DAMAGE in 60 yards.
    SPELL_JUVENILE_CHEEP_DAMAGE         = 140129, // Damage in 10 yards.
    SPELL_BEAM_VISUAL                   = 137526, // visual aur of beam
    SPELL_PLAYER_VISUAL_SPIN            = 89428,
    SPELL_PLAYER_CATCH_FALL             = 85282, // Ride Vehicle
    SPELL_PLAYER_EJECT                  = 68576,
    SPELL_FORCE_JUMP_TO_PLATFORM        = 138406,
    SPELL_JUMP_TO_PLATFORM              = 138359,
    SPELL_JUMP_TO_PLATFORM_AT           = 138360,
    SPELL_PREVENT_FALL_DAMAGE           = 139265,
    SPELL_DROP_FEATHERS_GO              = 140016,
    SPELL_FLIGHT_OUTRO                  = 140013,
    SPELL_DAEDALUS_WINGS_ADDIT          = 140014,


    /*Heroic*/
    SPELL_GUARDIAN_TALON_STRIKE         = 139100, // Weapon damage 180 degree arc.
    SPELL_GUARDIAN_SCREECH              = 140640, // Damage, Interrupt, Script Effect (Effect 2) for SPELL_GUARDIAN_SCREECH_SLOW. If not actively engaged in melee combat.
    SPELL_GUARDIAN_SCREECH_SLOW         = 134372, // Slow casting on targets (Removed after casting any spell). Mana users.

    // Misc
    SPELL_SOFT_HANDS                    = 139162, // BE_SPELL_TARGET_2
    SPELL_GENTLE_YET_FIRM               = 139168,
    SPELL_PASS                          = 139183,
    SPELL_RECEPTION                     = 139193,
    SPELL_SAFETY_NET_TRIGGER            = 136524,
    SPELL_DROPPED                       = 139224,
};

enum FeedMovementPoints
{
    POINT_GREEN_FEED_AIR = 1,
    POINT_GREEN_FEED_GROUND,
    POINT_FEED_CAME_TO_HATCHLING,
};

enum Actions
{
    ACTION_LAY_EGG,
    ACTION_DROP_TO_PLATFORM,
};

enum JuvenileMovementPoints
{
    POINT_MOVE_TO_CENTER_FLY = 1,
};

enum HatchlingMovementPoints
{
    POINT_MOVE_TO_EAT_POOL = 1,
};

enum JikunMovementPoints
{
    POINT1 = 1,
    POINT2 = 2,
};

enum EventsJiKun
{
    EVENT_TALON_RAKE = 1,
    EVENT_QUILLS,
    EVENT_DOWN_DRAFT,
    EVENT_DOWN_DRAFT_FALL,
    EVENT_CAW,
    EVENT_ACTIVATE_NEST,
    EVENT_FEED_YOUNG,
    EVENT_FIND_EAT,
    EVENT_CHEEP,
    EVENT_LAY_EGG,
    EVENT_START_FLY_AWAY,
    EVENT_JUVENILE_CHEEP,
    EVENT_SUMMON_HATCHLING,
    EVENT_CHECK_FOR_PLAYERS,
    EVENT_EVADE,
    EVENT_DROP_GOLDEN_EGG,
};

enum Yells
{
    TALK_UPPER_NESTS,
    TALK_LOWER_NESTS,
    TALK_QUILLS_ANN,
    TALK_DOWN_DRAFT_ANN,
    TALK_CAWS_ANN,

    // Incubator
    TALK_LOWER_1,
    TALK_LOWER_2,
    TALK_LOWER_3,
    TALK_LOWER_4,
    TALK_LOWER_5,
    TALK_UPPER_1,
    TALK_UPPER_2,
    TALK_UPPER_3,
    TALK_UPPER_4,
    TALK_UPPER_5,
};

// Cycle of Feed Nests. Sometimes it mix (low + high).
// For items in array use [n-1] of position list. key - number of cycle
const std::map<uint32, std::array<uint32, 2>> invNestType =
{
    { 1, { 1,  0 } },
    { 2, { 2,  0 } }, // first nest guardian here ([3n-1] for 10 HC, mod2 for 25HC)
    { 3, { 3,  0 } },
    { 4, { 4,  0 } },
    { 5, { 5,  9 } },
    { 6, { 6,  0 } },
    { 7, { 10, 0 } },
    { 8, { 7,  1 } },
    { 9, { 8,  2 } },
};

// Ji-Kun - 69712.
class boss_jikun : public CreatureScript
{
    public:
        boss_jikun() : CreatureScript("boss_jikun") { }

        struct boss_jikunAI : public BossAI
        {
            boss_jikunAI(Creature* creature) : BossAI(creature, DATA_JI_KUN)
            {
                PreIntro();
                SetCombatMovement(false);
                me->GetMap()->CreatureRelocation(me, 6147.18f, 4318.77f, -31.86f, 0.23f);
            }

            EventMap additionalEvents;
            bool introDone;
            uint32 nestKey;
            uint32 feedValue;
            uint32 guardianKey;
            uint32 juvenieCounter;
            bool allowAchiev;

            void PreIntro()
            {
                introDone = false;
                //me->SetVisible(false);
                //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                //me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                _Reset();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PRIMAL_NUTRIMENT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TALON_RAKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DAEDELIAN_WINGS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SLIMED_DMG_INCREASE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFECTED_TALONS_DAMAGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEED_POOL_EFF);
                }

                me->SetReactState(REACT_DEFENSIVE);
                me->SetDisableGravity(true);
                me->SetCanFly(true);
                nestKey   = 1;
                guardianKey = 1;
                feedValue = 0;
                juvenieCounter = 0;
                allowAchiev = false;
                events.Reset();
                additionalEvents.Reset();

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 244.0m in 10 normal
                    me->SetCreateHealth(244000000);
                    me->SetMaxHealth(244000000);
                    me->SetHealth(244000000);
                    me->ResetPlayerDamageReq();
                }

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_FEED:
                        feedValue = data > 1 ? 0 : data;
                        break;
                    case TYPE_SOFT_HANDS:
                        allowAchiev = data;
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_FEED:
                        return feedValue;
                    case TYPE_NEST:
                        return nestKey - 1;
                    case TYPE_SOFT_HANDS:
                        return allowAchiev ? 1 : 0;
                }

                return 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    for (auto&& itr : instance->instance->GetPlayers())
                    {
                        if (Player* player = itr.GetSource())
                            if (player->IsAlive() && !player->IsGameMaster())
                                player->CastSpell(player, SPELL_PREVENT_FALL_DAMAGE, true);
                    }
                }

                juvenieCounter = 0;

                DoCast(me, SPELL_INFECTED_TALONS_AURA);

                events.ScheduleEvent(EVENT_TALON_RAKE, 5000); 
                events.ScheduleEvent(EVENT_QUILLS, 40000);
                events.ScheduleEvent(EVENT_DOWN_DRAFT, 90000);
                events.ScheduleEvent(EVENT_CAW, 16000);
                DoAction(ACTION_ACTIVATE_NEST);

                // Achievement
                additionalEvents.ScheduleEvent(EVENT_DROP_GOLDEN_EGG, 20 * IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENTLE_YET_FIRM);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_JI_KUN, FAIL);
                }

                summons.DespawnAll();
                additionalEvents.Reset();

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, 6147.18f, 4318.77f, -31.86f, 0.23f);
            }

            void JustSummoned(Creature* summon) override
            {
                // Limit 15 - server freeze
                if (summon->GetEntry() == NPC_JIKUN_JUVENILE)
                {
                    if (juvenieCounter > 14)
                    {
                        summon->DespawnOrUnsummon();
                        return;
                    }

                    juvenieCounter++;
                }

                summons.Summon(summon);
                summon->setActive(true);

                if (me->IsInCombat() && summon->GetEntry() != NPC_GOLDEN_EGG)
                    summon->SetInCombatWithZone();
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                additionalEvents.Reset();

                if (instance)
                {
                    if (GetData(TYPE_SOFT_HANDS))
                        instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, SPELL_SOFT_HANDS);

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PRIMAL_NUTRIMENT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TALON_RAKE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_DAEDELIAN_WINGS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SLIMED_DMG_INCREASE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INFECTED_TALONS_DAMAGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEED_POOL_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENTLE_YET_FIRM);

                    if (Creature* chamberExit = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_EXIT_CHAMBER)))
                        chamberExit->CastSpell(chamberExit, SPELL_EXIT_CHAMBER_AT, true);

                    // Save in data cuz setBossState not possible check hanlde after crash. Need for activate feathers and portal
                    instance->SetData(DATA_JI_KUN_EVENT, DONE);
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                    {
                        if (introDone)
                            return;

                        me->GetMotionMaster()->MovePoint(POINT1, IntroMoving[0]);

                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetVisible(true);
                        me->SetHomePosition(IntroMoving[1]);

                        DoPlaySoundToSet(me, 36213); // pre agro

                        introDone = true;
                        break;
                    }
                    case ACTION_ACTIVATE_NEST:
                    {
                        auto key = invNestType.find(nestKey);
                        if (Creature* incubator = me->SummonCreature(NPC_INCUBATOR, nestPos[key->second[0] - 1], TEMPSUMMON_MANUAL_DESPAWN))
                            incubator->AI()->Talk(key->second[0] - 1);

                        if (IsHeroic())
                        {
                            if (Is25ManRaid() && guardianKey % 2 == 0)
                            {
                                me->SummonCreature(NPC_NEST_GUARDIAN, nestPos[key->second[0] - 1], TEMPSUMMON_MANUAL_DESPAWN);
                                guardianKey++;
                            }
                            else if (Is25ManRaid())
                                guardianKey++;
                            else // 10 ppl mode
                            {
                                if (nestKey == 3 * guardianKey - 1)
                                {
                                    me->SummonCreature(NPC_NEST_GUARDIAN, nestPos[key->second[0] - 1], TEMPSUMMON_MANUAL_DESPAWN);
                                    guardianKey++;
                                }
                            }
                        }

                        if (key->second[1])
                            if (Creature* incubator = me->SummonCreature(NPC_INCUBATOR, nestPos[key->second[1] - 1], TEMPSUMMON_MANUAL_DESPAWN))
                                incubator->AI()->Talk(key->second[1] - 1);

                        if (++nestKey > 9)
                            nestKey = 3;

                        events.ScheduleEvent(EVENT_FEED_YOUNG, 12000);

                        additionalEvents.ScheduleEvent(EVENT_ACTIVATE_NEST, Is25ManRaid() ? 30 * IN_MILLISECONDS : 40 * IN_MILLISECONDS);
                        break;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case POINT1: 
                        //float x, float y, float z, float speedXY, float speedZ, uint32 id, float orientation
                        me->GetMotionMaster()->MoveJump(IntroMoving[1].GetPositionX(), IntroMoving[1].GetPositionY(), IntroMoving[1].GetPositionZ(), 15.0f, 15.0f, POINT2/*, 10.0f*/);
                        break;
                    case POINT2:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        introDone = true;
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                additionalEvents.Update(diff);

                while (uint32 eventId = additionalEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ACTIVATE_NEST:
                            DoAction(ACTION_ACTIVATE_NEST);
                            break;
                        case EVENT_DROP_GOLDEN_EGG:
                            me->SummonCreature(NPC_GOLDEN_EGG, goldenEggThrowPos, TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FEED_YOUNG:
                            DoCast(me, SPELL_FEED_YOUNG);
                            break;
                        case EVENT_TALON_RAKE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_TALON_RAKE);

                            events.ScheduleEvent(EVENT_TALON_RAKE, 20000);
                            break;
                        case EVENT_QUILLS:
                            Talk(TALK_QUILLS_ANN);
                            DoCast(me, SPELL_QUILLS);
                            events.ScheduleEvent(EVENT_QUILLS, 60000);
                            break;
                        case EVENT_DOWN_DRAFT:
                            Talk(TALK_DOWN_DRAFT_ANN);
                            DoCast(SPELL_DOWN_DRAFT);
                            events.ScheduleEvent(EVENT_DOWN_DRAFT, 90000);
                            break;
                        case EVENT_CAW:
                            Talk(TALK_CAWS_ANN);
                            DoCast(SPELL_CAW);
                            events.ScheduleEvent(EVENT_CAW, 16000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_jikunAI(creature);
        }
};

// Incubator 69626
struct npc_jikun_incubator : public ScriptedAI
{
    npc_jikun_incubator(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        me->SetDisableGravity(true);
        me->SetCanFly(true);
        me->SetDisplayId(11686);
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

        // Spawn Eggs there
        bool highNest = me->GetPositionZ() > -25.0f ? true : false;

        uint32 eggCount = highNest ? 3 : Is25ManRaid() ? 5 : 3;
        for (uint32 i = 0; i < eggCount; i++)
            me->SummonCreature(highNest ? NPC_MATURE_EGG_OF_JIKUN : NPC_YOUNG_EGG_OF_JIKUN, me->GetPositionX() + frand(-6.5f, 6.5f), me->GetPositionY() + frand(-6.5f, 6.5f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);


        DoCast(me, SPELL_BEAM_VISUAL, true);
    }

    void UpdateAI(uint32 diff) override { }
};

// Young hatchling - 68192.
struct npc_young_hatchling_jikun : public ScriptedAI
{
    npc_young_hatchling_jikun(Creature* creature) : ScriptedAI(creature) { }

    EventMap events, nonCombatEvents;
    InstanceScript* instance;

    void IsSummonedBy(Unit* summoner) override
    {
        instance = me->GetInstanceScript();

        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        me->setRegeneratingHealth(false);
        me->PrepareChanneledCast(me->GetOrientation());

        // Temp Hackfix
        if (me->GetInstanceScript() && me->GetInstanceScript()->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
        {
            // should be 1.26m in 10 normal
            me->SetCreateHealth(1264000);
            me->SetMaxHealth(1264000);
            me->SetHealth(1264000);
            me->ResetPlayerDamageReq();
        }

        // set health value depend of current egg health
        if (summoner && summoner->ToCreature())
        {
            me->SetHealth((int32)(me->GetMaxHealth() * (summoner->GetHealthPct() / 100.0f)));
            summoner->ToCreature()->DespawnOrUnsummon();
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker)
            me->RemoveChanneledCast(attacker->GetGUID());
    }

    void Reset() override
    {
        events.Reset();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_LAY_EGG)
        {
            if (!me->IsAlive())
                return;

            DoCast(me, SPELL_HATCHLING_EVOLUTION, true);
            nonCombatEvents.ScheduleEvent(EVENT_LAY_EGG, urand(15000, 25000));
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CHEEP, urand(2.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        // sometimes happened
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_JI_KUN) != IN_PROGRESS)
            me->DespawnOrUnsummon();

        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_LAY_EGG:
                    if (me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        nonCombatEvents.RescheduleEvent(EVENT_LAY_EGG, urand(1 * IN_MILLISECONDS,2 * IN_MILLISECONDS));
                        break;
                    }
                
                    DoCast(SPELL_FLEDGLING_LAY_EGG);
                    nonCombatEvents.ScheduleEvent(EVENT_LAY_EGG, urand(40000, 50000));
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_CHEEP)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, me->HasAura(SPELL_HATCHLING_EVOLUTION) ? SPELL_FLEDGLING_CHEEP : SPELL_HATCHLING_CHEEP);

                events.ScheduleEvent(EVENT_CHEEP, urand(5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff, 8.0f);
    }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_DROP_FEATHERS_GO, true);
    }
};

// Juvenile - 70095.
struct npc_juvenile : public ScriptedAI
{
    npc_juvenile(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        me->SetInCombatWithZone();
        me->setRegeneratingHealth(false);
        me->OverrideInhabitType(INHABIT_AIR);
        me->UpdateMovementFlags();

        // Temp Hackfix
        if (me->GetInstanceScript() && me->GetInstanceScript()->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
        {
            // should be 1.89m in 10 normal
            me->SetCreateHealth(1897000);
            me->SetMaxHealth(1897000);
            me->SetHealth(1897000);
            me->ResetPlayerDamageReq();
        }

        // set health value depend of current egg health
        if (summoner && summoner->ToCreature())
        {
            me->SetHealth((int32)(me->GetMaxHealth() * (summoner->GetHealthPct() / 100.0f)));
            summoner->ToCreature()->DespawnOrUnsummon();
        }

        events.ScheduleEvent(EVENT_START_FLY_AWAY, 2500);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == POINT_MOVE_TO_CENTER_FLY)
            events.ScheduleEvent(EVENT_JUVENILE_CHEEP, urand(5000, 15000));
    }

    void UpdateAI(uint32 diff) override
    {
        // sometimes happened
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_JI_KUN) != IN_PROGRESS)
            me->DespawnOrUnsummon();

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_FLY_AWAY:
                    me->GetMotionMaster()->MovePoint(POINT_MOVE_TO_CENTER_FLY, jiKunCenter.GetPositionX() + frand(-45.0f, 45.0f), jiKunCenter.GetPositionY() + frand(-45.0f, 45.0f), 30.0f);
                    break;
                case EVENT_JUVENILE_CHEEP:
                    DoCast(me, SPELL_CHEEP_SELECTOR);
                    events.ScheduleEvent(EVENT_JUVENILE_CHEEP, urand(5000, 15000));
                    break;
            }
        }
    }
};

// Egg of Ji-Kun - 69628, 68194, 68202
struct npc_egg_of_jikun : public ScriptedAI
{
    npc_egg_of_jikun(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        if (me->GetEntry() == NPC_FLEDGLING_EGG_JIKUN)
            DoCast(me, SPELL_INCUBATION_HIGH_NEST, true);

        // Temp Hackfix
        if (me->GetInstanceScript() && me->GetInstanceScript()->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
        {
            uint32 value = me->GetMaxHealth();

            switch (me->GetEntry())
            {
                case NPC_YOUNG_EGG_OF_JIKUN:
                    value = 1264000;
                    break;
                case NPC_FLEDGLING_EGG_JIKUN:
                case NPC_MATURE_EGG_OF_JIKUN:
                    value = 1897000;
                    break;
            }

            me->SetCreateHealth(value);
            me->SetMaxHealth(value);
            me->SetHealth(value);
            me->ResetPlayerDamageReq();
        }
    }

    void JustDied(Unit* killer) override
    {
        DoCast(me, SPELL_DROP_FEATHERS_GO, true);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Jump to Boss Platform 69885
struct npc_jump_to_boss_platform : public ScriptedAI
{
    npc_jump_to_boss_platform(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            DoCast(me, SPELL_JUMP_TO_PLATFORM_AT, true);
            context.Repeat(Minutes(10));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Fall Catcher 69839
struct npc_fall_catcher_jikun : public ScriptedAI
{
    npc_fall_catcher_jikun(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetDisplayFromModel(1);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        summonerGUID = summoner->GetGUID();
        x = 0.0f; y = 0.0f;
        Movement::MoveSplineInit init(me);
        init.MoveTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 150.0f);

        init.SetFly();
        init.Launch();

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            if (Unit* passenger = ObjectAccessor::GetUnit(*me, summonerGUID))
            {
                passenger->ExitVehicle();
                
                if (Creature* jikun = ObjectAccessor::GetCreature(*me, passenger->GetInstanceScript() ? passenger->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
                {
                    GetPositionWithDistInOrientation(jikun, 38.0f, jikun->GetAngle(passenger), x, y);
                    passenger->CastSpell(x, y, jikunJumpPos.GetPositionZ(), SPELL_JUMP_TO_PLATFORM, true);
                }

                me->DespawnOrUnsummon();
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Feed of Ji-Kun 68178 70130
struct npc_jikun_feed : public ScriptedAI
{
    npc_jikun_feed(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap nonCombatEvents;
    std::vector<ObjectGuid> slimedGUIDs;

    void IsSummonedBy(Unit* summoner) override
    {
        slimedGUIDs.clear();
        me->SetDisplayFromModel(1);

        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        scheduler
            .Schedule(Milliseconds(200), [this](TaskContext context)
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, me, 3.0f);

            for (auto&& itr : pList)
            {
                if (std::find(slimedGUIDs.begin(), slimedGUIDs.end(), itr->GetGUID()) != slimedGUIDs.end())
                    continue;

                if (me->GetDistance(itr) <= 1.5f)
                {
                    itr->CastSpell(itr, SPELL_SLIMED_DMG_INCREASE, true);
                    itr->CastSpell(itr, SPELL_SLIMED, true);
                    slimedGUIDs.push_back(itr->GetGUID());

                    if (me->GetEntry() == NPC_FEED_HATCHLINGS && !itr->HasAura(SPELL_PRIMAL_NUTRIMENT))
                        itr->CastSpell(itr, SPELL_PRIMAL_NUTRIMENT, true);
                    else if (me->GetEntry() == NPC_FEED_POOL && itr->HasAura(SPELL_FLIGHT_ACTION_BUTTON) && !itr->HasAura(SPELL_PRIMAL_NUTRIMENT))
                    {
                        itr->CastSpell(itr, SPELL_PRIMAL_NUTRIMENT, true); // catch green slime only in flight
                        me->DespawnOrUnsummon();
                    }

                    break;
                }
            }

            context.Repeat(Milliseconds(200));
        });
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        switch (pointId)
        {
            case POINT_GREEN_FEED_AIR: // when came to position in air must fall to ground
                nonCombatEvents.ScheduleEvent(EVENT_START_FLY_AWAY, 0.2 * IN_MILLISECONDS);
                break;
            case POINT_GREEN_FEED_GROUND: // when falled on ground must summon pool
                DoCast(SPELL_SUMMON_POOL);
                me->DespawnOrUnsummon();
                break;
            case POINT_FEED_CAME_TO_HATCHLING: // when came summon eat for hatchling and select hatchling to eat it
                DoCast(SPELL_SUMMON_POOL_HATCHLINGS);
                me->DespawnOrUnsummon();
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_START_FLY_AWAY)
                me->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), -31.0f, 5.0f, 17.0f, POINT_GREEN_FEED_GROUND/*, 10.0f*/);
            break;

        }
    }
};

// Pool of feed 68188
struct npc_pool_of_feed_effect : public ScriptedAI
{
    npc_pool_of_feed_effect(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        DoCast(me, SPELL_FEED_POOL_VISUAL, true);

        // Not found selector for this in sniffs - only one not existed spell.
        scheduler
            .SetValidator([this] { return me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_JI_KUN) == IN_PROGRESS; })
            .Schedule(Milliseconds(200), [this](TaskContext context)
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, me, 10.0f);

            for (auto&& itr : pList)
            {
                if (itr->HasAura(SPELL_FEED_POOL_EFF, me->GetGUID()) && itr->GetExactDist2d(me) > 3.6f)
                    itr->RemoveAurasDueToSpell(SPELL_FEED_POOL_EFF, me->GetGUID());
                else if (!itr->HasAura(SPELL_FEED_POOL_EFF, me->GetGUID()) && itr->GetExactDist2d(me) <= 3.6f)
                    DoCast(itr, SPELL_FEED_POOL_EFF, true);
            }

            context.Repeat(Milliseconds(200));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Pool of Feed Hatchling Eff 70216
struct npc_pool_of_feed_hatchling_effect : public ScriptedAI
{
    npc_pool_of_feed_hatchling_effect(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        DoCast(me, SPELL_FEED_POOL_VISUAL_HATCHLING, true);

        // Same
        scheduler
            .SetValidator([this] { return me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_JI_KUN) == IN_PROGRESS; })
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            std::list<Creature*> hatchlingList;
            GetCreatureListWithEntryInGrid(hatchlingList, me, NPC_JIKUN_HATCHLING, 10.0f);
            hatchlingList.remove_if([=](Creature* target) { return target && (target->HasAura(SPELL_HATCHLING_EATING) || target->HasAura(SPELL_HATCHLING_EVOLUTION)); });

            for (auto&& itr : hatchlingList)
                itr->CastSpell(me, SPELL_HATCHLING_EATING, true);

            context.Repeat(Milliseconds(500));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Nest Guardian 70134
struct npc_nest_guardian : public ScriptedAI
{
    npc_nest_guardian(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    TaskScheduler scheduler;
    InstanceScript* instance;
    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = ObjectGuid::Empty;
        instance = me->GetInstanceScript();

        if (Creature* jiKun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
            jiKun->AI()->JustSummoned(me);

        me->setRegeneratingHealth(false);
        me->PrepareChanneledCast(me->GetOrientation());
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker)
            me->RemoveChanneledCast(attacker->GetGUID());
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_TALON_RAKE, urand(2.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void EnterEvadeMode() override
    {
        // Periodic AOE at evade
        scheduler
            .SetValidator([this] { return me->HasUnitState(UNIT_STATE_CASTING); })
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            DoCast(me, SPELL_GUARDIAN_SCREECH);
            context.Repeat(Milliseconds(2000));
        });
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
            if (eventId == EVENT_TALON_RAKE)
            {
                if (Unit* target = me->GetVictim())
                {
                    targetGUID = target->GetGUID();

                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_GUARDIAN_TALON_STRIKE);

                    scheduler
                        .SetValidator([this] { return me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_JI_KUN) == IN_PROGRESS; })
                        .Schedule(Milliseconds(1000), [this](TaskContext context)
                    {
                        me->RemoveChanneledCast(targetGUID);
                    });
                }

                events.ScheduleEvent(EVENT_TALON_RAKE, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff, 8.0f);
    }

    void JustDied(Unit* killer) override
    {
        me->DisappearAndDie();
        scheduler.CancelAll();
    }
};

// Golden Egg 69816
struct npc_jikun_golden_egg : public ScriptedAI
{
    npc_jikun_golden_egg(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        DoCast(me, SPELL_GENTLE_YET_FIRM);

        scheduler
            .SetValidator([this] { return me->HasAura(SPELL_GENTLE_YET_FIRM); })
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            DoCast(me, SPELL_DROPPED);
            context.Repeat(Milliseconds(500));
        });

        me->GetMotionMaster()->MoveJump(goldenEggJumpPos, 25.0f, 25.0f, EVENT_JUMP);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
            me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Caw 138923
class spell_caw : public SpellScript
{
    PrepareSpellScript(spell_caw);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_CAW_MISSILE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude casters
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_FLIGHT_ACTION_BUTTON); });

            uint32 targetCount = caster->GetMap()->Is25ManRaid() ? 5 : 2;
            // if we have enough players without tanks, meelee
            if (pList.size() >= targetCount)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);

                return;
            }

            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_FLIGHT_ACTION_BUTTON); });

            // if we haven`t enough players
            if (targets.size() > targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_caw::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_caw::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Infected talons aura proc 140094.
class spell_infected_talons : public AuraScript
{
    PrepareAuraScript(spell_infected_talons);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* target = eventInfo.GetProcTarget())
            if (Unit* owner = GetOwner()->ToUnit())
                owner->CastSpell(target, SPELL_INFECTED_TALONS_DAMAGE, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_infected_talons::HandleOnProc);
    }
};

// Hatchling Eated 134321
class spell_hatchling_eated_aura : public AuraScript
{
    PrepareAuraScript(spell_hatchling_eated_aura);

    void OnRemove(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_LAY_EGG);
    }

    void OnRemoveTarget(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_hatchling_eated_aura::OnRemove, EFFECT_1, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_hatchling_eated_aura::OnRemoveTarget, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Hatchling Eated 134321
class spell_hatchling_eated : public SpellScript
{
    PrepareSpellScript(spell_hatchling_eated);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_POOL_OF_FEED_HATCHLING; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hatchling_eated::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Incubated 134347, 134335
class spell_incubated : public AuraScript
{
    PrepareAuraScript(spell_incubated);

    void OnRemove(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* hatchling = GetOwner()->ToCreature())
            if (hatchling->IsAlive() && hatchling->GetInstanceScript() && hatchling->GetInstanceScript()->GetBossState(DATA_JI_KUN) == IN_PROGRESS)
                hatchling->CastSpell(hatchling, GetSpellInfo()->Id == SPELL_INCUBATION_HIGH_NEST ? SPELL_SUMMON_JUVENILE : SPELL_SUMMON_YOUNG_HATCHLING, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_incubated::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Feed Pool Eff 138319
class spell_feed_pool_eff : public AuraScript
{
    PrepareAuraScript(spell_feed_pool_eff);

    std::map<uint64, uint32> invPoolType;

    void OnPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // First tick
            if (invPoolType.find(owner->GetGUID()) == invPoolType.end())
                invPoolType.insert(std::pair<uint64, uint32>(owner->GetGUID(), 1));
            else
            {
                // If anyone got 3 ticks not leave - remove pool
                if (++invPoolType.find(owner->GetGUID())->second > 3)
                {
                    if (GetCaster() && GetCaster()->ToCreature())
                    {
                        owner->RemoveAurasDueToSpell(GetSpellInfo()->Id, GetCaster()->GetGUID());
                        owner->CastSpell(owner, SPELL_SLIMED_DMG_INCREASE, true);
                        owner->CastSpell(owner, SPELL_SLIMED, true);

                        // Remove whole from our debuff
                        std::list<Player*> pList;
                        GetPlayerListInGrid(pList, GetCaster(), 10.0f);

                        for (auto&& itr : pList)
                            itr->RemoveAurasDueToSpell(GetSpellInfo()->Id, GetCaster()->GetGUID());

                        GetCaster()->ToCreature()->DespawnOrUnsummon();
                    }
                }
            }
        }
    }

    void OnRemove(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (invPoolType.find(owner->GetGUID()) != invPoolType.end())
                invPoolType.find(owner->GetGUID())->second = 0;
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_feed_pool_eff::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        OnEffectRemove += AuraEffectRemoveFn(spell_feed_pool_eff::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);

    }
};

// Stack of wings remove 133755
class spell_remove_stack_wings : public AuraScript
{
    PrepareAuraScript(spell_remove_stack_wings);

    void OnApply(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Aura* wings = owner->GetAura(SPELL_DAEDELIAN_WINGS))
                wings->ModStackAmount(-1);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_remove_stack_wings::OnApply, EFFECT_0, SPELL_AURA_FLY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

// Quils Periodic Eff 138639
class spell_quils_periodic_eff : public AuraScript
{
    PrepareAuraScript(spell_quils_periodic_eff);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_QUILLS_VISUAL, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_quils_periodic_eff::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Quils Visual 138650
class spell_quils_visual : public SpellScript
{
    PrepareSpellScript(spell_quils_visual);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != GO_JIKUN_EGG; });

        if (targets.size() > 10)
            Trinity::Containers::RandomResizeList(targets, 10); // fps issue
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_quils_visual::SelectTargets, EFFECT_0, TARGET_GAMEOBJECT_SRC_AREA);
    }
};

// Incubate 137526
class spell_incubate_visual : public AuraScript
{
    PrepareAuraScript(spell_incubate_visual);

    void OnRemove(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectApplyFn(spell_incubate_visual::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Incubate 137526
class spell_incubate_visual_eff : public SpellScript
{
    PrepareSpellScript(spell_incubate_visual_eff);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
        {
            target->CastSpell(target, target->GetEntry() == NPC_MATURE_EGG_OF_JIKUN ? SPELL_INCUBATION_HIGH_NEST : SPELL_INCUBATION_LOW_NEST, true);
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            target->SetFaction(14);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_MATURE_EGG_OF_JIKUN && target->GetEntry() != NPC_YOUNG_EGG_OF_JIKUN && target->GetEntry() != NPC_FLEDGLING_EGG_JIKUN; });

        // Remove Cosmetic Eggs
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->GetDBTableGUIDLow(); });

    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_incubate_visual_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_incubate_visual_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Regurgitate 134385
class spell_regurgitate : public SpellScript
{
    PrepareSpellScript(spell_regurgitate);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (caster->AI()->GetData(TYPE_FEED))
            {
                // Not Used Feed
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (Creature* unusedFeed = caster->SummonCreature(NPC_FEED_POOL, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ() + 6.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 12000))
                    {
                        relocateFeed = unusedFeed->GetRandomPoint(jiKunCenter, 40.0f);
                        unusedFeed->GetMotionMaster()->MoveJump(relocateFeed.GetPositionX(), relocateFeed.GetPositionY(), 6.0f, 35.0f, 20.0f, POINT_GREEN_FEED_AIR/*, 10.0f*/);
                    }
                }

                caster->AI()->SetData(TYPE_FEED, 2);
            }
            else
            {
                uint32 nestEntry = 0;
                // Check if low nests were activated.
                if (invNestType.find(caster->AI()->GetData(TYPE_NEST))->first < 6)
                    nestEntry = invNestType.find(caster->AI()->GetData(TYPE_NEST))->second[0];
                else if (invNestType.find(caster->AI()->GetData(TYPE_NEST))->second[1] < 3)
                    nestEntry = invNestType.find(caster->AI()->GetData(TYPE_NEST))->second[1];
                
                if (nestEntry)
                {
                    // Feed for hatchlings
                    for (uint32 i = 0; i < 3; i++)
                    {
                        if (Creature* Feed = caster->SummonCreature(NPC_FEED_HATCHLINGS, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ() + 6.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 12000))
                        {
                            relocateFeed = Feed->GetRandomPoint(nestPos[nestEntry - 1], 5.5f);
                            Feed->GetMotionMaster()->MoveJump(relocateFeed.GetPositionX(), relocateFeed.GetPositionY(), relocateFeed.GetPositionZ() + 1.0f, 15.0f, 50.0f, POINT_FEED_CAME_TO_HATCHLING/*, 10.0f*/);
                        }
                    }
                } 
                else // If low nest aren`t activate - send not used feed into platform
                {
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        if (Creature* unusedFeed = caster->SummonCreature(NPC_FEED_POOL, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ() + 6.0f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 12000))
                        {
                            relocateFeed = unusedFeed->GetRandomPoint(jiKunCenter, 40.0f);
                            unusedFeed->GetMotionMaster()->MoveJump(relocateFeed.GetPositionX(), relocateFeed.GetPositionY(), 6.0f, 35.0f, 20.0f, POINT_GREEN_FEED_AIR/*, 10.0f*/);
                        }
                    }
                }

                caster->AI()->SetData(TYPE_FEED, 1);
            }
        }
    }

    private:
        Position relocateFeed;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_regurgitate::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Cheep 140227
class spell_cheep_selector : public SpellScript
{
    PrepareSpellScript(spell_cheep_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove whole exclude tanks and players in fly
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if([=](Player* target) { return target && target->HasAura(SPELL_FLIGHT_ACTION_BUTTON); });

            // if we have enough players without tanks and players in air
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_cheep_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_cheep_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Daedalian Wings 134339
class spell_daedalian_wings : public AuraScript
{
    PrepareAuraScript(spell_daedalian_wings);

    void OnApply(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetUnitOwner())
            SetStackAmount(owner->GetMap()->IsHeroic() ? 3 : 4);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_daedalian_wings::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// Gentle Yet Firm 139168
class spell_jikun_gentle_yet_firm : public AuraScript
{
    PrepareAuraScript(spell_jikun_gentle_yet_firm);

    void OnApply(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (Creature* jikun = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
                jikun->AI()->SetData(TYPE_SOFT_HANDS, 1);

            // Remove Daedalian Wings from catcher
            owner->RemoveAurasDueToSpell(SPELL_FLIGHT_ACTION_BUTTON);
        }
    }

    void OnRemove(AuraEffect const*  /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            if (Creature* jikun = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_JI_KUN) : ObjectGuid::Empty))
                jikun->AI()->SetData(TYPE_SOFT_HANDS, 0);
    }

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellInfo() && eventInfo.GetSpellInfo()->Id == SPELL_QUILLS_DAMAGE)
            return;

        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_jikun_gentle_yet_firm::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_jikun_gentle_yet_firm::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnProc += AuraProcFn(spell_jikun_gentle_yet_firm::HandleOnProc);
    }
};

// Pass 139183
class spell_jikun_pass : public SpellScript
{
    PrepareSpellScript(spell_jikun_pass);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(SPELL_GENTLE_YET_FIRM);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_jikun_pass::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// Dropped 139224
class spell_jikun_dropped : public SpellScript
{
    PrepareSpellScript(spell_jikun_dropped);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                if (caster->HasAura(SPELL_GENTLE_YET_FIRM))
                {
                    target->CastSpell(target, SPELL_GENTLE_YET_FIRM, true);
                    caster->RemoveAurasDueToSpell(SPELL_GENTLE_YET_FIRM);
                    caster->DespawnOrUnsummon();
                }
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_jikun_dropped::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_jikun_dropped::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// 587. Summoned by 134370 - Down Draft
class sat_down_draft : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && !triggering->ToPlayer()->HasAura(SPELL_FLIGHT_ACTION_BUTTON);
    }

    void OnTriggeringUpdate(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                if (target->IsAlive() && !target->HasForcedMovement())
                    target->SendApplyMovementForce(true, *caster, caster->GetMap()->IsHeroic() ? -12.5f : -10.0f);
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

// 800. Summoned by 138360 - Jump To Platform
class sat_jump_to_platform : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            target->CastSpell(target, SPELL_PREVENT_FALL_DAMAGE, true);
            target->CastSpell(jikunJumpPos.GetPositionX(), jikunJumpPos.GetPositionY(), jikunJumpPos.GetPositionZ(), SPELL_JUMP_TO_PLATFORM, true);
        }
    }
};

// 926. Summoned by 141014 - Exit Chamber
class sat_exit_chamber : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->NearTeleportTo(exitChamberJikun.GetPositionX(), exitChamberJikun.GetPositionY(), exitChamberJikun.GetPositionZ(), exitChamberJikun.GetOrientation());
    }
};

// Areatrigger 8897
class AreaTrigger_behind_jikun : public AreaTriggerScript
{
    public:
        AreaTrigger_behind_jikun() : AreaTriggerScript("AreaTrigger_behind_jikun") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->RemoveAurasDueToSpell(SPELL_PREVENT_FALL_DAMAGE);
            player->RemoveAurasDueToSpell(SPELL_FLIGHT_ACTION_BUTTON);
            player->RemoveAurasDueToSpell(SPELL_DAEDALUS_WINGS_ADDIT);
            player->RemoveAurasDueToSpell(SPELL_FLIGHT_OUTRO);
            return false;
        }
};

// Areatrigger 8848
class AreaTrigger_down_jikun : public AreaTriggerScript
{
    public:
        AreaTrigger_down_jikun() : AreaTriggerScript("AreaTrigger_down_jikun") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->IsOnVehicle())
                return true;

            player->RemoveAurasDueToSpell(SPELL_GENTLE_YET_FIRM);

            if (Creature* catcher = player->SummonCreature(NPC_FALL_CATCHER, *player, TEMPSUMMON_MANUAL_DESPAWN))
                player->CastSpell(catcher, SPELL_PLAYER_CATCH_FALL, true);

            return false;
        }
};

// Feather of Ji-Kun 216453
class go_feather_of_ji_kun : public GameObjectScript
{
    public:
        go_feather_of_ji_kun() : GameObjectScript("go_feather_of_ji_kun") { }
    
        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->HasAura(SPELL_LESSON_OF_ICARUS) || player->HasAura(SPELL_DAEDELIAN_WINGS))
                return false;
    
            player->CastSpell(player, SPELL_DAEDELIAN_WINGS, true);
            go->Delete();
            return true;
        }
};

void AddSC_boss_jikun()
{
    new boss_jikun();
    new creature_script<npc_egg_of_jikun>("npc_egg_of_jikun");
    new creature_script<npc_young_hatchling_jikun>("npc_young_hatchling_jikun");
    new creature_script<npc_juvenile>("npc_juvenile");
    new creature_script<npc_jikun_incubator>("npc_jikun_incubator");
    new creature_script<npc_jikun_feed>("npc_jikun_feed");
    new creature_script<npc_pool_of_feed_effect>("npc_pool_of_feed_effect");
    new creature_script<npc_pool_of_feed_hatchling_effect>("npc_pool_of_feed_hatchling_effect");
    new creature_script<npc_jump_to_boss_platform>("npc_jump_to_boss_platform");
    new creature_script<npc_fall_catcher_jikun>("npc_fall_catcher_jikun");
    new creature_script<npc_nest_guardian>("npc_nest_guardian");
    new creature_script<npc_jikun_golden_egg>("npc_jikun_golden_egg");

    new aura_script<spell_remove_stack_wings>("spell_remove_stack_wings");
    new aura_script<spell_incubated>("spell_incubated");
    new aura_script<spell_infected_talons>("spell_infected_talons");
    new spell_script<spell_caw>("spell_caw");
    new aura_script<spell_hatchling_eated_aura>("spell_hatchling_eated_aura");
    new spell_script<spell_hatchling_eated>("spell_hatchling_eated");
    new aura_script<spell_feed_pool_eff>("spell_feed_pool_eff");
    new aura_script<spell_quils_periodic_eff>("spell_quils_periodic_eff");
    new spell_script<spell_quils_visual>("spell_quils_visual");
    new aura_script<spell_incubate_visual>("spell_incubate_visual");
    new spell_script<spell_incubate_visual_eff>("spell_incubate_visual_eff");
    new spell_script<spell_regurgitate>("spell_regurgitate");
    new spell_script<spell_cheep_selector>("spell_cheep_selector");
    new aura_script<spell_daedalian_wings>("spell_daedalian_wings");
    new aura_script<spell_jikun_gentle_yet_firm>("spell_jikun_gentle_yet_firm");
    new spell_script<spell_jikun_pass>("spell_jikun_pass");
    new spell_script<spell_jikun_dropped>("spell_jikun_dropped");
    new atrigger_script<sat_down_draft>("sat_down_draft");
    new atrigger_script<sat_jump_to_platform>("sat_jump_to_platform");
    new atrigger_script<sat_exit_chamber>("sat_exit_chamber");
    new AreaTrigger_behind_jikun();
    new AreaTrigger_down_jikun();
    new go_feather_of_ji_kun();
}