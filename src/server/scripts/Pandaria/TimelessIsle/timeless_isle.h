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

#include "Group.h"
#include "Guild.h"

#define CELESTIAL_COURT_BOSS_INTRO_TIMER_1 1000
#define CELESTIAL_COURT_BOSS_INTRO_TIMER_2 15000

enum Datas
{
    DATA_CHI_JI                           = 0,
    DATA_NIUZAO                           = 1,
    DATA_YU_LON                           = 2,
    DATA_XUEN                             = 3,
    DATA_ORDOS                            = 4,
};

enum Bosses
{
    BOSS_CHI_JI                           = 71952,
    BOSS_NIUZAO                           = 71954,
    BOSS_YU_LON                           = 71955,
    BOSS_XUEN                             = 71953,
    BOSS_ORDOS                            = 72057,
};

enum Rares
{
    RARE_ARCHIEREUS_OF_FLAME              = 73174,
    RARE_ARCHIEREUS_OF_FLAME_CLOAK        = 73666, // the above and this line are the SAME npcs, but with different ids. One is spawned at the very ground, close to the center of the Timeless Isle, the other one is spawned at the top place of the Timeless Isle, that you need the cloak to get there.
    RARE_BUFO                             = 72775,
    RARE_CHAMPION_OF_THE_BLACK_FLAME      = 73171,
    RARE_CHELON                           = 72045,
    RARE_CINDERFALL                       = 73175,
    RARE_CRANEGNASHER                     = 72049,
    RARE_DREAD_SHIP_VAZUVIUS              = 73281,
    RARE_EMERALD_GANDER                   = 73158,
    RARE_EVERMAW                          = 73279,
    RARE_FLINTLORD_GAIRAN                 = 73172,
    RARE_GOLGANARR                        = 72970,
    RARE_GREAT_TURTLE_FURYSHELL           = 73161,
    RARE_GUCHI_THE_SWARMBRINGER           = 72909,
    RARE_HUOLON                           = 73167,
    RARE_IRONFUR_STEELHORN                = 73160,
    RARE_IMPERIAL_PYTHON                  = 73163,
    RARE_JAKUR_OF_ORDON                   = 73169,
    RARE_KARKANOS                         = 72193,
    RARE_LEAFMENDER                       = 73277,
    RARE_MONSTROUS_SPINECLAW              = 73166,
    RARE_RATTLESKEW                       = 72048,
    RARE_ROCK_MOSS                        = 73157,
    RARE_SPELURK                          = 71864,
    RARE_SPIRIT_OF_JADEFIRE               = 72769,
    RARE_TSAVOKA                          = 72808,
    RARE_URDUR_THE_CAUTERIZER             = 73173,
    RARE_WATCHER_OSU                      = 73170,
    RARE_ZESQUA                           = 72245,
    RARE_ZHU_GON_THE_SOUR                 = 71919,
    RARE_STINKBRAID                       = 73704,
};

enum TimelessCreatures
{
    NPC_SKUNKY_ALEMENTAL                  = 71908,
    NPC_EMPEROR_SHAOHAO_TI                = 73303,
    NPC_ETERNAL_KILNMASTER                = 72896,
    NPC_HIGH_PRIEST_OF_ORDOS              = 72898,
    NPC_BLAZEBOUND_CHANTER                = 72897,
};

enum Factions
{
    FACTION_FRIENDLY                      = 35,
    FACTION_HOSTILE_NEUTRAL               = 31,
};

// Scripts information

enum npcSpells
{
    // Turtles
    SPELL_GEYSER_1                        = 147572,
    SPELL_GEYSER_2                        = 147573,
    SPELL_SHELL_SPINE_1                   = 147571,
    SPELL_SHELL_SPINE_2                   = 147590,
    SPELL_SNAPPING_BITE_1                 = 147569,
    SPELL_SNAPPING_BITE_2                 = 147589,
    SPELL_SNAPPING_BITE_3                 = 147568,

    // Faction Sentries
    SPELL_INCAPACITATING_SHOUT            = 18328,
    SPELL_MORTAL_STRIKE                   = 19643,
    SPELL_NET                             = 81210,
    SPELL_SNAP_KICK                       = 15618,

    // Adders
    SPELL_HUGE_FANG_1                     = 147290,
    SPELL_HUGE_FANG_2                     = 147289,

