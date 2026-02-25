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
#include "ScriptedCreature.h"
#include "siege_of_orgrimmar.h"
#include "Vehicle.h"

enum Spells
{
    // Galakras
    SPELL_PULSING_FLAMES                       = 147042,
    SPELL_PULSING_FLAMES_EFF                   = 147043,
    SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF     = 147029,
    SPELL_FLAMES_OF_GALAKROND_ANN              = 148778,
    SPELL_FLAMES_OF_GALAKROND_MARK             = 147068,
    SPELL_FLAMES_OF_GALAKROND_EFF              = 146992,
    SPELL_FLAMES_OF_GALAKROND_AT               = 146991,
    SPELL_EJECT_ALL_PASSENGERS                 = 50630, // for dbm
                                               
    // Misc                                    
    SPELL_IN_A_TOWER                           = 147317,
    SPELL_TOWER_STRENGTH_S                     = 148119,
    SPELL_TOWER_STRENGTH_N                     = 148120,
    SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_S = 147897,
    SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_N = 147916,
    SPELL_GOT_THE_BOMB                         = 147966,
    SPELL_ANTI_AIR_CANNON                      = 146488,
    SPELL_ANTI_AIR_CANNON_EFF                  = 147514,
    SPELL_FIRE_BALL                            = 11839,
    SPELL_RIFFLE_SHOT                          = 147990,
    SPELL_JUMP                                 = 148887,
                                               
    // Dragonmaw Flameslinger                  
    SPELL_FLAME_ARROWS                         = 146763,
    SPELL_FLAME_ARROWS_EFF                     = 146764,
    SPELL_FLAME_ARROWS_AT_EFF                  = 146765,
    SPELL_FLAME_ARROWS_SELECTOR                = 147438,
                                               
    // Ranking Officers                        
    SPELL_SHATTERING_CLEAVE                    = 146849,
    SPELL_SKULL_CRACKER                        = 146848,
    SPELL_CRUSHERS_CALL                        = 146780, // pull
    SPELL_CRUSHERS_CALL_SELECTOR               = 146769,
    SPELL_CRUSHERS_CALL_CHAIN                  = 146778,
    SPELL_CRUSHERS_CALL_CHAIN_EFF              = 146775,
    SPELL_POISON_CLOUD_EFF                     = 147705,
    SPELL_POISON_CLOUD_AT                      = 147706,
    SPELL_POISON_BLAST                         = 146606,
    SPELL_POISON_TIPPED_BLADES_EFF             = 146902,
    SPELL_CURSE_OF_VENOM                       = 147711,
    SPELL_SHOOT                                = 146773,
    SPELL_MUZZLE_SPRAY_EFF                     = 147824,
    SPELL_MUZZLE_SPRAY                         = 147825,
    SPELL_THUNDER_CLAP                         = 147683,
    SPELL_ARCING_SMASH                         = 147688,

    // Any Dragonmaw Solders
    SPELL_SHATTERING_ROAR                      = 147204,
    SPELL_FRACTURE                             = 146899,
    SPELL_FRACTURE_EFF                         = 146901,
    SPELL_WAR_BANNER                           = 146736,
    SPELL_WAR_BANNER_EFF                       = 146735,
    SPELL_WAR_BANNER_AT                        = 147328,
    SPELL_CHAIN_HEAL                           = 146757,
    SPELL_HEALING_TIDE_TOTEM                   = 146753,
    SPELL_HEALING_TIDE_PROC                    = 146725,
    SPELL_HEALING_TIDE_PROC_EFF                = 146726,
    SPELL_TIDAL_WAVE                           = 148522,
    SPELL_TIDAL_WAVE_VISUAL                    = 148519,
    SPELL_TIDAL_WAVE_EFF                       = 147820,
    SPELL_DRAGONS_CLEAVE                       = 148025,
    SPELL_THROW_AXE                            = 147669,
    SPELL_FLAME_BREATH                         = 146776,
    SPELL_FLAME_BREATH_EFF                     = 146779,
    SPELL_DRAKE_FIRE                           = 148352,
    SPELL_DRAKE_FIRE_EFF                       = 148560,
    SPELL_DRAKE_FIRE_SELECTOR                  = 148351,
    SPELL_BOMBARD                              = 148302,
    SPELL_BOMBARD_EFF                          = 148311,
    SPELL_SHADOW_STALK                         = 146864,
    SPELL_SHADOW_ASSAULT                       = 146868,
    SPELL_SHADOW_ASSAULT_EFF                   = 146872,
    SPELL_CHARGE                               = 100,

    // Ally
    SPELL_BLIZZARD_AT                          = 146782,
    SPELL_BLIZZARD_AT_EFF                      = 147431,
    SPELL_FLAMESTRIKE                          = 148849,
    SPELL_FLAMESTRIKE_EFF                      = 148848,
    SPELL_LORTHEMAR_CLEAVE                     = 148321,
    SPELL_LORTHEMAR_ARCANE_SHOT                = 148232,
    SPELL_SYLVANAS_MULTI_SHOT                  = 146772,
    SPELL_SYLVANAS_SHOT                        = 146768,
    SPELL_AETHAS_FIREBALL                      = 146786,
    SPELL_VARIAN_CLEAVE                        = 146741,
    SPELL_JAINA_FROST_BOLT                     = 146781,
    SPELL_JAINA_SUMMON_WATER_ELEMENTAL         = 146783,

    // Heroic
    SPELL_FIXATE                               = 148243,
};

enum Events
{
    EVENT_PULSING_FLAMES = 1,
    EVENT_FLAMES_OF_GALAKROND,
    EVENT_CHECK_WIPE,
    EVENT_NEXT_WAVE,
    EVENT_SIEGE_TOWER,
    EVENT_SECOND_PHASE,
    EVENT_SUMMON_TOWER_GRUNT,

    // Dragonmaw Defenders
    EVENT_SHATTERING_ROAR,
    EVENT_FRACTURE,
    EVENT_WAR_BANNER,
    EVENT_CHAIN_HEAL,
    EVENT_TIDAL_WAVE,
    EVENT_HEALING_TIDE_TOTEM,
    EVENT_FLAME_ARROWS,
    EVENT_DRAGONS_CLEAVE,
    EVENT_POISON_CLOUD,
    EVENT_TIPPED_BLADES,
    EVENT_SNAKE_TRANSFORMATION,
    EVENT_POISON_BLAST,
    EVENT_RANGE_ASSIST,
    EVENT_THROW_AXE,
    EVENT_FLAMESTRIKE,
    EVENT_FROST_RING,
    EVENT_SHOOT,
    EVENT_MUZZLE_SPRAY,
    EVENT_THUNDER_CLAP,
    EVENT_ARCING_SMASH,
    EVENT_FLAME_BREATH,
    EVENT_DRAKE_FIRE,
    EVENT_CALL_DEMOLISHER,
    EVENT_BOMBARD,
    EVENT_SHATTERED_CLEAVE,
    EVENT_SKULLCRACKER,
    EVENT_SHADOW_STALK,
    EVENT_LORTHEMAR_CLEAVE,
    EVENT_LORTHEMAR_ARCANE_SHOT,
    EVENT_SYLVANAS_MULTI_SHOT,
    EVENT_SYLVANAS_SHOT,
    EVENT_AETHAS_FIREBALL,
    EVENT_JAINA_WATER_ELEMENTAL,
    EVENT_JAINA_FROST_BOLT,
    EVENT_VARIAN_CLEAVE,
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
};

enum Actions
{
    ACTION_SIEGE_SOUTH_TOWER = 1,
    ACTION_SIEGE_NORTH_TOWER,
    ACTION_SOUTH_TOWER_IS_BREAK,
    ACTION_NORTH_TOWER_IS_BREAK,
    ACTION_BRING_DOWN,
    ACTION_GALAKRAS_DEFEAT,
    ACTION_FACTIONS_EVADE,
    ACTION_IN_TOWER_S,
    ACTION_IN_TOWER_N,
    ACTION_CALL_DEMOLISHER,
    ACTION_RECALL_EXPERT,
};

enum TypeData
{
    TYPE_SOUTH_TOWER,
    TYPE_SOUTH_TOWER_COUNTER,
    TYPE_NORTH_TOWER,
    TYPE_NORTH_TOWER_COUNTER,
    TYPE_SOUTH_TOWER_INFLUENCE,
    TYPE_SOUTH_TOWER_INFLUENCE_COUNT,
    TYPE_NORTH_TOWER_INFLUENCE,
    TYPE_NORTH_TOWER_INFLUENCE_COUNT,
    TYPE_AIR_CANNON_ACTIVE,
};

const std::multimap<uint32, Position> invSouthTowerDefendersType =
{
    { NPC_LIEUTENANT_KRUGRUK,     { 1363.16f, -4840.75f, 71.92f, 1.95f } },
    { NPC_DRAGONMAW_FLAMESLINGER, { 1378.73f, -4846.43f, 71.78f, 5.20f } },
    { NPC_DRAGONMAW_FLAMESLINGER, { 1366.40f, -4854.79f, 71.75f, 4.91f } },
    { NPC_DRAGONMAW_FLAMESLINGER, { 1363.98f, -4856.54f, 71.57f, 5.28f } },
    { NPC_DRAGONMAW_FLAMESLINGER, { 1352.89f, -4838.47f, 43.86f, 6.21f } },
    { NPC_DRAGONMAW_FLAMESLINGER, { 1363.43f, -4830.00f, 37.71f, 4.50f } },
    { NPC_DRAGONMAW_GRUNT,        { 1363.14f, -4839.79f, 33.13f, 5.34f } },
};

const std::multimap<uint32, Position> invNorthTowerDefendersType =
{
    { NPC_MASTER_CANNONEER_DAGRYN, { 1454.31f, -4803.16f, 68.54f, 0.47f } },
    { NPC_DRAGONMAW_FLAMESLINGER,  { 1448.68f, -4814.89f, 68.38f, 4.01f } },
    { NPC_DRAGONMAW_FLAMESLINGER,  { 1451.18f, -4816.37f, 68.54f, 4.31f } },
    { NPC_DRAGONMAW_FLAMESLINGER,  { 1464.84f, -4821.24f, 68.46f, 4.44f } },
    { NPC_DRAGONMAW_FLAMESLINGER,  { 1458.17f, -4795.58f, 40.45f, 5.20f } },
    { NPC_DRAGONMAW_FLAMESLINGER,  { 1470.42f, -4801.08f, 34.23f, 3.62f } },
    { NPC_DRAGONMAW_GRUNT,         { 1461.85f, -4805.22f, 29.75f, 4.22f } },
};

const std::map<uint32, std::array<uint32, 6>> invWaveType =
{
    { 1, { NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_BONECRUSHER, NPC_GALAKRAS_DRAGONMAW_BONECRUSHER,  0 } },
    { 2, { NPC_DRAGONMAW_FLAMESLINGER,          NPC_DRAGONMAW_FLAMESLINGER,          NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_GRUNT,       NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN } },
    { 3, { NPC_KORGRA_THE_SNAKE,                NPC_GALAKRAS_DRAGONMAW_EBON_STALKER, NPC_GALAKRAS_DRAGONMAW_EBON_STALKER, 0,                                  0,                                   0 } },
    { 4, { NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  0,                                   0,                                  0,                                   0 } },
    { 5, { NPC_HIGH_ENFORCER_THRANOK,           NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN,                  0,                 0 } },
    { 6, { NPC_GALAKRAS_DRAGONMAW_BONECRUSHER,  NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_GRUNT,        0,                                  0,                                   0 } },
    { 7, { NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_DRAGONMAW_FLAMESLINGER,          NPC_DRAGONMAW_FLAMESLINGER,         NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN, 0 } },
    { 8, { NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN, NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_BONECRUSHER, 0,                                   0 } },
    { 9, { NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  0,                                   0,                                  0,                                   0 } },
    // next just repeat 6-9
};

