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
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "Vehicle.h"
#include "CombatAI.h"
#include "Random.h"

enum ZoneKunLaiSummitSpellData
{
    SPELL_VICIOUS_REND                      = 125624,
    SPELL_GRAPPLING_HOOK                    = 125623,
    SPELL_VANISH                            = 125632,
    SPELL_SMOKED_BLADE                      = 125633,
    SPELL_INEVITABILITY                     = 135843,

    // Tongue of Ba-Shon          
    SPELL_SUMMON_CHO_FORCE                  = 118996,
    SPELL_SUMMON_CHO                        = 118953,
    SPELL_CHO_MEDITATION                    = 118999,
    SPELL_CHO_OBJ_COMPLETE                  = 118957,
    SPELL_CHO_INVISIBILITY_DETECT           = 119016,
    SPELL_RECLAIM_STRENGTH                  = 118906,
    SPELL_WILD_MAGICS                       = 120373,
    SPELL_ENRAGE                            = 115430,

    // Begin your training: Master Cheng
    SPELL_SUMMON_GUIDE_TOUR                 = 131365,
    SPELL_SUMMON_GUIDE_TOUR_FORCE           = 131367,

    // Celestial Experience
    SPELL_HATED_BLAST                       = 126628,
    SPELL_SHA_CORRUPTION                    = 126625,
    SPELL_DEVASTATION                       = 126631,
    SPELL_WHIRWIND_OF_ANGER                 = 126633,
    SPELL_GARROSH_TRANSFORM                 = 126562,
    SPELL_VARIAN_TRANSFORM                  = 126564,

    SPELL_BLADE_FURY                        = 125370,
    SPELL_TORNADO                           = 125398,
    SPELL_TORNADO_DMG                       = 131693,
    SPELL_WINDSONG                          = 125373,

    SPELL_DRAIN_LIFE                        = 84533,
    SPELL_SHADOW_BOLT                       = 9613,
    SPELL_SHADOW_CRASH                      = 129132,

    SPELL_UNSTABLE_SERUM                    = 127373,

    SPELL_RAIN_DANCE                        = 124860,
    SPELL_TORRENT                           = 124935,
    SPELL_WATER_BOLT                        = 124854,

    SPELL_WICKED_BLADE                      = 115024,
    SPELL_GROUND_SMASH                      = 115015,
    SPELL_PETRIFYING_STRIKE                 = 118514,
    SPELL_SWOOP                             = 55079,
    SPELL_HAWK_REND                         = 114881,
    SPELL_SLAM                              = 91335,
    SPELL_BACKHAND                          = 120371,
    SPELL_STOMP                             = 84709,
    SPELL_KNEE_SMACKER                      = 130220,
    SPELL_SNOWBALL                          = 121291,
    SPELL_JADE_LIGHTNING                    = 120869,
    SPELL_JADE_STRIKE                       = 113503,

    SPELL_POSSESSED_BY_RAGE                 = 121154,

    SPELL_BELLOWING_RAGE                    = 124297,
    SPELL_HOOF_STOMP                        = 124289,
    SPELL_RUSHING_CHARGE                    = 124302,

    SPELL_SHADOWBOLTS                       = 125212,
    SPELL_VOIDCLOUD                         = 125241,

    SPELL_BANANA_RANG                       = 125311,
    SPELL_GOING_BANANAS                     = 125363,
    SPELL_TOSS_FILTH                        = 125365,

    SPELL_EARTH_SHOCK                       = 15501,
    SPELL_LIGHTNING_BOLT                    = 9532,
    SPELL_SLING_DERK                        = 114249,
    SPELL_POKE_YOU                          = 121249,

    SPELL_CLEANUP_SHA_POOL_VISUAL           = 120138,
    SPELL_GORAI_STAFF_QUEST_DUMMY           = 119729,
    SPELL_SHA_POOL_EFFECT                   = 119542,

    SPELL_AGONIZING_STRIKE                  = 58504,

    SPELL_COSMETIC_EXPLOSION                = 46419,

    SPELL_THE_BURLAP_GRIND_BEGIN_RIDE       = 117621,
    SPELL_HEAVY_HANDED                      = 117675
};

enum ZoneKunLaiSummitEvents
{
    EVENT_VICIOUS_REND                      = 1,
    EVENT_GRAPPLING_HOOK,
    EVENT_VANISH,
    EVENT_SMOKED_BLADE,
    EVENT_RECLAIM_STRG,
    EVENT_WILD_MAGIC,
    EVENT_HATED_BLAST,
    EVENT_SHA_CORRUPTION,
    EVENT_DEVASTATION,
    EVENT_WHIRLWIND,
    EVENT_INEVITABILITY,

    EVENT_BLADE_FURY,
    EVENT_TORNADO,
    EVENT_WINDSONG,

    EVENT_DRAIN_LIFE,
    EVENT_SHADOW_BOLT,
    EVENT_SHADOW_CRASH,

    EVENT_UNSTABLE_SERUM,

    EVENT_RAIN_DANCE,
    EVENT_TORRENT,
    EVENT_WATER_BOLT,

    EVENT_WICKED_BLADE,
    EVENT_GROUND_SMASH,
    EVENT_PETRFYING_STRIKE,
    EVENT_SWOOP,
    EVENT_HAWK_REND,
    EVENT_SLAM,
    EVENT_BACKHAND,
    EVENT_STOMP,
    EVENT_KNEE_SMACKER,
    EVENT_SNOWBALL,
    EVENT_JADE_LIGHTNING,
    EVENT_JADE_STRIKE,

    EVENT_BELLOWING_RAGE,
    EVENT_HOOF_STOMP,
    EVENT_RUSHING_CHARGE,

    EVENT_SHADOWBOLTS,
    EVENT_VOIDCLOUD,

    EVENT_BANANA_RANG,
    EVENT_GOING_BANANAS,
    EVENT_TOSS_FILTH,

    EVENT_EARTH_SHOCK,
    EVENT_LIGHTNING_BOLT,
    EVENT_SLING_DERK,
    EVENT_POKE_YOU,

    EVENT_AGONIZING_STRIKE
};

enum ZoneKunLaiSummitCreatures
{
    NPC_LOREWALKER_CHO_TONGUE_OF_BASHON     = 61315,
    NPC_SHAN_KIEN                           = 68357,
    NPC_RECLAIMER_ZUAN                      = 61305,
    NPC_YACHI                               = 61307,
    NPC_YAO                                 = 61306,
    NPC_INITIATE_CHUANG                     = 66844,
    NPC_INITIATE_CHUANGE_SPAWN              = 66854, // need for cast spell on him
    NPC_MASTER_HIGHT                        = 66260,
    NPC_XUEN                                = 64528,
    NPC_ANDUIN_WRYNN                        = 64540,
    NPC_ZHI_HARMONIOUS                      = 64537,
    NPC_TARAN_ZHU                           = 64536,
    NPC_SUNWALKER_DEZCO                     = 64542,
    NPC_SPIRIT_OF_VIOLENCE                  = 64656,
    NPC_SPIRIT_OF_ANGER                     = 64684,
    NPC_SPIRIT_OF_HATRED_H                  = 64742,
    NPC_SPIRIT_OF_HATRED_A                  = 64744,
    NPC_TORNADO                             = 64267,
    NPC_SWORDMISTRESS_MEI                   = 59273,
    NPC_EXPLOSIVE_HATRED                    = 61530,
    NPC_FARMSTEAD_SLAVE                     = 59577,
    NPC_URBATAAR                            = 59483,
    NPC_EASTERN_OIL_RIG                     = 60096,
    NPC_SOUTHERN_OIL_RIG                    = 60098,
    NPC_WESTERN_OIL_RIG                     = 60099
};

