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
#include "dragon_soul.h"
#include "LFGMgr.h"
#include "Group.h"

// send SendPlaySpellVisualKit from limbs
// 22445 + 0 before casting visual spell
// 22446 + 0 after visual spell

enum DeathwingScriptedTexts
{
    SAY_DEATHWING_AGGRO     = 0,
    SAY_DEATHWING_PHASE     = 1,
    SAY_DEATHWING_BOLT      = 2,
    ANN_IMPALE_ASPECT       = 3,
    ANN_ATTACK_ALEXSTRASZA  = 4,
    ANN_ATTACK_NOZDORMU     = 5,
    ANN_ATTACK_YSERA        = 6,
    ANN_ATTACK_KALECGOS     = 7,
    ANN_ASSAULT_ASPECTS     = 8,
    ANN_ELEMENTIUM_BOLT     = 9,
    ANN_HEMMORHAGE          = 10,
    ANN_CATACLYSM           = 11,
    ANN_SECOND_PHASE        = 12,
    SAY_BERSERK             = 13,

    ANN_BLIST_TENTACLES     = 0,
};

enum AlexstraszaScriptedTexts
{
    SAY_ALEXSTRASZA_INTRO_1     = 0,
    SAY_ALEXSTRASZA_INTRO_2     = 1,
    SAY_ALEXSTRASZA_PLATFORM    = 2,
    SAY_ALEXSTRASZA_CATACLYSM   = 3,
    SAY_ALEXSTRASZA_PHASE_TWO   = 4,
    SAY_ALEXSTRASZA_OUTRO       = 5
};

enum NozdormuScriptedTexts
{
    SAY_NOZDORMU_INTRO          = 0,
    SAY_NOZDORMU_PLATFORM       = 1,
    SAY_NOZDORMU_CATACLYSM      = 2,
    SAY_NOZDORMU_SECOND_PHASE   = 3,
    SAY_NOZDORMU_OUTRO          = 4
};

enum YseraScriptedTexts
{
    SAY_YSERA_PLATFORM          = 0,
    SAY_YSERA_CATACLYSM         = 1,
    SAY_YSERA_SECOND_PHASE      = 2,
    SAY_YSERA_OUTRO             = 3
};

enum KalecgosScriptedTexts
{
    SAY_KALECGOS_PLATFORM       = 0,
    SAY_KALECGOS_CATACLYSM      = 1,
    SAY_KALECGOS_SECOND_PHASE   = 2,
    SAY_KALECGOS_OUTRO          = 3
};

enum ThrallScriptedTexts
{
    SAY_THRALL_INTRO            = 0,
    SAY_THRALL_SECOND_PHASE     = 1
};

enum Spells
{
    // Thrall
    SPELL_ASTRAL_RECALL                 = 108537, 

    SPELL_IDLE                          = 106187, // tail tentacle has it

    SPELL_TRIGGER_ASPECT_BUFFS          = 106943, // casted by deathwing 56173
    SPELL_SHARE_HEALTH_1                = 109547, // casted by deathwing 56173 on self ?
    SPELL_SHARE_HEALTH_2                = 109548, // casted by deathwing 56173 on 57962
    SPELL_ASSAULT_ASPECTS               = 107018, // casted by deathwing 56173
    SPELL_ELEMENTIUM_BOLT               = 105651,
    SPELL_ELEMENTIUM_METEOR_SCRIPT      = 105599,
    SPELL_ELEMENTIUM_METEOR_SUMMON      = 105658,
    SPELL_ELEMENTIUM_BLAST              = 105723,
    SPELL_ELEMENTIUM_METEOR_TARGET      = 106242, // target mark
    SPELL_ELEMENTIUM_METEOR_TRANSFORM_1 = 106991, // summon from sniffs ?
    SPELL_ELEMENTIUM_METEOR_TRANSFORM_2 = 110663, 
    SPELL_ELEMENTIUM_METEOR_AURA        = 110628,
    SPELL_ELEMENTIUM_METEOR_AURA_DMG    = 110632,
    SPELL_CATACLYSM                     = 106523,
    SPELL_SLUMP_1                       = 106708, // phase 2
    SPELL_SLUMP_2                       = 110062, // on death ?
    SPELL_CORRUPTED_BLOOD               = 106834,
    SPELL_CORRUPTED_BLOOD_STACKER       = 106843,
    SPELL_CORRUPTING_PARASITE_AOE       = 108597,
    SPELL_CORRUPTING_PARASITE_DMG       = 108601,
    SPELL_CORRUPTING_PARASITE_AURA      = 108649,
    SPELL_PARASITIC_BACKSLASH           = 108787,
    SPELL_UNSTABLE_CORRUPTION           = 108813,
    SPELL_SHRAPNEL_TRIGGER_AURA         = 106818,
    SPELL_BERSERK                       = 64238,
    SPELL_DEATHWING_CREDIT              = 111533,

    // Limb Tentacle
    SPELL_LIMB_EMERGE_VISUAL            = 107991,
    SPELL_SUMMON_BLISTERING_TENTACLE    = 105549,
    SPELL_BURNING_BLOOD                 = 105401,
    SPELL_TRIGGER_CONCENTRATION         = 106940,
    SPELL_AGONIZING_PAIN                = 106548,

    // Mutated Corruption
    SPELL_SUMMON_TAIL_FORCE             = 106239,
    SPELL_SUMMON_TAIL                   = 106240,
    SPELL_CRUSH_FORCE                   = 106382,
    SPELL_CRUSH_SUMMON                  = 106384,
    SPELL_CRUSH                         = 106385,
    SPELL_IMPALE                        = 106400,
    SPELL_IMPALE_DMG                    = 106444,
    SPELL_IMPALE_ASPECT_AOE             = 107026,
    SPELL_IMPALE_ASPECT                 = 107029,

    // Blistering Tentacle
    SPELL_BLISTERING_TENTACLE_VEHICLE   = 105550,
    SPELL_BLISTERING_HEAT               = 105444,
    SPELL_BLISTERING_HEAT_DMG           = 105445,

    // Hemorrhage
    SPELL_HEMORRHAGE_SUMMON_AOE         = 105853,
    SPELL_HEMORRHAGE_AURA               = 105861,
    SPELL_HEMORRHAGE_SCRIPT             = 105862,
    SPELL_HEMORRHAGE_MISSILE            = 105863,
    SPELL_HEMORRHAGE_SUMMON             = 105875,
    SPELL_REGENERATIVE_BLOOD_PERIODIC   = 105932,
    SPELL_REGENERATIVE_BLOOD_SCRIPT     = 105934,
    SPELL_REGENERATIVE_BLOOD_HEAL       = 105937,
    SPELL_REGENERATIVE_BLOOD_AURA       = 105969, // scale aura

    // Congealing Blood
    SPELL_CONGEALING_BLOOD_SUMMON       = 109089,
    SPELL_CONGEALING_BLOOD_HEAL         = 109102,

    // Elementium Fragment
    SPELL_ELEMENTIUM_FRAGMENT_SUMMON    = 106776,
    SPELL_SHRAPNEL                      = 106791,
    SPELL_SHRAPNEL_TARGET               = 106794,

    // Elementium Terror
    SPELL_ELEMENTIUM_TERROR_SUMMON      = 106766,

    // Aspects
    SPELL_ALEXSTRASZA_PRESENCE          = 105825,
    SPELL_ALEXSTRASZA_PRESENCE_AURA     = 106028,
    SPELL_CAUTERIZE_1                   = 105565, // blistering tentacle
    SPELL_CAUTERIZE_1_DMG               = 105569,
    SPELL_CAUTERIZE_2_1                 = 106860, // deathwing phase 2 5 sec
    SPELL_CAUTERIZE_2_2                 = 106888, // deathwing phase 2 10 sec
    SPELL_CAUTERIZE_2_3                 = 106889, // deathwing phase 2 15 sec
    SPELL_CAUTERIZE_2_DMG               = 106886,
    SPELL_CAUTERIZE_3                   = 108840, // parasite
    SPELL_CAUTERIZE_3_DMG               = 109045,

    SPELL_NOZDORMU_PRESENCE             = 105823,
    SPELL_NOZDORMU_PRESENCE_AURA        = 106027,
    SPELL_TIME_ZONE_FORCE               = 106919, // first spell, there are npcs 'platform'
    SPELL_TIME_ZONE_SUMMON_TARGET       = 105793, // summon target
    SPELL_TIME_ZONE_MISSILE_1           = 105799,
    SPELL_TIME_ZONE_SUMMON_1            = 105802, // summon time zone
    SPELL_TIME_ZONE                     = 105831, // main aura on trigger
    SPELL_TIME_ZONE_AURA_1              = 105830, // aura (debuff)
    SPELL_TIME_ZONE_MISSILE_2           = 107055, // ? maybe heroic
    SPELL_TIME_ZONE_SUMMON_2            = 107057, // ? maybe heroic
    SPELL_TIME_ZONE_AURA_2              = 108646, // for parasite

    SPELL_YSERA_PRESENCE                = 106456,
    SPELL_YSERA_PRESENCE_AURA           = 106457,
    SPELL_THE_DREAMER                   = 106463,
    SPELL_ENTER_THE_DREAM               = 106464,
    SPELL_DREAM                         = 106466,

    SPELL_KALECGOS_PRESENCE             = 106026,
    SPELL_KALECGOS_PRESENCE_AURA        = 106029,
    SPELL_SPELLWEAVER                   = 106039,
    SPELL_SPELLWEAVING                  = 106040,

    SPELL_EXPOSE_WEAKNESS_1             = 106588,
    SPELL_EXPOSE_WEAKNESS_2             = 106600,
    SPELL_EXPOSE_WEAKNESS_3             = 106613,
    SPELL_EXPOSE_WEAKNESS_4             = 106624,

    SPELL_CONCENTRATION_1               = 106641,
    SPELL_CONCENTRATION_2               = 106642,
    SPELL_CONCENTRATION_3               = 106643,
    SPELL_CONCENTRATION_4               = 106644,

    // Jump Pad
    SPELL_CARRYING_WINDS_1              = 106663, // casted by player, from 1 to 2
    SPELL_CARRYING_WINDS_SCRIPT_1       = 106666, // casted by pad on player
    SPELL_CARRYING_WINDS_2              = 106668, // from 2 to 1
    SPELL_CARRYING_WINDS_SCRIPT_2       = 106669,
    SPELL_CARRYING_WINDS_3              = 106670, // from 2 to 3
    SPELL_CARRYING_WINDS_SCRIPT_3       = 106671,
    SPELL_CARRYING_WINDS_4              = 106672, // from 3 to 2
    SPELL_CARRYING_WINDS_SCRIPT_4       = 106673,
    SPELL_CARRYING_WINDS_5              = 106674, // from 3 to 4
    SPELL_CARRYING_WINDS_SCRIPT_5       = 106675,
    SPELL_CARRYING_WINDS_6              = 106676, // from 4 to 3
    SPELL_CARRYING_WINDS_SCRIPT_6       = 106677,
    SPELL_CARRYING_WINDS_DUMMY          = 106678, // visual ?
    SPELL_CARRYING_WINDS_SPEED_10       = 106664,

    // final scene
    SPELL_THRALL_HOVER                  = 90766, // wrong spell, but works
    SPELL_CHARDE_DRAGON_SOUL_GREEN      = 108833,
    SPELL_CHARDE_DRAGON_SOUL_BLUE       = 108836,
    SPELL_CHARDE_DRAGON_SOUL_RED        = 108837,
    SPELL_CHARDE_DRAGON_SOUL_YELLOW     = 108838,
    SPELL_FIRE_DRAGON_SOUL              = 109971,
    SPELL_DEATHWING_DEATH               = 110101,
};

enum Adds
{
    NPC_WING_TENTACLE       = 56168, // 1 & 4
    NPC_ARM_TENTACLE_1      = 56846, // 2
    NPC_ARM_TENTACLE_2      = 56167, // 3
    
    NPC_LEFT_ARM            = 57694,
    NPC_RIGHT_ARM           = 57686,

    NPC_MUTATED_CORRUPTION  = 56471,
    NPC_CRUSH_TARGET        = 56581,
    NPC_BLISTERING_TENTACLE = 56188,
    NPC_REGENERAIVE_BLOOD   = 56263,
    NPC_HEMORRHAGE_TARGET   = 56359,
    NPC_ELEMENTIUM_BOLT     = 56262,
    NPC_CLAWK_MARK          = 56545,
    NPC_CORRUPTING_PARASITE = 57479,
    NPC_TIME_ZONE_TARGET    = 56332,
    NPC_TIME_ZONE           = 56311,

    NPC_DEATHWING_1         = 57962, // invisible ?
    NPC_CONGEALING_BLOOD    = 57798,
    NPC_CONGEALING_BLOOD_T  = 57788,
    NPC_ELEMENTIUM_FRAGMENT = 56724,
    NPC_ELEMENTIUM_TERROR   = 56710,

    NPC_COSMETIC_TENTACLE   = 57693,
    NPC_TAIL_TENTACLE       = 56844,

    NPC_JUMP_PAD            = 56699,
    NPC_PLATFORM            = 56307,

    NPC_DRAGON_SOUL_MADNESS = 56694,
};

