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

#include "ScriptedCreature.h"
#include "siege_of_orgrimmar.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"

enum Spells
{
    SPELL_COSMETIC_CHANNEL          = 147449,
    SPELL_BUBLE_SHIELD              = 147333,
    SPELL_WATER_BOLT                = 147398,
    SPELL_CORRUPTED_WATER           = 147351,
    SPELL_RUSHING_WATERS            = 147185,
    SPELL_RUSHING_WATERS_EFF        = 147213,
    SPELL_VORTEX                    = 147305,
    SPELL_VORTEX_SELECTOR           = 147304,
    SPELL_VORTEX_SUMM               = 147515,
    SPELL_SWIRL_ZONE_NE             = 147181,
    SPELL_SWIRL_ZONE_N              = 147178,
    SPELL_SWIRL_ZONE_NW             = 147182,
    SPELL_SWIRL_ZONE_SW             = 147191,
    SPELL_SWIRL_ZONE_SE             = 147189,
    SPELL_SELF_ABSORBED             = 145861,
    SPELL_GROWING_PRIDE             = 145818,
    SPELL_SHA_NOVA                  = 145989,
    SPELL_SHA_SMASH                 = 145944,
    SPELL_CORRUPTED_TOUCH           = 149207,
    SPELL_CORRUPTED_TOUCH_EFF       = 149209,
    SPELL_PRESSURE_PLATE_OVERRIDE   = 147639,
    SPELL_MIGHTY_STOMP_JUMP         = 147638,
    SPELL_MIGHTY_STOMP_EFF          = 147641,
    SPELL_AMBER_BLAST               = 146511,
    SPELL_AMBER_GROWTH              = 146501,
    SPELL_RESONATING_AMBER_SELECTOR = 146499,
    SPELL_RESONATING_AMBER_SUMM     = 146455,
    SPELL_RESONATING_AMBER_AT       = 146454,
    SPELL_RESONATING_AMBER_EFF      = 146452,
    SPELL_FRENZIED_ASSAULT          = 146536,
    SPELL_PIERCE                    = 146556,
    SPELL_MIGHTY_CLEAVE             = 146605,
    SPELL_GROUND_SLAM               = 146621,
    SPELL_POISON_BLAST              = 146606,
    SPELL_CHARGE                    = 22120,
    SPELL_MORTAL_STRIKE             = 15708,
    SPELL_THREATINING_SHOUT         = 78710,
    SPELL_EXECUTE                   = 148918,
    SPELL_HEALING_TIDE_TOTEM        = 146722,
    SPELL_CHAIN_HEAL                = 146728,
    SPELL_TIDAL_WAVE                = 149187,
    SPELL_TIDAL_WAVE_EFF            = 149188,
    SPELL_SHATTERING_ROAR           = 146897,
    SPELL_FRACTURE                  = 147200,
    SPELL_FRACTURE_EFF              = 147202,
    SPELL_THROW_AXE                 = 147669,
    SPELL_SPIKE_MINE_AT             = 145740,
    SPELL_SPIKE_MINE_DETONATION     = 145752,
    SPELL_VANITY                    = 145942,
    SPELL_TELEPORT_TO_ORGRIMMAR     = 148032,
    SPELL_TELEPORT_TO_YSHAARJ_VAULT = 149407,
    SPELL_SNIPED                    = 146743,
    SPELL_BLADE_SALVO               = 148963,
    SPELL_BLADE_SALVO_EFF           = 148962,
    SPELL_BLIND_STRIKE              = 141189,
    SPELL_BLIND_CLEAVE              = 141192,
    SPELL_LAVA_BURST                = 142338,
    SPELL_ELECTRIFIED_GROUND        = 145547,
    SPELL_POISON_BOLT_TOTEM         = 145549,
    SPELL_POISON_BOLT               = 145548,
    SPELL_CLEAVE                    = 15284,
    SPELL_CHARGE_S                  = 22120,
    SPELL_ENRAGE                    = 145554,
    SPELL_COIN_TOSS                 = 145552,
    SPELL_BRIBE                     = 145553,
    SPELL_RESISTANCE_TOTEM          = 145729,
    SPELL_RESISTANCE_TOTEM_EFF      = 145730,
    SPELL_SIT                       = 149278,
    SPELL_SHADOWFLAME               = 145551,
    SPELL_MIND_SPIKE                = 145550,
    SPELL_REND                      = 140276,
    SPELL_BONECRACKER               = 145568,
    SPELL_HURL_BOULDER              = 145569,
    SPELL_CONTROL_POWERFUL_DEMON    = 145646,
    SPELL_SHADOW_BOLT_VOLLEY        = 145658,
    SPELL_CURSE_OF_TONGUES          = 15470,
    SPELL_CRIPPLE                   = 11443,
    SPELL_KORKRON_COSMETIC_DEAD     = 146587,
    SPELL_SKULLSPLITTER_RAGE        = 145937,
    SPELL_FIRESTORM                 = 146050,
    SPELL_FIRESTORM_SELECTOR        = 146053,
    SPELL_FIRESTORM_MISSLE          = 146055,
    SPELL_BLAZE_OF_GLORY            = 146038,
    SPELL_BLAZE_OF_GLORY_EFF        = 146039,
    SPELL_BALE_STRIKE               = 147447,
    SPELL_DARK_FURY                 = 145995,
    SPELL_UMBRAL_TEMPEST            = 146163,
    SPELL_UMBRAL_TEMPEST_SELECTOR   = 146167,
    SPELL_UMBRAL_TEMPEST_EFF        = 146170,
    SPELL_DREAD_HOLW                = 146002,
    SPELL_REND_2                    = 146927,
    SPELL_ANNIHILATION              = 146504,
    SPELL_ANNIHILATION_EFF          = 146503,
    SPELL_SCORCHED_EARTH            = 146223,
    SPELL_SCORCHED_EARTH_MISSLE     = 146221,
    SPELL_BATTLE_BOUND              = 145943,
    SPELL_SERRATED_RAMPAGE          = 145907,
    SPELL_SERRATED_RAMPAGE_EFF      = 145908,
    SPELL_ARCING_BLAST              = 146607,
    SPELL_ARCING_BLAST_EFF          = 146609,
    SPELL_SERRATED_CLEAVE           = 146995,
    SPELL_ACID_SPRAY_SELECTOR       = 146975,
    SPELL_ACID_SPRAY_MISSLE         = 146977,
    SPELL_DETERIORATE               = 145999,
    SPELL_INFUSION                  = 147884,
    SPELL_INFUSION_MISSLE           = 145120,
    SPELL_INFUSION_AT               = 147883,
    SPELL_INFUSION_EFF              = 147882,
    SPELL_SLOW_AND_STEADY           = 146908,
    SPELL_CALL_REINFORCEMENTS       = 145119,
    SPELL_MOLTEN_SLUG               = 146028,
    SPELL_STRENGTH_IN_NUMBERS       = 148795,
    SPELL_STRENGTH_IN_NUMBERS_EFF   = 148741,
    SPELL_OBLITERATE                = 148785,
    SPELL_SHREDDING_BLAST_SELECTOR  = 148572,
    SPELL_SHREDDING_BLAST_MISSLE    = 148575,
    SPELL_SHREDDING_BLAST_EFF       = 148576,
    SPELL_SIEGECRAFTER_SCENE_HORDE  = 148571,
    SPELL_SIEGECRAFTER_SCENE_ALLIAN = 147094,
    SPELL_VIVISECTION               = 146534,
    SPELL_OBLITERATING_STRIKE       = 146537,
    SPELL_FAN_OF_KNIVES             = 148140,
    SPELL_ENVENOMED_SHADOWSTEP      = 148141,
    SPELL_ADRENALINE_BOMB           = 147868,
    SPELL_ADRENALINE_BOMB_EFF       = 147877,
    SPELL_THROW_DYNAMITE            = 148138,
    SPELL_HEALING_SPRAY             = 148085,
    SPELL_DRILLSTORM                = 146479,
    SPELL_DRILLSCHARGE              = 146456,
    SPELL_DRILLCHARGE_AT            = 146460,
    SPELL_DRILLCHARGE_EFF           = 146459,
    SPELL_PIPE_TRANSFERT_UNDERHOLD  = 145238,
    SPELL_LASER_TURRET_BUNNY        = 143838,
    SPELL_CALL_OF_THE_ELEMENTS      = 147815,
    SPELL_ATTACK_GARROSH            = 147892,
    SPELL_EXHAUSTED                 = 147861,
    SPELL_ATTACK_THRALL             = 147831,
    SPELL_SUMMON_BLOOD_OF_YSHAARJ   = 147566,
    SPELL_SUMM_BLOOD_OF_YSHAARJ_EFF = 147567,
    SPELL_BLOOD_OF_YSHAARJ          = 147437,
    SPELL_BLOOD_OF_YSHAARJ_EFF      = 147554,
    SPELL_EMPOWERING_CORRUPTION     = 147564,
    SPELL_YSHAARJ_TORCHED           = 147553,
    SPELL_GRASP_OF_YSHAARJ          = 147647,
    SPELL_REAPER_CHARGE             = 147640,
    SPELL_REAPER                    = 147644,
    SPELL_REAPING_WHIRLWIND         = 147642,
    SPELL_UNLOCK_CHAINS             = 145671,
    SPELL_GAMON_CHAIN_1             = 145669,
    SPELL_GAMON_CHAIN_2             = 145670,
    SPELL_GAMON_COSMETIC_STATE      = 145662,
    SPELL_ALCHEMICAL_MASTERY        = 148451,
    SPELL_ALCHEMICAL_MASTERY_EFF    = 148456,
    SPELL_DANCING_FLAMES            = 120348,
    SPELL_LACERATING_BEAT           = 148136,
    SPELL_CRUSHING_SWIPE            = 148133,
    SPELL_WRECKING_BALL             = 147952,
    SPELL_WRECKING_BALL_BREAK       = 147906,
    SPELL_WRECKING_BALL_AT          = 147609,
    SPELL_MELEE                     = 149356,
    SPELL_KILL_JAILERS              = 147994,
    SPELL_SHA_CLOUD_JUMP            = 145591,
    SPELL_GOBLIN_COSMETIC_DEAD      = 145600,
    SPELL_SLAVE_CHAINS              = 145744,
    SPELL_YSHAARJ_CORRUPTION        = 142848, // achiev
};

enum Events
{
    EVENT_BUBLE_SHIELD = 1,
    EVENT_WATER_BOLT,
    EVENT_CORRUPTED_WATER,
    EVENT_RUSHING_WATERS,
    EVENT_VORTEX,
    EVENT_SELF_ABSORBED,
    EVENT_GROWING_PRIDE,
    EVENT_SHA_NOVA,
    EVENT_SHA_SMASH,
    EVENT_AMBER_BLAST,
    EVENT_RESONATING_AMBER,
    EVENT_FRENZIED_ASSAULT,
    EVENT_PIERCE,
    EVENT_MIGHTY_CLEAVE,
    EVENT_GROUND_SLAM,
    EVENT_POISON_BLAST,
    EVENT_KOMRAK_CHARGE,
    EVENT_MORTAL_STRIKE,
    EVENT_THREATENING_SHOUT,
    EVENT_HEALING_TIDE_TOTEM,
    EVENT_CHAIN_HEAL,
    EVENT_TIDAL_WAVE,
    EVENT_SHATTERING_ROAR,
    EVENT_FRACTURE,
    EVENT_THROW_AXE,
    EVENT_BLIND_STRIKE,
    EVENT_BLIND_CLEAVE,
    EVENT_LAVA_BURST,
    EVENT_ELECTRIFIED_GROUND,
    EVENT_POISON_BOLT_TOTEM,
    EVENT_POISON_BOLT,
    EVENT_CLEAVE,
    EVENT_CHARGE_S,
    EVENT_ENRAGE,
    EVENT_COIN_TOSS,
    EVENT_BRIBE,
    EVENT_SHADOWFLAME,
    EVENT_MIND_SPIKE,
    EVENT_REND,
    EVENT_BONECRACKER,
    EVENT_HURL_BOULDER,
    EVENT_SHADOW_BOLT_VOLLEY,
    EVENT_CURSE_OF_TONGUE,
    EVENT_CRIPPLE,
    EVENT_SKULLSPLITTER_RAGE,
    EVENT_BLAZE_OF_GLORY,
    EVENT_FIRE_STORM,
    EVENT_UMBRAL_TEMPEST,
    EVENT_BALE_STRIKE,
    EVENT_DARK_FURY,
    EVENT_DREAD_HOWL,
    EVENT_SCORCHED_EARTH,
    EVENT_SERRATED_RAMPAGE,
    EVENT_ARCING_BLAST,
    EVENT_SERRATED_CLEAVE,
    EVENT_ACID_SPRAY,
    EVENT_DETERIORATE,
    EVENT_INFUSION,
    EVENT_SLOW_AND_STEADY,
    EVENT_CALL_REINFORCEMENTS,
    EVENT_MOLTEN_SLUG,
    EVENT_OBLITERATE,
    EVENT_SHREDDING_BLAST,
    EVENT_VIVISECTION,
    EVENT_OBLITERATING_STRIKE,
    EVENT_ENVENOMED_SHADOWSTEP,
    EVENT_FAN_OF_KNIVES,
    EVENT_ADRENALINE_BOMB,
    EVENT_THROW_DYNAMITE,
    EVENT_HEALING_SPRAY,
    EVENT_DRILLSTORM,
    EVENT_DRILLCHARGE,
    EVENT_LASER_BEAM,
    EVENT_YSHAARJ_TOURCHED,
    EVENT_GRASP_OF_YSHAARJ,
    EVENT_REAPER,
    EVENT_REAPER_CHARGE,
    EVENT_ALCHEMICAL_MASTERY,
    EVENT_LACERATING_BEAT,
    EVENT_CRUSHING_SWIPE,
};

enum Yells
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
    TALK_SPECIAL_3,
    TALK_SPECIAL_4,
    TALK_SPECIAL_5,
    TALK_SPECIAL_6,
    TALK_SPECIAL_7,
    TALK_SPECIAL_8,
    TALK_SPECIAL_9,
    TALK_SPECIAL_10,
    TALK_SPECIAL_11,
    TALK_SPECIAL_12,
    TALK_SPECIAL_13,
    TALK_SPECIAL_14,
    TALK_SPECIAL_15,
    TALK_SPECIAL_16,
};

const std::map<uint32, uint32> invCageType =
{
    { GO_KORKRON_CAGE,   NPC_DOYODA            },
    { GO_KORKRON_CAGE_2, NPC_WILDHUMMER_SHAMAN },
};

