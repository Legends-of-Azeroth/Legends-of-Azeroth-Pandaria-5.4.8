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
#include "GossipDef.h"
#include "Group.h"

enum eSpells
{
    SPELL_GB_11010_ARMAGEDDON_CLASS_DEFENSE_SYSTEME = 145864,
    SPELL_UNSTABLE_DEFENSE_SYSTEME_AT_EFF           = 145685,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2          = 145688,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_UP_ENTRANCE    = 147672,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_UP_EXIT        = 147673,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_BOX            = 145687,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_BOX_AREA       = 145838,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_4              = 147673, // unk
    SPELL_UNSTABLE_DEFENSE_SYSTEM_TELEPORT          = 142722,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_INITIALIZE        = 144256,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_DUMMY             = 144273,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_INIT_VISUAL       = 144281,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER         = 144921, // room power
    SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER_PLAYER  = 148505,
    SPELL_UNSTABLE_DEFENSE_SYSTEM_LINK_TO_LEVER     = 146529, // visual
    SPELL_UNSTABLE_DEFENSE_SYSTEM_BEAM              = 149117,
    SPELL_CRISS_CROSS_ACHIEV                        = 148503,

    // Room
    SPELL_SPOILS_OF_PANDARIA_AT_UP_LEFT             = 136882,
    SPELL_SPOILS_OF_PANDARIA_AT_UP_RIGHT            = 142560,
    SPELL_SPOILS_OF_PANDARIA_AT_LOW_LEFT            = 145121,
    SPELL_SPOILS_OF_PANDARIA_AT_LOW_RIGHT           = 145997,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_1           = 142482, // not found any data in sniffs for these
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_2           = 142941,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_3           = 142943,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_4           = 144838,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_5           = 146584,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_6           = 148761,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_7           = 149038,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_8           = 149039,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_9           = 149040,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_10          = 149041,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_11          = 149044,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_12          = 149045,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_13          = 149050,
    SPELL_SPOILS_OF_PANDARIA_SCRIPT_EFF_14          = 149051,
    SPELL_SPOILS_OF_PANDARIA_LEVER_EFF              = 145125,

    // Creatures
    SPELL_AMBER_SHELL                               = 145828,
    SPELL_RETURN_TO_STONE                           = 144872,
    SPELL_DIAGNOSTIC_SEQUENCE                       = 142529,
    SPELL_STONE_STATUE                              = 142526,
    SPELL_SPARK_OF_LIFE                             = 142694,
    SPELL_SPARK_OF_LIFE_MISSLE                      = 142695,
    SPELL_ENCAPSULATED_PHEROMONES                   = 142559,
    SPELL_ENCAPSULATED_PHEROMONES_AURA              = 142524,
    SPELL_ENCAPSULATED_PHEROMONES_EFF               = 145748,
    SPELL_ENCAPSULATED_PHEROMONES_AURA_EFF          = 145747,
    SPELL_ENCAPSULATED_PHEROMONES_AT                = 145285,
    SPELL_MOGU_SHADOW_RITUALIST                     = 149285,
    SPELL_MOGU_SHADOW_RITUALIST_EFF                 = 144893, // summ
    SPELL_ENRAGE                                    = 145692,
    SPELL_ENRAGE_PROC_EFF                           = 148510,
    SPELL_GUSTING_BOMB                              = 145712,
    SPELL_GUSTING_BOMB_AURA_EFF                     = 145715,
    SPELL_GUSTING_BOMB_EFF                          = 145716,
    SPELL_GUSTING_BOMB_JUMP                         = 145717,
    SPELL_GUSTING_BOMB_JUMP_EFF                     = 145718,
    SPELL_GUSTING_BOMB_AT                           = 145714,
    SPELL_THROW_EXPLOSIVES                          = 145702,
    SPELL_WINDSTORM                                 = 145286,
    SPELL_WINDSTORM_EFF                             = 145817,
    SPELL_RAGE_OF_EMPRESS                           = 145812,
    SPELL_RAGE_OF_EMPRESS_EFF                       = 145813,
    SPELL_RESIDUE                                   = 145786,
    SPELL_RESIDUE_EFF                               = 145790,
    SPELL_MANTID_SWARM                              = 142539,
    SPELL_MANTID_SWARM_SUMM                         = 145806,
    SPELL_MANTID_SWARM_SUMM_RIGHT                   = 145807,
    SPELL_MANTID_SWARM_SUMM_LEFT                    = 145808,
    SPELL_PHEROMONE_CLOUD                           = 148762,
    SPELL_PHEROMONE_CLOUD_EFF                       = 148760,
    SPELL_SET_TO_BLOW                               = 145996,
    SPELL_SET_TO_BLOW_EFF                           = 147404,
    SPELL_SET_TO_BLOW_BUTTON                        = 145987,
    SPELL_SET_TO_BLOW_EXPLOSION_EFF                 = 145993,
    SPELL_SET_TO_BLOW_AT                            = 146365,
    SPELL_SET_TO_BLOW_VISUAL_1                      = 148054,
    SPELL_SET_TO_BLOW_VISUAL_2                      = 148055,
    SPELL_SET_TO_BLOW_VISUAL_3                      = 148056,
    SPELL_SET_TO_BLOW_VISUAL_4                      = 142997,
    SPELL_THROW_BOMB                                = 146364,
    SPELL_RUSH                                      = 144904,
    SPELL_CARNIVOROUS_BITE                          = 144853, // rush eff
    SPELL_SPARK_OF_LIFE_VISUAL                      = 142676,
    SPELL_PULSE                                     = 142765,
    SPELL_PULSE_EFF                                 = 142759,
    SPELL_NOVA                                      = 149277,
    SPELL_NOVA_EFF                                  = 142775,
    SPELL_EARTHEN_SHARD                             = 144923,
    SPELL_HARDEN_FLESH                              = 144922,
    SPELL_HARDEN_FLESH_EFF                          = 145218,
    SPELL_SHADOW_VOLLEY                             = 148515, // Jun-Wei
    SPELL_SHADOW_VOLLEY_EFF                         = 148516,
    SPELL_MOLTEN_FIST                               = 148518, // Zu-Yin
    SPELL_MOLTEN_FIST_EFF                           = 148517,
    SPELL_JADE_TEMPEST                              = 148582, // Xiang-Lin
    SPELL_JADE_TEMPEST_EFF                          = 148583,
    SPELL_FRACTURE                                  = 148513, // Kun Da
    SPELL_FRACTURE_EFF                              = 148514,
    SPELL_STRENGTH_OF_STONE                         = 145998, // empower for each stone copy at area
    SPELL_SHARED_RETURN_TO_STONE                    = 145489,
    SPELL_RETURN_TO_STONE_VISUAL                    = 148511,
    SPELL_RETURN_TO_STONE_EFF                       = 145514,
    SPELL_CRIMSON_RECONSTRUCTION_EFF                = 145271,
    SPELL_CRIMSON_RECONSTRUCTION_SELECTOR           = 142947,
    SPELL_CRIMSON_RECONSTRUCTION_AT                 = 145270,
    SPELL_MATTER_SCRAMBLE_EFF                       = 145393,
    SPELL_MATTER_SCRAMBLE_CHECK                     = 145369,
    SPELL_MATTER_SCRAMBLE_VISUAL                    = 145288,
    SPELL_MOGU_RUNE_OF_POWER                        = 145460,
    SPELL_MOGU_RUNE_OF_POWER_EFF                    = 145458,
    SPELL_TORMENT_EFF                               = 136885,
    SPELL_TORMENT                                   = 142983,
    SPELL_TORMENT_LINK                              = 142942,
    SPELL_FORBIDDEN_MAGIC                           = 145230,
    SPELL_FORBIDDEN_MAGIC_EFF                       = 145240,
    SPELL_MOGU_SHADOW_RITUALIST_SHARE               = 145245,
    SPELL_MOGU_SHADOW_RITUALIST_VISUAL              = 142508,
    SPELL_PANDAREN_SPIRIT_SPAWN                     = 145574,
    SPELL_PANDAREN_SPIRIT_VUSAL                     = 146041,
    SPELL_KEG_TOSS_LAUNCHER                         = 146213,
    SPELL_KEG_TOSS                                  = 146214,
    SPELL_KEG_TOSS_EFF                              = 146217,
    SPELL_BREATH_OF_FIRE                            = 146222,
    SPELL_BREATH_OF_FIRE_EFF                        = 146226,
    SPELL_BREATH_OF_FIRE_DISTRACT                   = 146235,
    SPELL_BLADE_OF_THE_HUNDRED_STEPS                = 146068,
    SPELL_BLADE_OF_THE_HUNDRED_STEPS_EFF            = 146081,
    SPELL_EMINENCE                                  = 146189,
    SPELL_EMINENCE_EFF                              = 146190,
    SPELL_GUSTING_CRANE_KICK                        = 146180,
    SPELL_GUSTING_CRANE_KICK_EFF                    = 146182,
    SPELL_STAFF_OF_RESONATING_WATER                 = 146099,
    SPELL_STAFF_OF_RESONATING_WATER_AT              = 146098,
    SPELL_STAFF_OF_RESONATING_WATER_EFF             = 146679,
    SPELL_STAFF_OF_RESONATING_WATER_HEAL_EFF        = 146582,
    SPELL_CLAW_OF_BURNING_ANGER                     = 146141,
    SPELL_CLAW_OF_BURNING_ANGER_EFF                 = 146142,
    SPELL_PATH_OF_BLOSSOMS                          = 146253,
    SPELL_PATH_OF_BLOSSOMS_EFF                      = 146257,
    SPELL_PATH_OF_BLOSSOMS_AT                       = 146255,
    SPELL_MASS_PARALYSIS                            = 146289,
    SPELL_RETURN_TO_STONE_EFF_WARLORD               = 145514,
    SPELL_STRENGTH_OF_THE_STONE                     = 145998,
    SPELL_ANIMATED_STRIKE                           = 145523,
    SPELL_SUPERNOVA                                 = 146815,
    SPELL_UNSTABLE_SPARK_SUMM                       = 146820,
    SPELL_UNSTABLE_SPARK_VISUAL                     = 146696,
    SPELL_UNSTABLE_SPARK_VISUAL_2                   = 146803,
    SPELL_UNSTABLE_SPARK_MISSLE                     = 146824,
    SPELL_CRIMSON_ACID                              = 149280,

    // Misc
    SPELL_VISUAL_ID                                 = 34364, // by lever
    SPELL_BREAKING                                  = 144229,
    SPELL_SPOILS_CREDIT                             = 145904,
    SPELL_ANIMA_VISUAL_ID                           = 32969,
    SPELL_ENCASED_PHEROMONES_VISUAL_ID              = 34287,
    SPELL_WEAPON_KIT                                = 35924,
    SPELL_PERMANENT_FEIGN_DEATH                     = 70628,
    SPELL_SPRINT                                    = 146387,
};

enum Events
{
    EVENT_ENRAGE = 1,
    EVENT_GUSTING_BOMB,
    EVENT_THROW_EXPLOSIVES,
    EVENT_WIND_STORM,
    EVENT_RAGE_OF_EMPRESS,
    EVENT_RESIDUE,
    EVENT_MANTID_SWARM,
    EVENT_SET_TO_BLOW,
    EVENT_PHEROMONES_CLOUD,
    EVENT_RUSH,
    EVENT_MATTER_SCRAMBLE,
    EVENT_CRIMSON_RECONSTRUCT,
    EVENT_RUNE_OF_POWER,
    EVENT_TORMENT,
    EVENT_FORBIDDEN_MAGIC,
    EVENT_EARTHEN_SHARD,
    EVENT_HARDEN_FLESH,
    EVENT_JADE_TEMPEST,
    EVENT_KEG_TOSS,
    EVENT_BREATH_OF_FIRE,
    EVENT_GUSTING_CRANE_KICK,
    EVENT_PATH_OF_BLOSSOMS,
    EVENT_CARNIVOROUS_BITE,
    EVENT_ANIMATED_STRIKE,
    EVENT_RETURN_TO_STONE,
};

enum Yells
{
    TALK_INTRO,
    TALK_WIPE,
    TALK_SYSTEM_PREPARE_1,
    TALK_SYSTEM_PREPARE_2, // done
    TALK_DEATH,
    TALK_INTRO_1,
    TALK_INTRO_2,
    TALK_INTRO_3,

    // Misc
    SPOILS_GOSSIP_MENU = 15814,
    SPOILS_TEXT_ID     = 22744,
};

enum Actions
{
    ACTION_INITIALIZE_COMBAT,
    ACTION_RESET,
    ACTION_SECOND_QUADRANT,
    ACTION_MATTER_SWAP,
    ACTION_MATTER_SWAP_EFF,
    ACTION_TORMENT_LINK,
};

const std::map<uint32, uint32> mantidCreatures =
{
    { NPC_ZARTHIK_AMBER_PRIEST,   3 },
    { NPC_SETTHIK_WIELDER,        3 },
    { NPC_SRITHIK_BOMBARDIER,     1 },
    { NPC_KORTHIK_WARCALLER,      1 },
    { NPC_AMBER_ENCASED_KUNCHONG, 1 },
    { NPC_COMMANDER_TIK,         14 },
    { NPC_COMMANDER_IKTAL,       14 },
    { NPC_COMMANDER_NAKAZ,       14 },
};

const std::map<uint32, uint32> moguCreatures =
{
    { NPC_MODIFIED_ANIMA_GOLEM,        3 },
    { NPC_MOGU_SHADOW_RITUALIST,       3 },
    { NPC_SHADOW_RITUALIST_PHYLACTERY, 0 },
    { NPC_ANIMATED_STONE_MOGU,         1 },
    { NPC_BURIAL_URN,                  1 },
    { NPC_QUILEN_GUARDIANS_MAIN,       0 },
    { NPC_QUILEN_GUARDIANS,            1 },
    { NPC_JUN_WEI,                    14 },
    { NPC_KUN_DA,                     14 },
    { NPC_XIANG_LIN,                  14 },
    { NPC_ZU_YIN,                     14 },
};

