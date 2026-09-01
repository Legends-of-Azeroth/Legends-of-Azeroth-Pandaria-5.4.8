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
#include "PassiveAI.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "CreatureTextMgr.h"
#include "ulduar.h"
#include "Group.h"
#include "Random.h"

enum Yells
{
    // Sara
    SAY_SARA_ULDUAR_SCREAM_1                = 0,  // screams randomly in a whole instance, unused on retail
    SAY_SARA_ULDUAR_SCREAM_2                = 1,  // screams randomly in a whole instance, unused on retail
    SAY_SARA_AGGRO                          = 2,
    SAY_SARA_FERVOR_HIT                     = 3,
    SAY_SARA_BLESSING_HIT                   = 4,
    SAY_SARA_KILL                           = 5,
    SAY_SARA_TRANSFORM_1                    = 6,
    SAY_SARA_TRANSFORM_2                    = 7,
    SAY_SARA_TRANSFORM_3                    = 8,
    SAY_SARA_TRANSFORM_4                    = 9,
    SAY_SARA_DEATH_RAY                      = 10,
    SAY_SARA_PSYCHOSIS_HIT                  = 11,

    // Yogg-Saron
    SAY_YOGG_SARON_SPAWN                    = 0,
    SAY_YOGG_SARON_MADNESS                  = 1,
    EMOTE_YOGG_SARON_MADNESS                = 2,
    SAY_YOGG_SARON_PHASE_3                  = 3,
    SAY_YOGG_SARON_DEAFENING_ROAR           = 4,
    EMOTE_YOGG_SARON_DEAFENING_ROAR         = 5,
    SAY_YOGG_SARON_DEATH                    = 6,
    EMOTE_YOGG_SARON_EMPOWERING_SHADOWS     = 7,
    EMOTE_YOGG_SARON_EXTINGUISH_ALL_LIFE    = 8,

    // Voice of Yogg-Saron
    WHISPER_VOICE_PHASE_1_WIPE              = 0,
    WHISPER_VOICE_INSANE                    = 1,

    // Brain of Yogg-Saron
    EMOTE_BRAIN_ILLUSION_SHATTERED          = 0,

    // Ominous Cloud
    EMOTE_OMINOUS_CLOUD_PLAYER_TOUCH        = 0,

    // Keepers
    SAY_KEEPER_CHOSEN_1                     = 0,
    SAY_KEEPER_CHOSEN_2                     = 1,

    // Yogg-Saron illusions
    SAY_STORMWIND_ROLEPLAY_4                = 0,
    SAY_STORMWIND_ROLEPLAY_7                = 1,
    SAY_ICECROWN_ROLEPLAY_5                 = 2,
    SAY_ICECROWN_ROLEPLAY_6                 = 3,
    SAY_CHAMBER_ROLEPLAY_5                  = 4,

    // Neltharion
    SAY_CHAMBER_ROLEPLAY_1                  = 0,
    SAY_CHAMBER_ROLEPLAY_3                  = 1,

    // Ysera
    SAY_CHAMBER_ROLEPLAY_2                  = 0,

    // Malygos
    SAY_CHAMBER_ROLEPLAY_4                  = 0,

    // Immolated Champion
    SAY_ICECROWN_ROLEPLAY_1                 = 0,
    SAY_ICECROWN_ROLEPLAY_3                 = 1,

    // The Lich King
    SAY_ICECROWN_ROLEPLAY_2                 = 0,
    SAY_ICECROWN_ROLEPLAY_4                 = 1,

    // Garona
    SAY_STORMWIND_ROLEPLAY_1                = 0,
    SAY_STORMWIND_ROLEPLAY_2                = 1,
    SAY_STORMWIND_ROLEPLAY_3                = 2,
    SAY_STORMWIND_ROLEPLAY_6                = 3,

    // King Llane
    SAY_STORMWIND_ROLEPLAY_5                = 0,
};

enum Spells
{
    // Voice of Yogg-Saron
    SPELL_SUMMON_GUARDIAN_2                 = 62978,
    SPELL_SANITY_PERIODIC                   = 63786,
    SPELL_SANITY                            = 63050,
    SPELL_INSANE_PERIODIC                   = 64554,
    SPELL_INSANE                            = 63120,
    //SPELL_CLEAR_INSANE                      = 63122,  // when it should be casted?
    SPELL_CONSTRICTOR_TENTACLE              = 64132,
    SPELL_CRUSHER_TENTACLE_SUMMON           = 64139,
    SPELL_CORRUPTOR_TENTACLE_SUMMON         = 64143,
    SPELL_IMMORTAL_GUARDIAN                 = 64158,

    // Sara
    SPELL_SARAS_FERVOR                      = 63138,
    SPELL_SARAS_FERVOR_TARGET_SELECTOR      = 63747,
    SPELL_SARAS_BLESSING                    = 63134,
    SPELL_SARAS_BLESSING_TARGET_SELECTOR    = 63745,
    SPELL_SARAS_ANGER                       = 63147,
    SPELL_SARAS_ANGER_TARGET_SELECTOR       = 63744,
    SPELL_FULL_HEAL                         = 43978,
    SPELL_PHASE_2_TRANSFORM                 = 65157,
    SPELL_SHADOWY_BARRIER_SARA              = 64775,
    SPELL_RIDE_YOGG_SARON_VEHICLE           = 61791,
    SPELL_PSYCHOSIS                         = 63795,
    SPELL_MALADY_OF_THE_MIND                = 63830,
    SPELL_BRAIN_LINK                        = 63802,
    SPELL_BRAIN_LINK_DAMAGE                 = 63803,  // red beam
    SPELL_BRAIN_LINK_NO_DAMAGE              = 63804,  // yellow beam
    SPELL_DEATH_RAY                         = 63891,

    // Ominous Cloud
    SPELL_OMINOUS_CLOUD_VISUAL              = 63084,
    SPELL_SUMMON_GUARDIAN_1                 = 63031,

    // Guardian of Yogg-Saron
    SPELL_DARK_VOLLEY                       = 63038,
    SPELL_SHADOW_NOVA                       = 62714,
    SPELL_SHADOW_NOVA_2                     = 65719,

    // Yogg-Saron
    SPELL_EXTINGUISH_ALL_LIFE               = 64166,
    SPELL_SHADOWY_BARRIER_YOGG              = 63894,
    SPELL_KNOCK_AWAY                        = 64022,
    SPELL_PHASE_3_TRANSFORM                 = 63895,
    SPELL_DEAFENING_ROAR                    = 64189,
    SPELL_LUNATIC_GAZE                      = 64163,
    SPELL_LUNATIC_GAZE_DAMAGE               = 64164,
    SPELL_SHADOW_BEACON                     = 64465,

    // Brain of Yogg-Saron
    SPELL_MATCH_HEALTH                      = 64066,
    SPELL_MATCH_HEALTH_2                    = 64069,
    SPELL_INDUCE_MADNESS                    = 64059,
    SPELL_BRAIN_HURT_VISUAL                 = 64361,
    SPELL_SHATTERED_ILLUSION                = 64173,
    SPELL_SHATTERED_ILLUSION_REMOVE         = 65238,

    // Tentacles
    SPELL_ERUPT                             = 64144,
    SPELL_TENTACLE_VOID_ZONE                = 64017,  // used by Corruptor Tentacle and Crusher Tentacle only

    // Crusher Tentacle
    SPELL_DIMINISH_POWER                    = 64145,
    SPELL_DIMINSH_POWER                     = 64148,
    SPELL_FOCUSED_ANGER                     = 57688,
    SPELL_CRUSH                             = 64146,
    //SPELL_CRUSH_2                           = 65201,  // triggered by SPELL_CRUSH, basepoints of SPELL_MALADY_OF_THE_MIND

    // Constrictor Tentacle
    SPELL_TENTACLE_VOID_ZONE_2              = 64384,
    SPELL_LUNGE                             = 64131,

    // Corruptor Tentacle
    SPELL_APATHY                            = 64156,
    SPELL_BLACK_PLAGUE                      = 64153,
    SPELL_CURSE_OF_DOOM                     = 64157,
    SPELL_DRAINING_POISON                   = 64152,

    // Immortal Guardian
    SPELL_EMPOWERING_SHADOWS                = 64468,
    SPELL_EMPOWERED                         = 64161,
    SPELL_EMPOWERED_BUFF                    = 65294,
    SPELL_WEAKENED                          = 64162,
    SPELL_DRAIN_LIFE                        = 64159,
    SPELL_RECENTLY_SPAWNED                  = 64497,
    SPELL_SIMPLE_TELEPORT                   = 64195,

    // Keepers at Observation Ring
    SPELL_TELEPORT                          = 62940,

    // Keepers
    SPELL_SIMPLE_TELEPORT_KEEPERS           = 12980,
    SPELL_KEEPER_ACTIVE                     = 62647,

    // Mimiron
    SPELL_SPEED_OF_INVENTION                = 62671,
    SPELL_DESTABILIZATION_MATRIX            = 65206,

    // Freya
    SPELL_RESILIENCE_OF_NATURE              = 62670,
    SPELL_SANITY_WELL_SUMMON                = 64170,

    // Sanity Well
    SPELL_SANITY_WELL_VISUAL                = 63288,
    SPELL_SANITY_WELL                       = 64169,

    // Thorim
    SPELL_FURY_OF_THE_STORM                 = 62702,
    SPELL_TITANIC_STORM                     = 64171,

    // Hodir
    SPELL_FORTITUDE_OF_FROST                = 62650,
    SPELL_HODIRS_PROTECTIVE_GAZE            = 64174,
    SPELL_FLASH_FREEZE_VISUAL               = 64176,

    // Death Orb
    SPELL_DEATH_RAY_ORIGIN_VISUAL           = 63893,

    // Death Ray
    SPELL_DEATH_RAY_WARNING_VISUAL          = 63882,
    SPELL_DEATH_RAY_PERIODIC                = 63883,
    SPELL_DEATH_RAY_DAMAGE_VISUAL           = 63886,

    // Laughing Skull
    SPELL_LUNATIC_GAZE_SKULL                = 64167,

    // Descend Into Madness
    SPELL_TELEPORT_PORTAL_VISUAL            = 64416,

    // Illusions
    SPELL_GRIM_REPRISAL                     = 63305,
    SPELL_GRIM_REPRISAL_DAMAGE              = 64039,

    // Suit of Armor
    SPELL_NONDESCRIPT_1                     = 64013,

    // Dragon Consorts & Deathsworn Zealot
    SPELL_NONDESCRIPT_2                     = 64010,

    // Garona
    SPELL_ASSASSINATE                       = 64063,

    // King Llane
    SPELL_PERMANENT_FEIGN_DEATH             = 29266,

    // The Lich King
    SPELL_DEATHGRASP                        = 63037,

    // Turned Champion
    SPELL_VERTEX_COLOR_BLACK                = 39662,

    // Player self cast spells
    SPELL_MALADY_OF_THE_MIND_JUMP           = 63881,
    SPELL_ILLUSION_ROOM                     = 63988,
    SPELL_HATE_TO_ZERO                      = 63984,
    SPELL_TELEPORT_BACK_TO_MAIN_ROOM        = 63992,
    SPELL_INSANE_VISUAL                     = 64464,
    SPELL_CONSTRICTOR_TENTACLE_SUMMON       = 64133,
    SPELL_SQUEEZE                           = 64125,
    SPELL_SQUEEZE_25                        = 64126,
    SPELL_FLASH_FREEZE                      = 64175,
    SPELL_LOW_SANITY_SCREEN_EFFECT          = 63752,

    SPELL_IN_THE_MAWS_OF_THE_OLD_GOD        = 64184,
};

enum Phases
{
    PHASE_ONE               = 1,
    PHASE_TRANSFORM         = 2,
    PHASE_TWO               = 3,
    PHASE_THREE             = 4,
};

enum Events
{
    // Voice of Yogg-Saron
    EVENT_LOCK_DOOR                         = 1,
    EVENT_SUMMON_GUARDIAN_OF_YOGG_SARON     = 2,
    EVENT_SUMMON_CORRUPTOR_TENTACLE         = 3,
    EVENT_SUMMON_CONSTRICTOR_TENTACLE       = 4,
    EVENT_SUMMON_CRUSHER_TENTACLE           = 5,
    EVENT_ILLUSION                          = 6,
    EVENT_SUMMON_IMMORTAL_GUARDIAN          = 7,
    EVENT_EXTINGUISH_ALL_LIFE               = 8,    // handled by Voice, timer starts at the beginning of the fight (Yogg-Saron is not spawned at this moment)

    // Sara
    EVENT_SARAS_FERVOR                      = 9,
    EVENT_SARAS_BLESSING                    = 10,
    EVENT_SARAS_ANGER                       = 11,
    EVENT_TRANSFORM_1                       = 12,
    EVENT_TRANSFORM_2                       = 13,
    EVENT_TRANSFORM_3                       = 14,
    EVENT_TRANSFORM_4                       = 15,
    EVENT_PSYCHOSIS                         = 16,
    EVENT_MALADY_OF_THE_MIND                = 17,
    EVENT_BRAIN_LINK                        = 18,
    EVENT_DEATH_RAY                         = 19,

    // Tentacles
    EVENT_DIMINISH_POWER                    = 20,
    EVENT_CAST_RANDOM_SPELL                 = 21,

    // Yogg-Saron
    EVENT_YELL_BOW_DOWN                     = 22,
    EVENT_SHADOW_BEACON                     = 23,
    EVENT_LUNATIC_GAZE                      = 24,
    EVENT_DEAFENING_ROAR                    = 25,  // only on 25-man with 0-3 keepers active (Hard Mode)

    // Guardian of Yogg-Saron
    EVENT_DARK_VOLLEY                       = 26,

    // Immortal Guardian
    EVENT_DRAIN_LIFE                        = 27,

    // Keepers
    EVENT_DESTABILIZATION_MATRIX            = 28,
    EVENT_HODIRS_PROTECTIVE_GAZE            = 29,

    // Chamber Illusion
    EVENT_CHAMBER_ROLEPLAY_1                = 30,
    EVENT_CHAMBER_ROLEPLAY_2                = 31,
    EVENT_CHAMBER_ROLEPLAY_3                = 32,
    EVENT_CHAMBER_ROLEPLAY_4                = 33,
    EVENT_CHAMBER_ROLEPLAY_5                = 34,

    // Icecrown Illusion
    EVENT_ICECROWN_ROLEPLAY_1               = 35,
    EVENT_ICECROWN_ROLEPLAY_2               = 36,
    EVENT_ICECROWN_ROLEPLAY_3               = 37,
    EVENT_ICECROWN_ROLEPLAY_4               = 38,
    EVENT_ICECROWN_ROLEPLAY_5               = 39,
    EVENT_ICECROWN_ROLEPLAY_6               = 40,