enum Events
{
    EVENT_ASPECTS_FACING        = 1,
    EVENT_SPAWN_CHEST           = 2,
    // Intro
    EVENT_THRALL_INTRO          = 3,
    EVENT_ALEXTRASZA_INTRO_1    = 4,
    EVENT_ALEXTRASZA_INTRO_2    = 5,
    EVENT_NOZDORMU_INTRO        = 6,
    EVENT_MOVE_THRALL           = 7,
    EVENT_SUMMON_DEATHWING      = 8,
    // Battle
    EVENT_SPAWN_LIMBS_1         = 9,
    EVENT_SPAWN_LIMBS_2         = 10,
    EVENT_SPAWN_LIMBS_3         = 11,
    EVENT_CHECK_PLAYERS         = 12,
    EVENT_ASSAULT_ASPECTS       = 13,
    EVENT_CRUSH                 = 14,
    EVENT_IMPALE                = 15,
    EVENT_CRUSH_1               = 16,
    EVENT_ASSAULT_ASPECTS_1     = 17,
    EVENT_HEMORRHAGE            = 18,
    EVENT_UPDATE_HEALTH         = 19,
    EVENT_START_MOVE            = 20,
    EVENT_ELEMENTIUM_BOLT       = 21,
    EVENT_CATACLYSM             = 22,
    EVENT_CORRUPTING_PARASITE   = 23,
    EVENT_UNSTABLE_CORRUPTION   = 24,
    EVENT_TIME_ZONE             = 25,
    EVENT_CAUTERIZE             = 26,
    EVENT_SELECT_VICTIM         = 27,
    EVENT_SLUMP_1               = 28,
    EVENT_SLUMP_2               = 29,
    EVENT_CONGEALING_BLOOD      = 30,
    EVENT_BERSERK               = 31,
    EVENT_ELEMENTIUM_FRAGMENT   = 32,
    EVENT_ELEMENTIUM_TERROR     = 33,
    EVENT_CORRUPTED_BLOOD       = 34,
    // Second phase
    EVENT_ASPECTS_MOVE          = 35,
    EVENT_ALEXTRASZA_PHASE_TWO  = 36,
    EVENT_NOZDORMU_SECOND_PHASE = 37,
    EVENT_YSERA_SECOND_PHASE    = 38,
    EVENT_KALECGOS_SECOND_PHASE = 39,
    EVENT_THRALL_SECOND_PHASE   = 40,
    // Outro
    EVENT_END_BATTLE            = 41,
    EVENT_THRALL_JUMP           = 42,
    EVENT_DS_CHARGING_1         = 43,
    EVENT_DS_CHARGING_2         = 44,
    EVENT_DS_CHARGING_3         = 45,
    EVENT_DEATHWING_EXPLOSION   = 46,
    EVENT_DEATHWING_SKYBOX      = 47,
    EVENT_DEATHWING_DESPAWN     = 48,
    EVENT_DELAY_PLAY_MOVIE      = 49,
    EVENT_ALEXTRASZA_OUTRO      = 50,
    EVENT_NOZDORMU_OUTRO        = 51,
    EVENT_YSERA_OUTRO           = 52,
    EVENT_KALECGOS_OUTRO        = 53,
    EVENT_FINAL_OUTRO           = 54,
    EVENT_TAKE_DRAGON_SOUL      = 55,
    EVENT_CHECK_ARMS            = 56,
};

enum Actions
{
    ACTION_START_INTRO              = 0,
    ACTION_CHECK_PLAYERS            = 1,
    ACTION_CRUSH                    = 2,
    ACTION_RESET_BATTLE             = 3,
    ACTION_ASSAULT_ASPECTS          = 4,
    ACTION_ASSAULT_ASPECTS_FIRST    = 5,
    ACTION_PARASITIC_BACKSLASH      = 6,
    ACTION_TIME_ZONE                = 7,
    ACTION_CAUTERIZE                = 8,
    ACTION_END_BATTLE               = 9,
    ACTION_SECOND_PHASE             = 10,
    ACTION_CAUTERIZE_PARASITE       = 11,
};

enum Other
{
    DATA_ASSAULT_PLATFORM   = 1,
    DATA_DESTROY_PLATFORM   = 2,
    DATA_CURRENT_PLATFORM   = 3,
    DATA_METEOR_POINT       = 4,
};

enum Points
{
    POINT_CONGEALING_BLOOD_HEAL = 1,
};

enum Misc
{
    WORLDSTATE_DEATHWING_MADNESS = 9954, // for second achievement that allow enter to heroic mode
};

uint8 GetGround(const WorldObject* who)
{
    if (who->GetPositionX() > -11979.311523f && who->GetPositionX() < -11938.857422f &&
        who->GetPositionY() > 12236.340820f && who->GetPositionY() < 12299.116211f)
        return 1;
    else if (who->GetPositionX() > -12074.251953f && who->GetPositionX() < -12002.294922f &&
        who->GetPositionY() > 12184.281250f && who->GetPositionY() < 12256.737305f)
        return 2;
    else if (who->GetPositionX() > -12135.291992f && who->GetPositionX() < -12054.627930f &&
        who->GetPositionY() > 12129.464844f && who->GetPositionY() < 12190.023438f)
        return 3;
    else if (who->GetPositionX() > -12164.257813f && who->GetPositionX() < -12075.483398f &&
        who->GetPositionY() > 12051.344727f && who->GetPositionY() < 12107.757813f)
        return 4;

    return 0;
}

const Position mutatedcorruptionPos[4] = 
{
    { -11993.3f, 12286.3f, -2.58115f, 5.91667f  }, // 1
    { -12028.8f, 12265.6f, -6.27147f, 4.13643f  }, // 2
    { -12107.4f, 12201.9f, -5.32397f, 5.16617f  }, // 3
    { -12160.9f, 12057.0f,  2.47362f, 0.733038f }  // 4
};

const Position limbsPos[4] = 
{
    { -11941.2f, 12248.9f,  12.1499f, 1.98968f  },
    { -12005.8f, 12190.3f, -6.59399f, 2.1293f   },
    { -12065.0f, 12127.2f, -3.2946f,  2.338740f },
    { -12097.8f, 12067.4f,  13.4888f, 2.21657f  }
};

const Position hemorrhagePos[4] = 
{
    { -11955.948242f, 12281.756836f,  1.30f, 0.0f },
    { -12048.077148f, 12237.601563f, -6.14f, 0.0f },
    { -12113.989258f, 12166.718750f, -2.72f, 0.0f },
    { -12146.370117f, 12093.588867f,  2.31f, 0.0f }
};

const Position boltPos[5] = 
{
    { -11929.83f,     12035.63f,      35.45797f, 0.0f},
    { -11961.268555f, 12286.041992f,  1.30f,     0.0f}, // 1
    { -12055.000977f, 12239.061523f, -6.14f,     0.0f}, // 2
    { -12112.834961f, 12170.205078f, -2.72f,     0.0f}, // 3
    { -12149.885742f, 12081.416992f,  2.31f,     0.0f}  // 4
};

const Position timezonePos[5] = 
{
    { -11951.963867f, 12265.349609f,  1.30f, 0.0f },
    { -12039.828125f, 12226.136719f, -6.14f, 0.0f },
    { -12098.596680f, 12156.881836f, -2.72f, 0.0f },
    { -12119.416992f, 12070.395508f,  2.31f, 0.0f },
    { -12106.200195f, 12162.099609f  -2.66f, 0.0f }
};

const Position blisteringPos[4] =
{                                                 
    { -11942.116211f, 12249.538086f,  1.37f, 0.0f },
    { -12025.414063f, 12213.312500f, -6.14f, 0.0f },
    { -12084.824219f, 12146.507813f, -2.72f, 0.0f },
    { -12102.188477f, 12067.497070f,  2.31f, 0.0f },
};

const Position thrallPos = { -12128.3f, 12253.8f, 0.0451f, 0.0f }; // Thrall teleports here
const Position deathwingPos = { -11903.9f, 11989.1f, -113.204f, 2.16421f };
const Position deathwing2Pos = { -12063.5f, 12198.9f, -13.04f, 2.16421f };

const Position alexstraszaPos = { -11957.3f, 12338.3f, 38.9364f, 5.06145f };
const Position nozdormuPos = { -12093.8f, 12312.0f, 43.228f, 5.42797f };
const Position yseraPos = { -12157.4f, 12212.5f, 36.0152f, 5.75959f };
const Position kalecgosPos = { -12224.8f, 12128.5f, 68.96f, 5.84685f };

const Position jumpPos[6] = 
{
    { -11972.8f, 12272.8f,  1.38396f, 0.0f }, // from 1 to 2
    { -12023.f,  12228.5f, -6.0689f,  0.0f }, // from 2 to 1
    { -12047.9f, 12213.9f, -5.94278f, 0.0f }, // from 2 to 3
    { -12087.5f, 12165.7f, -2.65091f, 0.0f }, // from 3 to 2
    { -12099.6f, 12152.8f, -2.65092f, 0.0f }, // from 3 to 4
    { -12118.2f, 12084.9f,  2.39029f, 0.0f }  // from 4 to 3
};

const Position armsPos[2] =
{
    { -11967.1f, 11958.8f, -49.9822f, 2.16421f }, // left
    { -11852.1f, 12036.4f, -49.9821f, 2.16421f }, // right
};

const Position jumpThrallPos = { -12072.790f, 12191.343f, 15.300f, 5.51f }; // jump pos

const Position outroPos[6] = 
{
    { -12066.1f, 12150.4f, -2.65091f, 3.05433f },  // Aggra
    { -12067.7f, 12146.4f, -2.65091f, 3.05433f },  // Thrall
    { -12069.2f, 12159.9f, -2.65091f, 5.23599f },  // Kalecgos
    { -12078.4f, 12147.5f, -2.65092f, 0.17453f },  // Nozdormu
    { -12077.3f, 12152.3f, -2.65092f, 6.00393f },  // Alextrasza
    { -12073.8f, 12156.6f, -2.65091f, 5.55015f }    // Ysera
};

enum Menus
{
    GOSSIP_MENU_MADNESS_START = 13295
};

class boss_madness_of_deathwing : public CreatureScript
{
    public:
        boss_madness_of_deathwing() : CreatureScript("boss_madness_of_deathwing") { }