const std::map<uint32, std::array<uint32, 6>> invWaveTypeFlex =
{
    { 1, { NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_GRUNT,        0 ,        NPC_GALAKRAS_DRAGONMAW_BONECRUSHER, 0,  0 } },
    { 2, { NPC_DRAGONMAW_FLAMESLINGER,          NPC_DRAGONMAW_FLAMESLINGER,          0,        NPC_GALAKRAS_DRAGONMAW_GRUNT,       0,   NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN } },
    { 3, { NPC_KORGRA_THE_SNAKE,                NPC_GALAKRAS_DRAGONMAW_EBON_STALKER, 0, 0,                                  0,                                   0 } },
    { 4, { NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  0,                                   0,                                  0,                                   0 } },
    { 5, { NPC_HIGH_ENFORCER_THRANOK,           NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   0,        NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN,                  0,                 0 } },
    { 6, { NPC_GALAKRAS_DRAGONMAW_BONECRUSHER,  NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   0,        0,                                  0,                                   0 } },
    { 7, { NPC_GALAKRAS_DRAGONMAW_GRUNT,        NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_DRAGONMAW_FLAMESLINGER,          0,         NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN, 0 } },
    { 8, { NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN, NPC_GALAKRAS_DRAGONMAW_FLAGBEARER,   NPC_GALAKRAS_DRAGONMAW_GRUNT,        0, 0,                                   0 } },
    { 9, { NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE,  0,                                   0,                                  0,                                   0 } },
    // This is for the Flex Raid Up to < 15 players - On Flex we do not have the same amount of mobs spawan
};

class boss_galakras : public CreatureScript
{
    public:
        boss_galakras() : CreatureScript("boss_galakras") { }

        struct boss_galakrasAI : public BossAI
        {
            boss_galakrasAI(Creature* creature) : BossAI(creature, DATA_GALAKRAS) 
            {
                hasPreEventDone = false;
            }

            EventMap berserkerEvents;
            Position pos;
            uint32 southTowerData;
            uint32 southTowerProgressData;
            uint32 northTowerData;
            uint32 northTowerProgressData;
            uint32 southInfluenceData;
            uint32 southInfluenceProgressData;
            uint32 northInfluenceData;
            uint32 northInfluenceProgressData;
            uint32 waveCounter;
            ObjectGuid flamesTargetGUID;
            bool hasEvade;
            bool hasPreEventDone;
            bool hasHitByCannon;

            void Reset() override
            {
                _Reset();

                events.Reset();
                berserkerEvents.Reset();
                southTowerData             = 0;
                northTowerData             = 0;
                southTowerProgressData     = 0;
                northTowerProgressData     = 0;
                southInfluenceData         = 0;
                southInfluenceProgressData = 100;
                northInfluenceData         = 0;
                northInfluenceProgressData = 100;
                flamesTargetGUID = ObjectGuid::Empty;
                waveCounter                = 0;
                hasEvade                   = false;
                hasHitByCannon             = false;

                me->GetMap()->SetWorldState(WORLDSTATE_IMMORTAL_VANGUARD, 1);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
                me->SetAnimTier(AnimTier::Fly);
                me->OverrideInhabitType(INHABIT_AIR);
                me->UpdateMovementFlags();

                scheduler
                    .Schedule(Milliseconds(1500), [this](TaskContext context)
                {
                    if (Creature* zeala = me->SummonCreature(NPC_ZAELA, *me, TEMPSUMMON_MANUAL_DESPAWN))
                        zeala->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
                });

                scheduler
                    .Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    // in case: sometimes galakras could move in air instead fly
                    me->SetAnimTier(AnimTier::Fly);
                    me->OverrideInhabitType(INHABIT_AIR);
                    me->UpdateMovementFlags();

                    pos = me->GetRandomPoint({ galakrasFlyPoint.GetPositionX(), galakrasFlyPoint.GetPositionY(), galakrasFlyPoint.GetPositionZ(), galakrasFlyPoint.GetOrientation() }, 80.0f);
                    me->GetMotionMaster()->MovePoint(0, pos.GetPositionX(), pos.GetPositionY(), galakrasFlyPoint.GetPositionZ());

                    context.Repeat(Milliseconds(me->GetSplineDuration()));
                });

                if (hasPreEventDone)
                    return;

                hasPreEventDone = true;