    // Stormwind Illusion
    EVENT_STORMWIND_ROLEPLAY_1              = 41,
    EVENT_STORMWIND_ROLEPLAY_2              = 42,
    EVENT_STORMWIND_ROLEPLAY_3              = 43,
    EVENT_STORMWIND_ROLEPLAY_4              = 44,
    EVENT_STORMWIND_ROLEPLAY_5              = 45,
    EVENT_STORMWIND_ROLEPLAY_6              = 46,
    EVENT_STORMWIND_ROLEPLAY_7              = 47,
};

enum EventGroups
{
    EVENT_GROUP_SUMMON_TENTACLES            = 1,
};

enum Actions
{
    ACTION_PHASE_TRANSFORM              = 0,
    ACTION_PHASE_TWO                    = 1,
    ACTION_PHASE_THREE                  = 2,
    ACTION_INDUCE_MADNESS               = 3,
    ACTION_SANITY_WELLS                 = 4,
    ACTION_FLASH_FREEZE                 = 5,
    ACTION_TENTACLE_KILLED              = 6,
    ACTION_START_ROLEPLAY               = 8,
    ACTION_TOGGLE_SHATTERED_ILLUSION    = 9,
    ACTION_DEACTIVATE_CLOUDS            = 10,
    ACTION_ACTIVATE_CLOUDS              = 11,
    ACTION_BRAIN_INDUCED_MADNESS        = 12,
};

Position const CloudsSpawnPos[6] =
{
    { 1993.54f, -17.934f, 324.973f, 0.0f   },
    { 1968.16f, -42.6564f, 324.968f, 0.0f  },
    { 1952.9f, -27.3639f, 325.693f, 0.0f   },
    { 1991.67f, -65.3991f, 328.343f, 0.0f  },
    { 2018.57f, 4.08873f, 328.874f, 0.0f   },
    { 2034.44f, -25.2996f, 329.055f, 0.0f  }
};
Position const CloudsPortals10[4] =
{
    { 1964.6f, -42.7056f, 325.085f, 0.0f   },
    { 1986.94f, -46.2108f, 324.98f, 0.0f   },
    { 1989.49f, -6.69694f, 325.079f, 0.0f  },
    { 1965.52f, -8.0893f, 324.948f, 0.0f   }
};
Position const CloudsPortals25[10] =
{
    { 1964.6f, -42.7056f, 325.085f, 0.0f   },
    { 1986.94f, -46.2108f, 324.98f, 0.0f   },
    { 1989.49f, -6.69694f, 325.079f, 0.0f  },
    { 1965.52f, -8.0893f, 324.948f, 0.0f   },
    { 2000.84f, -25.4001f, 325.192f, 0.0f  },
    { 1960.22f, -26.1414f, 325.01f, 0.0f   },
    { 1976.3f, -47.8254f, 325.112f, 0.0f   },
    { 1997.69f, -37.4608f, 325.039f, 0.0f  },
    { 1998.07f, -13.3567f, 325.167f, 0.0f  },
    { 1976.99f, -3.95909f, 325.169f, 0.0f  }
};
// Start Illusions
// Chamber Illusion Spawn Positions
Position const ChamberIllusionNeltharion         = { 2117.713f, -25.27073f, 242.73f, 3.141593f };
Position const ChamberIllusionMalygos            = { 2107.963f, -37.29351f, 242.73f, 2.024582f };
Position const ChamberIllusionAlextrasza         = { 2092.347f, -25.26042f, 242.73f, 6.265732f };
Position const ChamberIllusionYsera              = { 2109.765f, -14.37967f, 242.73f, 4.206244f };
Position const ChamberIllusionObsidianConsort[2] =
{
    { 2146.88f, -17.03125f, 239.8058f, 3.35103f   },
    { 2146.24f, -34.40451f, 239.8058f, 3.01942f   },
};
Position const ChamberIllusionAzureConsort[2]    =
{
    { 2113.33f, -65.71007f, 239.8058f, 1.780236f  },
    { 2139.83f, -50.28646f, 239.8058f, 2.460914f  },
};
Position const ChamberIllusionEmeraldConsort[2]  =
{
    { 2116.931f, 11.375f, 239.8058f, 4.415683f    },
    { 2136.641f, -1.996528f, 239.8058f, 3.839724f },
};
Position const ChamberIllusionRubyConsort[2]     =
{
    { 2069.479f, -5.699653f, 239.8058f, 5.427974f },
    { 2069.298f, -43.53168f, 239.8006f, 0.471238f },
};
Position const ChamberIllusionYoggIllusion       =  {2109.695f, -25.09549f, 222.3250f, 0.0f };
Position const ChamberIllusionLaughingSkull[7]   =
{
    { 2134.533f, 18.86881f, 239.8042f, 1.850049f  },
    { 2063.156f, 27.95839f, 244.2707f, 5.288348f  },
    { 2070.133f, -73.0108f, 242.5237f, 1.308997f  },
    { 2135.219f, -41.4427f, 239.8027f, 5.846853f  },
    { 2088.254f, 5.053657f, 239.8672f, 0.9948376f },
    { 2119.228f, 1.421441f, 239.8039f, 0.0f       },
    { 2102.441f, -75.0454f, 239.8635f, 1.658063f  }
};
// Icecrown Illusion Spawn Positions
Position const IcecrownIllusionLichKing          = { 1908.557f, -152.4427f, 240.0719f, 4.328416f };
Position const IcecrownIllusionImmolatedChampion = { 1903.599f, -160.3576f, 240.1064f, 1.082104f };
Position const IcecrownIllusionTurnedChampion    = { 1919.113f, -160.9657f, 240.0659f, 2.700784f };
Position const IcecrownIllusionDeathswornZealot[9] =
{
    { 1917.559f, -135.7448f, 240.0730f, 4.188790f },
    { 1919.125f, -140.9566f, 240.0730f, 3.979351f },
    { 1948.469f, -136.2951f, 240.0707f, 3.438299f },
    { 1956.444f, -138.4028f, 240.1078f, 3.368485f },
    { 1952.965f, -130.5295f, 240.1347f, 3.804818f },
    { 1902.132f, -111.3594f, 240.0698f, 4.852015f },
    { 1905.326f, -104.7865f, 240.0523f, 4.764749f },
    { 1897.345f, -106.6076f, 240.1444f, 4.939282f },
    { 1912.129f, -136.9340f, 240.0730f, 4.188790f }
};
Position const IcecrownIllusionYoggIllusion      = {1906.226f, -155.8941f, 223.4727f, 0.0f};
Position const IcecrownIllusionLaughingSkull[8]  =
{
    { 1863.897f, -100.9364f, 240.073f, 5.235988f },
    { 1948.668f, -152.4481f, 240.073f, 1.919862f },
    { 1932.213f, -93.62798f, 240.073f, 4.852015f },
    { 1964.457f, -149.5943f, 240.073f, 3.385939f },
    { 1992.463f, -131.2119f, 240.073f, 3.455752f },
    { 1901.194f, -90.4624f, 240.073f, 0.1396263f },
    { 1923.746f, -164.5339f, 240.073f, 3.455752f },
    { 1905.937f, -133.1651f, 240.073f, 5.77704f  }
};
// Stormwind Illusion Spawn Positions
Position const StormwindIllusionGarona           = {1931.064f, 60.5659f, 241.5013f, 2.084251f};
Position const StormwindIllusionKingLlane        = {1928.349f, 66.05186f, 242.4596f, 5.113815f};
Position const StormwindIllusionSuitArmor[8]     =
{
    { 1956.503f, 72.19462f, 239.7495f, 3.281219f },
    { 1951.04f, 49.88875f, 239.7495f, 2.495821f },
    { 1931.14f, 38.46949f, 239.7495f, 1.710423f },
    { 1908.993f, 44.26659f, 239.7495f, 0.9250245f },
    { 1897.344f, 64.31419f, 239.7495f, 0.1396263f },
    { 1903.393f, 86.60285f, 239.7495f, 5.61996f },
    { 1923.342f, 98.01228f, 239.7495f, 4.834562f },
    { 1945.442f, 92.17952f, 239.7495f, 4.049164f }
};
Position const StormwindIllusionYoggIllusion     = { 1929.160f, 67.75694f, 221.7322f, 0.0f };
Position const StormwindIllusionLaughingSkull[6]     =
{
    { 1941.561f, 109.4812f, 239.7496f, 3.316126f  },
    { 1922.55f, 54.68001f, 241.1263f,  0.1919862f },
    { 1893.146f, 44.24343f, 239.7496f, 0.0f       },
    { 1926.526f, 43.26215f, 239.7496f, 0.9948376f },
    { 1904.3f, 73.70443f, 239.7496f,   4.276057f  },
    { 1955.173f, 85.26153f, 239.7496f, 4.049164f  }
};
// End Illusions

Position const YoggSaronSpawnPos            = { 1980.43f, -25.7708f, 324.9724f, 3.141593f };
Position const ObservationRingKeepersPos[4] =
{
    { 1945.682f,  33.34201f, 411.4408f, 5.270895f },  // Freya
    { 1945.761f, -81.52171f, 411.4407f, 1.029744f },  // Hodir
    { 2028.822f, -65.73573f, 411.4426f, 2.460914f },  // Thorim
    { 2028.766f,  17.42014f, 411.4446f, 3.857178f },  // Mimiron
};
Position const YSKeepersPos[4] =
{
    { 2036.873f,  25.42513f, 338.4984f, 3.909538f },  // Freya
    { 1939.045f, -90.87457f, 338.5426f, 0.994837f },  // Hodir
    { 1939.148f,  42.49035f, 338.5427f, 5.235988f },  // Thorim
    { 2036.658f, -73.58822f, 338.4985f, 2.460914f },  // Mimiron
};
Position const IllusionsMiscPos[2] =
{
    { 1928.793f,  65.03109f, 242.3763f, 0.0f }, // Garona end position
    { 1912.324f, -155.7967f, 239.9896f, 0.0f }, // Saurfang end position
};

Position const WipePos            = { 1856.423f, -26.122f, 334.161f, 6.268605f };
 
enum MiscData
{
    ACHIEV_TIMED_START_EVENT                = 21001,
    SOUND_LUNATIC_GAZE                      = 15757,

    LOOT_ALWAYS = 1,
    LOOT_EASY   = 2,
    LOOT_HARD   = 4,
    LOOT_BONUS  = 8,
    LOOT_QUEST  = 16,
};

class StartAttackEvent : public BasicEvent
{
    public:
        StartAttackEvent(Creature* summoner, Creature* owner)
            : _summoner(summoner), _owner(owner)
        {
        }

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _owner->SetReactState(REACT_AGGRESSIVE);
            if (Unit* target = _summoner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 300.0f))
                _owner->AI()->AttackStart(target);
            return true;
        }

    private:
        Creature* _summoner;
        Creature* _owner;
};

class boss_voice_of_yogg_saron : public CreatureScript
{
    public:
        boss_voice_of_yogg_saron() : CreatureScript("boss_voice_of_yogg_saron") { }

        struct boss_voice_of_yogg_saronAI : public BossAI
        {
            boss_voice_of_yogg_saronAI(Creature* creature) : BossAI(creature, BOSS_YOGG_SARON)
            {
                SetCombatMovement(false);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                // TODO: MoveInLineOfSight doesn't work for such a big distance
                if (who->GetTypeId() == TYPEID_PLAYER && me->GetDistance2d(who) < 99.0f && !me->IsInCombat() && instance->CheckRequiredBosses(BOSS_YOGG_SARON))
                    me->SetInCombatWithZone();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();

                for (uint8 i = DATA_SARA; i <= DATA_MIMIRON_YS; ++i)
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(i)))
                        creature->AI()->EnterEvadeMode();