        struct boss_madness_of_deathwingAI : public BossAI
        {
            boss_madness_of_deathwingAI(Creature* creature) : BossAI(creature, DATA_MADNESS)
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
                
                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                memset(limbsGuids, 0, sizeof(limbsGuids));
                m_current_platform = 0;
                m_destroyed_platform = 0;
                m_isFirst = true;
                m_chromaticChampionCreditCriteria = 0;
            }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (me->Attack(target, true))
                    DoStartNoMovement(target);
            }

            void Reset() override
            {
                m_chromaticChampionCreditCriteria = 0;
                me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_A, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_K, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_N, 0);
                me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_Y, 0);
                events.Reset();
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                instance->SetBossState(DATA_MADNESS, IN_PROGRESS);
                events.ScheduleEvent(EVENT_BERSERK, 900000);
                events.ScheduleEvent(EVENT_SPAWN_LIMBS_1, 1);
                events.ScheduleEvent(EVENT_CHECK_ARMS, 500);
            }

            void JustSummoned(Creature* summon) override
            {
                BossAI::JustSummoned(summon);
                if (me->HasAura(SPELL_BERSERK))
                    summon->CastSpell(summon, SPELL_BERSERK, true);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_ASSAULT_PLATFORM)
                {
                    m_current_platform = (uint8)data;

                    events.ScheduleEvent(EVENT_ASSAULT_ASPECTS_1, 500);
                }
                else if (type == DATA_DESTROY_PLATFORM)
                    m_destroyed_platform |= (1 << data);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_DESTROY_PLATFORM)
                    return m_destroyed_platform;
                else if (type == DATA_CURRENT_PLATFORM)
                    return m_current_platform;

                return 0;
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_AGONIZING_PAIN)
                {
                    me->InterruptNonMeleeSpells(true);
                    events.CancelEvent(EVENT_CATACLYSM);
                    events.CancelEvent(EVENT_ELEMENTIUM_BOLT);

                    if ((m_destroyed_platform & (1 << 1)) > 0 && 
                        (m_destroyed_platform & (1 << 2)) > 0 &&
                        (m_destroyed_platform & (1 << 3)) > 0 &&
                        (m_destroyed_platform & (1 << 4)) > 0)
                        events.ScheduleEvent(EVENT_SLUMP_1, 5000);
                    else
                        events.ScheduleEvent(EVENT_ASSAULT_ASPECTS, 7000);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_ARMS:
                            if (Creature* pArmL = me->SummonCreature(NPC_LEFT_ARM, armsPos[0]))
                                pArmL->EnterVehicle(me, 1);
                            if (Creature* pArmR = me->SummonCreature(NPC_RIGHT_ARM, armsPos[1]))
                                pArmR->EnterVehicle(me, 0);
                            break;
                        case EVENT_SPAWN_LIMBS_1:
                            if (Creature* pLimb = me->SummonCreature(NPC_WING_TENTACLE, limbsPos[0]))
                            {
                                pLimb->SendPlaySpellVisualKit(22445, 0, 0);
                                limbsGuids[0] = pLimb->GetGUID();
                                pLimb->AI()->SetData(DATA_CURRENT_PLATFORM, 1);
                            }
                            if (Creature* pLimb = me->SummonCreature(NPC_ARM_TENTACLE_1, limbsPos[1]))
                            {
                                pLimb->SendPlaySpellVisualKit(22445, 0, 0);
                                limbsGuids[1] = pLimb->GetGUID();
                                pLimb->AI()->SetData(DATA_CURRENT_PLATFORM, 2);
                            }
                            if (Creature* pLimb = me->SummonCreature(NPC_ARM_TENTACLE_2, limbsPos[2]))
                            {
                                pLimb->SendPlaySpellVisualKit(22445, 0, 0);
                                limbsGuids[2] = pLimb->GetGUID();
                                pLimb->AI()->SetData(DATA_CURRENT_PLATFORM, 3);
                            }
                            if (Creature* pLimb = me->SummonCreature(NPC_WING_TENTACLE, limbsPos[3]))
                            {
                                pLimb->SendPlaySpellVisualKit(22445, 0, 0);
                                limbsGuids[3] = pLimb->GetGUID();
                                pLimb->AI()->SetData(DATA_CURRENT_PLATFORM, 4);
                            }
                            events.ScheduleEvent(EVENT_SPAWN_LIMBS_2, 100);
                            break;
                        case EVENT_SPAWN_LIMBS_2:
                            for (uint8 i = 0; i < 4; ++i)
                                if (Creature* pLimb = ObjectAccessor::GetCreature(*me, limbsGuids[i]))
                                    pLimb->CastSpell(pLimb, SPELL_LIMB_EMERGE_VISUAL, true);
                            events.ScheduleEvent(EVENT_SPAWN_LIMBS_3, 500);
                            break;
                        case EVENT_SPAWN_LIMBS_3:
                            for (uint8 i = 0; i < 4; ++i)
                                if (Creature* pLimb = ObjectAccessor::GetCreature(*me, limbsGuids[i]))
                                    pLimb->SendPlaySpellVisualKit(22446, 0, 0);
                            events.ScheduleEvent(EVENT_ASSAULT_ASPECTS, 2000);
                            break;
                        case EVENT_ASSAULT_ASPECTS:
                            Talk(ANN_ASSAULT_ASPECTS);
                            DoCastAOE(SPELL_ASSAULT_ASPECTS);
                            break;
                        case EVENT_ASSAULT_ASPECTS_1:
                        {
                            if (Creature* pTentacle = me->SummonCreature(NPC_MUTATED_CORRUPTION, mutatedcorruptionPos[m_current_platform - 1]))
                                DoZoneInCombat(pTentacle, 200.0f);

                            switch (m_current_platform)
                            {
                                case 1:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                        pAspect->AI()->Talk(SAY_ALEXSTRASZA_PLATFORM);
                                    if (!m_chromaticChampionCreditCriteria)
                                    {
                                        m_chromaticChampionCreditCriteria = CRITERIA_ALEXSTRASZA_FIRST;
                                        me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_A, 1);
                                    }
                                    Talk(ANN_ATTACK_ALEXSTRASZA);
                                    break;
                                case 2:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                        pAspect->AI()->Talk(SAY_NOZDORMU_PLATFORM);
                                    if (!m_chromaticChampionCreditCriteria)
                                    {
                                        m_chromaticChampionCreditCriteria = CRITERIA_NOZDORMU_FIRST;
                                        me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_N, 1);
                                    }
                                    Talk(ANN_ATTACK_NOZDORMU);
                                    break;
                                case 3:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                        pAspect->AI()->Talk(SAY_YSERA_PLATFORM);
                                    if (!m_chromaticChampionCreditCriteria)
                                    {
                                        m_chromaticChampionCreditCriteria = CRITERIA_YSERA_FIRST;
                                        me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_Y, 1);
                                    }
                                     Talk(ANN_ATTACK_YSERA);
                                    break;
                                case 4:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                        pAspect->AI()->Talk(SAY_KALECGOS_PLATFORM);
                                    if (!m_chromaticChampionCreditCriteria)
                                    {
                                        m_chromaticChampionCreditCriteria = CRITERIA_KALECGOS_FIRST;
                                        me->GetMap()->SetWorldState(WORLDSTATE_CHROMATIC_SHAMPION_K, 1);
                                    }
                                    Talk(ANN_ATTACK_KALECGOS);
                                    break;
                            }

                            if (Creature* pLimb = this->GetLimbTentacleAtPlatform(m_current_platform))
                                pLimb->AI()->DoAction(m_isFirst ? ACTION_ASSAULT_ASPECTS_FIRST : ACTION_ASSAULT_ASPECTS);

                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->AI()->DoAction(ACTION_TIME_ZONE);

                            events.ScheduleEvent(EVENT_ELEMENTIUM_BOLT, m_isFirst ? (40500 - 10000) : (55500 - 10000));
                            events.ScheduleEvent(EVENT_CATACLYSM, m_isFirst ? (115500 - 10000) : (130500 - 10000));

                            m_isFirst = false;
                            break;
                        }
                        case EVENT_ELEMENTIUM_BOLT:
                            DoCast(me, SPELL_ELEMENTIUM_BOLT);
                            Talk(ANN_ELEMENTIUM_BOLT);
                            Talk(SAY_DEATHWING_BOLT);
                            break;
                        case EVENT_CATACLYSM:
                        {
                            Talk(ANN_CATACLYSM);
                            DoCast(me, SPELL_CATACLYSM);

                            Creature* pLimb = GetLimbTentacleAtPlatform(m_current_platform);

                            switch (m_current_platform)
                            {
                                case 1:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                    {
                                        pAspect->AI()->Talk(SAY_ALEXSTRASZA_CATACLYSM);
                                        if (pLimb)
                                            pAspect->CastSpell(pLimb, SPELL_EXPOSE_WEAKNESS_1);
                                    }
                                    break;
                                case 2:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                    {
                                        pAspect->AI()->Talk(SAY_NOZDORMU_CATACLYSM);
                                        if (pLimb)
                                            pAspect->CastSpell(pLimb, SPELL_EXPOSE_WEAKNESS_2);
                                    }
                                    break;
                                case 3:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                    {
                                        pAspect->AI()->Talk(SAY_YSERA_CATACLYSM);
                                        if (pLimb)
                                            pAspect->CastSpell(pLimb, SPELL_EXPOSE_WEAKNESS_3);
                                    }
                                    break;
                                case 4:
                                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                    {
                                        pAspect->AI()->Talk(SAY_KALECGOS_CATACLYSM);
                                        if (pLimb)
                                            pAspect->CastSpell(pLimb, SPELL_EXPOSE_WEAKNESS_4);
                                    }
                                    break;
                            }
                            break;
                        }
                        case EVENT_SLUMP_1:
                            me->SetFacingTo(deathwingPos.GetOrientation());
                            Talk(SAY_DEATHWING_PHASE);
                            Talk(ANN_SECOND_PHASE);
                            DoCast(SPELL_SLUMP_1);
                            if (Creature* pThrall = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_THRALL_MADNESS)))
                                pThrall->AI()->DoAction(ACTION_SECOND_PHASE);
                            events.ScheduleEvent(EVENT_SLUMP_2, 700);
                            break;
                        case EVENT_SLUMP_2:
                            if (Creature* pDeathwing = me->SummonCreature(NPC_DEATHWING_1, deathwing2Pos))
                            {
                                pDeathwing->SetMaxHealth(me->GetMaxHealth());
                                pDeathwing->SetHealth(me->GetHealth());
                                pDeathwing->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            }
                            break;
                        case EVENT_BERSERK:
                        {
                            Talk(SAY_BERSERK);
                            DoCastAOE(SPELL_BERSERK, true);

                            std::list<Creature*> creatures;
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_MUTATED_CORRUPTION, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_BLISTERING_TENTACLE, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_REGENERAIVE_BLOOD, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_ELEMENTIUM_BOLT, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_CORRUPTING_PARASITE, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_CONGEALING_BLOOD, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_ELEMENTIUM_FRAGMENT, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_ELEMENTIUM_TERROR, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_TAIL_TENTACLE, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_WING_TENTACLE, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_ARM_TENTACLE_1, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_ARM_TENTACLE_2, 1000.0f);
                            GetCreatureListWithEntryInGrid(creatures, me, NPC_DEATHWING_1, 1000.0f);
                            if (!creatures.empty())
                                for (std::list<Creature*>::iterator itr = creatures.begin(); itr != creatures.end(); ++itr)
                                    if (Creature* creature = *itr)
                                        creature->CastSpell(creature, SPELL_BERSERK, true);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

        private:
            ObjectGuid limbsGuids[4];
            uint8 m_current_platform;
            uint8 m_destroyed_platform;
            bool m_isFirst;
            uint32 m_chromaticChampionCreditCriteria;

            Creature* GetLimbTentacleAtPlatform(uint8 platform)
            {
                if (platform == 1 || platform == 4)
                {
                    std::list<Creature*> creatures;
                    me->GetCreatureListWithEntryInGrid(creatures, NPC_WING_TENTACLE, 500.0f);
                    if (!creatures.empty())
                    {
                        for (std::list<Creature*>::const_iterator itr = creatures.begin(); itr != creatures.end(); ++itr)
                            if ((*itr)->IsAlive())
                            {
                                if (platform == 1 && (*itr)->GetPositionY() > 12200.0f)
                                    return (*itr);
                                else if (platform == 4 && (*itr)->GetPositionY() < 12200.0f)
                                    return (*itr);
                            }
                    }
                }
                else if (platform == 2)
                    return me->FindNearestCreature(NPC_ARM_TENTACLE_1, 500.0f, true);
                else if (platform == 3)
                    return me->FindNearestCreature(NPC_ARM_TENTACLE_2, 500.0f, true);

                return NULL;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_madness_of_deathwingAI>(creature);
        }
};