const std::map<uint32, uint32> pandarenCreatures =
{
    { NPC_ANCIENT_BREWMASTER_SPIRIT,    0 },
    { NPC_WISE_MISTEWEAVER_SPIRIT,      0 },
    { NPC_NAMELESSE_WINDWALLKER_SPIRIT, 0 },
};

bool IsMogu(uint32 entry)
{
    for (auto&& itr : moguCreatures)
        if (entry == itr.first)
            return true;

    return false;
}

const std::map<uint32, std::array<uint32, 2>> invSpoilBoxesType =
{
    { GO_MANTID_LOW_BOX_1,     { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_2,     { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_3,     { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_4,     { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_5,     { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_6,     { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_7,     { NPC_SRITHIK_BOMBARDIER,          NPC_KORTHIK_WARCALLER } },
    { GO_MANTID_LOW_BOX_8,     { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_9,     { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_10,    { NPC_SRITHIK_BOMBARDIER,          NPC_KORTHIK_WARCALLER } },
    { GO_MANTID_LOW_BOX_11,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_12,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_13,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_14,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_15,    { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_16,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_17,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_18,    { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_19,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_20,    { NPC_KORTHIK_WARCALLER,           NPC_AMBER_ENCASED_KUNCHONG } },
    { GO_MANTID_LOW_BOX_21,    { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_22,    { NPC_SRITHIK_BOMBARDIER,          NPC_AMBER_ENCASED_KUNCHONG } },
    { GO_MANTID_LOW_BOX_23,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_24,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_25,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_26,    { NPC_SRITHIK_BOMBARDIER,          NPC_KORTHIK_WARCALLER } },
    { GO_MANTID_LOW_BOX_27,    { NPC_SRITHIK_BOMBARDIER,          NPC_KORTHIK_WARCALLER } },
    { GO_MANTID_LOW_BOX_28,    { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_29,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_30,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_31,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_32,    { NPC_KORTHIK_WARCALLER,           0 } },
    { GO_MANTID_LOW_BOX_33,    { NPC_SRITHIK_BOMBARDIER,          0 } },
    { GO_MANTID_LOW_BOX_34,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_35,    { NPC_AMBER_ENCASED_KUNCHONG,      0 } },
    { GO_MANTID_LOW_BOX_36,    { NPC_AMBER_ENCASED_KUNCHONG,      NPC_SRITHIK_BOMBARDIER } },
    { GO_MANTID_MIDDLE_BOX_1,  { NPC_SETTHIK_WIELDER,             0 } },
    { GO_MANTID_MIDDLE_BOX_2,  { NPC_ZARTHIK_AMBER_PRIEST,        0 } },
    { GO_MANTID_MIDDLE_BOX_3,  { NPC_ZARTHIK_AMBER_PRIEST,        NPC_SETTHIK_WIELDER } },
    { GO_MANTID_MIDDLE_BOX_4,  { NPC_ZARTHIK_AMBER_PRIEST,        0 } },
    { GO_MANTID_MIDDLE_BOX_5,  { NPC_SETTHIK_WIELDER,             0 } },
    { GO_MANTID_MIDDLE_BOX_6,  { NPC_ZARTHIK_AMBER_PRIEST,        NPC_SETTHIK_WIELDER } },
    { GO_MANTID_MIDDLE_BOX_7,  { NPC_ZARTHIK_AMBER_PRIEST,        0 } },
    { GO_MANTID_MIDDLE_BOX_8,  { NPC_ZARTHIK_AMBER_PRIEST,        0 } },
    { GO_MANTID_MIDDLE_BOX_9,  { NPC_SETTHIK_WIELDER,             0 } },
    { GO_MANTID_MIDDLE_BOX_10, { NPC_SETTHIK_WIELDER,             0 } },
    { GO_MANTID_MIDDLE_BOX_11, { NPC_ZARTHIK_AMBER_PRIEST,        0 } },
    { GO_MANTID_MIDDLE_BOX_12, { NPC_SETTHIK_WIELDER,             0 } },
    { GO_MANTID_HIGH_BOX_1,    { NPC_COMMANDER_TIK,               NPC_COMMANDER_IKTAL } },
    { GO_MANTID_HIGH_BOX_2,    { NPC_COMMANDER_NAKAZ,             NPC_COMMANDER_TIK } },
    { GO_MANTID_HIGH_BOX_3,    { NPC_COMMANDER_NAKAZ,             NPC_COMMANDER_IKTAL } },
    { GO_MANTID_HIGH_BOX_4,    { NPC_COMMANDER_TIK,               NPC_COMMANDER_IKTAL } },
    { GO_MOGU_LOW_BOX_1,       { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_2,       { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_3,       { NPC_QUILEN_GUARDIANS,             0 } },
    { GO_MOGU_LOW_BOX_4,       { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_5,       { NPC_ANIMATED_STONE_MOGU,         NPC_BURIAL_URN } },
    { GO_MOGU_LOW_BOX_6,       { NPC_ANIMATED_STONE_MOGU,         NPC_QUILEN_GUARDIANS } },
    { GO_MOGU_LOW_BOX_7,       { NPC_QUILEN_GUARDIANS,             NPC_BURIAL_URN } },
    { GO_MOGU_LOW_BOX_8,       { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_9,       { NPC_ANIMATED_STONE_MOGU,         NPC_BURIAL_URN } },
    { GO_MOGU_LOW_BOX_10,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_11,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_12,      { NPC_QUILEN_GUARDIANS,             0 } },
    { GO_MOGU_LOW_BOX_13,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_14,      { NPC_QUILEN_GUARDIANS,             0 } },
    { GO_MOGU_LOW_BOX_15,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_16,      { NPC_QUILEN_GUARDIANS,             NPC_BURIAL_URN } },
    { GO_MOGU_LOW_BOX_17,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_18,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_19,      { NPC_QUILEN_GUARDIANS,             0 } },
    { GO_MOGU_LOW_BOX_20,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_21,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_22,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_23,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_24,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_25,      { NPC_QUILEN_GUARDIANS,             0 } },
    { GO_MOGU_LOW_BOX_26,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_27,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_28,      { NPC_QUILEN_GUARDIANS,             0 } },
    { GO_MOGU_LOW_BOX_29,      { NPC_ANIMATED_STONE_MOGU,         NPC_QUILEN_GUARDIANS } },
    { GO_MOGU_LOW_BOX_30,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_31,      { NPC_QUILEN_GUARDIANS, 0 } },
    { GO_MOGU_LOW_BOX_32,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_33,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_34,      { NPC_ANIMATED_STONE_MOGU,         0 } },
    { GO_MOGU_LOW_BOX_35,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_LOW_BOX_36,      { NPC_BURIAL_URN,                  0 } },
    { GO_MOGU_MIDDLE_BOX_1,    { NPC_MODIFIED_ANIMA_GOLEM,        0 } },
    { GO_MOGU_MIDDLE_BOX_2,    { NPC_MODIFIED_ANIMA_GOLEM,        NPC_SHADOW_RITUALIST_PHYLACTERY } },
    { GO_MOGU_MIDDLE_BOX_3,    { NPC_MODIFIED_ANIMA_GOLEM,        NPC_SHADOW_RITUALIST_PHYLACTERY } },
    { GO_MOGU_MIDDLE_BOX_4,    { NPC_MODIFIED_ANIMA_GOLEM,        NPC_SHADOW_RITUALIST_PHYLACTERY } },
    { GO_MOGU_MIDDLE_BOX_5,    { NPC_SHADOW_RITUALIST_PHYLACTERY, 0 } },
    { GO_MOGU_MIDDLE_BOX_6,    { NPC_MODIFIED_ANIMA_GOLEM,        0 } },
    { GO_MOGU_MIDDLE_BOX_7,    { NPC_SHADOW_RITUALIST_PHYLACTERY, 0 } },
    { GO_MOGU_MIDDLE_BOX_8,    { NPC_MODIFIED_ANIMA_GOLEM,        0 } },
    { GO_MOGU_MIDDLE_BOX_9,    { NPC_MODIFIED_ANIMA_GOLEM,        NPC_SHADOW_RITUALIST_PHYLACTERY } },
    { GO_MOGU_MIDDLE_BOX_10,   { NPC_MODIFIED_ANIMA_GOLEM,        NPC_SHADOW_RITUALIST_PHYLACTERY } },
    { GO_MOGU_MIDDLE_BOX_11,   { NPC_SHADOW_RITUALIST_PHYLACTERY, 0 } },
    { GO_MOGU_MIDDLE_BOX_12,   { NPC_MODIFIED_ANIMA_GOLEM,        0 } },
    { GO_MOGU_HIGH_BOX_1,      { NPC_JUN_WEI,                     NPC_KUN_DA } },
    { GO_MOGU_HIGH_BOX_2,      { NPC_JUN_WEI,                     NPC_KUN_DA } },
    { GO_MOGU_HIGH_BOX_3,      { NPC_XIANG_LIN,                   NPC_ZU_YIN } },
    { GO_MOGU_HIGH_BOX_4,      { NPC_XIANG_LIN,                   NPC_ZU_YIN } },
    { NPC_CRATE_OF_PANDAREN_RELIC_1, { NPC_ANCIENT_BREWMASTER_SPIRIT,    0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_2, { NPC_NAMELESSE_WINDWALLKER_SPIRIT, 0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_3, { NPC_WISE_MISTEWEAVER_SPIRIT,      0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_4, { NPC_WISE_MISTEWEAVER_SPIRIT,      0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_5, { NPC_NAMELESSE_WINDWALLKER_SPIRIT, 0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_6, { NPC_WISE_MISTEWEAVER_SPIRIT,      0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_7, { NPC_ANCIENT_BREWMASTER_SPIRIT,    0 } },
    { NPC_CRATE_OF_PANDAREN_RELIC_8, { NPC_WISE_MISTEWEAVER_SPIRIT,      0 } },
};

const std::map<uint32, Position> invControllerLeverType =
{
    { NPC_MOGU_SPOILS_1,   { 1682.55f, -5100.00f, -289.83f, 5.26f } },
    { NPC_MOGU_SPOILS_2,   { 1588.94f, -5163.27f, -289.83f, 5.31f } },
    { NPC_MANTID_SPOILS_1, { 1581.22f, -5151.83f, -289.83f, 2.15f } },
    { NPC_MANTID_SPOILS_2, { 1661.30f, -5098.02f, -289.82f, 2.14f } },
};

const std::map<uint32, std::array<uint32, 2>> invControllerRegionType =
{
    { NPC_MOGU_SPOILS_1,   { SPELL_SPOILS_OF_PANDARIA_AT_LOW_RIGHT, GO_DEFENSE_SYSTEM_LEVER_LOW_RIGHT } },
    { NPC_MOGU_SPOILS_2,   { SPELL_SPOILS_OF_PANDARIA_AT_LOW_LEFT,  GO_DEFENSE_SYSTEM_LEVER_LOW_LEFT  } },
    { NPC_MANTID_SPOILS_1, { SPELL_SPOILS_OF_PANDARIA_AT_UP_LEFT,   GO_DEFENSE_SYSTEM_LEVER_UP_LEFT   } },
    { NPC_MANTID_SPOILS_2, { SPELL_SPOILS_OF_PANDARIA_AT_UP_RIGHT,  GO_DEFENSE_SYSTEM_LEVER_UP_RIGHT  } },
};

const std::map<uint32, uint32> invMoguWarlordsType =
{
    { NPC_JUN_WEI, SPELL_SHADOW_VOLLEY  },
    { NPC_ZU_YIN, SPELL_MOLTEN_FIST     },
    { NPC_XIANG_LIN, SPELL_JADE_TEMPEST },
    { NPC_KUN_DA, SPELL_FRACTURE        },
};

const std::map<uint32, uint32> invMoguWarlordsSpellType =
{
    { SPELL_SHADOW_VOLLEY, SPELL_SHADOW_VOLLEY_EFF },
    { SPELL_MOLTEN_FIST,   SPELL_MOLTEN_FIST_EFF   },
    { SPELL_JADE_TEMPEST,  SPELL_JADE_TEMPEST_EFF  },
    { SPELL_FRACTURE,      SPELL_FRACTURE_EFF      },
};

const std::map<uint32, std::array<uint32, 3>> invPandarensType =
{
    { NPC_WISE_MISTEWEAVER_SPIRIT,      { NPC_STAFF_OF_RESONANTING_WATER, SPELL_STAFF_OF_RESONATING_WATER,  ROLES_HEALER } },
    { NPC_ANCIENT_BREWMASTER_SPIRIT,    { NPC_BLADE_OF_HUNDRED_STEPS,     SPELL_BLADE_OF_THE_HUNDRED_STEPS, ROLES_TANK   } },
    { NPC_NAMELESSE_WINDWALLKER_SPIRIT, { NPC_CLAW_OF_BURNING_ANGER,      SPELL_CLAW_OF_BURNING_ANGER,      ROLES_DPS    } },
};

const std::map<uint32, uint32> invSystemVoiceType =
{
    { GOSSIP_ACTION_INFO_DEF + 1, 5 },
    { GOSSIP_ACTION_INFO_DEF + 2, 9 },
    { GOSSIP_ACTION_INFO_DEF + 3, 7 },
    { GOSSIP_ACTION_INFO_DEF + 4, 8 },
    { GOSSIP_ACTION_INFO_DEF + 5, 6 },
    { GOSSIP_ACTION_INFO_DEF + 6, 5 },
    { GOSSIP_ACTION_INFO_DEF + 7, 6 },
    { GOSSIP_ACTION_INFO_DEF + 8, 7 },
    { GOSSIP_ACTION_INFO_DEF + 9, 8 },
    { GOSSIP_ACTION_INFO_DEF + 10, 9 },
};

class boss_spoils_of_pandaria : public CreatureScript
{
    public:
        boss_spoils_of_pandaria() : CreatureScript("boss_spoils_of_pandaria") { }

        struct boss_spoils_of_pandariaAI : public BossAI
        {
            boss_spoils_of_pandariaAI(Creature* creature) : BossAI(creature, DATA_SPOILS_OF_PANDARIA) { }

            bool hasStarted;
            bool hasDone;
            bool allowAchiev;
            uint32 timerValue;
            uint32 voiceActivatorID;
            uint32 systemActivate;
            std::map<uint64, uint32> crissCross;

            void Reset() override
            {
                _Reset();

                me->setActive(true);
                events.Reset();

                scheduler.CancelAll();
                summons.DespawnAll();
                hasStarted  = false;
                hasDone     = false;
                allowAchiev = true;
                timerValue = 270;
                voiceActivatorID = 0;
                systemActivate = 0;
                crissCross.clear();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->GetMap()->SetWorldState(WORLDSTATE_CRISS_CROSS, 1); // by default
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_SPOILS_OF_PANDARIA, FAIL);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                    {
                        if (hasStarted)
                            break;

                        hasStarted = true;

                        uint32 delay = 3000;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO_1);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 8400), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO_2);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 3700), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO_3);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6700), [this](TaskContext context)
                        {
                            Talk(TALK_INTRO);

                            if (Creature* moguSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_1) : ObjectGuid::Empty))
                            {
                                DoCast(me, SPELL_SPOILS_OF_PANDARIA_AT_LOW_RIGHT, true);
                                moguSpoils->AI()->DoAction(ACTION_INITIALIZE_COMBAT);
                            }

                            if (Creature* mantidSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_1) : ObjectGuid::Empty))
                            {
                                DoCast(me, SPELL_SPOILS_OF_PANDARIA_AT_UP_LEFT, true);
                                mantidSpoils->AI()->DoAction(ACTION_INITIALIZE_COMBAT);
                            }

                            // Unlock 2/4 Sectors
                            if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_LEFT))
                                instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_LEFT), true, NULL);

                            if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_LOW_RIGHT))
                                instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_LOW_RIGHT), true, NULL);

                            DoCast(me, SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_BOX);
                            DoCast(me, SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_BOX_AREA, true);
                            DoCast(me, SPELL_UNSTABLE_DEFENSE_SYSTEM_INITIALIZE);

                            me->SetInCombatWithZone();

                            scheduler
                                .Schedule(Milliseconds(5000), [this](TaskContext context)
                            {
                                systemActivate = 1; // init region check and damage around spoils
                            });

                            // worldstate
                            if (instance)
                            {
                                instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM, 1);
                                instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM_COUNT, timerValue);
                            }
                        });
                        break;
                    }
                    case ACTION_SECOND_QUADRANT:
                        Talk(TALK_SYSTEM_PREPARE_1);

                        if (Creature* mantidSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_2) : ObjectGuid::Empty))
                            mantidSpoils->AI()->DoAction(ACTION_INITIALIZE_COMBAT);

                        if (Creature* moguSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_2) : ObjectGuid::Empty))
                            moguSpoils->AI()->DoAction(ACTION_INITIALIZE_COMBAT);

                        // Remove Frames from old
                        if (Creature* mantidSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_1) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, mantidSpoils);

                        if (Creature* moguSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_1) : ObjectGuid::Empty))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, moguSpoils);

                        // Unlock 4/4 Sectors
                        if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_RIGHT))
                            instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_RIGHT), true, NULL);

                        if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_LOW_LEFT))
                            instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_LOW_LEFT), true, NULL);

                        // Unlock Cross Doors
                        if (instance && instance->GetGuidData(GO_SPOILS_CROSS_RIGHT))
                            instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_CROSS_RIGHT), true, NULL);

                        if (instance && instance->GetGuidData(GO_SPOILS_CROSS_LEFT))
                            instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_CROSS_LEFT), true, NULL);

                        timerValue = 270;

                        // worldstate
                        if (instance)
                            instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM_COUNT, timerValue);
                        break;
                }
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                if (crissCross.find(guid) == crissCross.end())
                    crissCross.insert(std::pair<uint64, uint32>(guid, type));
                else if (crissCross.find(guid)->second != type)
                    allowAchiev = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                scheduler
                    .Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    timerValue--;

                    if (instance)
                        instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM_COUNT, timerValue);

                    if (instance && instance->IsWipe(120.0f, me) || !timerValue)
                    {
                        EnterEvadeMode();
                        return;
                    }
                    context.Repeat(Seconds(1));
                });
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }

            void KilledUnit(Unit* /*victim*/) override { }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_CURRENT_VOICE_CHOISE)
                    voiceActivatorID = data;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_CURRENT_VOICE_CHOISE:
                        return voiceActivatorID;
                    case TYPE_DEFENSE_SYSTEM_ACTIVE:
                        return systemActivate;
                    case TYPE_CRISS_CROSS:
                        return allowAchiev ? 1 : 0;
                }

                return 0;
            }

            void EnterEvadeMode() override
            {
                DoCast(me, SPELL_GB_11010_ARMAGEDDON_CLASS_DEFENSE_SYSTEME);

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                Talk(TALK_WIPE);

                if (instance)
                {
                    instance->SetBossState(DATA_SPOILS_OF_PANDARIA, FAIL);

                    // Controller Reset
                    for (auto&& itr : invControllerLeverType)
                        if (Creature* regionSpoils = ObjectAccessor::GetCreature(*me, instance->GetGuidData(itr.first)))
                            regionSpoils->AI()->DoAction(ACTION_RESET);

                    // Remove Frames
                    for (auto&& itr : invControllerLeverType)
                        if (Creature* regionSpoils = ObjectAccessor::GetCreature(*me, instance->GetGuidData(itr.first)))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, regionSpoils);

                    // Lock Sectors
                    if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_LEFT))
                        instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_LEFT), false, NULL);

                    if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_LOW_RIGHT))
                        instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_LOW_RIGHT), false, NULL);

                    if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_RIGHT))
                        instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_HIGH_RIGHT), false, NULL);

                    if (instance && instance->GetGuidData(GO_SPOILS_ACCESS_LOW_LEFT))
                        instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_ACCESS_LOW_LEFT), false, NULL);

                    // Lock Cross Doors
                    if (instance && instance->GetGuidData(GO_SPOILS_CROSS_RIGHT))
                        instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_CROSS_RIGHT), false, NULL);

                    if (instance && instance->GetGuidData(GO_SPOILS_CROSS_LEFT))
                        instance->HandleGameObject(instance->GetGuidData(GO_SPOILS_CROSS_LEFT), false, NULL);

                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_DEFENSE_SYSTEME_AT_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER_PLAYER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAFF_OF_RESONATING_WATER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLAW_OF_BURNING_ANGER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLADE_OF_THE_HUNDRED_STEPS);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    // Worldstate
                    instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM_COUNT, 0);
                }

                me->RemoveAurasDueToSpell(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_BOX_AREA);
                me->RemoveAllAreasTrigger();
                summons.DespawnAll();
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (hasDone)
                    return;

                hasDone = true;

                _JustDied();

                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_DEFENSE_SYSTEME_AT_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER_PLAYER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAFF_OF_RESONATING_WATER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CLAW_OF_BURNING_ANGER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLADE_OF_THE_HUNDRED_STEPS);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    // Worldstate
                    instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM, 0);
                    instance->DoUpdateWorldState(WORLDSTATE_DEFENSE_SYSTEM_COUNT, 0);

                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_SPOILS_CREDIT, me);
                    instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_SPOILS_CREDIT);

                    if (allowAchiev)
                        instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CRISS_CROSS_ACHIEV);

                    for (auto&& itr : invControllerLeverType)
                        if (Creature* regionSpoils = ObjectAccessor::GetCreature(*me, instance->GetGuidData(itr.first)))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, regionSpoils);

                    // Despawn cosmetic Spoil Box
                    if (GameObject* box = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_SECURED_STOCKPILE_OF_SPOILS)))
                        box->Delete();
                }

                if (GameObject* chest = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(IsHeroic() ? GO_SPOILS_CHEST_HC : GO_SPOILS_CHEST) : ObjectGuid::Empty))
                    instance->DoRespawnGameObject(chest->GetGUID(), 7 * DAY * IN_MILLISECONDS);

                if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                {
                    player->RewardPersonalLootAndCurrency(me);

                    if (Group* group = player->GetGroup())
                        group->UpdateGuildAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_SPOILS_CREDIT, 0, 0, me, me);
                }

                me->RemoveAurasDueToSpell(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_BOX_AREA);
                me->RemoveAllAreasTrigger();
                summons.DespawnAll();

                me->Kill(me, false);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_spoils_of_pandariaAI(creature);
        }
};