                // not sure, spoken by Sara (sound), regarding to wowwiki Voice whispers it
                for (auto&& itr : me->GetMap()->GetPlayers())
                    if (Player* player = itr.GetSource())
                    {
                        if (events.IsInPhase(PHASE_ONE))
                            Talk(WHISPER_VOICE_PHASE_1_WIPE, player);

                        player->RemoveAurasDueToSpell(SPELL_SANITY);
                        player->RemoveAurasDueToSpell(SPELL_INSANE);

                        if (player->GetPositionZ() < 300)
                            player->TeleportTo(603, WipePos.GetPositionX(), WipePos.GetPositionY(), WipePos.GetPositionZ(), WipePos.GetOrientation());
                    }
            }

            void Reset() override
            {
                _Reset();
                events.SetPhase(PHASE_ONE);

                me->GetMap()->SetWorldState(WORLDSTATE_DRIVE_ME_CRAZY, 1);
                instance->DoStopTimedAchievement(CRITERIA_START_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
                instance->DoUpdateWorldState(WORLD_STATE_YOGG_SARON, 0);

                _guardiansCount = 0;
                _guardianTimer = 20000;
                _illusionShattered = false;
                _illusionsCount = 0;

                // Allow all grids to load before spawning, otherwise GetCreatureListWithEntryInGrid will fail to retrieve a cloud from another cell
                me->m_Events.Schedule(1, [this]
                {
                summons.DespawnEntry(33292);

                bool clockwise = false;
                for (uint8 i = 0; i < 6; ++i)
                    me->SummonCreature(33292, CloudsSpawnPos[i]);
                std::list<Creature*> clouds;
                GetCreatureListWithEntryInGrid(clouds, me, 33292, 150);
                clouds.sort(Trinity::ObjectDistanceOrderPred(me, true));
                for (auto&& cloud : clouds)
                {
                    if (!cloud->IsInWorld())
                        continue;
                    cloud->AI()->DoAction(int32(clockwise));
                    clockwise = !clockwise;
                }
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (!instance->CheckRequiredBosses(BOSS_YOGG_SARON))
                {
                    EnterEvadeMode();
                    return;
                }

                if (Creature* sara = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SARA)))
                    sara->SetInCombatWith(me);

                for (uint8 i = DATA_FREYA_YS; i <= DATA_MIMIRON_YS; ++i)
                    if (Creature* keeper = ObjectAccessor::GetCreature(*me, instance->GetGuidData(i)))
                        keeper->SetInCombatWith(me);

                instance->DoStartCriteria(CRITERIA_START_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);

                me->CastCustomSpell(SPELL_SUMMON_GUARDIAN_2, SPELLVALUE_MAX_TARGETS, 1);
                DoCast(me, SPELL_SANITY_PERIODIC);

                events.ScheduleEvent(EVENT_LOCK_DOOR, 15000);
                events.ScheduleEvent(EVENT_SUMMON_GUARDIAN_OF_YOGG_SARON, _guardianTimer, 0, PHASE_ONE);
                events.ScheduleEvent(EVENT_EXTINGUISH_ALL_LIFE, 900000);    // 15 minutes
            }

            void JustDied(Unit* killer) override
            {
                // don't despawn Yogg-Saron's corpse, remove him from SummonList!
                if (Creature* yogg = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_YOGG_SARON)))
                    summons.Despawn(yogg);

                BossAI::JustDied(killer);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                // don't summon tentacles when illusion is shattered, delay them
                if (_illusionShattered)
                    events.DelayEvents(diff, EVENT_GROUP_SUMMON_TENTACLES);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LOCK_DOOR:
                            DoCast(me, SPELL_INSANE_PERIODIC);
                            instance->SetBossState(BOSS_YOGG_SARON, IN_PROGRESS);
                            instance->DoUpdateWorldState(WORLD_STATE_YOGG_SARON, 1);
                            break;
                        case EVENT_EXTINGUISH_ALL_LIFE:
                            if (Creature* yogg = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_YOGG_SARON)))
                            {
                                yogg->AI()->Talk(EMOTE_YOGG_SARON_EXTINGUISH_ALL_LIFE, me);
                                yogg->CastSpell((Unit*)nullptr, SPELL_EXTINGUISH_ALL_LIFE, true);
                            }
                            events.ScheduleEvent(EVENT_EXTINGUISH_ALL_LIFE, 10000);    // cast it again after a short while, players can survive
                            break;
                        case EVENT_SUMMON_GUARDIAN_OF_YOGG_SARON:
                            me->CastCustomSpell(SPELL_SUMMON_GUARDIAN_2, SPELLVALUE_MAX_TARGETS, 1);
                            ++_guardiansCount;
                            if (_guardiansCount <= 6 && _guardiansCount % 3 == 0)
                                _guardianTimer -= 5000;
                            events.ScheduleEvent(EVENT_SUMMON_GUARDIAN_OF_YOGG_SARON, _guardianTimer, 0, PHASE_ONE);
                            break;
                        case EVENT_SUMMON_CORRUPTOR_TENTACLE:
                            DoCastAOE(SPELL_CORRUPTOR_TENTACLE_SUMMON);
                            events.ScheduleEvent(EVENT_SUMMON_CORRUPTOR_TENTACLE, 30000 / (_illusionsCount >= 4 ? 2 : 1), EVENT_GROUP_SUMMON_TENTACLES, PHASE_TWO);
                            break;
                        case EVENT_SUMMON_CONSTRICTOR_TENTACLE:
                            DoCastAOE(SPELL_CONSTRICTOR_TENTACLE);
                            events.ScheduleEvent(EVENT_SUMMON_CONSTRICTOR_TENTACLE, 25000 / (_illusionsCount >= 4 ? 2 : 1), EVENT_GROUP_SUMMON_TENTACLES, PHASE_TWO);
                            break;
                        case EVENT_SUMMON_CRUSHER_TENTACLE:
                            DoCastAOE(SPELL_CRUSHER_TENTACLE_SUMMON);
                            events.ScheduleEvent(EVENT_SUMMON_CRUSHER_TENTACLE, 60000 / (_illusionsCount >= 4 ? 2 : 1), EVENT_GROUP_SUMMON_TENTACLES, PHASE_TWO);
                            break;
                        case EVENT_ILLUSION:
                        {
                            ++_illusionsCount;

                            if (Creature* yogg = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_YOGG_SARON)))
                            {
                                yogg->AI()->Talk(EMOTE_YOGG_SARON_MADNESS);
                                yogg->AI()->Talk(SAY_YOGG_SARON_MADNESS);
                            }

                            for (uint8 i = 0; i < RAID_MODE(4, 10); ++i)
                                me->SummonCreature(34072, RAID_MODE(CloudsPortals10[i], CloudsPortals25[i]));

                            uint8 illusion = urand(CHAMBER_ILLUSION, STORMWIND_ILLUSION);
                            instance->SetData(DATA_ILLUSION, illusion);

                            if (Creature* brain = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BRAIN_OF_YOGG_SARON)))
                                brain->AI()->DoAction(ACTION_INDUCE_MADNESS);
                            events.ScheduleEvent(EVENT_ILLUSION, 80000, 0, PHASE_TWO);  // wowwiki says 80 secs, wowhead says something about 90 secs
                            break;
                        }
                        case EVENT_SUMMON_IMMORTAL_GUARDIAN:
                            DoCastAOE(SPELL_IMMORTAL_GUARDIAN);
                            events.ScheduleEvent(EVENT_SUMMON_IMMORTAL_GUARDIAN, 15000, 0, PHASE_THREE);
                            break;
                        default:
                            break;
                    }
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_PHASE_TRANSFORM:
                        events.SetPhase(PHASE_TRANSFORM);
                        summons.DespawnEntry(NPC_OMINOUS_CLOUD);
                        break;
                    case ACTION_PHASE_TWO:
                        instance->DoUpdateWorldState(WORLD_STATE_YOGG_SARON, 2);
                        events.SetPhase(PHASE_TWO);
                        me->SummonCreature(NPC_YOGG_SARON, YoggSaronSpawnPos);
                        if (Creature* brain = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BRAIN_OF_YOGG_SARON)))
                            brain->SetInCombatWithZone();
                        events.ScheduleEvent(EVENT_SUMMON_CORRUPTOR_TENTACLE, 1, EVENT_GROUP_SUMMON_TENTACLES, PHASE_TWO);
                        events.ScheduleEvent(EVENT_SUMMON_CONSTRICTOR_TENTACLE, 1, EVENT_GROUP_SUMMON_TENTACLES, PHASE_TWO);
                        events.ScheduleEvent(EVENT_SUMMON_CRUSHER_TENTACLE, 1, EVENT_GROUP_SUMMON_TENTACLES, PHASE_TWO);
                        events.ScheduleEvent(EVENT_ILLUSION, 60000, 0, PHASE_TWO);
                        break;
                    case ACTION_TOGGLE_SHATTERED_ILLUSION:
                        _illusionShattered = !_illusionShattered;
                        break;
                    case ACTION_PHASE_THREE:
                        instance->DoUpdateWorldState(WORLD_STATE_YOGG_SARON, 3);
                        events.SetPhase(PHASE_THREE);
                        events.ScheduleEvent(EVENT_SUMMON_IMMORTAL_GUARDIAN, 15000, 0, PHASE_THREE);
                        DoCastAOE(SPELL_IMMORTAL_GUARDIAN);
                        break;
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_GUARDIAN_OF_YOGG_SARON:
                        summon->m_Events.AddEvent(new StartAttackEvent(me, summon), summon->m_Events.CalculateTime(1000));
                        break;
                    case NPC_YOGG_SARON:
                        summon->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                        break;
                    case NPC_CONSTRICTOR_TENTACLE:
                        summon->CastSpell(summon, SPELL_LUNGE, true);
                        break;
                    case NPC_CRUSHER_TENTACLE:
                    case NPC_CORRUPTOR_TENTACLE:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                        summon->m_Events.AddEvent(new StartAttackEvent(me, summon), summon->m_Events.CalculateTime(5000));
                        break;
                    case NPC_DESCEND_INTO_MADNESS:
                        summon->CastSpell(summon, SPELL_TELEPORT_PORTAL_VISUAL, true);
                        break;
                    case NPC_IMMORTAL_GUARDIAN:
                        summon->CastSpell(summon, SPELL_SIMPLE_TELEPORT, true);
                        break;
                }

                BossAI::JustSummoned(summon);
            }

        private:
            uint8 _guardiansCount;
            uint32 _guardianTimer;
            bool _illusionShattered;
            uint32 _illusionsCount;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_voice_of_yogg_saronAI>(creature);
        }
};

class boss_sara : public CreatureScript
{
    public:
        boss_sara() : CreatureScript("boss_sara") { }

        struct boss_saraAI : public ScriptedAI
        {
            boss_saraAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            ObjectGuid GetLinkedPlayerGUID(ObjectGuid guid) const
            {
                std::map<ObjectGuid, ObjectGuid>::const_iterator itr = _linkData.find(guid);
                if (itr != _linkData.end())
                    return itr->second;

                return ObjectGuid::Empty;
            }

            void CloudHandling(bool start = false)
            {
                std::list<Creature*> cloudList;
                me->GetCreatureListWithEntryInGrid(cloudList, NPC_OMINOUS_CLOUD, 100.0f);
                for (auto&& cloud : cloudList)
                {
                    if (start)
                        cloud->AI()->DoAction(ACTION_ACTIVATE_CLOUDS);
                    else
                        cloud->AI()->DoAction(ACTION_DEACTIVATE_CLOUDS);
                }
            }

            void SetLinkBetween(ObjectGuid player1, ObjectGuid player2)
            {
                _linkData[player1] = player2;
                _linkData[player2] = player1;
            }

            // called once for each target on aura remove
            void RemoveLinkFrom(ObjectGuid player1)
            {
                _linkData.erase(player1);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (_events.IsInPhase(PHASE_TRANSFORM) && damage >= me->GetHealth())
                    damage = 0;
                if (_events.IsInPhase(PHASE_ONE) && damage >= me->GetHealth())
                {
                    damage = 0;

                    if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                        voice->AI()->DoAction(ACTION_PHASE_TRANSFORM);

                    Talk(SAY_SARA_TRANSFORM_1);
                    _events.SetPhase(PHASE_TRANSFORM);
                    _events.ScheduleEvent(EVENT_TRANSFORM_1, 4700, 0, PHASE_TRANSFORM);
                    _events.ScheduleEvent(EVENT_TRANSFORM_2, 9500, 0, PHASE_TRANSFORM);
                    _events.ScheduleEvent(EVENT_TRANSFORM_3, 14300, 0, PHASE_TRANSFORM);
                    _events.ScheduleEvent(EVENT_TRANSFORM_4, 14500, 0, PHASE_TRANSFORM);
                }
            }

            void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell) override
            {
                if (!roll_chance_i(30) || _events.IsInPhase(PHASE_TRANSFORM))
                    return;

                switch (spell->Id)
                {
                    case SPELL_SARAS_FERVOR:
                        Talk(SAY_SARA_FERVOR_HIT);
                        break;
                    case SPELL_SARAS_BLESSING:
                        Talk(SAY_SARA_BLESSING_HIT);
                        break;
                    case SPELL_PSYCHOSIS:
                        Talk(SAY_SARA_PSYCHOSIS_HIT);
                        break;
                    default:
                        break;
                }
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER && !me->IsInEvadeMode())
                    Talk(SAY_SARA_KILL);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_SARA_AGGRO);
                _events.ScheduleEvent(EVENT_SARAS_FERVOR, 5000, 0, PHASE_ONE);
                _events.ScheduleEvent(EVENT_SARAS_BLESSING, urand(10000, 30000), 0, PHASE_ONE);
                _events.ScheduleEvent(EVENT_SARAS_ANGER, urand(15000, 25000), 0, PHASE_ONE);
                CloudHandling(true);
            }

            void Reset() override
            {
                me->RemoveAllAuras();
                me->SetReactState(REACT_PASSIVE);
                me->SetFaction(35);
                me->SetVisible(true);
                _events.Reset();
                _events.SetPhase(PHASE_ONE);
                CloudHandling(false);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat())
                    return;

                if (me->HasAura(SPELL_SHATTERED_ILLUSION))
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SARAS_FERVOR:
                            me->CastCustomSpell(SPELL_SARAS_FERVOR_TARGET_SELECTOR, SPELLVALUE_MAX_TARGETS, 1);
                            _events.ScheduleEvent(EVENT_SARAS_FERVOR, 6000, 0, PHASE_ONE);
                            break;
                        case EVENT_SARAS_ANGER:
                            me->CastCustomSpell(SPELL_SARAS_ANGER_TARGET_SELECTOR, SPELLVALUE_MAX_TARGETS, 1);
                            _events.ScheduleEvent(EVENT_SARAS_ANGER, urand(6000, 8000), 0, PHASE_ONE);
                            break;
                        case EVENT_SARAS_BLESSING:
                            me->CastCustomSpell(SPELL_SARAS_BLESSING_TARGET_SELECTOR, SPELLVALUE_MAX_TARGETS, 1);
                            _events.ScheduleEvent(EVENT_SARAS_BLESSING, urand(6000, 30000), 0, PHASE_ONE);
                            break;
                        case EVENT_TRANSFORM_1:
                            Talk(SAY_SARA_TRANSFORM_2);
                            break;
                        case EVENT_TRANSFORM_2:
                            Talk(SAY_SARA_TRANSFORM_3);
                            break;
                        case EVENT_TRANSFORM_3:
                            Talk(SAY_SARA_TRANSFORM_4);
                            DoCast(me, SPELL_FULL_HEAL);
                            me->SetFaction(16);
                            if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                                voice->AI()->DoAction(ACTION_PHASE_TWO);
                            if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MIMIRON_YS)))
                                mimiron->AI()->DoAction(ACTION_PHASE_TWO);
                            break;
                        case EVENT_TRANSFORM_4:
                            DoCast(me, SPELL_PHASE_2_TRANSFORM);
                            if (Creature* yogg = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_YOGG_SARON)))
                                DoCast(yogg, SPELL_RIDE_YOGG_SARON_VEHICLE);
                            DoCast(me, SPELL_SHADOWY_BARRIER_SARA);
                            _events.SetPhase(PHASE_TWO);
                            _events.ScheduleEvent(EVENT_DEATH_RAY, 20000, 0, PHASE_TWO);    // almost never casted at scheduled time, why?
                            _events.ScheduleEvent(EVENT_MALADY_OF_THE_MIND, 18000, 0, PHASE_TWO);
                            _events.ScheduleEvent(EVENT_PSYCHOSIS, 1, 0, PHASE_TWO);
                            _events.ScheduleEvent(EVENT_BRAIN_LINK, 23000, 0, PHASE_TWO);
                            break;
                        case EVENT_DEATH_RAY:
                            DoCast(me, SPELL_DEATH_RAY);
                            _events.ScheduleEvent(EVENT_DEATH_RAY, 21000, 0, PHASE_TWO);
                            break;
                        case EVENT_MALADY_OF_THE_MIND:
                            me->CastCustomSpell(SPELL_MALADY_OF_THE_MIND, SPELLVALUE_MAX_TARGETS, 1);
                            _events.ScheduleEvent(EVENT_MALADY_OF_THE_MIND, urand(18000, 25000), 0, PHASE_TWO);
                            break;
                        case EVENT_PSYCHOSIS:
                            me->CastCustomSpell(SPELL_PSYCHOSIS, SPELLVALUE_MAX_TARGETS, 1);
                            _events.ScheduleEvent(EVENT_PSYCHOSIS, urand(4000, 6000), 0, PHASE_TWO);
                            break;
                        case EVENT_BRAIN_LINK:
                            me->CastCustomSpell(SPELL_BRAIN_LINK, SPELLVALUE_MAX_TARGETS, 2);
                            _events.ScheduleEvent(EVENT_BRAIN_LINK, urand(23000, 26000), 0, PHASE_TWO);
                            break;
                        default:
                            break;
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetReactState(REACT_PASSIVE);

                switch (summon->GetEntry())
                {
                    case NPC_DEATH_ORB:
                        Talk(SAY_SARA_DEATH_RAY);
                        summon->SetReactState(REACT_PASSIVE);
                        summon->CastSpell(summon, SPELL_DEATH_RAY_ORIGIN_VISUAL);
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            Position pos;
                            float radius = frand(25.0f, 50.0f);
                            float angle = frand(0.0f, 2.0f * M_PI);
                            pos.m_positionX = YoggSaronSpawnPos.GetPositionX() + radius * cosf(angle);
                            pos.m_positionY = YoggSaronSpawnPos.GetPositionY() + radius * sinf(angle);
                            pos.m_positionZ = me->GetMap()->GetHeight(me->GetPhaseMask(), pos.GetPositionX(), pos.GetPositionY(), YoggSaronSpawnPos.GetPositionZ() + 5.0f);
                            me->SummonCreature(NPC_DEATH_RAY, pos, TEMPSUMMON_TIMED_DESPAWN, 20000ms);
                        }
                        break;
                    case NPC_DEATH_RAY:
                        summon->SetReactState(REACT_PASSIVE);
                        summon->CastSpell(summon, SPELL_DEATH_RAY_WARNING_VISUAL);
                        break;
                }

                if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                    voice->AI()->JustSummoned(summon);
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_PHASE_THREE:    // Sara does nothing in phase 3
                        _events.SetPhase(PHASE_THREE);
                        me->SetVisible(false);
                        break;
                    default:
                        break;
                }
            }

            private:
                EventMap _events;
                InstanceScript* instance;
                std::map<ObjectGuid, ObjectGuid> _linkData;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_saraAI>(creature);
        }
};