class npc_dragon_soul_thrall_1 : public CreatureScript
{
    public:
        npc_dragon_soul_thrall_1() : CreatureScript("npc_dragon_soul_thrall_1") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->IsInCombat())
                return true;

            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->IsEncounterInProgress())
                    return true;

                if (instance->GetBossState(DATA_SPINE) != DONE ||
                    instance->GetBossState(DATA_MADNESS) == IN_PROGRESS ||
                    instance->GetBossState(DATA_MADNESS) == DONE)
                    return true;

                if (instance->instance->IsHeroic())
                {
                    if (Group* group = player->GetGroup())
                    {
                        Player* leader = ObjectAccessor::GetPlayer(*player, group->GetLeaderGUID());
                        if (!leader || !leader->HasAchieved(6115))
                            return true;
                    }
                    else if (!player->HasAchieved(6115))
                        return true;
                }

                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                
                return true;
            }
            return false;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->SendCloseGossip();

            if (player->IsInCombat())
                return true;

            if (InstanceScript* instance = creature->GetInstanceScript())
            {
                if (instance->IsEncounterInProgress())
                    return true;

                if (instance->GetBossState(DATA_SPINE) != DONE ||
                    instance->GetBossState(DATA_MADNESS) == IN_PROGRESS ||
                    instance->GetBossState(DATA_MADNESS) == DONE)
                    return true;

                if (instance->instance->IsHeroic())
                {
                    if (Group* group = player->GetGroup())
                    {
                        Player* leader = ObjectAccessor::GetPlayer(*player, group->GetLeaderGUID());
                        if (!leader || !leader->HasAchieved(6115))
                            return true;
                    }
                    else if (!player->HasAchieved(6115))
                        return true;
                }

                if (action == GOSSIP_ACTION_INFO_DEF + 1)
                    creature->AI()->DoAction(ACTION_START_INTRO);
            }
            return true;
        }

        struct npc_dragon_soul_thrall_1AI : public ScriptedAI
        {
            npc_dragon_soul_thrall_1AI(Creature* creature) : ScriptedAI(creature)
            {             
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                instance = me->GetInstanceScript();
                chestSpawned = false;
                m_combatStarted = false;
            }

            void Reset() override
            {
                events.Reset();
                if (instance->GetBossState(DATA_MADNESS) != DONE)
                    events.ScheduleEvent(EVENT_TAKE_DRAGON_SOUL, 2000);
                if (instance->GetBossState(DATA_MADNESS) == DONE)
                    events.ScheduleEvent(EVENT_FINAL_OUTRO, 500);
                if (!chestSpawned && instance->GetBossState(DATA_SPINE) == DONE)
                {
                    events.ScheduleEvent(EVENT_SPAWN_CHEST, 5000); // Hackish chest spawn
                    chestSpawned = true;
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_START_INTRO:
                        if (m_combatStarted)
                            return;
                        m_combatStarted = true;
                        events.ScheduleEvent(EVENT_MOVE_THRALL, 1000);
                        break;
                    case ACTION_SECOND_PHASE:
                        events.ScheduleEvent(EVENT_ASPECTS_MOVE, 1000);
                        break;
                    case ACTION_RESET_BATTLE:
                        ResetBattle();
                        break;
                    case ACTION_END_BATTLE:
                        events.ScheduleEvent(EVENT_END_BATTLE, 1000);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ASPECTS_FACING:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->SetFacingTo(5.06145f);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->SetFacingTo(5.42797f);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                pAspect->SetFacingTo(5.75959f);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                pAspect->SetFacingTo(5.84685f);
                            break;
                        case EVENT_TAKE_DRAGON_SOUL:
                            if (Creature* dSoul = me->FindNearestCreature(NPC_DRAGON_SOUL_MADNESS, 100.0f))
                                dSoul->EnterVehicle(me, 0);
                            break;
                        // intro event
                        case EVENT_SPAWN_CHEST:
                            instance->SetData(DATA_SPAWN_GREATER_CHEST, 0);
                            events.ScheduleEvent(EVENT_THRALL_INTRO, 1000);
                            break;
                        case EVENT_THRALL_INTRO:
                            Talk(SAY_THRALL_INTRO);
                            events.ScheduleEvent(EVENT_ALEXTRASZA_INTRO_1, 10000);
                            break;
                        case EVENT_ALEXTRASZA_INTRO_1:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->AI()->Talk(SAY_ALEXSTRASZA_INTRO_1);
                            events.ScheduleEvent(EVENT_ALEXTRASZA_INTRO_2, 8000);
                            break;
                        case EVENT_ALEXTRASZA_INTRO_2:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->AI()->Talk(SAY_ALEXSTRASZA_INTRO_2);
                            events.ScheduleEvent(EVENT_NOZDORMU_INTRO, 11000);
                            break;
                        case EVENT_NOZDORMU_INTRO:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->AI()->Talk(SAY_NOZDORMU_INTRO);
                            break;
                        case EVENT_MOVE_THRALL:
                            DoCast(me, SPELL_ASTRAL_RECALL);
                            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                            events.ScheduleEvent(EVENT_SUMMON_DEATHWING, 1000);
                            break;
                        case EVENT_SUMMON_DEATHWING:
                            if (Creature* pDeathwing = me->SummonCreature(NPC_DEATHWING, deathwingPos))
                            {
                                pDeathwing->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                                pDeathwing->AI()->DoZoneInCombat(pDeathwing, 500.0f);
                                pDeathwing->AI()->Talk(SAY_DEATHWING_AGGRO);
                            }
                            events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5000);
                            break;
                        // battle
                        case EVENT_CHECK_PLAYERS:
                            if (instance)
                            {
                                if (instance->GetBossState(DATA_MADNESS) == IN_PROGRESS && !CheckPlayers())
                                {
                                    ResetBattle();
                                }
                                else
                                    events.ScheduleEvent(EVENT_CHECK_PLAYERS, 1000);
                            }
                            break;
                        case EVENT_ASPECTS_MOVE:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                            {
                                pAspect->InterruptNonMeleeSpells(true);
                                pAspect->GetMotionMaster()->MovePoint(0, aspectsMadness[0]);
                                pAspect->SetHomePosition(aspectsMadness[0]);
                            }
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                            {
                                pAspect->InterruptNonMeleeSpells(true);
                                pAspect->GetMotionMaster()->MovePoint(0, aspectsMadness[1]);
                                pAspect->SetHomePosition(aspectsMadness[1]);
                            }
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                            {
                                pAspect->InterruptNonMeleeSpells(true);
                                pAspect->GetMotionMaster()->MovePoint(0, aspectsMadness[2]);
                                pAspect->SetHomePosition(aspectsMadness[2]);
                            }
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                            {
                                pAspect->InterruptNonMeleeSpells(true);
                                pAspect->GetMotionMaster()->MovePoint(0, aspectsMadness[3]);
                                pAspect->SetHomePosition(aspectsMadness[3]);
                            }
                            events.ScheduleEvent(RAND(EVENT_ALEXTRASZA_PHASE_TWO,EVENT_NOZDORMU_SECOND_PHASE,EVENT_YSERA_SECOND_PHASE,EVENT_KALECGOS_SECOND_PHASE), 16000);
                            break;
                        case EVENT_ALEXTRASZA_PHASE_TWO:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->AI()->Talk(SAY_ALEXSTRASZA_PHASE_TWO);
                            events.ScheduleEvent(EVENT_THRALL_SECOND_PHASE, 16000);
                            break;
                        case EVENT_NOZDORMU_SECOND_PHASE:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->AI()->Talk(SAY_NOZDORMU_SECOND_PHASE);
                            events.ScheduleEvent(EVENT_THRALL_SECOND_PHASE, 16000);
                            break;
                        case EVENT_YSERA_SECOND_PHASE:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                pAspect->AI()->Talk(SAY_YSERA_SECOND_PHASE);
                            events.ScheduleEvent(EVENT_THRALL_SECOND_PHASE, 16000);
                            break;
                        case EVENT_KALECGOS_SECOND_PHASE:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                pAspect->AI()->Talk(SAY_KALECGOS_SECOND_PHASE);
                            events.ScheduleEvent(EVENT_THRALL_SECOND_PHASE, 16000);
                            break;
                        case EVENT_THRALL_SECOND_PHASE:
                            Talk(SAY_THRALL_SECOND_PHASE);
                            break;
                        // outro
                        case EVENT_END_BATTLE:
                            ResetBattle(true);
                            DoCastAOE(SPELL_THRALL_HOVER);
                            if (Creature* pDeathwing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DEATHWING)))
                                pDeathwing->CastSpell(pDeathwing, SPELL_SLUMP_2);
                            events.ScheduleEvent(EVENT_THRALL_JUMP, 3000);
                            events.ScheduleEvent(RAND(EVENT_ALEXTRASZA_OUTRO,EVENT_NOZDORMU_OUTRO,EVENT_YSERA_OUTRO,EVENT_KALECGOS_OUTRO), 3000);
                            break;
                        case EVENT_ALEXTRASZA_OUTRO:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->AI()->Talk(SAY_ALEXSTRASZA_OUTRO);
                            break;
                        case EVENT_NOZDORMU_OUTRO:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->AI()->Talk(SAY_NOZDORMU_OUTRO);
                            break;
                        case EVENT_YSERA_OUTRO:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                pAspect->AI()->Talk(SAY_YSERA_OUTRO);
                            break;
                        case EVENT_KALECGOS_OUTRO:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                pAspect->AI()->Talk(SAY_KALECGOS_OUTRO);
                            break;
                        case EVENT_THRALL_JUMP:
                            me->GetMotionMaster()->MoveJump(jumpThrallPos.GetPositionX(), jumpThrallPos.GetPositionY(), jumpThrallPos.GetPositionZ(), 30.0f, 10.0f, jumpThrallPos.GetOrientation());
                            events.ScheduleEvent(EVENT_DS_CHARGING_1, 2500);
                            break;
                        case EVENT_DS_CHARGING_1:
                            if (!instance->GetData(DATA_IS_LFR))
                            {
                                instance->DoModifyPlayerCurrencies(CURRENCY_TYPE_VALOR_POINTS, 10000);
                                instance->DoModifyPlayerCurrencies(CURRENCY_TYPE_ESSENCE_OF_CORRUPTED_DEATHWING, 1);
                            }
                            DoCast(SPELL_FIRE_DRAGON_SOUL);
                            events.ScheduleEvent(EVENT_DS_CHARGING_2, 2000);
                            break;
                        case EVENT_DS_CHARGING_2:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->CastSpell(me, SPELL_CHARDE_DRAGON_SOUL_RED);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->CastSpell(me, SPELL_CHARDE_DRAGON_SOUL_YELLOW);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                pAspect->CastSpell(me, SPELL_CHARDE_DRAGON_SOUL_GREEN);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                pAspect->CastSpell(me, SPELL_CHARDE_DRAGON_SOUL_BLUE);
                            events.ScheduleEvent(EVENT_DS_CHARGING_3, 11000);
                            break;
                        case EVENT_DS_CHARGING_3:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->InterruptNonMeleeSpells(true);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                pAspect->InterruptNonMeleeSpells(true);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                pAspect->InterruptNonMeleeSpells(true);
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                pAspect->InterruptNonMeleeSpells(true);
                            events.ScheduleEvent(EVENT_DEATHWING_EXPLOSION, 3500);
                            break;
                        case EVENT_DEATHWING_EXPLOSION:
                            if (Creature* pDeathwing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DEATHWING)))
                                pDeathwing->CastSpell(pDeathwing, SPELL_DEATHWING_DEATH);
                            events.ScheduleEvent(EVENT_DEATHWING_SKYBOX, 5000);
                            break;
                        case EVENT_DEATHWING_SKYBOX:
                        {
                            Map::PlayerList const& players = instance->instance->GetPlayers();
                            if (!players.isEmpty())
                                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                                    if (Player* player = itr->GetSource())
                                        DoCast(player, SPELL_CALM_MAELSTROM_SKYBOX, true);
                            events.ScheduleEvent(EVENT_DEATHWING_DESPAWN, 3000);
                            break;
                        }
                        case EVENT_DEATHWING_DESPAWN:
                            DespawnCreatures(NPC_DEATHWING);
                            DespawnCreatures(NPC_LEFT_ARM);
                            DespawnCreatures(NPC_RIGHT_ARM);
                            events.ScheduleEvent(EVENT_DELAY_PLAY_MOVIE, 8000);
                            break;
                        case EVENT_DELAY_PLAY_MOVIE:
                            instance->DoStartMovie(76);
                            if (GameObject* ElemFragment = me->FindNearestGameObject(RAID_MODE(GO_ELEMENTIUM_FRAGMENT_10N, (instance->GetData(DATA_IS_LFR) ? GO_ELEMENTIUM_FRAGMENT_LFR : GO_ELEMENTIUM_FRAGMENT_25N), GO_ELEMENTIUM_FRAGMENT_10H, GO_ELEMENTIUM_FRAGMENT_25H), 300.0f))
                                ElemFragment->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE | GO_FLAG_NOT_SELECTABLE);
                            events.ScheduleEvent(EVENT_FINAL_OUTRO, 500);
                            break;
                        case EVENT_FINAL_OUTRO:
                            me->SummonCreature(NPC_AGGRA, outroPos[0]);
                            me->SummonCreature(NPC_THRALL_3, outroPos[1]);
                            me->SummonCreature(NPC_KALECGOS_2, outroPos[2]);
                            me->SummonCreature(NPC_NOZDORMU_2, outroPos[3]);
                            me->SummonCreature(NPC_ALEXSTRASZA_2, outroPos[4]);
                            me->SummonCreature(NPC_YSERA_2, outroPos[5]);
                            DespawnCreatures(NPC_DRAGON_SOUL_MADNESS);
                            DespawnCreatures(NPC_ALEXSTRASZA_DRAGON);
                            DespawnCreatures(NPC_NOZDORMU_DRAGON);
                            DespawnCreatures(NPC_YSERA_DRAGON);
                            DespawnCreatures(NPC_KALECGOS_DRAGON);
                            me->DespawnOrUnsummon(1000);
                            break;
                    }
                }
            }

            void ResetBattle(bool done = false)
            {
                events.Reset();
                if (!done)
                    m_combatStarted = false;

                if (instance)
                {
                    instance->SetBossState(DATA_MADNESS, (done ? DONE : NOT_STARTED));

                    if (instance->GetBossState(DATA_MADNESS) != DONE)
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_SPELLCLICK);
                }

                DespawnCreatures(NPC_CORRUPTING_PARASITE);
                DespawnCreatures(NPC_WING_TENTACLE);
                DespawnCreatures(NPC_ARM_TENTACLE_1);
                DespawnCreatures(NPC_ARM_TENTACLE_2);
                DespawnCreatures(NPC_MUTATED_CORRUPTION);
                DespawnCreatures(NPC_BLISTERING_TENTACLE);
                DespawnCreatures(NPC_REGENERAIVE_BLOOD);
                DespawnCreatures(NPC_ELEMENTIUM_BOLT);
                DespawnCreatures(NPC_TIME_ZONE);
                DespawnCreatures(NPC_DEATHWING_1);
                DespawnCreatures(NPC_CONGEALING_BLOOD);
                DespawnCreatures(NPC_ELEMENTIUM_FRAGMENT);
                DespawnCreatures(NPC_ELEMENTIUM_TERROR);

                if (instance->GetBossState(DATA_MADNESS) != DONE)
                {
                    events.ScheduleEvent(EVENT_ASPECTS_FACING, 7500);
                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                    {
                        pAspect->Respawn(true);
                        pAspect->GetMotionMaster()->MovePoint(0, alexstraszaPos);
                        pAspect->SetHomePosition(alexstraszaPos);
                        pAspect->AddAura(SPELL_ALEXSTRASZA_PRESENCE, pAspect);
                        pAspect->AddAura(52455, pAspect);
                        pAspect->AI()->DoAction(ACTION_RESET_BATTLE);
                    }
                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                    {
                        pAspect->Respawn(true);
                        pAspect->GetMotionMaster()->MovePoint(0, nozdormuPos);
                        pAspect->SetHomePosition(nozdormuPos);
                        pAspect->AddAura(SPELL_NOZDORMU_PRESENCE, pAspect);
                        pAspect->AddAura(52455, pAspect);
                        pAspect->AI()->DoAction(ACTION_RESET_BATTLE);
                    }
                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                    {
                        pAspect->Respawn(true);
                        pAspect->GetMotionMaster()->MovePoint(0, yseraPos);
                        pAspect->SetHomePosition(yseraPos);
                        pAspect->AddAura(SPELL_YSERA_PRESENCE, pAspect);
                        pAspect->AddAura(SPELL_THE_DREAMER, pAspect);
                        pAspect->AddAura(52455, pAspect);
                        pAspect->AI()->DoAction(ACTION_RESET_BATTLE);
                    }
                    if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                    {
                        pAspect->Respawn(true);
                        pAspect->GetMotionMaster()->MovePoint(0, kalecgosPos);
                        pAspect->SetHomePosition(kalecgosPos);
                        pAspect->AddAura(SPELL_KALECGOS_PRESENCE, pAspect);
                        pAspect->AddAura(SPELL_SPELLWEAVER, pAspect);
                        pAspect->AddAura(52455, pAspect);
                        pAspect->AI()->DoAction(ACTION_RESET_BATTLE);
                    }
                    DespawnCreatures(NPC_DEATHWING);
                    DespawnCreatures(NPC_LEFT_ARM);
                    DespawnCreatures(NPC_RIGHT_ARM);
                    me->NearTeleportTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation());
                    me->UpdateObjectVisibility(true);
                }
            }

        private:
            EventMap events;
            InstanceScript* instance;
            bool chestSpawned;
            bool m_combatStarted;

            void DespawnCreatures(uint32 entry)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                     (*iter)->DespawnOrUnsummon(2000);
            }

            bool CheckPlayers()
            {
                Player* player = NULL;
                AnyLivePlayerNoGmCheck check(me, 500.0f);
                Trinity::PlayerSearcher<AnyLivePlayerNoGmCheck> searcher(me, player, check);
                me->VisitNearbyWorldObject(500.0f, searcher);
                return (player ? true : false);
            }

            class AnyLivePlayerNoGmCheck
            {
                public:
                    AnyLivePlayerNoGmCheck(WorldObject const* obj, float range) : _obj(obj), _range(range) { }
                    bool operator()(Player* u)
                    {
                        if (!u->IsAlive())
                            return false;

                        if (!_obj->IsWithinDistInMap(u, _range))
                            return false;

                        if (u->IsGameMaster())
                            return false;

                        return true;
                    }

                private:
                    WorldObject const* _obj;
                    float _range;
            };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dragon_soul_thrall_1AI>(creature);
        }
};