// Artifact Spoils 73721, 73720, 73722, 71512
struct npc_spoils_artifact_spoils : public ScriptedAI
{
    npc_spoils_artifact_spoils(Creature* creature) : ScriptedAI(creature) 
    { 
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;
    uint32 powerValue;
    uint32 regionData;
    ObjectGuid leverGUID;

    void Reset() override
    {
        powerValue = 0;
        regionData = 0;
        leverGUID = ObjectGuid::Empty;
        SetData(TYPE_LEVER_DATA, NOT_STARTED);
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return leverGUID;
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_LEVER_DATA:
            {
                regionData = data;

                // break if just special state of lever
                if (regionData != DONE)
                    break;

                // Open second quadrant if prev twice are completed or complete encounter
                bool firstPair = (me->GetEntry() == NPC_MOGU_SPOILS_1 || me->GetEntry() == NPC_MANTID_SPOILS_1);
                uint8 counter = 0;

                if (firstPair)
                {
                    if (Creature* moguSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_1) : ObjectGuid::Empty))
                        if (moguSpoils->AI()->GetData(TYPE_LEVER_DATA) == DONE)
                            counter++;

                    if (Creature* mantidSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_1) : ObjectGuid::Empty))
                        if (mantidSpoils->AI()->GetData(TYPE_LEVER_DATA) == DONE)
                            counter++;

                    if (counter > 1) // switch to second quadrant
                    {
                        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                            spoils->AI()->DoAction(ACTION_SECOND_QUADRANT);
                    }
                }
                else
                {
                    if (Creature* moguSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_2) : ObjectGuid::Empty))
                        if (moguSpoils->AI()->GetData(TYPE_LEVER_DATA) == DONE)
                            counter++;

                    if (Creature* mantidSpoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_2) : ObjectGuid::Empty))
                        if (mantidSpoils->AI()->GetData(TYPE_LEVER_DATA) == DONE)
                            counter++;

                    if (counter > 1) // Complete encounter
                    {
                        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                            spoils->AI()->JustDied(me);
                    }
                }
                break;
            }
            case TYPE_LEVER_POWER:
                if (powerValue >= 50)
                    break;

                powerValue += data;

                if (powerValue >= 50)
                {
                    powerValue = 50;

                    if (Creature* myLever = ObjectAccessor::GetCreature(*me, leverGUID))
                        myLever->CastSpell(myLever, SPELL_UNSTABLE_DEFENSE_SYSTEM_BEAM, true);

                    // Unlock lever to use
                    if (GameObject* lever = ObjectAccessor::GetGameObject(*me, instance ? instance->GetGuidData(invControllerRegionType.find(me->GetEntry())->second[1]) : ObjectGuid::Empty))
                        lever->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_NOT_SELECTABLE);

                    SetData(TYPE_LEVER_DATA, SPECIAL);
                }

                me->SetPower(POWER_ALTERNATE_POWER, powerValue);

                // Send update to players
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_LEVER_DATA:
                return regionData;
            case TYPE_LEVER_POWER:
                return powerValue;
        }

        return 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_INITIALIZE_COMBAT:
            {
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_SPOILS_OF_PANDARIA)))
                    spoils->CastSpell(me, SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER, true);

                DoCast(me, invControllerRegionType.find(me->GetEntry())->second[0], true);
                if (Creature* lever = me->SummonCreature(NPC_SPOILS_OF_PANDARIA_LEVER, invControllerLeverType.find(me->GetEntry())->second, TEMPSUMMON_MANUAL_DESPAWN))
                    leverGUID = lever->GetGUID();

                SetData(TYPE_LEVER_DATA, IN_PROGRESS);
                break;
            }
            case ACTION_RESET:
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->RemoveAurasDueToSpell(SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER);
                Reset();
                break;
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Lever 72281
struct npc_spoils_lever : public ScriptedAI
{
    npc_spoils_lever(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);

        ownerGUID = summoner->GetGUID();
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return ownerGUID;
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

struct spoils_baseAI : public ScriptedAI
{
    spoils_baseAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    InstanceScript* instance;
    EventMap events;
    SummonList summons;
    ObjectGuid summonerGUID;
    ObjectGuid targetGUID;
    uint32 windCount;
    uint32 mantidSwarmCount;
    uint32 statueCount;
    uint8 targetsCount;
    std::list<Position> swapList;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
        events.Reset();
        windCount  = 0;
        targetGUID = ObjectGuid::Empty;
        targetsCount = 2;
        mantidSwarmCount = 0;
        statueCount = 0;