class boss_yogg_saron : public CreatureScript
{
    public:
        boss_yogg_saron() : CreatureScript("boss_yogg_saron") { }

        struct boss_yogg_saronAI : public PassiveAI
        {
            boss_yogg_saronAI(Creature* creature) : PassiveAI(creature), instance(creature->GetInstanceScript()) { }

            void Reset() override
            {
                _events.Reset();
                _events.SetPhase(PHASE_TWO);
                _events.ScheduleEvent(EVENT_YELL_BOW_DOWN, 3000, 0, PHASE_TWO);
                DoCast(me, SPELL_SHADOWY_BARRIER_YOGG);
                DoCast(me, SPELL_KNOCK_AWAY);

                // Restore by default
                me->GetMap()->SetWorldState(WORLDSTATE_THREE_LIGHTS_IN_DARKNESS, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_TWO_LIGHTS_IN_DARKNESS, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_ONE_LIGHT_IN_DARKNESS, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_ALONE_IN_DARKNESS, 0);

                me->ResetLootMode();
                switch (instance->GetData(DATA_KEEPERS_COUNT))
                {
                    case 0:
                        me->AddLootMode(LOOT_ALWAYS | LOOT_HARD | LOOT_BONUS);
                        break;
                    case 1:
                        me->AddLootMode(LOOT_ALWAYS | LOOT_HARD);
                        break;
                    case 2:
                    case 3:
                        me->AddLootMode(LOOT_ALWAYS | LOOT_EASY);
                        break;
                }

                if (instance->GetData(DATA_KEEPERS_COUNT) <= 3)
                    me->GetMap()->SetWorldState(WORLDSTATE_THREE_LIGHTS_IN_DARKNESS, 1);

                if (instance->GetData(DATA_KEEPERS_COUNT) <= 2)
                    me->GetMap()->SetWorldState(WORLDSTATE_TWO_LIGHTS_IN_DARKNESS, 1);

                if (instance->GetData(DATA_KEEPERS_COUNT) <= 1)
                    me->GetMap()->SetWorldState(WORLDSTATE_ONE_LIGHT_IN_DARKNESS, 1);

                if (!instance->GetData(DATA_KEEPERS_COUNT))
                    me->GetMap()->SetWorldState(WORLDSTATE_ALONE_IN_DARKNESS, 1);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_IN_THE_MAWS_OF_THE_OLD_GOD)
                    me->AddLootMode(LOOT_QUEST);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_YOGG_SARON_DEATH);

                if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                    me->Kill(creature);

                for (uint8 i = DATA_SARA; i <= DATA_BRAIN_OF_YOGG_SARON; ++i)
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(i)))
                        creature->DisappearAndDie();

                for (uint8 i = DATA_FREYA_YS; i <= DATA_MIMIRON_YS; ++i)
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetGuidData(i)))
                        creature->AI()->EnterEvadeMode();

                for (auto&& itr : me->GetMap()->GetPlayers())
                    if (Player* player = itr.GetSource())
                    {
                        player->RemoveAurasDueToSpell(SPELL_SANITY);
                        player->RemoveAurasDueToSpell(SPELL_INSANE);
                    }
            }

            void UpdateAI(uint32 diff) override
            {
                me->SetSwim(true); // Needed for correct death animation. Will not spam broadcast packets.

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_YELL_BOW_DOWN:
                            Talk(SAY_YOGG_SARON_SPAWN);
                            break;
                        case EVENT_SHADOW_BEACON:
                            me->CastCustomSpell(SPELL_SHADOW_BEACON, SPELLVALUE_MAX_TARGETS, /*me->GetMap()->Is25ManRaid() ? 5 :*/ 3, me, false); // 3 targets in any mode after 3.2.0
                            Talk(EMOTE_YOGG_SARON_EMPOWERING_SHADOWS);
                            _events.ScheduleEvent(EVENT_SHADOW_BEACON, 45000, 0, PHASE_THREE);
                            break;
                        case EVENT_LUNATIC_GAZE:
                            DoCast(me, SPELL_LUNATIC_GAZE);
                            sCreatureTextMgr->SendSound(me, SOUND_LUNATIC_GAZE, CHAT_MSG_MONSTER_YELL, 0, TEXT_RANGE_NORMAL, TEAM_OTHER, false);
                            _events.ScheduleEvent(EVENT_LUNATIC_GAZE, 12000, 0, PHASE_THREE);
                            break;
                        case EVENT_DEAFENING_ROAR:
                            DoCastAOE(SPELL_DEAFENING_ROAR);
                            Talk(SAY_YOGG_SARON_DEAFENING_ROAR);
                            Talk(EMOTE_YOGG_SARON_DEAFENING_ROAR);
                            _events.ScheduleEvent(EVENT_DEAFENING_ROAR, 60000, 0, PHASE_THREE);
                            break;
                        default:
                            break;
                    }
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_PHASE_THREE:
                        _events.SetPhase(PHASE_THREE);
                        _events.ScheduleEvent(EVENT_SHADOW_BEACON, 46000, 0, PHASE_THREE);
                        _events.ScheduleEvent(EVENT_LUNATIC_GAZE, 12000, 0, PHASE_THREE);
                        if (me->GetMap()->Is25ManRaid() && instance->GetData(DATA_KEEPERS_COUNT) < 4)
                            _events.ScheduleEvent(EVENT_DEAFENING_ROAR, 30000, 0, PHASE_THREE);
                        Talk(SAY_YOGG_SARON_PHASE_3);
                        DoCast(me, SPELL_PHASE_3_TRANSFORM);
                        me->RemoveAurasDueToSpell(SPELL_SHADOWY_BARRIER_YOGG);
                        break;
                    default:
                        break;
                }
            }

            bool IsPassived() { return false; }

        private:
            EventMap _events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_yogg_saronAI>(creature);
        }
};

class boss_brain_of_yogg_saron : public CreatureScript
{
    public:
        boss_brain_of_yogg_saron() : CreatureScript("boss_brain_of_yogg_saron") { }

        struct boss_brain_of_yogg_saronAI : public PassiveAI
        {
            boss_brain_of_yogg_saronAI(Creature* creature) : PassiveAI(creature), instance(creature->GetInstanceScript()), _summons(creature) { }

            void Reset() override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                DoCast(me, SPELL_MATCH_HEALTH);
                _summons.DespawnAll();
                _playersNearBrainTimer = 0;
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (me->HealthBelowPctDamaged(30, damage) && !me->HasAura(SPELL_BRAIN_HURT_VISUAL))
                {
                    _playersNearBrainTimer = 0;

                    me->RemoveAllAuras();
                    me->InterruptNonMeleeSpells(true);
                    DoCastAOE(SPELL_SHATTERED_ILLUSION_REMOVE, true);
                    DoCast(me, SPELL_MATCH_HEALTH_2, true); // it doesn't seem to hit Yogg-Saron here
                    DoCast(me, SPELL_BRAIN_HURT_VISUAL, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);

                    if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                        voice->AI()->DoAction(ACTION_PHASE_THREE);
                    if (Creature* sara = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SARA)))
                        sara->AI()->DoAction(ACTION_PHASE_THREE);
                    if (Creature* yogg = ObjectAccessor::GetCreature(*me, instance->GetGuidData(BOSS_YOGG_SARON)))
                        yogg->AI()->DoAction(ACTION_PHASE_THREE);

                    for (uint8 i = DATA_THORIM_YS; i <= DATA_MIMIRON_YS; ++i)
                        if (Creature* keeper = ObjectAccessor::GetCreature(*me, instance->GetGuidData(i)))
                            keeper->AI()->DoAction(ACTION_PHASE_THREE);

                    std::list<Creature*> descent_into_madness;
                    GetCreatureListWithEntryInGrid(descent_into_madness, me, 34072, 150);
                    for (auto&& desc : descent_into_madness)
                        if (desc)
                            desc->DespawnOrUnsummon();
                }
            }

            SpellMissInfo SpellHitResult(Unit* attacker, SpellInfo const* spell, Spell const* /*spellInstance*/) override
            {
                return attacker != me && _playersNearBrainTimer ? SPELL_MISS_IMMUNE : SPELL_MISS_NONE;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat() || !_playersNearBrainTimer)
                    return;

                if (_playersNearBrainTimer <= diff)
                {
                    static Position const center = { 1984.88f, -31.4655f, 261.69f };

                    _playersNearBrainTimer = 2000;
                    Map::PlayerList const& players = me->GetMap()->GetPlayers();
                    for (auto itr = players.begin(); itr != players.end(); ++itr)
                        if (Player* player = itr->GetSource())
                            if (player->GetPositionZ() < 300.0f && player->GetExactDist2d(&center) <= 50.0f)
                                me->Kill(player);
                }
                else
                    _playersNearBrainTimer -= diff;
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_INDUCE_MADNESS:
                    {
                        _tentaclesKilled = 0;
                        _playersNearBrainTimer = 2000;

                        if (instance->GetData(DATA_ILLUSION) == CHAMBER_ILLUSION)
                        {
                            me->SummonCreature(33523, ChamberIllusionNeltharion);
                            me->SummonCreature(33535, ChamberIllusionMalygos);
                            me->SummonCreature(33536, ChamberIllusionAlextrasza);
                            me->SummonCreature(33495, ChamberIllusionYsera);
                            for (uint8 i = 0; i < 2; ++i)
                                me->SummonCreature(33720, ChamberIllusionObsidianConsort[i]);
                            for (uint8 i = 0; i < 2; ++i)
                                me->SummonCreature(33717, ChamberIllusionAzureConsort[i]);
                            for (uint8 i = 0; i < 2; ++i)
                                me->SummonCreature(33719, ChamberIllusionEmeraldConsort[i]);
                            for (uint8 i = 0; i < 2; ++i)
                                me->SummonCreature(33716, ChamberIllusionRubyConsort[i]);
                            me->SummonCreature(33552, ChamberIllusionYoggIllusion);
                            for (uint8 i = 0; i < 7; ++i)
                                me->SummonCreature(33990, ChamberIllusionLaughingSkull[i]);
                        }
                        else if (instance->GetData(DATA_ILLUSION) == ICECROWN_ILLUSION)
                        {
                            me->SummonCreature(33441, IcecrownIllusionLichKing);
                            me->SummonCreature(33442, IcecrownIllusionImmolatedChampion);
                            me->SummonCreature(33962, IcecrownIllusionTurnedChampion);
                            for (uint8 i = 0; i < 9; ++i)
                                me->SummonCreature(33567, IcecrownIllusionDeathswornZealot[i]);
                            me->SummonCreature(33552, IcecrownIllusionYoggIllusion);
                            for (uint8 i = 0; i < 8; ++i)
                                me->SummonCreature(33990, IcecrownIllusionLaughingSkull[i]);
                        }
                        else if (instance->GetData(DATA_ILLUSION) == STORMWIND_ILLUSION)
                        {
                            me->SummonCreature(33436, StormwindIllusionGarona);
                            me->SummonCreature(33437, StormwindIllusionKingLlane);
                            for (uint8 i = 0; i < 8; ++i)
                                me->SummonCreature(33433, StormwindIllusionSuitArmor[i]);
                            me->SummonCreature(33552, StormwindIllusionYoggIllusion);
                            for (uint8 i = 0; i < 6; ++i)
                                me->SummonCreature(33990, StormwindIllusionLaughingSkull[i]);
                        }

                        // make sure doors won't be opened
                        for (uint32 i = GO_BRAIN_ROOM_DOOR_1; i <= GO_BRAIN_ROOM_DOOR_3; ++i)
                            instance->HandleGameObject(instance->GetGuidData(i), false);

                        DoCastAOE(SPELL_INDUCE_MADNESS);
                        break;
                    }
                    case ACTION_TENTACLE_KILLED:
                    {
                        uint8 illusion = instance->GetData(DATA_ILLUSION);
                        if (++_tentaclesKilled >= (illusion == ICECROWN_ILLUSION ? 9 : 8))
                        {
                            _playersNearBrainTimer = 0;

                            sCreatureTextMgr->SendChat(me, EMOTE_BRAIN_ILLUSION_SHATTERED, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_AREA);
                            _summons.DespawnAll();
                            DoCastAOE(SPELL_SHATTERED_ILLUSION, true);
                            instance->HandleGameObject(instance->GetGuidData(GO_BRAIN_ROOM_DOOR_1 + illusion), true);

                            if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                                voice->AI()->DoAction(ACTION_TOGGLE_SHATTERED_ILLUSION);
                        }
                        break;
                    }
                    case ACTION_BRAIN_INDUCED_MADNESS:
                        _summons.DespawnAll();
                        break;
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* summon) override
            {
                _summons.Summon(summon);
            }

            bool IsPassived() { return false; }

        private:
            InstanceScript* instance;
            SummonList _summons;
            uint8 _tentaclesKilled;
            uint32 _playersNearBrainTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_brain_of_yogg_saronAI>(creature);
        }
};