class npc_madness_of_deathwing_mutated_corruption : public CreatureScript
{
    public:
        npc_madness_of_deathwing_mutated_corruption() : CreatureScript("npc_madness_of_deathwing_mutated_corruption") { }

        struct npc_madness_of_deathwing_mutated_corruptionAI : public ScriptedAI
        {
            npc_madness_of_deathwing_mutated_corruptionAI(Creature* creature) : ScriptedAI(creature)
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
                
                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                SetCombatMovement(false);

                instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_CRUSH)
                    events.ScheduleEvent(EVENT_CRUSH_1, 500);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                m_first = true;
                events.ScheduleEvent(EVENT_CRUSH, urand(10000, 15000));
                events.ScheduleEvent(EVENT_IMPALE, 12000);
                events.ScheduleEvent(EVENT_SELECT_VICTIM, 5000);
            }
            
            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon(2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CRUSH:
                            DoCastAOE(SPELL_CRUSH_FORCE, true);
                            events.ScheduleEvent(EVENT_CRUSH, 8000);
                            break;
                        case EVENT_IMPALE:
                            DoCastVictim(SPELL_IMPALE);
                            events.ScheduleEvent(EVENT_IMPALE, 35000);
                            break;
                        case EVENT_SELECT_VICTIM:
                        {
                            Unit* pNearest = NULL;
                            if (!me->GetVictim() || !me->IsWithinMeleeRange(me->GetVictim()))
                            {
                                pNearest = SelectTarget(SELECT_TARGET_NEAREST, 0, 0.0f, true);

                                // Cast Impale Aspect
                                if (!pNearest || !me->IsWithinMeleeRange(pNearest))
                                {
                                    if (instance)
                                    {
                                        if (Creature* pDeathwing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DEATHWING)))
                                        {
                                            Unit* pAspect = NULL;
                                            uint8 platform = pDeathwing->AI()->GetData(DATA_CURRENT_PLATFORM);
                                            
                                            switch (platform)
                                            {
                                                case 1: 
                                                    pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON));
                                                    break;
                                                case 2: 
                                                    pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON));
                                                    break;
                                                case 3: 
                                                    pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON));
                                                    break;
                                                case 4: 
                                                    pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON));
                                                    break;
                                            }

                                            if (pAspect)
                                            {
                                                if (m_first)
                                                {
                                                    pDeathwing->AI()->Talk(ANN_IMPALE_ASPECT);
                                                    m_first = false;
                                                }
                                                DoCast(pAspect, SPELL_IMPALE_ASPECT);
                                            }
                                        }
                                    }
                                }
                                else
                                    me->Attack(pNearest, true);
                            }

                            events.ScheduleEvent(EVENT_SELECT_VICTIM, 2000);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance;
            EventMap events;
            bool m_first;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_mutated_corruptionAI>(creature);
        }
};

class npc_madness_of_deathwing_crush_target : public CreatureScript
{
    public:
        npc_madness_of_deathwing_crush_target() : CreatureScript("npc_madness_of_deathwing_crush_target") { }

        struct npc_madness_of_deathwing_crush_targetAI : public ScriptedAI
        {
            npc_madness_of_deathwing_crush_targetAI(Creature* creature) : ScriptedAI(creature)
            { 
                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (Creature* pCorruption = me->FindNearestCreature(NPC_MUTATED_CORRUPTION, 80.0f))
                {
                    pCorruption->SetFacingToObject(me);
                    pCorruption->CastSpell(me, SPELL_CRUSH);
               }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_crush_targetAI>(creature);
        }
};

class npc_madness_of_deathwing_limb_tentacle : public CreatureScript
{
    public:
        npc_madness_of_deathwing_limb_tentacle() : CreatureScript("npc_madness_of_deathwing_limb_tentacle") { }

        struct npc_madness_of_deathwing_limb_tentacleAI : public ScriptedAI
        {
            npc_madness_of_deathwing_limb_tentacleAI(Creature* creature) : ScriptedAI(creature)
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
                SetCombatMovement(false);

                phase = 0;
                tentacles = 0;
                m_current_platform = 0;

                instance = me->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                phase = 0;
                tentacles = 0;
                m_current_platform = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                phase = 0;
                tentacles = 0;
                m_current_platform = 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_CURRENT_PLATFORM)
                    m_current_platform = data;
            }

            void JustSummoned(Creature* summon) override
            {
                DoZoneInCombat(summon);

                if (summon->GetEntry() == NPC_BLISTERING_TENTACLE)
                    tentacles++;
                else if (summon->GetEntry() == NPC_HEMORRHAGE_TARGET)
                    summon->CastSpell(summon, SPELL_HEMORRHAGE_AURA, true);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_BLISTERING_TENTACLE)
                    tentacles--;
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_ASSAULT_ASPECTS_FIRST)
                {
                    events.ScheduleEvent(EVENT_ASSAULT_ASPECTS, 100);
                    events.ScheduleEvent(EVENT_UPDATE_HEALTH, 2000);
                    events.ScheduleEvent(EVENT_HEMORRHAGE, IsHeroic() ? (55500 - 10000) : (85500 - 10000));
                    if (IsHeroic())
                    {
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE, 12000 - 10000);
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE, 72000 - 10000);
                    }
                }
                else if (action == ACTION_ASSAULT_ASPECTS)
                {
                    events.ScheduleEvent(EVENT_ASSAULT_ASPECTS, 100);
                    events.ScheduleEvent(EVENT_UPDATE_HEALTH, 2000);
                    events.ScheduleEvent(EVENT_HEMORRHAGE, IsHeroic() ? (70500 - 10000) : (100500 - 10000));
                    if (IsHeroic())
                    {
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE, 22000 - 10000);
                        events.ScheduleEvent(EVENT_CORRUPTING_PARASITE, 82000 - 10000);
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();

                if (instance && m_current_platform)
                {
                    if (Creature* pDeathwing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DEATHWING)))
                        pDeathwing->AI()->SetData(DATA_DESTROY_PLATFORM, m_current_platform);

                    switch (m_current_platform)
                    {
                        case 1:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                DoCast(pAspect, SPELL_TRIGGER_CONCENTRATION, true);
                            break;
                        case 2:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                                DoCast(pAspect, SPELL_TRIGGER_CONCENTRATION, true);
                            break;
                        case 3:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                                DoCast(pAspect, SPELL_TRIGGER_CONCENTRATION, true);
                            break;
                        case 4:
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                                DoCast(pAspect, SPELL_TRIGGER_CONCENTRATION, true);
                            break;
                    }
                }

                DoCastAOE(SPELL_AGONIZING_PAIN, true);

                DespawnCreatures(NPC_TIME_ZONE);
                DespawnCreatures(NPC_TIME_ZONE_TARGET);
                DespawnCreatures(NPC_BLISTERING_TENTACLE);

                me->DespawnOrUnsummon(2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if ((me->GetHealthPct() <= 70 && phase == 0) ||
                    (me->GetHealthPct() <= 35 && phase == 1))
                {
                    phase++;
                    if (tentacles < 5)
                    {
                        Talk(ANN_BLIST_TENTACLES);
                        uint8 need_tentacles = 5 - tentacles;
                        for (uint8 i = 0; i < need_tentacles; ++i)
                            me->SummonCreature(NPC_BLISTERING_TENTACLE, blisteringPos[m_current_platform]);

                        if (instance)
                            if (Creature* pAspect = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                                pAspect->AI()->DoAction(ACTION_CAUTERIZE);
                            
                    }
                    return;
                }

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ASSAULT_ASPECTS:
                            //DoCast(me, SPELL_BURNING_BLOOD, true);
                            break;
                        case EVENT_HEMORRHAGE:
                            Talk(ANN_HEMMORHAGE);
                            if (m_current_platform > 0)
                                me->SummonCreature(NPC_HEMORRHAGE_TARGET, hemorrhagePos[m_current_platform - 1], TEMPSUMMON_TIMED_DESPAWN, 10000ms);
                            break;
                        case EVENT_UPDATE_HEALTH:
                        {
                            uint8 newstack = 100 - me->GetHealthPct();
                            if (newstack > 100) newstack = 100;
                            if (newstack < 1) newstack = 1;
                            if (Aura* aur = me->GetAura(SPELL_BURNING_BLOOD))
                            {
                                if (aur->GetStackAmount() != newstack)
                                    aur->SetStackAmount(newstack);
                            }
                            else if (newstack >= 10)
                            {
                                DoCast(me, SPELL_BURNING_BLOOD, true);
                                if (Aura* aur = me->GetAura(SPELL_BURNING_BLOOD))
                                    aur->SetStackAmount(newstack);
                            }
                            events.ScheduleEvent(EVENT_UPDATE_HEALTH, 3000);
                            break;
                        }
                        case EVENT_CORRUPTING_PARASITE:
                            me->CastCustomSpell(SPELL_CORRUPTING_PARASITE_AOE, SPELLVALUE_MAX_TARGETS, 1, NULL, true);
                            break;
                    }
                }
            }

        private:
            InstanceScript* instance;
            EventMap events;
            uint8 phase;
            uint8 tentacles;
            uint8 m_current_platform;

            void DespawnCreatures(uint32 entry)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                     (*iter)->DespawnOrUnsummon(2000);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_limb_tentacleAI>(creature);
        }
};

class npc_madness_of_deathwing_blistering_tentacle : public CreatureScript
{
    public:
        npc_madness_of_deathwing_blistering_tentacle() : CreatureScript("npc_madness_of_deathwing_blistering_tentacle") { }

        struct npc_madness_of_deathwing_blistering_tentacleAI : public ScriptedAI
        {
            npc_madness_of_deathwing_blistering_tentacleAI(Creature* creature) : ScriptedAI(creature)
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
                SetCombatMovement(false);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 50.0f);
                
                if (summoner->GetEntry() == NPC_WING_TENTACLE ||
                    summoner->GetEntry() == NPC_ARM_TENTACLE_1 ||
                    summoner->GetEntry() == NPC_ARM_TENTACLE_2)
                {
                    DoCast(summoner, SPELL_BLISTERING_TENTACLE_VEHICLE, true);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon(500);
            }

            SpellMissInfo SpellHitResult(Unit* attacker, SpellInfo const* spell, Spell const* spellInstance) override
            {
                return (attacker && attacker->GetEntry() == NPC_ALEXSTRASZA_DRAGON || !spellInstance || spellInstance && spellInstance->m_targets.GetUnitTarget() == me) ? SPELL_MISS_NONE : SPELL_MISS_MISS;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_blistering_tentacleAI>(creature);
        }
};

class npc_madness_of_deathwing_regenerative_blood : public CreatureScript
{
    public:
        npc_madness_of_deathwing_regenerative_blood() : CreatureScript("npc_madness_of_deathwing_regenerative_blood") { }

        struct npc_madness_of_deathwing_regenerative_bloodAI : public ScriptedAI
        {
            npc_madness_of_deathwing_regenerative_bloodAI(Creature* creature) : ScriptedAI(creature)
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

                me->setActive(true);

                me->SetReactState(REACT_PASSIVE);

                platform = GetGround(me);
            }

            void Reset() override
            {
                events.Reset();
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 10);
                me->SetPower(POWER_ENERGY, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_UPDATE_HEALTH, 1000);
                events.ScheduleEvent(EVENT_START_MOVE, 1000);
            }

            void AttackStart(Unit* target) override
            {
                if (!target)
                    return;

                if (platform && platform != GetGround(target))
                    me->Attack(target, false);
                else
                {
                    if (me->Attack(target, true))
                        me->GetMotionMaster()->MoveChase(target);
                }
            }
            
            void JustDied(Unit* /*killer*/) override
            {   
                me->DespawnOrUnsummon(2000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_START_MOVE:
                            me->SetReactState(REACT_AGGRESSIVE);
                            break;
                        case EVENT_UPDATE_HEALTH:
                        {
                            int32 val = me->GetPower(POWER_ENERGY);
                            val++;
                            if (val > 9)
                            {
                                me->SetPower(POWER_ENERGY, 0);
                                if (me->IsSpawnedInLFRDungeon())
                                {
                                    int32 bp0 = 50;
                                    me->CastCustomSpell(me, SPELL_REGENERATIVE_BLOOD_HEAL, &bp0, NULL, NULL, true);
                                }
                                else
                                    DoCast(me, SPELL_REGENERATIVE_BLOOD_HEAL, true);
                            }
                            else
                                me->SetPower(POWER_ENERGY, val);

                            events.ScheduleEvent(EVENT_UPDATE_HEALTH, 1000);
                            break;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
            uint8 platform;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_regenerative_bloodAI>(creature);
        }
};

class npc_madness_of_deathwing_elementium_bolt : public CreatureScript
{
    public:
        npc_madness_of_deathwing_elementium_bolt() : CreatureScript("npc_madness_of_deathwing_elementium_bolt") { }

        struct npc_madness_of_deathwing_elementium_boltAI : public ScriptedAI
        {
            npc_madness_of_deathwing_elementium_boltAI(Creature* creature) : ScriptedAI(creature)
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_SPEED_ALWAYS, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_SPEED_NOT_STACK, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_SPEED_SLOW_ALL, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_INCREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_DECREASE_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACK, true);

                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->setActive(true);
                me->SetSpeed(MOVE_FLIGHT, 4.5f, true);
                me->SetSpeed(MOVE_WALK, 50.f, true);
                me->SetSpeed(MOVE_RUN, 50.f, true);

                me->SetReactState(REACT_PASSIVE);

                platform = 0;
                isSlow = false;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (!summoner || !summoner->ToCreature())
                    return;

                platform = summoner->ToCreature()->AI()->GetData(DATA_CURRENT_PLATFORM);
                events.Reset();
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (!platform)
                    return;

                if (spell->Id == SPELL_ELEMENTIUM_METEOR_SCRIPT)
                {
                    // not offlike !!!
                    //if (Creature* pTrigger = me->SummonCreature(NPC_CLAWK_MARK, boltPos[platform], TEMPSUMMON_TIMED_DESPAWN, 25000ms))
                    //    pTrigger->AddAura(SPELL_ELEMENTIUM_METEOR_TARGET, pTrigger);
                    if (Creature* pTrigger = me->SummonCreature(NPC_PLATFORM, boltPos[platform], TEMPSUMMON_TIMED_DESPAWN, 25000ms))
                        pTrigger->AddAura(SPELL_ELEMENTIUM_METEOR_TARGET, pTrigger);

                    me->GetMotionMaster()->MovePoint(DATA_METEOR_POINT, boltPos[platform]);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE)
                    if (pointId == DATA_METEOR_POINT)
                    {
                        DoCastAOE(SPELL_ELEMENTIUM_BLAST, true);
                        DoCast(me, SPELL_ELEMENTIUM_METEOR_AURA, true);
                        events.ScheduleEvent(EVENT_ELEMENTIUM_BOLT, 5000);
                        me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 0.39f);
                        me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 1.f);
                    }
            }

            void JustDied(Unit* /*killer*/) override
            {
                events.Reset();
                me->DespawnOrUnsummon(1000);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_ELEMENTIUM_BOLT:
                        DoCastAOE(SPELL_ELEMENTIUM_BLAST, true);
                        events.ScheduleEvent(EVENT_ELEMENTIUM_BOLT, 5000);
                        break;
                    default:
                        break;
                }

                if (!isSlow && me->FindNearestCreature(NPC_TIME_ZONE, 15.0f, true))
                {
                    isSlow = true;
                    me->SetSpeed(MOVE_FLIGHT, 0.225f, true);
                    me->SetSpeed(MOVE_WALK, 2.5f, true);
                    me->SetSpeed(MOVE_RUN, 2.5f, true);
                }
                else if (isSlow && !me->FindNearestCreature(NPC_TIME_ZONE, 15.0f, true))
                {
                    me->SetSpeed(MOVE_FLIGHT, 4.5f, true);
                    me->SetSpeed(MOVE_WALK, 50.f, true);
                    me->SetSpeed(MOVE_RUN, 50.f, true);
                }
            }

        private:
            EventMap events;
            uint8 platform;
            bool isSlow;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_elementium_boltAI>(creature);
        }
};