enum ZoneKunLaiSummitQuests
{
    QUEST_MEMORY_WINE_A                     = 32371,
    QUEST_MEMORY_WINE_H                     = 32368,
    QUEST_TONGUE_OF_BASHON                  = 30829,
    QUEST_BEGIN_YOUR_TRAINING               = 31834,
    QUEST_CELESTIAL_EXPERIENCE_H            = 31394,
    QUEST_CELESTIAL_EXPERIENCE_A            = 31395,
    QUEST_A_KNOCKOFF_GRUMPLEFLOOT           = 32836,
    QUEST_BURLAP_GRIND                      = 30821,
    QUEST_BOOM_BOOMS_FUSE                   = 32840,
    QUEST_UNBELIEVABLE                      = 30752,
    QUEST_THE_RITUAL                        = 30480,
    QUEST_FARMHAND_FREEDOM                  = 30571,
    QUEST_CHALLENGE_ACCEPTED                = 30514,
    QUEST_FREE_THE_DISSENTERS               = 30967
};

enum ZoneKunLaiSummitItems
{
    ITEM_KNOCKOFF_GRUMPLEFLOOT              = 97978,
    ITEM_HOZEN_FUR_FUSE                     = 97980,
    ITEM_YETI_SHACKLE_KEY                   = 80535
};

enum ZoneKunLaiSummitTexts
{
    SAY_INTRO                               = 0,
    SAY_SPECIAL_1,
    SAY_SPECIAL_2,
    SAY_SPECIAL_3,
    SAY_SPECIAL_4,
    SAY_SPECIAL_5,
    SAY_SPECIAL_6,
    SAY_SPECIAL_7,
    SAY_SPECIAL_8,
    SAY_SPECIAL_9,
    SAY_SPECIAL_10,
    SAY_SPECIAL_11,

    SAY_SENTINEL_1                          = 0,
    SAY_SENTINEL_2,

    SAY_KORDA_TORROS_AGGRO                  = 0,

    SAY_BORGINN_DARKFIST_AGGRO              = 0,

    SAY_SCRITCH_AGGRO                       = 1,

    SAY_YOWLER_AGGRO                        = 0,

    SAY_WATERSPEAKER_GORAI_1                = 0,
    SAY_WATERSPEAKER_GORAI_2,
    SAY_WATERSPEAKER_GORAI_3,
    SAY_WATERSPEAKER_GORAI_4,
    SAY_WATERSPEAKER_GORAI_5,
    SAY_WATERSPEAKER_GORAI_6,
    SAY_WATERSPEAKER_GORAI_7,
    SAY_WATERSPEAKER_GORAI_8,

    SAY_FARMSTEAD_SLAVE                     = 0,

    SAY_URBATAAR                            = 0
};

enum ZoneKunLaiSummitActions
{
    ACTION_ZUAN_DEFEAT,
    ACTION_XUEN_CELESTIAL_EVENT,
    ACTION_REMOVE_DEVASTATION,
};

enum ZoneKunLaiSummitMisc
{
    GOSSIP_MENU_SENTINEL                    = 13843,

    JINYU_CORPSES_CLEANSED_KILL_CREDIT      = 61654,

    RESCUE_DISSENTER_KILL_CREDIT            = 61481
};

const Position BashonSpawn[3]
{
    { 3463.0f, 2120.0f, 1084.0f, 5.6f }, // Zuan
    { 3460.0f, 2118.0f, 1084.2f, 5.6f },
    { 3466.0f, 2124.0f, 1083.0f, 5.6f },
};

const Position ChuangPath[19]
{
    { 3827.57f, 1789.56f, 950.35f, 3.67f },
    { 3825.11f, 1772.85f, 951.00f, 4.56f },
    { 3834.47f, 1742.70f, 950.10f, 5.00f },
    { 3841.09f, 1722.93f, 946.42f, 1.43f },
    { 3849.49f, 1724.63f, 946.24f, 0.05f }, // 2
    { 3873.41f, 1725.53f, 932.66f, 0.04f },
    { 3865.27f, 1700.08f, 932.66f, 4.40f },
    { 3883.61f, 1743.41f, 932.51f, 1.01f }, // 3
    { 3893.68f, 1757.35f, 918.93f, 0.97f },
    { 3896.73f, 1769.72f, 917.98f, 1.34f },
    { 3899.53f, 1785.60f, 905.24f, 1.42f },
    { 3924.55f, 1823.06f, 904.33f, 0.53f },
    { 3983.23f, 1833.73f, 904.33f, 6.16f }, // 4
    { 3958.52f, 1807.12f, 904.33f, 4.05f }, // 5 [m_ori+pi/2]
    { 3931.98f, 1792.54f, 904.33f, 3.58f }, // 6
    { 3904.75f, 1809.04f, 904.33f, 2.50f }, // 7
    { 3905.49f, 1835.16f, 904.33f, 1.54f }, // 8 [m_ori+pi/2]
    { 3913.37f, 1854.60f, 904.24f, 1.18f }, // 9
    { 3941.24f, 1883.54f, 904.35f, 0.80f },
};

const Position CelestialProtectorsSpawn[4]
{
    { 3822.69f, 518.68f, 639.10f, 1.21f }, // Taran
    { 3825.66f, 541.35f, 639.17f, 4.64f }, // Dezco
    { 3817.75f, 544.83f, 639.00f, 5.53f }, // Anduin
    { 3814.79f, 517.63f, 639.00f, 0.66f }, // Zhi
};

const std::map<uint32, uint32> CelestialDefenders =
{
    { NPC_ANDUIN_WRYNN,    2 },
    { NPC_ZHI_HARMONIOUS,  3 },
    { NPC_TARAN_ZHU,       0 },
    { NPC_SUNWALKER_DEZCO, 1 },
};

const Position ChoPointMediation   = { 3485.97f, 2100.91f, 1084.13f, 0.0f  };
const Position PullCelestialPoints = { 3781.31f,  535.48f,  639.00f, 0.0f  };
const Position ArenaCenterPos      = { 3795.73f,  533.61f,  639.00f, 2.97f };

const std::vector<AuraType> controlMechanicType =
{
    SPELL_AURA_MOD_STUN,
    SPELL_AURA_MOD_FEAR,
    SPELL_AURA_MOD_FEAR_2,
    SPELL_AURA_MOD_BLIND,
    SPELL_AURA_MOD_CONFUSE,
};

const Position pootPootLeavePos [2] = 
{ 
    { 3144.28f, 1784.07f, 628.582f, 0.0f },
    { 3105.52f, 1777.65f, 634.111f, 0.0f },
};