// Ooze Controller 73222
struct npc_siege_of_orgrimmar_ooze_controller : public customCreatureAI
{
    npc_siege_of_orgrimmar_ooze_controller(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    bool hasTriggered;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();
        hasTriggered = false;

        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            if (instance && instance->GetBossState(DATA_IMMERSEUS) == DONE)
                return;

            me->SummonCreature(urand(0, 1) ? NPC_LESSER_SHA_PUDDLE : NPC_LESSER_CONTAMINATED_PUDDLE, me->GetPositionX(), me->GetPositionY(), 245.92f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

            context.Repeat(Seconds(8), Seconds(10));
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;
            scheduler.CancelAll();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Lesser Sha Puddle 73197, 73260
struct npc_siege_of_orgrimmar_lesser_sha_puddle : public customCreatureAI
{
    npc_siege_of_orgrimmar_lesser_sha_puddle(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        if (me->GetEntry() == NPC_LESSER_CONTAMINATED_PUDDLE)
        {
            me->setRegeneratingHealth(false);
            me->SetHealth((uint32)(me->GetMaxHealth() * 0.1f));
        }

        if (me->GetDBTableGUIDLow())
            return;

        me->GetMotionMaster()->MovePoint(0, immerseusPuddleEvent);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Tormented Initiate 73349
struct npc_tormented_initiate : public customCreatureAI
{
    npc_tormented_initiate(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();

        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (me->GetPositionZ() < 250.0f)
            {
                if (Creature* poolTender = me->FindNearestCreature(NPC_FALLEN_POOL_TENDER, 20.0f, true))
                    DoCast(poolTender, SPELL_COSMETIC_CHANNEL);
                return;
            }

            if (Creature* waterStalker = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_INVISIBLE_STALKER) : ObjectGuid::Empty))
                if (me->GetExactDist2d(waterStalker) < 70.0f)
                    DoCast(waterStalker, SPELL_COSMETIC_CHANNEL);
        });
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_TORMENTED_INITIATE, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_FALLEN_POOL_TENDER, 40.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        // hacky yes, but not way without npc groups
        if (me->GetDBTableGUIDLow() == 559524 || me->GetDBTableGUIDLow() == 559550)
            Talk(TALK_INTRO);

        me->CallForHelp(28.0f);
        me->InterruptNonMeleeSpells(true, SPELL_COSMETIC_CHANNEL);
        events.ScheduleEvent(EVENT_BUBLE_SHIELD, urand(10 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_BUBLE_SHIELD, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS), EVENT_BUBLE_SHIELD, eventId, PRIORITY_ALLY);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Fallen Pool Tender 73342
struct npc_fallen_pool_tender : public customCreatureAI
{
    npc_fallen_pool_tender(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        isCaster = true;
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
        events.ScheduleEvent(EVENT_BUBLE_SHIELD, urand(1.5 * IN_MILLISECONDS, 2 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_CORRUPTED_WATER, 10 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_WATER_BOLT, urand(3.1 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS), EVENT_WATER_BOLT, eventId);
            ExecuteTargetEvent(SPELL_CORRUPTED_WATER, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_CORRUPTED_WATER, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Aqueous Defender 73191
struct npc_aqueous_defender : public customCreatureAI
{
    npc_aqueous_defender(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_VORTEX, urand(4.5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_RUSHING_WATERS, 11 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->CastSpell(summon, SPELL_VORTEX, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_AQUEOUS_DEFENDERS_FALLEN_COUNT) < 3)
            me->GetInstanceScript()->SetData(DATA_AQUEOUS_DEFENDERS_FALLEN_COUNT, 1);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_VORTEX_SELECTOR, urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS), EVENT_VORTEX, eventId);
            ExecuteTargetEvent(SPELL_RUSHING_WATERS, 15 * IN_MILLISECONDS, EVENT_RUSHING_WATERS, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Lorewalker Cho 73330
struct npc_siege_of_orgrimmar_lorewalker_cho : public customCreatureAI
{
    npc_siege_of_orgrimmar_lorewalker_cho(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    uint32 delay;
    bool hasTriggered;
    bool hasTormented;

    void Reset() override
    {
        instance = me->GetInstanceScript();
        delay = 0;
        hasTriggered = false;
        hasTormented = false;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        DoAction(invChoEventsType.find(summoner->GetEntry())->second);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                if (hasTriggered)
                    break;

                hasTriggered = true;

                delay = 2000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 11900), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });
                break;
            case ACTION_UNDER_TORMENTED:
                if (hasTormented)
                    break;

                hasTormented = true;

                me->GetMotionMaster()->MovePoint(0, lorewalkerUpWaters);

                delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 10700), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 13300), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_4);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_5);
                });
                break;
            case ACTION_IMMERSIUS_DEFEAT:
                me->GetMotionMaster()->MovePoint(0, choPastImmerseusEvent[1]);

                delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_6);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7800), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_7);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_8);
                });
                break;
            case ACTION_SCARRED_VALE:
            {
                Movement::MoveSplineInit init(me);
                for (auto itr : choScarredValePath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                init.SetUncompressed();
                init.Launch();

                delay = me->GetSplineDuration();
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_9);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_10);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7900), [this](TaskContext context)
                {
                    if (Creature* rook = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROOK_STONETOE) : ObjectGuid::Empty))
                        rook->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6900), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_11);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7600), [this](TaskContext context)
                {
                    if (Creature* rook = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROOK_STONETOE) : ObjectGuid::Empty))
                        rook->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 9300), [this](TaskContext context)
                {
                    if (Creature* sun = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SUN_TENDERHEART) : ObjectGuid::Empty))
                        sun->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6200), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_12);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 12500), [this](TaskContext context)
                {
                    if (Creature* rook = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROOK_STONETOE) : ObjectGuid::Empty))
                        rook->AI()->Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4400), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_13);
                });
                break;
            }
            case ACTION_CHO_PAST_PROTECTORS:
                Talk(TALK_SPECIAL_14);

                scheduler
                    .Schedule(Milliseconds(5500), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_15);
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Lorewalker Cho 72872
struct npc_siege_of_orgrimmar_lorewalker_cho_norushen : public customCreatureAI
{
    npc_siege_of_orgrimmar_lorewalker_cho_norushen(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    uint32 delay;
    ObjectGuid summonerGUID;

    void Reset() override
    {
        instance = me->GetInstanceScript();
        delay = 0;
    }

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();

        Talk(TALK_INTRO);
        me->GetMotionMaster()->MovePoint(0, choPreNorushenEvent[1]);
        delay = 6300;

        scheduler
            .Schedule(Milliseconds(delay), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_1);
        });

        scheduler
            .Schedule(Milliseconds(delay += 7000), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_2);
        });

        scheduler
            .Schedule(Milliseconds(delay += 7300), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
                norushen->AI()->Talk(TALK_INTRO);
        });

        scheduler
            .Schedule(Milliseconds(delay += 2300), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_3);
        });

        scheduler
            .Schedule(Milliseconds(delay += 6600), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
                norushen->AI()->Talk(TALK_SPECIAL_1);
        });

        scheduler
            .Schedule(Milliseconds(delay += 6600), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_4);
        });

        scheduler
            .Schedule(Milliseconds(delay += 8300), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
                norushen->AI()->Talk(TALK_SPECIAL_2);
        });

        scheduler
            .Schedule(Milliseconds(delay += 5100), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_5);
        });

        scheduler
            .Schedule(Milliseconds(delay += 3800), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
                norushen->AI()->Talk(TALK_SPECIAL_3);
        });

        scheduler
            .Schedule(Milliseconds(delay += 13500), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
                norushen->AI()->Talk(TALK_SPECIAL_4);
        });

        scheduler
            .Schedule(Milliseconds(delay += 8400), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
                norushen->AI()->Talk(TALK_SPECIAL_5);
        });

        scheduler
            .Schedule(Milliseconds(delay += 7700), [this](TaskContext context)
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*me, summonerGUID))
            {
                norushen->AI()->Talk(TALK_SPECIAL_6);
                norushen->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            me->GetMotionMaster()->MovePoint(0, choPreNorushenEvent[0]);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI));
            });
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Vanity 72662
struct npc_vanity : public customCreatureAI
{
    npc_vanity(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SELF_ABSORBED, 9 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* zeal = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_ZEAL) : ObjectGuid::Empty))
            zeal->AI()->DoAction(ACTION_START_INTRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SELF_ABSORBED, 10.5 * IN_MILLISECONDS, EVENT_SELF_ABSORBED, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Fragment of Pride 72655
struct npc_fragment_of_pride : public customCreatureAI
{
    npc_fragment_of_pride(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_GROWING_PRIDE, urand(5.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_GROWING_PRIDE, urand(8 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_GROWING_PRIDE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Amalgamated Hubris 72658
struct npc_amalgamated_hubris : public customCreatureAI
{
    npc_amalgamated_hubris(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHA_NOVA, 8.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SHA_NOVA, 12 * IN_MILLISECONDS, EVENT_SHA_NOVA, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Zeal 72661
struct npc_zeal : public customCreatureAI
{
    npc_zeal(Creature* creature) : customCreatureAI(creature) 
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        hasVanityNotAlive = false;
    }

    bool hasVanityNotAlive;

    void Reset() override
    {
        events.Reset();

        if (hasVanityNotAlive)
            me->RemoveAurasDueToSpell(SPELL_VANITY);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (!hasVanityNotAlive)
            events.ScheduleEvent(EVENT_SELF_ABSORBED, 9 * IN_MILLISECONDS);

        events.ScheduleEvent(EVENT_SHA_SMASH, 6 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasVanityNotAlive)
        {
            hasVanityNotAlive = true;
            me->RemoveAurasDueToSpell(SPELL_VANITY);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_NORUSHEN_PRE_EVENT_1, DONE);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SELF_ABSORBED, 10.5 * IN_MILLISECONDS, EVENT_SELF_ABSORBED, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_SHA_SMASH, urand(14 * IN_MILLISECONDS, 16 * IN_MILLISECONDS), EVENT_SHA_SMASH, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Lingering Corruption 72791
struct npc_lingering_corruption : public customCreatureAI
{
    npc_lingering_corruption(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    Position pos;

    void Reset() override
    {
        scheduler.CancelAll();
        me->SetSpeed(MOVE_RUN, 0.9f);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            pos = me->GetRandomPoint({ me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation() }, 5.5f);
            me->GetMotionMaster()->MovePoint(0, pos);
            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->GetMotionMaster()->Clear();
        scheduler.CancelAll();
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_CORRUPTED_TOUCH);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Pressure Plate 73124
struct npc_pressure_plate : public customCreatureAI
{
    npc_pressure_plate(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_FALLEN_PROTECTORS_PRE_EVENT) != DONE)
            DoCast(me, SPELL_PRESSURE_PLATE);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Portal to Orgrimmar 73536
struct npc_siege_of_orgrimmar_portal_to_galakras : public customCreatureAI
{
    npc_siege_of_orgrimmar_portal_to_galakras(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->SetVisible(false);

        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SHA_OF_PRIDE_PAST_EVENT) == DONE)
                me->SetVisible(true);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Lady Jaina Proudmoore 73598
struct npc_lady_jaina_proudmoore_sha_room : public customCreatureAI
{
    npc_lady_jaina_proudmoore_sha_room(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    float x, y;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetVisible(true);
            me->SetWalk(true);
            GetPositionWithDistInOrientation(me, 85.0f, me->GetOrientation(), x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, 356.07f, 0.0f);

            if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LORTHEMAR_PRIDE_ROOM) : ObjectGuid::Empty))
            {
                lorthemar->SetVisible(true);
                lorthemar->SetWalk(true);
                GetPositionWithDistInOrientation(lorthemar, 85.0f, lorthemar->GetOrientation(), x, y);
                lorthemar->GetMotionMaster()->MovePoint(0, x, y, 356.07f, 0.0f);
            }

            uint32 delay = 5000;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 10200), [this](TaskContext context)
            {
                if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LORTHEMAR_PRIDE_ROOM) : ObjectGuid::Empty))
                    lorthemar->AI()->Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 4100), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 2600), [this](TaskContext context)
            {
                if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LORTHEMAR_PRIDE_ROOM) : ObjectGuid::Empty))
                    lorthemar->AI()->Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 3800), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });

            scheduler
                .Schedule(Milliseconds(delay += 7200), [this](TaskContext context)
            {
                if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LORTHEMAR_PRIDE_ROOM) : ObjectGuid::Empty))
                    lorthemar->AI()->Talk(TALK_SPECIAL_2);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6400), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_3);
            });

            scheduler
                .Schedule(Milliseconds(delay += 12000), [this](TaskContext context)
            {
                if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LORTHEMAR_PRIDE_ROOM) : ObjectGuid::Empty))
                    lorthemar->AI()->Talk(TALK_SPECIAL_3);
            });

            bool isLfr = me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_LFR);
            scheduler
                .Schedule(Milliseconds(delay += 5000), [this, isLfr](TaskContext context)
            {
                Talk(TALK_SPECIAL_4);

                me->GetMotionMaster()->MovePoint(0, jainaLorthemarShaRoomPortals[0]);

                if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LORTHEMAR_PRIDE_ROOM) : ObjectGuid::Empty))
                    lorthemar->GetMotionMaster()->MovePoint(0, jainaLorthemarShaRoomPortals[1]);

                if (!isLfr)
                {
                    // Unlock Portals
                    std::list<Creature*> portalsList;
                    GetCreatureListWithEntryInGrid(portalsList, me, NPC_PORTAL_TO_ORGRIMMAR, 200.0f);

                    for (auto&& itr : portalsList)
                        itr->SetVisible(true);
                }
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Dragonmaw Tidal Shaman 72367
struct npc_dragonmaw_tidal_shaman : public customCreatureAI
{
    npc_dragonmaw_tidal_shaman(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_TIDAL_WAVE, 12 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CHAIN_HEAL, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HEALING_TIDE_TOTEM, 9 * IN_MILLISECONDS);
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_TIDAL_SHAMAN, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_CANNONEER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_BONECRUSHER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_ELITE_GRUNT, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_FLAGBEATER, 80.0f);
        tmpTargets.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_HEALING_TIDE_TOTEM, 12.5 * IN_MILLISECONDS, EVENT_HEALING_TIDE_TOTEM, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CHAIN_HEAL, urand(10.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_CHAIN_HEAL, eventId, PRIORITY_ALLY);
            ExecuteTargetEvent(SPELL_TIDAL_WAVE, 12 * IN_MILLISECONDS, EVENT_TIDAL_WAVE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dragonmaw Bonecrusher 72354
struct npc_dragonmaw_bonecrusher : public customCreatureAI
{
    npc_dragonmaw_bonecrusher(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHATTERING_ROAR, 6 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FRACTURE, 8 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SHATTERING_ROAR, 10 * IN_MILLISECONDS, EVENT_SHATTERING_ROAR, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_FRACTURE_EFF, urand(9 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_FRACTURE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dragonmaw Elite Grunt 72350
struct npc_dragonmaw_elite_grunt : public customCreatureAI
{
    npc_dragonmaw_elite_grunt(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_THROW_AXE, urand(2.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_THROW_AXE, urand(4 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_THROW_AXE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Pressure Mine 72910
struct npc_pressure_mine : public customCreatureAI
{
    npc_pressure_mine(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        DoCast(me, SPELL_SPIKE_MINE_AT);
    }
};

// Korkron Elite Sniper 72261
struct npc_siege_of_orgrimmar_korkron_elite_sniper : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_elite_sniper(Creature* creature) : customCreatureAI(creature) { }

    void AttackStart(Unit* victim) override { } // ignore any hostile
};

// Voljin 73942
struct npc_siege_of_orgrimmar_voljin : public customCreatureAI
{
    npc_siege_of_orgrimmar_voljin(Creature* creature) : customCreatureAI(creature)
    {
        hasPreEventInit = false;
        hasCombatInit   = false;
        darkSpearWarriorsList.clear();
    }

    TaskScheduler scheduler;
    std::list<Creature*> darkSpearWarriorsList;
    bool hasPreEventInit;
    bool hasCombatInit;
    uint32 delay;

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
            {
                if (hasPreEventInit)
                    break;
                
                hasPreEventInit = true;
                
                // Warrios Leader (hasn`t any way to filter him)
                std::list<Creature*> tempLeaderList;
                GetCreatureListWithEntryInGrid(darkSpearWarriorsList, me, NPC_DARKSPEAR_WARRIOR, 70.0f);
                tempLeaderList = darkSpearWarriorsList;

                tempLeaderList.remove_if([=](Creature* target) { return target && target->GetDBTableGUIDLow() != 560041; });

                if (!tempLeaderList.empty())
                    if (Creature* warriorLeader = tempLeaderList.front())
                        warriorLeader->AI()->Talk(TALK_INTRO);

                delay = 1500;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    for (auto&& itr : darkSpearWarriorsList)
                        itr->HandleEmoteStateCommand(EMOTE_STATE_EXCLAIM);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                {
                    for (auto&& itr : darkSpearWarriorsList)
                    {
                        itr->HandleEmoteStateCommand(EMOTE_STATE_POINT);

                        Movement::MoveSplineInit init(itr);
                        for (auto itr : blackSpearWarriorsSuicide)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-4.5f, 4.5f), itr.GetPositionY() + frand(-4.5f, 4.5f), itr.GetPositionZ()));

                        init.SetUncompressed();
                        init.Launch();
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
                {
                    for (auto&& itr : darkSpearWarriorsList)
                        itr->Kill(itr, true);

                    if (Creature* baine = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BAINE_BLOODHOOF) : ObjectGuid::Empty))
                        baine->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5500), [this](TaskContext context)
                {
                    if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NAZGRIM_PRE_EVENT) : ObjectGuid::Empty))
                        nazgrim->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 9000), [this](TaskContext context)
                {
                    Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 10500), [this](TaskContext context)
                {
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BAINE_BLOODHOOF) : ObjectGuid::Empty))
                        baine->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7000), [this](TaskContext context)
                {
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BAINE_BLOODHOOF) : ObjectGuid::Empty))
                        baine->AI()->Talk(TALK_SPECIAL_2);
                });
                break;
            }
            case ACTION_IRON_JUGGERNAUT_COMBAT_INIT:
                if (hasCombatInit)
                    break;
            
                hasCombatInit = true;

                Talk(TALK_SPECIAL_1);
                me->GetMotionMaster()->MovePoint(0, blackSpearLeadersPos[0]);

                if (Creature* baine = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BAINE_BLOODHOOF) : ObjectGuid::Empty))
                    baine->GetMotionMaster()->MovePoint(0, blackSpearLeadersPos[1]);

                scheduler
                    .Schedule(Milliseconds(4500), [this](TaskContext context)
                {
                    if (Creature* baine = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BAINE_BLOODHOOF) : ObjectGuid::Empty))
                        baine->AI()->Talk(TALK_SPECIAL_3);
                });

                break;
            case ACTION_IRON_JUGGERNAUT_DEFEAT:
                Talk(TALK_SPECIAL_2);
                me->GetMotionMaster()->MovePoint(0, blackSpearLeadersPosDefeat[0]);

                if (Creature* baine = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_BAINE_BLOODHOOF) : ObjectGuid::Empty))
                    baine->GetMotionMaster()->MovePoint(0, blackSpearLeadersPosDefeat[1]);

                delay = 2500;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NAZGRIM_PRE_EVENT) : ObjectGuid::Empty))
                        nazgrim->AI()->Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8000), [this](TaskContext context)
                {
                    if (Creature* tyrande = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TYRANDE) : ObjectGuid::Empty))
                        tyrande->AI()->DoAction(ACTION_START_INTRO);
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Tyrande 73962
struct npc_siege_of_orgrimmar_tyrande : public customCreatureAI
{
    npc_siege_of_orgrimmar_tyrande(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    float x, y;
    uint32 delay;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            Talk(TALK_INTRO);

            // send sentinels jump forward and launch Glawe Thower
            std::list<Creature*> sentinelsList;
            GetCreatureListWithEntryInGrid(sentinelsList, me, NPC_NIGHT_ELF_SENTINEL, 200.0f);

            for (auto&& itr : sentinelsList)
            {
                GetPositionWithDistInOrientation(itr, 41.0f, itr->GetOrientation(), x, y);
                itr->GetMotionMaster()->MoveJump(x, y, 26.03f, 20.0f, 15.0f);
                itr->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
                itr->SetHomePosition(x, y, 26.03f, 4.88f);
            }

            GetPositionWithDistInOrientation(me, 41.0f, me->GetOrientation(), x, y);
            me->GetMotionMaster()->MoveJump(x, y, 26.03f, 20.0f, 15.0f);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            me->SetHomePosition(x, y, 26.03f, 4.88f);

            if (Creature* glaiwe = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GLAIWE_THROWER) : ObjectGuid::Empty))
                glaiwe->CastSpell(glaiwe, SPELL_BLADE_SALVO);

            delay = 6000;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_NAZGRIM_PRE_EVENT) : ObjectGuid::Empty))
                {
                    nazgrim->AI()->Talk(TALK_SPECIAL_2);
                    nazgrim->DespawnOrUnsummon(6 * IN_MILLISECONDS);
                }
            });

            scheduler
                .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);

                if (me->GetInstanceScript() && me->GetInstanceScript()->GetData64(GO_THE_GATES_OF_ORGRIMMAR))
                    me->GetInstanceScript()->HandleGameObject(me->GetInstanceScript()->GetGuidData(GO_THE_GATES_OF_ORGRIMMAR), true, NULL);
            });

            scheduler
                .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
            {
                // Call Korkron Here
                std::list<Creature*> korkronsList;
                GetCreatureListWithEntryInGrid(korkronsList, me, NPC_KORKRON_GRUNT, 200.0f);

                for (auto&& itr : korkronsList)
                {
                    GetPositionWithDistInOrientation(itr, 50.0f, itr->GetOrientation(), x, y);
                    itr->GetMotionMaster()->MovePoint(0, x, y, 26.03f, 0.0f);
                }
            });

            scheduler
                .Schedule(Milliseconds(delay += 3400), [this](TaskContext context)
            {
                if (Creature* voljin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                    voljin->AI()->Talk(TALK_SPECIAL_3);
            });

            scheduler
                .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Korkron Grunt 73964
struct npc_siege_of_orgrimmar_korkron_grunt_gates : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_grunt_gates(Creature* creature) : customCreatureAI(creature) { }

    void AttackStart(Unit* target) override
    {
        if (!target || target->GetEntry() != NPC_NIGHT_ELF_SENTINEL)
            return;

        me->Attack(target, true);
        me->GetMotionMaster()->MoveChase(target, 0.0f, me->GetAngle(target));
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        me->SetHomePosition(*me);

        if (Creature* nightElf = me->FindNearestCreature(NPC_NIGHT_ELF_SENTINEL, 70.0f, true))
            AttackStart(nightElf);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Aysa Cloudsinger 72438
struct npc_siege_of_orgrimmar_aysa_cloudsinger : public customCreatureAI
{
    npc_siege_of_orgrimmar_aysa_cloudsinger(Creature* creature) : customCreatureAI(creature) 
    {
        hasTriggered      = false;
        hasKomakTriggered = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;
    bool hasKomakTriggered;

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            Movement::MoveSplineInit init(me);
            for (auto itr : aysaPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetUncompressed();
            init.Launch();
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && !hasTriggered && me->GetExactDist2d(who) <= 20.0f)
        {
            hasTriggered = true;
            Talk(TALK_INTRO);
            me->GetMotionMaster()->MoveJump(aysaPath[0], 10.0f, 10.0f, EVENT_JUMP);
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasKomakTriggered)
        {
            hasKomakTriggered = true;
            me->GetMotionMaster()->MovePoint(0, aysaJiPath);

            uint32 delay = me->GetSplineDuration();
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_3);
            });

            scheduler
                .Schedule(Milliseconds(delay += 13600), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_4);
            });

            scheduler
                .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
            {
                if (Creature* ji = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(NPC_JI_FIREPAW) : ObjectGuid::Empty))
                    ji->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

                me->GetMotionMaster()->MoveTargetedHome();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    if (Creature* ji = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(NPC_JI_FIREPAW) : ObjectGuid::Empty))
                        ji->DespawnOrUnsummon();

                    me->DespawnOrUnsummon();
                });
            });

        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Blind Blademaster 72131