class npc_ominous_cloud : public CreatureScript
{
    public:
        npc_ominous_cloud() : CreatureScript("npc_ominous_cloud") { }

        struct npc_ominous_cloudAI : public PassiveAI
        {
            npc_ominous_cloudAI(Creature* creature) : PassiveAI(creature) { }

            void Reset() override
            {
                DoCast(me, SPELL_OMINOUS_CLOUD_VISUAL);
                me->RemoveAurasDueToSpell(SPELL_SUMMON_GUARDIAN_1);
            }

            void MoveInLineOfSight(Unit* target) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (instance && instance->GetBossState(BOSS_YOGG_SARON) == IN_PROGRESS)
                        if (target && me->GetDistance2d(target) <= 6 && target->ToPlayer() && !target->ToPlayer()->IsGameMaster() && !target->HasAura(SPELL_FLASH_FREEZE))
                            TriggerGuardianSpawn();
            }

            void DoAction(int32 action) override
            {
                switch(action)
                {
                    case true:
                    case false:
                        me->GetMotionMaster()->MoveCirclePath(YoggSaronSpawnPos.GetPositionX(), YoggSaronSpawnPos.GetPositionY(), me->GetPositionZ() + 2, me->GetExactDist2d(YoggSaronSpawnPos.GetPositionX(), YoggSaronSpawnPos.GetPositionY()), action, 16);
                        break;
                    case ACTION_DEACTIVATE_CLOUDS:
                        me->RemoveAurasDueToSpell(SPELL_SUMMON_GUARDIAN_1);
                        me->SetReactState(REACT_PASSIVE);
                        break;
                    case ACTION_ACTIVATE_CLOUDS:
                        me->SetReactState(REACT_AGGRESSIVE);
                        break;
                }
            }

            void TriggerGuardianSpawn()
            {
                if (!me->HasAura(SPELL_SUMMON_GUARDIAN_1))
                    DoCast(me, SPELL_SUMMON_GUARDIAN_1, true);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ominous_cloudAI>(creature);
        }
};

class npc_guardian_of_yogg_saron : public CreatureScript
{
    public:
        npc_guardian_of_yogg_saron() : CreatureScript("npc_guardian_of_yogg_saron") { }

        struct npc_guardian_of_yogg_saronAI : public ScriptedAI
        {
            npc_guardian_of_yogg_saronAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetInstanceScript())
                    me->GetInstanceScript()->DoStartCriteria(CRITERIA_START_TYPE_CREATURE, me->GetEntry());

                DoCastAOE(SPELL_SHADOW_NOVA);
                DoCastAOE(SPELL_SHADOW_NOVA_2);
                me->DespawnOrUnsummon(10000);
            }

            void Reset() override
            {
                _events.ScheduleEvent(EVENT_DARK_VOLLEY, urand(10000, 15000));
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                me->LowerPlayerDamageReq(me->GetMaxHealth());

                if (!me->HasLootRecipient())
                    if (Player* player = me->FindNearestPlayer(200.0f))
                        me->SetLootRecipient(player);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DARK_VOLLEY:
                            DoCastAOE(SPELL_DARK_VOLLEY);
                            _events.ScheduleEvent(EVENT_DARK_VOLLEY, urand(10000, 15000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (summoner->GetEntry() != NPC_OMINOUS_CLOUD)
                    return;

                // Guardian can be summoned both by Voice of Yogg-Saron and by Ominous Cloud
                if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                    voice->AI()->JustSummoned(me);
            }

        private:
            EventMap _events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_guardian_of_yogg_saronAI>(creature);
        }
};

class npc_corruptor_tentacle : public CreatureScript
{
    public:
        npc_corruptor_tentacle() : CreatureScript("npc_corruptor_tentacle") { }

        struct npc_corruptor_tentacleAI : public ScriptedAI
        {
            npc_corruptor_tentacleAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                DoCast(me, SPELL_TENTACLE_VOID_ZONE);
                DoCastAOE(SPELL_ERUPT);
                _events.ScheduleEvent(EVENT_CAST_RANDOM_SPELL, 1);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_TENTACLE_VOID_ZONE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->DespawnOrUnsummon(60000);
            }

            void PassengerBoarded(Unit* passenger, int8 seat, bool apply) override
            {
                if (apply && passenger)
                {
                    passenger->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
                    passenger->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasAura(SPELL_SHATTERED_ILLUSION))
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CAST_RANDOM_SPELL:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target, RAND(SPELL_BLACK_PLAGUE, SPELL_CURSE_OF_DOOM, SPELL_APATHY, SPELL_DRAINING_POISON));
                            _events.ScheduleEvent(EVENT_CAST_RANDOM_SPELL, 3000);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_corruptor_tentacleAI>(creature);
        }
};

class npc_constrictor_tentacle : public CreatureScript
{
    public:
        npc_constrictor_tentacle() : CreatureScript("npc_constrictor_tentacle") { }

        struct npc_constrictor_tentacleAI : public ScriptedAI
        {
            npc_constrictor_tentacleAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript())
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                DoCast(me, SPELL_TENTACLE_VOID_ZONE_2);
                DoCastAOE(SPELL_ERUPT);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_TENTACLE_VOID_ZONE_2);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->DespawnOrUnsummon(5000);
            }

            void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
            {
                if (!apply)
                    passenger->RemoveAurasDueToSpell(Is25ManRaid() ? SPELL_SQUEEZE_25 : SPELL_SQUEEZE);
                if (apply && passenger)
                {
                    passenger->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
                    passenger->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                }
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                UpdateVictim();
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (Creature* voice = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                    voice->AI()->JustSummoned(me);
            }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_constrictor_tentacleAI>(creature);
        }
};

class npc_crusher_tentacle : public CreatureScript
{
    public:
        npc_crusher_tentacle() : CreatureScript("npc_crusher_tentacle") { }

        struct npc_crusher_tentacleAI : public ScriptedAI
        {
            npc_crusher_tentacleAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            void Reset() override
            {
                _targetSearchTimer = 1000;

                DoCast(me, SPELL_CRUSH);
                DoCast(me, SPELL_TENTACLE_VOID_ZONE);
                DoCast(me, SPELL_DIMINSH_POWER);
                DoCast(me, SPELL_FOCUSED_ANGER);
                DoCastAOE(SPELL_ERUPT);

                _events.ScheduleEvent(EVENT_DIMINISH_POWER, urand(6000, 8000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->RemoveAurasDueToSpell(SPELL_TENTACLE_VOID_ZONE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->DespawnOrUnsummon(5000);
            }

            void PassengerBoarded(Unit* passenger, int8 seat, bool apply) override
            {
                if (apply && passenger)
                {
                    passenger->ClearUnitState(UNIT_STATE_FOLLOW_FORMATION);
                    passenger->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_SHATTERED_ILLUSION))
                    return;

                // update timers when the Diminish Power is not being channeled so the next one
                // is not cast immediately after interrupt
                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DIMINISH_POWER:
                            if (!me->GetVictim() || !me->IsWithinMeleeRange(me->GetVictim()))
                                DoCast(SPELL_DIMINISH_POWER);
                            _events.ScheduleEvent(EVENT_DIMINISH_POWER, 2000);
                            break;
                        default:
                            break;
                    }
                }

                if (me->GetVictim() && !me->IsWithinMeleeRange(me->GetVictim()))
                {
                    if (_targetSearchTimer <= diff)
                    {
                        _targetSearchTimer = 1000;
                        if (Unit* newVictim = me->SelectNearestTarget(20.0f))
                        {
                            if (newVictim != me->GetVictim())
                            {
                                float threat = me->GetThreatManager().getThreat(me->GetVictim());
                                me->GetThreatManager().modifyThreatPercent(me->GetVictim(), -100);
                                me->AddThreat(newVictim, threat);
                                AttackStartNoMove(newVictim);
                            }
                        }
                    }
                    else
                        _targetSearchTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            uint32 _targetSearchTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crusher_tentacleAI>(creature);
        }
};

class npc_influence_tentacle : public CreatureScript
{
    public:
        npc_influence_tentacle() : CreatureScript("npc_influence_tentacle") { }

        struct npc_influence_tentacleAI : public PassiveAI
        {
            npc_influence_tentacleAI(Creature* creature) : PassiveAI(creature), instance(creature->GetInstanceScript()) { }

            void Reset() override
            {
                DoCast(me, me->GetEntry() == NPC_SUIT_OF_ARMOR ? SPELL_NONDESCRIPT_1 : SPELL_NONDESCRIPT_2, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* brain = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BRAIN_OF_YOGG_SARON)))
                    brain->AI()->DoAction(ACTION_TENTACLE_KILLED);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->DespawnOrUnsummon(5000);
            }

            void UpdateAI(uint32 /*diff*/) override { }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_influence_tentacleAI>(creature);
        }
};

typedef boss_sara::boss_saraAI SaraAI;

class npc_descend_into_madness : public CreatureScript
{
    public:
        npc_descend_into_madness() : CreatureScript("npc_descend_into_madness") { }

        struct npc_descend_into_madnessAI : public PassiveAI
        {
            npc_descend_into_madnessAI(Creature* creature) : PassiveAI(creature), instance(creature->GetInstanceScript())
            {
                me->DespawnOrUnsummon(30000);
            }

            void OnSpellClick(Unit* clicker, bool& result) override
            {
                if (!result)
                    return;

                uint32 spellId;
                switch (instance->GetData(DATA_ILLUSION))
                {
                    case CHAMBER_ILLUSION: spellId = 63997; break;
                    case ICECROWN_ILLUSION: spellId = 63998; break;
                    case STORMWIND_ILLUSION: spellId = 63989; break;
                    default: return;
                }
                clicker->RemoveAurasDueToSpell(SPELL_BRAIN_LINK);
                DoCast(clicker, spellId, true);
                me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 /*diff*/) override { }

        private:
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_descend_into_madnessAI>(creature);
        }
};

class npc_immortal_guardian : public CreatureScript
{
    public:
        npc_immortal_guardian() : CreatureScript("npc_immortal_guardian") { }

        struct npc_immortal_guardianAI : public ScriptedAI
        {
            npc_immortal_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoCast(me, SPELL_EMPOWERED);
                DoCast(me, SPELL_RECENTLY_SPAWNED);
                _events.ScheduleEvent(EVENT_DRAIN_LIFE, urand(3000, 13000));
            }

            void DamageTaken(Unit* /*who*/, uint32& damage) override
            {
                if (me->HealthBelowPctDamaged(1, damage))
                    damage = me->GetHealth() - me->CountPctFromMaxHealth(1);   // or set immune to damage? should be done here or in SPELL_WEAKENED spell script?
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DRAIN_LIFE:
                            DoCast(SPELL_DRAIN_LIFE);
                            _events.ScheduleEvent(EVENT_DRAIN_LIFE, urand(20000, 30000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_immortal_guardianAI>(creature);
        }
};

class npc_observation_ring_keeper : public CreatureScript
{
    public:
        npc_observation_ring_keeper() : CreatureScript("npc_observation_ring_keeper") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (!player || !creature)
                return false;

            if (Group* group = player->GetGroup())
            {
                if (player->GetGUID() == group->GetLeaderGUID())
                    return false;
            }
            else
                return false;

            player->PlayerTalkClass->GetGossipMenu().SetMenuId(0);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU ? "Я не лидер рейда..." : "I'm not the raid leader...", 0, 0);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_observation_ring_keeperAI : public ScriptedAI
        {
            npc_observation_ring_keeperAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoCast(SPELL_SIMPLE_TELEPORT_KEEPERS);  // not visible here
                DoCast(SPELL_KEEPER_ACTIVE);
            }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 /*action*/) override
            {
                if (sender != 10333)
                    return false;

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->DespawnOrUnsummon(2000);
                DoCast(SPELL_TELEPORT);
                Talk(SAY_KEEPER_CHOSEN_1, player);
                Talk(SAY_KEEPER_CHOSEN_2, player);

                switch (me->GetEntry())
                {
                    case NPC_FREYA_OBSERVATION_RING:
                        me->SummonCreature(NPC_FREYA_YS, YSKeepersPos[0]);
                        break;
                    case NPC_HODIR_OBSERVATION_RING:
                        me->SummonCreature(NPC_HODIR_YS, YSKeepersPos[1]);
                        break;
                    case NPC_THORIM_OBSERVATION_RING:
                        me->SummonCreature(NPC_THORIM_YS, YSKeepersPos[2]);
                        break;
                    case NPC_MIMIRON_OBSERVATION_RING:
                        me->SummonCreature(NPC_MIMIRON_YS, YSKeepersPos[3]);
                        break;
                }
                return true;
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_observation_ring_keeperAI>(creature);
        }
};

class npc_yogg_saron_keeper : public CreatureScript
{
    public:
        npc_yogg_saron_keeper() : CreatureScript("npc_yogg_saron_keeper") { }

        struct npc_yogg_saron_keeperAI : public ScriptedAI
        {
            npc_yogg_saron_keeperAI(Creature* creature) : ScriptedAI(creature) { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                DoCast(SPELL_SIMPLE_TELEPORT_KEEPERS);
            }

            void Reset() override
            {
                _events.Reset();
                _events.SetPhase(PHASE_ONE);
                me->SetReactState(REACT_PASSIVE);
                me->RemoveAllAuras();

                DoCast(SPELL_KEEPER_ACTIVE);    // can we skip removing this aura somehow?

                if (me->GetEntry() == NPC_FREYA_YS)
                {
                    std::list<Creature*> wells;
                    GetCreatureListWithEntryInGrid(wells, me, NPC_SANITY_WELL, 200.0f);
                    for (auto&& well : wells)
                    {
                        well->RemoveAurasDueToSpell(SPELL_SANITY_WELL);
                        well->RemoveAurasDueToSpell(SPELL_SANITY_WELL_VISUAL);
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                switch (me->GetEntry())
                {
                    case NPC_FREYA_YS:
                        DoCast(SPELL_RESILIENCE_OF_NATURE);
                        DoCast(SPELL_SANITY_WELL_SUMMON);
                        break;
                    case NPC_HODIR_YS:
                        DoCast(SPELL_FORTITUDE_OF_FROST);
                        DoCast(SPELL_HODIRS_PROTECTIVE_GAZE);
                        break;
                    case NPC_THORIM_YS:
                        DoCast(SPELL_FURY_OF_THE_STORM);
                        break;
                    case NPC_MIMIRON_YS:
                        DoCast(SPELL_SPEED_OF_INVENTION);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->IsInCombat())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DESTABILIZATION_MATRIX:
                            me->CastCustomSpell(SPELL_DESTABILIZATION_MATRIX, SPELLVALUE_MAX_TARGETS, 1);
                            _events.ScheduleEvent(EVENT_DESTABILIZATION_MATRIX, urand(15000, 25000), 0, PHASE_TWO);
                            break;
                        case EVENT_HODIRS_PROTECTIVE_GAZE:
                            DoCast(SPELL_HODIRS_PROTECTIVE_GAZE);
                            break;
                    }
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    // setting the phases is only for Thorim and Mimiron
                    case ACTION_PHASE_TWO:
                        _events.SetPhase(PHASE_TWO);
                        _events.ScheduleEvent(EVENT_DESTABILIZATION_MATRIX, urand(5000, 15000), 0, PHASE_TWO);
                        break;
                    case ACTION_PHASE_THREE:
                        _events.SetPhase(PHASE_THREE);
                        if (me->GetEntry() == NPC_THORIM_YS)
                            DoCast(SPELL_TITANIC_STORM);
                        break;
                    case ACTION_SANITY_WELLS:
                    {
                        std::list<Creature*> wells;
                        GetCreatureListWithEntryInGrid(wells, me, NPC_SANITY_WELL, 200.0f);
                        for (auto&& well : wells)
                        {
                            well->CastSpell(well, SPELL_SANITY_WELL);
                            well->CastSpell(well, SPELL_SANITY_WELL_VISUAL);
                        }
                        break;
                    }
                    case ACTION_FLASH_FREEZE:
                        DoCast(SPELL_FLASH_FREEZE_VISUAL);
                        _events.ScheduleEvent(EVENT_HODIRS_PROTECTIVE_GAZE, urand(25000, 30000));
                        break;
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_yogg_saron_keeperAI>(creature);
        }
};

class npc_yogg_saron_illusions : public CreatureScript
{
    public:
        npc_yogg_saron_illusions() : CreatureScript("npc_yogg_saron_illusions") { }

