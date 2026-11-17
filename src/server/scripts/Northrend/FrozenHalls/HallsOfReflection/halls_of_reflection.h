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

#ifndef DEF_HALLS_OF_REFLECTION_H
#define DEF_HALLS_OF_REFLECTION_H

// Not from sniffs, but follow to leader bad idea cuz pathfinder working not good right here
const Position lichKingPath[13] =
{
    { 5604.10f, 2180.18f, 731.00f, 0.0f },
    { 5590.01f, 2154.18f, 731.02f, 0.0f },
    { 5561.38f, 2121.81f, 730.76f, 0.0f },
    { 5544.22f, 2094.86f, 731.22f, 0.0f },
    { 5521.25f, 2040.40f, 731.45f, 0.0f },
    { 5511.23f, 2001.50f, 734.63f, 0.0f },
    { 5477.61f, 1958.29f, 738.84f, 0.0f },
    { 5461.02f, 1917.52f, 744.58f, 0.0f },
    { 5433.13f, 1875.21f, 752.60f, 0.0f },
    { 5390.67f, 1820.92f, 758.84f, 0.0f },
    { 5313.33f, 1744.02f, 773.24f, 0.0f },
    { 5282.84f, 1703.20f, 784.62f, 0.0f },
    { 5279.94f, 1699.12f, 785.32f, 0.0f },
};

const Position quelDelarCombatReachPos = { 5293.58f, 1990.19f, 707.69f, 0.0f };

enum Data
{
    MAX_ENCOUNTER          = 4,

    DATA_FALRIC_EVENT      = 1,
    DATA_MARWYN_EVENT      = 2,
    DATA_FROSWORN_EVENT    = 3,
    DATA_LICHKING_EVENT    = 4,

    DATA_INTRO_EVENT       = 5,
    DATA_TEAM_IN_INSTANCE  = 6,
    DATA_WAVE_COUNT        = 7,

    DATA_SUMMONS           = 8,
    DATA_ICE_WALL_1        = 9,
    DATA_ICE_WALL_2        = 10,
    DATA_ICE_WALL_3        = 11,
    DATA_ICE_WALL_4        = 12,

    DATA_PHASE             = 13,
};

enum Data64
{
    DATA_FALRIC            = 101,
    DATA_MARWYN            = 102,
    DATA_LICHKING          = 103,
    DATA_ESCAPE_LIDER      = 104,
    DATA_FROSTMOURNE       = 105,
    DATA_FRONT_DOOR        = 106,
    DATA_FROSTWORN_DOOR    = 107,
    DATA_ARTHAS_DOOR       = 108,
    DATA_GUNSHIP,
};

enum Creatures
{
    NPC_FALRIC                                    = 38112,
    NPC_MARWYN                                    = 38113,
    NPC_FROSTWORN_GENERAL                         = 36723,
    NPC_REFLECTION                                = 37107,
    NPC_LICH_KING_EVENT                           = 37226,
    NPC_QUELDELAR                                 = 37158,
    NPC_FROSTMOURNE_ALTAR_BUNNY                   = 37704,
    NPC_LICH_KING                                 = 36954,

    NPC_UTHER                                     = 37225,
    NPC_JAINA_PART1                               = 37221,
    NPC_JAINA_OUTRO                               = 36955,
    NPC_SYLVANAS_PART1                            = 37223,
    NPC_SYLVANAS_OUTRO                            = 37554,
    NPC_KORELN                                    = 37582,
    NPC_LORALEN                                   = 37779,

    NPC_WAVE_MERCENARY                            = 38177,
    NPC_WAVE_FOOTMAN                              = 38173,
    NPC_WAVE_RIFLEMAN                             = 38176,
    NPC_WAVE_PRIEST                               = 38175,
    NPC_WAVE_MAGE                                 = 38172,
    NPC_WAVE_LOOT_DUMMY                           = 37906,