    // Yaks
    SPELL_HEADBUTT_1                      = 147384,
    SPELL_HEADBUTT_2                      = 147354,
    SPELL_HEADBUTT_3                      = 147382,
    SPELL_IRON_FUR                        = 147368,
    SPELL_OX_CHARGE                       = 147385,
    SPELL_OX_CHARGE_TRIGGER               = 147386,

    // Cranes
    SPELL_GUST_OF_WIND                    = 147310,
    SPELL_HEALING_SONG                    = 147309,
    SPELL_WINDFEATHER                     = 147306,

    // Silkworms
    SPELL_VENOM_SPIT                      = 147535,
    SPELL_POISON_SPIT                     = 147496,

    // Crabs
    SPELL_CLAW_FLURRY_1                   = 147557,
    SPELL_CLAW_FLURRY_2                   = 147560,

    // Spectrals
    SPELL_GHOST_VISUAL                    = 145945,
    SPELL_DRUNKEN_STUPOR                  = 146948,
    SPELL_CHI_BURST                       = 148729,
    SPELL_HEALING_MISTS                   = 148728,
    SPELL_SPINNING_CRANE_KICK             = 148730,

    // Cats
    SPELL_DEFENSIVE_LEAP                  = 147650,
    SPELL_POUNCE_CHARGE                   = 147649,
    SPELL_POUNCE_STUN                     = 147651,
    SPELL_RENDING_SWIPE                   = 147646,
    SPELL_GREATER_SWIPE                   = 147652,

    // Alementals
    SPELL_BREW_BOLT                       = 147345,
    SPELL_RAIN_OF_SKUNKY_ALE              = 147347,
    SPELL_SKUNKY_ALE_CRASH                = 144882,

    // Spirits
    SPELL_SPIRITFIRE_BOLT                 = 147446,
    SPELL_SPIRITFLAME_STRIKE              = 147448,
    SPELL_JADEFIRE_BOLT                   = 147454,
    SPELL_JADEFLAME_STRIKE                = 147456,

    // Giants
    SPELL_BOULDER                         = 147546,
    SPELL_FRENZY                          = 147510,
    SPELL_STOMP                           = 147500,
    SPELL_FRENZY_STOMP                    = 147512,
    SPELL_EXHAUSTION                      = 147513,

    // Frogs
    SPELL_GULP_FROG_TOXIN                 = 147655,
    SPELL_TOXIC_SKIN                      = 147654,
    SPELL_INSTAKILL_TOXIN                 = 147656,

    // Dragons
    SPELL_FIRE_BLOSSOM                    = 147818,
    SPELL_FLAME_BREATH                    = 147817,
    SPELL_LIGHTNING_BREATH                = 147826,
    SPELL_STORM_BLOSSOM                   = 147828,

    // Ordos minions
    SPELL_BLAZING_BLOW                    = 148003,
    SPELL_CONJURE_ETERNAL_KILN            = 148004,
    SPELL_FIERY_CHARGE                    = 147704,
    SPELL_CAUTERIZE                       = 147997,
    SPELL_CONJURE_FLARECORE_GOLEM         = 148001,
    SPELL_FIRE_STORM                      = 147998,
    SPELL_GREATER_PYROBLAST               = 148002,

    // Eternal Kiln
    SPELL_KILNFIRE                        = 148005,

    // Damp shamblers
    SPELL_MURKY_CLOUD                     = 147805,
    SPELL_RENEWING_MISTS                  = 147769,
    SPELL_POISON_BREATH                   = 144892,

    // Ordon guards
    SPELL_CRACKING_BLOW                   = 147674,
    SPELL_DEFENSIVE_SHIELD                = 147689,
    SPELL_OATH_OF_GUARDIANSHIP            = 147682,

    // Ordos warriors
    SPELL_BLAZING_CLEAVE                  = 147702,
    SPELL_BURNING_FURY                    = 147703,
    SPELL_FORAGE                          = 149463,

    // Ordon watchers
    SPELL_FALLING_FLAMES                  = 147723,
    SPELL_PYROBLAST                       = 147720,

    // Karkanos
    SPELL_GUSHING_MAW                     = 147843,

    // Zesqua
    SPELL_FURIOUS_SPLASH                  = 147335,
    SPELL_WATER_BLOSSOM                   = 147439,
    SPELL_WATER_BOLT_VOLLEY               = 147344,

    // Jademist Dancer
    SPELL_STEAM_BLAST                     = 147863,

    // Molten Guardian
    SPELL_MOLTEN_INFERNO_1                = 147879,
    SPELL_MOLTEN_INFERNO_2                = 147880,
    SPELL_MOLTEN_INFERNO_3                = 147881,