        struct npc_yogg_saron_illusionsAI : public ScriptedAI
        {
            npc_yogg_saron_illusionsAI(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                switch (instance->GetData(DATA_ILLUSION))
                {
                    case CHAMBER_ILLUSION:
                        // i think the first Talk should be delayed as in this moment
                        // players are too far away to be able to see it
                        if (Creature* neltharion = me->FindNearestCreature(NPC_NELTHARION, 50.0f))
                            neltharion->AI()->Talk(SAY_CHAMBER_ROLEPLAY_1);

                        _events.ScheduleEvent(EVENT_CHAMBER_ROLEPLAY_1, 16000);
                        _events.ScheduleEvent(EVENT_CHAMBER_ROLEPLAY_2, 22000);
                        _events.ScheduleEvent(EVENT_CHAMBER_ROLEPLAY_3, 28000);
                        _events.ScheduleEvent(EVENT_CHAMBER_ROLEPLAY_4, 36000);
                        break;
                    case ICECROWN_ILLUSION:
                        // same here
                        _events.ScheduleEvent(EVENT_ICECROWN_ROLEPLAY_1, 1000);
                        _events.ScheduleEvent(EVENT_ICECROWN_ROLEPLAY_2, 7500);
                        _events.ScheduleEvent(EVENT_ICECROWN_ROLEPLAY_3, 19500);
                        _events.ScheduleEvent(EVENT_ICECROWN_ROLEPLAY_4, 25500);
                        _events.ScheduleEvent(EVENT_ICECROWN_ROLEPLAY_5, 33000);
                        _events.ScheduleEvent(EVENT_ICECROWN_ROLEPLAY_6, 41300);
                        break;
                    case STORMWIND_ILLUSION:
                        _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_4, 33800); // "A thousand deaths..."
                        _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_5, 38850);
                        _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_7, 58750);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHAMBER_ROLEPLAY_1:
                            if (Creature* ysera = me->FindNearestCreature(NPC_YSERA, 50.0f))
                                ysera->AI()->Talk(SAY_CHAMBER_ROLEPLAY_2);
                            break;
                        case EVENT_CHAMBER_ROLEPLAY_2:
                            if (Creature* neltharion = me->FindNearestCreature(NPC_NELTHARION, 50.0f))
                                neltharion->AI()->Talk(SAY_CHAMBER_ROLEPLAY_3);
                            break;
                        case EVENT_CHAMBER_ROLEPLAY_3:
                            if (Creature* malygos = me->FindNearestCreature(NPC_MALYGOS, 50.0f))
                                malygos->AI()->Talk(SAY_CHAMBER_ROLEPLAY_4);
                            break;
                        case EVENT_CHAMBER_ROLEPLAY_4:
                            Talk(SAY_CHAMBER_ROLEPLAY_5);
                            break;
                        case EVENT_ICECROWN_ROLEPLAY_1:
                            if (Creature* bolvar = me->FindNearestCreature(NPC_IMMOLATED_CHAMPION, 50.0f))
                            {
                                bolvar->AI()->Talk(SAY_ICECROWN_ROLEPLAY_1);

                                if (Creature* lichKing = me->FindNearestCreature(NPC_THE_LICH_KING, 50.0f))
                                    lichKing->CastSpell(bolvar, SPELL_DEATHGRASP);
                            }
                            break;
                        case EVENT_ICECROWN_ROLEPLAY_2:
                            if (Creature* lichKing = me->FindNearestCreature(NPC_THE_LICH_KING, 50.0f))
                                lichKing->AI()->Talk(SAY_ICECROWN_ROLEPLAY_2);
                            break;
                        case EVENT_ICECROWN_ROLEPLAY_3:
                            if (Creature* bolvar = me->FindNearestCreature(NPC_IMMOLATED_CHAMPION, 50.0f))
                                bolvar->AI()->Talk(SAY_ICECROWN_ROLEPLAY_3);
                            if (Creature* saurfang = me->FindNearestCreature(NPC_TURNED_CHAMPION, 50.0f))
                                saurfang->AI()->DoAction(ACTION_START_ROLEPLAY);
                            break;
                        case EVENT_ICECROWN_ROLEPLAY_4:
                            if (Creature* lichKing = me->FindNearestCreature(NPC_THE_LICH_KING, 50.0f))
                                lichKing->AI()->Talk(SAY_ICECROWN_ROLEPLAY_4);
                            break;
                        case EVENT_ICECROWN_ROLEPLAY_5:
                            Talk(SAY_ICECROWN_ROLEPLAY_5);
                            break;
                        case EVENT_ICECROWN_ROLEPLAY_6:
                            Talk(SAY_ICECROWN_ROLEPLAY_6);
                            break;
                        case EVENT_STORMWIND_ROLEPLAY_4:
                            Talk(SAY_STORMWIND_ROLEPLAY_4);
                            break;
                        case EVENT_STORMWIND_ROLEPLAY_5:
                            if (Creature* llane = me->FindNearestCreature(NPC_KING_LLANE, 50.0f))
                                llane->AI()->Talk(SAY_STORMWIND_ROLEPLAY_5);
                            break;
                        case EVENT_STORMWIND_ROLEPLAY_7:
                            Talk(SAY_STORMWIND_ROLEPLAY_7);
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_yogg_saron_illusionsAI>(creature);
        }
};

class npc_garona : public CreatureScript
{
    public:
        npc_garona() : CreatureScript("npc_garona") { }

        struct npc_garonaAI : public ScriptedAI
        {
            npc_garonaAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                _events.Reset();

                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, IllusionsMiscPos[0]);

                _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_1, 9250);
                _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_2, 16700);
                _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_3, 24150);
                _events.ScheduleEvent(EVENT_STORMWIND_ROLEPLAY_6, 52700);
            }

            void UpdateAI(uint32 diff) override
            {
                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STORMWIND_ROLEPLAY_1:
                            Talk(SAY_STORMWIND_ROLEPLAY_1);
                            break;
                        case EVENT_STORMWIND_ROLEPLAY_2:
                            Talk(SAY_STORMWIND_ROLEPLAY_2);
                            break;
                        case EVENT_STORMWIND_ROLEPLAY_3:
                            Talk(SAY_STORMWIND_ROLEPLAY_3);
                            break;
                        case EVENT_STORMWIND_ROLEPLAY_6:
                            Talk(SAY_STORMWIND_ROLEPLAY_6);
                            if (Creature* llane = me->FindNearestCreature(NPC_KING_LLANE, 50.0f))
                            {
                                DoCast(SPELL_ASSASSINATE);
                                llane->CastSpell(llane, SPELL_PERMANENT_FEIGN_DEATH);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap _events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_garonaAI>(creature);
        }
};

class npc_turned_champion : public CreatureScript
{
    public:
        npc_turned_champion() : CreatureScript("npc_turned_champion") { }

        struct npc_turned_championAI : public ScriptedAI
        {
            npc_turned_championAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                DoCast(SPELL_VERTEX_COLOR_BLACK);
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type != POINT_MOTION_TYPE || point != 0)
                    return;

                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_START_ROLEPLAY)
                    return;

                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, IllusionsMiscPos[1]);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_turned_championAI>(creature);
        }
};

class npc_laughing_skull : public CreatureScript
{
    public:
        npc_laughing_skull() : CreatureScript("npc_laughing_skull") { }

        struct npc_laughing_skullAI : public ScriptedAI
        {
            npc_laughing_skullAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_LUNATIC_GAZE_SKULL);
            }

            // don't evade, otherwise the Lunatic Gaze aura is removed
            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_laughing_skullAI>(creature);
        }
};

class npc_death_ray : public CreatureScript
{
    public:
        npc_death_ray() : CreatureScript("npc_death_ray") { }

        struct npc_death_rayAI : public ScriptedAI
        {
            npc_death_rayAI(Creature* creature) : ScriptedAI(creature) { }

            void DoAction(int32 /*action*/) override
            {
                me->SetSpeed(MOVE_RUN, 0.5f);
                MoveToNextPos();
            }

            void MovementInform(uint32 type, uint32 /*point*/) override
            {
                if (type = POINT_MOTION_TYPE)
                    MoveToNextPos();
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (moveToNextPos)
                {
                    moveToNextPos = false;
                    me->GetMotionMaster()->MovePoint(0, nextPos);
                }
            }

        private:
            void MoveToNextPos()
            {
                float dist = (float)rand_norm() * 10.0f;
                float angle = (float)rand_norm();
                nextPos = me->GetHomePosition();
                nextPos.m_positionX += dist * cosf(angle);
                nextPos.m_positionY += dist * sinf(angle);
                //me->UpdateAllowedPositionZ(nextPos.GetPositionX(), nextPos.GetPositionY(), nextPos.m_positionZ, 5.0f);
                me->UpdateAllowedPositionZ(nextPos.GetPositionX(), nextPos.GetPositionY(), nextPos.m_positionZ);
                moveToNextPos = true;
            }

            bool moveToNextPos = false;
            Position nextPos;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_death_rayAI>(creature);
        }
};

class spell_yogg_saron_target_selectors : public SpellScriptLoader    // 63744, 63745, 63747, 65206
{
    public:
        spell_yogg_saron_target_selectors() : SpellScriptLoader("spell_yogg_saron_target_selectors") { }

        class spell_yogg_saron_target_selectors_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_target_selectors_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, uint32(GetEffectValue()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_target_selectors_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_target_selectors_SpellScript();
        }
};

class SanityReduction : public SpellScript
{
    public:
        SanityReduction() : SpellScript() { }
        SanityReduction(uint8 stacks) : SpellScript(), _stacks(stacks) { }

    void RemoveSanity(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            if (Aura* sanity = target->GetAura(SPELL_SANITY))
                sanity->ModStackAmount(-int32(_stacks), AURA_REMOVE_BY_ENEMY_SPELL);
    }

    protected:
        uint8 _stacks;
};

class HighSanityTargetSelector
{
    public:
        HighSanityTargetSelector() { }

        bool operator()(WorldObject* object)
        {
            if (Unit* unit = object->ToUnit())
                if (Aura* sanity = unit->GetAura(SPELL_SANITY))
                    return sanity->GetStackAmount() <= 40;
            return true;
        }
};

// 63795, 65301
class spell_yogg_saron_psychosis : public SpellScriptLoader
{
    public:
        spell_yogg_saron_psychosis() : SpellScriptLoader("spell_yogg_saron_psychosis") { }

        class spell_yogg_saron_psychosis_SpellScript : public SanityReduction
        {
            PrepareSpellScript(spell_yogg_saron_psychosis_SpellScript);

            bool Load() override
            {
                _stacks = GetSpellInfo()->Id == SPELL_PSYCHOSIS ? 9 : 12;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(HighSanityTargetSelector());
                targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_ILLUSION_ROOM));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_psychosis_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_psychosis_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_psychosis_SpellScript::RemoveSanity, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_psychosis_SpellScript();
        }
};

// 63830, 63881
class spell_yogg_saron_malady_of_the_mind : public SpellScriptLoader
{
    public:
        spell_yogg_saron_malady_of_the_mind() : SpellScriptLoader("spell_yogg_saron_malady_of_the_mind") { }

        class spell_yogg_saron_malady_of_the_mind_SpellScript : public SanityReduction
        {
            public:
                spell_yogg_saron_malady_of_the_mind_SpellScript() : SanityReduction(3) { }