class npc_madness_of_deathwing_corrupting_parasite : public CreatureScript
{
    public:
        npc_madness_of_deathwing_corrupting_parasite() : CreatureScript("npc_madness_of_deathwing_corrupting_parasite") { }

        struct npc_madness_of_deathwing_corrupting_parasiteAI : public ScriptedAI
        {
            npc_madness_of_deathwing_corrupting_parasiteAI(Creature* creature) : ScriptedAI(creature)
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
                
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                events.Reset();
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_PARASITIC_BACKSLASH)
                {
                    if (InstanceScript* instance = me->GetInstanceScript())
                        if (Creature* alexstrasza = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                            alexstrasza->AI()->DoAction(ACTION_CAUTERIZE_PARASITE);
                        me->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE, -70.0f, false);
                    events.ScheduleEvent(EVENT_UNSTABLE_CORRUPTION, 200);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_UNSTABLE_CORRUPTION)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        DoCastAOE(SPELL_UNSTABLE_CORRUPTION);
                    }
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->DespawnOrUnsummon(1000);
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_corrupting_parasiteAI>(creature);
        }
};

class npc_madness_of_deathwing_deathwing : public CreatureScript
{
    public:
        npc_madness_of_deathwing_deathwing() : CreatureScript("npc_madness_of_deathwing_deathwing") { }

        struct npc_madness_of_deathwing_deathwingAI : public ScriptedAI
        {
            npc_madness_of_deathwing_deathwingAI(Creature* creature) : ScriptedAI(creature)
            {             
                me->SetDisableGravity(true);

                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);

                instance = me->GetInstanceScript();

                isDead = false;

                m_congealingBloodSpawned[0] = false;
                m_congealingBloodSpawned[1] = false;
                m_congealingBloodSpawned[2] = false;
            }

            void Reset() override
            {
                m_congealingBloodSpawned[0] = false;
                m_congealingBloodSpawned[1] = false;
                m_congealingBloodSpawned[2] = false;
                m_congealingBloodSpawnsLeft = 0;
                m_congealingBloodSpawnTargetGuid = ObjectGuid::Empty;
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ELEMENTIUM_FRAGMENT, 10500);
                events.ScheduleEvent(EVENT_ELEMENTIUM_TERROR, 35500);
                events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 1000);
                events.ScheduleEvent(EVENT_TIME_ZONE, 15000);
                DoCastAOE(SPELL_CORRUPTED_BLOOD_STACKER);
                if (Creature* dragon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                    dragon->CastSpell(dragon, SPELL_ALEXSTRASZA_PRESENCE);
                if (Creature* dragon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                    dragon->CastSpell(dragon, SPELL_NOZDORMU_PRESENCE);
                if (Creature* dragon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_YSERA_DRAGON)))
                {
                    dragon->CastSpell(dragon, SPELL_YSERA_PRESENCE);
                    dragon->CastSpell(dragon, SPELL_THE_DREAMER);
                }
                if (Creature* dragon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_KALECGOS_DRAGON)))
                {
                    dragon->CastSpell(dragon, SPELL_KALECGOS_PRESENCE);
                    dragon->CastSpell(dragon, SPELL_SPELLWEAVER);
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (me->HasAura(SPELL_BERSERK))
                    summon->CastSpell(summon, SPELL_BERSERK, true);
            }

            SpellMissInfo SpellHitResult(Unit* attacker, SpellInfo const* spell, Spell const* /*spellInstance*/) override
            {
                return attacker && attacker->GetCharmerOrOwnerPlayerOrPlayerItself() && GetGround(attacker) != 3 ? SPELL_MISS_EVADE : SPELL_MISS_NONE;
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (attacker && GetGround(attacker) != 3)
                {
                    damage = 0;
                    return;
                }
                if (!isDead)
                {
                    if (me->GetHealth() <= damage)
                    {
                        isDead = true;

                        if (instance)
                        {
                            me->GetMap()->SetWorldState(WORLDSTATE_DEATHWING_MADNESS, 1);
                            instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_DEATHWING_CREDIT, me);
                            instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_DEATHWING_CREDIT);
                            instance->DoFinishLFGDungeon(417);
                        }

                        if (Creature* pThrall = me->FindNearestCreature(NPC_THRALL_2, 500.0f))
                            pThrall->AI()->DoAction(ACTION_END_BATTLE);
                        switch (GetDifficulty())
                        {
                            case RAID_DIFFICULTY_10MAN_NORMAL:
                                instance->DoRespawnGameObject(instance->GetGuidData(DATA_ELEM_FRAGMENT_10N), DAY);
                                break;
                            case RAID_DIFFICULTY_25MAN_NORMAL:
                                instance->DoRespawnGameObject(instance->GetGuidData(DATA_ELEM_FRAGMENT_25N), DAY);
                                break;
                            case RAID_DIFFICULTY_10MAN_HEROIC:
                                instance->DoRespawnGameObject(instance->GetGuidData(DATA_ELEM_FRAGMENT_10H), DAY);
                                break;
                            case RAID_DIFFICULTY_25MAN_HEROIC:
                                instance->DoRespawnGameObject(instance->GetGuidData(DATA_ELEM_FRAGMENT_25H), DAY);
                                break;
                        }
                        if (instance->GetData(DATA_IS_LFR))
                        {
                            Map::PlayerList const& players = instance->instance->GetPlayers();
                            if (!players.isEmpty())
                                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                                    if (Player* player = itr->GetSource())
                                        player->AddLootLockout(LootLockoutType::PersonalLoot, GO_ELEMENTIUM_FRAGMENT_LFR, RAID_DIFFICULTY_25MAN_LFR);
                        }
                        return;
                    }
                    if (!m_congealingBloodSpawned[0] && int64(me->GetHealth()) - damage < ceil(me->GetMaxHealth() * 0.05f))
                    {
                        m_congealingBloodSpawned[0] = true;
                        SpawnCongealingBlood();
                        me->ModifyPower(POWER_ALTERNATE_POWER, 15);
                        if (Creature* alexstrasza = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                            alexstrasza->CastSpell(me, SPELL_CAUTERIZE_2_3, true);
                    }
                    if (!m_congealingBloodSpawned[1] && int64(me->GetHealth()) - damage < ceil(me->GetMaxHealth() * 0.10f))
                    {
                        m_congealingBloodSpawned[1] = true;
                        SpawnCongealingBlood();
                        me->ModifyPower(POWER_ALTERNATE_POWER, 10);
                        if (Creature* alexstrasza = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                            alexstrasza->CastSpell(me, SPELL_CAUTERIZE_2_2, true);
                    }
                    if (!m_congealingBloodSpawned[2] && int64(me->GetHealth()) - damage < ceil(me->GetMaxHealth() * 0.15f))
                    {
                        m_congealingBloodSpawned[2] = true;
                        SpawnCongealingBlood();
                        me->ModifyPower(POWER_ALTERNATE_POWER, 5);
                        if (Creature* alexstrasza = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_ALEXSTRASZA_DRAGON)))
                            alexstrasza->CastSpell(me, SPELL_CAUTERIZE_2_1, true);
                    }
                }
            }

            void HealReceived(Unit* /*healer*/, uint32& heal) override
            {
                if (!isDead)
                {
                    if (m_congealingBloodSpawned[0] && int64(me->GetHealth()) + heal > ceil(me->GetMaxHealth() * 0.05f))
                        m_congealingBloodSpawned[0] = false;
                    if (m_congealingBloodSpawned[1] && int64(me->GetHealth()) + heal > ceil(me->GetMaxHealth() * 0.10f))
                        m_congealingBloodSpawned[1] = false;
                    if (m_congealingBloodSpawned[2] && int64(me->GetHealth()) + heal > ceil(me->GetMaxHealth() * 0.15f))
                        m_congealingBloodSpawned[2] = false;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_CONGEALING_BLOOD:
                        if (m_congealingBloodSpawnsLeft)
                        {
                            if (Creature* target = instance->instance->GetCreature(m_congealingBloodSpawnTargetGuid))
                                DoCast(target, SPELL_CONGEALING_BLOOD_SUMMON, true);
                            if (--m_congealingBloodSpawnsLeft)
                                events.ScheduleEvent(EVENT_CONGEALING_BLOOD, 500);
                        }
                        break;
                    case EVENT_ELEMENTIUM_FRAGMENT:
                        for (uint8 i = 0; i < RAID_MODE(3, 8); ++i)
                            DoCastAOE(SPELL_ELEMENTIUM_FRAGMENT_SUMMON, true);
                        me->RemoveAurasDueToSpell(SPELL_SHRAPNEL_TRIGGER_AURA);
                        DoCastAOE(SPELL_SHRAPNEL_TRIGGER_AURA);
                        events.ScheduleEvent(EVENT_ELEMENTIUM_FRAGMENT, 90000);
                        break;
                    case EVENT_ELEMENTIUM_TERROR:
                        for (uint8 i = 0; i < 2; ++i)
                            DoCastAOE(SPELL_ELEMENTIUM_TERROR_SUMMON, true);
                        events.ScheduleEvent(EVENT_ELEMENTIUM_TERROR, 90000);
                        break;
                    case EVENT_CORRUPTED_BLOOD:
                        if (me->GetHealthPct() < 20 && me->GetPower(POWER_ALTERNATE_POWER) < 20 - ceil(me->GetHealthPct()))
                            me->ModifyPower(POWER_ALTERNATE_POWER, 1);
                        events.ScheduleEvent(EVENT_CORRUPTED_BLOOD, 1000);
                        break;
                    case EVENT_TIME_ZONE:
                        if (Creature* nozdormu = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_NOZDORMU_DRAGON)))
                            nozdormu->CastSpell(timezonePos[4].GetPositionX(), timezonePos[4].GetPositionY(), timezonePos[4].GetPositionZ(), SPELL_TIME_ZONE_MISSILE_2, true);
                        events.ScheduleEvent(EVENT_TIME_ZONE, urand(60000, 90000));
                        break;
                    default:
                        break;
                }
            }

        private:
            InstanceScript* instance;
            EventMap events;
            bool m_congealingBloodSpawned[3];
            ObjectGuid m_congealingBloodSpawnTargetGuid;
            uint32 m_congealingBloodSpawnsLeft;

            bool isDead;

            void SpawnCongealingBlood()
            {
                if (!IsHeroic())
                    return;
                std::list<Creature*> targets;
                GetCreatureListWithEntryInGrid(targets, me, NPC_CONGEALING_BLOOD_T, 100);
                DistanceCheck check(me);
                targets.remove_if(check);
                if (targets.empty())
                    return;
                Trinity::RandomResizeList(targets, 1);
                m_congealingBloodSpawnTargetGuid = (*targets.begin())->GetGUID();
                m_congealingBloodSpawnsLeft += 10;
                events.CancelEvent(EVENT_CONGEALING_BLOOD);
                events.ScheduleEvent(EVENT_CONGEALING_BLOOD, 1);
            }

            class DistanceCheck
            {
                public:
                    DistanceCheck(Creature* obj) : _obj(obj) { }
                    bool operator()(Creature* u) { return _obj->GetExactDist(u) < 40; }

                private:
                    Creature* _obj;
            };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_deathwingAI>(creature);
        }
};

class npc_madness_of_deathwing_congealing_blood : public CreatureScript
{
    public:
        npc_madness_of_deathwing_congealing_blood() : CreatureScript("npc_madness_of_deathwing_congealing_blood") { }