struct npc_siege_of_orgrimmar_blind_blademaster : public customCreatureAI
{
    npc_siege_of_orgrimmar_blind_blademaster(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->PrepareChanneledCast(me->GetOrientation());
        events.ScheduleEvent(EVENT_BLIND_STRIKE, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BLIND_CLEAVE, 8 * IN_MILLISECONDS);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            if (me->GetInstanceScript() && me->GetInstanceScript()->IsWipe((me, 40.0f)))
            {
                EnterEvadeMode();
                return;
            }

            context.Repeat(Milliseconds(1000));
        });
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BLIND_STRIKE:
                    me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_BLIND_STRIKE);
                    events.ScheduleEvent(EVENT_BLIND_STRIKE, 3.5 * IN_MILLISECONDS);
                    break;
                case EVENT_BLIND_CLEAVE:
                    DoCast(me, SPELL_BLIND_CLEAVE);
                    events.ScheduleEvent(EVENT_BLIND_CLEAVE, 8 * IN_MILLISECONDS);
                    break;
            }
        }
    }
};

// Korkron Dark Shaman 72451
struct npc_siege_of_orgrimmar_korkron_dark_shaman : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_dark_shaman(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();

        // Init delay
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            if (Creature* wolf = me->FindNearestCreature(NPC_KORKRON_DIRE_WOLF, 5.0f, true))
                if (!wolf->HasAuraType(SPELL_AURA_CONTROL_VEHICLE))
                    DoCast(wolf, VEHICLE_SPELL_RIDE_HARDCODED, true);
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LAVA_BURST, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_ELECTRIFIED_GROUND, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_POISON_BOLT_TOTEM, 5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetInCombatWithZone();
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
            ExecuteTargetEvent(SPELL_LAVA_BURST, urand(2.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS), EVENT_LAVA_BURST, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_ELECTRIFIED_GROUND, urand(10 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_ELECTRIFIED_GROUND, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_POISON_BOLT_TOTEM, 15 * IN_MILLISECONDS, EVENT_POISON_BOLT_TOTEM, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Poison Bolt Totem 72562
struct npc_siege_of_orgrimmar_poison_bolt_totem : public customCreatureAI
{
    npc_siege_of_orgrimmar_poison_bolt_totem(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            DoCast(me, SPELL_POISON_BOLT);
            context.Repeat(Milliseconds(2600));
        });
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Korkron Grunt 72412
struct npc_siege_of_orgrimmar_korkron_grunt_2 : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_grunt_2(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->RemoveAurasDueToSpell(SPELL_SIT);
        events.ScheduleEvent(EVENT_CLEAVE, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_CLEAVE, urand(4.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_CLEAVE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Overseeker Mojka 72490
struct npc_siege_of_orgrimmar_overseeker_mojka : public customCreatureAI
{
    npc_siege_of_orgrimmar_overseeker_mojka(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who, SPELL_CHARGE);

        events.ScheduleEvent(EVENT_CHARGE_S, 15 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MORTAL_STRIKE, 4.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THREATENING_SHOUT, 11 * IN_MILLISECONDS);

        // Send near prisoners in combat
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_MOJKA_PRISONERS_COMBAT, 1);
    }

    void JustDied(Unit* /*killer*/) override
    {
        // Free our prisoners
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_MOJKA_PRISONERS_LEAVE, 1);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_CHARGE_S, 15 * IN_MILLISECONDS, EVENT_CHARGE_S, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_MORTAL_STRIKE, 7 * IN_MILLISECONDS, EVENT_MORTAL_STRIKE, eventId);
            ExecuteTargetEvent(SPELL_THREATINING_SHOUT, 11 * IN_MILLISECONDS, EVENT_THREATENING_SHOUT, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Treasure Guard 72434
struct npc_siege_of_orgrimmar_treasure_guard : public customCreatureAI
{
    npc_siege_of_orgrimmar_treasure_guard(Creature* creature) : customCreatureAI(creature) { }

    bool hasTriggered;

    void Reset() override
    {
        hasTriggered = false;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(60) && !hasTriggered)
        {
            hasTriggered = true;
            DoCast(me, SPELL_ENRAGE);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Overseer Komak 72455
struct npc_overseer_komak : public customCreatureAI
{
    npc_overseer_komak(Creature* creature) : customCreatureAI(creature) 
    {
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override 
    {
        DoCast(who, SPELL_CHARGE);

        events.ScheduleEvent(EVENT_CHARGE_S, 15 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MORTAL_STRIKE, 4.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THREATENING_SHOUT, 11 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;

            uint32 delay = 500;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_INTRO);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 11200), [this](TaskContext context)
            {
                if (Creature* aysa = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(NPC_AYSA_CLOUDSINGER) : ObjectGuid::Empty))
                    aysa->AI()->Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
            {
                if (Creature* aysa = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(NPC_AYSA_CLOUDSINGER) : ObjectGuid::Empty))
                    aysa->AI()->Talk(TALK_SPECIAL_2);
            });
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* aysa = ObjectAccessor::GetCreature(*me, me->GetInstanceId() ? me->GetInstanceScript()->GetGuidData(NPC_AYSA_CLOUDSINGER) : ObjectGuid::Empty))
            aysa->AI()->DoAction(ACTION_START_INTRO);
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
            ExecuteTargetEvent(SPELL_CHARGE_S, 15 * IN_MILLISECONDS, EVENT_CHARGE_S, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_MORTAL_STRIKE, 7 * IN_MILLISECONDS, EVENT_MORTAL_STRIKE, eventId);
            ExecuteTargetEvent(SPELL_THREATINING_SHOUT, 11 * IN_MILLISECONDS, EVENT_THREATENING_SHOUT, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Mokvar the Treasurer 72433
struct npc_siege_of_orgrimmar_mokvar_the_treasurer : public customCreatureAI
{
    npc_siege_of_orgrimmar_mokvar_the_treasurer(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_COIN_TOSS, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BRIBE, 12 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
            if (Player* target = itr.GetSource())
                if (target->IsCharmed())
                    target->RemoveCharmAuras();
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        for (auto&& itr : me->GetInstanceScript()->instance->GetPlayers())
            if (Player* target = itr.GetSource())
                if (target->IsCharmed())
                    me->Kill(target);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_COIN_TOSS, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_COIN_TOSS, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_BRIBE, 15 * IN_MILLISECONDS, EVENT_BRIBE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Cage Prisoner 72486, 72640
struct npc_siege_of_orgrimmar_cage_prisoner : public customCreatureAI
{
    npc_siege_of_orgrimmar_cage_prisoner(Creature* creature) : customCreatureAI(creature) 
    { 
        me->Respawn();
        hasIntro = false;
    }
    
    TaskScheduler scheduler;
    bool hasIntro;

    void Reset() override
    {
        // in server crash case
        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SHAMANS_PRISON) == DONE && me->GetEntry() == 72486)
                me->GetMotionMaster()->MovePoint(0, cagePrisonerLeavePos);
        });
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                // save data in crash case
                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(DATA_SHAMANS_PRISON, DONE);

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(DATA_PRISONERS_COUNT, 1);

                Talk(me->GetEntry() == NPC_DOYODA ? TALK_SPECIAL_1 : TALK_INTRO);
                me->GetMotionMaster()->MovePoint(0, cagePrisonerLeavePos);
                break;
            case ACTION_CAGE_PRISONERS_INTRO:
                if (hasIntro || !me->IsAlive())
                    break;

                hasIntro = true;
                Talk(TALK_INTRO);
                break;
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (!me->FindNearestCreature(NPC_RESISTANCE_TOTEM, 50.0f, true))
            DoCast(me, SPELL_RESISTANCE_TOTEM);

        me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Resistance Totem 72643
struct npc_siege_of_orgrimmar_resistance_totem : public customCreatureAI
{
    npc_siege_of_orgrimmar_resistance_totem(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        if (Player* target = me->FindNearestPlayer(100.0f))
            DoCast(target, SPELL_RESISTANCE_TOTEM_EFF);
        else // wait players
        {
            scheduler
                .Schedule(Seconds(2), [this](TaskContext context)
            {
                if (Player* target = me->FindNearestPlayer(100.0f))
                {
                    DoCast(target, SPELL_RESISTANCE_TOTEM_EFF);
                    return;
                }

                context.Repeat(Seconds(2));
            });
        }
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Korkron Shadowmage 72150
struct npc_siege_of_orgrimmar_korkron_shadowmage : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_shadowmage(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_MIND_SPIKE, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHADOWFLAME, 11 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker && attacker->GetTypeId() != TYPEID_PLAYER && HealthBelowPct(70))
            damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_MIND_SPIKE, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_MIND_SPIKE, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_SHADOWFLAME, 12.5 * IN_MILLISECONDS, EVENT_SHADOWFLAME, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Overseer 72421
struct npc_siege_of_orgrimmar_korkron_overseer : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_overseer(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();

        if (me->GetEntry() == NPC_OVERSEER_THATUNG)
        {
            scheduler
                .Schedule(Milliseconds(1500), [this](TaskContext context)
            {
                DoCast(me, SPELL_SLAVE_CHAINS);
            });
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who, SPELL_CHARGE);

        events.ScheduleEvent(EVENT_CHARGE_S, 15 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MORTAL_STRIKE, 4.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THREATENING_SHOUT, 11 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetEntry() == NPC_OVERSEER_THATUNG && me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_THATUNG_PRISONERS_LEAVE, 1);
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
            ExecuteTargetEvent(SPELL_CHARGE_S, 15 * IN_MILLISECONDS, EVENT_CHARGE_S, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_MORTAL_STRIKE, 7 * IN_MILLISECONDS, EVENT_MORTAL_STRIKE, eventId);
            ExecuteTargetEvent(SPELL_THREATINING_SHOUT, 11 * IN_MILLISECONDS, EVENT_THREATENING_SHOUT, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Dire Wolf 72452
struct npc_siege_of_orgrimmar_korkron_dire_wolf : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_dire_wolf(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_REND, 2.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_REND, urand(6.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_REND, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Eitrigg 72432
struct npc_siege_of_orgrimmar_eitrigg : public customCreatureAI
{
    npc_siege_of_orgrimmar_eitrigg(Creature* creature) : customCreatureAI(creature)
    {
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;
            Talk(TALK_INTRO);

            uint32 delay = 9600;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 5800), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Overlord Runthak 72191
struct npc_siege_of_orgrimmar_overlord_runthak : public customCreatureAI
{
    npc_siege_of_orgrimmar_overlord_runthak(Creature* creature) : customCreatureAI(creature)
    {
        hasTriggered = false;
    }

    bool hasTriggered;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_BONECRACKER, 3 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;
            Talk(TALK_INTRO);

            // Set Demolishers in combat state
            std::list<Creature*> demolishersList;
            GetCreatureListWithEntryInGrid(demolishersList, me, NPC_HELLSCREAM_DEMOLISHER, 50.0f);

            for (auto&& itr : demolishersList)
                itr->SetInCombatWithZone();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_BONECRACKER, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_BONECRACKER, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Hellscream Demolisher 72194
struct npc_siege_of_orgrimmar_hellscream_demolisher : public customCreatureAI
{
    npc_siege_of_orgrimmar_hellscream_demolisher(Creature* creature) : customCreatureAI(creature) 
    { 
        SetCombatMovement(false);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_HURL_BOULDER, urand(1.5 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_HURL_BOULDER)
            {
                if (!me->GetVictim() || me->GetVictim() && me->GetExactDist2d(me->GetVictim()) > 4.72f) // spell range
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_HURL_BOULDER);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_HURL_BOULDER);
                }

                events.ScheduleEvent(EVENT_HURL_BOULDER, urand(2 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Groldar 72145
struct npc_siege_of_orgrimmar_groldar : public customCreatureAI
{
    npc_siege_of_orgrimmar_groldar(Creature* creature) : customCreatureAI(creature) 
    { 
        hasTriggered = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;

    void Reset() override
    {
        // Delay Init
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            DoCast(me, SPELL_CONTROL_POWERFUL_DEMON);
        });
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;
            Talk(TALK_INTRO);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Doomlord 72564
struct npc_siege_of_orgrimmar_doomlord : public customCreatureAI
{
    npc_siege_of_orgrimmar_doomlord(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_CRIPPLE, urand(1.5 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHADOW_BOLT_VOLLEY, 8.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CURSE_OF_TONGUE, 12 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker && attacker->GetTypeId() != TYPEID_PLAYER && HealthBelowPct(70))
            damage = 0;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* groldar = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GROLDAR) : ObjectGuid::Empty))
            me->Kill(groldar, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_CRIPPLE, urand(6 * IN_MILLISECONDS, 9 * IN_MILLISECONDS), EVENT_CRIPPLE, eventId);
            ExecuteTargetEvent(SPELL_SHADOW_BOLT_VOLLEY, 9 * IN_MILLISECONDS, EVENT_SHADOW_BOLT_VOLLEY, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CURSE_OF_TONGUES, 15 * IN_MILLISECONDS, EVENT_CURSE_OF_TONGUE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Blood Axe 72728
struct npc_siege_of_orgrimmar_korkron_blood_axe : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_blood_axe(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        // no way to subgroup this without regions
        if (me->GetDBTableGUIDLow() == 558870 || me->GetDBTableGUIDLow() == 558872 || me->GetDBTableGUIDLow() == 558863 || me->GetDBTableGUIDLow() == 558880
            || me->GetDBTableGUIDLow() == 558868 || me->GetDBTableGUIDLow() == 558900 || me->GetDBTableGUIDLow() == 558915 || me->GetDBTableGUIDLow() == 558991)
            Talk(TALK_INTRO);

        events.ScheduleEvent(EVENT_CLEAVE, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_SKULLSPLITTER_RAGE);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), false);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->HandleEmoteStateCommand(EMOTE_STATE_READY2H);
                me->SetHomePosition(*me);
            });
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
            ExecuteTargetEvent(SPELL_CLEAVE, urand(4.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_CLEAVE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Gunner 72784, Korkron Wild Gun 73250
struct npc_siege_of_orgrimmar_korkron_gunner : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_gunner(Creature* creature) : customCreatureAI(creature)
    {
        x = 0.0f; y = 0.0f; z = me->GetHomePosition().GetPositionZ();
        GetPositionWithDistInOrientation(me, 55.0f, me->GetOrientation(), x, y);
        me->UpdateAllowedPositionZ(x, y, z);
        SetCombatMovement(false);
    }

    TaskScheduler scheduler;
    bool hasTriggered;
    float x, y, z;

    void Reset() override
    {
        events.Reset();
        hasTriggered = false;

        if (me->GetHomePosition().GetPositionZ() < -224.0f)
            return;

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            Movement::MoveSplineInit init(me);
            init.Path().push_back(G3D::Vector3(x, y, z));
            init.Path().push_back(G3D::Vector3(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ()));
            init.Path().push_back(G3D::Vector3(x, y, z));
            init.Path().push_back(G3D::Vector3(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ()));

            init.SetUncompressed();
            init.SetWalk(true);
            init.SetCyclic();
            init.Launch();
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(30) && !hasTriggered)
        {
            hasTriggered = true;
            events.ScheduleEvent(EVENT_BLAZE_OF_GLORY, 0.5 * IN_MILLISECONDS);
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), false);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->HandleEmoteStateCommand(EMOTE_STATE_READY_RIFLE);
                me->SetHomePosition(*me);
            });
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (me->GetHomePosition().GetPositionZ() > -224.0f) // stop patrol
        {
            me->StopMoving();
            scheduler.CancelAll();
        }

        events.ScheduleEvent(EVENT_FIRE_STORM, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_MOLTEN_SLUG, 1.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_MOLTEN_SLUG, urand(2.2 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS), EVENT_MOLTEN_SLUG, eventId);
            ExecuteTargetEvent(SPELL_FIRESTORM, urand(12 * IN_MILLISECONDS, 24 * IN_MILLISECONDS), EVENT_FIRE_STORM, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_BLAZE_OF_GLORY, 0, EVENT_BLAZE_OF_GLORY, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Dark Farseer 72770
struct npc_siege_of_orgrimmar_korkron_dark_farseer : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_dark_farseer(Creature* creature) : customCreatureAI(creature) 
    {
        hasYell = false;
    }

    TaskScheduler scheduler;
    bool hasTriggered;
    bool hasYell;

    void Reset() override
    {
        hasTriggered = false;
        events.Reset();
        summons.DespawnAll();

        // Not using wolf for both
        if (me->GetDBTableGUIDLow() == 558933 || me->GetDBTableGUIDLow() == 559010)
            return;

        // Delay Init
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            if (Creature* darkWolf = me->SummonCreature(NPC_KORKRON_WAR_WOLF, *me, TEMPSUMMON_MANUAL_DESPAWN))
                me->CastSpell(darkWolf, VEHICLE_SPELL_RIDE_HARDCODED, true);
        });
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(50) && !hasTriggered)
        {
            hasTriggered = true;
            events.ScheduleEvent(EVENT_DARK_FURY, 0.5 * IN_MILLISECONDS);
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_BALE_STRIKE, urand(1.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_UMBRAL_TEMPEST, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
            case ACTION_CORRUPTED_SKULLSPLITTER:
                if (hasYell)
                    break;

                hasYell = true;
                Talk(actionId == ACTION_START_INTRO ? TALK_SPECIAL_1 : TALK_SPECIAL_2);
                break;
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (me->IsAlive())
            Talk(TALK_INTRO);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        summons.DespawnAll();
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
            ExecuteTargetEvent(SPELL_BALE_STRIKE, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS), EVENT_BALE_STRIKE, eventId);
            ExecuteTargetEvent(SPELL_UMBRAL_TEMPEST, 12 * IN_MILLISECONDS, EVENT_UMBRAL_TEMPEST, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_DARK_FURY, 0, EVENT_DARK_FURY, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Dark Wolf 72768
struct npc_siege_of_orgrimmar_korkron_dark_wolf : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_dark_wolf(Creature* creature) : customCreatureAI(creature) 
    {
        x = 0.0f; y = 0.0f; z = me->GetHomePosition().GetPositionZ();
        GetPositionWithDistInOrientation(me, 55.0f, me->GetOrientation(), x, y);
        me->UpdateAllowedPositionZ(x, y, z);
    }

    TaskScheduler scheduler;
    float x, y, z;

    void Reset() override
    {
        events.Reset();

        if (me->GetPositionZ() < -224.0f)
            return;

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            Movement::MoveSplineInit init(me);
            init.Path().push_back(G3D::Vector3(x, y, z));
            init.Path().push_back(G3D::Vector3(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ()));
            init.Path().push_back(G3D::Vector3(x, y, z));
            init.Path().push_back(G3D::Vector3(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ()));

            init.SetUncompressed();
            init.SetWalk(true);
            init.SetCyclic();
            init.Launch();
        });
    }

    void JustEngagedWith(Unit* who) override
    {
        me->StopMoving();
        scheduler.CancelAll();
        events.ScheduleEvent(EVENT_REND, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DREAD_HOWL, urand(8.5 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_REND_2, urand(6.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_REND, eventId);
            ExecuteTargetEvent(SPELL_DREAD_HOLW, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS), EVENT_DREAD_HOWL, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Hellscream Annihilator 72847
struct npc_siege_of_orgrimmar_hellscream_annihilator : public customCreatureAI
{
    npc_siege_of_orgrimmar_hellscream_annihilator(Creature* creature) : customCreatureAI(creature)
    {
        SetCombatMovement(false);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(me, SPELL_ANNIHILATION);
        events.ScheduleEvent(EVENT_SCORCHED_EARTH, urand(1.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SCORCHED_EARTH, 5 * IN_MILLISECONDS, EVENT_SCORCHED_EARTH, eventId, PRIORITY_NOT_VICTIM);
            break;
        }
    }
};

// Korkron Skull Splitter 72744
struct npc_siege_of_orgrimmar_korkron_skull_splitter : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_skull_splitter(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        Talk(TALK_INTRO);
        DoCast(me, SPELL_BATTLE_BOUND);
        events.ScheduleEvent(EVENT_SERRATED_RAMPAGE, urand(5.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->GetMotionMaster()->MovePath(me->GetDBTableGUIDLow(), false);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                me->SetHomePosition(*me);
            });
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
            ExecuteTargetEvent(SPELL_SERRATED_RAMPAGE, urand(11 * IN_MILLISECONDS, 16.5 * IN_MILLISECONDS), EVENT_SERRATED_RAMPAGE, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Corrupted Skullsplitter 72983
struct npc_siege_of_orgrimmar_corrupted_skullsplitter : public customCreatureAI
{
    npc_siege_of_orgrimmar_corrupted_skullsplitter(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();
        me->SetReactState(REACT_AGGRESSIVE);
        targetGUID = ObjectGuid::Empty;
        me->SetFaction(14);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        DoCast(summon, SPELL_ARCING_BLAST_EFF);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            me->RemoveChanneledCast(targetGUID);
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetFaction(1375);
        events.ScheduleEvent(EVENT_ARCING_BLAST, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        summons.DespawnAll();
        scheduler.CancelAll();
    }

    void JustDied(Unit* /*killer*/) override
    {
        scheduler.CancelAll();
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
            if (eventId == EVENT_ARCING_BLAST)
            {
                if (Unit* target = me->GetVictim())
                    targetGUID = target->GetGUID();

                me->PlayOneShotAnimKitId(4308);
                me->PrepareChanneledCast(me->GetOrientation(), SPELL_ARCING_BLAST);

                events.ScheduleEvent(EVENT_ARCING_BLAST, 15 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Iron Scorpion 73194
struct npc_siege_of_orgrimmar_korkron_iron_scorpion : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_iron_scorpion(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SERRATED_CLEAVE, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_ACID_SPRAY, 11 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SERRATED_CLEAVE, urand(6.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_SERRATED_CLEAVE, eventId);
            ExecuteTargetEvent(SPELL_ACID_SPRAY_SELECTOR, urand(13.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_ACID_SPRAY, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Arcweaver Reinforcements 73153
struct npc_siege_of_orgrimmar_arcweaver_reinforcements : public customCreatureAI
{
    npc_siege_of_orgrimmar_arcweaver_reinforcements(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DETERIORATE, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_INFUSION, 8 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_DETERIORATE, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_DETERIORATE, eventId);
            ExecuteTargetEvent(SPELL_INFUSION, urand(13.5 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_INFUSION, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Brute Reinforcements 73155
struct npc_siege_of_orgrimmar_brute_reinforcements : public customCreatureAI
{
    npc_siege_of_orgrimmar_brute_reinforcements(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SLOW_AND_STEADY, urand(2.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SLOW_AND_STEADY, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS), EVENT_SLOW_AND_STEADY, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Storeroom Guard 73152
struct npc_siege_of_orgrimmar_storeroom_guard : public customCreatureAI
{
    npc_siege_of_orgrimmar_storeroom_guard(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_CALL_REINFORCEMENTS);
        events.ScheduleEvent(EVENT_CALL_REINFORCEMENTS, 10 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetInCombatWithZone();
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        summons.DespawnAll();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_CALL_REINFORCEMENTS, 12 * IN_MILLISECONDS, EVENT_CALL_REINFORCEMENTS, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Jailer 73195
struct npc_siege_of_orgrimmar_korkron_jailer_thrash : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_jailer_thrash(Creature* creature) : customCreatureAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == 0)
            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
    }
};

// Korkron Iron Sentinel 73904
struct npc_korkron_iron_sentinel : public customCreatureAI
{
    npc_korkron_iron_sentinel(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_STRENGTH_IN_NUMBERS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Korkron Gunner RP event 74158
struct npc_siege_of_orgrimmar_korkron_gunner_rp_event : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_gunner_rp_event(Creature* creature) : customCreatureAI(creature) 
    { 
        hasTriggeredRP = false;
        SetCombatMovement(false);
    }

    TaskScheduler scheduler;
    bool hasTriggered;
    bool hasTriggeredRP;

    void Reset() override
    {
        events.Reset();
        hasTriggered = false;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(30) && !hasTriggered)
        {
            hasTriggered = true;
            events.ScheduleEvent(EVENT_BLAZE_OF_GLORY, 0.5 * IN_MILLISECONDS);
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggeredRP && me->IsAlive())
        {
            hasTriggeredRP = true;
            Talk(TALK_INTRO);

            scheduler
                .Schedule(Milliseconds(3000), [this](TaskContext context)
            {
                if (Creature* gunnerRP = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KORKRON_GUNNER_RP + 1) : ObjectGuid::Empty))
                    gunnerRP->AI()->Talk(TALK_SPECIAL_1);
            });
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FIRE_STORM, urand(8 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_MOLTEN_SLUG, 1.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_MOLTEN_SLUG, urand(2.2 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS), EVENT_MOLTEN_SLUG, eventId);
            ExecuteTargetEvent(SPELL_FIRESTORM, urand(12 * IN_MILLISECONDS, 24 * IN_MILLISECONDS), EVENT_FIRE_STORM, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_BLAZE_OF_GLORY, 0, EVENT_BLAZE_OF_GLORY, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// War Master Kragg 73775
struct npc_war_master_kragg : public customCreatureAI
{
    npc_war_master_kragg(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_OBLITERATE, 8 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_OBLITERATE, urand(12 * IN_MILLISECONDS, 16 * IN_MILLISECONDS), EVENT_OBLITERATE, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Thresher Turret 73742
struct npc_siege_of_orgrimmar_thresher_turret : public customCreatureAI
{
    npc_siege_of_orgrimmar_thresher_turret(Creature* creature) : customCreatureAI(creature) 
    {
        SetCombatMovement(false);
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHREDDING_BLAST, urand(1.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_SHREDDING_BLAST_SELECTOR, urand(3.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS), EVENT_SHREDDING_BLAST, eventId, PRIORITY_SELF);
            break;
        }
    }
};

// Enraged Mushan Beast 73185
struct npc_siege_of_orgrimmar_enraged_mushan_beast : public customCreatureAI
{
    npc_siege_of_orgrimmar_enraged_mushan_beast(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CRUSHING_SWIPE, 11 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LACERATING_BEAT, 3.5 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
            me->GetMotionMaster()->MoveJump(thokBloodthristyRelocate[1], 25.0f, 25.0f, EVENT_JUMP);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI / 2));
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* yeti = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_STARVED_YETI_TRASH) : ObjectGuid::Empty))
        {
            if (!yeti->IsAlive())
            {
                if (Creature* thok = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                {
                    if (thok->IsAlive() && thok->HasAura(SPELL_MELEE))
                    {
                        thok->CastSpell(thok, SPELL_KILL_JAILERS, true);
                        thok->GetMotionMaster()->MovePoint(2, thokBloodthristyRelocate[1]);
                    }
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_LACERATING_BEAT, urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS), EVENT_LACERATING_BEAT, eventId);
            ExecuteTargetEvent(SPELL_CRUSHING_SWIPE, 15 * IN_MILLISECONDS, EVENT_CRUSHING_SWIPE, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Starved Yeti 73184
struct npc_siege_of_orgrimmar_starved_yeti : public ScriptedAI
{
    npc_siege_of_orgrimmar_starved_yeti(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetWalk(false);
            me->GetMotionMaster()->MoveJump(starvedYetiTrash, 25.0f, 25.0f, EVENT_JUMP);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* mushan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_ENRAGED_MUSHAN_BEAST) : ObjectGuid::Empty))
        {
            if (!mushan->IsAlive())
            {
                if (Creature* thok = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                {
                    if (thok->IsAlive() && thok->HasAura(SPELL_MELEE))
                    {
                        thok->CastSpell(thok, SPELL_KILL_JAILERS, true);
                        thok->GetMotionMaster()->MovePoint(2, thokBloodthristyRelocate[1]);
                    }
                }
            }
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            scheduler
                .Schedule(Seconds(1), [this](TaskContext context)
            {
                DoCast(me, SPELL_WRECKING_BALL);
            });
        }

        if (pointId == EVENT_CHARGE)
        {
            me->RemoveAurasDueToSpell(SPELL_WRECKING_BALL_AT);

            me->SetFacingTo(Position::NormalizeOrientation(me->GetAngle(&thokBloodthristyRelocate[1]) - M_PI));
            DoCast(me, SPELL_WRECKING_BALL_BREAK);

            scheduler
                .Schedule(Milliseconds(16000), [this](TaskContext context)
            {
                DoCast(me, SPELL_WRECKING_BALL);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Aggron 72981
struct npc_siege_of_orgrimmar_aggron : public customCreatureAI
{
    npc_siege_of_orgrimmar_aggron(Creature* creature) : customCreatureAI(creature) 
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE)
            me->Respawn(); // server crash case
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_OBLITERATING_STRIKE, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_VIVISECTION, 10.5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* gorodan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_GORODAN) : ObjectGuid::Empty))
            gorodan->AI()->DoAction(ACTION_START_INTRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_VIVISECTION, 13 * IN_MILLISECONDS, EVENT_VIVISECTION, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_OBLITERATING_STRIKE, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS), EVENT_OBLITERATING_STRIKE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Blackfuse Korkron Overseer 73098
struct npc_siege_of_orgrimmar_blackfuse_korkron_overseer : public customCreatureAI
{
    npc_siege_of_orgrimmar_blackfuse_korkron_overseer(Creature* creature) : customCreatureAI(creature) 
    { 
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE)
            me->Respawn(); // server crash case
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_OBLITERATING_STRIKE, 7 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_OBLITERATING_STRIKE, urand(9 * IN_MILLISECONDS, 14 * IN_MILLISECONDS), EVENT_OBLITERATING_STRIKE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Blackfuse Sellsword 73091
struct npc_siege_of_orgrimmar_blackfuse_sellsword : public customCreatureAI
{
    npc_siege_of_orgrimmar_blackfuse_sellsword(Creature* creature) : customCreatureAI(creature) 
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE)
            me->Respawn(); // server crash case
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ENVENOMED_SHADOWSTEP, urand(7.5 * IN_MILLISECONDS, 11 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
            events.ScheduleEvent(EVENT_FAN_OF_KNIVES, 0.5 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_ENVENOMED_SHADOWSTEP, 12 * IN_MILLISECONDS, EVENT_ENVENOMED_SHADOWSTEP, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_FAN_OF_KNIVES, 0, EVENT_FAN_OF_KNIVES, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Blackfuse Engineer 73095
struct npc_siege_of_orgrimmar_blackfuse_engineer : public customCreatureAI
{
    npc_siege_of_orgrimmar_blackfuse_engineer(Creature* creature) : customCreatureAI(creature) 
    { 
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE)
            me->Respawn(); // server crash case
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ADRENALINE_BOMB, urand(11 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_HEALING_SPRAY, urand(5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_THROW_DYNAMITE, 1 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        }
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_BLACKFUSE_SELLSWORD, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_BLACKFUSE_ENGINEER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_BLACKFUSE_KORKRON_OVERSEER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_AGGRON, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_GORODAN, 80.0f);
        tmpTargets.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_ADRENALINE_BOMB, 12 * IN_MILLISECONDS, EVENT_ADRENALINE_BOMB, eventId, PRIORITY_ALLY);
            ExecuteTargetEvent(SPELL_HEALING_SPRAY, 18 * IN_MILLISECONDS, EVENT_HEALING_SPRAY, eventId, PRIORITY_ALLY);
            ExecuteTargetEvent(SPELL_THROW_DYNAMITE, 4.5 * IN_MILLISECONDS, EVENT_THROW_DYNAMITE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Gorodan 72964
struct npc_siege_of_orgrimmar_gorodan : public customCreatureAI
{
    npc_siege_of_orgrimmar_gorodan(Creature* creature) : customCreatureAI(creature) 
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE)
            me->Respawn(); // server crash case
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
        events.ScheduleEvent(EVENT_DRILLCHARGE, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DRILLSTORM, 10 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* shanna = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SHANNA_SPARKFIZZ) : ObjectGuid::Empty))
            shanna->AI()->DoAction(ACTION_START_INTRO);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            // pull to underhold
            std::list<Creature*> blackfuseElite;
            GetCreatureListWithEntryInGrid(blackfuseElite, me, NPC_BLACKFUSE_ENGINEER, 35.0f);
            GetCreatureListWithEntryInGrid(blackfuseElite, me, NPC_BLACKFUSE_SELLSWORD, 35.0f);
            GetCreatureListWithEntryInGrid(blackfuseElite, me, NPC_BLACKFUSE_KORKRON_OVERSEER, 35.0f);

            for (auto&& itr : blackfuseElite)
                itr->CastSpell(itr, SPELL_PIPE_TRANSFERT_UNDERHOLD, true);

            DoCast(me, SPELL_PIPE_TRANSFERT_UNDERHOLD);

            if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                siegeCrafter->AI()->Talk(TALK_SPECIAL_6);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_DRILLSCHARGE, 8 * IN_MILLISECONDS, EVENT_DRILLCHARGE, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_DRILLSTORM, 15 * IN_MILLISECONDS, EVENT_DRILLSTORM, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Shanna Sparkfizz 72986
struct npc_siege_of_orgrimmar_shanna_sparkfizz : public customCreatureAI
{
    npc_siege_of_orgrimmar_shanna_sparkfizz(Creature* creature) : customCreatureAI(creature) 
    { 
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE)
            me->Respawn(); // server crash case

        SetCombatMovement(false);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LASER_BEAM, 2 * IN_MILLISECONDS);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
        summons.DespawnAll();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
        {
            if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                siegeCrafter->AI()->Talk(TALK_SPECIAL_8);

            me->GetInstanceScript()->SetData(DATA_SIEGECRAFTER_PRE_EVENT, DONE);
        }

        summons.DespawnAll();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            // pull to underhold
            std::list<Creature*> blackfuseElite;
            GetCreatureListWithEntryInGrid(blackfuseElite, me, NPC_BLACKFUSE_ENGINEER, 35.0f);
            GetCreatureListWithEntryInGrid(blackfuseElite, me, NPC_BLACKFUSE_SELLSWORD, 35.0f);
            GetCreatureListWithEntryInGrid(blackfuseElite, me, NPC_BLACKFUSE_KORKRON_OVERSEER, 35.0f);

            for (auto&& itr : blackfuseElite)
                itr->CastSpell(itr, SPELL_PIPE_TRANSFERT_UNDERHOLD, true);

            DoCast(me, SPELL_PIPE_TRANSFERT_UNDERHOLD);

            if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                siegeCrafter->AI()->Talk(TALK_SPECIAL_7);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_LASER_BEAM)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                    target->CastSpell(target, SPELL_LASER_TURRET_BUNNY, true);
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                    target->CastSpell(target, SPELL_LASER_TURRET_BUNNY, true);

                events.ScheduleEvent(EVENT_LASER_BEAM, 20 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sra`thik Amber-Master 72929
struct npc_klaxxi_srathik_amber_master : public customCreatureAI
{
    npc_klaxxi_srathik_amber_master(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
        summons.DespawnAll();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_AMBER_BLAST, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_RESONATING_AMBER, 8.5 * IN_MILLISECONDS);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->CastSpell(summon, SPELL_RESONATING_AMBER_AT, true);
        summon->CastSpell(summon, SPELL_AMBER_GROWTH, false);
    }

    void JustDied(Unit* /*killer*/) override
    {
        summons.DespawnAll();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_AMBER_BLAST, urand(3.1 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS), EVENT_AMBER_BLAST, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_RESONATING_AMBER_SELECTOR, urand(18 * IN_MILLISECONDS, 26 * IN_MILLISECONDS), EVENT_RESONATING_AMBER, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Kor`thik Honor Guard 72954
struct npc_klaxxi_korthik_honor_guard : public customCreatureAI
{
    npc_klaxxi_korthik_honor_guard(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FRENZIED_ASSAULT, 9 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_PIERCE, 5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_FRENZIED_ASSAULT, 12 * IN_MILLISECONDS, EVENT_FRENZIED_ASSAULT, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_PIERCE, 9 * IN_MILLISECONDS, EVENT_PIERCE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Kovok 72927
struct npc_klaxxi_kovok : public customCreatureAI
{
    npc_klaxxi_kovok(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MIGHTY_CLEAVE, 3.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_GROUND_SLAM, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_POISON_BLAST, 13 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_MIGHTY_CLEAVE, 6 * IN_MILLISECONDS, EVENT_MIGHTY_CLEAVE, eventId);
            ExecuteTargetEvent(SPELL_GROUND_SLAM, 10 * IN_MILLISECONDS, EVENT_GROUND_SLAM, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_POISON_BLAST, urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_POISON_BLAST, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Grand Master Alchemist Kixen 73696
struct npc_klaxxi_grand_master_alchemist_kixen : public customCreatureAI
{
    npc_klaxxi_grand_master_alchemist_kixen(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ALCHEMICAL_MASTERY, 2.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_ALCHEMICAL_MASTERY, 7 * IN_MILLISECONDS, EVENT_ALCHEMICAL_MASTERY, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Overlord Saurfang 73716
struct npc_siege_of_orgrimmar_overlord_saurfang : public customCreatureAI
{
    npc_siege_of_orgrimmar_overlord_saurfang(Creature* creature) : customCreatureAI(creature)
    {
        hasTriggered = false;
        delay = 0;
    }

    TaskScheduler scheduler;
    bool hasTriggered;
    uint32 delay;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;

            Talk(TALK_INTRO);

            delay = 6800;
            scheduler
                .Schedule(Milliseconds(delay), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_1);
            });

            scheduler
                .Schedule(Milliseconds(delay += 5500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_2);
            });

            scheduler
                .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_3);
            });

            scheduler
                .Schedule(Milliseconds(delay += 8500), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_4);
            });

            scheduler
                .Schedule(Milliseconds(delay += 11300), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_5);
            });

            scheduler
                .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
            {
                Talk(TALK_SPECIAL_6);
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Thrall 73483
struct npc_siege_of_orgrimmar_thrall : public customCreatureAI
{
    npc_siege_of_orgrimmar_thrall(Creature* creature) : customCreatureAI(creature)
    {
        hasTriggered = false;
        instance = me->GetInstanceScript();
        delay = 0;

        if (instance && instance->GetData(DATA_GARROSH_HELLSCREAM_PREVE_EVENT) == DONE)
            me->DespawnOrUnsummon();
    }

    InstanceScript* instance;
    TaskScheduler scheduler;
    bool hasTriggered;
    uint32 delay;

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
            {
                if (hasTriggered)
                    break;

                hasTriggered = true;

                Talk(TALK_INTRO);

                delay = 12500;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                        garrosh->AI()->Talk(TALK_SPECIAL_12);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 21800), [this](TaskContext context)
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                        garrosh->AI()->Talk(TALK_SPECIAL_13);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 13500), [this](TaskContext context)
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                        garrosh->AI()->Talk(TALK_SPECIAL_14);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 18300), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8200), [this](TaskContext context)
                {
                    DoCast(me, SPELL_CALL_OF_THE_ELEMENTS);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 6400), [this](TaskContext context)
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                        garrosh->AI()->Talk(TALK_SPECIAL_15);
                });
                break;
            }
            case ACTION_THRALL_UNLEASHED:
                me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 56228); // hammer!
                Talk(TALK_SPECIAL_2);

                delay = 6000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                    {
                        garrosh->AI()->Talk(TALK_SPECIAL_16);
                        garrosh->GetMotionMaster()->MovePoint(5, garroshPathReach[0]);
                    }
                });

                scheduler
                    .Schedule(Milliseconds(delay += 2000), [this](TaskContext context)
                {
                    me->GetMotionMaster()->MovePoint(0, garroshPathReach[1]);

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration() + 500), [this](TaskContext context)
                    {
                        DoCast(me, SPELL_ATTACK_GARROSH);

                        if (instance)
                            instance->SetData(DATA_GARROSH_HELLSCREAM_PREVE_EVENT, DONE);

                        if (Creature* garrosh = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GARROSH_HELLSCREAM) : ObjectGuid::Empty))
                            garrosh->CastSpell(garrosh, SPELL_ATTACK_THRALL, false);
                    });
                });
                break;
            case ACTION_THRALL_KNELL:
                delay = 2500;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    DoCast(me, SPELL_EXHAUSTED);
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Blood of Y`shaarj 73411
struct npc_siege_of_orgrimmar_blood_of_yshaarj_trigger : public customCreatureAI
{
    npc_siege_of_orgrimmar_blood_of_yshaarj_trigger(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    bool hasLaunch;
    uint32 ichorCount;

    void Reset() override
    {
        DoCast(me, SPELL_BLOOD_OF_YSHAARJ);
        DoCast(me, SPELL_SUMMON_BLOOD_OF_YSHAARJ);
        hasLaunch = false;
        ichorCount = 0;
    }

    void JustSummoned(Creature* summon) override
    {
        ichorCount++; // limit is 4
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        ichorCount--;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_ICHOR_OF_YSHAARJ_COUNT)
            return ichorCount;

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_RESET_YSHAARJ_TIMER:
                hasLaunch = false;
                scheduler.CancelAll();
                break;
            case ACTION_START_INTRO:
                if (hasLaunch)
                    break;

                hasLaunch = true;

                scheduler
                    .Schedule(Seconds(5), [this](TaskContext context)
                {
                    me->RemoveAllAreasTrigger();
                    me->DespawnOrUnsummon();
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Ichor of Y`shaarj 73415
struct npc_siege_of_orgrimmar_ichor_of_yshaarj : public customCreatureAI
{
    npc_siege_of_orgrimmar_ichor_of_yshaarj(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->GetMotionMaster()->MoveRandom(5.0f);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_EMPOWERING_CORRUPTION);
        me->DespawnOrUnsummon(2.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Harbinger of Y`shaarj 73452
struct npc_siege_of_orgrimmar_harbinger_of_yshaarj : public customCreatureAI
{
    npc_siege_of_orgrimmar_harbinger_of_yshaarj(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->CallForHelp(15.0f);
        events.ScheduleEvent(EVENT_YSHAARJ_TOURCHED, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_GRASP_OF_YSHAARJ, urand(8 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_HARBINGER_OF_YSHAARJ, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KORKRON_REAPER, 40.0f);
        tmpTargets.remove_if([=](Creature* target) { return target && !target->IsAlive(); });

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_GRASP_OF_YSHAARJ, 18 * IN_MILLISECONDS, EVENT_GRASP_OF_YSHAARJ, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_YSHAARJ_TORCHED, 12 * IN_MILLISECONDS, EVENT_YSHAARJ_TOURCHED, eventId, PRIORITY_ALLY);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Kor`kron Reaper 73414
struct npc_siege_of_orgrimmar_korkron_reaper : public customCreatureAI
{
    npc_siege_of_orgrimmar_korkron_reaper(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->CallForHelp(15.0f);
        events.ScheduleEvent(EVENT_REAPER, 6.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_REAPER_CHARGE, 7 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_REAPER, 14.5 * IN_MILLISECONDS, EVENT_REAPER, eventId);
            ExecuteTargetEvent(SPELL_REAPER_CHARGE, 15 * IN_MILLISECONDS, EVENT_REAPER_CHARGE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Gamon 72192
struct npc_siege_of_orgrimmar_gamon : public customCreatureAI
{
    npc_siege_of_orgrimmar_gamon(Creature* creature) : customCreatureAI(creature)
    {
        hasTriggered = false;
        reachNazgrim = false;
        instance = me->GetInstanceScript();
        delay = 0;
    }

    InstanceScript* instance;
    TaskScheduler scheduler;
    bool hasTriggered;
    bool reachNazgrim;
    uint32 delay;

    void Reset() override
    {
        if (!hasTriggered)
        {
            // Init delay
            scheduler
                .Schedule(Milliseconds(1500), [this](TaskContext context)
            {
                DoCast(me, SPELL_GAMON_CHAIN_1);
                DoCast(me, SPELL_GAMON_CHAIN_2);
            });
        }
        else if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_GENERAL_NAZGRIM) != DONE)
        {
            scheduler.CancelAll();
            me->StopMoving();
            me->GetMotionMaster()->Clear();

            Movement::MoveSplineInit init(me);
            for (auto itr : gamonPath)
            {
                if (itr.GetPositionZ() > me->GetPositionZ()) // break already reached points
                    continue;

                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));
            }

            init.SetUncompressed();
            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                reachNazgrim = true;

                if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
                {
                    me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US, 1);
                    me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US_2, 1);
                    Talk(TALK_SPECIAL_10);
                    me->SetFacingToObject(nazgrim);
                }
            });
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        me->SetHomePosition(*me);
        Talk(urand(TALK_SPECIAL_7, TALK_SPECIAL_9));
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_REACH_NAZGRIM)
            return reachNazgrim ? 1 : 0;

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
            {
                if (hasTriggered)
                    break;

                hasTriggered = true;

                std::list<Creature*> bunnysList;
                GetCreatureListWithEntryInGrid(bunnysList, me, NPC_GAMMON_CHAINS_BUNNY, 10.0f);

                for (auto&& itr : bunnysList)
                {
                    itr->RemoveAurasDueToSpell(SPELL_GAMON_CHAIN_1);
                    itr->RemoveAurasDueToSpell(SPELL_GAMON_CHAIN_2);
                }

                me->RemoveAurasDueToSpell(SPELL_GAMON_COSMETIC_STATE);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                Talk(TALK_INTRO);

                delay = 4000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 7300), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_2);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 9000), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_3);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 3800), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_4);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 8600), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_5);
                    me->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID, 63252);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_6);
                    me->SetFaction(1665);

                    // Move to Nazgrim
                    Movement::MoveSplineInit init(me);
                    for (auto itr : gamonPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.SetUncompressed();
                    init.Launch();

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        reachNazgrim = true;

                        if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
                        {
                            me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US, 1);
                            me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US_2, 1);
                            Talk(TALK_SPECIAL_10);
                            me->SetFacingToObject(nazgrim);
                        }
                    });
                });
                break;
            }
            case ACTION_GAMON_OUTRO:
                if (!reachNazgrim || !me->IsAlive())
                    return;

                scheduler.CancelAll();

                if (Creature* nazgrim = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GENERAL_NAZGRIM) : ObjectGuid::Empty))
                {
                    me->GetMotionMaster()->MovePoint(0, *nazgrim);

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        me->HandleEmoteStateCommand(EMOTE_STATE_LOOT);
                        Talk(TALK_SPECIAL_11);
                    });
                }
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        scheduler.CancelAll();
        me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US, 0);
        me->GetMap()->SetWorldState(WORLDSTATE_GAMON_WILL_SAVE_US_2, 0);
    }

    void UpdateAI(uint32 diff) override
    {
        if (hasTriggered && me->HasAura(SPELL_GAMON_COSMETIC_STATE))
            me->RemoveAurasDueToSpell(SPELL_GAMON_COSMETIC_STATE);

        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Dancing Flame 61795
struct npc_siege_of_orgrimmar_dancing_flame : public customCreatureAI
{
    npc_siege_of_orgrimmar_dancing_flame(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler
            .Schedule(Seconds(1), Seconds(3), [this](TaskContext context)
        {
            DoCast(me, SPELL_DANCING_FLAMES);
            context.Repeat(Seconds(1), Seconds(3));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Sha Cloud 72588
struct npc_siege_of_orgrimmar_sha_cloud : public ScriptedAI
{
    npc_siege_of_orgrimmar_sha_cloud(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->SetDisplayFromModel(1);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_FALLEN_PROTECTORS) == IN_PROGRESS)
            return;

        clicker->CastSpell(clicker, SPELL_SHA_CLOUD_JUMP, true);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Korkron Prisoners 72492, 72493, 72484, 72485, 72483
struct npc_siege_of_orgrimmar_korkron_prisoners : public ScriptedAI
{
    npc_siege_of_orgrimmar_korkron_prisoners(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler
            .Schedule(Seconds(1), Seconds(3), [this](TaskContext context)
        {
            me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
            context.Repeat(Seconds(2), Seconds(5));
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        scheduler.CancelAll();

        if (me->GetEntry() == NPC_SHIMRA)
            Talk(TALK_INTRO);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetFaction(35);
            scheduler.CancelAll();

            if (me->GetEntry() == NPC_SHIMRA)
                Talk(TALK_SPECIAL_1);

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_PRISONERS_COUNT, 1);

            // Leave from Orgrimmar
            Movement::MoveSplineInit init(me);
            for (auto itr : korkronPrisonersLeavePath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetWalk(false);
            init.Launch();
            me->DespawnOrUnsummon(me->GetSplineDuration());
        }
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Theramore Citizen 72498
struct npc_siege_of_orgrimmar_theramore_prisoners : public ScriptedAI
{
    npc_siege_of_orgrimmar_theramore_prisoners(Creature* creature) : ScriptedAI(creature) { }

    void InitializeAI() override
    {
        if (me->GetPositionX() < 1500.0f)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            DoCast(me, SPELL_GOBLIN_COSMETIC_DEAD);
        }
        else
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
        }
    }
    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetFaction(35);
            me->RemoveAurasDueToSpell(SPELL_SLAVE_CHAINS);

            if (me->GetHomePosition().GetPositionX() > 1599.0f)
                Talk(TALK_INTRO);

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_PRISONERS_COUNT, 1);

            // Leave from Orgrimmar
            Movement::MoveSplineInit init(me);
            for (auto itr : theramorePrisonersLeavePath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetWalk(false);
            init.Launch();
            me->DespawnOrUnsummon(me->GetSplineDuration());
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Korkron Cage 221563, 221564
class go_siege_of_orgrimmar_korkron_cage : public GameObjectScript
{
    public:
        go_siege_of_orgrimmar_korkron_cage() : GameObjectScript("go_siege_of_orgrimmar_korkron_cage") { }

        void OnGameObjectStateChanged(GameObject* go, uint32 state) 
        {
            if (state != GO_STATE_ACTIVE)
                return;

            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            if (Creature* prisoner = ObjectAccessor::GetCreature(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(invCageType.find(go->GetEntry())->second) : ObjectGuid::Empty))
                prisoner->AI()->DoAction(ACTION_START_INTRO);
        }
};

// Siegecrafter Iron Door 223057
class go_siege_of_orgrimmar_siegecrafter_iron_door : public GameObjectScript
{
    public:
        go_siege_of_orgrimmar_siegecrafter_iron_door() : GameObjectScript("go_siege_of_orgrimmar_siegecrafter_iron_door") { }

        void OnGameObjectStateChanged(GameObject* go, uint32 state)
        {
            if (state != GO_STATE_ACTIVE)
                return;

            go->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);

            if (go->GetInstanceScript())
            {
                for (auto&& players : go->GetInstanceScript()->instance->GetPlayers())
                    if (Player* itr = players.GetSource())
                        itr->CastSpell(itr, go->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? SPELL_SIEGECRAFTER_SCENE_HORDE : SPELL_SIEGECRAFTER_SCENE_ALLIAN, true);
            }
        }
};

// Vortex 147304
class spell_aqueous_defender_vortex : public SpellScript
{
    PrepareSpellScript(spell_aqueous_defender_vortex);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 2 : 1;

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_VORTEX_SUMM, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aqueous_defender_vortex::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_aqueous_defender_vortex::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class VortexPredicate 
{
    public:
        VortexPredicate(Unit* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            return object && object->ToUnit() && _caster->GetExactDist2d(object->ToUnit()) < 8.5f;
    
            return false;
        }
    
    private:
        Unit const* _caster;
};

// Vortex Eff 147321
class spell_aqueous_defender_vortex_eff : public SpellScript
{
    PrepareSpellScript(spell_aqueous_defender_vortex_eff);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(VortexPredicate(GetCaster()));
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aqueous_defender_vortex_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Rushing Waters Eff 147213
class spell_rushing_waters_eff : public AuraScript
{
    PrepareAuraScript(spell_rushing_waters_eff);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        SetDuration(1.5 * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rushing_waters_eff::HandleOnApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Growing Pride 145818
class spell_growing_pride : public SpellScript
{
    PrepareSpellScript(spell_growing_pride);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_FRAGMENT_OF_PRIDE && target->GetEntry() != NPC_AMALGAMATED_HUBRIS; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_growing_pride::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_growing_pride::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Sha Smash 145944
class spell_zeal_sha_smash : public SpellScript
{
    PrepareSpellScript(spell_zeal_sha_smash);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targetsCount = targets.size();
    }

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (targetsCount > 0)
            SetHitDamage(GetHitDamage() / targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zeal_sha_smash::CheckTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        OnEffectHitTarget += SpellEffectFn(spell_zeal_sha_smash::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Corrupted Touch 149207
class spell_corrupted_touch : public SpellScript
{
    PrepareSpellScript(spell_corrupted_touch);

    std::list<WorldObject*> m_targets;

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_CORRUPTED_TOUCH); });

        if (targets.size() >= 3) // doestn`t matter - maxAffected is 3
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_corrupted_touch::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Corrupted Touch 149207
class spell_corrupted_touch_aura : public AuraScript
{
    PrepareAuraScript(spell_corrupted_touch_aura);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_CORRUPTED_TOUCH_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_corrupted_touch_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Mighty Stomp 147638
class spell_siege_mighty_stomp : public SpellScript
{
    PrepareSpellScript(spell_siege_mighty_stomp);

    void HandleOnCast()
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_MIGHTY_STOMP_EFF, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_siege_mighty_stomp::HandleOnCast);
    }
};

// Resonating Amber 146499
class spell_resonating_amber : public SpellScript
{
    PrepareSpellScript(spell_resonating_amber);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_resonating_amber::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Ten Percent of Health 145683
class spell_siege_of_orgrimmar_ten_percent_of_health : public AuraScript
{
    PrepareAuraScript(spell_siege_of_orgrimmar_ten_percent_of_health);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->setRegeneratingHealth(false);
            owner->SetHealth((int32)(owner->GetMaxHealth() * 0.1f));
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_siege_of_orgrimmar_ten_percent_of_health::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Tidal Wave 149187
class spell_dragonmaw_tidal_wave : public SpellScript
{
    PrepareSpellScript(spell_dragonmaw_tidal_wave);

    std::list<WorldObject*> copyTargets;

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_1].BasePoints, true);
    }

    void HandleWaveTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && !GetCaster()->HasInArc(M_PI / 3, target); });
        
        if (!targets.empty())
            copyTargets.push_back(targets.front());
    }

    void FilterWaveTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dragonmaw_tidal_wave::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_dragonmaw_tidal_wave::HandleWaveTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dragonmaw_tidal_wave::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dragonmaw_tidal_wave::FilterWaveTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Fracture Eff 147202
class spell_bonecrusher_fracture_eff : public SpellScript
{
    PrepareSpellScript(spell_bonecrusher_fracture_eff);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_FRACTURE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bonecrusher_fracture_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Sniped! 146743
class spell_siege_of_orgrimmar_sniped : public AuraScript
{
    PrepareAuraScript(spell_siege_of_orgrimmar_sniped);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* sniper = owner->FindNearestCreature(NPC_KORKRON_ELITE_SNIPER, 70.0f, true))
                sniper->CastSpell(owner, SPELL_SNIPED);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_siege_of_orgrimmar_sniped::HandleOnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Blade Salvo 148962
class spell_siege_of_orgrimmar_blade_salvo : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_blade_salvo);

    void SelectTargets(SpellDestination& dest)
    {
        Position newPos = { 1393.31f, -4367.54f, 33.0f + frand(-2.0f, 2.0f), 0 };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_siege_of_orgrimmar_blade_salvo::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Bribe 145553
class spell_siege_of_orgrimmar_bribe : public AuraScript
{
    PrepareAuraScript(spell_siege_of_orgrimmar_bribe);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            if (owner->HealthBelowPct(GetSpellInfo()->Effects[EFFECT_5].BasePoints))
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_siege_of_orgrimmar_bribe::OnTrigger, EFFECT_5, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Fire Storm Selector 146053
class spell_siege_of_orgrimmar_fire_storm_selector : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_fire_storm_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_fire_storm_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Umbral Tempest 146167
class spell_siege_of_orgrimmar_umbral_tempest_selector : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_umbral_tempest_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_umbral_tempest_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Scorched Earth 146223
class spell_siege_of_orgrimmar_scorched_selector : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_scorched_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_scorched_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Skullspitters Rage 145937
class spell_siege_of_orgrimmar_skullspitters_rage : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_skullspitters_rage);

    std::list<WorldObject*> copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetEntry() != NPC_KORKRON_SKULL_SPLITTER && target->ToUnit() && !target->ToUnit()->IsInCombat(); });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_skullspitters_rage::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_skullspitters_rage::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_skullspitters_rage::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Acid Spray 146975
class spell_siege_of_orgrimmar_acid_spray_selector : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_acid_spray_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_acid_spray_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Infusion 147884
class spell_siege_of_orgrimmar_infusion : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_infusion);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ARCWEAVER_REINFORCEMENTS && target->GetEntry() != NPC_BRUTE_REINFORCEMENTS && target->GetEntry() != NPC_STOREROOM_GUARD; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_infusion::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_infusion::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Call Reinforcements 145119
class spell_siege_of_orgrimmar_call_reinforcements : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_call_reinforcements);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            caster->SummonCreature(urand(0, 1) ? NPC_ARCWEAVER_REINFORCEMENTS : NPC_BRUTE_REINFORCEMENTS, *GetHitDest(), TEMPSUMMON_MANUAL_DESPAWN);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_siege_of_orgrimmar_call_reinforcements::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Cannon Blast 146390 
class spell_siege_of_orgrimmar_cannon_blast : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_cannon_blast);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DRAGONMAW_TIDAL_SHAMAN && target->GetEntry() != NPC_DRAGONMAW_CANNONEER && target->GetEntry() != NPC_DRAGONMAW_BONECRUSHER && target->GetEntry() != NPC_DRAGONMAW_ELITE_GRUNT && target->GetEntry() != NPC_DRAGONMAW_FLAGBEATER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_cannon_blast::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Spike Mine Detonation 145752
class spell_siege_of_orgrimmar_spike_mine_detonation : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_spike_mine_detonation);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->DisappearAndDie();
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_BEACH_CANNON; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_spike_mine_detonation::HandleEffectHitTarget, EFFECT_4, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_spike_mine_detonation::FilterTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Portal to Orgrimmar 148032
class spell_siege_of_orgrimmar_portal_to_orgrimmar : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_portal_to_orgrimmar);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_LFR))
            PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_portal_to_orgrimmar::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Strength in Numbers 148741