        // Just summoned couldn`t have called cuz summoned by object
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && (!me->IsWithinLOS(victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ()) || victim->GetPositionZ() > -274.5f))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // try select near target and attack him if possible
            if (Player* target = me->FindNearestPlayer(30.0f))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        summonerGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return summonerGUID;
    }

    void JustDied(Unit* /*killer*/) override
    {
        bool mogu = IsMogu(me->GetEntry());
        int32 bp = mogu ? (int32)moguCreatures.find(me->GetEntry())->second : (int32)mantidCreatures.find(me->GetEntry())->second;

        // Some creatures not gain energy
        if (!bp)
            return;

        // Achievement
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
        {
            if (spoils->AI()->GetData(TYPE_CRISS_CROSS)) // allow achiev, no reason to check this if not available
            {
                std::list<Player*> achievTargets;
                GetPlayerListInGrid(achievTargets, me, 70.0f);

                achievTargets.remove_if([=](Player* target) { return target && !target->IsWithinLOS(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ()); });

                for (auto&& itr : achievTargets)
                    spoils->AI()->SetGUID(itr->GetGUID(), mogu ? 1 : 2);
            }
        }

        // For Mogu
        if (mogu)
        {
            if (Creature* moguController1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_1) : ObjectGuid::Empty))
            {
                if (moguController1->AI()->GetData(TYPE_LEVER_DATA) == IN_PROGRESS)
                {
                    if (Creature* currentMoguLever = ObjectAccessor::GetCreature(*me, moguController1->AI()->GetGUID()))
                    {
                        me->CastSpell(currentMoguLever, SPELL_UNSTABLE_DEFENSE_SYSTEM_LINK_TO_LEVER, true);
                        me->CastCustomSpell(currentMoguLever, SPELL_SPOILS_OF_PANDARIA_LEVER_EFF, &bp, 0, 0, true);

                        if (IsHeroic())
                        {
                            if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                                spoils->CastSpell(sparkSpawnRoomPos[0].GetPositionX() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[0].GetPositionY() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[0].GetPositionZ(), SPELL_UNSTABLE_SPARK_MISSLE, true);
                        }
                    }
                }
            }

            if (Creature* moguController2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_2) : ObjectGuid::Empty))
            {
                if (moguController2->AI()->GetData(TYPE_LEVER_DATA) == IN_PROGRESS)
                {
                    if (Creature* currentMoguLever = ObjectAccessor::GetCreature(*me, moguController2->AI()->GetGUID()))
                    {
                        me->CastSpell(currentMoguLever, SPELL_UNSTABLE_DEFENSE_SYSTEM_LINK_TO_LEVER, true);
                        me->CastCustomSpell(currentMoguLever, SPELL_SPOILS_OF_PANDARIA_LEVER_EFF, &bp, 0, 0, true);

                        if (IsHeroic())
                        {
                            if (Creature* mantidController2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_2) : ObjectGuid::Empty))
                                mantidController2->CastSpell(sparkSpawnRoomPos[2].GetPositionX() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[2].GetPositionY() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[2].GetPositionZ(), SPELL_UNSTABLE_SPARK_MISSLE, true);
                        }
                    }
                }
            }
        }
        else
        {
            // For Mantids
            if (Creature* mantidController1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_1) : ObjectGuid::Empty))
            {
                if (mantidController1->AI()->GetData(TYPE_LEVER_DATA) == IN_PROGRESS)
                {
                    if (Creature* currentMoguLever = ObjectAccessor::GetCreature(*me, mantidController1->AI()->GetGUID()))
                    {
                        me->CastSpell(currentMoguLever, SPELL_UNSTABLE_DEFENSE_SYSTEM_LINK_TO_LEVER, true);
                        me->CastCustomSpell(currentMoguLever, SPELL_SPOILS_OF_PANDARIA_LEVER_EFF, &bp, 0, 0, true);

                        if (IsHeroic())
                        {
                            if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                                spoils->CastSpell(sparkSpawnRoomPos[1].GetPositionX() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[1].GetPositionY() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[1].GetPositionZ(), SPELL_UNSTABLE_SPARK_MISSLE, true);
                        }
                    }
                }
            }

            if (Creature* mantidController2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_SPOILS_2) : ObjectGuid::Empty))
            {
                if (mantidController2->AI()->GetData(TYPE_LEVER_DATA) == IN_PROGRESS)
                {
                    if (Creature* currentMoguLever = ObjectAccessor::GetCreature(*me, mantidController2->AI()->GetGUID()))
                    {
                        me->CastSpell(currentMoguLever, SPELL_UNSTABLE_DEFENSE_SYSTEM_LINK_TO_LEVER, true);
                        me->CastCustomSpell(currentMoguLever, SPELL_SPOILS_OF_PANDARIA_LEVER_EFF, &bp, 0, 0, true);

                        if (IsHeroic())
                        {
                            if (Creature* moguController2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MOGU_SPOILS_2) : ObjectGuid::Empty))
                                moguController2->CastSpell(sparkSpawnRoomPos[3].GetPositionX() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[3].GetPositionY() + frand(-8.0f, 8.0f), sparkSpawnRoomPos[3].GetPositionZ(), SPELL_UNSTABLE_SPARK_MISSLE, true);
                        }
                    }
                }
            }
        }
    }

    ObjectGuid GetCasterGuidIfPossible()
    {
        std::list<Player*> targets, m_targets;
        GetPlayerListInGrid(targets, me, 70.0f);
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (!targets.empty())
            return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (!targets.empty())
            return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

        return ObjectGuid::Empty;
    }
};