    // Cranegnasher
    SPELL_BITE_MORSEL                     = 147400,
    SPELL_CLAW                            = 147395,
    SPELL_POUNCE                          = 147394,
    SPELL_RAKE                            = 147396,
    SPELL_STEALTH                         = 128933,

    // Nice Sprite
    SPELL_NATURE_S_WRATH                  = 114997,
    SPELL_STICKY_SAP                      = 144424,

    // Ordon Candlekeeper
    SPELL_BREATH_OF_FIRE                  = 147416,
    SPELL_BURNING_SACRIFICE               = 147422,

    // Garnia
    SPELL_RUBY_BOLT                       = 147896,

    // Rattleskew
    SPELL_CURSE_OF_PAIN                   = 38048,
    SPELL_DROWNING_DEATH                  = 16555,
    SPELL_SHADOW_BOLT                     = 43330,
    SPELL_SPIKED_BONE                     = 128511,

    // Southsea Lookout
    SPELL_SPY_GLASS                       = 141528,

    // Leafmender
    SPELL_GREAT_WRATH                     = 147562,
    SPELL_HEALING_TOUCH                   = 147563,
    SPELL_SUN_WORSHIP                     = 147565,

    // Playful Water Spirit
    SPELL_BUBBLE_POP                      = 147444,
    SPELL_WATER_BOLT                      = 32011,
    SPELL_WATER_SPOUT                     = 39207,

    // Cove Shark
    SPELL_SAVAGE_BITE                     = 126062,
    SPELL_SCENT_OF_BLOOD                  = 126063,

    // Stinkbraid
    SPELL_LEAPING_CLEAVE                  = 83622,
    SPELL_ROTTEN_STENCH                   = 85253,

    // Spectral Pirate
    SPELL_BLACK_FOG                       = 128049,
    SPELL_SINISTER_STRIKE                 = 129040,
};

enum GameObjects
{
    GO_CELESTIAL_DOOR                     = 223501,
    GO_HEATET_DOOR                        = 221032,
};

enum EmperorActions
{
    ACTION_XUEN           = 1,
    ACTION_CHIJI          = 2,
    ACTION_NIUZAO         = 3,
    ACTION_YULON          = 4,
    ACTION_MOVE_TO_MIDDLE = 100
};

enum EmprerorTalk
{
    EMPEROR_TALK_INTRO_YULON,
    EMPEROR_TALK_INTRO_XUEN,
    EMPEROR_TALK_INTRO_CHIJI,
    EMPEROR_TALK_INTRO_NIUZAO,
    EMPEROR_TALK_OUTRO_YULON,
    EMPEROR_TALK_OUTRO_XUEN,
    EMPEROR_TALK_OUTRO_CHIJI,
    EMPEROR_TALK_OUTRO_NIUZAO,
    EMPEROR_TALK_INTRO
};