                scheduler
                    .Schedule(Milliseconds(2500), [this](TaskContext context)
                {
                    if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                        zaela->AI()->Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(19500), [this](TaskContext context)
                {
                    if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                        zaela->AI()->Talk(TALK_SPECIAL_1);
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                me->GetMotionMaster()->Clear();
                scheduler.CancelAll();

                scheduler
                    .Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    pos = me->GetRandomPoint({ galakrasFlyPoint.GetPositionX(), galakrasFlyPoint.GetPositionY(), galakrasFlyPoint.GetPositionZ(), galakrasFlyPoint.GetOrientation() }, 80.0f);
                    me->GetMotionMaster()->MovePoint(0, pos.GetPositionX(), pos.GetPositionY(), galakrasFlyPoint.GetPositionZ());

                    context.Repeat(Milliseconds(me->GetSplineDuration()));
                });

                if (IsHeroic())
                    berserkerEvents.ScheduleEvent(EVENT_SUMMON_TOWER_GRUNT, 6 * IN_MILLISECONDS);
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return flamesTargetGUID;
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                            zaela->AI()->Talk(TALK_SPECIAL_3);

                        if (Creature* sylvana = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, sylvana);

                        if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LORTHEMAR_THERON) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, lorthemar);

                        if (Creature* aethas = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ARCHMAGE_AETHAS_SUNREAVER) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, aethas);

                        if (Creature* jaina = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, jaina);

                        if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, varian);

                        if (Creature* vereesa = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_VEREESA_WINDRUNNER) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, vereesa);

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        me->SetInCombatWithZone();

                        if (instance)
                            instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS, 1);

                        berserkerEvents.ScheduleEvent(EVENT_SIEGE_TOWER, 1 * IN_MILLISECONDS);
                        berserkerEvents.ScheduleEvent(EVENT_NEXT_WAVE, 1.5 * IN_MILLISECONDS);
                        break;
                    case ACTION_BRING_DOWN:
                        if (hasHitByCannon)
                            break;

                        hasHitByCannon = true;
                        Talk(TALK_SPECIAL_1);
                        scheduler.CancelAll();
                        berserkerEvents.CancelEvent(EVENT_SIEGE_TOWER);
                        berserkerEvents.CancelEvent(EVENT_NEXT_WAVE);

                        if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                        {
                            zaela->ExitVehicle();
                            zaela->AI()->Talk(TALK_SPECIAL_14); // Announce
                            zaela->AI()->Talk(TALK_SPECIAL_15);
                            zaela->DespawnOrUnsummon(3 * IN_MILLISECONDS);
                            DoCast(me, SPELL_EJECT_ALL_PASSENGERS);
                        }

                        me->GetMotionMaster()->MovePoint(0, galakrasWavePoint[0]);
                        berserkerEvents.ScheduleEvent(EVENT_SECOND_PHASE, me->GetSplineDuration());
                        break;
                    case ACTION_CALL_DEMOLISHER:
                        berserkerEvents.ScheduleEvent(EVENT_CALL_DEMOLISHER, 9.5 * IN_MILLISECONDS);
                        break;
                    case ACTION_RECALL_EXPERT:
                        berserkerEvents.ScheduleEvent(EVENT_SIEGE_TOWER, 1 * IN_MILLISECONDS);
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (hasEvade)
                    return;

                hasEvade = true;

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_GALAKRAS, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PULSING_FLAMES_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FLAME_ARROWS_AT_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IN_A_TOWER);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    // Worldstates
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS_COUNT, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS_COUNT, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE_COUNT, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE_COUNT, 0);

                    if (Creature* sylvana = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                        sylvana->AI()->DoAction(ACTION_FACTIONS_EVADE);

                    if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LORTHEMAR_THERON) : ObjectGuid::Empty))
                        lorthemar->AI()->DoAction(ACTION_FACTIONS_EVADE);

                    if (Creature* aethas = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ARCHMAGE_AETHAS_SUNREAVER) : ObjectGuid::Empty))
                        aethas->AI()->DoAction(ACTION_FACTIONS_EVADE);

                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                        jaina->AI()->DoAction(ACTION_FACTIONS_EVADE);

                    if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                        varian->AI()->DoAction(ACTION_FACTIONS_EVADE);

                    if (Creature* vereesa = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_VEREESA_WINDRUNNER) : ObjectGuid::Empty))
                        vereesa->AI()->DoAction(ACTION_FACTIONS_EVADE);

                    if (GameObject* towerDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_PORTCULLIS)))
                        towerDoor->SetGoState(GO_STATE_READY);

                    if (GameObject* towerDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_PORTCULLIS + 1)))
                        towerDoor->SetGoState(GO_STATE_READY);

                    // Lock Cannons
                    if (Creature* southCannon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANTI_AIR_CANNON)))
                        southCannon->AI()->SetData(TYPE_AIR_CANNON_ACTIVE, 0);

                    if (Creature* northCannon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANTI_AIR_CANNON + 1)))
                        northCannon->AI()->SetData(TYPE_AIR_CANNON_ACTIVE, 0);

                    // Lock Rope
                    if (Creature* sRope = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ROPE)))
                        sRope->SetVisible(false);

                    if (Creature* nRope = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ROPE + 1)))
                        nRope->SetVisible(false);
                }

                berserkerEvents.Reset();
                summons.DespawnAll();
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PULSING_FLAMES_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FLAME_ARROWS_AT_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IN_A_TOWER);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    // Worldstates
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS_COUNT, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS_COUNT, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE_COUNT, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE_COUNT, 0);

                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_LADY_JAINA_PROUDMOORE)))
                    {
                        jaina->AI()->DoAction(ACTION_GALAKRAS_DEFEAT);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, jaina);
                    }

                    if (Creature* lorthemar = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LORTHEMAR_THERON) : ObjectGuid::Empty))
                    {
                        lorthemar->AI()->DoAction(ACTION_GALAKRAS_DEFEAT);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, lorthemar);
                    }

                    if (Creature* aethas = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ARCHMAGE_AETHAS_SUNREAVER)))
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, aethas);

                    if (Creature* sylvana = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, sylvana);

                    if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, varian);

                    if (Creature* vereesa = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_VEREESA_WINDRUNNER) : ObjectGuid::Empty))
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, vereesa);

                    if (GameObject* towerDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_PORTCULLIS)))
                        towerDoor->SetGoState(GO_STATE_READY);                     
                                                                                   
                    if (GameObject* towerDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_PORTCULLIS + 1)))
                        towerDoor->SetGoState(GO_STATE_READY);

                    // Lock Cannons
                    if (Creature* southCannon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANTI_AIR_CANNON)))
                        southCannon->AI()->SetData(TYPE_AIR_CANNON_ACTIVE, 0);

                    if (Creature* northCannon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ANTI_AIR_CANNON + 1)))
                        northCannon->AI()->SetData(TYPE_AIR_CANNON_ACTIVE, 0);

                    // Lock Rope
                    if (Creature* sRope = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ROPE)))
                        sRope->SetVisible(false);

                    if (Creature* nRope = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_ROPE + 1)))
                        nRope->SetVisible(false);
                }

                berserkerEvents.Reset();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_GALAKRAS_DRAGONMAW_GRUNT_2)
                    return;

                // Ignore combat assistant for flameslingers INNER tower
                if (summon->GetPositionZ() < 24.5f || (summon->GetEntry() == NPC_DRAGONMAW_FLAMESLINGER && summon->GetPositionZ() > 44.0f))
                    summon->SetInCombatWithZone();
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_SOUTH_TOWER:
                        return southTowerData;
                    case TYPE_SOUTH_TOWER_COUNTER:
                        return southTowerProgressData;
                    case TYPE_NORTH_TOWER:
                        return northTowerData;
                    case TYPE_NORTH_TOWER_COUNTER:
                        return northTowerProgressData;
                    case TYPE_SOUTH_TOWER_INFLUENCE:
                        return southInfluenceData;
                    case TYPE_SOUTH_TOWER_INFLUENCE_COUNT:
                        return southInfluenceProgressData;
                    case TYPE_NORTH_TOWER_INFLUENCE:
                        return northInfluenceData;
                    case TYPE_NORTH_TOWER_INFLUENCE_COUNT:
                        return northInfluenceProgressData;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override 
            {
                switch (type)
                {
                    case TYPE_SOUTH_TOWER:
                        southTowerData = data;
                        break;
                    case TYPE_SOUTH_TOWER_COUNTER:
                    {
                        southTowerProgressData = data;

                        if (!instance)
                            break;

                        if (southTowerProgressData < 100)
                            instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS_COUNT, data);
                        else // door unlock
                        {
                            instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS_COUNT, 100);
                            instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE, 1);
                            instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE_COUNT, 100);

                            // open door
                            if (GameObject* towerDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_PORTCULLIS)))
                                towerDoor->SetGoState(GO_STATE_ACTIVE);

                            // Tower Citizen
                            for (auto&& itr : invSouthTowerDefendersType)
                                if (Creature* towerDefender = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                                    towerDefender->AI()->DoAction(ACTION_IN_TOWER_S);

                            if (IsHeroic())
                                berserkerEvents.RescheduleEvent(EVENT_SUMMON_TOWER_GRUNT, 24 * IN_MILLISECONDS);
                        }
                        break;
                    }
                    case TYPE_NORTH_TOWER:
                        northTowerData = data;
                        break;
                    case TYPE_NORTH_TOWER_COUNTER:
                        northTowerProgressData = data;

                        if (!instance)
                            break;

                        if (northTowerProgressData < 100)
                            instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS_COUNT, data);
                        else // door unlock
                        {
                            instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE, 0);
                            instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS_COUNT, 100);
                            instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE, 1);
                            instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE_COUNT, 100);

                            // open door
                            if (GameObject* towerDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_PORTCULLIS + 1)))
                                towerDoor->SetGoState(GO_STATE_ACTIVE);

                            // Tower Citizen
                            for (auto&& itr : invNorthTowerDefendersType)
                                if (Creature* towerDefender = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                                    towerDefender->AI()->DoAction(ACTION_IN_TOWER_N);

                            if (IsHeroic())
                                berserkerEvents.CancelEvent(EVENT_SUMMON_TOWER_GRUNT);
                        }
                        break;
                    case TYPE_SOUTH_TOWER_INFLUENCE:
                        southInfluenceData = data;
                        break;
                    case TYPE_SOUTH_TOWER_INFLUENCE_COUNT:
                    {
                        southInfluenceProgressData -= data;

                        instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS, 0);
                        instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_INFLUENCE_COUNT, southInfluenceProgressData);
                        if (southInfluenceProgressData)
                            break;

                        // South tower got by players Controll
                        instance->DoUpdateWorldState(WORLDSTATE_SOUTH_TOWER_PROGRESS, 0);

                        // Announce
                        if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                            zaela->AI()->Talk(TALK_SPECIAL_10);

                        // Unlock Tower Gun
                        if (Creature* cannon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANTI_AIR_CANNON) : ObjectGuid::Empty))
                            cannon->AI()->SetData(TYPE_AIR_CANNON_ACTIVE, 1);

                        // Unlock Rope
                        if (Creature* rope = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROPE) : ObjectGuid::Empty))
                            rope->SetVisible(true);
                        break;
                    }
                    case TYPE_NORTH_TOWER_INFLUENCE:
                        northInfluenceData = data;
                        break;
                    case TYPE_NORTH_TOWER_INFLUENCE_COUNT:
                    {
                        northInfluenceProgressData -= data;

                        instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS, 0);
                        instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_INFLUENCE_COUNT, northInfluenceProgressData);
                        if (northInfluenceProgressData)
                            break;

                        // North tower got by players Controll
                        instance->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS, 0);

                        // Announce
                        if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                            zaela->AI()->Talk(TALK_SPECIAL_12);

                        // Unlock Tower Gun
                        if (Creature* cannon = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ANTI_AIR_CANNON + 1) : ObjectGuid::Empty))
                            cannon->AI()->SetData(TYPE_AIR_CANNON_ACTIVE, 1);

                        // Unlock Rope
                        if (Creature* rope = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ROPE + 1) : ObjectGuid::Empty))
                            rope->SetVisible(true);
                        break;
                    }
                }
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
                        case EVENT_SIEGE_TOWER:
                            if (GetData(TYPE_SOUTH_TOWER) && GetData(TYPE_NORTH_TOWER))
                                break;

                            if (IsHeroic() && me->FindNearestCreature(NPC_DEMOLITIONS_EXPERT, 500.0f, true)) // in case if expert has die
                            {
                                berserkerEvents.RescheduleEvent(EVENT_SIEGE_TOWER, 2 * IN_MILLISECONDS);
                                break;
                            }

                            if (Creature* expert = me->SummonCreature(NPC_DEMOLITIONS_EXPERT, demolotionsSiegeSouthPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                                expert->AI()->DoAction(GetData(TYPE_SOUTH_TOWER) ? ACTION_SIEGE_NORTH_TOWER : ACTION_SIEGE_SOUTH_TOWER);

                            berserkerEvents.ScheduleEvent(EVENT_SIEGE_TOWER, 2 * MINUTE * IN_MILLISECONDS);
                            break;
                        case EVENT_NEXT_WAVE:
                        {
                            if (++waveCounter > 9) // after 9td wave just repeat last 4
                                waveCounter = 6;

                            auto key = invWaveType.find(waveCounter);
                            auto keyflex = invWaveTypeFlex.find(waveCounter);

                            if (instance && instance->GetData(DATA_FLEX) && instance->instance->GetPlayersCountExceptGMs() < 15) // If we have less than 15 players on flex, use flex script
                            {
                                    if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                                        zaela->AI()->Talk(urand(TALK_SPECIAL_4, TALK_SPECIAL_5));
                                for (uint8 i = 0; i < 6; i++)
                                    if (keyflex->second[i])
                                        me->SummonCreature(keyflex->second[i], galakrasWavePoint[i], TEMPSUMMON_MANUAL_DESPAWN);
                            }
                            else 
                                if (instance && !instance->GetData(DATA_FLEX)) // If we are in instance and not flex raid, use normal script
                                {
                                    if (key->first != 4 && key->first != 9) // not announce dragons
                                        if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                                            zaela->AI()->Talk(urand(TALK_SPECIAL_4, TALK_SPECIAL_5));
                                    for (uint8 i = 0; i < 6; i++)
                                        if (key->second[i])
                                            me->SummonCreature(key->second[i], galakrasWavePoint[i], TEMPSUMMON_MANUAL_DESPAWN);
                                }
                            else 
                                    if (instance && instance->GetData(DATA_FLEX) && instance->instance->GetPlayersCountExceptGMs() > 15) // If We are in Flex and Have more than 15 players we use normal spawns
                                    {
                                        if (key->first != 4 && key->first != 9) // not announce dragons
                                            if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                                                zaela->AI()->Talk(urand(TALK_SPECIAL_4, TALK_SPECIAL_5));
                                        for (uint8 i = 0; i < 6; i++)
                                            if (key->second[i])
                                                me->SummonCreature(key->second[i], galakrasWavePoint[i], TEMPSUMMON_MANUAL_DESPAWN);
                                    }
                        if (instance && !instance->GetData(DATA_FLEX))
                            berserkerEvents.ScheduleEvent(EVENT_NEXT_WAVE, 60 * IN_MILLISECONDS); 
                            else
                                if (instance && instance->GetData(DATA_FLEX) && instance->instance->GetPlayersCountExceptGMs() < 26)
                                    berserkerEvents.ScheduleEvent(EVENT_NEXT_WAVE, 50 * IN_MILLISECONDS); // Wave in Flex Have -10 Seconds cause we have less mobs

                            break;
                        }
                        case EVENT_SECOND_PHASE:
                            me->StopMoving();
                            me->GetMotionMaster()->Clear();
                            me->SetAnimTier(AnimTier::Ground);
                            me->OverrideInhabitType(INHABIT_GROUND);
                            me->UpdateMovementFlags();

                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            DoZoneInCombat(me, 150.0f);

                            if (Unit* target = me->GetVictim())
                                me->RemoveChanneledCast(target->GetGUID());

                            if (instance)
                                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                            events.ScheduleEvent(EVENT_FLAMES_OF_GALAKROND, 5.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_PULSING_FLAMES, 12.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_CALL_DEMOLISHER:
                            if (Creature* zaela = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                            {
                                zaela->AI()->Talk(TALK_SPECIAL_7);
                                zaela->AI()->Talk(TALK_SPECIAL_8); // Announce
                            }

                            me->SummonCreature(NPC_GALAKRAS_KORKRON_DEMOLISHER, demolisherSpawnPos, TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                        case EVENT_SUMMON_TOWER_GRUNT:
                            if (GetData(TYPE_SOUTH_TOWER) && GetData(TYPE_NORTH_TOWER))
                                break;

                            if (GetData(TYPE_SOUTH_TOWER))
                                me->SummonCreature(NPC_GALAKRAS_DRAGONMAW_GRUNT_2, towerGruntsPosition[2], TEMPSUMMON_MANUAL_DESPAWN);
                            else
                                me->SummonCreature(NPC_GALAKRAS_DRAGONMAW_GRUNT_2, towerGruntsPosition[0], TEMPSUMMON_MANUAL_DESPAWN);

                            berserkerEvents.ScheduleEvent(EVENT_SUMMON_TOWER_GRUNT, 1 * MINUTE * IN_MILLISECONDS);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PULSING_FLAMES:
                            DoCast(me, SPELL_PULSING_FLAMES);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_PULSING_FLAMES, 14 * IN_MILLISECONDS);
                            break;
                        case EVENT_FLAMES_OF_GALAKROND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                            {
                                flamesTargetGUID = target->GetGUID();
                                DoCast(target, SPELL_FLAMES_OF_GALAKROND_ANN, true);
                                DoCast(target, SPELL_FLAMES_OF_GALAKROND_MARK);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                flamesTargetGUID = target->GetGUID();
                                DoCast(target, SPELL_FLAMES_OF_GALAKROND_ANN, true);
                                DoCast(target, SPELL_FLAMES_OF_GALAKROND_MARK);
                            }
                            events.ScheduleEvent(EVENT_FLAMES_OF_GALAKROND, 9.5 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (hasHitByCannon)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_galakrasAI(creature);
        }
};

// Master Cannoneer Dagryn 72356
struct npc_master_cannoneer_dagryn : public ScriptedAI
{
    npc_master_cannoneer_dagryn(Creature* creature) : ScriptedAI(creature)  { }

    TaskScheduler scheduler;
    EventMap events;
    Position pos;
    ObjectGuid targetGUID;

    // proto drakon doesn`t exists at second tower
    void IsSummonedBy(Unit* summoner) override { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        scheduler.CancelAll();
        targetGUID = ObjectGuid::Empty;
    }

    void AttackStart(Unit* target) override
    {
        if (!target || target && target->GetPositionZ() < 57.0f)
            return;

        if (me->GetVictim())
            return;

        me->Attack(target, true);
        me->GetMotionMaster()->MoveChase(target, 0.0f, me->GetAngle(target));
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
        events.ScheduleEvent(EVENT_SHOOT, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MUZZLE_SPRAY, 10 * IN_MILLISECONDS);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            if (me->GetExactDist2d(&dagrynCenterPos) > 25.0f)
            {
                EnterEvadeMode();
                return;
            }
            context.Repeat(Milliseconds(1000));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        scheduler.CancelAll();

        Talk(TALK_SPECIAL_1);

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            galakras->AI()->SetData(TYPE_NORTH_TOWER_INFLUENCE_COUNT, 40);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
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
                case EVENT_SHOOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHOOT);

                    events.ScheduleEvent(EVENT_SHOOT, urand(2 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS));
                    break;
                case EVENT_MUZZLE_SPRAY:
                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation());
                    me->GetMotionMaster()->MovePoint(0, dagrynCenterPos);

                    uint32 delay = me->GetSplineDuration();
                    scheduler
                        .Schedule(Milliseconds(delay), [this](TaskContext context)
                    {
                        me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_MUZZLE_SPRAY);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
                    {
                        me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_MUZZLE_SPRAY);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
                    {
                        me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_MUZZLE_SPRAY);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 4500), [this](TaskContext context)
                    {
                        me->RemoveChanneledCast(targetGUID);
                    });

                    events.ScheduleEvent(EVENT_MUZZLE_SPRAY, 22 * IN_MILLISECONDS);
                    break;
            }
        }
    }
};