struct npc_nessos_the_oracle : public ScriptedAI
{
    npc_nessos_the_oracle(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_VICIOUS_REND, 7s);
        events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 17s);
        events.ScheduleEvent(EVENT_VANISH, 12s);
    }

    void JustDied(Unit* /*killer*/) override { }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_VICIOUS_REND:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_VICIOUS_REND, false);

                    events.ScheduleEvent(EVENT_VICIOUS_REND, 7s);
                    break;
                case EVENT_GRAPPLING_HOOK:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_GRAPPLING_HOOK, false);

                    events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 17s);
                    break;
                case EVENT_VANISH:
                    me->CastSpell(me, SPELL_VANISH, false);
                    events.ScheduleEvent(EVENT_VANISH, 20s);
                    events.ScheduleEvent(EVENT_SMOKED_BLADE, randtime(0ms, 8s));
                    break;
                case EVENT_SMOKED_BLADE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_SMOKED_BLADE, false);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_ski_thik : public ScriptedAI
{
    npc_ski_thik(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_BLADE_FURY, 8s);
        events.ScheduleEvent(EVENT_TORNADO, 40s);
        events.ScheduleEvent(EVENT_WINDSONG, 32s);
    }

    void JustDied(Unit* /*killer*/) override { }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_TORNADO)
        {
            summon->AddAura(SPELL_TORNADO_DMG, summon);
            summon->SetReactState(REACT_PASSIVE);
            summon->GetMotionMaster()->MoveRandom(20.0f);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BLADE_FURY:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_BLADE_FURY, false);

                    events.ScheduleEvent(EVENT_BLADE_FURY, 8s);
                    break;
                case EVENT_TORNADO:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_TORNADO, false);

                    events.ScheduleEvent(EVENT_TORNADO, 40s);
                    break;
                case EVENT_WINDSONG:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_WINDSONG, false);

                    events.ScheduleEvent(EVENT_WINDSONG, 32s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_mogujia_soul_caller : public ScriptedAI
{
    npc_mogujia_soul_caller(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_DRAIN_LIFE, 20s);
        events.ScheduleEvent(EVENT_SHADOW_BOLT, 15s);
        events.ScheduleEvent(EVENT_SHADOW_CRASH, 32s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DRAIN_LIFE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_DRAIN_LIFE, false);

                    events.ScheduleEvent(EVENT_DRAIN_LIFE, 20s);
                    break;
                case EVENT_SHADOW_BOLT:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_SHADOW_BOLT, false);

                    events.ScheduleEvent(EVENT_SHADOW_BOLT, 15s);
                    break;
                case EVENT_SHADOW_CRASH:
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        me->CastSpell(target, SPELL_SHADOW_CRASH, false);

                    events.ScheduleEvent(EVENT_SHADOW_CRASH, 32s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_quilen_stonemaw : public ScriptedAI
{
    npc_quilen_stonemaw(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_UNSTABLE_SERUM, 5s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_UNSTABLE_SERUM:
                    if (Unit* target = me->SelectNearestTarget(5.0f))
                        me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);

                    events.ScheduleEvent(EVENT_UNSTABLE_SERUM, 5s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_zai_the_outcast : public ScriptedAI
{
    npc_zai_the_outcast(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_RAIN_DANCE, 8s);
        events.ScheduleEvent(EVENT_TORRENT, 40s);
        events.ScheduleEvent(EVENT_WATER_BOLT, 32s);
    }

    void JustDied(Unit* /*killer*/) override { }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_TORNADO)
        {
            summon->AddAura(SPELL_TORNADO_DMG, summon);
            summon->SetReactState(REACT_PASSIVE);
            summon->GetMotionMaster()->MoveRandom(20.0f);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_RAIN_DANCE:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_RAIN_DANCE, false);

                    events.ScheduleEvent(EVENT_RAIN_DANCE, 8s);
                    break;
                case EVENT_TORRENT:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_TORRENT, false);

                    events.ScheduleEvent(EVENT_TORRENT, 40s);
                    break;
                case EVENT_WATER_BOLT:
                    if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                        me->CastSpell(target, SPELL_WATER_BOLT, false);

                    events.ScheduleEvent(EVENT_WATER_BOLT, 32s);
                    break;

                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_terracotta_gladiator : public ScriptedAI
{
    npc_terracotta_gladiator(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_WICKED_BLADE, randtime(4s + 500ms, 13s));
        events.ScheduleEvent(EVENT_GROUND_SMASH, 10s);
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
                case EVENT_WICKED_BLADE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_WICKED_BLADE);

                    events.ScheduleEvent(EVENT_WICKED_BLADE, randtime(4s + 500ms, 13s));
                    break;
                case EVENT_GROUND_SMASH:
                    DoCast(me, SPELL_GROUND_SMASH);
                    events.ScheduleEvent(EVENT_GROUND_SMASH, 13s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_terracotta_defender : public ScriptedAI
{
    npc_terracotta_defender(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_PETRFYING_STRIKE, randtime(4s + 500ms, 12s));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_PETRFYING_STRIKE)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_PETRIFYING_STRIKE);

                events.ScheduleEvent(EVENT_PETRFYING_STRIKE, randtime(4s + 500ms, 12s));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_upland_raven : public ScriptedAI
{
    npc_upland_raven(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SWOOP, randtime(4s + 500ms, 13s));
        events.ScheduleEvent(EVENT_HAWK_REND, 8s);
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
                case EVENT_SWOOP:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SWOOP);

                    events.ScheduleEvent(EVENT_SWOOP, randtime(4s + 500ms, 13s));
                    break;
                case EVENT_HAWK_REND:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_HAWK_REND);

                    events.ScheduleEvent(EVENT_HAWK_REND, randtime(12s, 18s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_zandalari_warrior : public ScriptedAI
{
    npc_zandalari_warrior(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SLAM, randtime(4s + 500ms, 13s));
        events.ScheduleEvent(EVENT_BACKHAND, 8s);
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
                case EVENT_SLAM:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SLAM);

                    events.ScheduleEvent(EVENT_SLAM, randtime(4s + 500ms, 13s));
                    break;
                case EVENT_BACKHAND:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_BACKHAND);

                    events.ScheduleEvent(EVENT_BACKHAND, randtime(12s, 18s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_hungry_yeti : public ScriptedAI
{
    npc_hungry_yeti(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_STOMP, 8s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_STOMP)
            {
                DoCast(me, SPELL_STOMP);
                events.ScheduleEvent(EVENT_STOMP, 15s);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_mischievous_snow_sprite : public ScriptedAI
{
    npc_mischievous_snow_sprite(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool hasKnee;

    void Reset() override
    {
        events.Reset();
        hasKnee = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SNOWBALL, randtime(4s + 500ms, 6s));
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->HealthBelowPct(31) && !hasKnee && attacker && attacker->IsAlive())
        {
            hasKnee = true;
            DoCast(attacker, SPELL_KNEE_SMACKER);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SNOWBALL)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_SNOWBALL);

                events.ScheduleEvent(EVENT_SNOWBALL, randtime(4s + 500ms, 6s));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_tomb_guardian : public ScriptedAI
{
    npc_tomb_guardian(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_JADE_LIGHTNING, randtime(4s + 500ms, 12s));
        events.ScheduleEvent(EVENT_JADE_STRIKE, 10s);
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
                case EVENT_JADE_LIGHTNING:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_JADE_LIGHTNING);

                    events.ScheduleEvent(EVENT_JADE_LIGHTNING, randtime(4s + 500ms, 12s));
                    break;
                case EVENT_JADE_STRIKE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_JADE_STRIKE);

                    events.ScheduleEvent(EVENT_JADE_STRIKE, 10s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

class npc_waterspeaker_gorai : public CreatureScript
{
    public:
        npc_waterspeaker_gorai() : CreatureScript("npc_waterspeaker_gorai") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (creature->IsVendor())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

            if (player->GetQuestStatus(QUEST_THE_RITUAL) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm ready. Begin the ritual.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->KilledMonsterCredit(creature->GetEntry());
                creature->AI()->SetGUID(player->GetGUID(), ObjectGuid::Empty);
                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                player->CLOSE_GOSSIP_MENU();
            }
            else if (action == GOSSIP_ACTION_TRADE)
                player->GetSession()->SendListInventory(creature->GetGUID());

            return false;
        }

        struct npc_waterspeaker_goraiAI : public ScriptedAI
        {
            npc_waterspeaker_goraiAI(Creature* creature) : ScriptedAI(creature) { }

            uint32 phase;
            uint32 phaseTimer;
            ObjectGuid playerGUID;

            void Reset() override
            {
                phase = 0;
                phaseTimer = 0;
                playerGUID = ObjectGuid::Empty;
            }

            void SummonedCreatureDies(Creature* creature, Unit*  killer) override
            {
                if (auto swordmistress = GetClosestCreatureWithEntry(me, NPC_SWORDMISTRESS_MEI, 30.f))
                {
                    swordmistress->AI()->Talk(0);
                    swordmistress->CastSpell(swordmistress, SPELL_CLEANUP_SHA_POOL_VISUAL, true);
                }

                phase = 7;
                phaseTimer = 5000;
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                playerGUID = guid;
                phase = 1;
                phaseTimer = 10000;
                Talk(SAY_WATERSPEAKER_GORAI_1);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!phase)
                    return;

                if (phaseTimer <= diff)
                {
                    if (!me->SelectNearestPlayer(40.f))
                        Reset();

                    if (phase == 1)
                    {
                        Talk(SAY_WATERSPEAKER_GORAI_2);
                        phaseTimer = 12000;
                    }
                    else if (phase == 2)
                    {
                        me->CastSpell(me, SPELL_GORAI_STAFF_QUEST_DUMMY, true);
                        Talk(SAY_WATERSPEAKER_GORAI_3);
                        phaseTimer = 7000;
                    }
                    else if (phase == 3)
                    {
                        Talk(SAY_WATERSPEAKER_GORAI_4);
                        phaseTimer = 4000;

                        if (auto player = me->GetPlayer(*me, playerGUID))
                            player->KilledMonsterCredit(JINYU_CORPSES_CLEANSED_KILL_CREDIT);
                    }
                    else if (phase == 4)
                    {
                        Talk(SAY_WATERSPEAKER_GORAI_5);
                        phaseTimer = 5000;

                        if (auto swordmistress = GetClosestCreatureWithEntry(me, NPC_SWORDMISTRESS_MEI, 30.f))
                            swordmistress->CastSpell(swordmistress, SPELL_SHA_POOL_EFFECT, true);
                    }
                    else if (phase == 5)
                    {
                        Talk(SAY_WATERSPEAKER_GORAI_6);

                        if (auto spawn = me->SummonCreature(NPC_EXPLOSIVE_HATRED, 1921.28f, 167.02f, 477.382f, 4.82f, TEMPSUMMON_DEAD_DESPAWN, 60 * IN_MILLISECONDS))
                            if (auto player = me->GetPlayer(*me, playerGUID))
                                spawn->AI()->AttackStart(player);
                    }
                    else if (phase == 6) // We return here - phase is finished when summon is killed
                        return;
                    else if (phase == 7)
                    {
                        Talk(SAY_WATERSPEAKER_GORAI_7, me->GetPlayer(*me, playerGUID));
                        phaseTimer = 6000;
                    }
                    else if (phase == 8)
                    {
                        Talk(SAY_WATERSPEAKER_GORAI_8);
                        Reset();
                    }

                    ++phase;
                }
                else
                    phaseTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_waterspeaker_goraiAI(creature);
        }
};

struct npc_ordo_overseer : public ScriptedAI
{
    npc_ordo_overseer(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*victim*/) override
    {
        events.ScheduleEvent(EVENT_AGONIZING_STRIKE, randtime(5s, 10s));
    }

    void JustDied(Unit* killer) override
    {
        if (auto player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            if (player->GetQuestStatus(QUEST_FARMHAND_FREEDOM) == QUEST_STATUS_INCOMPLETE)
            {
                std::list<Creature*> clist;
                GetCreatureListWithEntryInGrid(clist, me, NPC_FARMSTEAD_SLAVE, 20.f);

                for (auto&& slave : clist)
                {
                    player->KilledMonsterCredit(slave->GetEntry());
                    slave->AI()->Talk(SAY_FARMSTEAD_SLAVE);
                    Position pos = slave->GetRandomNearPosition(30.f);
                    slave->GetMotionMaster()->MovePoint(1, pos);
                    slave->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                }
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->GetVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_AGONIZING_STRIKE:
                {
                    DoCastVictim(SPELL_AGONIZING_STRIKE);
                    events.RescheduleEvent(EVENT_AGONIZING_STRIKE, randtime(5s, 10s));
                }
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap events;
};

class go_yaungol_banner : public GameObjectScript
{
    public:
        go_yaungol_banner() : GameObjectScript("go_yaungol_banner") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->GetQuestStatus(QUEST_CHALLENGE_ACCEPTED) == QUEST_STATUS_INCOMPLETE)
            {
                if (GetClosestCreatureWithEntry(player, NPC_URBATAAR, 30.f))
                    return false;

                player->SummonGameObject(NPC_URBATAAR, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, { }, 60 * IN_MILLISECONDS);

                if (auto urbataar = player->SummonCreature(NPC_URBATAAR, 2110.05f, 1182.91f, 476.32f, 0, TEMPSUMMON_TIMED_DESPAWN, 120 * IN_MILLISECONDS))
                {
                    urbataar->AI()->Talk(SAY_URBATAAR);
                    urbataar->AI()->AttackStart(player);
                }
            }
            return true;
        }
};

enum ExplosivesBarrelMiscs 
{
    EMOTE_EXPLO_BARREL_THREE = 0,
    EMOTE_EXPLO_BARREL_TWO   = 1,
    EMOTE_EXPLO_BARREL_ONE   = 2,
    EMOTE_EXPLO_BARREL_BOOM  = 3
};

struct npc_explosives_barrel : public ScriptedAI
{
    npc_explosives_barrel(Creature* creature) : ScriptedAI(creature) { }

    uint32 phase;
    uint32 tickTimer;

    void Reset() override
    {
        phase = 4;
        tickTimer = 1000;
    }

    void UpdateAI(uint32 diff) override
    {
        if (tickTimer <= diff)
        {
            --phase;

            if (auto owner = me->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                if (phase == 3)
                    Talk(EMOTE_EXPLO_BARREL_THREE);
                else if (phase == 2)
                    Talk(EMOTE_EXPLO_BARREL_TWO);
                else if (phase == 1)
                    Talk(EMOTE_EXPLO_BARREL_ONE);
                else if (phase == 0)
                    Talk(EMOTE_EXPLO_BARREL_BOOM);

                if (phase == 0)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        if (auto oilRig = GetClosestCreatureWithEntry(me, NPC_EASTERN_OIL_RIG, 10.f))
                        {
                            me->CastSpell(me, SPELL_COSMETIC_EXPLOSION, true);
                            owner->KilledMonsterCredit(oilRig->GetEntry());
                            me->DespawnOrUnsummon();
                        }
                        else if (auto oilRig = GetClosestCreatureWithEntry(me, NPC_SOUTHERN_OIL_RIG, 10.f))
                        {
                            me->CastSpell(me, SPELL_COSMETIC_EXPLOSION, true);
                            owner->KilledMonsterCredit(oilRig->GetEntry());
                            me->DespawnOrUnsummon();
                        }
                        else if (auto oilRig = GetClosestCreatureWithEntry(me, NPC_WESTERN_OIL_RIG, 10.f))
                        {
                            me->CastSpell(me, SPELL_COSMETIC_EXPLOSION, true);
                            owner->KilledMonsterCredit(oilRig->GetEntry());
                            me->DespawnOrUnsummon();
                        }
                    }
                }
            }

            tickTimer = 2000;
        }
        else
            tickTimer -= diff;
    }
};

class npc_inkgill_dissenter : public CreatureScript
{
    public:
        npc_inkgill_dissenter() : CreatureScript("npc_inkgill_dissenter") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetQuestStatus(QUEST_FREE_THE_DISSENTERS) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "There's still hope - Gorai is still alive, to the south. Go!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->KilledMonsterCredit(RESCUE_DISSENTER_KILL_CREDIT);
                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                creature->AI()->Talk(0);
                creature->DespawnOrUnsummon(3 * IN_MILLISECONDS);
                float x, y, z;
                creature->GetClosePoint(x, y, z, creature->GetObjectSize() / 3, 10.f);
                creature->GetMotionMaster()->MovePoint(1, x, y, z);
                player->CLOSE_GOSSIP_MENU();
            }

            return false;
        }
};

class npc_shado_pan_sentinel : public CreatureScript
{
    public:
        npc_shado_pan_sentinel() : CreatureScript("npc_shado_pan_sentinel") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetQuestStatus(QUEST_UNBELIEVABLE) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM_DB(GOSSIP_MENU_SENTINEL, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->CLOSE_GOSSIP_MENU();
                creature->AI()->Talk(SAY_SENTINEL_1);
                creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

                uint32 delay = 0;
                creature->m_Events.Schedule(delay += 3000, [creature]()
                {
                    creature->AI()->Talk(SAY_SENTINEL_2);
                    creature->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);
                });

                creature->m_Events.Schedule(delay += 5000, [creature]()
                {
                    creature->CastSpell(creature, SPELL_POSSESSED_BY_RAGE);
                });

                creature->m_Events.Schedule(delay += 1000, [creature, player]()
                {
                    creature->SetFaction(14);
                    creature->Attack(player, true);
                });
            }

            return false;
        }

        struct npc_shado_pan_sentinelAI : public ScriptedAI
        {
            npc_shado_pan_sentinelAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetFaction(me->GetCreatureTemplate()->faction);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shado_pan_sentinelAI(creature);
        }
};