class spell_siege_of_orgrimmar_strength_in_numbers_eff : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_strength_in_numbers_eff);

    std::list<WorldObject*> copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_KORKRON_IRON_SENTINEL; });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_strength_in_numbers_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_strength_in_numbers_eff::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_strength_in_numbers_eff::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Shredding Blast Selector 148572
class spell_siege_of_orgrimmar_shredding_blast_selector : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_shredding_blast_selector);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_shredding_blast_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Envenomed Shadowstep 148141
class spell_siege_of_orgrimmar_envenomed_shadowstep : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_envenomed_shadowstep);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_envenomed_shadowstep::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Fan of Knives 148140
class spell_siege_of_orgrimmar_fan_of_knives : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_fan_of_knives);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_fan_of_knives::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Drillstorm 146479
class spell_siege_of_orgrimmar_drillstorm : public AuraScript
{
    PrepareAuraScript(spell_siege_of_orgrimmar_drillstorm);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->ClearUnitState(UNIT_STATE_CASTING);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_siege_of_orgrimmar_drillstorm::OnAuraEffectApply, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Call of the Elements 147815
class spell_siege_of_orgrimmar_call_of_the_elements : public AuraScript
{
    PrepareAuraScript(spell_siege_of_orgrimmar_call_of_the_elements);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_THRALL_UNLEASHED);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_siege_of_orgrimmar_call_of_the_elements::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Attack Thrall 147831
class spell_siege_of_orgrimmar_attack_thrall : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_attack_thrall);

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature()) // we can`t use spell from dbc instant on effect hit, cuz it`ll prevented our knockback
            target->AI()->DoAction(ACTION_THRALL_KNELL);

        if (Creature* caster = GetCaster()->ToCreature())
            caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_attack_thrall::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Blood of Y`shaarj 147554
class spell_siege_of_orgrimmar_blood_of_yshaarj : public AuraScript
{
    PrepareAuraScript(spell_siege_of_orgrimmar_blood_of_yshaarj);

    bool Load() override
    {
        allowStack = true;
        return true;
    }

    void SetGuid(uint32 type, ObjectGuid guid) override
    {
        allowStack = type ? true : false;
    }

    void OnTrigger(AuraEffect const* aurEff)
    {
        if (Unit* target = GetOwner()->ToUnit())
        {
            if (aurEff->GetTickNumber() > 1 && allowStack)
            {
                SetStackAmount(GetStackAmount() + 1);
                RefreshDuration();
            }
        }
    }

    private:
        bool allowStack;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_siege_of_orgrimmar_blood_of_yshaarj::OnTrigger, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Empowering Corruption 147564
class spell_siege_of_orgrimmar_empowering_corruption : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_empowering_corruption);

    std::list<WorldObject*> copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ICHOR_OF_YSHAARJ && target->GetEntry() != NPC_KORKRON_REAPER && target->GetEntry() != NPC_HARBINGER_OF_YSHAARJ; });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_empowering_corruption::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_empowering_corruption::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_empowering_corruption::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Summon Blood of Y`shaarj eff 147567