        struct npc_madness_of_deathwing_congealing_bloodAI : public ScriptedAI
        {
            npc_madness_of_deathwing_congealing_bloodAI(Creature* creature) : ScriptedAI(creature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
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

                me->setActive(true);

                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* summoner) override
            {
                if (Creature* target = GetClosestCreatureWithEntry(summoner, NPC_CONGEALING_BLOOD_T, 40))
                {
                    Position pos = target->GetPosition();
                    me->GetMotionMaster()->MovePoint(POINT_CONGEALING_BLOOD_HEAL, pos);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == POINT_CONGEALING_BLOOD_HEAL)
                {
                    DoCastAOE(SPELL_CONGEALING_BLOOD_HEAL);
                    me->Kill(me);
                }
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };
        
        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_congealing_bloodAI>(creature);
        }
};

class npc_madness_of_deathwing_elementium_fragment : public CreatureScript
{
    public:
        npc_madness_of_deathwing_elementium_fragment() : CreatureScript("npc_madness_of_deathwing_elementium_fragment") { }

        struct npc_madness_of_deathwing_elementium_fragmentAI : public ScriptedAI
        {
            npc_madness_of_deathwing_elementium_fragmentAI(Creature* creature) : ScriptedAI(creature)
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

                me->setActive(true);
                SetCombatMovement(false);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_elementium_fragmentAI>(creature);
        }
};

class npc_madness_of_deathwing_elementium_terror : public CreatureScript
{
    public:
        npc_madness_of_deathwing_elementium_terror() : CreatureScript("npc_madness_of_deathwing_elementium_terror") { }

        struct npc_madness_of_deathwing_elementium_terrorAI : public ScriptedAI
        {
            npc_madness_of_deathwing_elementium_terrorAI(Creature* creature) : ScriptedAI(creature)
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

                me->setActive(true);
            }

            void Reset() override
            {
                if (Unit* target = me->SelectNearestTarget(100))
                    AttackStart(target);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_elementium_terrorAI>(creature);
        }
};

class npc_dragon_soul_alexstrasza_dragon : public CreatureScript
{
    public:
        npc_dragon_soul_alexstrasza_dragon() : CreatureScript("npc_dragon_soul_alexstrasza_dragon") { }

        struct npc_dragon_soul_alexstrasza_dragonAI : public ScriptedAI
        {
            npc_dragon_soul_alexstrasza_dragonAI(Creature* creature) : ScriptedAI(creature)
            {             
                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetWalk(false);
                me->SetSpeed(MOVE_FLIGHT, 3, true);

                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);

                isActive = true;
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                return;
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_TRIGGER_CONCENTRATION)
                {
                    isActive = false;
                    events.Reset();
                    me->RemoveAura(SPELL_ALEXSTRASZA_PRESENCE);
                    me->InterruptNonMeleeSpells(true);
                    DoCast(SPELL_CONCENTRATION_1);                    
                }
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_RESET_BATTLE)
                    isActive = true;
                if (isActive && action == ACTION_CAUTERIZE)
                    events.ScheduleEvent(EVENT_CAUTERIZE, 2000);
                if (isActive && action == ACTION_CAUTERIZE_PARASITE)
                    DoCast(me, SPELL_CAUTERIZE_3, true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* pThrall = me->FindNearestCreature(NPC_THRALL_2, 500.0f))
                    pThrall->AI()->DoAction(ACTION_RESET_BATTLE);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CAUTERIZE:
                            DoCast(me, SPELL_CAUTERIZE_1, true);
                            break;
                    }
                }
            }

        private:
            EventMap events;
            bool isActive;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dragon_soul_alexstrasza_dragonAI>(creature);
        }
};

class npc_dragon_soul_nozdormu_dragon : public CreatureScript
{
    public:
        npc_dragon_soul_nozdormu_dragon() : CreatureScript("npc_dragon_soul_nozdormu_dragon") { }

        struct npc_dragon_soul_nozdormu_dragonAI : public ScriptedAI
        {
            npc_dragon_soul_nozdormu_dragonAI(Creature* creature) : ScriptedAI(creature)
            {             
                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetWalk(false);
                me->SetSpeed(MOVE_FLIGHT, 3, true);

                me->SetReactState(REACT_PASSIVE);

                instance = me->GetInstanceScript();

                isActive = true;
                SetCombatMovement(false);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                return;
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_TRIGGER_CONCENTRATION)
                {
                    isActive = false;
                    events.Reset();
                    me->RemoveAura(SPELL_NOZDORMU_PRESENCE);
                    me->InterruptNonMeleeSpells(true);
                    DoCast(SPELL_CONCENTRATION_2);                    
                }
            }

            void DoAction(int32 action) override
            {
                if (action == ACTION_RESET_BATTLE)
                    isActive = true;
                if (isActive && action == ACTION_TIME_ZONE)
                    events.ScheduleEvent(EVENT_TIME_ZONE, urand(2000, 6000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* pThrall = me->FindNearestCreature(NPC_THRALL_2, 500.0f))
                    pThrall->AI()->DoAction(ACTION_RESET_BATTLE);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TIME_ZONE:
                            if (Creature* pDeathwing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_DEATHWING)))
                            {
                                uint8 platform = pDeathwing->AI()->GetData(DATA_CURRENT_PLATFORM);
                                if (platform < 1)
                                    break;

                                me->CastSpell(timezonePos[platform - 1].GetPositionX(), timezonePos[platform - 1].GetPositionY(), timezonePos[platform - 1].GetPositionZ(), SPELL_TIME_ZONE_MISSILE_1, true);
                            }
                            break;
                    }
                }
            }

        private:
            InstanceScript* instance;
            EventMap events;
            bool isActive;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dragon_soul_nozdormu_dragonAI>(creature);
        }
};

class npc_dragon_soul_ysera_dragon : public CreatureScript
{
    public:
        npc_dragon_soul_ysera_dragon() : CreatureScript("npc_dragon_soul_ysera_dragon") { }

        struct npc_dragon_soul_ysera_dragonAI : public ScriptedAI
        {
            npc_dragon_soul_ysera_dragonAI(Creature* creature) : ScriptedAI(creature)
            {             
                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetWalk(false);
                me->SetSpeed(MOVE_FLIGHT, 3, true);

                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                return;
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_TRIGGER_CONCENTRATION)
                {
                    me->RemoveAura(SPELL_YSERA_PRESENCE);
                    me->RemoveAura(SPELL_THE_DREAMER);
                    me->InterruptNonMeleeSpells(true);
                    DoCast(SPELL_CONCENTRATION_3);                    
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* pThrall = me->FindNearestCreature(NPC_THRALL_2, 500.0f))
                    pThrall->AI()->DoAction(ACTION_RESET_BATTLE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dragon_soul_ysera_dragonAI>(creature);
        }
};

class npc_dragon_soul_kalecgos_dragon : public CreatureScript
{
    public:
        npc_dragon_soul_kalecgos_dragon() : CreatureScript("npc_dragon_soul_kalecgos_dragon") { }

        struct npc_dragon_soul_kalecgos_dragonAI : public ScriptedAI
        {
            npc_dragon_soul_kalecgos_dragonAI(Creature* creature) : ScriptedAI(creature)
            {             
                me->setActive(true);
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetWalk(false);
                me->SetSpeed(MOVE_FLIGHT, 3, true);

                me->SetReactState(REACT_PASSIVE);
                SetCombatMovement(false);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                return;
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_TRIGGER_CONCENTRATION)
                {
                    me->RemoveAura(SPELL_KALECGOS_PRESENCE);
                    me->RemoveAura(SPELL_SPELLWEAVER);
                    me->InterruptNonMeleeSpells(true);
                    DoCast(SPELL_CONCENTRATION_4);                    
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* pThrall = me->FindNearestCreature(NPC_THRALL_2, 500.0f))
                    pThrall->AI()->DoAction(ACTION_RESET_BATTLE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_dragon_soul_kalecgos_dragonAI>(creature);
        }
};

class npc_madness_of_deathwing_jump_pad : public CreatureScript
{
    public:
        npc_madness_of_deathwing_jump_pad() : CreatureScript("npc_madness_of_deathwing_jump_pad") { }

        struct npc_madness_of_deathwing_jump_padAI : public ScriptedAI
        {
            npc_madness_of_deathwing_jump_padAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                checkTimer = 500;
                pos = 0;
                spellId = 0;
                spellIdEx1 = 0;
                spellIdEx2 = 0;
                otherJumpPadGuid = ObjectGuid::Empty;
                SetCombatMovement(false);
            }

            void Reset() override
            {
                for (uint8 i = 0; i < 6; ++i)
                {
                    if (me->GetExactDist(&jumpPos[i]) < 5.0f)
                    {
                        pos = i + 1;
                        switch (pos)
                        {
                            case 1:
                                spellId = SPELL_CARRYING_WINDS_SCRIPT_1;
                                spellIdEx1 = SPELL_CARRYING_WINDS_1;
                                spellIdEx2 = SPELL_CARRYING_WINDS_2;
                                break;
                            case 2:
                                spellId = SPELL_CARRYING_WINDS_SCRIPT_2;
                                spellIdEx1 = SPELL_CARRYING_WINDS_2;
                                spellIdEx2 = SPELL_CARRYING_WINDS_1;
                                break;
                            case 3:
                                spellId = SPELL_CARRYING_WINDS_SCRIPT_3;
                                spellIdEx1 = SPELL_CARRYING_WINDS_3;
                                spellIdEx2 = SPELL_CARRYING_WINDS_4;
                                break;
                            case 4:
                                spellId = SPELL_CARRYING_WINDS_SCRIPT_4;
                                spellIdEx1 = SPELL_CARRYING_WINDS_4;
                                spellIdEx2 = SPELL_CARRYING_WINDS_3;
                                break;
                            case 5:
                                spellId = SPELL_CARRYING_WINDS_SCRIPT_5;
                                spellIdEx1 = SPELL_CARRYING_WINDS_5;
                                spellIdEx2 = SPELL_CARRYING_WINDS_6;
                                break;
                            case 6:
                                spellId = SPELL_CARRYING_WINDS_SCRIPT_6;
                                spellIdEx1 = SPELL_CARRYING_WINDS_6;
                                spellIdEx2 = SPELL_CARRYING_WINDS_5;
                                break;
                        }
                        break;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!spellId)
                    return;

                if (checkTimer <= diff)
                {
                    if (!otherJumpPadGuid)
                    {
                        Creature* otherJumpPad = NULL;
                        OtherJumpPadCheck otherJumpPadCheck((pos % 2) == 1 ? pos : pos - 2);
                        Trinity::CreatureSearcher<OtherJumpPadCheck> jumpPadSearcher(me, otherJumpPad, otherJumpPadCheck);
                        me->VisitNearbyObject(100.0f, jumpPadSearcher);

                        if (!otherJumpPad)
                        {
                            //sLog->outError(LOG_FILTER_TSCR, "OtherJumpPad not found for GUID %u", me->GetGUIDLow());
                            checkTimer = 1000;
                            return;
                        }
                        else
                        {
                            otherJumpPadGuid = otherJumpPad->GetGUID();
                            me->SetOrientation(me->GetAngle(otherJumpPad));
                        }
                    }

                    // Disable jump pads which lead away from Deathwing on phase 2
                    if ((pos == 2 || pos == 4 || pos == 5) && me->FindNearestCreature(NPC_DEATHWING_1, 1000.0f))
                    {
                        checkTimer = 5000;
                        return;
                    }

                    std::list<Player*> players;
                    PlayerCheck check(me, spellIdEx1, spellIdEx2);
                    Trinity::PlayerListSearcher<PlayerCheck> searcher(me, players, check);
                    me->VisitNearbyObject(32.0f, searcher);
                    if (!players.empty())
                        for (std::list<Player*>::const_iterator itr = players.begin(); itr != players.end(); ++ itr)
                            DoCast((*itr), spellId, true);

                    checkTimer = 100;
                }
                else
                    checkTimer -= diff;
            }

        private:
            uint32 checkTimer;
            uint8 pos;
            uint32 spellId;
            uint32 spellIdEx1;
            uint32 spellIdEx2;
            ObjectGuid otherJumpPadGuid;

            class PlayerCheck
            {
                public:
                    PlayerCheck(WorldObject const* obj, uint32 spellEx1, uint32 spellEx2) : _obj(obj), _spellEx1(spellEx2), _spellEx2(spellEx2) { }
                    bool operator()(Player* u)
                    {
                        if (!u->IsAlive())
                            return false;

                        if (!u->IsFalling() || _obj->GetPositionZ() < (u->GetPositionZ() + 3.0f))
                            return false;

                        if (!_obj->IsWithinDistInMap(u, 32.0f, false))
                            return false;

                        if (!_obj->HasInArc(M_PI, u))
                            return false;

                        if (u->HasAura(_spellEx1) || u->HasAura(_spellEx2))
                            return false;

                        return true;
                    }

                private:
                    WorldObject const* _obj;
                    uint32 _spellEx1, _spellEx2;
            };

            class OtherJumpPadCheck
            {
                public:
                    OtherJumpPadCheck(uint8 otherPos) : _otherPos(otherPos) { }
                    bool operator()(Creature* u)
                    {
                        return u->GetEntry() == NPC_JUMP_PAD && u->GetExactDist(&jumpPos[_otherPos]) < 5;
                    }

                private:
                    uint8 _otherPos;
            };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_madness_of_deathwing_jump_padAI>(creature);
        }
};

class spell_madness_of_deathwing_assault_aspects : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_assault_aspects() : SpellScriptLoader("spell_madness_of_deathwing_assault_aspects") { }

        class spell_madness_of_deathwing_assault_aspects_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_assault_aspects_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.empty())
                    return;

                uint8 players[4];
                memset(players, 0, sizeof(players));

                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if (WorldObject* object = (*itr))
                    {
                        uint8 ground =  GetGround(object);
                        if (ground > 0)
                            players[ground - 1]++;
                    }