            PrepareSpellScript(spell_yogg_saron_malady_of_the_mind_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(HighSanityTargetSelector());
                targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_ILLUSION_ROOM));
            }

            void Register() override
            {
                if (m_scriptSpellId == SPELL_MALADY_OF_THE_MIND)
                {
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_malady_of_the_mind_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_malady_of_the_mind_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                    OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_malady_of_the_mind_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
                }

                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_malady_of_the_mind_SpellScript::RemoveSanity, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        class spell_yogg_saron_malady_of_the_mind_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_malady_of_the_mind_AuraScript);

            bool Validate(SpellInfo const* /*spell*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MALADY_OF_THE_MIND_JUMP))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                switch (GetTargetApplication()->GetRemoveMode())
                {
                    case AURA_REMOVE_BY_ENEMY_SPELL:
                    case AURA_REMOVE_BY_EXPIRE:
                    case AURA_REMOVE_BY_DEATH:
                        break;
                    default:
                        return;
                }

                GetTarget()->CastCustomSpell(SPELL_MALADY_OF_THE_MIND_JUMP, SPELLVALUE_MAX_TARGETS, 1);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_malady_of_the_mind_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_MOD_FEAR, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_malady_of_the_mind_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_malady_of_the_mind_AuraScript();
        }
};

// 63802
class spell_yogg_saron_brain_link : public SpellScriptLoader
{
    public:
        spell_yogg_saron_brain_link() : SpellScriptLoader("spell_yogg_saron_brain_link") { }

        class spell_yogg_saron_brain_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_brain_link_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, SPELL_ILLUSION_ROOM));

                if (targets.size() < 2)
                {
                    targets.clear();
                    return;
                }
                else if (targets.size() > 2)
                    Trinity::Containers::RandomResizeList(targets, 2);

                if (SaraAI* ai = CAST_AI(SaraAI, GetCaster()->GetAI()))
                    ai->SetLinkBetween(targets.front()->GetGUID(), targets.back()->GetGUID());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_brain_link_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_yogg_saron_brain_link_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_brain_link_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BRAIN_LINK_DAMAGE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BRAIN_LINK_NO_DAMAGE))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (SaraAI* ai = CAST_AI(SaraAI, caster->GetAI()))
                {
                    if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                        ai->RemoveLinkFrom(GetTarget()->GetGUID());
                    else
                    {
                        if (Player* player = ObjectAccessor::GetPlayer(*GetTarget(), ai->GetLinkedPlayerGUID(GetTarget()->GetGUID())))
                        {
                            ai->RemoveLinkFrom(GetTarget()->GetGUID());
                            player->RemoveAurasDueToSpell(SPELL_BRAIN_LINK);
                        }
                    }
                }
            }

            void DummyTick(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                SaraAI* ai = CAST_AI(SaraAI, caster->GetAI());
                if (!ai)
                    return;

                Player* linked = ObjectAccessor::GetPlayer(*GetTarget(), ai->GetLinkedPlayerGUID(GetTarget()->GetGUID()));
                if (!linked)
                    return;

                GetTarget()->CastSpell(linked, (GetTarget()->GetDistance(linked) > (float)aurEff->GetAmount()) ? SPELL_BRAIN_LINK_DAMAGE : SPELL_BRAIN_LINK_NO_DAMAGE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_yogg_saron_brain_link_AuraScript::DummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                OnEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_brain_link_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_brain_link_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_brain_link_AuraScript();
        }
};

// 63803
class spell_yogg_saron_brain_link_damage : public SpellScriptLoader
{
    public:
        spell_yogg_saron_brain_link_damage() : SpellScriptLoader("spell_yogg_saron_brain_link_damage") { }

        class spell_yogg_saron_brain_link_damage_SpellScript : public SanityReduction
        {
            public:
                spell_yogg_saron_brain_link_damage_SpellScript() : SanityReduction(2) { }

            PrepareSpellScript(spell_yogg_saron_brain_link_damage_SpellScript);

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_brain_link_damage_SpellScript::RemoveSanity, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_brain_link_damage_SpellScript();
        }
};

// 63030
class spell_yogg_saron_boil_ominously : public SpellScriptLoader
{
    public:
        spell_yogg_saron_boil_ominously() : SpellScriptLoader("spell_yogg_saron_boil_ominously") { }

        class spell_yogg_saron_boil_ominously_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_boil_ominously_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_GUARDIAN_1))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    if (!target->HasAura(SPELL_FLASH_FREEZE) && !GetCaster()->HasAura(SPELL_SUMMON_GUARDIAN_1) && !GetCaster()->HasAura(SPELL_SUMMON_GUARDIAN_2))
                    {
                        if (Creature* caster = GetCaster()->ToCreature())
                            caster->AI()->Talk(EMOTE_OMINOUS_CLOUD_PLAYER_TOUCH, target);

                        GetCaster()->CastSpell(GetCaster(), SPELL_SUMMON_GUARDIAN_1, true);
                    }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_boil_ominously_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_boil_ominously_SpellScript();
        }
};

// 64465
class spell_yogg_saron_shadow_beacon : public SpellScriptLoader
{
    public:
        spell_yogg_saron_shadow_beacon() : SpellScriptLoader("spell_yogg_saron_shadow_beacon") { }

        class spell_yogg_saron_shadow_beacon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_shadow_beacon_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* target = GetTarget()->ToCreature())
                    target->SetEntry(NPC_MARKED_IMMORTAL_GUARDIAN);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* target = GetTarget()->ToCreature())
                    target->SetEntry(NPC_IMMORTAL_GUARDIAN);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_yogg_saron_shadow_beacon_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_shadow_beacon_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_shadow_beacon_AuraScript();
        }
};

class spell_yogg_saron_empowering_shadows_range_check : public SpellScriptLoader    // 64466
{
    public:
        spell_yogg_saron_empowering_shadows_range_check() : SpellScriptLoader("spell_yogg_saron_empowering_shadows_range_check") { }

        class spell_yogg_saron_empowering_shadows_range_check_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_empowering_shadows_range_check_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_empowering_shadows_range_check_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_empowering_shadows_range_check_SpellScript();
        }
};

// 64467
class spell_yogg_saron_empowering_shadows_missile : public SpellScriptLoader
{
    public:
        spell_yogg_saron_empowering_shadows_missile() : SpellScriptLoader("spell_yogg_saron_empowering_shadows_missile") { }

        class spell_yogg_saron_empowering_shadows_missile_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_empowering_shadows_missile_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_EMPOWERING_SHADOWS))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell((Unit*)nullptr, SPELL_EMPOWERING_SHADOWS, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_empowering_shadows_missile_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_empowering_shadows_missile_SpellScript();
        }
};

// it works, but is it scripted correctly? why is it aura with 2500ms duration?
// 64132
class spell_yogg_saron_constrictor_tentacle : public SpellScriptLoader
{
    public:
        spell_yogg_saron_constrictor_tentacle() : SpellScriptLoader("spell_yogg_saron_constrictor_tentacle") { }

        class spell_yogg_saron_constrictor_tentacle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_constrictor_tentacle_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (Unit* caster = GetCaster())
                    targets.remove_if(MainRoomTargetCheck(caster, 100));
                targets.remove_if(Trinity::UnitAuraCheck(true, 64125)); // Squeeze
                targets.remove_if(Trinity::UnitAuraCheck(true, 64126)); // Squeeze
                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_constrictor_tentacle_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            struct MainRoomTargetCheck
            {
                MainRoomTargetCheck(Unit* source, float dist) : _source(source), _dist(dist) { }

                bool operator()(WorldObject* unit)
                {
                    if (_source->GetExactDist2d(unit) > _dist)
                        return true;
                    if (unit->GetPositionZ() < 320)
                        return true;
                    return false;
                }

                Unit* _source;
                float _dist;
            };
        };

        class spell_yogg_saron_constrictor_tentacle_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_constrictor_tentacle_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CONSTRICTOR_TENTACLE_SUMMON))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_CONSTRICTOR_TENTACLE_SUMMON);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_yogg_saron_constrictor_tentacle_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_constrictor_tentacle_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_constrictor_tentacle_AuraScript();
        }
};

// 64131
class spell_yogg_saron_lunge : public SpellScriptLoader
{
    public:
        spell_yogg_saron_lunge() : SpellScriptLoader("spell_yogg_saron_lunge") { }

        class spell_yogg_saron_lunge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_lunge_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SQUEEZE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->CastSpell(target, GetCaster()->GetMap()->Is25ManRaid() ? SPELL_SQUEEZE_25 : SPELL_SQUEEZE, true);
                    target->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_lunge_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_lunge_SpellScript();
        }
};

// 64125
class spell_yogg_saron_squeeze : public SpellScriptLoader
{
    public:
        spell_yogg_saron_squeeze() : SpellScriptLoader("spell_yogg_saron_squeeze") { }

        class spell_yogg_saron_squeeze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_squeeze_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* vehicle = GetTarget()->GetVehicleBase())
                    if (vehicle->IsAlive())
                        vehicle->Kill(vehicle); // should tentacle die or just release its target?
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_squeeze_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_squeeze_AuraScript();
        }
};

// 64148
class spell_yogg_saron_diminsh_power : public SpellScriptLoader
{
    public:
        spell_yogg_saron_diminsh_power() : SpellScriptLoader("spell_yogg_saron_diminsh_power") { }

        class spell_yogg_saron_diminsh_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_diminsh_power_AuraScript);

            void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                if (Spell* spell = GetTarget()->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                    if (spell->getState() == SPELL_STATE_CASTING)
                        GetTarget()->InterruptSpell(CURRENT_CHANNELED_SPELL);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_yogg_saron_diminsh_power_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_diminsh_power_AuraScript();
        }
};

// not sure about SPELL_WEAKENED part, where should it be handled?
// 64161
class spell_yogg_saron_empowered : public SpellScriptLoader
{
    public:
        spell_yogg_saron_empowered() : SpellScriptLoader("spell_yogg_saron_empowered") { }

        class spell_yogg_saron_empowered_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_empowered_AuraScript);

            bool Validate(SpellInfo const* spellInfo) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_EMPOWERED_BUFF))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_WEAKENED))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastCustomSpell(SPELL_EMPOWERED_BUFF, SPELLVALUE_AURA_STACK, 9, GetTarget(), TRIGGERED_FULL_MASK);
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();
                float stack = ceil((target->GetHealthPct() / 10) - 1);
                target->RemoveAurasDueToSpell(SPELL_EMPOWERED_BUFF);

                if (stack)
                {
                    target->RemoveAurasDueToSpell(SPELL_WEAKENED);
                    target->CastCustomSpell(SPELL_EMPOWERED_BUFF, SPELLVALUE_AURA_STACK, stack, target, TRIGGERED_FULL_MASK);
                }
                else if (!target->HealthAbovePct(1) && !target->HasAura(SPELL_WEAKENED))
                    target->CastSpell(target, SPELL_WEAKENED, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_yogg_saron_empowered_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_yogg_saron_empowered_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_empowered_AuraScript();
        }
};

// 64069
class spell_yogg_saron_match_health : public SpellScriptLoader
{
    public:
        spell_yogg_saron_match_health() : SpellScriptLoader("spell_yogg_saron_match_health") { }

        class spell_yogg_saron_match_health_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_match_health_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->SetHealth(target->CountPctFromMaxHealth((int32)GetCaster()->GetHealthPct()));
                    ((Creature*)target)->ResetPlayerDamageReq();
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_match_health_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_match_health_SpellScript();
        }
};

// 65238
class spell_yogg_saron_shattered_illusion : public SpellScriptLoader
{
    public:
        spell_yogg_saron_shattered_illusion() : SpellScriptLoader("spell_yogg_saron_shattered_illusion") { }

        class spell_yogg_saron_shattered_illusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_shattered_illusion_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    target->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_shattered_illusion_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_shattered_illusion_SpellScript();
        }
};

// 63882
class spell_yogg_saron_death_ray_warning_visual : public SpellScriptLoader
{
    public:
        spell_yogg_saron_death_ray_warning_visual() : SpellScriptLoader("spell_yogg_saron_death_ray_warning_visual") { }

        class spell_yogg_saron_death_ray_warning_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_death_ray_warning_visual_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DEATH_RAY_PERIODIC))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_DEATH_RAY_DAMAGE_VISUAL))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->CastSpell(caster, SPELL_DEATH_RAY_PERIODIC, true);
                    caster->CastSpell((Unit*)nullptr, SPELL_DEATH_RAY_DAMAGE_VISUAL, true);
                    if (caster->IsAIEnabled)
                        caster->GetAI()->DoAction(0);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_death_ray_warning_visual_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_death_ray_warning_visual_AuraScript();
        }
};

// 63993
class spell_yogg_saron_cancel_illusion_room_aura : public SpellScriptLoader
{
    public:
        spell_yogg_saron_cancel_illusion_room_aura() : SpellScriptLoader("spell_yogg_saron_cancel_illusion_room_aura") { }

        class spell_yogg_saron_cancel_illusion_room_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_cancel_illusion_room_aura_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TELEPORT_BACK_TO_MAIN_ROOM))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->CastSpell(target, SPELL_TELEPORT_BACK_TO_MAIN_ROOM);
                    target->RemoveAurasDueToSpell(uint32(GetEffectValue()));
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_cancel_illusion_room_aura_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_cancel_illusion_room_aura_SpellScript();
        }
};

// 64010, 64013
class spell_yogg_saron_nondescript : public SpellScriptLoader
{
    public:
        spell_yogg_saron_nondescript() : SpellScriptLoader("spell_yogg_saron_nondescript") { }

        class spell_yogg_saron_nondescript_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_nondescript_AuraScript);

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), uint32(aurEff->GetAmount()), true);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_nondescript_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_nondescript_AuraScript();
        }
};

// 64012
class spell_yogg_saron_revealed_tentacle : public SpellScriptLoader
{
    public:
        spell_yogg_saron_revealed_tentacle() : SpellScriptLoader("spell_yogg_saron_revealed_tentacle") { }

        class spell_yogg_saron_revealed_tentacle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_revealed_tentacle_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TENTACLE_VOID_ZONE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_GRIM_REPRISAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Creature* caster = GetCaster()->ToCreature())
                {
                    caster->CastSpell(caster, SPELL_TENTACLE_VOID_ZONE, true);
                    caster->CastSpell(caster, SPELL_GRIM_REPRISAL, true);
                    caster->UpdateEntry(NPC_INFLUENCE_TENTACLE, 0, caster->GetCreatureData());
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_revealed_tentacle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_revealed_tentacle_SpellScript();
        }
};

// 63305
class spell_yogg_saron_grim_reprisal : public SpellScriptLoader
{
    public:
        spell_yogg_saron_grim_reprisal() : SpellScriptLoader("spell_yogg_saron_grim_reprisal") { }

        class spell_yogg_saron_grim_reprisal_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_grim_reprisal_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GRIM_REPRISAL_DAMAGE))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                int32 damage = CalculatePct(int32(eventInfo.GetDamageInfo()->GetDamage()), 60);
                GetTarget()->CastCustomSpell(SPELL_GRIM_REPRISAL_DAMAGE, SPELLVALUE_BASE_POINT0, damage, eventInfo.GetDamageInfo()->GetAttacker(), true, nullptr, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_yogg_saron_grim_reprisal_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_grim_reprisal_AuraScript();
        }
};

// 64059
class spell_yogg_saron_induce_madness : public SpellScriptLoader
{
    public:
        spell_yogg_saron_induce_madness() : SpellScriptLoader("spell_yogg_saron_induce_madness") { }

        class spell_yogg_saron_induce_madness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_induce_madness_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TELEPORT_BACK_TO_MAIN_ROOM))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SHATTERED_ILLUSION_REMOVE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->CastSpell(target, SPELL_TELEPORT_BACK_TO_MAIN_ROOM);
                    target->RemoveAurasDueToSpell(SPELL_SANITY, 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
                    target->RemoveAurasDueToSpell(uint32(GetEffectValue()));
                }
            }

            void ClearShatteredIllusion()
            {
                GetCaster()->CastSpell((Unit*)nullptr, SPELL_SHATTERED_ILLUSION_REMOVE);

                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    if (Creature* voice = ObjectAccessor::GetCreature(*GetCaster(), instance->GetGuidData(DATA_VOICE_OF_YOGG_SARON)))
                        voice->AI()->DoAction(ACTION_TOGGLE_SHATTERED_ILLUSION);

                if (GetCaster()->IsAIEnabled)
                    GetCaster()->GetAI()->DoAction(ACTION_BRAIN_INDUCED_MADNESS);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_induce_madness_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                AfterCast += SpellCastFn(spell_yogg_saron_induce_madness_SpellScript::ClearShatteredIllusion);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_induce_madness_SpellScript();
        }
};

// 63050
class spell_yogg_saron_sanity : public SpellScriptLoader
{
    public:
        spell_yogg_saron_sanity() : SpellScriptLoader("spell_yogg_saron_sanity") { }

        class spell_yogg_saron_sanity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_sanity_SpellScript);

            // don't target players outside of room or handle it in SPELL_INSANE_PERIODIC?

            void ModSanityStacks()
            {
                GetSpell()->SetSpellValue(SPELLVALUE_AURA_STACK, 100);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_yogg_saron_sanity_SpellScript::ModSanityStacks);
            }
        };

        class spell_yogg_saron_sanity_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_sanity_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LOW_SANITY_SCREEN_EFFECT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_INSANE))
                    return false;
                return true;
            }

            void DummyTick(AuraEffect const* /*aurEff*/)
            {
                if (GetTarget()->HasAura(SPELL_SANITY_WELL))
                    ModStackAmount(20);

                if (GetStackAmount() <= 40 && !GetTarget()->HasAura(SPELL_LOW_SANITY_SCREEN_EFFECT))
                    GetTarget()->CastSpell(GetTarget(), SPELL_LOW_SANITY_SCREEN_EFFECT, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
                    instance->instance->SetWorldState(WORLDSTATE_DRIVE_ME_CRAZY, 0);

                GetTarget()->RemoveAurasDueToSpell(SPELL_BRAIN_LINK);

                if (Unit* caster = GetCaster())
                    caster->CastSpell(GetTarget(), SPELL_INSANE, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_yogg_saron_sanity_AuraScript::DummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_sanity_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_sanity_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_sanity_AuraScript();
        }
};

// 63120
class spell_yogg_saron_insane : public SpellScriptLoader
{
    public:
        spell_yogg_saron_insane() : SpellScriptLoader("spell_yogg_saron_insane") { }

        class spell_yogg_saron_insane_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_insane_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_INSANE_VISUAL))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (Creature* yogg = caster->ToCreature())
                        yogg->AI()->Talk(WHISPER_VOICE_INSANE, GetTarget());

                GetTarget()->CastSpell(GetTarget(), SPELL_INSANE_VISUAL, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->IsAlive())
                    GetTarget()->Kill(GetTarget());
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_yogg_saron_insane_AuraScript::OnApply, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_yogg_saron_insane_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_insane_AuraScript();
        }
};