    NPC_ICE_WALL_TARGET                           = 37014,
    NPC_RAGING_GNOUL                              = 36940,
    NPC_RISEN_WITCH_DOCTOR                        = 36941,
    NPC_ABON                                      = 37069,

    NPC_GUNSHIP_CANNON_HORDE                    = 37593,
    NPC_JUSTIN_BARTLETT                         = 30344,
    NPC_KORM_BLACKSCAR                          = 30824,
    NPC_CHIEF_ENGINEER_COPPERCLAW               = 30825,

    NPC_WORLD_TRIGGER                           = 22515
};

enum GameObjects
{
    GO_FROSTMOURNE               = 202302,
    GO_FROSTMOURNE_ALTAR         = 202236,
    GO_FRONT_DOOR                = 201976,
    GO_FROSTWORN_DOOR            = 197341,
    GO_ARTHAS_DOOR               = 197342,
    GO_RUN_DOOR                  = 197343,

    GO_ICE_WALL                  = 201385,
    GO_CAVE                      = 201596,

    GO_THE_SKYBREAKER                           = 201598,
    GO_ORGRIMS_HAMMER                           = 201599,
    GO_THE_SKYBREAKER_STAIRS                    = 201709,
    GO_ORGRIMS_HAMMER_STAIRS                    = 202211,
    GO_PORTAL_TO_DALARAN                        = 195682,

    GO_THE_CAPTAIN_CHEST_ALLIANCE_NORMAL        = 201710,
    GO_THE_CAPTAIN_CHEST_HORDE_NORMAL           = 202212,
    GO_THE_CAPTAIN_CHEST_ALLIANCE_HEROIC        = 202336,
    GO_THE_CAPTAIN_CHEST_HORDE_HEROIC           = 202337
};

enum Achievements
{
    ACHIEV_HALLS_OF_REFLECTION_N   = 4518,
    ACHIEV_HALLS_OF_REFLECTION_H   = 4521,
    ACHIEV_NOT_RETREATING_EVENT    = 22615,
};

enum InstanceSpells
{
    SPELL_SPIRIT_ACTIVATE_VISUAL                = 72130,
    SPELL_WELL_OF_SOULS_SMALL                   = 72630,
    SPELL_START_HALLS_OF_REFLECTION_QUEST_A     = 71351,
    SPELL_START_HALLS_OF_REFLECTION_QUEST_H     = 71542,
    SPELL_START_HALLS_OF_REFLECTION_QUEST_AE    = 72900,
    SPELL_ACHIEV_CHECK                          = 72830,
};

const Position OutroSpawns[2] = 
{
    {5564.25f, 2274.69f, 733.01f, 3.93f}, // Lich King
    {5556.27f, 2266.28f, 733.01f, 0.8f},  // Jaina/Sylvana
};

const Position impenetrableDoorPos = { 5354.01f, 2053.53f, 707.695f, 0.785398f };

enum HorWorldStates
{
    WORLD_STATE_HOR                               = 4884,
    WORLD_STATE_HOR_WAVE_COUNT                    = 4882,
    WORLD_STATE_HOR_APPROACHING_LICH_KING         = 4956,
    WORLD_STATE_HOR_LICH_KING_CHASE               = 4957,
};

// Common actions from Instance Script to Boss Script
enum Actions
{
    ACTION_ENTER_COMBAT,
    ACTION_SAY_INTRO_1,
    ACTION_SAY_INTRO_2,
    ACTION_SAY_GAUNTLET_WIPE,
    ACTION_SAY_GAUNTLET_RESUME_AFTER_WIPE,
    ACTION_SAY_GAUNTLET_RESUME,
    ACTION_LICH_KING_STOP_FOLLOW,
    ACTION_GUNSHIP_ARRIVAL                      = -668004,
    ACTION_GUNSHIP_ARRIVAL_2                    = -668005
};