class spell_siege_of_orgrimmar_summon_blood_of_yshaarj_eff : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_summon_blood_of_yshaarj_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (caster->AI()->GetData(TYPE_ICHOR_OF_YSHAARJ_COUNT) > 3)
                PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_siege_of_orgrimmar_summon_blood_of_yshaarj_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Reaper Charge 147640
class spell_siege_of_orgrimmar_reaper_charge : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_reaper_charge);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_REAPING_WHIRLWIND, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siege_of_orgrimmar_reaper_charge::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_CHARGE);
    }
};

// Dancing Flames 120348
class spell_siege_of_orgrimmar_dancing_flames : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_dancing_flames);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DANCING_FLAME; });

        if (targets.empty())
            return;

        targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
        targets.resize(1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_dancing_flames::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Slave Chains 145744
class spell_siege_of_orgrimmar_slave_chains : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_slave_chains);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_THERAMORE_CITIZEN; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_slave_chains::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Ground Slam 146621
class spell_siege_of_orgrimmar_ground_slam : public SpellScript
{
    PrepareSpellScript(spell_siege_of_orgrimmar_ground_slam);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_of_orgrimmar_ground_slam::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 1267. Summoned by 147184 - Swirl Zone
class sat_swirl_zone : public IAreaTriggerAura
{
    void OnInit() override
    {
        if (Unit* caster = GetCaster())
        {
            if (AreaTrigger* at = GetAreaTrigger())
            {
                float dist = 50.0f;
                float x, y;
                float angle = 0.0f;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_SWIRL_ZONE_NE:
                        angle = Position::NormalizeOrientation(caster->GetOrientation() - M_PI / 4);
                        break;
                    case SPELL_SWIRL_ZONE_N:
                        angle = caster->GetOrientation();
                        break;
                    case SPELL_SWIRL_ZONE_NW:
                        angle = Position::NormalizeOrientation(caster->GetOrientation() + M_PI / 4);
                        break;
                    case SPELL_SWIRL_ZONE_SW:
                        angle = Position::NormalizeOrientation(caster->GetOrientation() + M_PI - M_PI / 4);
                        break;
                    case SPELL_SWIRL_ZONE_SE:
                        angle = Position::NormalizeOrientation(caster->GetOrientation() + M_PI + M_PI / 4);
                        break;
                }

                if (!angle)
                    return;

                std::vector<Position> path;

                for (uint32 i = 1; i < 17; ++i)
                {
                    x = at->GetPositionX() + (((dist * i) / 17.0f) * cos(angle));
                    y = at->GetPositionY() + (((dist * i) / 17.0f) * sin(angle));
                    float z = at->GetPositionZ();
                    at->UpdateAllowedPositionZ(x, y, z);
                    path.push_back({ x, y, z });
                }

                at->InitSpline(path, at->GetDuration());
            }
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (!itr->HasAura(SPELL_RUSHING_WATERS_EFF))
                itr->CastSpell(itr, SPELL_RUSHING_WATERS_EFF, true);
    }
};

// 1298. Summoned by 147626 - Pressure Plate
class sat_pressure_plate : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_PRESSURE_PLATE_OVERRIDE, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_PRESSURE_PLATE_OVERRIDE);
    }
};