// Modified Anima Golem 71395
struct npc_modified_anima_golem : public spoils_baseAI
{
    npc_modified_anima_golem(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MATTER_SCRAMBLE, 4 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CRIMSON_RECONSTRUCT, urand(6 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
    }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        if (type > 0)
        {
            summonerGUID = guid;
            return;
        }

        if (swapList.empty())
            return;

        if (Player* target = ObjectAccessor::GetPlayer(*me, guid))
        {
            auto swapPos = *swapList.begin();
            target->NearTeleportTo(swapPos.GetPositionX(), swapPos.GetPositionY(), swapPos.GetPositionZ(), target->GetOrientation());
            swapList.pop_front();
        }
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_MATTER_SWAP:
            {
                float x, y, z;
                swapList.clear();

                for (uint8 i = 0; i < targetsCount; i++)
                {
                    me->GetClosePoint(x, y, z, 3.0f, 4.0f * (i + 1), frand(0.0f, 2 * M_PI));
                    me->SendPlaySpellVisual(SPELL_ANIMA_VISUAL_ID, x, y, z, 5.0f);
                    Position pos = { x, y, z, 0.0f };
                    swapList.push_back(pos);
                }

                targetsCount++;
                break;
            }
            case ACTION_MATTER_SWAP_EFF:
                for (auto&& itr : swapList)
                    me->CastSpell(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ(), SPELL_MATTER_SCRAMBLE_CHECK, true);
                break;
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
                case EVENT_MATTER_SCRAMBLE:
                    DoCast(me, SPELL_MATTER_SCRAMBLE_VISUAL);
                    events.ScheduleEvent(EVENT_MATTER_SCRAMBLE, urand(13 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                    break;
                case EVENT_CRIMSON_RECONSTRUCT:
                    DoCast(me, SPELL_CRIMSON_RECONSTRUCTION_SELECTOR);
                    events.ScheduleEvent(EVENT_CRIMSON_RECONSTRUCT, 12 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Mogu Shadow Ritualist 71393
struct npc_mogu_shadow_ritualist : public spoils_baseAI
{
    npc_mogu_shadow_ritualist(Creature* creature) : spoils_baseAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        DoCast(summoner, SPELL_MOGU_SHADOW_RITUALIST_SHARE);
        summoner->CastSpell(me, SPELL_MOGU_SHADOW_RITUALIST_SHARE, true);
        me->SetInCombatWithZone();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RUNE_OF_POWER, 2.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_TORMENT, 7.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FORBIDDEN_MAGIC, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_TORMENT_LINK && me->IsAlive())
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                DoCast(target, SPELL_TORMENT_LINK);
            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                DoCast(target, SPELL_TORMENT_LINK);
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
                case EVENT_TORMENT:
                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                        DoCast(target, SPELL_TORMENT);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                        DoCast(target, SPELL_TORMENT);

                    events.ScheduleEvent(EVENT_TORMENT, urand(11 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                    break;
                case EVENT_FORBIDDEN_MAGIC:
                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                        DoCast(target, SPELL_FORBIDDEN_MAGIC);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                        DoCast(target, SPELL_FORBIDDEN_MAGIC);

                    events.ScheduleEvent(EVENT_FORBIDDEN_MAGIC, 12 * IN_MILLISECONDS);
                    break;
                case EVENT_RUNE_OF_POWER:
                    DoCast(me, SPELL_MOGU_RUNE_OF_POWER);
                    events.ScheduleEvent(EVENT_FORBIDDEN_MAGIC, 10 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Zarthik Amber Priest 71397
struct npc_zarthik_amber_priest : public spoils_baseAI
{
    npc_zarthik_amber_priest(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RESIDUE, 7.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MANTID_SWARM, urand(3 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_MANTID_SWARM_COUNT)
            return mantidSwarmCount;

        return 0;
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
                case EVENT_RESIDUE:
                    DoCast(me, SPELL_RESIDUE);
                    events.ScheduleEvent(EVENT_RESIDUE, urand(9 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                    break;
                case EVENT_MANTID_SWARM:
                    mantidSwarmCount++;
                    DoCast(me, SPELL_MANTID_SWARM);
                    events.ScheduleEvent(EVENT_MANTID_SWARM, 12 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Setthik Wind Wielder 71405
struct npc_setthik_wielder : public spoils_baseAI
{
    npc_setthik_wielder(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RAGE_OF_EMPRESS, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_WIND_STORM, 1.5 * IN_MILLISECONDS);
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
                case EVENT_RAGE_OF_EMPRESS:
                    DoCast(me, SPELL_RAGE_OF_EMPRESS);
                    events.ScheduleEvent(EVENT_RAGE_OF_EMPRESS, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                    break;
                case EVENT_WIND_STORM:
                    if (++windCount < 4)
                    {
                        if (Unit* target = me->GetVictim())
                            targetGUID = target->GetGUID();

                        me->PrepareChanneledCast(me->GetOrientation(), SPELL_WINDSTORM);
                        events.ScheduleEvent(EVENT_WIND_STORM, 1.2 * IN_MILLISECONDS);
                    }
                    else
                    {
                        windCount = 0;
                        me->RemoveChanneledCast(targetGUID);
                        events.ScheduleEvent(EVENT_WIND_STORM, 21 * IN_MILLISECONDS);
                    }
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Animated Stone Mogu 71380
struct npc_animated_stone_mogu : public spoils_baseAI
{
    npc_animated_stone_mogu(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_EARTHEN_SHARD, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_HARDEN_FLESH, 10 * IN_MILLISECONDS);
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
                case EVENT_EARTHEN_SHARD:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, DpsSpecTargetSelector()))
                        DoCast(target, SPELL_EARTHEN_SHARD);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                        DoCast(target, SPELL_EARTHEN_SHARD);
                
                    events.ScheduleEvent(EVENT_EARTHEN_SHARD, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
                case EVENT_HARDEN_FLESH:
                    DoCast(me, SPELL_HARDEN_FLESH);
                    events.ScheduleEvent(EVENT_EARTHEN_SHARD, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Burial Urn 71382
struct npc_burial_urn : public spoils_baseAI
{
    npc_burial_urn(Creature* creature) : spoils_baseAI(creature) { }

    void DoAction(int32 actionId) override
    {
        DoCast(me, SPELL_SPARK_OF_LIFE);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;
    }
};

// Shadow Ritualist Phylactery 71392
struct npc_shadow_ritualist_phylactery : public spoils_baseAI
{
    npc_shadow_ritualist_phylactery(Creature* creature) : spoils_baseAI(creature) { }

    void DoAction(int32 actionId) override
    {
        DoCast(me, SPELL_MOGU_SHADOW_RITUALIST);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;
    }
};

// Quilen Guardian Controller 72223
struct npc_quilen_guardians_controller : public spoils_baseAI
{
    npc_quilen_guardians_controller(Creature* creature) : spoils_baseAI(creature) { }

    float x, y;
    uint8 quilensCount;

    void DoAction(int32 actionId) override
    {
        for (uint8 i = 1; i < 5; i++)
        {
            GetPositionWithDistInOrientation(me, 0.5f, (M_PI / 4) * i , x, y);
            me->SummonCreature(NPC_QUILEN_GUARDIANS_MAIN, x, y, me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
            quilensCount = 0;
        }
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        summons.Despawn(summon);

        if (++quilensCount > 3) // calculate
            me->Kill(me);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        summon->SetInCombatWithZone();
    }
};

// Quilen Guardian 71378
struct npc_spoils_quilen_guardian : public spoils_baseAI
{
    npc_spoils_quilen_guardian(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CARNIVOROUS_BITE, urand(2.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon(1 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_CARNIVOROUS_BITE)
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, DpsSpecTargetSelector()))
                    DoCast(target, SPELL_CARNIVOROUS_BITE);
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 80.0f, true))
                    DoCast(target, SPELL_CARNIVOROUS_BITE);

                events.ScheduleEvent(EVENT_CARNIVOROUS_BITE, urand(2.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Srithik Bombardier 71385
struct npc_srithik_bombardier : public spoils_baseAI
{
    npc_srithik_bombardier(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_THROW_EXPLOSIVES, urand(1.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_GUSTING_BOMB, 8 * IN_MILLISECONDS);
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
                case EVENT_THROW_EXPLOSIVES:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        DoCast(target, SPELL_THROW_EXPLOSIVES);

                    events.ScheduleEvent(EVENT_THROW_EXPLOSIVES, urand(2.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                    break;
                case EVENT_GUSTING_BOMB:
                    DoCast(me, SPELL_GUSTING_BOMB);
                    events.ScheduleEvent(EVENT_GUSTING_BOMB, 9.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Korthik Warcaller 71383
struct npc_korthik_warcaller : public spoils_baseAI
{
    npc_korthik_warcaller(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ENRAGE, urand(1.5 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ENRAGE)
            {
                DoCast(me, SPELL_ENRAGE);
                events.ScheduleEvent(EVENT_ENRAGE, 9 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Mogu Warlord 73723, 71408, 73725, 73724
struct npc_spoils_mogu_warlord : public spoils_baseAI
{
    npc_spoils_mogu_warlord(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_JADE_TEMPEST, 6 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_RETURN_TO_STONE, 3 * IN_MILLISECONDS);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_RETURN_TO_STONE_VALUE)
            return statueCount;

        return 0;
    }

    void JustSummoned(Creature* summon) override
    {
        if (!me->HasAura(SPELL_STRENGTH_OF_THE_STONE))
            DoCast(me, SPELL_STRENGTH_OF_THE_STONE);

        summons.Summon(summon);
        statueCount++;
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        summons.Despawn(summon);
        statueCount--;
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
                case EVENT_JADE_TEMPEST:
                    DoCast(me, invMoguWarlordsType.find(me->GetEntry())->second);
                    events.ScheduleEvent(EVENT_JADE_TEMPEST, 20 * IN_MILLISECONDS);
                    break;
                case EVENT_RETURN_TO_STONE:
                    for (uint8 i = 0; i < 2; i++)
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                            DoCast(target, SPELL_RETURN_TO_STONE_EFF_WARLORD);
                        else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            DoCast(target, SPELL_RETURN_TO_STONE_EFF_WARLORD);
                    }
                    events.ScheduleEvent(EVENT_RETURN_TO_STONE, 11 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Mantid Commander 73951, 73948, 73949
struct npc_spoils_mantid_commander : public spoils_baseAI
{
    npc_spoils_mantid_commander(Creature* creature) : spoils_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_PHEROMONES_CLOUD, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SET_TO_BLOW, urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
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
                case EVENT_PHEROMONES_CLOUD:
                    DoCast(me, SPELL_PHEROMONE_CLOUD);
                    break;
                case EVENT_SET_TO_BLOW:
                    DoCast(me, SPELL_SET_TO_BLOW);
                    events.ScheduleEvent(EVENT_SET_TO_BLOW, 9.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Amber Encased Kunchong 71388
struct npc_spoils_amber_encased_kunchong : public spoils_baseAI
{
    npc_spoils_amber_encased_kunchong(Creature* creature) : spoils_baseAI(creature) { }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INITIALIZE_COMBAT)
            DoCast(me, SPELL_ENCAPSULATED_PHEROMONES_AURA);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Lift Hook 72972
struct npc_spoils_lift_hook : public ScriptedAI
{
    npc_spoils_lift_hook(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasPassenger;
    float x, y, oriDiff;
    ObjectGuid clickerGUID;

    void Reset() override
    {
        me->SetDisplayFromModel(1);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        hasPassenger = false;
        clickerGUID = ObjectGuid::Empty;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasPassenger)
            return;

        hasPassenger = true;
        oriDiff = me->GetPositionX() > 1630.0f ? Position::NormalizeOrientation(0.55f - M_PI) : 0.55f;

        clickerGUID = clicker->GetGUID();
        clicker->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), -261.14f);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                GetPositionWithDistInOrientation(me, 15.0f, oriDiff, x, y);
                me->GetMotionMaster()->MovePoint(0, x, y, -261.14f);

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    if (me->GetVehicleKit())
                        me->GetVehicleKit()->RemoveAllPassengers();

                    if (Player* target = ObjectAccessor::GetPlayer(*me, clickerGUID))
                        target->CastSpell(target, SPELL_SPRINT, true);

                    me->GetMotionMaster()->MovePoint(0, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), -261.14f);

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetHomePosition().GetPositionZ());

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                        {
                            clickerGUID = ObjectGuid::Empty;
                            hasPassenger = false;
                        });
                    });
                });
            });
        });
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Ancient Brewmaster Spirit 71427
struct npc_spoils_ancient_brewmaster_spirit : public ScriptedAI
{
    npc_spoils_ancient_brewmaster_spirit(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap events;
    ObjectGuid summonerGUID;
    ObjectGuid targetGUID;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
        events.Reset();
        targetGUID = ObjectGuid::Empty;

        // Temp Hackfix
        if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
        {
            // should be 7.1 in 10 normal
            me->SetCreateHealth(7100000);
            me->SetMaxHealth(7100000);
            me->SetHealth(7100000);
            me->ResetPlayerDamageReq();
        }

        // Just summoned couldn`t have called cuz summoned by object
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && (!me->IsWithinLOS(victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ()) || victim->GetPositionZ() > -274.5f))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // try select near target and attack him if possible
            if (Player* target = me->FindNearestPlayer(30.0f))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        summonerGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return summonerGUID;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_KEG_TOSS, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BREATH_OF_FIRE, urand(6 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->RemoveAllAreasTrigger();
        me->SummonCreature(invPandarensType.find(me->GetEntry())->second[0], *me, TEMPSUMMON_TIMED_DESPAWN, 3 * MINUTE * IN_MILLISECONDS);
        me->DisappearAndDie();
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
                case EVENT_KEG_TOSS:
                    DoCast(me, SPELL_KEG_TOSS_LAUNCHER);

                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                        DoCast(target, SPELL_KEG_TOSS);
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                        DoCast(target, SPELL_KEG_TOSS);

                    events.ScheduleEvent(EVENT_KEG_TOSS, 10 * IN_MILLISECONDS);
                    break;
                case EVENT_BREATH_OF_FIRE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_BREATH_OF_FIRE);

                    events.ScheduleEvent(EVENT_BREATH_OF_FIRE, 14.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    private:
        ObjectGuid GetCasterGuidIfPossible()
        {
            std::list<Player*> targets, m_targets;
            GetPlayerListInGrid(targets, me, 70.0f);
            m_targets = targets;

            targets.remove_if(TankSpecTargetSelector());
            targets.remove_if(MeeleSpecTargetSelector());

            if (!targets.empty())
                return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (!targets.empty())
                return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

            return ObjectGuid::Empty;
        }
};

// Wise Mistweaver Spirit 71428
struct npc_spoils_wise_mistweaver_spirit : public ScriptedAI
{
    npc_spoils_wise_mistweaver_spirit(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap events;
    ObjectGuid summonerGUID;
    ObjectGuid targetGUID;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
        events.Reset();
        targetGUID = ObjectGuid::Empty;

        // Temp Hackfix
        if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
        {
            // should be 7.1 in 10 normal
            me->SetCreateHealth(7100000);
            me->SetMaxHealth(7100000);
            me->SetHealth(7100000);
            me->ResetPlayerDamageReq();
        }

        // Just summoned couldn`t have called cuz summoned by object
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && (!me->IsWithinLOS(victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ()) || victim->GetPositionZ() > -274.5f))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // try select near target and attack him if possible
            if (Player* target = me->FindNearestPlayer(30.0f))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        summonerGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return summonerGUID;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_EMINENCE);
        events.ScheduleEvent(EVENT_GUSTING_CRANE_KICK, 3 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->RemoveAllAreasTrigger();
        me->SummonCreature(invPandarensType.find(me->GetEntry())->second[0], *me, TEMPSUMMON_TIMED_DESPAWN, 3 * MINUTE * IN_MILLISECONDS);
        me->DisappearAndDie();
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
            if (eventId == EVENT_GUSTING_CRANE_KICK)
            {
                DoCast(me, SPELL_GUSTING_CRANE_KICK);
                events.ScheduleEvent(EVENT_GUSTING_CRANE_KICK, 18.5 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Nameless Windwalker Spirit 71430
struct npc_spoils_nameless_windwalker_spirit : public ScriptedAI
{
    npc_spoils_nameless_windwalker_spirit(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    TaskScheduler scheduler;
    EventMap events;
    ObjectGuid summonerGUID;
    ObjectGuid targetGUID;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
        events.Reset();
        targetGUID = ObjectGuid::Empty;

        // Temp Hackfix
        if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
        {
            // should be 7.1 in 10 normal
            me->SetCreateHealth(7100000);
            me->SetMaxHealth(7100000);
            me->SetHealth(7100000);
            me->ResetPlayerDamageReq();
        }

        // Just summoned couldn`t have called cuz summoned by object
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && (!me->IsWithinLOS(victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ()) || victim->GetPositionZ() > -274.5f))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // try select near target and attack him if possible
            if (Player* target = me->FindNearestPlayer(30.0f))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        summonerGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return summonerGUID;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_PATH_OF_BLOSSOMS, 6.5 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->RemoveAllAreasTrigger();
        me->SummonCreature(invPandarensType.find(me->GetEntry())->second[0], *me, TEMPSUMMON_TIMED_DESPAWN, 3 * MINUTE * IN_MILLISECONDS);
        me->DisappearAndDie();
    }

    void MovementInform(uint32 /*type*/, uint32 pointId) override
    {
        if (pointId == EVENT_CHARGE)
        {
            me->StopMoving();
            me->GetMotionMaster()->Clear();

            if (Unit* target = me->GetVictim())
                targetGUID = target->GetGUID();

            me->PrepareChanneledCast(me->GetOrientation());

            scheduler
                .Schedule(Milliseconds(500), [this](TaskContext context)
            {
                DoCast(me, SPELL_MASS_PARALYSIS);
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
            if (eventId == EVENT_PATH_OF_BLOSSOMS)
            {
                if (Unit* target = ObjectAccessor::GetUnit(*me, GetCasterGuidIfPossible()))
                    DoCast(target, SPELL_PATH_OF_BLOSSOMS);
                else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                    DoCast(target, SPELL_PATH_OF_BLOSSOMS);

                events.ScheduleEvent(EVENT_PATH_OF_BLOSSOMS, 20 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }

private:
    ObjectGuid GetCasterGuidIfPossible()
    {
        std::list<Player*> targets, m_targets;
        GetPlayerListInGrid(targets, me, 70.0f);
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (!targets.empty())
            return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (!targets.empty())
            return Trinity::Containers::SelectRandomContainerElement(targets)->GetGUID();

        return ObjectGuid::Empty;
    }
};

// Spark of Life 71433
struct npc_spoils_spark_of_life : public ScriptedAI
{
    npc_spoils_spark_of_life(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y, z;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void Reset() override
    {
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING); // box collision issue
        DoCast(me, SPELL_SPARK_OF_LIFE_VISUAL);
        DoCast(me, SPELL_PULSE);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            me->GetClosePoint(x, y, z, 3.0f, 15.0f, frand(0.0f, 2 * M_PI));
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            context.Repeat(Milliseconds(me->GetSplineDuration() + 2000));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DisappearAndDie();
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Pandarens Gift 72810, 72787, 72828
struct npc_spoils_pandarens_gift : public ScriptedAI
{
    npc_spoils_pandarens_gift(Creature* creature) : ScriptedAI(creature) { }

    uint32 summonerEntry;
    bool hasUsed;

    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->ToCreature())
            me->SetDisplayId(summoner->ToCreature()->GetCreatureTemplate()->GetModelByIdx(0)->CreatureDisplayID); // for visible, no data in sniffs more than it (should`ve a green weapon under circle like me remember)

        summonerEntry = summoner->GetEntry();

        if (Creature* spoils = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void Reset() override
    {
        hasUsed = false;
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        me->SendPlaySpellVisualKit(SPELL_WEAPON_KIT, 0, 0);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasUsed)
            return;

        hasUsed = true;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

        auto weaponPtr = invPandarensType.find(summonerEntry);
        uint32 weaponForRole = weaponPtr->second[2];

        std::list<Player*> targets;
        GetPlayerListInGrid(targets, me, 70.0f);

        targets.remove_if([=](Player* target) { return target && target->GetRoleForGroup(target->GetTalentSpecialization()) != weaponForRole; });

        for (auto&& itr : targets)
        {
            if (summonerEntry == NPC_NAMELESSE_WINDWALLKER_SPIRIT)
                itr->CastSpell(itr, weaponPtr->second[1], true);
            else
                me->CastSpell(itr, weaponPtr->second[1], true);
        }

        me->SetVisible(false);
    }
};

// Zarthik Swarmer 71398
struct npc_spoils_zarthik_swarmer : public ScriptedAI
{
    npc_spoils_zarthik_swarmer(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        me->OverrideInhabitType(INHABIT_AIR);
        me->UpdateMovementFlags();

        if (Creature* spoils = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void AttackStart(Unit* victim) override
    {
        if (victim && (!me->IsWithinLOS(victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ()) || victim->GetPositionZ() > -274.5f))
        {
            me->GetThreatManager().doAddThreat(victim, -10000.0f); // remove thread from this target to exclude new pull

            // try select near target and attack him if possible
            if (Player* target = me->FindNearestPlayer(30.0f))
                if (me->Attack(target, true))
                    me->GetMotionMaster()->MoveChase(target);

            return;
        }

        if (victim && me->Attack(victim, true))
            me->GetMotionMaster()->MoveChase(victim);
    }

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 10.0f);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->OverrideInhabitType(INHABIT_GROUND);
                me->UpdateMovementFlags();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                me->SetInCombatWithZone();
            });
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Stone Statue 72535
struct npc_spoils_stone_statue_warlord : public ScriptedAI
{
    npc_spoils_stone_statue_warlord(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void SetGUID(ObjectGuid guid, int32 type) override
    {
        ownerGUID = guid;
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->DespawnOrUnsummon(0.5 * IN_MILLISECONDS);
    }

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, ownerGUID))
            {
                me->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() - 10.0f);

                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                {
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                    me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_ANIMATED_STRIKE);
                    context.Repeat(Milliseconds(3500));
                });
            }
            else
            {
                scheduler
                    .Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED))
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                    me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_ANIMATED_STRIKE);
                    context.Repeat(Milliseconds(3500));
                });
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Unstable Spark 73104
struct npc_spoils_unstable_spark : public ScriptedAI
{
    npc_spoils_unstable_spark(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasTriggered;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* spoils = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
            spoils->AI()->JustSummoned(me);
    }

    void Reset() override
    {
        me->SetInCombatWithZone();
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        DoCast(me, SPELL_UNSTABLE_SPARK_VISUAL_2);
        hasTriggered = false;

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            DoCast(me, SPELL_SUPERNOVA);
        });
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_BLADE_OF_THE_HUNDRED_STEPS_EFF)
        {
            scheduler
                .Schedule(Milliseconds(2000), [this](TaskContext context)
            {
                if (!hasTriggered)
                    DoCast(me, SPELL_SUPERNOVA);
            });
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO) // fake death
        {
            me->InterruptNonMeleeSpells(false, SPELL_SUPERNOVA);
            me->RemoveAllAuras();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (hasTriggered)
            damage = 0;

        if (damage >= me->GetHealth() && !hasTriggered)
        {
            hasTriggered = true;
            damage = 0;
            DoAction(ACTION_START_INTRO);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};


// Secured Stockpile of Spoils 220823
class go_spoils_secured_stockpile_of_spoils : public GameObjectScript
{
    public:
        go_spoils_secured_stockpile_of_spoils() : GameObjectScript("go_spoils_secured_stockpile_of_spoils") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->GetInstanceScript() && (player->GetInstanceScript()->GetBossState(DATA_SPOILS_OF_PANDARIA) == DONE || player->GetInstanceScript()->GetBossState(DATA_SPOILS_OF_PANDARIA) == IN_PROGRESS))
                return true;

            // Choise 3 strings of voice system
            std::list<uint32> leadersVoice = { 0, 1, 2, 3, 4 };
            Trinity::Containers::RandomResizeList(leadersVoice, 3);

            for (auto&& itr : leadersVoice)
                player->ADD_GOSSIP_ITEM_DB(SPOILS_GOSSIP_MENU, itr, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1 + itr);

            player->SEND_GOSSIP_MENU(SPOILS_TEXT_ID, go->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            // Launch
            if (action > GOSSIP_ACTION_INFO_DEF + 5)
            {
                uint32 choisedVoice = invSystemVoiceType.find(action)->second;

                if (Creature* spoils = ObjectAccessor::GetCreature(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                {
                    if (choisedVoice == spoils->AI()->GetData(TYPE_CURRENT_VOICE_CHOISE))
                        spoils->AI()->DoAction(ACTION_START_INTRO);
                }

                player->CLOSE_GOSSIP_MENU();
            }
            else
            {
                // Choise 3 strings of voice system varianty include right
                std::list<uint32> leadersVoiceChoise = { 5, 6, 7, 8, 9 };
                uint32 mainVarianty = invSystemVoiceType.find(action)->second;

                if (std::find(leadersVoiceChoise.begin(), leadersVoiceChoise.end(), mainVarianty) != leadersVoiceChoise.end())
                    leadersVoiceChoise.erase(std::find(leadersVoiceChoise.begin(), leadersVoiceChoise.end(), mainVarianty));

                Trinity::Containers::RandomResizeList(leadersVoiceChoise, 2);
                leadersVoiceChoise.push_back(mainVarianty);

                // Mix this
                std::vector<uint32> mixedVoiceChoise(leadersVoiceChoise.begin(), leadersVoiceChoise.end());
                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(mixedVoiceChoise.begin(), mixedVoiceChoise.end(), g);                

                // save this value for check
                if (Creature* spoils = ObjectAccessor::GetCreature(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                    spoils->AI()->SetData(TYPE_CURRENT_VOICE_CHOISE, mainVarianty);

                for (auto&& itr : mixedVoiceChoise)
                    player->ADD_GOSSIP_ITEM_DB(SPOILS_GOSSIP_MENU, itr, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + itr + 1);

                player->SEND_GOSSIP_MENU(SPOILS_TEXT_ID, go->GetGUID());
            }
            return true;
        }
};

// Spoils Lever 221771, 221772, 221773, 221774
class go_spoils_lever : public GameObjectScript
{
    public:
        go_spoils_lever() : GameObjectScript("go_spoils_lever") { }

        void OnGameObjectStateChanged(GameObject* go, uint32 state) override
        {
            if (state == GO_STATE_ACTIVE && go->GetInstanceScript())
            {
                uint32 regionEntry = 0;

                // Get our region controller ID
                for (auto&& itr : invControllerRegionType)
                    if (itr.second[1] == go->GetEntry())
                        regionEntry = itr.first;

                // Send data to this
                if (Creature* regionController = ObjectAccessor::GetCreature(*go, go->GetInstanceScript()->GetGuidData(regionEntry)))
                    regionController->AI()->SetData(TYPE_LEVER_DATA, DONE);
            }
        }
};

// Unstable Defense System Initialize 144256
class spell_spoils_unstable_defense_system_initialize : public SpellScript
{
    PrepareSpellScript(spell_spoils_unstable_defense_system_initialize);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (GameObject* box = GetHitGObj())
                caster->CastSpell(box->GetPositionX(), box->GetPositionY(), box->GetPositionZ(), SPELL_UNSTABLE_DEFENSE_SYSTEM_INIT_VISUAL, true);
    }

    void HandleInitializeBox(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (GameObject* box = GetHitGObj())
            {
                // initialize box template
                auto key = invSpoilBoxesType.find(box->GetEntry());

                uint32 npcEntry = 0;
                if (key->second[1])
                    npcEntry = urand(0, 1) ? key->second[0] : key->second[1];
                else
                    npcEntry = key->second[0];

                if (Creature* loot = box->SummonCreature(npcEntry, *box, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    loot->AI()->SetGUID(box->GetGUID(), 1);
                    loot->SetVisible(false);
                }
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToGameObject() && !isBox(target->ToGameObject()); });
    }

    bool isBox(GameObject* box)
    {
        for (auto&& itr : invSpoilBoxesType)
            if (box->GetEntry() == itr.first)
                return true;

        return false;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_unstable_defense_system_initialize::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnEffectHitTarget += SpellEffectFn(spell_spoils_unstable_defense_system_initialize::HandleInitializeBox, EFFECT_1, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spoils_unstable_defense_system_initialize::FilterTargets, EFFECT_0, TARGET_GAMEOBJECT_DEST_AREA);
    }
};

// Breaking 144229
class spell_spoils_breaking : public SpellScript
{
    PrepareSpellScript(spell_spoils_breaking);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (GameObject* box = GetHitGObj())
        {
            std::list<Creature*> spoilLoots;
            
            // we won`t activate not our npc...
            for (auto&& itr : mantidCreatures)
                GetCreatureListWithEntryInGrid(spoilLoots, box, itr.first, 10.0f);

            for (auto&& itr : moguCreatures)
                GetCreatureListWithEntryInGrid(spoilLoots, box, itr.first, 10.0f);

            for (auto&& itr : pandarenCreatures)
                GetCreatureListWithEntryInGrid(spoilLoots, box, itr.first, 10.0f);

            spoilLoots.remove_if([=](Creature* target) { return target && target->AI()->GetGUID() != box->GetGUID(); });

            for (auto&& itr : spoilLoots)
            {
                if (itr->GetEntry() == NPC_ANCIENT_BREWMASTER_SPIRIT || itr->GetEntry() == NPC_WISE_MISTEWEAVER_SPIRIT || itr->GetEntry() == NPC_NAMELESSE_WINDWALLKER_SPIRIT)
                    itr->CastSpell(itr, SPELL_PANDAREN_SPIRIT_SPAWN, true);
                else
                    itr->CastSpell(itr, IsMogu(itr->GetEntry()) ? SPELL_STONE_STATUE : SPELL_AMBER_SHELL, true);

                itr->AI()->DoAction(ACTION_INITIALIZE_COMBAT);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_breaking::HandleHit, EFFECT_0, SPELL_EFFECT_ACTIVATE_OBJECT);
    }
};

// Amber Shell 145828, Stone Statue 142526
class spell_spoils_stone_statue : public AuraScript
{
    PrepareAuraScript(spell_spoils_stone_statue)

    void HandleApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetVisible(true);
            if (owner->GetEntry() != NPC_QUILEN_GUARDIANS)
                owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            owner->RemoveAurasDueToSpell(SPELL_RETURN_TO_STONE);
            owner->RemoveAurasDueToSpell(SPELL_DIAGNOSTIC_SEQUENCE);
        }
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (owner->GetEntry() != NPC_AMBER_ENCASED_KUNCHONG && owner->GetEntry() != NPC_BURIAL_URN && owner->GetEntry() != NPC_SHADOW_RITUALIST_PHYLACTERY && owner->GetEntry() != NPC_QUILEN_GUARDIANS)
                owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

            owner->SetInCombatWithZone();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_spoils_stone_statue::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_spoils_stone_statue::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Pandaren Spirit 145574
class spell_spoils_pandaren_spirit_spawn : public AuraScript
{
    PrepareAuraScript(spell_spoils_pandaren_spirit_spawn)

    void HandleApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetVisible(true);
            owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            owner->SetInCombatWithZone();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_spoils_pandaren_spirit_spawn::HandleApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_spoils_pandaren_spirit_spawn::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Spoils of Pandaria Lever Eff 145125
class spell_spoils_lever_eff : public SpellScript
{
    PrepareSpellScript(spell_spoils_lever_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            if (Creature* leverController = ObjectAccessor::GetCreature(*target, target->AI()->GetGUID()))
                leverController->AI()->SetData(TYPE_LEVER_POWER, GetSpellValue()->EffectBasePoints[effIndex]);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_lever_eff::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Encapsulated Pheromones Periodic 142524
class spell_spoils_encapsulated_pheromones_periodic : public AuraScript
{
    PrepareAuraScript(spell_spoils_encapsulated_pheromones_periodic);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* caster = GetOwner()->ToCreature())
            if (Unit* target = caster->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 70.0f, true))
                caster->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_ENCAPSULATED_PHEROMONES_AT, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_encapsulated_pheromones_periodic::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Gusting Bomb Periodic 145715
class spell_spoils_gusting_bomb_periodic : public AuraScript
{
    PrepareAuraScript(spell_spoils_gusting_bomb_periodic);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->CastSpell(owner, SPELL_GUSTING_BOMB_EFF, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_gusting_bomb_periodic::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Rage of Empress 145812
class spell_spoils_rage_of_empress : public SpellScript
{
    PrepareSpellScript(spell_spoils_rage_of_empress);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return IsMogu(target->GetEntry()); });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->IsInCombat(); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_rage_of_empress::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spoils_rage_of_empress::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Residue 145786
class spell_spoils_residue : public SpellScript
{
    PrepareSpellScript(spell_spoils_residue);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return IsMogu(target->GetEntry()); });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->IsInCombat(); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_residue::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spoils_residue::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Mantid Swarm 142539
class spell_spoils_mantid_swarm : public SpellScript
{
    PrepareSpellScript(spell_spoils_mantid_swarm);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->CastSpell(caster, SPELL_MANTID_SWARM_SUMM_RIGHT, true);

            uint32 swarmEnrage = caster->AI()->GetData(TYPE_MANTID_SWARM_COUNT);

            if (swarmEnrage > 1)
                caster->CastSpell(caster, SPELL_MANTID_SWARM_SUMM_LEFT, true);

            // soft enrage
            if (swarmEnrage > 2)
            {
                for (uint8 i = 0; i < swarmEnrage - 2; i++)
                {
                    GetPositionWithDistInOrientation(caster, frand(2.5f, 10.0f), frand(0.0f, 2 * M_PI), x, y);
                    caster->CastSpell(x, y, caster->GetPositionZ() + 10.0f, SPELL_MANTID_SWARM_SUMM, true);
                }
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_mantid_swarm::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Pheromone Cloud 148762
class spell_spoils_pheromone_cloud : public AuraScript
{
    PrepareAuraScript(spell_spoils_pheromone_cloud);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // selector spell doesn`t exist
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, owner, 80.0f);

            for (auto&& itr : targets)
                owner->CastSpell(itr, SPELL_PHEROMONE_CLOUD_EFF, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_pheromone_cloud::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Set to Blow Button 145987
class spell_spoils_set_to_blow_button : public AuraScript
{
    PrepareAuraScript(spell_spoils_set_to_blow_button)

    void HandleApply(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            SetCharges(5);
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->RemoveAurasDueToSpell(SPELL_SET_TO_BLOW_VISUAL_1);
            owner->RemoveAurasDueToSpell(SPELL_SET_TO_BLOW_VISUAL_2);
            owner->RemoveAurasDueToSpell(SPELL_SET_TO_BLOW_VISUAL_3);
            owner->RemoveAurasDueToSpell(SPELL_SET_TO_BLOW_VISUAL_4);

            if (pAuraEffect->GetBase()->GetDuration() > 0)
                return;

            if (Unit* caster = GetCaster())
            {
                int32 bp = (int32)(sSpellMgr->GetSpellInfo(SPELL_SET_TO_BLOW_EXPLOSION_EFF, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints * GetCharges());
                caster->CastCustomSpell(owner, SPELL_SET_TO_BLOW_EXPLOSION_EFF, &bp, 0, 0, true);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_spoils_set_to_blow_button::HandleApply, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_spoils_set_to_blow_button::HandleRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
    }
};

// Set to Blow Eff 147404
class spell_spoils_set_to_blow_eff : public SpellScript
{
    PrepareSpellScript(spell_spoils_set_to_blow_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_SET_TO_BLOW_BUTTON, true);

                // Visual Bomb Pack
                target->CastSpell(target, SPELL_SET_TO_BLOW_VISUAL_1, true);
                target->CastSpell(target, SPELL_SET_TO_BLOW_VISUAL_2, true);
                target->CastSpell(target, SPELL_SET_TO_BLOW_VISUAL_3, true);
                target->CastSpell(target, SPELL_SET_TO_BLOW_VISUAL_4, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_set_to_blow_eff::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Throw Bomb 146364
class spell_spoils_throw_bomb : public SpellScript
{
    PrepareSpellScript(spell_spoils_throw_bomb);

    void HandleLaunch(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Aura* bombs = caster->GetAura(SPELL_SET_TO_BLOW_BUTTON))
            {
                if (bombs->GetCharges() > 1)
                    bombs->SetCharges(bombs->GetCharges() - 1);
                else
                    caster->RemoveAurasDueToSpell(SPELL_SET_TO_BLOW_BUTTON);
            }
        }
    }

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetHitDest()->GetPositionX(), GetHitDest()->GetPositionY(), GetHitDest()->GetPositionZ(), SPELL_SET_TO_BLOW_AT, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(spell_spoils_throw_bomb::HandleLaunch, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
        OnEffectHit += SpellEffectFn(spell_spoils_throw_bomb::HandleHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// Set to Blow 145996
class spell_spoils_set_to_blow : public SpellScript
{
    PrepareSpellScript(spell_spoils_set_to_blow);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            // selector spell doesn`t exist
            std::list<Player*> targets, m_targets;
            GetPlayerListInGrid(targets, caster, 80.0f);
            m_targets = targets;

            targets.remove_if(TankSpecTargetSelector());
            targets.remove_if(MeeleSpecTargetSelector());
            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_SET_TO_BLOW_BUTTON); });

            if (targets.empty())
            {
                targets.clear();

                for (auto&& itr : m_targets)
                    targets.push_back(itr);

                targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_SET_TO_BLOW_BUTTON); });
            }

            if (!targets.empty())
                if (Player* target = Trinity::Containers::SelectRandomContainerElement(targets))
                    caster->CastSpell(target, SPELL_SET_TO_BLOW_EFF, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_set_to_blow::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Armageddon Defense System 145864
class spell_spoils_armageddon_defense_system : public SpellScript
{
    PrepareSpellScript(spell_spoils_armageddon_defense_system);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                target->CastSpell(caster, SPELL_UNSTABLE_DEFENSE_SYSTEM_TELEPORT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_armageddon_defense_system::HandleHit, EFFECT_0, SPELL_EFFECT_INSTAKILL);
    }
};

// Nova 149277
class spell_spoils_nova : public AuraScript
{
    PrepareAuraScript(spell_spoils_nova)

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_NOVA_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_spoils_nova::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Spark of Life Missle 142695
class spell_spoils_spark_of_life_missle : public SpellScript
{
    PrepareSpellScript(spell_spoils_spark_of_life_missle);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            caster->SummonCreature(NPC_SPARK_OF_LIFE, *GetHitDest(), TEMPSUMMON_MANUAL_DESPAWN);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_spoils_spark_of_life_missle::HandleHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// Crimson Reconstruction Selector 142947
class spell_spoils_crimson_reconstruction_selector : public SpellScript
{
    PrepareSpellScript(spell_spoils_crimson_reconstruction_selector);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return !IsMogu(target->GetEntry()) || target->GetEntry() == NPC_SHADOW_RITUALIST_PHYLACTERY || target->GetEntry() == NPC_BURIAL_URN; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->IsInCombat(); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_crimson_reconstruction_selector::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spoils_crimson_reconstruction_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Matter Scramble 145288
class spell_matter_scramble : public AuraScript
{
    PrepareAuraScript(spell_matter_scramble)

    void HandleApply(AuraEffect const* /*pAuraEffect*/, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_MATTER_SWAP);
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (pAuraEffect->GetBase()->GetDuration() > 0) // prevent instant hit if owner has die
                return;

            owner->AI()->DoAction(ACTION_MATTER_SWAP_EFF);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_matter_scramble::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_matter_scramble::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Matter Scramble Check 145369
class spell_spoils_matter_scramble_check : public SpellScript
{
    PrepareSpellScript(spell_spoils_matter_scramble_check);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = GetHitUnit())
                caster->AI()->SetGUID(target->GetGUID());
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });

        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (targets.empty())
            {
                caster->CastSpell(caster, SPELL_MATTER_SCRAMBLE_EFF, true);
                return;
            }

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_matter_scramble_check::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spoils_matter_scramble_check::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Torment 142983
class spell_spoils_torment : public AuraScript
{
    PrepareAuraScript(spell_spoils_torment);

    void OnTrigger(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            uint8 tickCount = aurEff->GetTickNumber();
            float mod = sSpellMgr->GetSpellInfo(SPELL_TORMENT, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints / 2.0f;
            int32 bp = (int32)(sSpellMgr->GetSpellInfo(SPELL_TORMENT, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints + tickCount * mod);// additional 50% per each tick

            if (Unit* caster = GetCaster())
                caster->CastCustomSpell(owner, SPELL_TORMENT_EFF, &bp, 0, 0, true);
        }
    }

    void HandleRemove(AuraEffect const* pAuraEffect, AuraEffectHandleModes eMode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_TORMENT_LINK);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_torment::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_spoils_torment::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Torment Link 142942
class spell_spoils_torment_link : public SpellScript
{
    PrepareSpellScript(spell_spoils_torment_link);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
            if (GetCaster() && GetCaster()->IsAlive())
                GetCaster()->CastSpell(target, SPELL_TORMENT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_torment_link::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Mogu Warlord Jade Tempest 148515, 148518, 148582, 148513
class spell_spoils_mogu_warlord_jade_tempest : public SpellScript
{
    PrepareSpellScript(spell_spoils_mogu_warlord_jade_tempest);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            int32 bp = sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints;

            std::list<Player*> targets;
            GetPlayerListInGrid(targets, caster, 80.0f);

            for (auto&& itr : targets)
                caster->CastCustomSpell(itr, invMoguWarlordsSpellType.find(GetSpellInfo()->Id)->second, &bp, NULL, NULL, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_mogu_warlord_jade_tempest::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Unstable Defense System Alt Power 148505
class spell_spoils_unstable_defense_system_alt_power : public AuraScript
{
    PrepareAuraScript(spell_spoils_unstable_defense_system_alt_power);

    ObjectGuid linkedSpoilsGUID;

    bool Load() override
    {
        linkedSpoilsGUID = ObjectGuid::Empty;
        return true;
    }

    void SetGuid(uint32 /*type*/, ObjectGuid guid) override
    {
        linkedSpoilsGUID = guid;
    }

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (Creature* linkSpoils = ObjectAccessor::GetCreature(*owner, linkedSpoilsGUID))
            {
                int32 bp = (int32)linkSpoils->AI()->GetData(TYPE_LEVER_POWER);

                if (bp > owner->GetPower(POWER_ALTERNATE_POWER))
                    owner->SetPower(POWER_ALTERNATE_POWER, bp);
            }
        }
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_spoils_unstable_defense_system_alt_power::OnUpdate, EFFECT_0, SPELL_AURA_ENABLE_ALT_POWER);
    }
};

// Eminence 146189
class spell_spoils_eminence : public AuraScript
{
    PrepareAuraScript(spell_spoils_eminence);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            int32 healEff = (sSpellMgr->GetSpellInfo(SPELL_EMINENCE_EFF, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints / 100.0f) * int32(eventInfo.GetDamageInfo()->GetDamage());
            owner->CastCustomSpell(owner, SPELL_EMINENCE_EFF, &healEff, 0, 0, true);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_spoils_eminence::HandleOnProc);
    }
};

// Gusting Crane Kick 146180
class spell_spoils_gusting_crane_kick : public AuraScript
{
    PrepareAuraScript(spell_spoils_gusting_crane_kick);

    void OnTrigger(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_GUSTING_CRANE_KICK_EFF, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_gusting_crane_kick::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Path of Blossoms 146253
class spell_spoils_path_of_blossoms : public SpellScript
{
    PrepareSpellScript(spell_spoils_path_of_blossoms);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                uint8 count = (uint8)(caster->GetExactDist2d(target) / 2);

                for (uint8 i = 1; i < count; i++)
                {
                    GetPositionWithDistInOrientation(caster, 2.0f * i, caster->GetAngle(target), x, y);
                    caster->CastSpell(x, y, caster->GetPositionZ(), SPELL_PATH_OF_BLOSSOMS_AT, true);
                }

                caster->GetMotionMaster()->MoveCharge(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 30.0f, EVENT_CHARGE);
            }
        }
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_path_of_blossoms::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Mantid Swarm Summon 145807
class spell_spoils_mantid_swarm_summon_right : public SpellScript
{
    PrepareSpellScript(spell_spoils_mantid_swarm_summon_right);

    void SelectTargets(SpellDestination& dest)
    {
        Position newPos = { dest._position.GetPositionX(), dest._position.GetPositionY(), dest._position.GetPositionZ() + 10.0f, 0 };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_spoils_mantid_swarm_summon_right::SelectTargets, EFFECT_0, TARGET_DEST_CASTER_RIGHT);
    }
};

// Mantid Swarm Summon 145808
class spell_spoils_mantid_swarm_summon_left : public SpellScript
{
    PrepareSpellScript(spell_spoils_mantid_swarm_summon_left);

    void SelectTargets(SpellDestination& dest)
    {
        Position newPos = { dest._position.GetPositionX(), dest._position.GetPositionY(), dest._position.GetPositionZ() + 10.0f, 0 };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_spoils_mantid_swarm_summon_left::SelectTargets, EFFECT_0, TARGET_DEST_CASTER_LEFT);
    }
};

// Return to Stone Eff 145514
class spell_spoils_return_to_stone_eff : public SpellScript
{
    PrepareSpellScript(spell_spoils_return_to_stone_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                if (Creature* stoneStature = caster->SummonCreature(NPC_STONE_STATUE, *caster, TEMPSUMMON_MANUAL_DESPAWN))
                    stoneStature->AI()->SetGUID(target->GetGUID());
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_return_to_stone_eff::HandleHit, EFFECT_1, SPELL_EFFECT_KNOCK_BACK);
    }
};

// Harden Flesh 144922
class spell_spoils_harden_flesh : public SpellScript
{
    PrepareSpellScript(spell_spoils_harden_flesh);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, caster, 70.0f);

            for (auto&& itr : targets)
                caster->CastSpell(itr, SPELL_HARDEN_FLESH_EFF, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_spoils_harden_flesh::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Blade of the Hundred Steps 146068
class spell_spoils_blade_of_hundred_steps : public AuraScript
{
    PrepareAuraScript(spell_spoils_blade_of_hundred_steps);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (Unit* attacker = eventInfo.GetActor())
                owner->CastSpell(attacker, SPELL_BLADE_OF_THE_HUNDRED_STEPS_EFF, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_spoils_blade_of_hundred_steps::HandleOnProc);
    }
};

// Strength of the Stone 145998
class spell_spoils_strength_of_the_stone : public AuraScript
{
    PrepareAuraScript(spell_spoils_strength_of_the_stone);

    void OnTrigger(AuraEffect const* aurEff)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            int32 statueValue = (int32)(owner->AI()->GetData(TYPE_RETURN_TO_STONE_VALUE) * sSpellMgr->GetSpellInfo(SPELL_STRENGTH_OF_THE_STONE, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints);
            aurEff->GetBase()->GetEffect(EFFECT_0)->SetAmount(statueValue);

            // Should take this visual
            if (owner->AI()->GetData(TYPE_RETURN_TO_STONE_VALUE) > 0)
                aurEff->GetBase()->SetCharges(owner->AI()->GetData(TYPE_RETURN_TO_STONE_VALUE));
            else
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_strength_of_the_stone::OnTrigger, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Unstable Defense System Periodic 145685
class spell_spoils_unstable_defense_system_periodic : public AuraScript
{
    PrepareAuraScript(spell_spoils_unstable_defense_system_periodic);

    bool Load() override
    {
        tickCounter = 0;
        return true;
    }

    void OnTrigger(AuraEffect const* aurEff)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            // Prevent damage 5s after start (but aura should`ve at encounter starting)
            if (Creature* spoils = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_SPOILS_OF_PANDARIA) : ObjectGuid::Empty))
                if (!spoils->AI()->GetData(TYPE_DEFENSE_SYSTEM_ACTIVE))
                    return;

            int32 baseValue = tickCounter ? (int32)(sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints + ((tickCounter * 5.0f) / 100.0f) * sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints) : sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2, owner->GetMap()->GetDifficulty())->Effects[0].BasePoints;

            // Increment 5% per tick till remove
            tickCounter++;

            if (Unit* caster = GetCaster())
                caster->CastCustomSpell(owner, SPELL_UNSTABLE_DEFENSE_SYSTEM_AT_EFF_2, &baseValue, 0, 0, true);
        }
    }

    private:
        uint32 tickCounter;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_spoils_unstable_defense_system_periodic::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Unstable Spark Missle 146824
class spell_spoils_unstable_spark_missle : public SpellScript
{
    PrepareSpellScript(spell_spoils_unstable_spark_missle);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_SPOILS_OF_PANDARIA) == IN_PROGRESS)
            GetCaster()->CastSpell(GetHitDest()->GetPositionX(), GetHitDest()->GetPositionY(), GetHitDest()->GetPositionZ(), SPELL_UNSTABLE_SPARK_SUMM, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_spoils_unstable_spark_missle::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Supernova 146815
class spell_spoils_supernova : public SpellScript
{
    PrepareSpellScript(spell_spoils_supernova);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_spoils_supernova::HandleAfterCast);
    }
};

// 1273, 1272, 1274, 1271. Summoned by 136882, 142560, 145121, 145997 - Spoils of Pandaria Region
class sat_spoils_of_pandaria_region : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_LEVER_DATA) == IN_PROGRESS;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (auto const aura = itr->AddAura(SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER_PLAYER, itr))
                aura->SetScriptGuid(0, GetCaster()->GetGUID());

            // Set Current Power
            itr->CastSpell(itr, SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER_PLAYER, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_UNSTABLE_DEFENSE_SYSTEM_ALT_POWER_PLAYER);
    }
};

// 1314. Summoned by 145285 - Encapsulated Pheromones
class sat_spoils_encapsulated_pheromones : public IAreaTriggerAura
{
    void OnInit() override
    {
        if (AreaTrigger* at = GetAreaTrigger())
            at->SetDuration(58 * IN_MILLISECONDS);
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_ENCAPSULATED_PHEROMONES_AURA_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_ENCAPSULATED_PHEROMONES_AURA_EFF);
    }
};

// 1204. Summoned by 145286 - Wind Storm
class sat_spoils_wind_storm : public IAreaTriggerAura
{
    void OnInit() override
    {
        if (Unit* caster = GetCaster())
        {
            if (AreaTrigger* at = GetAreaTrigger())
            {
                float dist = 0.5f;
                float x, y;

                std::vector<Position> path;

                for (uint8 i = 1; i < 65; ++i)
                {
                    x = caster->GetPositionX() + dist * cos(Position::NormalizeOrientation(at->GetAngle(caster->GetPositionX(), caster->GetPositionY()) + (i * M_PI / 16) - M_PI));
                    y = caster->GetPositionY() + dist * sin(Position::NormalizeOrientation(at->GetAngle(caster->GetPositionX(), caster->GetPositionY()) + (i * M_PI / 16) - M_PI));

                    dist += 0.32f;
                    path.push_back({ x, y, at->GetPositionZ() });
                }

                at->InitSpline(path, at->GetDuration());
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
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_WINDSTORM_EFF, true);
    }
};

// 1228. Summoned by 146365 - Set to Blow
class sat_spoils_set_to_blow : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 27500; // delay to explosion
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            if (AreaTrigger* at = GetAreaTrigger())
                itr->CastSpell(at->GetPositionX(), at->GetPositionY(), at->GetPositionZ(), SPELL_SET_TO_BLOW_EXPLOSION_EFF, true);
    }

    void OnDestroy() override
    {
        if (Unit* caster = GetCaster())
        {
            if (AreaTrigger* at = GetAreaTrigger())
            {
                if (at->GetDuration() > 100) // prevent double explosion at reach
                    return;

                caster->CastSpell(at->GetPositionX(), at->GetPositionY(), at->GetPositionZ(), SPELL_SET_TO_BLOW_EXPLOSION_EFF, true);
            }
        }
    }
};

// 1163. Summoned by 145270 - Crimson Reconstruction
class sat_spoils_crimson_reconstruction : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && ((triggering->ToCreature() && IsMogu(triggering->ToCreature()->GetEntry()) && triggering->ToCreature()->GetEntry() != NPC_SHADOW_RITUALIST_PHYLACTERY && triggering->ToCreature()->GetEntry() != NPC_BURIAL_URN && GetAreaTrigger()) || (GetCaster() && GetCaster()->GetMap()->IsHeroic() && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive())) && GetAreaTrigger()->GetDuration() < 42500; // delay
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_CRIMSON_RECONSTRUCTION_EFF, true);
        }
        else if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_CRIMSON_ACID, true);
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_CRIMSON_RECONSTRUCTION_EFF);
        else if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_CRIMSON_ACID);
    }
};

// 1158. Summoned by 145460 - Mogu Rune of Power
class sat_spoils_mogu_rune_of_power : public IAreaTriggerAura
{
    std::vector<ObjectGuid> affectedTargetGUIDs;

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && ((triggering->ToCreature() && IsMogu(triggering->ToCreature()->GetEntry())) || triggering->ToPlayer() && triggering->ToPlayer()->IsAlive());
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            target->CastSpell(target, SPELL_MOGU_RUNE_OF_POWER_EFF, true);
            affectedTargetGUIDs.push_back(target->GetGUID());
        }
        else if (Creature* target = triggering->ToCreature())
            target->CastSpell(target, SPELL_MOGU_RUNE_OF_POWER_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            target->RemoveAurasDueToSpell(SPELL_MOGU_RUNE_OF_POWER_EFF);

            if (std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), target->GetGUID()) != affectedTargetGUIDs.end())
                affectedTargetGUIDs.erase(std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), target->GetGUID()));
        }
        else if (Creature* target = triggering->ToCreature())
            target->RemoveAurasDueToSpell(SPELL_MOGU_RUNE_OF_POWER_EFF);
    }
};

// 1224. Summoned by 146222 - Breath of Fire
class sat_spoils_breath_of_fire : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_BREATH_OF_FIRE_DISTRACT) && GetCaster() && GetCaster()->HasInArc(M_PI / 6, triggering->ToPlayer());
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(itr, SPELL_BREATH_OF_FIRE_EFF, true);

                if (itr->HasAura(SPELL_KEG_TOSS_EFF))
                    caster->CastSpell(itr, SPELL_BREATH_OF_FIRE_DISTRACT, true);
            }
        }
    }
};

// 1222. Summoned by 146180 - Gusting Crane Kick
class sat_spoils_gusting_crane_kick : public IAreaTriggerAura
{
    std::vector<ObjectGuid> affectedTargetGUIDs;
    Position forceMovementPos;

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                forceMovementPos = caster->GetPosition();
                target->SendApplyMovementForce(true, forceMovementPos, 9.5f);
                affectedTargetGUIDs.push_back(target->GetGUID());
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            target->SendApplyMovementForce(false, forceMovementPos);

            if (std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), target->GetGUID()) != affectedTargetGUIDs.end())
                affectedTargetGUIDs.erase(std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), target->GetGUID()));
        }
    }
};