                uint8 found = 0;
                uint8 max_val = 0;

                uint8 destroyed = 0;
                if (Creature* pDeathwing = GetCaster()->ToCreature())
                {
                    destroyed = pDeathwing->AI()->GetData(DATA_DESTROY_PLATFORM);

                    if (destroyed == 30)
                        return;

                    for (uint8 i = 0; i < 4; ++i)
                        if (players[i] > max_val && (destroyed & (1 << (i + 1))) == 0)
                        {
                            max_val = players[i];
                            found = i + 1;
                        }

                    if (found == 0)
                        for (uint8 i = 0; i < 4; ++i)
                            if ((destroyed & (1 << (i + 1))) == 0)
                            {
                                found = i + 1;
                                break;
                            }

                    if (found > 0)
                        pDeathwing->AI()->SetData(DATA_ASSAULT_PLATFORM, found);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_madness_of_deathwing_assault_aspects_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_assault_aspects_SpellScript();
        }
};

class spell_madness_of_deathwing_crush_force : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_crush_force() : SpellScriptLoader("spell_madness_of_deathwing_crush_force") { }

        class spell_madness_of_deathwing_crush_force_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_crush_force_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster())
                    return;

                if (targets.empty())
                    return;

                Trinity::Containers::RandomResizeList(targets, 1);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_madness_of_deathwing_crush_force_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_crush_force_SpellScript();
        }
};

class spell_madness_of_deathwing_hemorrhage_script : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_hemorrhage_script() : SpellScriptLoader("spell_madness_of_deathwing_hemorrhage_script") { }

        class spell_madness_of_deathwing_hemorrhage_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_hemorrhage_script_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetCaster(), SPELL_HEMORRHAGE_MISSILE, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_hemorrhage_script_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_hemorrhage_script_SpellScript();
        }
};

class spell_madness_of_deathwing_elementium_meteor_summon : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_elementium_meteor_summon() : SpellScriptLoader("spell_madness_of_deathwing_elementium_meteor_summon") { }

        class spell_madness_of_deathwing_elementium_meteor_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_elementium_meteor_summon_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                WorldLocation summonPos = *GetExplTargetDest();
                summonPos.Relocate(boltPos[0]);
                SetExplTargetDest(summonPos);
                GetHitDest()->Relocate(boltPos[0]);
            }

            void Register() override
            {
                OnEffectHit += SpellEffectFn(spell_madness_of_deathwing_elementium_meteor_summon_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_elementium_meteor_summon_SpellScript();
        }
};

class spell_madness_of_deathwing_elementium_blast : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_elementium_blast() : SpellScriptLoader("spell_madness_of_deathwing_elementium_blast") { }

        class spell_madness_of_deathwing_elementium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_elementium_blast_SpellScript);

            void HandleDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                float distance = GetCaster()->GetExactDist2d(GetHitUnit());
                if (distance < 12.0f)
                    distance = 12.0f;
                SetHitDamage(int32(GetHitDamage() / (distance / 12.0f)));
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_madness_of_deathwing_elementium_blast_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_elementium_blast_SpellScript();
        }
};

class spell_madness_of_deathwing_corrupting_parasite_aoe : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_corrupting_parasite_aoe() : SpellScriptLoader("spell_madness_of_deathwing_corrupting_parasite_aoe") { }

        class spell_madness_of_deathwing_corrupting_parasite_aoe_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_corrupting_parasite_aoe_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                Unit* target = GetHitUnit();

                GetCaster()->CastSpell(GetHitUnit(), SPELL_CORRUPTING_PARASITE_AURA, true);
                GetCaster()->CastSpell(GetHitUnit(), SPELL_CORRUPTING_PARASITE_DMG, true);
                if (Creature* pParasite = GetCaster()->SummonCreature(NPC_CORRUPTING_PARASITE, *target))
                {
                    pParasite->AI()->DoZoneInCombat();
                    pParasite->EnterVehicle(GetHitUnit(), -1, true);
                    pParasite->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE, -70.0f, true);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_corrupting_parasite_aoe_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_corrupting_parasite_aoe_SpellScript();
        }
};

class spell_madness_of_deathwing_corrupting_parasite_aura : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_corrupting_parasite_aura() : SpellScriptLoader("spell_madness_of_deathwing_corrupting_parasite_aura") { }

        class spell_madness_of_deathwing_corrupting_parasite_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_madness_of_deathwing_corrupting_parasite_aura_AuraScript);

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetUnitOwner())
                {
                    AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                    if (removeMode == AURA_REMOVE_BY_EXPIRE || removeMode == AURA_REMOVE_BY_DEATH)
                    {
                        owner->CastSpell((Unit*)NULL, SPELL_PARASITIC_BACKSLASH, true);
                        if (Creature* pParasite = owner->FindNearestCreature(NPC_CORRUPTING_PARASITE, 10.0f))
                            pParasite->AI()->DoAction(ACTION_PARASITIC_BACKSLASH);
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_madness_of_deathwing_corrupting_parasite_aura_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_madness_of_deathwing_corrupting_parasite_aura_AuraScript();
        }
};

class spell_madness_of_deathwing_unstable_corruption : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_unstable_corruption() : SpellScriptLoader("spell_madness_of_deathwing_unstable_corruption") { }

        class spell_madness_of_deathwing_unstable_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_unstable_corruption_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(TargetCheck());
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                int32 damage = CalculatePct(GetCaster()->GetHealth(), 10);

                SetHitDamage(damage);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (caster && target)
                    caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].TriggerSpell, true);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_madness_of_deathwing_unstable_corruption_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_unstable_corruption_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_unstable_corruption_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

            struct TargetCheck
            {
                bool operator()(WorldObject* obj)
                {
                    if (Unit* unit = obj->ToUnit())
                        if (unit->GetCharmerOrOwnerPlayerOrPlayerItself())
                            return false;

                    return true;
                }
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_unstable_corruption_SpellScript();
        }
};

class spell_madness_of_deathwing_agonizing_pain : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_agonizing_pain() : SpellScriptLoader("spell_madness_of_deathwing_agonizing_pain") { }

        class spell_madness_of_deathwing_agonizing_pain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_agonizing_pain_SpellScript);

            void HandleDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                int32 damage = GetHitUnit()->CountPctFromMaxHealth(20);

                SetHitDamage(damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_madness_of_deathwing_agonizing_pain_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_agonizing_pain_SpellScript();
        }
};

class spell_madness_of_deathwing_impale_aspect : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_impale_aspect() : SpellScriptLoader("spell_madness_of_deathwing_impale_aspect") { }

        class spell_madness_of_deathwing_impale_aspect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_impale_aspect_SpellScript);

            void HandleDamage()
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                int32 damage = GetHitUnit()->CountPctFromMaxHealth(26);

                SetHitDamage(damage);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_madness_of_deathwing_impale_aspect_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_impale_aspect_SpellScript();
        }
};

class spell_madness_of_deathwing_carrying_winds_script : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_carrying_winds_script(const char* name, uint8 pos) : SpellScriptLoader(name), _pos(pos) { }

        class spell_madness_of_deathwing_carrying_winds_script_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_carrying_winds_script_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster() || !GetHitUnit())
                    return;

                uint32 spellId = SPELL_CARRYING_WINDS_1;
                switch (_pos)
                {
                    case 1: spellId = SPELL_CARRYING_WINDS_1; break;
                    case 2: spellId = SPELL_CARRYING_WINDS_2; break;
                    case 3: spellId = SPELL_CARRYING_WINDS_3; break;
                    case 4: spellId = SPELL_CARRYING_WINDS_4; break;
                    case 5: spellId = SPELL_CARRYING_WINDS_5; break;
                    case 6: spellId = SPELL_CARRYING_WINDS_6; break;
                    default: break;
                }

                uint8 otherPos = (_pos % 2) == 1 ? _pos : _pos - 2;
                GetHitUnit()->CastSpell(jumpPos[otherPos].GetPositionX(), jumpPos[otherPos].GetPositionY(), jumpPos[otherPos].GetPositionZ(), spellId, true);
                GetHitUnit()->AddAura(spellId, GetHitUnit());
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_carrying_winds_script_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        public:
            spell_madness_of_deathwing_carrying_winds_script_SpellScript(uint8 pos) : SpellScript(), _pos(pos) { } 

        private:
            uint8 _pos;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_carrying_winds_script_SpellScript(_pos);
        }

    private:
        uint8 _pos;
};

class spell_madness_of_deathwing_corrupted_blood_stacker : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_corrupted_blood_stacker() : SpellScriptLoader("spell_madness_of_deathwing_corrupted_blood_stacker") { }

        class spell_madness_of_deathwing_corrupted_blood_stacker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_madness_of_deathwing_corrupted_blood_stacker_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (!GetCaster())
                    return;

                if (AuraEffect* effect = GetCaster()->GetAuraEffect(SPELL_CORRUPTED_BLOOD, EFFECT_1))
                    effect->SetAmount(GetCaster()->GetPower(POWER_ALTERNATE_POWER) * sSpellMgr->GetSpellInfo(SPELL_CORRUPTED_BLOOD, GetCaster()->GetMap()->GetDifficulty())->Effects[EFFECT_1].BasePoints);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_madness_of_deathwing_corrupted_blood_stacker_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_madness_of_deathwing_corrupted_blood_stacker_AuraScript();
        }
};

class spell_madness_of_deathwing_shrapnel : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_shrapnel() : SpellScriptLoader("spell_madness_of_deathwing_shrapnel") { }

        class spell_madness_of_deathwing_shrapnel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_shrapnel_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                if (!GetCaster() || targets.empty())
                    return;

                m_fragments = std::vector<WorldObject*>(targets.begin(), targets.end());
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(m_fragments.begin(), m_fragments.end(), g);                
            }

            void FilterEnemies(std::list<WorldObject*>& targets)
            {
                if (!GetCaster() || targets.empty())
                    return;

                targets.remove_if(EnemyCheck());
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* target = GetHitUnit();

                if (!GetCaster() || !target || m_fragments.empty())
                    return;

                Unit* fragment = m_fragments.back()->ToUnit();
                m_fragments.pop_back();

                fragment->CastSpell(target, SPELL_SHRAPNEL_TARGET, true);
                fragment->CastSpell(target, SPELL_SHRAPNEL);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_madness_of_deathwing_shrapnel_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_madness_of_deathwing_shrapnel_SpellScript::FilterEnemies, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_madness_of_deathwing_shrapnel_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

            std::vector<WorldObject*> m_fragments;

            class EnemyCheck
            {
                public:
                    bool operator()(WorldObject* u) const
                    {
                        if (u->GetTypeId() == TYPEID_PLAYER)
                            return false;

                        switch (u->GetEntry())
                        {
                            case NPC_ALEXSTRASZA_DRAGON:
                            case NPC_NOZDORMU_DRAGON:
                            case NPC_YSERA_DRAGON:
                            case NPC_KALECGOS_DRAGON:
                                return true;
                            default:
                                return false;
                        }
                    }
            };
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_shrapnel_SpellScript();
        }
};

class spell_madness_of_deathwing_dream : public SpellScriptLoader
{
    public:
        spell_madness_of_deathwing_dream() : SpellScriptLoader("spell_madness_of_deathwing_dream") { }

        class spell_madness_of_deathwing_dream_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_madness_of_deathwing_dream_SpellScript);

            SpellCastResult CheckCast()
            {
                return GetCaster() && GetCaster()->GetAreaId() == 5893 ? SPELL_CAST_OK : SPELL_FAILED_INCORRECT_AREA;
            }

            void Register() override
            {
                OnCheckCast += SpellCheckCastFn(spell_madness_of_deathwing_dream_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_madness_of_deathwing_dream_SpellScript();
        }
};

void AddSC_madness_of_deathwing()
{
    new boss_madness_of_deathwing();
    new npc_dragon_soul_thrall_1();
    new npc_madness_of_deathwing_mutated_corruption();
    new npc_madness_of_deathwing_crush_target();
    new npc_madness_of_deathwing_limb_tentacle();
    new npc_madness_of_deathwing_blistering_tentacle();
    new npc_madness_of_deathwing_regenerative_blood();
    new npc_madness_of_deathwing_elementium_bolt();
    new npc_madness_of_deathwing_corrupting_parasite();
    new npc_madness_of_deathwing_deathwing();
    new npc_madness_of_deathwing_congealing_blood();
    new npc_madness_of_deathwing_elementium_fragment();
    new npc_madness_of_deathwing_elementium_terror();
    new npc_dragon_soul_alexstrasza_dragon();
    new npc_dragon_soul_nozdormu_dragon();
    new npc_dragon_soul_ysera_dragon();
    new npc_dragon_soul_kalecgos_dragon();
    new npc_madness_of_deathwing_jump_pad();
    new spell_madness_of_deathwing_assault_aspects();
    new spell_madness_of_deathwing_crush_force();
    new spell_madness_of_deathwing_hemorrhage_script();
    new spell_madness_of_deathwing_elementium_meteor_summon();
    new spell_madness_of_deathwing_elementium_blast();
    new spell_madness_of_deathwing_corrupting_parasite_aoe();
    new spell_madness_of_deathwing_corrupting_parasite_aura();
    new spell_madness_of_deathwing_unstable_corruption();
    new spell_madness_of_deathwing_agonizing_pain();
    new spell_madness_of_deathwing_impale_aspect();
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_1", 1);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_2", 2);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_3", 3);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_4", 4);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_5", 5);
    new spell_madness_of_deathwing_carrying_winds_script("spell_madness_of_deathwing_carrying_winds_script_6", 6);
    new spell_madness_of_deathwing_corrupted_blood_stacker();
    new spell_madness_of_deathwing_shrapnel();
    new spell_madness_of_deathwing_dream();
}