// 1230. Summoned by 146454 - Resonating Amber
class sat_resonating_amber : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_RESONATING_AMBER_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_RESONATING_AMBER_EFF);
    }
};

// 1203. Summoned by 145740 - Spike Mine
class sat_spike_mine : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(caster, SPELL_SPIKE_MINE_DETONATION, true);
    }
};

// 1310. Summoned by 147883 - Infusion
class sat_siege_of_orgrimmar_infusion : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_INFUSION_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_INFUSION_EFF);
    }
};

// 1309. Summoned by 147876 - Adrenaline Bomb
class sat_adrenaline_bomb : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->CastSpell(itr, SPELL_ADRENALINE_BOMB_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_ADRENALINE_BOMB_EFF);
    }
};

// 1231. Summoned by 146460 - Drill Charge
class sat_drill_charge : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_DRILLCHARGE_EFF, true);
    }
};

// 1289. Summoned by 147437 - Blood of Y`shaarj
class sat_blood_of_yshaarj : public IAreaTriggerAura
{
    void OnInit() override
    {
        count = 0;
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            count++;
            target->CastSpell(target, SPELL_BLOOD_OF_YSHAARJ_EFF, true);

            if (Aura* blood = target->GetAura(SPELL_BLOOD_OF_YSHAARJ_EFF))
                blood->SetScriptGuid(1, ObjectGuid::Empty);

            if (GetCaster() && GetCaster()->ToCreature()) // send timer to expire
                GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Aura* blood = target->GetAura(SPELL_BLOOD_OF_YSHAARJ_EFF))
                blood->SetScriptGuid(0, ObjectGuid::Empty);