enum EscapeEnum
{
    SAY_JAINA_SYLVANAS_AGGRO        = 0,
    SAY_JAINA_SYLVANAS_WALL_01      = 1,
    SAY_JAINA_SYLVANAS_WALL_02      = 2,
    SAY_JAINA_SYLVANAS_WALL_03      = 3,
    SAY_JAINA_SYLVANAS_WALL_04      = 4,
    SAY_JAINA_SYLVANAS_ESCAPE_01    = 5,
    SAY_JAINA_SYLVANAS_ESCAPE_02    = 6,
    SAY_JAINA_SYLVANAS_FINAL_2      = 7,
    SAY_JAINA_FINAL_3               = 8,

    SAY_LICH_KING_AGGRO_A           = 1,
    SAY_LICH_KING_AGGRO_H           = 2,
    SAY_LICH_KING_WINTER            = 3,
    SAY_LICH_KING_WIN               = 7,
    SAY_LICH_KING_SUMMON_GHOULS     = 9,
    SAY_LICH_KING_SUMMON_MINIONS    = 10,
    SAY_LICH_KING_WALL_01           = 6,
    SAY_LICH_KING_WALL_02           = 4,
    SAY_LICH_KING_WALL_03           = 5,
    SAY_LICH_KING_WALL_04           = 8,
    SAY_LICH_KING_ESCAPE_03         = 0,

    SAY_BARTLETT_KORM_FIRE          = 0,
    SAY_BARTLETT_KORM_FINAL_1       = 1,

    SPELL_REMORSELESS_WINTER        = 69780,
    SPELL_FURY_OF_FROSTMOURNE       = 70063,
    SPELL_PAIN_AND_SUFFERING        = 74115,
    SPELL_ICE_PRISON                = 69708,
    SPELL_DARK_ARROW                = 70194,
    SPELL_STUN_BREAK_JAINA          = 69764,
    SPELL_STUN_BREAK_SYLVANAS       = 70200,
    SPELL_ICE_BARRIER               = 69787,
    SPELL_DESTROY_ICE_WALL_A        = 69784,
    SPELL_DESTROY_ICE_WALL_H        = 70224,
    SPELL_SYLVANA_JUMP              = 68339,
    SPELL_SYLVANA_STEP              = 69087,

    // Quest Credits
    SPELL_CREDIT_FINDING_SYLVANAS               = 71536,
    SPELL_CREDIT_FINDING_JAINA                  = 71538,
    SPELL_CREDIT_ESCAPING_ARTHAS                = 71352,

    // Gunship
    SPELL_GUNSHIP_CANNON_FIRE                   = 70017,
    SPELL_GUNSHIP_CANNON_FIRE_MISSILE_ALLIANCE  = 70021,
    SPELL_GUNSHIP_CANNON_FIRE_MISSILE_HORDE     = 70246
};

enum InstanceEventIds
{
    EVENT_GUNSHIP_ARRIVAL                       = 22709,
    EVENT_GUNSHIP_ARRIVAL_2                     = 22714,
    EVENT_ICE_WALL_SUMMONED                     = 22795
};

Position const GauntletEvadeCheckPosition = { 5230.523926f, 1923.353516f, 707.694824f };
float const GauntletEvadeCheckDistance = 50.0f;

// Base class for FALRIC and MARWYN
// handled the summonList and the notification events to/from the InstanceScript
struct boss_horAI : ScriptedAI
{
    boss_horAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;
    EventMap events;

    void Reset() override
    {
        events.Reset();
        me->SetVisible(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_IMMUNE_TO_PC);
        me->SetReactState(REACT_PASSIVE);
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
    {
        ScriptedAI::EnterEvadeMode();
        instance->SetData(DATA_WAVE_COUNT, NOT_STARTED);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            damage = 0;
    }

    bool CanAIAttack(Unit const* who) const override
    {
        return who->GetExactDist2d(&GauntletEvadeCheckPosition) > GauntletEvadeCheckDistance;
    }
};

#endif