// High Enforcer Thranok 72355
struct npc_high_enforcer_thranok : public ScriptedAI
{
    npc_high_enforcer_thranok(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHATTERED_CLEAVE, 6.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SKULLCRACKER, 17.5 * IN_MILLISECONDS);
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
                case EVENT_SHATTERED_CLEAVE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHATTERING_CLEAVE);

                    events.ScheduleEvent(EVENT_SHATTERED_CLEAVE, 10.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SKULLCRACKER:
                    DoCast(me, SPELL_CRUSHERS_CALL_SELECTOR, true);
                    DoCast(me, SPELL_SKULL_CRACKER);
                    events.ScheduleEvent(EVENT_SKULLCRACKER, 19.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Lieutenant Krugruk 72357
struct npc_lieutenant_krugruk : public ScriptedAI
{
    npc_lieutenant_krugruk(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    Position pos;
    ObjectGuid targetGUID;
    uint32 dragonsCount; // 10 is limit

    void IsSummonedBy(Unit* summoner) override
    {
        dragonsCount = 0;

        // Summon Proto Drakes Each 15s
        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            if (++dragonsCount > 10)
                return;

            pos = me->GetRandomPoint({ galakrasFlyPoint.GetPositionX(), galakrasFlyPoint.GetPositionY(), galakrasFlyPoint.GetPositionZ(), galakrasFlyPoint.GetOrientation() }, 30.0f);

            me->SummonCreature(NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE, pos.GetPositionX(), pos.GetPositionY(), 81.55f, TEMPSUMMON_MANUAL_DESPAWN);
            context.Repeat(Milliseconds(15000));
        });
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
        events.ScheduleEvent(EVENT_THUNDER_CLAP, 4.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_ARCING_SMASH, 12.5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        scheduler.CancelAll();

        Talk(TALK_SPECIAL_1);

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            galakras->AI()->SetData(TYPE_SOUTH_TOWER_INFLUENCE_COUNT, 40);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
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
                case EVENT_THUNDER_CLAP:
                    DoCast(me, SPELL_THUNDER_CLAP);
                    events.ScheduleEvent(EVENT_THUNDER_CLAP, 14.5 * IN_MILLISECONDS);
                    break;
                case EVENT_ARCING_SMASH:
                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation());
                    me->GetMotionMaster()->MoveTargetedHome();

                    uint32 delay = me->GetSplineDuration();
                    scheduler
                        .Schedule(Milliseconds(delay), [this](TaskContext context)
                    {
                        me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_ARCING_SMASH);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
                    {
                        me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_ARCING_SMASH);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
                    {
                        me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_ARCING_SMASH);
                    });

                    scheduler
                        .Schedule(Milliseconds(delay += 2500), [this](TaskContext context)
                    {
                        me->RemoveChanneledCast(targetGUID);
                    });

                    events.ScheduleEvent(EVENT_ARCING_SMASH, 22 * IN_MILLISECONDS);
                    break;
            }
        }

        EnterEvadeIfOutOfCombatArea(diff, 25.0f);
        DoMeleeAttackIfReady();
    }
};

// Korgra the Snake 72456
struct npc_korgra_the_snake : public ScriptedAI
{
    npc_korgra_the_snake(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool hasTriggered;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        hasTriggered = false;
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_POISON_CLOUD, 5.5 * IN_MILLISECONDS); // In blizzard this spell is only casted Once
        events.ScheduleEvent(EVENT_TIPPED_BLADES, 4.5 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (HealthBelowPct(50) && !hasTriggered)
        {
            hasTriggered = true;
            events.ScheduleEvent(EVENT_SNAKE_TRANSFORMATION, 0.5 * IN_MILLISECONDS);
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
            switch (eventId)
            {
                case EVENT_POISON_CLOUD:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector(SPELL_IN_A_TOWER)))
                        DoCast(target, SPELL_POISON_CLOUD_AT);
                    break;
                case EVENT_TIPPED_BLADES:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_POISON_TIPPED_BLADES_EFF);