class npc_lorewalker_cho_bashon : public CreatureScript
{
    public:
        npc_lorewalker_cho_bashon() : CreatureScript("npc_lorewalker_cho_bashon") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->CastSpell(player, SPELL_SUMMON_CHO_FORCE, true);
                player->AddAura(SPELL_SUMMON_CHO, player);
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(QUEST_TONGUE_OF_BASHON) != QUEST_STATUS_INCOMPLETE)
                return false;

            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

struct npc_lorewalker_cho_bashon_summoned : public ScriptedAI
{
    npc_lorewalker_cho_bashon_summoned(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid summonerGUID;
    uint32 delay;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        me->SetPhaseMask(2, true);
        me->HandleEmoteStateCommand(EMOTE_STATE_USE_STANDING);

        delay = 0;
        me->m_Events.Schedule(delay += 2500, 1, [this]()
        {
            me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
            Talk(SAY_INTRO);
        });

        me->m_Events.Schedule(delay += 4800, 2, [this]()
        {
            Talk(SAY_SPECIAL_1);
            me->GetMotionMaster()->MovePoint(0, ChoPointMediation);
        });

        me->m_Events.Schedule(delay += 6000, 4, [this]()
        {
            me->SummonCreature(NPC_RECLAIMER_ZUAN, BashonSpawn[0], TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS);
            me->SummonCreature(NPC_YACHI, BashonSpawn[1], TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS);
            me->SummonCreature(NPC_YAO, BashonSpawn[2], TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS);
            me->DespawnOrUnsummon(300 * IN_MILLISECONDS);
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (pointId == 0)
        {
            me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() + M_PI));
            DoCast(me, SPELL_CHO_MEDITATION);
            me->HandleEmoteCommand(EMOTE_STATE_FLY_SIT_GROUND);
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_ZUAN_DEFEAT)
        {
            delay = 0;
            me->m_Events.Schedule(delay += 3500, 5, [this]()
            {
                me->RemoveAurasDueToSpell(SPELL_CHO_MEDITATION);
                me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                Talk(SAY_SPECIAL_2);
            });

            me->m_Events.Schedule(delay += 5000, 6, [this]()
            {
                me->DespawnOrUnsummon(1 * IN_MILLISECONDS);

                if (Player* owner = ObjectAccessor::GetPlayer(*me, summonerGUID))
                    owner->RemoveAurasDueToSpell(SPELL_SUMMON_CHO);
            });
        }
    }
};