// 64555
class spell_yogg_saron_insane_periodic : public SpellScriptLoader
{
    public:
        spell_yogg_saron_insane_periodic() : SpellScriptLoader("spell_yogg_saron_insane_periodic") { }

        class spell_yogg_saron_insane_periodic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_insane_periodic_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, uint32(GetEffectValue()), true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_insane_periodic_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_insane_periodic_SpellScript();
        }
};

class LunaticGazeTargetSelector
{
    public:
        LunaticGazeTargetSelector(Unit* caster) : _caster(caster) { }

        bool operator()(WorldObject* object)
        {
            return !object->HasInArc(static_cast<float>(M_PI), _caster);
        }

    private:
        Unit* _caster;
};

// 64164, 64168
class spell_yogg_saron_lunatic_gaze : public SpellScriptLoader
{
    public:
        spell_yogg_saron_lunatic_gaze() : SpellScriptLoader("spell_yogg_saron_lunatic_gaze") { }

        class spell_yogg_saron_lunatic_gaze_SpellScript : public SanityReduction
        {
            PrepareSpellScript(spell_yogg_saron_lunatic_gaze_SpellScript);

            bool Load() override
            {
                _stacks = GetSpellInfo()->Id == SPELL_LUNATIC_GAZE_DAMAGE ? 4 : 2;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(LunaticGazeTargetSelector(GetCaster()));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_lunatic_gaze_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_lunatic_gaze_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_lunatic_gaze_SpellScript::RemoveSanity, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_lunatic_gaze_SpellScript();
        }
};

// 62650, 62670, 62671, 62702
class spell_yogg_saron_keeper_aura : public SpellScriptLoader
{
    public:
        spell_yogg_saron_keeper_aura() : SpellScriptLoader("spell_yogg_saron_keeper_aura") { }

        class spell_yogg_saron_keeper_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_keeper_aura_AuraScript);

            bool CanApply(Unit* target)
            {
                if (target->GetTypeId() != TYPEID_PLAYER && target != GetCaster())
                    return false;
                return true;
            }

            void Register() override
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_yogg_saron_keeper_aura_AuraScript::CanApply);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_keeper_aura_AuraScript();
        }
};

// 63984
class spell_yogg_saron_hate_to_zero : public SpellScriptLoader
{
    public:
        spell_yogg_saron_hate_to_zero() : SpellScriptLoader("spell_yogg_saron_hate_to_zero") { }

        class spell_yogg_saron_hate_to_zero_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_hate_to_zero_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    if (target->CanHaveThreatList())
                        target->GetThreatManager().modifyThreatPercent(GetCaster(), -100);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_hate_to_zero_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_hate_to_zero_SpellScript();
        }
};

// 64184
class spell_yogg_saron_in_the_maws_of_the_old_god : public SpellScriptLoader
{
    public:
        spell_yogg_saron_in_the_maws_of_the_old_god() : SpellScriptLoader("spell_yogg_saron_in_the_maws_of_the_old_god") { }

        class spell_yogg_saron_in_the_maws_of_the_old_god_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_in_the_maws_of_the_old_god_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    if (Creature* yogg = ObjectAccessor::GetCreature(*GetCaster(), instance->GetGuidData(BOSS_YOGG_SARON)))
                        if (yogg->FindCurrentSpellBySpellId(SPELL_DEAFENING_ROAR))
                        {
                            if (GetCaster()->GetDistance(yogg) > 20.0f)
                                return SPELL_FAILED_OUT_OF_RANGE;
                            else
                                return SPELL_CAST_OK;
                        }

                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_yogg_saron_in_the_maws_of_the_old_god_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_in_the_maws_of_the_old_god_SpellScript();
        }
};

// 64172
class spell_yogg_saron_titanic_storm : public SpellScriptLoader
{
    public:
        spell_yogg_saron_titanic_storm() : SpellScriptLoader("spell_yogg_saron_titanic_storm") { }

        class spell_yogg_saron_titanic_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_titanic_storm_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->Kill(target);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_yogg_saron_titanic_storm_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_titanic_storm_SpellScript();
        }
};

// 64174
class spell_yogg_saron_hodirs_protective_gaze : public SpellScriptLoader
{
    public:
        spell_yogg_saron_hodirs_protective_gaze() : SpellScriptLoader("spell_yogg_saron_hodirs_protective_gaze") { }

        class spell_yogg_saron_hodirs_protective_gaze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_yogg_saron_hodirs_protective_gaze_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FLASH_FREEZE))
                    return false;
                return true;
            }

            bool CanApply(Unit* target)
            {
                if (target->GetTypeId() != TYPEID_PLAYER && target != GetCaster())
                    return false;
                return true;
            }

            void OnAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                if (dmgInfo.GetDamage() >= GetTarget()->GetHealth())
                {
                    absorbAmount = dmgInfo.GetDamage();
                    // or absorbAmount = dmgInfo.GetDamage() - GetTarget()->GetHealth() + 1
                    GetTarget()->CastSpell(GetTarget(), SPELL_FLASH_FREEZE, true);
                }
                else
                    PreventDefaultAction();
            }

            void Register() override
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_yogg_saron_hodirs_protective_gaze_AuraScript::CanApply);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_yogg_saron_hodirs_protective_gaze_AuraScript::OnAbsorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_yogg_saron_hodirs_protective_gaze_AuraScript();
        }
};

class spell_yogg_saron_summon_tentacle_SpellScript : public SpellScript
{
    PrepareSpellScript(spell_yogg_saron_summon_tentacle_SpellScript);

protected:
    void TargetSelect(SpellDestination& dest)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        SpellImplicitTargetInfo const& targetType = GetSpellInfo()->Effects[EFFECT_0].TargetA;
        float dist = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(caster);
        float angle = targetType.CalcDirectionAngle();
        float objSize = caster->GetObjectSize();

        if (dist == 0)
            dist = objSize;
        else if (targetType.GetTarget() == TARGET_DEST_CASTER_RANDOM)
            dist = objSize + (dist - objSize) * (float)rand_norm();

        if (GetSpellInfo()->Id != 64133)
        {
            float origDist = dist;
            do
            {
                dest._position = caster->GetWorldLocation();
                dest._position.m_positionZ = 330.0f;
                dist = frand(30.0f, 30.0f + origDist);
                caster->MovePositionToFirstCollision(dest._position, dist, angle);
            }
            while (caster->GetExactDist2d(&dest._position) < 25.0f);
        }
        else
        {
            dest._position = caster->GetWorldLocation();
            dest._position.m_positionZ += 1.0f;
            caster->MovePositionToFirstCollision(dest._position, dist, angle);
        }

        caster->UpdateGroundPositionZ(dest._position.GetPositionX(), dest._position.GetPositionY(), dest._position.m_positionZ);
    }
};

// 64133
class spell_yogg_saron_summon_constrictor_tentacle : public SpellScriptLoader
{
    public:
        spell_yogg_saron_summon_constrictor_tentacle() : SpellScriptLoader("spell_yogg_saron_summon_constrictor_tentacle") { }

        class spell_yogg_saron_summon_constrictor_tentacle_SpellScript : public spell_yogg_saron_summon_tentacle_SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_summon_constrictor_tentacle_SpellScript);

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_yogg_saron_summon_constrictor_tentacle_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_CASTER_RADIUS);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_summon_constrictor_tentacle_SpellScript();
        }
};

// 64139
class spell_yogg_saron_summon_crusher_tentacle : public SpellScriptLoader
{
    public:
        spell_yogg_saron_summon_crusher_tentacle() : SpellScriptLoader("spell_yogg_saron_summon_crusher_tentacle") { }

        class spell_yogg_saron_summon_crusher_tentacle_SpellScript : public spell_yogg_saron_summon_tentacle_SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_summon_crusher_tentacle_SpellScript);

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_yogg_saron_summon_crusher_tentacle_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_summon_crusher_tentacle_SpellScript();
        }
};

// 64143
class spell_yogg_saron_summon_corruptor_tentacle : public SpellScriptLoader
{
    public:
        spell_yogg_saron_summon_corruptor_tentacle() : SpellScriptLoader("spell_yogg_saron_summon_corruptor_tentacle") { }

        class spell_yogg_saron_summon_corruptor_tentacle_SpellScript : public spell_yogg_saron_summon_tentacle_SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_summon_corruptor_tentacle_SpellScript);

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_yogg_saron_summon_corruptor_tentacle_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_summon_corruptor_tentacle_SpellScript();
        }
};

// 64158
class spell_yogg_saron_summon_immortal_guardian : public SpellScriptLoader
{
    public:
        spell_yogg_saron_summon_immortal_guardian() : SpellScriptLoader("spell_yogg_saron_summon_immortal_guardian") { }

        class spell_yogg_saron_summon_immortal_guardian_SpellScript : public spell_yogg_saron_summon_tentacle_SpellScript
        {
            PrepareSpellScript(spell_yogg_saron_summon_immortal_guardian_SpellScript);

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_yogg_saron_summon_immortal_guardian_SpellScript::TargetSelect, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_summon_immortal_guardian_SpellScript();
        }
};

// 63884
class spell_yogg_saron_death_ray_damage : public SpellScriptLoader
{
    public:
        spell_yogg_saron_death_ray_damage() : SpellScriptLoader("spell_yogg_saron_death_ray_damage") { }

        class spell_yogg_saron_death_ray_damage_SpellScript : public SanityReduction
        {
            PrepareSpellScript(spell_yogg_saron_death_ray_damage_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(Trinity::UnitAuraCheck(true, 63830)); // Malady of the Mind
                targets.remove_if(Trinity::UnitAuraCheck(true, 63881)); // Malady of the Mind
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_yogg_saron_death_ray_damage_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_yogg_saron_death_ray_damage_SpellScript();
        }
};

void AddSC_boss_yogg_saron()
{
    new boss_voice_of_yogg_saron();
    new boss_sara();
    new boss_yogg_saron();
    new boss_brain_of_yogg_saron();
    new npc_ominous_cloud();
    new npc_guardian_of_yogg_saron();
    new npc_corruptor_tentacle();
    new npc_constrictor_tentacle();
    new npc_crusher_tentacle();
    new npc_influence_tentacle();
    new npc_descend_into_madness();
    new npc_immortal_guardian();
    new npc_observation_ring_keeper();
    new npc_yogg_saron_keeper();
    new npc_yogg_saron_illusions();
    new npc_garona();
    new npc_turned_champion();
    new npc_laughing_skull();
    new npc_death_ray();
    new spell_yogg_saron_target_selectors();
    new spell_yogg_saron_psychosis();
    new spell_yogg_saron_malady_of_the_mind();
    new spell_yogg_saron_brain_link();
    new spell_yogg_saron_brain_link_damage();
    new spell_yogg_saron_boil_ominously();
    new spell_yogg_saron_shadow_beacon();
    new spell_yogg_saron_empowering_shadows_range_check();
    new spell_yogg_saron_empowering_shadows_missile();
    new spell_yogg_saron_constrictor_tentacle();
    new spell_yogg_saron_lunge();
    new spell_yogg_saron_squeeze();
    new spell_yogg_saron_diminsh_power();
    new spell_yogg_saron_empowered();
    new spell_yogg_saron_match_health();
    new spell_yogg_saron_shattered_illusion();
    new spell_yogg_saron_death_ray_warning_visual();
    new spell_yogg_saron_cancel_illusion_room_aura();
    new spell_yogg_saron_nondescript();
    new spell_yogg_saron_revealed_tentacle();
    new spell_yogg_saron_grim_reprisal();
    new spell_yogg_saron_induce_madness();
    new spell_yogg_saron_sanity();
    new spell_yogg_saron_insane();
    new spell_yogg_saron_insane_periodic();
    new spell_yogg_saron_lunatic_gaze();
    new spell_yogg_saron_keeper_aura();
    new spell_yogg_saron_hate_to_zero();
    new spell_yogg_saron_in_the_maws_of_the_old_god();
    new spell_yogg_saron_titanic_storm();
    new spell_yogg_saron_hodirs_protective_gaze();
    new spell_yogg_saron_summon_constrictor_tentacle();
    new spell_yogg_saron_summon_crusher_tentacle();
    new spell_yogg_saron_summon_corruptor_tentacle();
    new spell_yogg_saron_summon_immortal_guardian();
    new spell_yogg_saron_death_ray_damage();
}