                    events.ScheduleEvent(EVENT_TIPPED_BLADES, 7.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SNAKE_TRANSFORMATION:
                    events.Reset();
                    DoCast(me, SPELL_CURSE_OF_VENOM);
                    events.ScheduleEvent(EVENT_POISON_BLAST, 3 * IN_MILLISECONDS);
                    break;
                case EVENT_POISON_BLAST:
                    DoCast(me, SPELL_POISON_BLAST, true);
                    events.ScheduleEvent(EVENT_POISON_BLAST, urand(3.5 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Bonecrusher 72945
struct npc_galakras_dragonmaw_bonecrusher : public ScriptedAI
{
    npc_galakras_dragonmaw_bonecrusher(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    ObjectGuid targetGUID;
    bool hasControll;

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
        hasControll = false;
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHATTERING_ROAR, urand(8 * IN_MILLISECONDS, 14.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_FRACTURE, urand(17 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (!hasControll && spell->GetEffectMechanic(MECHANIC_STUN))
            hasControll = true;
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        if (pointId == EVENT_CHARGE && !hasControll)
        {
            me->StopMoving();
            me->GetMotionMaster()->Clear();

            if (Unit* target = me->GetVictim())
                targetGUID = target->GetGUID();

            me->PrepareChanneledCast(me->GetOrientation());

            scheduler
                .Schedule(Milliseconds(500), [this](TaskContext context)
            {
                DoCast(me, SPELL_FRACTURE);
            });

            scheduler
                .Schedule(Milliseconds(8500), [this](TaskContext context)
            {
                me->RemoveChanneledCast(targetGUID);
            });
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
            me->RemoveChanneledCast(targetGUID);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHATTERING_ROAR:
                    DoCast(me, SPELL_SHATTERING_ROAR);
                    events.ScheduleEvent(SPELL_SHATTERING_ROAR, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_FRACTURE:
                    hasControll = false;

                    if (Creature* factionLeader = ObjectAccessor::GetCreature(*me, GetLeaderGUID()))
                        me->GetMotionMaster()->MoveCharge(factionLeader->GetPositionX(), factionLeader->GetPositionY(), factionLeader->GetPositionZ(), 25.0f, EVENT_CHARGE);

                    events.ScheduleEvent(EVENT_FRACTURE, urand(31 * IN_MILLISECONDS, 35 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    ObjectGuid GetLeaderGUID()
    {
        std::list<Creature*> leadersList;
        GetCreatureListWithEntryInGrid(leadersList, me, NPC_LORTHEMAR_THERON, 200.0f);
        GetCreatureListWithEntryInGrid(leadersList, me, NPC_LADY_SYLVANAS_WINDRUNNER, 200.0f);
        GetCreatureListWithEntryInGrid(leadersList, me, NPC_ARCHMAGE_AETHAS_SUNREAVER, 200.0f);
        GetCreatureListWithEntryInGrid(leadersList, me, NPC_KING_VARIAN_WRYNN, 200.0f);
        GetCreatureListWithEntryInGrid(leadersList, me, NPC_LADY_JAINA_PROUDMOORE, 200.0f);
        GetCreatureListWithEntryInGrid(leadersList, me, NPC_VEREESA_WINDRUNNER, 200.0f);

        if (leadersList.empty())
            return ObjectGuid::Empty;

        return Trinity::Containers::SelectRandomContainerElement(leadersList)->GetGUID();
    }
};

// Flagbearer 72942
struct npc_galakras_dragonmaw_flagbearer : public ScriptedAI
{
    npc_galakras_dragonmaw_flagbearer(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_WAR_BANNER, 8.5 * IN_MILLISECONDS);
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
            if (eventId == EVENT_WAR_BANNER)
            {
                DoCast(me, SPELL_WAR_BANNER);
                events.ScheduleEvent(EVENT_WAR_BANNER, 15 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dragonmaw Tidal Shaman 72958
struct npc_galakras_dragonmaw_tidal_shaman : public ScriptedAI
{
    npc_galakras_dragonmaw_tidal_shaman(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_TIDAL_WAVE, 12 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CHAIN_HEAL, 5.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_HEALING_TIDE_TOTEM, 9 * IN_MILLISECONDS);
    }

    ObjectGuid GetLowestFriendGUID()
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_TIDAL_SHAMAN, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_CANNONEER, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_BONECRUSHER, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_ELITE_GRUNT, 40.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_DRAGONMAW_FLAGBEATER, 40.0f);
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
            switch (eventId)
            {
                case EVENT_TIDAL_WAVE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankSpecTargetSelector()))
                        DoCast(target, SPELL_TIDAL_WAVE);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_TIDAL_WAVE);

                    events.ScheduleEvent(EVENT_TIDAL_WAVE, 10.5 * IN_MILLISECONDS);
                    break;
                case EVENT_CHAIN_HEAL:
                    if (Unit* targetAlly = ObjectAccessor::GetUnit(*me, GetLowestFriendGUID()))
                        DoCast(targetAlly, SPELL_CHAIN_HEAL);

                    events.ScheduleEvent(EVENT_CHAIN_HEAL, urand(10.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    break;
                case EVENT_HEALING_TIDE_TOTEM:
                    DoCast(me, SPELL_HEALING_TIDE_TOTEM);
                    events.ScheduleEvent(EVENT_HEALING_TIDE_TOTEM, 15 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dragonmaw Flameslinger 72353
struct npc_galakras_dragonmaw_flameslinger : public ScriptedAI
{
    npc_galakras_dragonmaw_flameslinger(Creature* creature) : ScriptedAI(creature)
    {
        innerTower = me->GetPositionZ() > 32.0f && me->GetPositionZ() < 44.0f;
        inTower = 0;

        if (!innerTower)
            SetCombatMovement(false);
    }

    EventMap events;
    ObjectGuid arrowTargetGUID;
    uint8 inTower;
    bool innerTower;

    void Reset() override
    {
        events.Reset();
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return arrowTargetGUID;
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        arrowTargetGUID = guid;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FLAME_ARROWS, 2.5 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        inTower = actionId == ACTION_IN_TOWER_S ? 1 : 2;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_SLINGER_IN_TOWER)
            return innerTower ? 1 : 0;

        return 0;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (inTower && me->GetInstanceScript())
            if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS)))
                galakras->AI()->SetData(inTower > 1 ? TYPE_NORTH_TOWER_INFLUENCE_COUNT : TYPE_SOUTH_TOWER_INFLUENCE_COUNT, 10);
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
            if (eventId == EVENT_FLAME_ARROWS)
            {
                DoCast(me, SPELL_FLAME_ARROWS_SELECTOR);
                events.ScheduleEvent(EVENT_FLAME_ARROWS, 3.5 * IN_MILLISECONDS);
            }
            break;
        }

        if (innerTower)
            EnterEvadeIfOutOfCombatArea(diff, 35.0f);

        DoMeleeAttackIfReady();
    }
};

// Healing Tide Totem 73094
struct npc_galakras_healing_tide_totem : public ScriptedAI
{
    npc_galakras_healing_tide_totem(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        DoCast(me, SPELL_HEALING_TIDE_PROC);
    }

    void EnterEvadeMode() override { }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }
};

// Dragonmaw War Banner 73088
struct npc_galakras_dragonmaw_war_banner : public ScriptedAI
{
    npc_galakras_dragonmaw_war_banner(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->SetDisplayFromModel(0);
        DoCast(me, SPELL_WAR_BANNER_AT);
    }
    void EnterEvadeMode() override { }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }
};

// Dragonmaw Grunt 73530, 72941
struct npc_galakras_dragonmaw_grunt : public ScriptedAI
{
    npc_galakras_dragonmaw_grunt(Creature* creature) : ScriptedAI(creature) 
    {
        inTower = 0;
    }

    EventMap events;
    uint8 inTower;

    void Reset() override
    {
        events.Reset();
    }

    void DoAction(int32 actionId) override
    {
        inTower = actionId == ACTION_IN_TOWER_S ? 1 : 2;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(me->GetEntry() == NPC_DRAGONMAW_GRUNT ? EVENT_THROW_AXE : EVENT_DRAGONS_CLEAVE, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER) && !inTower)
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (inTower && me->GetInstanceScript())
            if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS)))
                galakras->AI()->SetData(inTower > 1 ? TYPE_NORTH_TOWER_INFLUENCE_COUNT : TYPE_SOUTH_TOWER_INFLUENCE_COUNT, 10);
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
            switch (eventId)
            {
                case EVENT_THROW_AXE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector(SPELL_IN_A_TOWER)))
                        DoCast(target, SPELL_THROW_AXE);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        DoCast(target, SPELL_THROW_AXE);

                    events.ScheduleEvent(EVENT_THROW_AXE, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
                case EVENT_DRAGONS_CLEAVE:
                    DoCast(me, SPELL_DRAGONS_CLEAVE);
                    events.ScheduleEvent(EVENT_DRAGONS_CLEAVE, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dragonmaw Grunt 73530
struct npc_galakras_dragonmaw_grunt_tower : public ScriptedAI
{
    npc_galakras_dragonmaw_grunt_tower(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
        {
            if (galakras->AI()->GetData(TYPE_SOUTH_TOWER) && galakras->AI()->GetData(TYPE_NORTH_TOWER))
            {
                me->DespawnOrUnsummon();
                return;
            }

            if (galakras->AI()->GetData(TYPE_SOUTH_TOWER))
                me->GetMotionMaster()->MoveJump(towerGruntsPosition[3], 20.0f, 20.0f, EVENT_JUMP);
            else
                me->GetMotionMaster()->MoveJump(towerGruntsPosition[1], 20.0f, 20.0f, EVENT_JUMP);
        }
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
            me->SetInCombatWithZone();
            DoCast(me, SPELL_FIXATE);
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DRAGONS_CLEAVE, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void AttackStart(Unit* victim) override
    {
        if (Creature* expert = me->FindNearestCreature(NPC_DEMOLITIONS_EXPERT, 500.0f, true))
        {
            if (me->Attack(expert, true))
            {
                DoCast(me, SPELL_FIXATE);
                me->GetMotionMaster()->MoveChase(expert);
            }
        }
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim && victim->GetEntry() == NPC_DEMOLITIONS_EXPERT)
            me->DespawnOrUnsummon();
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
            if (eventId == EVENT_DRAGONS_CLEAVE)
            {
                DoCast(me, SPELL_DRAGONS_CLEAVE);
                events.ScheduleEvent(EVENT_DRAGONS_CLEAVE, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dragonmaw Proto Drake 72943
struct npc_galakras_dragonmaw_proto_drake : public ScriptedAI
{
    npc_galakras_dragonmaw_proto_drake(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    Position pos;
    bool InAir;

    void IsSummonedBy(Unit* summoner) override
    {
        InAir = summoner->GetEntry() != NPC_GALAKRAS;

        if (InAir)
        {
            if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                galakras->AI()->JustSummoned(me);

            scheduler
                .Schedule(Milliseconds(1000), [this](TaskContext context)
            {
                pos = me->GetRandomPoint({ galakrasFlyPoint.GetPositionX(), galakrasFlyPoint.GetPositionY(), galakrasFlyPoint.GetPositionZ(), galakrasFlyPoint.GetOrientation() }, 30.0f);
                me->GetMotionMaster()->MovePoint(0, pos.GetPositionX(), pos.GetPositionY(), 81.55f, 0.0f);
                context.Repeat(Milliseconds(me->GetSplineDuration()));
            });

            me->PrepareChanneledCast(me->GetOrientation());

            me->OverrideInhabitType(INHABIT_AIR);
            me->UpdateMovementFlags();

            events.ScheduleEvent(EVENT_DRAKE_FIRE, 2.5 * IN_MILLISECONDS);
        }
        else
        {
            events.ScheduleEvent(EVENT_FLAME_BREATH, urand(6.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
        }
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER) && !InAir)
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon(8 * IN_MILLISECONDS);
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
                case EVENT_DRAKE_FIRE:
                    DoCast(me, SPELL_DRAKE_FIRE_SELECTOR);
                    events.ScheduleEvent(EVENT_DRAKE_FIRE, urand(4.5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
                    break;
                case EVENT_FLAME_BREATH:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_FLAME_BREATH);

                    events.ScheduleEvent(EVENT_FLAME_BREATH, 20 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Korkron Demolisher 72947
struct npc_galakras_korkron_demolisher : public ScriptedAI
{
    npc_galakras_korkron_demolisher(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    uint32 towerIndex;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        towerIndex = 0;

        // Set facing to Assaulted Tower
        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            towerIndex = galakras->AI()->GetData(TYPE_NORTH_TOWER) ? 1 : 0;

        me->SetInCombatWithZone();

        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext m_context)
        {
            me->GetMotionMaster()->MovePoint(0, demolisherMovePos);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->SetFacingTo(assaultTowers[towerIndex].GetOrientation());
                me->CastSpell(assaultTowers[towerIndex].GetPositionX(), assaultTowers[towerIndex].GetPositionY(), assaultTowers[towerIndex].GetPositionZ(), SPELL_BOMBARD, false);

                context.Repeat(Milliseconds(6500));
            });
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon(8 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Varian Wrynn 72311
struct npc_galakras_varian_wrynn : public ScriptedAI
{
    npc_galakras_varian_wrynn(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool hasEvade;

    void Reset() override
    {
        events.Reset();
        hasEvade = false;
        me->setRegeneratingHealth(false);

        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_GROUP_FACTION))
            me->DespawnOrUnsummon();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_VARIAN_CLEAVE, 4 * IN_MILLISECONDS);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_FACTIONS_EVADE && !hasEvade)
        {
            hasEvade = true;

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            uint32 corpseDelay = me->GetCorpseDelay();
            uint32 respawnDelay = me->GetRespawnDelay();

            me->SetCorpseDelay(1);
            me->SetRespawnDelay(29);

            me->DespawnOrUnsummon();

            me->SetCorpseDelay(corpseDelay);
            me->SetRespawnDelay(respawnDelay);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->Respawn();

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            galakras->AI()->EnterEvadeMode();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_VARIAN_CLEAVE)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_VARIAN_CLEAVE);

                events.ScheduleEvent(EVENT_VARIAN_CLEAVE, 5.5 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sylvanas Windrunner 72561
struct npc_galakras_sylvanas_windrunner : public ScriptedAI
{
    npc_galakras_sylvanas_windrunner(Creature* creature) : ScriptedAI(creature) 
    {
        SetCombatMovement(false);
    }

    EventMap events;
    bool hasEvade;

    void Reset() override
    {
        events.Reset();
        hasEvade = false;
        me->setRegeneratingHealth(false);

        if (me->GetInstanceScript() && !me->GetInstanceScript()->GetData(DATA_GROUP_FACTION))
            me->DespawnOrUnsummon();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SYLVANAS_SHOT, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SYLVANAS_MULTI_SHOT, 9 * IN_MILLISECONDS);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_FACTIONS_EVADE && !hasEvade)
        {
            hasEvade = true;

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            uint32 corpseDelay = me->GetCorpseDelay();
            uint32 respawnDelay = me->GetRespawnDelay();

            me->SetCorpseDelay(1);
            me->SetRespawnDelay(29);

            me->DespawnOrUnsummon();

            me->SetCorpseDelay(corpseDelay);
            me->SetRespawnDelay(respawnDelay);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->Respawn();

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            galakras->AI()->EnterEvadeMode();
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
            switch (eventId)
            {
                case EVENT_SYLVANAS_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SYLVANAS_SHOT);

                    events.ScheduleEvent(EVENT_SYLVANAS_SHOT, 2.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SYLVANAS_MULTI_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SYLVANAS_MULTI_SHOT);

                    events.ScheduleEvent(EVENT_SYLVANAS_MULTI_SHOT, 10 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Aethas Sunreaver 73909
struct npc_galakras_aethas_sunreaver : public ScriptedAI
{
    npc_galakras_aethas_sunreaver(Creature* creature) : ScriptedAI(creature) 
    {
        SetCombatMovement(false);
    }

    EventMap events;
    bool hasEvade;

    void Reset() override
    {
        events.Reset();
        hasEvade = false;
        me->setRegeneratingHealth(false);

        if (me->GetInstanceScript() && !me->GetInstanceScript()->GetData(DATA_GROUP_FACTION))
            me->DespawnOrUnsummon();
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_FLAMESTRIKE, 24 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_AETHAS_FIREBALL, 1 * IN_MILLISECONDS);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_FACTIONS_EVADE && !hasEvade)
        {
            hasEvade = true;

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            uint32 corpseDelay = me->GetCorpseDelay();
            uint32 respawnDelay = me->GetRespawnDelay();

            me->SetCorpseDelay(1);
            me->SetRespawnDelay(29);

            me->DespawnOrUnsummon();

            me->SetCorpseDelay(corpseDelay);
            me->SetRespawnDelay(respawnDelay);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->Respawn();

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            galakras->AI()->EnterEvadeMode();
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
            switch (eventId)
            {
                case EVENT_FLAMESTRIKE:
                    Talk(TALK_INTRO);
                    me->CastSpell(flamestrikePos.GetPositionX(), flamestrikePos.GetPositionY(), flamestrikePos.GetPositionZ(), SPELL_FLAMESTRIKE, false);

                    events.ScheduleEvent(EVENT_FLAMESTRIKE, 35 * IN_MILLISECONDS);
                    break;
                case EVENT_AETHAS_FIREBALL:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_AETHAS_FIREBALL);

                    events.ScheduleEvent(EVENT_AETHAS_FIREBALL, 2 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Vereesa Windrunner 73910
struct npc_galakras_vereesa_windrunner : public ScriptedAI
{
    npc_galakras_vereesa_windrunner(Creature* creature) : ScriptedAI(creature) 
    {
        SetCombatMovement(false);
    }

    EventMap events;
    bool hasEvade;

    void Reset() override
    {
        events.Reset();
        hasEvade = false;
        me->setRegeneratingHealth(false);

        if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_GROUP_FACTION))
            me->DespawnOrUnsummon();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SYLVANAS_SHOT, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SYLVANAS_MULTI_SHOT, 9 * IN_MILLISECONDS);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_FACTIONS_EVADE && !hasEvade)
        {
            hasEvade = true;
            
            if (me->GetInstanceScript())
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            uint32 corpseDelay = me->GetCorpseDelay();
            uint32 respawnDelay = me->GetRespawnDelay();

            me->SetCorpseDelay(1);
            me->SetRespawnDelay(29);

            me->DespawnOrUnsummon();

            me->SetCorpseDelay(corpseDelay);
            me->SetRespawnDelay(respawnDelay);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->Respawn();

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            galakras->AI()->EnterEvadeMode();
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
            switch (eventId)
            {
                case EVENT_SYLVANAS_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SYLVANAS_SHOT);
                
                    events.ScheduleEvent(EVENT_SYLVANAS_SHOT, 2.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SYLVANAS_MULTI_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SYLVANAS_MULTI_SHOT);
                
                    events.ScheduleEvent(EVENT_SYLVANAS_MULTI_SHOT, 10 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Jaina Proudmoore 72302
class npc_galakras_jaina_proudmoore : public CreatureScript
{
    public:
        npc_galakras_jaina_proudmoore() : CreatureScript("npc_galakras_jaina_proudmoore") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_GALAKRAS) == DONE)
                return false;

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I`m ready!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(ACTION_START_INTRO);

            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        struct npc_galakras_jaina_proudmooreAI : public ScriptedAI
        {
            npc_galakras_jaina_proudmooreAI(Creature* creature) : ScriptedAI(creature) 
            {
                SetCombatMovement(false);
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            TaskScheduler scheduler;
            float x, y;
            uint32 delay;
            bool hasEvade;

            void Reset() override
            {
                events.Reset();
                hasEvade = false;
                me->setRegeneratingHealth(false);

                if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_GROUP_FACTION))
                    me->DespawnOrUnsummon();

                if (instance && instance->GetBossState(DATA_GALAKRAS) != IN_PROGRESS)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FROST_RING, 24 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_JAINA_FROST_BOLT, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_JAINA_WATER_ELEMENTAL, 16 * IN_MILLISECONDS);
            }

            void EnterEvadeMode() override
            {
                ScriptedAI::EnterEvadeMode();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                        if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                            varian->AI()->Talk(TALK_INTRO);

                        delay = 6000;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3600), [this](TaskContext context)
                        {
                            if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                                varian->AI()->Talk(TALK_SPECIAL_1);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 11100), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_1);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 5100), [this](TaskContext context)
                        {
                            if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                                varian->AI()->Talk(TALK_SPECIAL_2);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                        {
                            if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                                varian->AI()->Talk(TALK_SPECIAL_3);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3000), [this](TaskContext context)
                        {
                            if (Creature* galakras = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                                galakras->AI()->DoAction(ACTION_START_INTRO);
                        });
                        break;
                    case ACTION_GALAKRAS_DEFEAT:
                        delay = 1000;

                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_6);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 1800), [this](TaskContext context)
                        {
                            if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                                varian->AI()->Talk(TALK_SPECIAL_5);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_7);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 9700), [this](TaskContext context)
                        {
                            if (Creature* varian = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KING_VARIAN_WRYNN) : ObjectGuid::Empty))
                                varian->AI()->Talk(TALK_SPECIAL_6);
                        });
                        break;
                    case ACTION_FACTIONS_EVADE:
                        if (hasEvade)
                            break;

                        hasEvade = true;

                        if (me->GetInstanceScript())
                            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                        uint32 corpseDelay = me->GetCorpseDelay();
                        uint32 respawnDelay = me->GetRespawnDelay();

                        me->SetCorpseDelay(1);
                        me->SetRespawnDelay(29);

                        me->DespawnOrUnsummon();

                        me->SetCorpseDelay(corpseDelay);
                        me->SetRespawnDelay(respawnDelay);
                        break;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->Respawn();

                if (Creature* galakras = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                    galakras->AI()->EnterEvadeMode();
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
                        case EVENT_FROST_RING:
                            Talk(TALK_SPECIAL_3);
                            me->CastSpell(flamestrikePos.GetPositionX(), flamestrikePos.GetPositionY(), flamestrikePos.GetPositionZ(), SPELL_BLIZZARD_AT, false);

                            events.ScheduleEvent(EVENT_FROST_RING, 35 * IN_MILLISECONDS);
                            break;
                        case EVENT_JAINA_WATER_ELEMENTAL:
                            DoCast(me, SPELL_JAINA_SUMMON_WATER_ELEMENTAL);
                            break;
                        case EVENT_JAINA_FROST_BOLT:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_JAINA_FROST_BOLT);

                            events.ScheduleEvent(EVENT_JAINA_FROST_BOLT, 2.5 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_galakras_jaina_proudmooreAI(creature);
        }
};

// Lorthemar Therone 72560
class npc_galakras_lorthemar_theron : public CreatureScript
{
    public:
        npc_galakras_lorthemar_theron() : CreatureScript("npc_galakras_lorthemar_theron") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_GALAKRAS) == DONE)
                return false;

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I`m ready!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
    
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(ACTION_START_INTRO);
    
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        struct npc_galakras_lorthemar_theronAI : public ScriptedAI
        {
            npc_galakras_lorthemar_theronAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            InstanceScript* instance;
            EventMap events;
            TaskScheduler scheduler;
            float x, y;
            uint32 delay;
            bool hasEvade;

            void Reset() override
            {
                events.Reset();
                hasEvade = false;
                me->setRegeneratingHealth(false);

                if (me->GetInstanceScript() && !me->GetInstanceScript()->GetData(DATA_GROUP_FACTION))
                    me->DespawnOrUnsummon();

                if (instance && instance->GetBossState(DATA_GALAKRAS) != IN_PROGRESS)
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void EnterEvadeMode() override
            {
                ScriptedAI::EnterEvadeMode();
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Talk(TALK_INTRO);

                        delay = 4700;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                                sylvanas->AI()->Talk(TALK_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 5000), [this](TaskContext context)
                        {
                            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                                sylvanas->AI()->Talk(TALK_SPECIAL_1);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_1);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6100), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_2);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6200), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_3);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                        {
                            if (Creature* galakras = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                                galakras->AI()->DoAction(ACTION_START_INTRO);
                        });
                        break;
                    case ACTION_GALAKRAS_DEFEAT:
                        delay = 1000;

                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_7);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 1800), [this](TaskContext context)
                        {
                            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                                sylvanas->AI()->Talk(TALK_SPECIAL_5);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_8);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 9700), [this](TaskContext context)
                        {
                            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LADY_SYLVANAS_WINDRUNNER) : ObjectGuid::Empty))
                                sylvanas->AI()->Talk(TALK_SPECIAL_6);
                        });
                        break;
                    case ACTION_FACTIONS_EVADE:
                        if (hasEvade)
                            break;

                        hasEvade = true;

                        if (me->GetInstanceScript())
                            me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        uint32 corpseDelay = me->GetCorpseDelay();
                        uint32 respawnDelay = me->GetRespawnDelay();

                        me->SetCorpseDelay(1);
                        me->SetRespawnDelay(29);

                        me->DespawnOrUnsummon();

                        me->SetCorpseDelay(corpseDelay);
                        me->SetRespawnDelay(respawnDelay);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LORTHEMAR_ARCANE_SHOT, 8.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_LORTHEMAR_CLEAVE, 3.5 * IN_MILLISECONDS);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->Respawn();

                if (Creature* galakras = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                    galakras->AI()->EnterEvadeMode();
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
                        case EVENT_LORTHEMAR_CLEAVE:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_LORTHEMAR_CLEAVE);

                            events.ScheduleEvent(EVENT_LORTHEMAR_CLEAVE, 5 * IN_MILLISECONDS);
                            break;
                        case EVENT_LORTHEMAR_ARCANE_SHOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f))
                                DoCast(target, SPELL_LORTHEMAR_ARCANE_SHOT);

                            events.ScheduleEvent(EVENT_LORTHEMAR_ARCANE_SHOT, 11.5 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_galakras_lorthemar_theronAI(creature);
        }
};

// Demolitions Expert 73550
struct npc_galakras_demolitions_expert : public ScriptedAI
{
    npc_galakras_demolitions_expert(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasSouth;

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(me, SPELL_GOT_THE_BOMB);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (!IsHeroic())
            return;

        if (Creature* galakras = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
        {
            if (galakras->AI()->GetData(TYPE_SOUTH_TOWER) && galakras->AI()->GetData(TYPE_NORTH_TOWER))
                return;

            if (galakras->AI()->GetData(TYPE_SOUTH_TOWER))
                galakras->AI()->SetData(TYPE_NORTH_TOWER_COUNTER, 0);
            else
                galakras->AI()->SetData(TYPE_SOUTH_TOWER_COUNTER, 0);

            galakras->AI()->DoAction(ACTION_RECALL_EXPERT);
        }

    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_SIEGE_SOUTH_TOWER:
            {
                Movement::MoveSplineInit init(me);

                for (auto itr : demolotionsSiegeSouthPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 2.0f), itr.GetPositionY() + frand(-2.5f, 2.5f), itr.GetPositionZ()));

                init.SetUncompressed();
                init.Launch();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    DoCast(me, SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_S);
                });
                break;
            }
            case ACTION_SIEGE_NORTH_TOWER:
            {
                Movement::MoveSplineInit init(me);

                for (auto itr : demolotionsSiegeNorthPath)
                    init.Path().push_back(G3D::Vector3(itr.GetPositionX() + frand(-2.0f, 2.0f), itr.GetPositionY() + frand(-2.5f, 2.5f), itr.GetPositionZ()));

                init.SetUncompressed();
                init.Launch();

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    DoCast(me, SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_N);

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->DoUpdateWorldState(WORLDSTATE_NORTH_TOWER_PROGRESS, 1);
                });
                break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Sunreaver Magus 72581, 72534
struct npc_galakras_ally_range_assist : public ScriptedAI
{
    npc_galakras_ally_range_assist(Creature* creature) : ScriptedAI(creature) 
    {
        SetCombatMovement(false);
    }

    EventMap events;
    uint32 spellId;

    void Reset() override
    {
        events.Reset();
        spellId = me->GetEntry() == NPC_SUNREAVER_MAGUS ? SPELL_FIRE_BALL : SPELL_RIFFLE_SHOT;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RANGE_ASSIST, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->GetMap()->SetWorldState(WORLDSTATE_IMMORTAL_VANGUARD, 0);
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
            if (eventId == EVENT_RANGE_ASSIST)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, spellId);

                events.ScheduleEvent(EVENT_RANGE_ASSIST, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
            }
            break;
        }
    }
};

// Sunreaver Sentinel 72579, 72580, 72549, 72548
struct npc_galakras_ally_melee_assist : public ScriptedAI
{
    npc_galakras_ally_melee_assist(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* /*killer*/) override
    {
        me->GetMap()->SetWorldState(WORLDSTATE_IMMORTAL_VANGUARD, 0);
    }
};

// Anti-Air Cannon 72408
struct npc_galakras_anti_air_cannon : public ScriptedAI
{
    npc_galakras_anti_air_cannon(Creature* creature) : ScriptedAI(creature) { }

    uint32 allowUse;

    void Reset() override
    {
        allowUse = 0;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (!allowUse)
            return;

        if (me->GetVehicleKit() && me->GetVehicleKit()->GetPassenger(0))
            return;

        clicker->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
    }

    void EnterEvadeMode() override { }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_AIR_CANNON_ACTIVE)
            allowUse = data;
    }
};