struct npc_relcaimer_zuan_pets : public ScriptedAI
{
    npc_relcaimer_zuan_pets(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid summonerGUID;
    uint32 delay;
    bool HasEnrage;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        HasEnrage = false;
        me->SetPhaseMask(2, true);

        if (me->GetEntry() == NPC_RECLAIMER_ZUAN)
            Talk(SAY_INTRO);
    }

    void Reset() override
    {
        events.Reset();
        HasEnrage = false;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (me->HealthBelowPct(31) && me->GetEntry() == NPC_RECLAIMER_ZUAN && !HasEnrage)
        {
            HasEnrage = true;
            DoCast(me, SPELL_ENRAGE, true);
        }
    }

    void JustEngagedWith(Unit* /*victim*/) override
    {
        if (me->GetEntry() == NPC_RECLAIMER_ZUAN)
        {
            events.ScheduleEvent(EVENT_RECLAIM_STRG, randtime(5s, 6s));
            events.ScheduleEvent(EVENT_WILD_MAGIC, 15s);
        }
    }

    void JustDied(Unit* /*killer*/) override 
    {
        if (me->GetEntry() == NPC_RECLAIMER_ZUAN)
        {
            DoCast(me, SPELL_CHO_OBJ_COMPLETE);

            if (Creature* owner = ObjectAccessor::GetCreature(*me, summonerGUID))
                owner->AI()->DoAction(ACTION_ZUAN_DEFEAT);
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
                case EVENT_RECLAIM_STRG:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_RECLAIM_STRENGTH);
                        
                    events.ScheduleEvent(EVENT_RECLAIM_STRG, 20s);
                    break;
                case EVENT_WILD_MAGIC:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_WILD_MAGICS);

                    events.ScheduleEvent(EVENT_WILD_MAGIC, 15s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

};

class npc_master_hight : public CreatureScript
{
    public:
        npc_master_hight() : CreatureScript("npc_master_hight") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_BEGIN_YOUR_TRAINING)
            {
                creature->AI()->Talk(SAY_INTRO);

                if (Creature* initiate = player->FindNearestCreature(NPC_INITIATE_CHUANGE_SPAWN, 100.0f, true))
                    player->CastSpell(initiate, SPELL_SUMMON_GUIDE_TOUR_FORCE, true);

                player->AddAura(SPELL_SUMMON_GUIDE_TOUR, player);
            }

            return false;
        }

        struct npc_master_hightAI : public ScriptedAI
        {
            npc_master_hightAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_master_hightAI(creature);
        }
};

struct npc_initiate_chuang_summoned : public ScriptedAI
{
    npc_initiate_chuang_summoned(Creature* creature) : ScriptedAI(creature), init(me) { }

    ObjectGuid summonerGUID;
    uint32 delay, pointMod;
    Movement::MoveSplineInit init;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        pointMod = 0;

        delay = 0;
        me->m_Events.Schedule(delay += 2000, 1, [this]()
        {
            Talk(SAY_INTRO);
        });

        me->m_Events.Schedule(delay += 1500, 2, [this]()
        {
            for (uint8 i = 0; i < 4; ++i)
                init.Path().push_back(G3D::Vector3(ChuangPath[i].GetPositionX(), ChuangPath[i].GetPositionY(), ChuangPath[i].GetPositionZ()));

            init.SetWalk(true);
            init.Launch();

            delay = 0;
            me->m_Events.Schedule(delay += me->GetSplineDuration(), 3, [this]()
            {
                Talk(SAY_SPECIAL_1);
                me->StopMoving();

                delay = 0;
                me->m_Events.Schedule(delay += 2000, 4, [this]()
                {
                    init.Path().clear();

                    for (uint8 i = 4; i < 7; ++i)
                        init.Path().push_back(G3D::Vector3(ChuangPath[i].GetPositionX(), ChuangPath[i].GetPositionY(), ChuangPath[i].GetPositionZ()));

                    init.SetWalk(true);
                    init.Launch();

                    delay = 0;
                    me->m_Events.Schedule(delay += me->GetSplineDuration(), 5, [this]()
                    {
                        Talk(SAY_SPECIAL_2);
                        me->StopMoving();

                        delay = 0;
                        me->m_Events.Schedule(delay += 2000, 6, [this]()
                        {
                            init.Path().clear();

                            for (uint8 i = 7; i < 12; ++i)
                                init.Path().push_back(G3D::Vector3(ChuangPath[i].GetPositionX(), ChuangPath[i].GetPositionY(), ChuangPath[i].GetPositionZ()));

                            init.SetWalk(true);
                            init.Launch();

                            delay = 0;
                            me->m_Events.Schedule(delay += me->GetSplineDuration(), 7, [this]()
                            {
                                Talk(SAY_SPECIAL_3);
                                me->StopMoving();

                                delay = 0;
                                me->m_Events.Schedule(delay += 2000, 8, [this]()
                                {
                                    me->GetMotionMaster()->MovePoint(pointMod, ChuangPath[pointMod + 12]);
                                });
                            });
                        });
                    });
                });
            });
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                Talk(pointId + 4);

                pointMod++;
                delay = 0;
                me->m_Events.Schedule(delay += 3000, pointMod + 8, [this]()
                {
                    me->GetMotionMaster()->MovePoint(pointMod, ChuangPath[pointMod + 12]);
                });
                break;
            case 6:
                Talk(pointId + 4);
                delay = 0;

                me->m_Events.Schedule(delay += 5000, pointMod + 8, [this]()
                {
                    Talk(SAY_SPECIAL_11);
                    me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
                });
                break;
        }
    }
};