enum TimelessRareChests
{
    // Objects
    GO_SUNKEN_TREASURE                = 220832,
    GO_GLEAMING_TREASURE_CHEST        = 220901,
    GO_ROPE_BOUND_TREASURE_CHEST      = 220902,
    GO_MIST_COVERED_TREASURE_CHEST    = 220908,
    GO_BLACKGUARDS_JETSAM             = 220986,
    GO_GLEAMING_TREASURE_SATCHEL      = 221036,
    GO_SKULL_COVERED_CHEST            = 221617,
    GO_STURDY_CHEST_1                 = 221671,
    GO_STURDY_CHEST_2                 = 223116,
    GO_STURDY_CHEST_3                 = 223117,
    GO_STURDY_CHEST_4                 = 223118,
    GO_SMOLDERING_CHEST_1             = 221672,
    GO_SMOLDERING_CHEST_2             = 223119,
    GO_BLAZING_CHEST                  = 221673,
    GO_MOSS_COVERED_CHEST_1           = 221670,
    GO_MOSS_COVERED_CHEST_2           = 223084,
    GO_MOSS_COVERED_CHEST_3           = 223085,
    GO_MOSS_COVERED_CHEST_4           = 223086,
    GO_MOSS_COVERED_CHEST_5           = 223087,
    GO_MOSS_COVERED_CHEST_6           = 223088,
    GO_MOSS_COVERED_CHEST_7           = 223089,
    GO_MOSS_COVERED_CHEST_8           = 223090,
    GO_MOSS_COVERED_CHEST_9           = 223091,
    GO_MOSS_COVERED_CHEST_10          = 223092,
    GO_MOSS_COVERED_CHEST_11          = 223093,
    GO_MOSS_COVERED_CHEST_12          = 223094,
    GO_MOSS_COVERED_CHEST_13          = 223095,
    GO_MOSS_COVERED_CHEST_14          = 223096,
    GO_MOSS_COVERED_CHEST_15          = 223097,
    GO_MOSS_COVERED_CHEST_16          = 223098,
    GO_MOSS_COVERED_CHEST_17          = 223099,
    GO_MOSS_COVERED_CHEST_18          = 223100,
    GO_MOSS_COVERED_CHEST_19          = 223101,
    GO_MOSS_COVERED_CHEST_20          = 223102,
    GO_MOSS_COVERED_CHEST_21          = 223103,
    GO_MOSS_COVERED_CHEST_22          = 223104,
    GO_MOSS_COVERED_CHEST_23          = 223105,
    GO_MOSS_COVERED_CHEST_24          = 223106,
    GO_MOSS_COVERED_CHEST_25          = 223107,
    GO_MOSS_COVERED_CHEST_26          = 223108,
    GO_MOSS_COVERED_CHEST_27          = 223109,
    GO_MOSS_COVERED_CHEST_28          = 223110,
    GO_MOSS_COVERED_CHEST_29          = 223111,
    GO_MOSS_COVERED_CHEST_30          = 223112,
    GO_MOSS_COVERED_CHEST_31          = 223113,
    GO_MOSS_COVERED_CHEST_32          = 223114,
    GO_MOSS_COVERED_CHEST_33          = 223115,
    GO_MOSS_COVERED_CHEST_34          = 223204,
    GO_NEVERENDING_SPRITEWOOD         = 220807,
    GO_CHALLENGERS_STONE              = 223203,
    GO_CRANE_PLATFORM                 = 220907,
    GO_CRANE_CLOUD                    = 220906,

    // Quests
    QUEST_SUNKEN_TREASURE             = 32957,
    QUEST_GLEAMING_TREASURE_CHEST     = 32969,
    QUEST_ROPE_BOUND_TREASURE_CHEST   = 32968,
    QUEST_MIST_COVERED_TREASURE_CHEST = 32971,
    QUEST_BLACKGUARDS_JETSAM          = 32956,
    QUEST_GLEAMING_TREASURE_SATCHEL   = 32970,
    QUEST_SKULL_COVERED_CHEST         = 33203,
    QUEST_STURDY_CHEST_1              = 33204,
    QUEST_STURDY_CHEST_2              = 33205,
    QUEST_STURDY_CHEST_3              = 33206,
    QUEST_STURDY_CHEST_4              = 33207,
    QUEST_SMOLDERING_CHEST_1          = 33208,
    QUEST_SMOLDERING_CHEST_2          = 33209,
    QUEST_BLAZING_CHEST               = 33210,
    QUEST_MOSS_COVERED_CHEST_1        = 33170,
    QUEST_MOSS_COVERED_CHEST_2        = 33171,
    QUEST_MOSS_COVERED_CHEST_3        = 33172,
    QUEST_MOSS_COVERED_CHEST_4        = 33173,
    QUEST_MOSS_COVERED_CHEST_5        = 33174,
    QUEST_MOSS_COVERED_CHEST_6        = 33175,
    QUEST_MOSS_COVERED_CHEST_7        = 33176,
    QUEST_MOSS_COVERED_CHEST_8        = 33177,
    QUEST_MOSS_COVERED_CHEST_9        = 33178,
    QUEST_MOSS_COVERED_CHEST_10       = 33179,
    QUEST_MOSS_COVERED_CHEST_11       = 33180,
    QUEST_MOSS_COVERED_CHEST_12       = 33181,
    QUEST_MOSS_COVERED_CHEST_13       = 33182,
    QUEST_MOSS_COVERED_CHEST_14       = 33183,
    QUEST_MOSS_COVERED_CHEST_15       = 33184,
    QUEST_MOSS_COVERED_CHEST_16       = 33185,
    QUEST_MOSS_COVERED_CHEST_17       = 33186,
    QUEST_MOSS_COVERED_CHEST_18       = 33187,
    QUEST_MOSS_COVERED_CHEST_19       = 33188,
    QUEST_MOSS_COVERED_CHEST_20       = 33189,
    QUEST_MOSS_COVERED_CHEST_21       = 33190,
    QUEST_MOSS_COVERED_CHEST_22       = 33191,
    QUEST_MOSS_COVERED_CHEST_23       = 33192,
    QUEST_MOSS_COVERED_CHEST_24       = 33193,
    QUEST_MOSS_COVERED_CHEST_25       = 33194,
    QUEST_MOSS_COVERED_CHEST_26       = 33195,
    QUEST_MOSS_COVERED_CHEST_27       = 33196,
    QUEST_MOSS_COVERED_CHEST_28       = 33197,
    QUEST_MOSS_COVERED_CHEST_29       = 33198,
    QUEST_MOSS_COVERED_CHEST_30       = 33199,
    QUEST_MOSS_COVERED_CHEST_31       = 33200,
    QUEST_MOSS_COVERED_CHEST_32       = 33201,
    QUEST_MOSS_COVERED_CHEST_33       = 33202,
    QUEST_MOSS_COVERED_CHEST_34       = 33227,
    QUEST_NEVERENDING_SPRITEWOOD      = 32961,
    QUEST_ZARHYM_TRACKER              = 32962,
    QUEST_ARCHIEREUS_OF_FLAME         = 33312,
};