// Dragonmaw Ebon Stalker 72352
struct npc_galakras_dragonmaw_ebon_stalker : public ScriptedAI
{
    npc_galakras_dragonmaw_ebon_stalker(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    EventMap events;
    ObjectGuid shadowStepTargetGUID;
    ObjectGuid targetGUID;

    void Reset() override
    {
        events.Reset();
        shadowStepTargetGUID = ObjectGuid::Empty;
        targetGUID = ObjectGuid::Empty;
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && victim->HasAura(SPELL_IN_A_TOWER))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // select faction leader
            if (Creature* target = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetData(DATA_GROUP_FACTION) ? NPC_LADY_SYLVANAS_WINDRUNNER : NPC_LADY_JAINA_PROUDMOORE) : ObjectGuid::Empty))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHADOW_STALK, urand(5.5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            if (Unit* target = me->GetVictim())
                targetGUID = target->GetGUID();

            me->PrepareChanneledCast(me->GetOrientation());

            scheduler
                .Schedule(Milliseconds(500), [this](TaskContext context)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, shadowStepTargetGUID))
                    me->PrepareChanneledCast(me->GetAngle(target), SPELL_SHADOW_ASSAULT);
            });

            scheduler
                .Schedule(Milliseconds(8000), [this](TaskContext context)
            {
                me->RemoveChanneledCast(targetGUID);
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
            if (eventId == EVENT_SHADOW_STALK)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector(SPELL_IN_A_TOWER)))
                {
                    shadowStepTargetGUID = target->GetGUID();
                    DoCast(target, SPELL_SHADOW_STALK);
                }
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f, true, -SPELL_IN_A_TOWER))
                {
                    shadowStepTargetGUID = target->GetGUID();
                    DoCast(target, SPELL_SHADOW_STALK);
                }

                events.ScheduleEvent(EVENT_SHADOW_STALK, urand(12.5 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Rope 73920
struct npc_galakras_rope : public ScriptedAI
{
    npc_galakras_rope(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetVisible(false);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        uint8 index = me->GetPositionX() > 1400.0f ? 1 : 0;
        clicker->GetMotionMaster()->MoveJump(ropeJumpPos[index], 20.0f, 15.0f);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Crushers Call Selector 146769
class spell_galakras_crushers_call_selector : public SpellScript
{
    PrepareSpellScript(spell_galakras_crushers_call_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Player* target = GetHitPlayer())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks and victim affected by titans gift
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_IN_A_TOWER); });

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 10 : 5;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 5; // min 5, max 10
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 18)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount) // doesn`t matter count - MaxAffect is 5(10)
            return;

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_IN_A_TOWER); });

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 5; // min 5, max 10
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                targetsCount++;

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 18)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_galakras_crushers_call_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_crushers_call_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Crusher Call Chain Eff 146775
class spell_galakras_crushers_call_chain_eff : public SpellScript
{
    PrepareSpellScript(spell_galakras_crushers_call_chain_eff);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Player* target = GetHitPlayer())
                target->CastSpell(caster, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_galakras_crushers_call_chain_eff::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Most Complicated Bomb in the World 147897, 147916
class spell_most_complicated_bomb_in_the_world : public AuraScript
{
    PrepareAuraScript(spell_most_complicated_bomb_in_the_world);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (Creature* galakras = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            {
                towerId = GetSpellInfo()->Id == SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_S ? TYPE_SOUTH_TOWER_COUNTER : TYPE_NORTH_TOWER_COUNTER;
                uint32 towerProgressValue = galakras->AI()->GetData(towerId);

                if (towerProgressValue > 99)
                {
                    galakras->AI()->SetData(GetSpellInfo()->Id == SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_S ? TYPE_SOUTH_TOWER : TYPE_NORTH_TOWER, 1);

                    // Announce
                    if (Creature* zaela = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_ZAELA) : ObjectGuid::Empty))
                        zaela->AI()->Talk(GetSpellInfo()->Id == SPELL_MOST_COMPLICATED_BOMB_IN_THE_WORLD_S ? TALK_SPECIAL_6 : TALK_SPECIAL_11);

                    if (Creature* galakras = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                        galakras->AI()->DoAction(ACTION_CALL_DEMOLISHER);

                    owner->DespawnOrUnsummon();
                    return;
                }

                galakras->AI()->SetData(towerId, ++towerProgressValue);
            }
        }
    }

    private:
        uint32 towerId;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_most_complicated_bomb_in_the_world::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Anti Air Cannon Eff 147514