// 1225. Summoned by 146255 - Path of Blossoms
class sat_spoils_path_of_blossoms : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 28000;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_PATH_OF_BLOSSOMS_EFF, true);
    }
};

// 1239. Summoned by 146098 - Staff of Resonating Water
class sat_spoils_staff_of_resonating_water : public IAreaTriggerAura
{
    void OnInit() override
    {
        if (Unit* caster = GetCaster())
        {
            // max restore health per all targets
            healthBaseValue = sSpellMgr->GetSpellInfo(SPELL_STAFF_OF_RESONATING_WATER, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints;

            if (AreaTrigger* at = GetAreaTrigger())
            {
                float dist = 35.0f;
                float angle = caster->GetOrientation();

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

                float x = at->GetPositionX() + (dist * std::cos(angle));
                float y = at->GetPositionY() + (dist * std::sin(angle));
                float z = at->GetPositionZ();
                at->UpdateAllowedPositionZ(x, y, z);
                path.push_back({ x, y, z });

                at->InitSpline(path, at->GetDuration());
            }              
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToPlayer() || triggering->ToCreature() && GetCaster() && GetCaster()->IsValidAttackTarget(triggering->ToCreature()));
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* target = triggering->ToCreature())
                caster->CastSpell(target, SPELL_STAFF_OF_RESONATING_WATER_EFF, true);
            else if (Player* target = triggering->ToPlayer())
            {
                if (healthBaseValue > 0)
                {
                    int32 reqHeal = target->GetMaxHealth() - target->GetHealth();

                    if (!reqHeal)
                        return;

                    if (reqHeal > healthBaseValue)
                    {
                        reqHeal = healthBaseValue;
                        healthBaseValue = 0;
                    }
                    else
                        healthBaseValue -= reqHeal;

                    caster->CastCustomSpell(target, SPELL_STAFF_OF_RESONATING_WATER_HEAL_EFF, &reqHeal, 0, 0, true);
                }
            }
        }
    }

    private:
        int32 healthBaseValue;
};