class npc_xuen_celestial_experience : public CreatureScript
{
    public:
        npc_xuen_celestial_experience() : CreatureScript("npc_xuen_celestial_experience") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (!creature->GetDBTableGUIDLow() || (player->GetQuestStatus(QUEST_CELESTIAL_EXPERIENCE_A) != QUEST_STATUS_INCOMPLETE && player->GetQuestStatus(QUEST_CELESTIAL_EXPERIENCE_H) != QUEST_STATUS_INCOMPLETE))
                return false;

            player->KilledMonsterCredit(NPC_XUEN);
            player->SummonCreature(NPC_XUEN, *creature, TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS);
            player->SetPhaseMask(3, true);
            player->CLOSE_GOSSIP_MENU();;
            return true;
        }

        struct npc_xuen_celestial_experienceAI : public ScriptedAI
        {
            npc_xuen_celestial_experienceAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;
            std::vector<ObjectGuid> cList;

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetPhaseMask(2, true);
                summonerGUID = summoner->GetGUID();

                for (auto&& cItr : CelestialDefenders)
                {
                    if (Creature* celestialDef = me->SummonCreature(cItr.first, CelestialProtectorsSpawn[cItr.second], TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
                    {
                        cList.push_back(celestialDef->GetGUID());
                        celestialDef->SetPhaseMask(2, true);
                    }
                }

                delay = 0;
                me->m_Events.Schedule(delay += 3000, 1, [this]()
                {
                    Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 6000, 2, [this]()
                {
                    if (Creature* TaranZhu = ObjectAccessor::GetCreature(*me, cList[0]))
                        TaranZhu->AI()->Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 6000, 3, [this]()
                {
                    if (Creature* Anduin = ObjectAccessor::GetCreature(*me, cList[2]))
                        Anduin->AI()->Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 6250, 4, [this]()
                {
                    if (Creature* TaranZhu = ObjectAccessor::GetCreature(*me, cList[0]))
                        TaranZhu->AI()->Talk(SAY_SPECIAL_1);
                });

                me->m_Events.Schedule(delay += 14500, 5, [this]()
                {
                    Talk(SAY_SPECIAL_1);
                });

                me->m_Events.Schedule(delay += 11000, 6, [this]()
                {
                    // Should be sha fontain effect at pull [miss]
                    if (Player* pCaster = ObjectAccessor::GetPlayer(*me, summonerGUID))
                    {
                        pCaster->GetMotionMaster()->MoveJump(PullCelestialPoints, 20.0f, 20.0f);

                        delay = 0;
                        me->m_Events.Schedule(delay += 4000, 7, [this, pCaster]()
                        {
                            if (Creature* SpiritOfViolence = me->SummonCreature(NPC_SPIRIT_OF_VIOLENCE, ArenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                            {
                                SpiritOfViolence->Attack(pCaster, true);
                                SpiritOfViolence->GetMotionMaster()->MoveChase(pCaster);
                            }
                        });

                        me->m_Events.Schedule(delay += 12000, 8, [this]()
                        {
                            if (Creature* TaranZhu = ObjectAccessor::GetCreature(*me, cList[0]))
                                TaranZhu->AI()->Talk(SAY_SPECIAL_2);
                        });

                        me->m_Events.Schedule(delay += 6000, 9, [this]()
                        {
                            if (Creature* Anduin = ObjectAccessor::GetCreature(*me, cList[2]))
                                Anduin->AI()->Talk(SAY_SPECIAL_1);
                        });

                        me->m_Events.Schedule(delay += 12200, 10, [this]()
                        {
                            if (Creature* Zhi = ObjectAccessor::GetCreature(*me, cList[1]))
                                Zhi->AI()->Talk(SAY_INTRO);
                        });
                    }
                });
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case NPC_SPIRIT_OF_VIOLENCE:
                        delay = 0;
                        me->m_Events.Schedule(delay += 5500, 11, [this]()
                        {
                            Talk(SAY_SPECIAL_2);
                        });

                        me->m_Events.Schedule(delay += 5000, 6, [this]()
                        {
                            // Should be sha fontain effect at pull [miss]
                            if (Player* pCaster = ObjectAccessor::GetPlayer(*me, summonerGUID))
                            {
                                pCaster->GetMotionMaster()->MoveJump(PullCelestialPoints, 20.0f, 20.0f);

                                me->m_Events.Schedule(delay += 5000, 12, [this, pCaster]()
                                {
                                    if (Creature* SpiritOfAnger = me->SummonCreature(NPC_SPIRIT_OF_ANGER, ArenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        SpiritOfAnger->Attack(pCaster, true);
                                        SpiritOfAnger->GetMotionMaster()->MoveChase(pCaster);
                                    }
                                });

                                me->m_Events.Schedule(delay += 6600, 13, [this]()
                                {
                                    if (Creature* Anduin = ObjectAccessor::GetCreature(*me, cList[2]))
                                        Anduin->AI()->Talk(SAY_SPECIAL_2);
                                });

                                me->m_Events.Schedule(delay += 13500, 14, [this]()
                                {
                                    if (Creature* TaranZhu = ObjectAccessor::GetCreature(*me, cList[0]))
                                        TaranZhu->AI()->Talk(SAY_SPECIAL_3);
                                });

                                me->m_Events.Schedule(delay += 4500, 15, [this]()
                                {
                                    if (Creature* Zhi = ObjectAccessor::GetCreature(*me, cList[1]))
                                        Zhi->AI()->Talk(SAY_SPECIAL_1);
                                });
                            }
                        });
                        break;
                    case NPC_SPIRIT_OF_ANGER:
                        Talk(SAY_SPECIAL_3);
                        
                        delay = 0;
                        me->m_Events.Schedule(delay += 14000, 16, [this]()
                        {
                            if (Player* pCaster = ObjectAccessor::GetPlayer(*me, summonerGUID))
                            {
                                pCaster->GetMotionMaster()->MoveJump(PullCelestialPoints, 20.0f, 20.0f);

                                me->m_Events.Schedule(delay += 3000, 17, [this, pCaster]()
                                {
                                    if (Creature* SpiritOfHatred = me->SummonCreature(pCaster->GetTeam() == HORDE ? NPC_SPIRIT_OF_HATRED_H : NPC_SPIRIT_OF_HATRED_A, ArenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        SpiritOfHatred->Attack(pCaster, true);
                                        SpiritOfHatred->GetMotionMaster()->MoveChase(pCaster);
                                    }
                                });

                                me->m_Events.Schedule(delay += 14000, 18, [this, pCaster]()
                                {
                                    if (Creature* Anduin = ObjectAccessor::GetCreature(*me, cList[2]))
                                        Anduin->AI()->Talk(SAY_SPECIAL_3);

                                    if (Creature* TaranZhu = ObjectAccessor::GetCreature(*me, cList[0]))
                                        TaranZhu->AI()->Talk(SAY_SPECIAL_4);
                                });

                                me->m_Events.Schedule(delay += 9300, 19, [this, pCaster]()
                                {
                                    if (Creature* Anduin = ObjectAccessor::GetCreature(*me, cList[2]))
                                        Anduin->AI()->Talk(SAY_SPECIAL_4);

                                });

                                me->m_Events.Schedule(delay += 6700, 20, [this, pCaster]()
                                {
                                    if (Creature* Zhi = ObjectAccessor::GetCreature(*me, cList[1]))
                                        Zhi->AI()->Talk(SAY_SPECIAL_2);
                                });

                                me->m_Events.Schedule(delay += 7000, 21, [this, pCaster]()
                                {
                                    if (Creature* Anduin = ObjectAccessor::GetCreature(*me, cList[2]))
                                        Anduin->AI()->Talk(SAY_SPECIAL_5);

                                });
                            }
                        });
                        break;
                    case NPC_SPIRIT_OF_HATRED_A:
                    case NPC_SPIRIT_OF_HATRED_H:
                        for (auto&& cItr : cList)
                            if (Creature* celestialDef = ObjectAccessor::GetCreature(*me, cItr))
                                celestialDef->DespawnOrUnsummon();

                        if (Player* pCaster = ObjectAccessor::GetPlayer(*me, summonerGUID))
                            pCaster->SetPhaseMask(1, true);

                        me->DespawnOrUnsummon();
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_xuen_celestial_experienceAI(creature);
        }
};

struct celestial_experience_sha : public ScriptedAI
{
    celestial_experience_sha(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid summonerGUID, victimGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetFaction(16);
        me->SetPhaseMask(2, true);
        summonerGUID = summoner->GetGUID();
        victimGUID = ObjectGuid::Empty;
        me->DespawnOrUnsummon(300 * IN_MILLISECONDS);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*victim*/) override
    {
        events.ScheduleEvent(EVENT_HATED_BLAST, randtime(2s, 3s));
        events.ScheduleEvent(EVENT_SHA_CORRUPTION, 8s);

        switch (me->GetEntry())
        {
            case NPC_SPIRIT_OF_ANGER:
                events.ScheduleEvent(EVENT_DEVASTATION, 13s);
                break;
            case NPC_SPIRIT_OF_HATRED_A:
            case NPC_SPIRIT_OF_HATRED_H:
                DoCast(me, me->GetEntry() == NPC_SPIRIT_OF_HATRED_H ? SPELL_GARROSH_TRANSFORM : SPELL_VARIAN_TRANSFORM);
                events.ScheduleEvent(EVENT_WHIRLWIND, 13s);
                events.ScheduleEvent(EVENT_DEVASTATION, 23s);
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* summoner = ObjectAccessor::GetCreature(*me, summonerGUID))
            summoner->AI()->DoAction(me->GetEntry());
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_REMOVE_DEVASTATION)
            me->RemoveChanneledCast(victimGUID);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (caster->ToPlayer() && (HasAnyControlEffects() || spell->HasEffect(SPELL_EFFECT_KNOCK_BACK)))
            me->RemoveChanneledCast(victimGUID);
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
                case EVENT_HATED_BLAST:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_HATED_BLAST, false);

                    events.ScheduleEvent(EVENT_HATED_BLAST, randtime(8s, 12s));
                    break;
                case EVENT_SHA_CORRUPTION:
                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_SHA_CORRUPTION, false);

                    events.ScheduleEvent(EVENT_SHA_CORRUPTION, 16s);
                    break;
                case EVENT_DEVASTATION:
                    if (Unit* vict = me->GetVictim())
                    {
                        victimGUID = vict->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(vict), SPELL_DEVASTATION);
                    }

                    events.ScheduleEvent(EVENT_DEVASTATION, randtime(25s, 30s));
                    break;
                case EVENT_WHIRLWIND:
                    DoCast(me, SPELL_WHIRWIND_OF_ANGER);
                    events.ScheduleEvent(EVENT_WHIRLWIND, 25s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    private:
        bool HasAnyControlEffects()
        {
            for (auto cList : controlMechanicType)
                if (me->HasAuraType(cList))
                    return true;

            return false;
        }
};

struct npc_varatus_the_conqueror : public customCreatureAI
{
    npc_varatus_the_conqueror(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_INEVITABILITY, randtime(1s + 500ms, 2s + 500ms));
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
            ExecuteTargetEvent(SPELL_INEVITABILITY, (4 * IN_MILLISECONDS, 4.5 * IN_MILLISECONDS), EVENT_INEVITABILITY, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class spell_summ_lorewalker_cho_bashon : public AuraScript
{
    PrepareAuraScript(spell_summ_lorewalker_cho_bashon);

    void Remove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
            pCaster->SetPhaseMask(1, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
            pCaster->SetPhaseMask(3, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_summ_lorewalker_cho_bashon::Remove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_summ_lorewalker_cho_bashon::Apply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_celestial_experience_whirlwind_of_anger : public AuraScript
{
    PrepareAuraScript(spell_celestial_experience_whirlwind_of_anger);

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* pCaster = GetCaster()->ToCreature())
            pCaster->ClearUnitState(UNIT_STATE_CASTING);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_celestial_experience_whirlwind_of_anger::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_celestial_experience_devastation : public SpellScript
{
    PrepareSpellScript(spell_celestial_experience_devastation);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_REMOVE_DEVASTATION);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_celestial_experience_devastation::HandleAfterCast);
    }
};

class scene_memory_wine : public SceneScript
{
    public:
        scene_memory_wine() : SceneScript("scene_memory_wine") { }

        void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            CreditScene(player);
        }

        void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            CreditScene(player);
        }

        void CreditScene(Player* player)
        {
            if (player->GetQuestStatus(QUEST_MEMORY_WINE_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_MEMORY_WINE_H) == QUEST_STATUS_INCOMPLETE)
                player->KilledMonsterCredit(NPC_SHAN_KIEN);
        }
};

struct npc_kota_kon : public VehicleAI
{
    npc_kota_kon(Creature* creature) : VehicleAI(creature)
    {
        guid = ObjectGuid::Empty;
    }

    ObjectGuid guid;

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        guid = player->GetGUID();
        player->CLOSE_GOSSIP_MENU();
        me->CastSpell(player, SPELL_THE_BURLAP_GRIND_BEGIN_RIDE);
        return true;
    }

    void JustSummoned(Creature* summon) override
    {
        summon->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        summon->CastSpell(summon, SPELL_HEAVY_HANDED, true);
        summon->SetPhaseMask(2, true);

        if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
        {
            player->SetPhaseMask(2, true);
            player->EnterVehicle(summon, 0);
        }
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (!apply)
        {
            me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
            who->SetPhaseMask(1, true);
        }
    }
};

struct npc_korda_torros : public ScriptedAI
{
    npc_korda_torros(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_KORDA_TORROS_AGGRO);

        events.ScheduleEvent(EVENT_BELLOWING_RAGE, 20s);
        events.ScheduleEvent(EVENT_HOOF_STOMP, 10s);
        events.ScheduleEvent(EVENT_RUSHING_CHARGE, 10s);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BELLOWING_RAGE:
                    DoCastVictim(SPELL_BELLOWING_RAGE);
                    events.ScheduleEvent(EVENT_BELLOWING_RAGE, 30s);
                    break;
                case EVENT_HOOF_STOMP:
                    DoCastVictim(SPELL_HOOF_STOMP);
                    events.ScheduleEvent(EVENT_HOOF_STOMP, 15s);
                    break;
                case EVENT_RUSHING_CHARGE:
                    DoCastVictim(SPELL_RUSHING_CHARGE);
                    events.ScheduleEvent(EVENT_RUSHING_CHARGE, 10s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_borginn_darkfist : public ScriptedAI
{
    npc_borginn_darkfist(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_BORGINN_DARKFIST_AGGRO);

        events.ScheduleEvent(EVENT_SHADOWBOLTS, 50s);
        events.ScheduleEvent(EVENT_VOIDCLOUD, 15s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHADOWBOLTS:
                    DoCastVictim(SPELL_SHADOWBOLTS);
                    events.ScheduleEvent(EVENT_SHADOWBOLTS, 70s);
                    break;
                case EVENT_VOIDCLOUD:
                    DoCastVictim(SPELL_VOIDCLOUD);
                    events.ScheduleEvent(EVENT_VOIDCLOUD, 35s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_scritch : public ScriptedAI
{
    npc_scritch(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_SCRITCH_AGGRO);

        events.ScheduleEvent(EVENT_GOING_BANANAS, 12s);
        events.ScheduleEvent(EVENT_BANANA_RANG, 8s);
        events.ScheduleEvent(EVENT_TOSS_FILTH, 15s);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_GOING_BANANAS:
                    DoCastVictim(SPELL_GOING_BANANAS);
                    events.ScheduleEvent(EVENT_GOING_BANANAS, 10s);
                    break;
                case EVENT_BANANA_RANG:
                    DoCastVictim(SPELL_BANANA_RANG);
                    events.ScheduleEvent(EVENT_BANANA_RANG, 20s);
                    break;
                case EVENT_TOSS_FILTH:
                    DoCastVictim(SPELL_TOSS_FILTH);
                    events.ScheduleEvent(EVENT_TOSS_FILTH, 15s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_the_yowler : public ScriptedAI
{
    npc_the_yowler(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_YOWLER_AGGRO);

        events.ScheduleEvent(EVENT_BANANA_RANG, 8s);
        events.ScheduleEvent(EVENT_GOING_BANANAS, 12s);
        events.ScheduleEvent(EVENT_TOSS_FILTH, 15s);
    };

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BANANA_RANG:
                    DoCastVictim(SPELL_BANANA_RANG);
                    events.ScheduleEvent(EVENT_BANANA_RANG, 10s);
                    break;
                case EVENT_GOING_BANANAS:
                    DoCastVictim(SPELL_GOING_BANANAS);
                    events.ScheduleEvent(EVENT_GOING_BANANAS, 20s);
                    break;
                case EVENT_TOSS_FILTH:
                    DoCastVictim(SPELL_TOSS_FILTH);
                    events.ScheduleEvent(EVENT_TOSS_FILTH, 15s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_kun_lai_grandpa_grumplefloot : public ScriptedAI
{
    npc_kun_lai_grandpa_grumplefloot(Creature* creature) : ScriptedAI(creature) { }

    void ReceiveEmote(Player* player, uint32 uiTextEmote) override
    {
        if (uiTextEmote == TEXT_EMOTE_DANCE && !player->HasItemCount(ITEM_KNOCKOFF_GRUMPLEFLOOT, 1, true) && player->GetQuestStatus(QUEST_A_KNOCKOFF_GRUMPLEFLOOT) == QUEST_STATUS_NONE)
            player->AddItem(ITEM_KNOCKOFF_GRUMPLEFLOOT, 1);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

class spell_kunlai_release_old_poot_poot : public SpellScript
{
    PrepareSpellScript(spell_kunlai_release_old_poot_poot);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Player* caster = GetCaster()->ToPlayer())
            if (!caster->HasItemCount(ITEM_YETI_SHACKLE_KEY, 1))
                PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_kunlai_release_old_poot_poot::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
        OnEffectHitTarget += SpellEffectFn(spell_kunlai_release_old_poot_poot::HandleEffectHitTarget, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_kunlai_release_old_poot_poot::HandleEffectHitTarget, EFFECT_2, SPELL_EFFECT_KILL_CREDIT);
    }
};

struct npc_kunlai_old_poof_poof : public ScriptedAI
{
    npc_kunlai_old_poof_poof(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        // @Todo:: Launch scripts with VEHICLE and move to grumle base (now just leave from cave)
        me->GetMotionMaster()->MoveJump(pootPootLeavePos[0], 10.0f, 15.0f, EVENT_JUMP);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->StopMoving();

            scheduler.Schedule(Milliseconds(1000), [this](TaskContext context)
            {
                me->GetMotionMaster()->MovePoint(0, pootPootLeavePos[1]);
                me->DespawnOrUnsummon(me->GetSplineDuration());
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

struct hozen_grind_baseAI : public customCreatureAI
{
    hozen_grind_baseAI(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustDied(Unit* killer) override
    {
        if (killer && killer->ToPlayer() && roll_chance_i(10) && !killer->ToPlayer()->HasItemCount(ITEM_HOZEN_FUR_FUSE, 1) && killer->ToPlayer()->GetQuestStatus(QUEST_BOOM_BOOMS_FUSE) == QUEST_STATUS_NONE)
            killer->ToPlayer()->AddItem(ITEM_HOZEN_FUR_FUSE, 1);
    }
};

struct npc_ooking_shaman : public hozen_grind_baseAI
{
    npc_ooking_shaman(Creature* creature) : hozen_grind_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 1s);
        events.ScheduleEvent(EVENT_EARTH_SHOCK, 6s);
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
            ExecuteTargetEvent(SPELL_LIGHTNING_BOLT, 1.7 * IN_MILLISECONDS, EVENT_LIGHTNING_BOLT, eventId);
            ExecuteTargetEvent(SPELL_EARTH_SHOCK, (8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_EARTH_SHOCK, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_silverback_piker : public hozen_grind_baseAI
{
    npc_silverback_piker(Creature* creature) : hozen_grind_baseAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_POKE_YOU, 2s + 500ms);
        events.ScheduleEvent(EVENT_SLING_DERK, 4s);
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
            ExecuteTargetEvent(SPELL_SLING_DERK, 11 * IN_MILLISECONDS, EVENT_SLING_DERK, eventId);
            ExecuteTargetEvent(SPELL_POKE_YOU, (8 * IN_MILLISECONDS, 9 * IN_MILLISECONDS), EVENT_POKE_YOU, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

struct npc_broketooth_leaper : public hozen_grind_baseAI
{
    npc_broketooth_leaper(Creature* creature) : hozen_grind_baseAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

void AddSC_kun_lai_summit()
{
    new creature_script<npc_nessos_the_oracle>("npc_nessos_the_oracle");
    new creature_script<npc_ski_thik>("npc_ski_thik");
    new creature_script<npc_mogujia_soul_caller>("npc_mogujia_soul_caller");
    new creature_script<npc_quilen_stonemaw>("npc_quilen_stonemaw");
    new creature_script<npc_zai_the_outcast>("npc_zai_the_outcast");
    new creature_script<npc_terracotta_gladiator>("npc_terracotta_gladiator");
    new creature_script<npc_terracotta_defender>("npc_terracotta_defender");
    new creature_script<npc_upland_raven>("npc_upland_raven");
    new creature_script<npc_zandalari_warrior>("npc_zandalari_warrior");
    new creature_script<npc_hungry_yeti>("npc_hungry_yeti");
    new creature_script<npc_mischievous_snow_sprite>("npc_mischievous_snow_sprite");
    new creature_script<npc_tomb_guardian>("npc_tomb_guardian");
    // Quest scripts
    new npc_waterspeaker_gorai();
    new creature_script<npc_ordo_overseer>("npc_ordo_overseer");
    new go_yaungol_banner();
    new creature_script<npc_explosives_barrel>("npc_explosives_barrel");
    new npc_inkgill_dissenter();
    new npc_shado_pan_sentinel();
    new npc_lorewalker_cho_bashon();
    new creature_script<npc_lorewalker_cho_bashon_summoned>("npc_lorewalker_cho_bashon_summoned");
    new creature_script<npc_relcaimer_zuan_pets>("npc_relcaimer_zuan_pets");
    new npc_master_hight();
    new creature_script<npc_initiate_chuang_summoned>("npc_initiate_chuang_summoned");
    new npc_xuen_celestial_experience();
    new creature_script<celestial_experience_sha>("celestial_experience_sha");
    new creature_script<npc_varatus_the_conqueror>("npc_varatus_the_conqueror");
    new aura_script<spell_summ_lorewalker_cho_bashon>("spell_summ_lorewalker_cho_bashon");
    new aura_script<spell_celestial_experience_whirlwind_of_anger>("spell_celestial_experience_whirlwind_of_anger");
    new spell_script<spell_celestial_experience_devastation>("spell_celestial_experience_devastation");
    new scene_memory_wine();
    new creature_script<npc_kota_kon>("npc_kota_kon");
    new creature_script<npc_korda_torros>("npc_korda_torros");
    new creature_script<npc_borginn_darkfist>("npc_borginn_darkfist");
    new creature_script<npc_scritch>("npc_scritch");
    new creature_script<npc_the_yowler>("npc_the_yowler");
    new creature_script<npc_kun_lai_grandpa_grumplefloot>("npc_kun_lai_grandpa_grumplefloot");
    new spell_script<spell_kunlai_release_old_poot_poot>("spell_kunlai_release_old_poot_poot");
    new creature_script<npc_kunlai_old_poof_poof>("npc_kunlai_old_poof_poof");
    new creature_script<npc_ooking_shaman>("npc_ooking_shaman");
    new creature_script<npc_silverback_piker>("npc_silverback_piker");
    new creature_script<npc_broketooth_leaper>("npc_broketooth_leaper");
}