static std::map<uint32, uint32> timelessRareChestsMap =
{
    { GO_SUNKEN_TREASURE,             QUEST_SUNKEN_TREASURE             },
    { GO_GLEAMING_TREASURE_CHEST,     QUEST_GLEAMING_TREASURE_CHEST     },
    { GO_ROPE_BOUND_TREASURE_CHEST,   QUEST_ROPE_BOUND_TREASURE_CHEST   },
    { GO_MIST_COVERED_TREASURE_CHEST, QUEST_MIST_COVERED_TREASURE_CHEST },
    { GO_CRANE_PLATFORM,              QUEST_MIST_COVERED_TREASURE_CHEST },
    { GO_CRANE_CLOUD,                 QUEST_MIST_COVERED_TREASURE_CHEST },
    { GO_BLACKGUARDS_JETSAM,          QUEST_BLACKGUARDS_JETSAM          },
    { GO_GLEAMING_TREASURE_SATCHEL,   QUEST_GLEAMING_TREASURE_SATCHEL   },
    { GO_SKULL_COVERED_CHEST,         QUEST_SKULL_COVERED_CHEST         },
    { GO_STURDY_CHEST_1,              QUEST_STURDY_CHEST_1              },
    { GO_STURDY_CHEST_2,              QUEST_STURDY_CHEST_2              },
    { GO_STURDY_CHEST_3,              QUEST_STURDY_CHEST_3              },
    { GO_STURDY_CHEST_4,              QUEST_STURDY_CHEST_4              },
    { GO_SMOLDERING_CHEST_1,          QUEST_SMOLDERING_CHEST_1          },
    { GO_SMOLDERING_CHEST_2,          QUEST_SMOLDERING_CHEST_2          },
    { GO_BLAZING_CHEST,               QUEST_BLAZING_CHEST               },
    { GO_MOSS_COVERED_CHEST_1,        QUEST_MOSS_COVERED_CHEST_1        },
    { GO_MOSS_COVERED_CHEST_2,        QUEST_MOSS_COVERED_CHEST_2        },
    { GO_MOSS_COVERED_CHEST_3,        QUEST_MOSS_COVERED_CHEST_3        },
    { GO_MOSS_COVERED_CHEST_4,        QUEST_MOSS_COVERED_CHEST_4        },
    { GO_MOSS_COVERED_CHEST_5,        QUEST_MOSS_COVERED_CHEST_5        },
    { GO_MOSS_COVERED_CHEST_6,        QUEST_MOSS_COVERED_CHEST_6        },
    { GO_MOSS_COVERED_CHEST_7,        QUEST_MOSS_COVERED_CHEST_7        },
    { GO_MOSS_COVERED_CHEST_8,        QUEST_MOSS_COVERED_CHEST_8        },
    { GO_MOSS_COVERED_CHEST_9,        QUEST_MOSS_COVERED_CHEST_9        },
    { GO_MOSS_COVERED_CHEST_10,       QUEST_MOSS_COVERED_CHEST_10       },
    { GO_MOSS_COVERED_CHEST_11,       QUEST_MOSS_COVERED_CHEST_11       },
    { GO_MOSS_COVERED_CHEST_12,       QUEST_MOSS_COVERED_CHEST_12       },
    { GO_MOSS_COVERED_CHEST_13,       QUEST_MOSS_COVERED_CHEST_13       },
    { GO_MOSS_COVERED_CHEST_14,       QUEST_MOSS_COVERED_CHEST_14       },
    { GO_MOSS_COVERED_CHEST_15,       QUEST_MOSS_COVERED_CHEST_15       },
    { GO_MOSS_COVERED_CHEST_16,       QUEST_MOSS_COVERED_CHEST_16       },
    { GO_MOSS_COVERED_CHEST_17,       QUEST_MOSS_COVERED_CHEST_17       },
    { GO_MOSS_COVERED_CHEST_18,       QUEST_MOSS_COVERED_CHEST_18       },
    { GO_MOSS_COVERED_CHEST_19,       QUEST_MOSS_COVERED_CHEST_19       },
    { GO_MOSS_COVERED_CHEST_20,       QUEST_MOSS_COVERED_CHEST_20       },
    { GO_MOSS_COVERED_CHEST_21,       QUEST_MOSS_COVERED_CHEST_21       },
    { GO_MOSS_COVERED_CHEST_22,       QUEST_MOSS_COVERED_CHEST_22       },
    { GO_MOSS_COVERED_CHEST_23,       QUEST_MOSS_COVERED_CHEST_23       },
    { GO_MOSS_COVERED_CHEST_24,       QUEST_MOSS_COVERED_CHEST_24       },
    { GO_MOSS_COVERED_CHEST_25,       QUEST_MOSS_COVERED_CHEST_25       },
    { GO_MOSS_COVERED_CHEST_26,       QUEST_MOSS_COVERED_CHEST_26       },
    { GO_MOSS_COVERED_CHEST_27,       QUEST_MOSS_COVERED_CHEST_27       },
    { GO_MOSS_COVERED_CHEST_28,       QUEST_MOSS_COVERED_CHEST_28       },
    { GO_MOSS_COVERED_CHEST_29,       QUEST_MOSS_COVERED_CHEST_29       },
    { GO_MOSS_COVERED_CHEST_30,       QUEST_MOSS_COVERED_CHEST_30       },
    { GO_MOSS_COVERED_CHEST_31,       QUEST_MOSS_COVERED_CHEST_31       },
    { GO_MOSS_COVERED_CHEST_32,       QUEST_MOSS_COVERED_CHEST_32       },
    { GO_MOSS_COVERED_CHEST_33,       QUEST_MOSS_COVERED_CHEST_33       },
    { GO_MOSS_COVERED_CHEST_34,       QUEST_MOSS_COVERED_CHEST_34       },
    { GO_NEVERENDING_SPRITEWOOD,      QUEST_NEVERENDING_SPRITEWOOD      },
};