// 1295. Summoned by 145687 - Unstable Defense System Box
class sat_spoils_unstable_defense_system_box : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->JumpTo(20.0f, 5.0f, false);
    }
};

// 1213. Summoned by 145838 - Unstable Defense System Box Area
class sat_spoils_unstable_defense_system_box_area : public IAreaTriggerAura
{
    std::vector<ObjectGuid> affectedTargetGUIDs;

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && triggering->ToPlayer()->GetPositionZ() > -276.5f;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(target, SPELL_UNSTABLE_DEFENSE_SYSTEME_AT_EFF, true);
                affectedTargetGUIDs.push_back(target->GetGUID());
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            target->RemoveAurasDueToSpell(SPELL_UNSTABLE_DEFENSE_SYSTEME_AT_EFF);

            if (std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), target->GetGUID()) != affectedTargetGUIDs.end())
                affectedTargetGUIDs.erase(std::find(affectedTargetGUIDs.begin(), affectedTargetGUIDs.end(), target->GetGUID()));
        }
    }
};

void AddSC_spoils_of_pandaria()
{
    new boss_spoils_of_pandaria();
    new creature_script<npc_spoils_artifact_spoils>("npc_spoils_artifact_spoils");
    new creature_script<npc_spoils_lever>("npc_spoils_lever");
    new creature_script<npc_modified_anima_golem>("npc_modified_anima_golem");
    new creature_script<npc_mogu_shadow_ritualist>("npc_mogu_shadow_ritualist");
    new creature_script<npc_zarthik_amber_priest>("npc_zarthik_amber_priest");
    new creature_script<npc_setthik_wielder>("npc_setthik_wielder");
    new creature_script<npc_animated_stone_mogu>("npc_animated_stone_mogu");
    new creature_script<npc_burial_urn>("npc_burial_urn");
    new creature_script<npc_shadow_ritualist_phylactery>("npc_shadow_ritualist_phylactery");
    new creature_script<npc_quilen_guardians_controller>("npc_quilen_guardians_controller");
    new creature_script<npc_spoils_quilen_guardian>("npc_spoils_quilen_guardian");
    new creature_script<npc_srithik_bombardier>("npc_srithik_bombardier");
    new creature_script<npc_korthik_warcaller>("npc_korthik_warcaller");
    new creature_script<npc_spoils_mogu_warlord>("npc_spoils_mogu_warlord");
    new creature_script<npc_spoils_mantid_commander>("npc_spoils_mantid_commander");
    new creature_script<npc_spoils_amber_encased_kunchong>("npc_spoils_amber_encased_kunchong");
    new creature_script<npc_spoils_lift_hook>("npc_spoils_lift_hook");
    new creature_script<npc_spoils_ancient_brewmaster_spirit>("npc_spoils_ancient_brewmaster_spirit");
    new creature_script<npc_spoils_wise_mistweaver_spirit>("npc_spoils_wise_mistweaver_spirit");
    new creature_script<npc_spoils_nameless_windwalker_spirit>("npc_spoils_nameless_windwalker_spirit");
    new creature_script<npc_spoils_spark_of_life>("npc_spoils_spark_of_life");
    new creature_script<npc_spoils_pandarens_gift>("npc_spoils_pandarens_gift");
    new creature_script<npc_spoils_zarthik_swarmer>("npc_spoils_zarthik_swarmer");
    new creature_script<npc_spoils_stone_statue_warlord>("npc_spoils_stone_statue_warlord");
    new creature_script<npc_spoils_unstable_spark>("npc_spoils_unstable_spark");
    new go_spoils_secured_stockpile_of_spoils();
    new go_spoils_lever();

    new spell_script<spell_spoils_unstable_defense_system_initialize>("spell_spoils_unstable_defense_system_initialize");
    new spell_script<spell_spoils_breaking>("spell_spoils_breaking");
    new aura_script<spell_spoils_stone_statue>("spell_spoils_stone_statue");
    new aura_script<spell_spoils_pandaren_spirit_spawn>("spell_spoils_pandaren_spirit_spawn");
    new spell_script<spell_spoils_lever_eff>("spell_spoils_lever_eff");
    new aura_script<spell_spoils_encapsulated_pheromones_periodic>("spell_spoils_encapsulated_pheromones_periodic");
    new aura_script<spell_spoils_gusting_bomb_periodic>("spell_spoils_gusting_bomb_periodic");
    new spell_script<spell_spoils_rage_of_empress>("spell_spoils_rage_of_empress");
    new spell_script<spell_spoils_residue>("spell_spoils_residue");
    new spell_script<spell_spoils_mantid_swarm>("spell_spoils_mantid_swarm");
    new aura_script<spell_spoils_pheromone_cloud>("spell_spoils_pheromone_cloud");
    new aura_script<spell_spoils_set_to_blow_button>("spell_spoils_set_to_blow_button");
    new spell_script<spell_spoils_set_to_blow_eff>("spell_spoils_set_to_blow_eff");
    new spell_script<spell_spoils_throw_bomb>("spell_spoils_throw_bomb");
    new spell_script<spell_spoils_set_to_blow>("spell_spoils_set_to_blow");
    new spell_script<spell_spoils_armageddon_defense_system>("spell_spoils_armageddon_defense_system");
    new aura_script<spell_spoils_nova>("spell_spoils_nova");
    new spell_script<spell_spoils_spark_of_life_missle>("spell_spoils_spark_of_life_missle");
    new spell_script<spell_spoils_crimson_reconstruction_selector>("spell_spoils_crimson_reconstruction_selector");
    new aura_script<spell_matter_scramble>("spell_matter_scramble");
    new spell_script<spell_spoils_matter_scramble_check>("spell_spoils_matter_scramble_check");
    new aura_script<spell_spoils_torment>("spell_spoils_torment");
    new spell_script<spell_spoils_mogu_warlord_jade_tempest>("spell_spoils_mogu_warlord_jade_tempest");
    new aura_script<spell_spoils_unstable_defense_system_alt_power>("spell_spoils_unstable_defense_system_alt_power");
    new aura_script<spell_spoils_eminence>("spell_spoils_eminence");
    new aura_script<spell_spoils_gusting_crane_kick>("spell_spoils_gusting_crane_kick");
    new spell_script<spell_spoils_path_of_blossoms>("spell_spoils_path_of_blossoms");
    new spell_script<spell_spoils_mantid_swarm_summon_right>("spell_spoils_mantid_swarm_summon_right");
    new spell_script<spell_spoils_mantid_swarm_summon_left>("spell_spoils_mantid_swarm_summon_left");
    new spell_script<spell_spoils_return_to_stone_eff>("spell_spoils_return_to_stone_eff");
    new spell_script<spell_spoils_harden_flesh>("spell_spoils_harden_flesh");
    new aura_script<spell_spoils_blade_of_hundred_steps>("spell_spoils_blade_of_hundred_steps");
    new aura_script<spell_spoils_strength_of_the_stone>("spell_spoils_strength_of_the_stone");
    new aura_script<spell_spoils_unstable_defense_system_periodic>("spell_spoils_unstable_defense_system_periodic");
    new spell_script<spell_spoils_unstable_spark_missle>("spell_spoils_unstable_spark_missle");
    new spell_script<spell_spoils_supernova>("spell_spoils_supernova");
    new spell_script<spell_spoils_torment_link>("spell_spoils_torment_link");
    new atrigger_script<sat_spoils_of_pandaria_region>("sat_spoils_of_pandaria_region");
    new atrigger_script<sat_spoils_encapsulated_pheromones>("sat_spoils_encapsulated_pheromones");
    new atrigger_script<sat_spoils_wind_storm>("sat_spoils_wind_storm");
    new atrigger_script<sat_spoils_set_to_blow>("sat_spoils_set_to_blow");
    new atrigger_script<sat_spoils_crimson_reconstruction>("sat_spoils_crimson_reconstruction");
    new atrigger_script<sat_spoils_mogu_rune_of_power>("sat_spoils_mogu_rune_of_power");
    new atrigger_script<sat_spoils_breath_of_fire>("sat_spoils_breath_of_fire");
    new atrigger_script<sat_spoils_gusting_crane_kick>("sat_spoils_gusting_crane_kick");
    new atrigger_script<sat_spoils_path_of_blossoms>("sat_spoils_path_of_blossoms");
    new atrigger_script<sat_spoils_staff_of_resonating_water>("sat_spoils_staff_of_resonating_water");
    new atrigger_script<sat_spoils_unstable_defense_system_box>("sat_spoils_unstable_defense_system_box");
    new atrigger_script<sat_spoils_unstable_defense_system_box_area>("sat_spoils_unstable_defense_system_box_area");
};