class spell_galakras_anti_air_cannon_eff : public SpellScript
{
    PrepareSpellScript(spell_galakras_anti_air_cannon_eff);

    void HandleBeforeHit()
    {
        if (GetHitCreature() && GetHitCreature()->GetEntry() == NPC_GALAKRAS && GetHitCreature()->HasAura(GetSpellInfo()->Id))
            GetHitCreature()->AI()->DoAction(ACTION_BRING_DOWN);
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_GALAKRAS && target->GetEntry() != NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE; });
    }

    void Register() override
    {
        BeforeHit += SpellHitFn(spell_galakras_anti_air_cannon_eff::HandleBeforeHit);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_anti_air_cannon_eff::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// // Anti Air Cannon Eff Aura 147514
class spell_galakras_anti_air_cannon_eff_aura : public AuraScript
{
    PrepareAuraScript(spell_galakras_anti_air_cannon_eff_aura);

    void OnApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (Creature* target = GetOwner()->ToCreature())
        {
            if (target->GetEntry() == NPC_GALAKRAS) // require double hit
                target->AI()->Talk(TALK_INTRO);
            else
                target->Kill(target, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_galakras_anti_air_cannon_eff_aura::OnApply, EFFECT_0, SPELL_AURA_STRANGULATE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Anti-Air Cannon Missle 146488
class spell_galakras_anti_air_cannon_missle : public SpellScript
{
    PrepareSpellScript(spell_galakras_anti_air_cannon_missle);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            // Traj has an wrong traectory of hit...
            if (Creature* galakras = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            {
                float dist = caster->GetExactDist2d(galakras);
                GetPositionWithDistInOrientation(caster, dist, caster->GetAngle(galakras), x, y);
                caster->CastSpell(x, y, galakrasFlyPoint.GetPositionZ(), SPELL_ANTI_AIR_CANNON_EFF, true);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_galakras_anti_air_cannon_missle::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// In a Tower 147317
class spell_galakras_in_a_tower : public AuraScript
{
    PrepareAuraScript(spell_galakras_in_a_tower);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->RemoveAurasDueToSpell(SPELL_TOWER_STRENGTH_S);
            owner->RemoveAurasDueToSpell(SPELL_TOWER_STRENGTH_N);
        }
    }

    void OnUpdate(uint32 diff)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            if (!hasInTower(owner))
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    bool hasInTower(Unit* owner)
    {
        if (owner->GetPositionZ() < 28.5f)
            return false;

        for (auto&& itr : inTowerCenter)
        {
            float dist = owner->GetExactDist2d(itr.GetPositionX(), itr.GetPositionY());
            if (owner->GetExactDist2d(itr.GetPositionX(), itr.GetPositionY()) < 19.0f)
                return true;
        }

        return false;
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_galakras_in_a_tower::HandleOnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnAuraUpdate += AuraUpdateFn(spell_galakras_in_a_tower::OnUpdate);
    }
};

// Shattering Roar 147204
class spell_galakras_shattering_roar : public SpellScript
{
    PrepareSpellScript(spell_galakras_shattering_roar);

    void SelectMainTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_IN_A_TOWER); });
    }

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ARCHMAGE_AETHAS_SUNREAVER && target->GetEntry() != NPC_LADY_SYLVANAS_WINDRUNNER && target->GetEntry() != NPC_LORTHEMAR_THERON && target->GetEntry() != NPC_KING_VARIAN_WRYNN && target->GetEntry() != NPC_LADY_JAINA_PROUDMOORE && target->GetEntry() != NPC_VEREESA_WINDRUNNER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_shattering_roar::SelectMainTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_shattering_roar::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Healing Tide Eff 146726
class spell_galakras_healing_tide_eff : public SpellScript
{
    PrepareSpellScript(spell_galakras_healing_tide_eff);

    void SelectMainTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() == NPC_GALAKRAS_KORKRON_DEMOLISHER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_healing_tide_eff::SelectMainTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
    }
};

// Tidal Wave 148522
class spell_galakras_dragonmaw_tidal_wave : public SpellScript
{
    PrepareSpellScript(spell_galakras_dragonmaw_tidal_wave);

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
        OnEffectHitTarget += SpellEffectFn(spell_galakras_dragonmaw_tidal_wave::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_galakras_dragonmaw_tidal_wave::HandleWaveTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_dragonmaw_tidal_wave::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_dragonmaw_tidal_wave::FilterWaveTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Poison Blast 146606
class spell_galakras_poison_blast : public SpellScript
{
    PrepareSpellScript(spell_galakras_poison_blast);

    std::list<WorldObject*> m_targets, copyTargets;

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_IN_A_TOWER); });
        targets.remove_if(TankSpecTargetSelector());

        uint8 minCount = GetCaster()->GetMap()->Is25ManRaid() ? 7 : 3;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            minCount = 3; // min 3, max 7
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

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                minCount = 3; // min 3, max 7
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

            copyTargets = targets;
            return;
        }

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_poison_blast::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_poison_blast::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Flame Arrows 146763
class spell_galakras_flame_arrows : public SpellScript
{
    PrepareSpellScript(spell_galakras_flame_arrows);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                caster->CastSpell(target, SPELL_FLAME_ARROWS_EFF, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_galakras_flame_arrows::HandleAfterCast);
    }
};

// Tower Strength 148119
class spell_galakras_tower_strength : public AuraScript
{
    PrepareAuraScript(spell_galakras_tower_strength);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* galakras = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                owner->SetPower(POWER_ALTERNATE_POWER, galakras->AI()->GetData(TYPE_SOUTH_TOWER_INFLUENCE_COUNT));
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* galakras = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                owner->SetPower(POWER_ALTERNATE_POWER, galakras->AI()->GetData(TYPE_SOUTH_TOWER_INFLUENCE_COUNT));
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_galakras_tower_strength::HandleOnApply, EFFECT_1, SPELL_AURA_ENABLE_ALT_POWER, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_galakras_tower_strength::OnTrigger, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Tower Strength North 148120
class spell_galakras_tower_strength_north : public AuraScript
{
    PrepareAuraScript(spell_galakras_tower_strength_north);

    void HandleOnApply(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* galakras = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                owner->SetPower(POWER_ALTERNATE_POWER, galakras->AI()->GetData(TYPE_NORTH_TOWER_INFLUENCE_COUNT));
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Creature* galakras = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
                owner->SetPower(POWER_ALTERNATE_POWER, galakras->AI()->GetData(TYPE_NORTH_TOWER_INFLUENCE_COUNT));
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_galakras_tower_strength_north::HandleOnApply, EFFECT_0, SPELL_AURA_ENABLE_ALT_POWER, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_galakras_tower_strength_north::OnTrigger, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Flames of Galakrond 147068
class spell_galakras_flames_of_galakrond : public AuraScript
{
    PrepareAuraScript(spell_galakras_flames_of_galakrond);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_FLAMES_OF_GALAKROND_AT, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_galakras_flames_of_galakrond::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Healing Tide Proc 146725
class spell_galakras_healing_tide_proc : public AuraScript
{
    PrepareAuraScript(spell_galakras_healing_tide_proc);

    void HandleOnRemove(AuraEffect const* /*aureff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_galakras_healing_tide_proc::HandleOnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Drakefire Selector 148351
class spell_galakras_drakefire_selector : public SpellScript
{
    PrepareSpellScript(spell_galakras_drakefire_selector);

    std::list<WorldObject*> m_targets, copyTargets;

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_DRAKE_FIRE, false);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if([=](WorldObject* target) { return target && (target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_IN_A_TOWER) || !target->ToPlayer()); });
        targets.remove_if(TankSpecTargetSelector());

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
        OnEffectHitTarget += SpellEffectFn(spell_galakras_drakefire_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_drakefire_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_drakefire_selector::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Shadow Stalk 146864
class spell_galakras_shadow_stalk : public SpellScript
{
    PrepareSpellScript(spell_galakras_shadow_stalk);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_galakras_shadow_stalk::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_TELEPORT_UNITS);
    }
};

// Fracture 146899
class spell_galakras_fracture : public AuraScript
{
    PrepareAuraScript(spell_galakras_fracture);

    void HandleOnRemove(AuraEffect const* aureff, AuraEffectHandleModes /*mode*/)
    {
        if (aureff->GetBase()->GetDuration() > 0 && GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_galakras_fracture::HandleOnRemove, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Flames of Galakrond Periodic 147029
class spell_galakras_flames_of_galakrond_periodic : public AuraScript
{
    PrepareAuraScript(spell_galakras_flames_of_galakrond_periodic);

    int32 splitAmount;

    bool Load() override
    {
        splitAmount = 0;
        return true;
    }

    void SetGuid(uint32 type, ObjectGuid guid) override
    {
        splitAmount += int32(type);
    }

    void CalculateTick(AuraEffect const* eff, int32& damage)
    {
        damage = eff->GetBaseAmount() + splitAmount;
    }

    void HandleEffectCalcAmount(AuraEffect const* aurEff, float& amount, bool& canBeRecalculated)
    {
        amount = aurEff->GetBaseAmount() + splitAmount;
        canBeRecalculated = true;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_galakras_flames_of_galakrond_periodic::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_galakras_flames_of_galakrond_periodic::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// Fixate 148243
class spell_galakras_fixate : public AuraScript
{
    PrepareAuraScript(spell_galakras_fixate);

    void HandleOnApply(AuraEffect const* aureff, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature())
        {
            if (Creature* target = GetOwner()->ToCreature())
            {
                GetCaster()->ToCreature()->AI()->AttackStart(target);
                GetCaster()->ToCreature()->SetReactState(REACT_PASSIVE);
                GetCaster()->ToCreature()->ClearUnitState(UNIT_STATE_CASTING);
            }
        }
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_galakras_fixate::HandleOnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Flame Arrows Selector 147438
class spell_galakras_flame_arrows_selector : public SpellScript
{
    PrepareSpellScript(spell_galakras_flame_arrows_selector);

    std::list<WorldObject*> m_targets, copyTargets;

    void HandleEffectHitTarget(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
        {
            if (GetCaster()->ToCreature())
                GetCaster()->ToCreature()->AI()->SetGUID(target->GetGUID());

            GetCaster()->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, false);
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            bool inTower = caster->AI()->GetData(TYPE_SLINGER_IN_TOWER) ? true : false;

            m_targets = targets;

            if (inTower)
            {
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (!target->ToPlayer()->HasAura(SPELL_IN_A_TOWER) || !GetCaster()->IsWithinLOS(target->ToPlayer()->GetPositionX(), target->ToPlayer()->GetPositionY(), target->ToPlayer()->GetPositionZ())); });
            }
            else
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_IN_A_TOWER); });

            if (targets.size() < 1)
            {
                targets.clear();

                for (auto&& itr : m_targets)
                    targets.push_back(itr);

                if (inTower)
                    targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && !GetCaster()->IsWithinLOS(target->ToPlayer()->GetPositionX(), target->ToPlayer()->GetPositionY(), target->ToPlayer()->GetPositionZ()); });

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_galakras_flame_arrows_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_galakras_flame_arrows_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 1259. Summoned by 146764 - Flame Arrows
class sat_galakras_flame_arrows : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_FLAME_ARROWS_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_FLAME_ARROWS_AT_EFF);
    }
};

// 1301. Summoned by 147706 - Poison Cloud
class sat_galakras_poison_cloud : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_POISON_CLOUD_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_POISON_CLOUD_EFF);
    }
};