            if (--count == 0 && GetCaster() && GetCaster()->ToCreature()) // send reset timer to expire
                GetCaster()->ToCreature()->AI()->DoAction(ACTION_RESET_YSHAARJ_TIMER);
        }
    }

    private:
        uint32 count;
};

// Areatrigger 9355
class AreaTrigger_at_siege_of_orgrimmar_entrance : public AreaTriggerScript
{
    public:
        AreaTrigger_at_siege_of_orgrimmar_entrance() : AreaTriggerScript("AreaTrigger_at_siege_of_orgrimmar_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (Creature* lorewalkerCho = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_LOREWALKER_CHO) : ObjectGuid::Empty))
                lorewalkerCho->AI()->DoAction(ACTION_START_INTRO);

            return false;
        }
};

// Areatrigger 9356
class AreaTrigger_at_siege_of_orgrimmar_corrupted_waters : public AreaTriggerScript
{
    public:
        AreaTrigger_at_siege_of_orgrimmar_corrupted_waters() : AreaTriggerScript("AreaTrigger_at_siege_of_orgrimmar_corrupted_waters") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (Creature* lorewalkerCho = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_LOREWALKER_CHO) : ObjectGuid::Empty))
                lorewalkerCho->AI()->DoAction(ACTION_UNDER_TORMENTED);
    
            return false;
        }
};

// Areatrigger 9316.
class AreaTrigger_at_soo_vault_of_yshaarj_entrance : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_vault_of_yshaarj_entrance() : AreaTriggerScript("AreaTrigger_at_soo_vault_of_yshaarj_entrance") { }
    
        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* norushen = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_NORUSHEN) : ObjectGuid::Empty))
                norushen->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9280.
class AreaTrigger_at_soo_forward_valley_of_strength : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_valley_of_strength() : AreaTriggerScript("AreaTrigger_at_soo_forward_valley_of_strength") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* aysa = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_OVERSEER_KOMAK) : ObjectGuid::Empty))
                aysa->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9272.
class AreaTrigger_at_soo_forward_dark_korkron_shamans : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_dark_korkron_shamans() : AreaTriggerScript("AreaTrigger_at_soo_forward_dark_korkron_shamans") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* eitrigg = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_EITRIGG) : ObjectGuid::Empty))
                eitrigg->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9357.
class AreaTrigger_at_soo_behind_galakras : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_behind_galakras() : AreaTriggerScript("AreaTrigger_at_soo_behind_galakras") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_GALAKRAS) == DONE)
                return false;

            if (player->HasAura(SPELL_SNIPED))
                return false;

            player->AddAura(SPELL_SNIPED, player);

            return true;
        }
};

// Areatrigger 9438.
class AreaTrigger_at_soo_forward_iron_juggernaut : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_iron_juggernaut() : AreaTriggerScript("AreaTrigger_at_soo_forward_iron_juggernaut") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* voljin = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_VOLJIN) : ObjectGuid::Empty))
                voljin->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9273.
class AreaTrigger_at_soo_behind_dark_korkron_shamans : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_behind_dark_korkron_shamans() : AreaTriggerScript("AreaTrigger_at_soo_behind_dark_korkron_shamans") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* runthak = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_OVERLORD_RUNTHAK) : ObjectGuid::Empty))
                runthak->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9271.
class AreaTrigger_at_soo_forward_cleft_of_shadow : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_cleft_of_shadow() : AreaTriggerScript("AreaTrigger_at_soo_forward_cleft_of_shadow") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* groldar = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_GROLDAR) : ObjectGuid::Empty))
                groldar->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9344.
class AreaTrigger_at_soo_forward_malkorok : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_malkorok() : AreaTriggerScript("AreaTrigger_at_soo_forward_malkorok") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* malkorok = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_MALKOROK) : ObjectGuid::Empty))
                malkorok->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9347.
class AreaTrigger_at_soo_forward_thok_bloodthirsty : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_thok_bloodthirsty() : AreaTriggerScript("AreaTrigger_at_soo_forward_thok_bloodthirsty") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* thok = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_THOK_THE_BLOODTHIRSTY) : ObjectGuid::Empty))
                thok->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9374.
class AreaTrigger_at_soo_forward_garrosh_hellscream : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_garrosh_hellscream() : AreaTriggerScript("AreaTrigger_at_soo_forward_garrosh_hellscream") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* thrall = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_THRALL_GARROSH_EVENT) : ObjectGuid::Empty))
                thrall->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9346.
class AreaTrigger_at_soo_behind_malkorok : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_behind_malkorok() : AreaTriggerScript("AreaTrigger_at_soo_behind_malkorok") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* korkronGunnerRP = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_KORKRON_GUNNER_RP) : ObjectGuid::Empty))
                korkronGunnerRP->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9563
class AreaTrigger_at_soo_forward_paragon_chamber : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_paragon_chamber() : AreaTriggerScript("AreaTrigger_at_soo_forward_paragon_chamber") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* saurfang = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_OVERLORD_SAURFANG) : ObjectGuid::Empty))
                saurfang->AI()->DoAction(ACTION_START_INTRO);

            // Initialize Paragon here
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetData(DATA_PARAGON_PRE_EVENT) != DONE)
                player->GetInstanceScript()->SetData(DATA_PARAGON_PRE_EVENT, DONE);

            return true;
        }
};

// Areatrigger 9352
class AreaTrigger_at_soo_forward_korkron_barracks : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_korkron_barracks() : AreaTriggerScript("AreaTrigger_at_soo_forward_korkron_barracks") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* farseer = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_KORKRON_DARK_FARSEER) : ObjectGuid::Empty))
                farseer->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

// Areatrigger 9353
class AreaTrigger_at_soo_forward_korkron_skullsplitter : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_korkron_skullsplitter() : AreaTriggerScript("AreaTrigger_at_soo_forward_korkron_skullsplitter") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* farseer = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_KORKRON_DARK_FARSEER + 1) : ObjectGuid::Empty))
                farseer->AI()->DoAction(ACTION_CORRUPTED_SKULLSPLITTER);

            return true;
        }
};

// Areatrigger 9278
class AreaTrigger_at_soo_forward_korkron_cage_prisoners : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_korkron_cage_prisoners() : AreaTriggerScript("AreaTrigger_at_soo_forward_korkron_cage_prisoners") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* cagePrisoner = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_DOYODA) : ObjectGuid::Empty))
                cagePrisoner->AI()->DoAction(ACTION_CAGE_PRISONERS_INTRO);
    
            return true;
        }
};

class cond_gamon_event : public ConditionScript
{
    public:
        cond_gamon_event() : ConditionScript("cond_gamon_event") { }

        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            if (source.mConditionTargets[0] && source.mConditionTargets[1])
                if (Creature* caster = source.mConditionTargets[1]->ToCreature())
                    if (Creature* target = source.mConditionTargets[0]->ToCreature())
                        if (target->GetPositionZ() > 37.5f)
                            return true;

            return false;
        }
};