static Position _timelessIsleMiddle = { -650.04f, -5016.84f, -6.27f + 2.0f, 1.573f };

static void RewardPlayers(Creature* me)
{
    std::set<uint32> rewardedGuilds;

    for (auto&& ref : me->GetThreatManager().GetUnsortedThreatList())
    {
        if (Player* target = ObjectAccessor::GetPlayer(*me, ref->GetVictim()->GetGUID()))
        {
            target->KilledMonsterCredit(me->GetEntry());
            if (Guild* guild = target->GetGuild())
            {
                if (rewardedGuilds.find(guild->GetId()) == rewardedGuilds.end())
                {
                    guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, me->GetEntry(), 1, 0, me, target);
                    rewardedGuilds.insert(guild->GetId());
                }
            }
        }
    }

    if (auto group = me->GetLootRecipientGroup())
    {
        if (group->GetFirstMember())
            group->GetFirstMember()->GetSource()->RewardPersonalLootAndCurrency(me, 71953);
    }
    else if (auto player = me->GetLootRecipient())
        player->RewardPersonalLootAndCurrency(me, 71953);
}

static void HandleDoor(Creature* me, uint32 go, bool open)
{
    std::list<GameObject*> objects;
    me->GetGameObjectListWithEntryInGrid(objects, go, 300.0f);
    for (auto&& door : objects)
        door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

static void UpdateHealth(Creature* me)
{
    uint8 count = 0;
    for (auto&& threat : me->GetThreatManager().GetUnsortedThreatList())
        if (Player* player = Unit::GetPlayer(*me, threat->GetVictim()->GetGUID()))
            if (player->IsWithinDist(me, 100.0f))
                count++;

    if (count > 35)
        count = 35;

    auto info = sObjectMgr->GetCreatureScalingData(me->GetEntry(), count);
    if (!info)
        return;

    me->RecalculateDynamicHealth(info->Health);
}

#define MIDDLE_FACING_ANGLE 1.573f