// 1287. Summoned by 146782 - Blizzard
class sat_galakras_blizzard : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_DRAGONMAW_FLAMESLINGER || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_BONECRUSHER || 
            triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_GRUNT || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_FLAGBEARER || 
            triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_EBON_STALKER || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_BLIZZARD_AT_EFF, true);

    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_BLIZZARD_AT_EFF);
    }
};

// 1340. Summoned by 148849 - Flamestrike
class sat_galakras_flamestrike : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_DRAGONMAW_FLAMESLINGER || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_BONECRUSHER ||
            triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_GRUNT || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_FLAGBEARER ||
            triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_EBON_STALKER || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_FLAMESTRIKE_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_FLAMESTRIKE_EFF);
    }
};

// 1283. Summoned by 147328 - War Banner
class sat_galakras_war_banner : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_DRAGONMAW_FLAMESLINGER || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_BONECRUSHER ||
            triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_TIDAL_SHAMAN || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_GRUNT || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_FLAGBEARER ||
            triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_EBON_STALKER || triggering->ToCreature()->GetEntry() == NPC_GALAKRAS_DRAGONMAW_PROTO_DRAKE);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->CastSpell(itr, SPELL_WAR_BANNER_EFF, true);

    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_WAR_BANNER_EFF);
    }
};

// 1265. Summoned by 146991 - Flames of Galakrond
class sat_galakras_flames_of_galakrond : public IAreaTriggerAura
{
    void OnInit() override
    {
        affectedCount = 0;

        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (AreaTrigger* at = GetAreaTrigger())
            {
                if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                {
                    float dist = at->GetExactDist2d(target);
                    float angle = at->GetAngle(target);

                    // Blizzlike only 2 points, but it's okay for now
                    std::vector<Position> path;

                    path.push_back(at->GetPosition());

                    int32 count = dist / 2;

                    for (int32 i = 1; i < count - 1; ++i)
                    {
                        Position newPos = path.back();
                        float x = at->GetPositionX() + (2.0f * i * std::cos(angle));
                        float y = at->GetPositionY() + (2.0f * i * std::sin(angle));

                        float z = at->GetPositionZ();
                        at->UpdateAllowedPositionZ(x, y, z);
                        path.push_back({ x, y, z });
                    }

                    path.push_back(target->GetPosition());

                    // Confirmed by sniffs
                    int32 time = dist * 0.1f * IN_MILLISECONDS;
                    at->InitSpline(path, time);
                }
            }
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                uint32 maxAffectedCount = caster->GetMap()->Is25ManRaid() ? 10 : 6;

                if (affectedCount < maxAffectedCount) // only 5 dots per split (9 in 25 ppl mode)
                {
                    float diffMod = caster->GetMap()->Is25ManRaid() ? 0.1f : 0.2f;
                    float affixMod = affectedCount > 3 ? 3.0f * diffMod : affectedCount * diffMod; // not lower than it
                    int32 amount = affectedCount > 0 ? (int32)(sSpellMgr->GetSpellInfo(SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints * (1 - affixMod)) : sSpellMgr->GetSpellInfo(SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints; // calculate amount of tick on this step
                    if (auto const aura = itr->GetAura(SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF))
                    {
                        aura->SetScriptGuid(amount, ObjectGuid::Empty);
                        aura->SetStackAmount(aura->GetStackAmount() + 1);
                        aura->RefreshDuration();
                    }
                    else
                        caster->CastCustomSpell(itr, SPELL_FLAMES_OF_GALAKROND_PERIODIC_EFF, &amount, NULL, NULL, true);
                }

                affectedCount++;
            }
        }
    }

    void OnDestinationReached() override
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (caster->GetMap()->Is25ManRaid())
                affectedCount = affectedCount / 2;

            float diffMod = caster->GetMap()->IsHeroic() ? 5.0f : 3.0f;
            float mod = caster->GetAura(SPELL_PULSING_FLAMES_EFF) ? (caster->GetAura(SPELL_PULSING_FLAMES_EFF)->GetStackAmount() * diffMod) / 100.0f : 0;

            int32 bp = affectedCount > 1 ? (int32)(sSpellMgr->GetSpellInfo(SPELL_FLAMES_OF_GALAKROND_EFF, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints / affectedCount + ((sSpellMgr->GetSpellInfo(SPELL_FLAMES_OF_GALAKROND_EFF, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints) / affectedCount) * mod) : (int32)(sSpellMgr->GetSpellInfo(SPELL_FLAMES_OF_GALAKROND_EFF, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints + sSpellMgr->GetSpellInfo(SPELL_FLAMES_OF_GALAKROND_EFF, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints * mod);
            caster->CastCustomSpell(GetAreaTrigger()->GetPositionX(), GetAreaTrigger()->GetPositionY(), GetAreaTrigger()->GetPositionZ(), SPELL_FLAMES_OF_GALAKROND_EFF, &bp, NULL, NULL, true);
        }
    }

    uint32 affectedCount;
};

// Areatrigger 9360
class AreaTrigger_at_soo_south_tower : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_south_tower() : AreaTriggerScript("AreaTrigger_at_soo_south_tower") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            // Prevent show tower state if not in combat yet
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_GALAKRAS) != IN_PROGRESS)
                return false;

            if (Creature* galakras = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            {
                if (galakras->AI()->GetData(TYPE_SOUTH_TOWER) && !player->HasAura(SPELL_IN_A_TOWER))
                {
                    player->CastSpell(player, SPELL_IN_A_TOWER, true);
                    player->CastSpell(player, SPELL_TOWER_STRENGTH_S, true);
                }
            }

            return true;
        }
};

// Areatrigger 9361
class AreaTrigger_at_soo_north_tower : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_north_tower() : AreaTriggerScript("AreaTrigger_at_soo_north_tower") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            // Prevent show tower state if not in combat yet
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_GALAKRAS) != IN_PROGRESS)
                return false;

            if (Creature* galakras = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(DATA_GALAKRAS) : ObjectGuid::Empty))
            {
                if (galakras->AI()->GetData(TYPE_NORTH_TOWER) && !player->HasAura(SPELL_IN_A_TOWER))
                {
                    player->CastSpell(player, SPELL_IN_A_TOWER, true);
                    player->CastSpell(player, SPELL_TOWER_STRENGTH_N, true);
                }
            }

            return true;
        }
};

void AddSC_galakras()
{
    new boss_galakras();

    new creature_script<npc_master_cannoneer_dagryn>("npc_master_cannoneer_dagryn");
    new creature_script<npc_high_enforcer_thranok>("npc_high_enforcer_thranok");
    new creature_script<npc_lieutenant_krugruk>("npc_lieutenant_krugruk");
    new creature_script<npc_korgra_the_snake>("npc_korgra_the_snake");
    new creature_script<npc_galakras_dragonmaw_bonecrusher>("npc_galakras_dragonmaw_bonecrusher");
    new creature_script<npc_galakras_dragonmaw_flagbearer>("npc_galakras_dragonmaw_flagbearer");
    new creature_script<npc_galakras_dragonmaw_tidal_shaman>("npc_galakras_dragonmaw_tidal_shaman");
    new creature_script<npc_galakras_dragonmaw_flameslinger>("npc_galakras_dragonmaw_flameslinger");
    new creature_script<npc_galakras_dragonmaw_war_banner>("npc_galakras_dragonmaw_war_banner");
    new creature_script<npc_galakras_healing_tide_totem>("npc_galakras_healing_tide_totem");
    new creature_script<npc_galakras_dragonmaw_grunt>("npc_galakras_dragonmaw_grunt");
    new creature_script<npc_galakras_dragonmaw_grunt_tower>("npc_galakras_dragonmaw_grunt_tower");
    new creature_script<npc_galakras_dragonmaw_proto_drake>("npc_galakras_dragonmaw_proto_drake");
    new creature_script<npc_galakras_korkron_demolisher>("npc_galakras_korkron_demolisher");
    new creature_script<npc_galakras_varian_wrynn>("npc_galakras_varian_wrynn");
    new creature_script<npc_galakras_sylvanas_windrunner>("npc_galakras_sylvanas_windrunner");
    new creature_script<npc_galakras_aethas_sunreaver>("npc_galakras_aethas_sunreaver");
    new creature_script<npc_galakras_vereesa_windrunner>("npc_galakras_vereesa_windrunner");
    new npc_galakras_jaina_proudmoore();
    new npc_galakras_lorthemar_theron();
    new creature_script<npc_galakras_demolitions_expert>("npc_galakras_demolitions_expert");
    new creature_script<npc_galakras_ally_range_assist>("npc_galakras_ally_range_assist");
    new creature_script<npc_galakras_ally_melee_assist>("npc_galakras_ally_melee_assist");
    new creature_script<npc_galakras_anti_air_cannon>("npc_galakras_anti_air_cannon");
    new creature_script<npc_galakras_dragonmaw_ebon_stalker>("npc_galakras_dragonmaw_ebon_stalker");
    new creature_script<npc_galakras_rope>("npc_galakras_rope");

    new spell_script<spell_galakras_crushers_call_selector>("spell_galakras_crushers_call_selector");
    new spell_script<spell_galakras_crushers_call_chain_eff>("spell_galakras_crushers_call_chain_eff");
    new aura_script<spell_most_complicated_bomb_in_the_world>("spell_most_complicated_bomb_in_the_world");
    new spell_script<spell_galakras_anti_air_cannon_eff>("spell_galakras_anti_air_cannon_eff");
    new aura_script<spell_galakras_anti_air_cannon_eff_aura>("spell_galakras_anti_air_cannon_eff_aura");
    new spell_script<spell_galakras_anti_air_cannon_missle>("spell_galakras_anti_air_cannon_missle");
    new aura_script<spell_galakras_in_a_tower>("spell_galakras_in_a_tower");
    new spell_script<spell_galakras_shattering_roar>("spell_galakras_shattering_roar");
    new spell_script<spell_galakras_healing_tide_eff>("spell_galakras_healing_tide_eff");
    new spell_script<spell_galakras_dragonmaw_tidal_wave>("spell_galakras_dragonmaw_tidal_wave");
    new spell_script<spell_galakras_poison_blast>("spell_galakras_poison_blast");
    new spell_script<spell_galakras_flame_arrows>("spell_galakras_flame_arrows");
    new aura_script<spell_galakras_tower_strength>("spell_galakras_tower_strength");
    new aura_script<spell_galakras_tower_strength_north>("spell_galakras_tower_strength_north");
    new aura_script<spell_galakras_flames_of_galakrond>("spell_galakras_flames_of_galakrond");
    new aura_script<spell_galakras_healing_tide_proc>("spell_galakras_healing_tide_proc");
    new spell_script<spell_galakras_drakefire_selector>("spell_galakras_drakefire_selector");
    new spell_script<spell_galakras_shadow_stalk>("spell_galakras_shadow_stalk");
    new aura_script<spell_galakras_fracture>("spell_galakras_fracture");
    new aura_script<spell_galakras_flames_of_galakrond_periodic>("spell_galakras_flames_of_galakrond_periodic");
    new aura_script<spell_galakras_fixate>("spell_galakras_fixate");
    new spell_script<spell_galakras_flame_arrows_selector>("spell_galakras_flame_arrows_selector");
    new atrigger_script<sat_galakras_flame_arrows>("sat_galakras_flame_arrows");
    new atrigger_script<sat_galakras_poison_cloud>("sat_galakras_poison_cloud");
    new atrigger_script<sat_galakras_blizzard>("sat_galakras_blizzard");
    new atrigger_script<sat_galakras_flamestrike>("sat_galakras_flamestrike");
    new atrigger_script<sat_galakras_war_banner>("sat_galakras_war_banner");
    new atrigger_script<sat_galakras_flames_of_galakrond>("sat_galakras_flames_of_galakrond");
    new AreaTrigger_at_soo_south_tower();
    new AreaTrigger_at_soo_north_tower();
}