void AddSC_siege_of_orgrimmar()
{
    new creature_script<npc_siege_of_orgrimmar_ooze_controller>("npc_siege_of_orgrimmar_ooze_controller");
    new creature_script<npc_siege_of_orgrimmar_lesser_sha_puddle>("npc_siege_of_orgrimmar_lesser_sha_puddle");
    new creature_script<npc_tormented_initiate>("npc_tormented_initiate");
    new creature_script<npc_fallen_pool_tender>("npc_fallen_pool_tender");
    new creature_script<npc_aqueous_defender>("npc_aqueous_defender");
    new creature_script<npc_siege_of_orgrimmar_lorewalker_cho>("npc_siege_of_orgrimmar_lorewalker_cho");
    new creature_script<npc_siege_of_orgrimmar_lorewalker_cho_norushen>("npc_siege_of_orgrimmar_lorewalker_cho_norushen");
    new creature_script<npc_vanity>("npc_vanity");
    new creature_script<npc_fragment_of_pride>("npc_fragment_of_pride");
    new creature_script<npc_amalgamated_hubris>("npc_amalgamated_hubris");
    new creature_script<npc_zeal>("npc_zeal");
    new creature_script<npc_lingering_corruption>("npc_lingering_corruption");
    new creature_script<npc_pressure_plate>("npc_pressure_plate");
    new creature_script<npc_siege_of_orgrimmar_portal_to_galakras>("npc_siege_of_orgrimmar_portal_to_galakras");
    new creature_script<npc_lady_jaina_proudmoore_sha_room>("npc_lady_jaina_proudmoore_sha_room");
    new creature_script<npc_dragonmaw_tidal_shaman>("npc_dragonmaw_tidal_shaman");
    new creature_script<npc_dragonmaw_bonecrusher>("npc_dragonmaw_bonecrusher");
    new creature_script<npc_dragonmaw_elite_grunt>("npc_dragonmaw_elite_grunt");
    new creature_script<npc_pressure_mine>("npc_pressure_mine");
    new creature_script<npc_siege_of_orgrimmar_korkron_elite_sniper>("npc_siege_of_orgrimmar_korkron_elite_sniper");
    new creature_script<npc_siege_of_orgrimmar_voljin>("npc_siege_of_orgrimmar_voljin");
    new creature_script<npc_siege_of_orgrimmar_tyrande>("npc_siege_of_orgrimmar_tyrande");
    new creature_script<npc_siege_of_orgrimmar_korkron_grunt_gates>("npc_siege_of_orgrimmar_korkron_grunt_gates");
    new creature_script<npc_siege_of_orgrimmar_aysa_cloudsinger>("npc_siege_of_orgrimmar_aysa_cloudsinger");
    new creature_script<npc_siege_of_orgrimmar_blind_blademaster>("npc_siege_of_orgrimmar_blind_blademaster");
    new creature_script<npc_siege_of_orgrimmar_korkron_dark_shaman>("npc_siege_of_orgrimmar_korkron_dark_shaman");
    new creature_script<npc_siege_of_orgrimmar_poison_bolt_totem>("npc_siege_of_orgrimmar_poison_bolt_totem");
    new creature_script<npc_siege_of_orgrimmar_korkron_grunt_2>("npc_siege_of_orgrimmar_korkron_grunt_2");
    new creature_script<npc_siege_of_orgrimmar_overseeker_mojka>("npc_siege_of_orgrimmar_overseeker_mojka");
    new creature_script<npc_siege_of_orgrimmar_treasure_guard>("npc_siege_of_orgrimmar_treasure_guard");
    new creature_script<npc_overseer_komak>("npc_overseer_komak");
    new creature_script<npc_siege_of_orgrimmar_mokvar_the_treasurer>("npc_siege_of_orgrimmar_mokvar_the_treasurer");
    new creature_script<npc_siege_of_orgrimmar_cage_prisoner>("npc_siege_of_orgrimmar_cage_prisoner");
    new creature_script<npc_siege_of_orgrimmar_resistance_totem>("npc_siege_of_orgrimmar_resistance_totem");
    new creature_script<npc_siege_of_orgrimmar_korkron_shadowmage>("npc_siege_of_orgrimmar_korkron_shadowmage");
    new creature_script<npc_siege_of_orgrimmar_korkron_overseer>("npc_siege_of_orgrimmar_korkron_overseer");
    new creature_script<npc_siege_of_orgrimmar_korkron_dire_wolf>("npc_siege_of_orgrimmar_korkron_dire_wolf");
    new creature_script<npc_siege_of_orgrimmar_eitrigg>("npc_siege_of_orgrimmar_eitrigg");
    new creature_script<npc_siege_of_orgrimmar_overlord_runthak>("npc_siege_of_orgrimmar_overlord_runthak");
    new creature_script<npc_siege_of_orgrimmar_hellscream_demolisher>("npc_siege_of_orgrimmar_hellscream_demolisher");
    new creature_script<npc_siege_of_orgrimmar_groldar>("npc_siege_of_orgrimmar_groldar");
    new creature_script<npc_siege_of_orgrimmar_doomlord>("npc_siege_of_orgrimmar_doomlord");
    new creature_script<npc_siege_of_orgrimmar_korkron_blood_axe>("npc_siege_of_orgrimmar_korkron_blood_axe");
    new creature_script<npc_siege_of_orgrimmar_korkron_gunner>("npc_siege_of_orgrimmar_korkron_gunner");
    new creature_script<npc_siege_of_orgrimmar_korkron_dark_farseer>("npc_siege_of_orgrimmar_korkron_dark_farseer");
    new creature_script<npc_siege_of_orgrimmar_korkron_dark_wolf>("npc_siege_of_orgrimmar_korkron_dark_wolf");
    new creature_script<npc_siege_of_orgrimmar_hellscream_annihilator>("npc_siege_of_orgrimmar_hellscream_annihilator");
    new creature_script<npc_siege_of_orgrimmar_korkron_skull_splitter>("npc_siege_of_orgrimmar_korkron_skull_splitter");
    new creature_script<npc_siege_of_orgrimmar_corrupted_skullsplitter>("npc_siege_of_orgrimmar_corrupted_skullsplitter");
    new creature_script<npc_siege_of_orgrimmar_korkron_iron_scorpion>("npc_siege_of_orgrimmar_korkron_iron_scorpion");
    new creature_script<npc_siege_of_orgrimmar_arcweaver_reinforcements>("npc_siege_of_orgrimmar_arcweaver_reinforcements");
    new creature_script<npc_siege_of_orgrimmar_brute_reinforcements>("npc_siege_of_orgrimmar_brute_reinforcements");
    new creature_script<npc_siege_of_orgrimmar_storeroom_guard>("npc_siege_of_orgrimmar_storeroom_guard");
    new creature_script<npc_siege_of_orgrimmar_korkron_jailer_thrash>("npc_siege_of_orgrimmar_korkron_jailer_thrash");
    new creature_script<npc_korkron_iron_sentinel>("npc_korkron_iron_sentinel");
    new creature_script<npc_siege_of_orgrimmar_korkron_gunner_rp_event>("npc_siege_of_orgrimmar_korkron_gunner_rp_event");
    new creature_script<npc_war_master_kragg>("npc_war_master_kragg");
    new creature_script<npc_siege_of_orgrimmar_thresher_turret>("npc_siege_of_orgrimmar_thresher_turret");
    new creature_script<npc_siege_of_orgrimmar_enraged_mushan_beast>("npc_siege_of_orgrimmar_enraged_mushan_beast");
    new creature_script<npc_siege_of_orgrimmar_starved_yeti>("npc_siege_of_orgrimmar_starved_yeti");
    new creature_script<npc_siege_of_orgrimmar_aggron>("npc_siege_of_orgrimmar_aggron");
    new creature_script<npc_siege_of_orgrimmar_blackfuse_korkron_overseer>("npc_siege_of_orgrimmar_blackfuse_korkron_overseer");
    new creature_script<npc_siege_of_orgrimmar_blackfuse_sellsword>("npc_siege_of_orgrimmar_blackfuse_sellsword");
    new creature_script<npc_siege_of_orgrimmar_blackfuse_engineer>("npc_siege_of_orgrimmar_blackfuse_engineer");
    new creature_script<npc_siege_of_orgrimmar_gorodan>("npc_siege_of_orgrimmar_gorodan");
    new creature_script<npc_siege_of_orgrimmar_shanna_sparkfizz>("npc_siege_of_orgrimmar_shanna_sparkfizz");
    new creature_script<npc_klaxxi_srathik_amber_master>("npc_klaxxi_srathik_amber_master");
    new creature_script<npc_klaxxi_korthik_honor_guard>("npc_klaxxi_korthik_honor_guard");
    new creature_script<npc_klaxxi_kovok>("npc_klaxxi_kovok");
    new creature_script<npc_klaxxi_grand_master_alchemist_kixen>("npc_klaxxi_grand_master_alchemist_kixen");
    new creature_script<npc_siege_of_orgrimmar_overlord_saurfang>("npc_siege_of_orgrimmar_overlord_saurfang");
    new creature_script<npc_siege_of_orgrimmar_thrall>("npc_siege_of_orgrimmar_thrall");
    new creature_script<npc_siege_of_orgrimmar_blood_of_yshaarj_trigger>("npc_siege_of_orgrimmar_blood_of_yshaarj_trigger");
    new creature_script<npc_siege_of_orgrimmar_ichor_of_yshaarj>("npc_siege_of_orgrimmar_ichor_of_yshaarj");
    new creature_script<npc_siege_of_orgrimmar_harbinger_of_yshaarj>("npc_siege_of_orgrimmar_harbinger_of_yshaarj");
    new creature_script<npc_siege_of_orgrimmar_korkron_reaper>("npc_siege_of_orgrimmar_korkron_reaper");
    new creature_script<npc_siege_of_orgrimmar_gamon>("npc_siege_of_orgrimmar_gamon");
    new creature_script<npc_siege_of_orgrimmar_dancing_flame>("npc_siege_of_orgrimmar_dancing_flame");
    new creature_script<npc_siege_of_orgrimmar_sha_cloud>("npc_siege_of_orgrimmar_sha_cloud");
    new creature_script<npc_siege_of_orgrimmar_korkron_prisoners>("npc_siege_of_orgrimmar_korkron_prisoners");
    new creature_script<npc_siege_of_orgrimmar_theramore_prisoners>("npc_siege_of_orgrimmar_theramore_prisoners");
    new go_siege_of_orgrimmar_korkron_cage();
    new go_siege_of_orgrimmar_siegecrafter_iron_door();

    new spell_script<spell_aqueous_defender_vortex>("spell_aqueous_defender_vortex");
    new spell_script<spell_aqueous_defender_vortex_eff>("spell_aqueous_defender_vortex_eff");
    new aura_script<spell_rushing_waters_eff>("spell_rushing_waters_eff");
    new spell_script<spell_growing_pride>("spell_growing_pride");
    new spell_script<spell_zeal_sha_smash>("spell_zeal_sha_smash");
    new spell_script<spell_corrupted_touch>("spell_corrupted_touch");
    new aura_script<spell_corrupted_touch_aura>("spell_corrupted_touch_aura");
    new spell_script<spell_siege_mighty_stomp>("spell_siege_mighty_stomp");
    new spell_script<spell_resonating_amber>("spell_resonating_amber");
    new aura_script<spell_siege_of_orgrimmar_ten_percent_of_health>("spell_siege_of_orgrimmar_ten_percent_of_health");
    new spell_script<spell_dragonmaw_tidal_wave>("spell_dragonmaw_tidal_wave");
    new spell_script<spell_bonecrusher_fracture_eff>("spell_bonecrusher_fracture_eff");
    new aura_script<spell_siege_of_orgrimmar_sniped>("spell_siege_of_orgrimmar_sniped");
    new spell_script<spell_siege_of_orgrimmar_blade_salvo>("spell_siege_of_orgrimmar_blade_salvo");
    new aura_script<spell_siege_of_orgrimmar_bribe>("spell_siege_of_orgrimmar_bribe");
    new spell_script<spell_siege_of_orgrimmar_fire_storm_selector>("spell_siege_of_orgrimmar_fire_storm_selector");
    new spell_script<spell_siege_of_orgrimmar_umbral_tempest_selector>("spell_siege_of_orgrimmar_umbral_tempest_selector");
    new spell_script<spell_siege_of_orgrimmar_scorched_selector>("spell_siege_of_orgrimmar_scorched_selector");
    new spell_script<spell_siege_of_orgrimmar_skullspitters_rage>("spell_siege_of_orgrimmar_skullspitters_rage");
    new spell_script<spell_siege_of_orgrimmar_acid_spray_selector>("spell_siege_of_orgrimmar_acid_spray_selector");
    new spell_script<spell_siege_of_orgrimmar_infusion>("spell_siege_of_orgrimmar_infusion");
    new spell_script<spell_siege_of_orgrimmar_call_reinforcements>("spell_siege_of_orgrimmar_call_reinforcements");
    new spell_script<spell_siege_of_orgrimmar_cannon_blast>("spell_siege_of_orgrimmar_cannon_blast");
    new spell_script<spell_siege_of_orgrimmar_spike_mine_detonation>("spell_siege_of_orgrimmar_spike_mine_detonation");
    new spell_script<spell_siege_of_orgrimmar_portal_to_orgrimmar>("spell_siege_of_orgrimmar_portal_to_orgrimmar");
    new spell_script<spell_siege_of_orgrimmar_strength_in_numbers_eff>("spell_siege_of_orgrimmar_strength_in_numbers_eff");
    new spell_script<spell_siege_of_orgrimmar_shredding_blast_selector>("spell_siege_of_orgrimmar_shredding_blast_selector");
    new spell_script<spell_siege_of_orgrimmar_envenomed_shadowstep>("spell_siege_of_orgrimmar_envenomed_shadowstep");
    new spell_script<spell_siege_of_orgrimmar_fan_of_knives>("spell_siege_of_orgrimmar_fan_of_knives");
    new aura_script<spell_siege_of_orgrimmar_drillstorm>("spell_siege_of_orgrimmar_drillstorm");
    new aura_script<spell_siege_of_orgrimmar_call_of_the_elements>("spell_siege_of_orgrimmar_call_of_the_elements");
    new spell_script<spell_siege_of_orgrimmar_attack_thrall>("spell_siege_of_orgrimmar_attack_thrall");
    new aura_script<spell_siege_of_orgrimmar_blood_of_yshaarj>("spell_siege_of_orgrimmar_blood_of_yshaarj");
    new spell_script<spell_siege_of_orgrimmar_empowering_corruption>("spell_siege_of_orgrimmar_empowering_corruption");
    new spell_script<spell_siege_of_orgrimmar_summon_blood_of_yshaarj_eff>("spell_siege_of_orgrimmar_summon_blood_of_yshaarj_eff");
    new spell_script<spell_siege_of_orgrimmar_reaper_charge>("spell_siege_of_orgrimmar_reaper_charge");
    new spell_script<spell_siege_of_orgrimmar_dancing_flames>("spell_siege_of_orgrimmar_dancing_flames");
    new spell_script<spell_siege_of_orgrimmar_slave_chains>("spell_siege_of_orgrimmar_slave_chains");
    new spell_script<spell_siege_of_orgrimmar_ground_slam>("spell_siege_of_orgrimmar_ground_slam");
    new atrigger_script<sat_swirl_zone>("sat_swirl_zone");
    new atrigger_script<sat_pressure_plate>("sat_pressure_plate");
    new atrigger_script<sat_resonating_amber>("sat_resonating_amber");
    new atrigger_script<sat_spike_mine>("sat_spike_mine");
    new atrigger_script<sat_siege_of_orgrimmar_infusion>("sat_siege_of_orgrimmar_infusion");
    new atrigger_script<sat_adrenaline_bomb>("sat_adrenaline_bomb");
    new atrigger_script<sat_drill_charge>("sat_drill_charge");
    new atrigger_script<sat_blood_of_yshaarj>("sat_blood_of_yshaarj");
    new AreaTrigger_at_siege_of_orgrimmar_entrance();
    new AreaTrigger_at_siege_of_orgrimmar_corrupted_waters();
    new AreaTrigger_at_soo_vault_of_yshaarj_entrance();
    new AreaTrigger_at_soo_forward_valley_of_strength();
    new AreaTrigger_at_soo_forward_dark_korkron_shamans();
    new AreaTrigger_at_soo_behind_galakras();
    new AreaTrigger_at_soo_forward_iron_juggernaut();
    new AreaTrigger_at_soo_behind_dark_korkron_shamans();
    new AreaTrigger_at_soo_forward_cleft_of_shadow();
    new AreaTrigger_at_soo_forward_malkorok();
    new AreaTrigger_at_soo_forward_thok_bloodthirsty();
    new AreaTrigger_at_soo_forward_garrosh_hellscream();
    new AreaTrigger_at_soo_behind_malkorok();
    new AreaTrigger_at_soo_forward_paragon_chamber();
    new AreaTrigger_at_soo_forward_korkron_barracks();
    new AreaTrigger_at_soo_forward_korkron_skullsplitter();
    new AreaTrigger_at_soo_forward_korkron_cage_prisoners();
    new cond_gamon_event();
}