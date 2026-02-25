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

enum Spells
{                                   
    // Kilruk                        
    SPELL_MUTILATE                   = 143941,
    SPELL_DEATH_FROM_ABOVE           = 142263, // summon 71309
    SPELL_REAVE_EFF                  = 142232,
    SPELL_EXPOSED_VEINS              = 142931,
    SPELL_GOUGE                      = 143939,
    SPELL_RAZOR_SHARP_BLADES         = 142922, // trigger 142931
    SPELL_REAVE_JUMP                 = 148681,
    SPELL_DEATH_FROM_ABOVE_MARK      = 144126,
    SPELL_DEATH_FROM_ABOVE_JUMP_EFF  = 142265,
    SPELL_DEATH_FROM_ABOVE_SUMM      = 142263,
    SPELL_REAVE                      = 148676,
    SPELL_REAVE_HEROIC_EFF           = 142228,
    SPELL_REAVE_SELECTOR             = 148677,

    // Xaril
    SPELL_TOXIC_INJECTION            = 142528,
    SPELL_CATALYST_BLUE              = 142725,
    SPELL_CATALYST_RED               = 142726,
    SPELL_CATALYST_YELLOW            = 142727,
    SPELL_TOXIN_RED                  = 142533,
    SPELL_TOXIN_BLUE                 = 142532,
    SPELL_TOXIN_YELLOW               = 142534,
    SPELL_BLOODY_EXPLOSION           = 142317, // on 10 charges of 142315
    SPELL_REACTION_RED               = 142736,
    SPELL_REACTION_YELLOW            = 142737,
    SPELL_REACTION_BLUE              = 142735,
    SPELL_REACTION_YELLOW_EFF        = 142797,
    //Heroic
    SPELL_CATALYST_ORANGE            = 142728,
    SPELL_CATALYST_PURPLE            = 142729,
    SPELL_CATALYST_GREEN             = 142730,
    SPELL_CATALYST_WHITE             = 142731,
    SPELL_TOXIN_ORANGE               = 142547,
    SPELL_TOXIN_PURPLE               = 142548,
    SPELL_TOXIN_GREEN                = 142549,
    SPELL_TOXIN_WHITE                = 142550,
    SPELL_REACTION_ORANGE            = 142738,
    SPELL_REACTION_PURPLE            = 142739,
    SPELL_REACTION_GREEN             = 142740,
    SPELL_REACTION_WHITE             = 142741,
    SPELL_TENDERIZING_STRIKES        = 142929,
    SPELL_CAUSTIC_BLOOD              = 142315,
    SPELL_CANNED_HEAT_LAUNCH         = 143570,
    SPELL_CANNED_HEAT                = 143572,
    SPELL_CANNED_HEAT_AT             = 143575,
    SPELL_CANNED_HEAT_EFF            = 143576,
    SPELL_EERIE_FOG_SUMM             = 142813,
    SPELL_EERIE_FOG_AT               = 142812,
    SPELL_EERIE_FOG_EFF              = 142945,
    SPELL_CHILED_TO_BONE             = 144216,
    SPELL_EXPLOSIVE_RING_EFF         = 142803,
    SPELL_EXPLOSIVE_RING_VISUAL_25   = 144191,
    SPELL_EXPLOSIVE_RING_VISUAL_20   = 144192,
    SPELL_EXPLOSIVE_RING_VISUAL_15   = 144193,
    SPELL_EXPLOSIVE_RING_VISUAL_30   = 144195,
    SPELL_EXPLOSIVE_RING_AT_25       = 142798,
    SPELL_EXPLOSIVE_RING_AT_20       = 142799,
    SPELL_EXPLOSIVE_RING_AT_15       = 142800,
    SPELL_EXPLOSIVE_RING_AT_30       = 142802,

    // Kaz`tik
    SPELL_SONIC_PROJECTION           = 143765,
    SPELL_SONIC_PROJECTION_EFF       = 143768,
    SPELL_SUMMON_HUNGRY_KUNCHLONG    = 146891,
    SPELL_MESMERIZE                  = 142668,
    SPELL_THICK_SHELL                = 142667,
    SPELL_HUNGRY                     = 142630,
    SPELL_DEVOUR                     = 142649,
    SPELL_DEVOUR_P                   = 142636, // aura for player
    SPELL_DEVOUR_UNK                 = 142705,
    SPELL_MOLT                       = 142651, // if it`ll gain 100 energy
    SPELL_SWIPE                      = 142655,
    SPELL_MESMERIZE_AURA             = 142671,
                                     
    // Korven
    SPELL_VICIOUS_ASSAULT            = 143977,
    SPELL_VICIOUS_ASSAULT_AURA       = 143979,
    SPELL_VICIOUS_ASSAULT_PROC_1     = 143980,
    SPELL_VICIOUS_ASSAULT_PROC_2     = 143981,
    SPELL_VICIOUS_ASSAULT_PROC_3     = 143982,
    SPELL_VICIOUS_ASSAULT_PROC_4     = 143984,
    SPELL_VICIOUS_ASSAULT_PROC_5     = 143985,
    SPELL_SHIELD_BASH                = 143974,
    SPELL_ENCASE_IN_AMBER            = 142564, // also summ 71407
    SPELL_AMBER_REGENERATION         = 144119,
    SPELL_AMBER_REGENERATION_EFF     = 142576,
    SPELL_AMBER_BARRIER              = 144120,

    // Iyyokuk
    SPELL_DIMINISH                   = 143666,
    SPELL_INSANE_CALC_FIERY_EDGE     = 142416,
    SPELL_BLUE_BOMB                  = 143617,
    SPELL_BLUE_DRUM                  = 143612,
    SPELL_BLUE_MANTID                = 143622,
    SPELL_BLUE_STAFF                 = 143629,
    SPELL_BLUE_SWORD                 = 143607,
    SPELL_GREEN_BOMB                 = 143618,
    SPELL_GREEN_DRUM                 = 143613,
    SPELL_GREEN_MANTID               = 143623,
    SPELL_GREEN_STAFF                = 143630,
    SPELL_GREEN_SWORD                = 143608,
    SPELL_RED_BOMB                   = 143615,
    SPELL_RED_DRUM                   = 143610,
    SPELL_RED_MANTID                 = 143620,
    SPELL_RED_STAFF                  = 143627,
    SPELL_RED_SWORD                  = 143605,
    SPELL_YELLOW_BOMB                = 143619,
    SPELL_YELLOW_DRUM                = 143614,
    SPELL_YELLOW_MANTID              = 143624,
    SPELL_YELLOW_STAFF               = 143631,
    SPELL_YELLOW_SWORD               = 143609,
    SPELL_FIERY_EDGE_BEAM            = 142811,
    SPELL_FIERY_EDGE_EFF             = 142809,
    SPELL_FIERY_EDGE_AURA            = 142808,
    SPELL_CALCULATED                 = 144095,

    // Karoz
    SPELL_THROW_AMBER                = 143966,
    SPELL_HURL_AMBER_THROW           = 143759,
    SPELL_HURL_AMBER                 = 143733,
    SPELL_HURL_AMBER_EFF             = 143735,
    SPELL_CREATE_AMBER_BOMB          = 143696,
    SPELL_STORE_KINETIC_ENERGY       = 143709,
    SPELL_FLASH                      = 143700,
    SPELL_FLASH_CHARGE               = 143704,
    SPELL_FLASH_EFF                  = 148594,
    SPELL_WHIRLING                   = 143701,
    SPELL_WHIRLING_AT_EFF            = 143702,
    SPELL_PICK_CHARGE_TARGET         = 143710,
    SPELL_CAUSTIC_AMBER              = 143740,
    SPELL_CAUSTIC_AMBER_EFF          = 143735,
    VISUAL_FLASH_ID                  = 32229, // on path

    // Skir
    SPELL_BLOODLETTING_PARAGON       = 143280,
    SPELL_BLOODLETTING_SUMMON        = 143283,
    SPELL_HEWN                       = 143275,
    SPELL_BLOODLETTING_MISSLE_1      = 143284,
    SPELL_BLOODLETTING_MISSLE_2      = 143287,
    SPELL_BLOODLETTING_MISSLE_3      = 143288,
    SPELL_BLOODLETTING_MISSLE_4      = 143289,
    SPELL_HEWER_OF_FOES              = 143273,
    SPELL_HEW                        = 143274,
    SPELL_BLOOD_INFUSION             = 143294,

    // Rikkal
    SPELL_INJECTION_SUMM             = 143340,
    SPELL_GENETIC_ALTERATION_EFF     = 143278,
    SPELL_GENETIC_ALTERATION         = 143278,
    SPELL_GENETIC_ALTERATION_EFF_2   = 143279,
    SPELL_INJECTION                  = 143339,
    SPELL_MUTATE                     = 143337,
    SPELL_MUTATE_SELECTOR            = 143336,
    SPELL_FEED                       = 143362,
    SPELL_GENETIC_MODIFICATION       = 143355,
    SPELL_INJECTION_SUMMON           = 143383,
    SPELL_INJECTION_SUMM_EFF         = 143379,
    SPELL_INJECTION_INITIALIZE       = 143340,
    SPELL_FAULTY_MUTATION            = 148589,
    SPELL_FAULTY_MUTATION_EFF        = 148587,

    // Hisek
    SPELL_AIM_MARK                   = 144759,
    SPELL_SONIC_RESONANCE            = 144094, // should be cast by player, that got hit on line aim
    SPELL_POINT_BLANK_SHOT           = 143505,
    SPELL_SHOT                       = 142950,
    SPELL_MULTY_SHOT                 = 144839,
    SPELL_AIM                        = 142948,
    SPELL_RAPID_FIRE                 = 143243,
    SPELL_SONIC_PULSE_1              = 143226,
    SPELL_SONIC_PULSE_2              = 143227,
    SPELL_SONIC_PULSE_3              = 143228,
    SPELL_SONIC_PULSE_4              = 143229,
    SPELL_SONIC_PULSE_5              = 143230,
    SPELL_SONIC_PULSE_6              = 143231,
    SPELL_SONIC_PULSE_7              = 143232,
    SPELL_SONIC_PULSE_8              = 143233,
    SPELL_SONIC_PULSE_9              = 143234,
    SPELL_SONIC_PULSE_10             = 143236,
    SPELL_SONIC_PULSE_11             = 143237,
    SPELL_SONIC_PULSE_12             = 143238,
    SPELL_SONIC_PULSE_EFF            = 143240,

    // Player mechanic abilities
    SPELL_REAVE_PLAYER               = 141859, // dd
    SPELL_REAVE_PLAYER_EFF           = 142272,
    SPELL_APOTHECARY_KNOWLEDGE       = 141856, // healer
    SPELL_VOLATILE_POULTICE          = 142598,
    SPELL_VOLATILE_POULTICE_STORAGE  = 142877,
    SPELL_VOLATILE_POULTICE_HEAL_EFF = 142897,
    SPELL_MASTER_OF_PUPPETS          = 141855, // any
    SPELL_MASTER_OF_AMBER            = 141854, // tank
    SPELL_CALCULATOR                 = 141858, // healer
    SPELL_CALCULATED_HEAL            = 144111,
    SPELL_STRONG_LEGS                = 141853, // dd
    SPELL_BLOODLETTING               = 141851, // dd
    SPELL_BLOODLETTING_MISSLE_AT     = 143320,
    SPELL_BLOODLETTING_AT            = 143321,
    SPELL_REGENERATION               = 143322,
    SPELL_GENE_SPLICE                = 141857, // any
    SPELL_SNIPER_SHOT                = 141852, // dd

    SPELL_REAVE_PLAYER_TR            = 142269, // dd
    SPELL_APOTHECARY_KNOWLEDGE_TR    = 142546, // healer
    SPELL_MASTER_OF_PUPPETS_TR       = 142701, // any
    SPELL_MASTER_OF_AMBER_TR         = 142616, // tank
    SPELL_CALCULATOR_TR              = 142513, // healer
    SPELL_STRONG_LEGS_TR             = 143963, // dd
    SPELL_BLOODLETTING_TR            = 143318, // dd
    SPELL_GENE_SPLICE_TR             = 143372, // any
    SPELL_SNIPER_SHOT_TR             = 143216, // dd

    // Misc
    SPELL_SUMMON_FIGHT_STARTER       = 143552, // mantid amber
    SPELL_PING_AURA                  = 144354,
    SPELL_PING_AURA_EFF              = 144353, // evade check ?
    SPELL_READY_TO_FIGHT             = 143542,
    SPELL_COSMETIC_TO_FIGHT          = 143548,
    SPELL_PARAGONS_PURPOSE           = 143483,
    SPELL_PARAGONS_PURPOSE_AURA      = 143482,
    SPELL_BERSERK                    = 146982,
    SPELL_BERSERK_TIMER              = 146983, // 12 minutes
    SPELL_JUMP_TO_CENTER             = 143545,
    SPELL_WIN                        = 148512,
    SPELL_GROW                       = 44833,
    SPELL_NON_REGENERATE_POWER       = 72242,
    SPELL_SUMMON_PARAGON_SKEER       = 141795,
    SPELL_SUMMON_PARAGON_RIKKAL      = 141801,
    SPELL_SUMMON_PARAGON_HISEK       = 141796,
    SPELL_SUMMON_PARAGON_KAROZ       = 141797,
    SPELL_SUMMON_PARAGON_KORVEN      = 141798,
    SPELL_SUMMON_PARAGON_IYOKKUK     = 141802,
    SPELL_SUMMON_PARAGON_XARIL       = 141800,
    SPELL_SUMMON_PARAGON_KAZTIK      = 141799,
    SPELL_SUMMON_PARAGON_KILLRUK     = 141803,
    SPELL_SUMMON_PARAGONS            = 141804,
    SPELL_PERMANENT_FEIGN_DEATH      = 70628,
    SPELL_CLICK_ME                   = 144833,
};

enum Events
{
    EVENT_INIT = 1,
    EVENT_CHECK_WIPE,
    EVENT_MUTILATE,
    EVENT_DEATH_FROM_ABOVE,
    EVENT_TOXIN_REACT,
    EVENT_CAUSTIC_BLOOD,
    EVENT_TENDERIZING_STRIKES,
    EVENT_TOXIC_INJECTION,
    EVENT_SONIC_PROJECTION,
    EVENT_KUNCHONG,
    EVENT_MESMERIZE,
    EVENT_VICIOUS_ASSAULT,
    EVENT_SHIELD_BASH,
    EVENT_AMBER_REGENERATION,
    EVENT_VICIOUS_ASSAULT_REMOVE,
    EVENT_DIMINISH,
    EVENT_INSANE,
    EVENT_CALCULATE,
    EVENT_STORE_KINETIC_ENERGY,
    EVENT_CHECK,
    EVENT_PREPARE_TO_JUMP,
    EVENT_JUMP_TO_AMBER,
    EVENT_JUMP_BACK,
    EVENT_BLOODLETTING,
    EVENT_INJECTION,
    EVENT_MUTATE,
    EVENT_MULTY_SHOT,
    EVENT_AIM,
    EVENT_DEVOUR,
    EVENT_SWIPE,
    EVENT_REMOVE_ADDITIONAL_ABILITY,
    EVENT_APPLY_ADITIONAL_ABILITY,
    EVENT_JUMP_TO_COMBAT,
    EVENT_CHECK_EVADE,
    EVENT_FLASH,
    EVENT_LAST_FLASH,
    EVENT_PARAGON_PURPOSE,
    EVENT_RAPID_FIRE,
    EVENT_REAVE,
    EVENT_FOG_MOVE,
};

enum Actions
{
    ACTION_KLAXXI_COMBAT = 1,
    ACTION_NEXT_PARAGON_MEMBER,
    ACTION_VICIOUS_ASSAULT,
    ACTION_MUTILATE,
    ACTION_REMOVE_AIM,
};

enum Yells
{
    TALK_PARAGON_INTRO,
    TALK_PARAGON_LANDING,
    TALK_PARAGON_LANDING_ADDITIONAL,
    TALK_PARAGON_DEATH,
};

const std::vector<uint32> m_ToxinList
{
    SPELL_TOXIN_RED,
    SPELL_TOXIN_BLUE,
    SPELL_TOXIN_YELLOW,
};

const std::vector<uint32> m_CalculationList
{
    SPELL_BLUE_BOMB,
    SPELL_BLUE_DRUM,
    SPELL_BLUE_MANTID,
    SPELL_BLUE_STAFF,
    SPELL_BLUE_SWORD,
    SPELL_GREEN_BOMB,
    SPELL_GREEN_DRUM,
    SPELL_GREEN_MANTID,
    SPELL_GREEN_STAFF,
    SPELL_GREEN_SWORD,
    SPELL_RED_BOMB,
    SPELL_RED_DRUM,
    SPELL_RED_MANTID,
    SPELL_RED_STAFF,
    SPELL_RED_SWORD,
    SPELL_YELLOW_BOMB,
    SPELL_YELLOW_DRUM,
    SPELL_YELLOW_MANTID,
    SPELL_YELLOW_STAFF,
    SPELL_YELLOW_SWORD,
};

const std::map<uint32, Position> invParagonSummonsType =
{
    { SPELL_SUMMON_PARAGON_SKEER,   { 1608.894f, -5740.578f, -290.9124f, 2.023022f  } },
    { SPELL_SUMMON_PARAGON_RIKKAL,  { 1624.321f, -5730.018f, -290.9202f, 2.331927f  } },
    { SPELL_SUMMON_PARAGON_HISEK,   { 1519.401f, -5637.149f, -291.0173f, 5.62359f   } },
    { SPELL_SUMMON_PARAGON_KAROZ,   { 1552.964f, -5640.806f, -291.0728f, 5.289442f  } },
    { SPELL_SUMMON_PARAGON_KORVEN,  { 1650.016f, -5732.452f, -291.0029f, 2.539645f  } },
    { SPELL_SUMMON_PARAGON_IYOKKUK, { 1576.089f, -5698.48f,  -279.7763f, 3.551682f  } },
    { SPELL_SUMMON_PARAGON_XARIL,   { 1602.252f, -5765.936f, -290.9713f, 1.814512f  } },
    { SPELL_SUMMON_PARAGON_KAZTIK,  { 1561.88f,  -5608.125f, -291.0153f, 4.969732f  } },
    { SPELL_SUMMON_PARAGON_KILLRUK, { 1581.194f, -5660.761f, -286.9361f, 3.193093f  } },
};

const std::map<uint32, uint32> invParagonDeathYells =
{
    { NPC_KILRUK_THE_WIND_REAVER,  2 },
    { NPC_XARIL_THE_POISONED_MIND, 3 },
    { NPC_KAZTIK_THE_MANIPULATOR,  2 },
    { NPC_KORVEN_THE_PRIME,        2 },
    { NPC_IYYOKUK_THE_LUCID,       3 },
    { NPC_KAROZ_THE_LOCUST,        2 },
    { NPC_SKEER_THE_BLOODSEEKER,   1 },
    { NPC_RIKKAL_THE_DISSECTOR,    2 },
    { NPC_HISEK_THE_SWARMKEEPER,   1 },
};

const std::map<uint32, float> invSonicType =
{
    { 1,   (2 * M_PI) / 6 },
    { 2,   (0 * M_PI) / 6 },
    { 3,   (5 * M_PI) / 6 },
    { 4,   (8 * M_PI) / 6 },
    { 5,   (1 * M_PI) / 6 },
    { 6,  (11 * M_PI) / 6 },
    { 7,   (3 * M_PI) / 6 },
    { 8,   (6 * M_PI) / 6 },
    { 9,   (4 * M_PI) / 6 },
    { 10,  (7 * M_PI) / 6 },
    { 11,  (9 * M_PI) / 6 },
    { 12, (10 * M_PI) / 6 },
};

const std::vector<uint32> paragonStrengthList =
{
    SPELL_REAVE_PLAYER_TR,
    SPELL_APOTHECARY_KNOWLEDGE_TR,
    SPELL_MASTER_OF_PUPPETS_TR,
    SPELL_MASTER_OF_AMBER_TR,
    SPELL_CALCULATOR_TR,
    SPELL_STRONG_LEGS_TR,
    SPELL_BLOODLETTING_TR,
    SPELL_GENE_SPLICE_TR,
    SPELL_SNIPER_SHOT_TR,
};

// Paragon of the klaxxi - Helper 71592
class boss_paragon_of_the_klaxxi : public CreatureScript
{
    public:
        boss_paragon_of_the_klaxxi() : CreatureScript("boss_paragon_of_the_klaxxi") { }

        struct boss_paragon_of_the_klaxxiAI : public BossAI
        {
            boss_paragon_of_the_klaxxiAI(Creature* creature) : BossAI(creature, DATA_PARAGONS_OF_THE_KLAXXI) 
            {
                me->setActive(true);
                hasIntro = false;
            }

            EventMap evadeEvents;
            std::list<uint32> paragonSequence;
            ObjectGuid c_nextUID;
            uint32 defeatCount;
            bool hasEvade;
            bool hasIntro;

            void Reset() override
            {
                _Reset();
                events.Reset();
                summons.DespawnAll();
                scheduler.CancelAll();
                paragonSequence.clear();
                evadeEvents.Reset();
                me->RemoveAurasDueToSpell(SPELL_BERSERK_TIMER);
                c_nextUID = ObjectGuid::Empty;
                defeatCount = 0;
                hasEvade = false;

                // simulated delay (should solve issue with missing killruk at first initialize)
                scheduler
                    .Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    if (instance && instance->GetData(DATA_PARAGON_PRE_EVENT) != DONE)
                    {
                        context.Repeat(Seconds(1));
                        return;
                    }

                    DoCast(me, SPELL_SUMMON_PARAGONS);
                });

                if (me->IsAlive() && me->GetPositionZ() < -1000.0f)
                    EnterEvadeMode();

                paragonSequence = { NPC_RIKKAL_THE_DISSECTOR, NPC_HISEK_THE_SWARMKEEPER, NPC_SKEER_THE_BLOODSEEKER, NPC_KAROZ_THE_LOCUST, NPC_KORVEN_THE_PRIME, NPC_IYYOKUK_THE_LUCID, NPC_XARIL_THE_POISONED_MIND, NPC_KAZTIK_THE_MANIPULATOR, NPC_KILRUK_THE_WIND_REAVER };
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                evadeEvents.ScheduleEvent(EVENT_CHECK_EVADE, 1 * IN_MILLISECONDS);
                
                if (IsHeroic())
                    evadeEvents.ScheduleEvent(EVENT_PARAGON_PURPOSE, 35 * IN_MILLISECONDS);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                    {
                        if (hasIntro)
                            break;

                        hasIntro = true;

                        if (Creature* killruk = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KILRUK_THE_WIND_REAVER) : ObjectGuid::Empty))
                            killruk->AI()->Talk(TALK_PARAGON_INTRO);

                        uint32 delay = 4800;
                        scheduler
                            .Schedule(Milliseconds(delay), [this](TaskContext context)
                        {
                            if (Creature* rikkal = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_RIKKAL_THE_DISSECTOR) : ObjectGuid::Empty))
                                rikkal->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6900), [this](TaskContext context)
                        {
                            if (Creature* iyokkuk = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_IYYOKUK_THE_LUCID) : ObjectGuid::Empty))
                                iyokkuk->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
                        {
                            if (Creature* xaril = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_XARIL_THE_POISONED_MIND) : ObjectGuid::Empty))
                                xaril->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 7400), [this](TaskContext context)
                        {
                            if (Creature* kaztik = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KAZTIK_THE_MANIPULATOR) : ObjectGuid::Empty))
                                kaztik->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6000), [this](TaskContext context)
                        {
                            if (Creature* korven = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KORVEN_THE_PRIME) : ObjectGuid::Empty))
                                korven->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 10700), [this](TaskContext context)
                        {
                            if (Creature* karoz = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_KAROZ_THE_LOCUST) : ObjectGuid::Empty))
                                karoz->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 7160), [this](TaskContext context)
                        {
                            if (Creature* skeer = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SKEER_THE_BLOODSEEKER) : ObjectGuid::Empty))
                                skeer->AI()->Talk(TALK_PARAGON_INTRO);
                        });

                        scheduler
                            .Schedule(Milliseconds(delay += 6800), [this](TaskContext context)
                        {
                            if (Creature* hisek = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_HISEK_THE_SWARMKEEPER) : ObjectGuid::Empty))
                                hisek->AI()->Talk(TALK_PARAGON_INTRO);
                        });
                        break;
                    }
                    case ACTION_KLAXXI_COMBAT:
                    {
                        if (paragonSequence.size() < 4)
                        {
                            TC_LOG_ERROR("shitlog", "boss_paragon_of_the_klaxxi paragonSequence.size() %u, boss state %u\n", uint32(paragonSequence.size()), instance ? instance->GetBossState(DATA_PARAGONS_OF_THE_KLAXXI) : 0);
                            return;
                        }

                        me->SetInCombatWithZone();

                        // Send combat for first combined setup
                        for (uint8 i = 0; i < 3; i++)
                        {
                            uint32 paragonMember = *paragonSequence.begin();

                            if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(paragonMember) : ObjectGuid::Empty))
                                klaxxi->AI()->DoAction(ACTION_KLAXXI_COMBAT);

                            paragonSequence.pop_front();
                        }

                        // type new klaxxi with ready aura
                        uint32 nextParagonMember = *paragonSequence.begin();
                        if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(nextParagonMember) : ObjectGuid::Empty))
                        {
                            me->CastSpell(klaxxi, SPELL_READY_TO_FIGHT, true);
                            c_nextUID = klaxxi->GetGUID();
                        }

                        DoCast(me, SPELL_BERSERK_TIMER);
                        break;
                    }
                    case ACTION_NEXT_PARAGON_MEMBER:
                    {
                        if (++defeatCount > 8)
                        {
                            JustDied(me);
                            return;
                        }

                        // Buff alive klaxxi if inCombat but not for heroic mode
                        for (auto&& paragonMemberEntry : eParagonsOfTheKlaaxi)
                            if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(paragonMemberEntry) : ObjectGuid::Empty))
                                if (klaxxi->IsInCombat() && !klaxxi->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                                    klaxxi->CastSpell(klaxxi, SPELL_PARAGONS_PURPOSE, true);

                        // Send combat for klaxxi with ready aura
                        if (Creature* cNextParagon = ObjectAccessor::GetCreature(*me, c_nextUID))
                            cNextParagon->AI()->DoAction(ACTION_KLAXXI_COMBAT);

                        if (!paragonSequence.empty())
                        {
                            paragonSequence.pop_front();

                            // type new klaxxi with ready aura
                            uint32 nextParagonMember = *paragonSequence.begin();
                            if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(nextParagonMember) : ObjectGuid::Empty))
                            {
                                me->CastSpell(klaxxi, SPELL_READY_TO_FIGHT, true);
                                c_nextUID = klaxxi->GetGUID();
                            }
                        }
                        break;
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                if (!isParagonMember(summon->GetEntry()))
                    summons.Summon(summon);
            }

            void EnterEvadeMode() override
            {
                if (hasEvade)
                    return;

                hasEvade = true;

                _EnterEvadeMode();
                summons.DespawnAll();

                for (auto itr : eParagonsOfTheKlaaxi)
                    if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(itr) : ObjectGuid::Empty))
                        klaxxi->AI()->EnterEvadeMode();

                if (instance)
                {
                    instance->SetBossState(DATA_PARAGONS_OF_THE_KLAXXI, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENETIC_ALTERATION_EFF_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MUTATE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_RED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_BLUE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_YELLOW);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPOSED_VEINS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GOUGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TENDERIZING_STRIKES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HEWN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENETIC_ALTERATION);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INJECTION);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REAVE_PLAYER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_APOTHECARY_KNOWLEDGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_PUPPETS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_AMBER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CALCULATOR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STRONG_LEGS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODLETTING);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENE_SPLICE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SNIPER_SHOT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REAVE_PLAYER_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_APOTHECARY_KNOWLEDGE_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_PUPPETS_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_AMBER_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CALCULATOR_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STRONG_LEGS_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODLETTING_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENE_SPLICE_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SNIPER_SHOT_TR);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                if (Creature* mantidAmber = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_AMBER) : ObjectGuid::Empty))
                    mantidAmber->AI()->EnterEvadeMode();

                _DespawnAtEvade();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENETIC_ALTERATION_EFF_2);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MUTATE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_RED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_BLUE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_YELLOW);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPOSED_VEINS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GOUGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TENDERIZING_STRIKES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HEWN);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENETIC_ALTERATION);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_INJECTION);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REAVE_PLAYER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_APOTHECARY_KNOWLEDGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_PUPPETS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_AMBER);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CALCULATOR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STRONG_LEGS);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODLETTING);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENE_SPLICE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SNIPER_SHOT);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REAVE_PLAYER_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_APOTHECARY_KNOWLEDGE_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_PUPPETS_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASTER_OF_AMBER_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CALCULATOR_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STRONG_LEGS_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOODLETTING_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENE_SPLICE_TR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SNIPER_SHOT_TR);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                evadeEvents.Reset();
                HandleFinishEncounter();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);
                evadeEvents.Update(diff);

                while (uint32 eventId = evadeEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_EVADE:
                            if (instance && instance->IsWipe(90.0f, me))
                            {
                                EnterEvadeMode();
                                break;
                            }

                            evadeEvents.ScheduleEvent(EVENT_CHECK_EVADE, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_PARAGON_PURPOSE: // in heroic this buff each 30s
                            for (auto&& paragonMemberEntry : eParagonsOfTheKlaaxi)
                                if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(paragonMemberEntry) : ObjectGuid::Empty))
                                    if (klaxxi->IsInCombat() && !klaxxi->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                                        klaxxi->CastSpell(klaxxi, SPELL_PARAGONS_PURPOSE_AURA, true);

                            evadeEvents.ScheduleEvent(EVENT_PARAGON_PURPOSE, 30 * IN_MILLISECONDS);
                            break;
                    }
                }
            }

            private:
                void HandleFinishEncounter()
                {
                    DoCast(me, SPELL_WIN);
                    me->Kill(me);

                    if (Player* player = me->GetMap()->GetFirstPlayerInInstance())
                        player->RewardPersonalLootAndCurrency(me, NPC_KILRUK_THE_WIND_REAVER);

                    // select any paragon member for allow loot them
                    for (auto&& paragonMemberEntry : eParagonsOfTheKlaaxi)
                    {
                        if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(paragonMemberEntry) : ObjectGuid::Empty))
                        {
                            klaxxi->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            klaxxi->SetFaction(14);
                            klaxxi->RemoveFlag(UNIT_FIELD_FLAGS_2, 69240832);
                            klaxxi->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);

                            klaxxi->RemoveAura(SPELL_PERMANENT_FEIGN_DEATH);

                            if (klaxxi->GetEntry() != NPC_KILRUK_THE_WIND_REAVER)
                            {
                                klaxxi->SetLootRecipient(NULL);
                                me->Kill(klaxxi);
                            }
                            else if (Player* target = me->FindNearestPlayer(200.0f))
                                target->Kill(klaxxi);
                        }
                    }
                }

                bool isParagonMember(uint32 entry)
                {
                    for (auto&& itr : eParagonsOfTheKlaaxi)
                        if (itr == entry)
                            return true;

                    return false;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_paragon_of_the_klaxxiAI(creature);
        }
};

struct soo_paragon_typeAI : public ScriptedAI
{
    soo_paragon_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    EventMap events, nonCombatEvents;
    SummonList summons;
    Position pos;
    uint8 kunchlongCount;
    uint32 prevToxin, paragonStrength, roleType, aimCount, aimTarget, calculateDone;
    ObjectGuid targetGUID;
    ObjectGuid amberGUID;
    ObjectGuid aimTargetGUID;
    ObjectGuid bloodlettingTargetGUID;
    uint32 whirlCount;
    uint32 flashDistance;
    float x, y;
    bool inWhirl;
    bool hasEvade;
    bool hasMeleeProc;
    bool hasMutilateSeq;
    bool deathfromAbove;
    bool hasTriggeredBuff;
    bool hasDeath;
    bool hasCalled;
    bool throwAmberPreparation;

    void InitializeAI() override
    {
        instance = me->GetInstanceScript();

        Reset();

        if (me->GetEntry() == NPC_KAZTIK_THE_MANIPULATOR || me->GetEntry() == NPC_HISEK_THE_SWARMKEEPER)
            SetCombatMovement(false);

        scheduler
            .SetValidator([this] { return me->GetEntry() == NPC_KILRUK_THE_WIND_REAVER || me->GetEntry() == NPC_IYYOKUK_THE_LUCID; })
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            // flight simulate
            me->SetAnimTier(AnimTier::Fly);
            me->OverrideInhabitType(INHABIT_AIR);
            me->UpdateMovementFlags();

            pos = me->GetRandomPoint({ me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), me->GetHomePosition().GetOrientation() }, 20.0f);
            me->GetMotionMaster()->MovePoint(0, pos.GetPositionX(), pos.GetPositionY(), me->GetHomePosition().GetPositionZ());

            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });

        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            if (me->GetEntry() != NPC_KILRUK_THE_WIND_REAVER && me->GetEntry() != NPC_IYYOKUK_THE_LUCID)
                me->SetFacingTo(me->GetAngle(paragonController));

        // First Sequence
        if (me->GetEntry() != NPC_SKEER_THE_BLOODSEEKER && me->GetEntry() != NPC_RIKKAL_THE_DISSECTOR && me->GetEntry() != NPC_HISEK_THE_SWARMKEEPER)
            return;

        DoCast(me, SPELL_READY_TO_FIGHT);
    }

    void Reset() override
    {
        summons.DespawnAll();
        events.Reset();
        nonCombatEvents.Reset();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
        me->SetFaction(14);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        prevToxin       = 0;
        targetGUID = ObjectGuid::Empty;
        paragonStrength = 0;
        roleType        = 0;
        aimCount        = 0;
        calculateDone   = 0;
        kunchlongCount  = 0;
        amberGUID = ObjectGuid::Empty;
        whirlCount      = 0;
        aimTargetGUID = ObjectGuid::Empty;
        flashDistance   = 0;
        bloodlettingTargetGUID = ObjectGuid::Empty;
        inWhirl         = false;
        hasEvade        = false;
        hasMeleeProc    = false;
        hasMutilateSeq  = false;
        deathfromAbove  = false;
        hasTriggeredBuff = false;
        hasDeath         = false;
        hasCalled        = false;
        throwAmberPreparation = false;

        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

        me->SetCorpseDelay(30 * MINUTE);
    }

    void EnterEvadeMode() override
    {
        if (hasEvade)
            return;

        hasEvade = true;

        if (instance)
        {
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            for (auto&& calculation : m_CalculationList)
                instance->DoRemoveAurasDueToSpellOnPlayers(calculation);

            if (Creature* paragonController = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI)))
                paragonController->AI()->EnterEvadeMode();
        }

        me->DespawnOrUnsummon();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_KLAXXI_COMBAT:
            {
                if (hasCalled)
                    break;

                hasCalled = true;
                scheduler.CancelAll();
                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->RemoveAurasDueToSpell(SPELL_READY_TO_FIGHT);

                if (me->GetEntry() != NPC_SKEER_THE_BLOODSEEKER && me->GetEntry() != NPC_HISEK_THE_SWARMKEEPER)
                    Talk(TALK_PARAGON_LANDING);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                uint32 jumpTimer = 5000;

                switch (me->GetEntry())
                {
                    case NPC_HISEK_THE_SWARMKEEPER:
                        jumpTimer += 1000;
                        break;
                    case NPC_SKEER_THE_BLOODSEEKER:
                        jumpTimer += 2000;
                        break;
                }

                nonCombatEvents.ScheduleEvent(EVENT_JUMP_TO_COMBAT, jumpTimer);
                break;
            }
            case ACTION_VICIOUS_ASSAULT:
            {
                if (Unit* target = me->GetVictim())
                {
                    targetGUID = target->GetGUID();
                    me->PrepareChanneledCast(me->GetAngle(target));
                }

                nonCombatEvents.ScheduleEvent(EVENT_APPLY_ADITIONAL_ABILITY, 0.2 * IN_MILLISECONDS);
                nonCombatEvents.ScheduleEvent(EVENT_REMOVE_ADDITIONAL_ABILITY, 7.2 * IN_MILLISECONDS);
                break;
            }
            case ACTION_REMOVE_AIM:
            {
                nonCombatEvents.ScheduleEvent(EVENT_REMOVE_ADDITIONAL_ABILITY, 1 * IN_MILLISECONDS);
                break;
            }
            case ACTION_LAUNCH_AMBER_BOMB:
            {
                nonCombatEvents.ScheduleEvent(5, 3 * IN_MILLISECONDS);
                break;
            }
            case ACTION_REMOVE_REAVE:
            {
                me->RemoveChanneledCast(targetGUID);
                break;
            }
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        switch (pointId)
        {
            case EVENT_JUMP:
                if (me->GetEntry() == NPC_IYYOKUK_THE_LUCID || me->GetEntry() == NPC_XARIL_THE_POISONED_MIND)
                    Talk(TALK_PARAGON_LANDING_ADDITIONAL); // additional comat yell

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->SetInCombatWithZone();
                break;
            case EVENT_JUMP + 3:
                DoCast(me, SPELL_HURL_AMBER_THROW);
                nonCombatEvents.ScheduleEvent(EVENT_REMOVE_ADDITIONAL_ABILITY, 7 * IN_MILLISECONDS);
                break;
            case EVENT_JUMP + 4:
                me->RemoveChanneledCast(targetGUID);
                throwAmberPreparation = false;
                break;
            case EVENT_CHARGE:
                if (me->GetEntry() == NPC_KAROZ_THE_LOCUST && ++whirlCount < 4)
                {
                    nonCombatEvents.ScheduleEvent(EVENT_FLASH, 0.5 * IN_MILLISECONDS); // micro-delay
                    nonCombatEvents.ScheduleEvent(EVENT_APPLY_ADITIONAL_ABILITY, 1.5 * IN_MILLISECONDS);

                    // Visual Animation
                    uint8 count = (uint8)(GetData(TYPE_FLASH_DATA) / 3);

                    for (uint8 i = 0; i < count; i++)
                    {
                        GetPositionWithDistInOrientation(me, 3.0f + i * 3.0f, Position::NormalizeOrientation(me->GetOrientation() - M_PI), x, y);
                        me->SendPlaySpellVisual(VISUAL_FLASH_ID, x, y, me->GetPositionZ(), 1.0f);
                    }
                }
                else
                {
                    nonCombatEvents.ScheduleEvent(EVENT_LAST_FLASH, 0.5 * IN_MILLISECONDS); // micro-delay

                    // Visual Animation
                    uint8 count = (uint8)(GetData(TYPE_FLASH_DATA) / 3);

                    for (uint8 i = 0; i < count; i++)
                    {
                        GetPositionWithDistInOrientation(me, 3.0f + i * 3.0f, Position::NormalizeOrientation(me->GetOrientation() - M_PI), x, y);
                        me->SendPlaySpellVisual(VISUAL_FLASH_ID, x, y, me->GetPositionZ(), 1.0f);
                    }
                }
                break;
        }
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        switch (me->GetEntry())
        {
            case NPC_KILRUK_THE_WIND_REAVER:
                roleType = ROLES_DPS;
                paragonStrength = SPELL_REAVE_PLAYER;
                break;
            case NPC_XARIL_THE_POISONED_MIND:
                roleType = ROLES_HEALER;
                paragonStrength = SPELL_APOTHECARY_KNOWLEDGE;
                break;
            case NPC_KAZTIK_THE_MANIPULATOR:
                roleType = 0;
                paragonStrength = SPELL_MASTER_OF_PUPPETS;
                break;
            case NPC_KORVEN_THE_PRIME:
                roleType = ROLES_TANK;
                paragonStrength = SPELL_MASTER_OF_AMBER;
                break;
            case NPC_IYYOKUK_THE_LUCID:
                roleType = ROLES_HEALER;
                paragonStrength = SPELL_CALCULATOR;
                break;
            case NPC_KAROZ_THE_LOCUST:
                roleType = ROLES_DPS;
                paragonStrength = SPELL_STRONG_LEGS;
                break;
            case NPC_SKEER_THE_BLOODSEEKER:
                roleType = ROLES_DPS;
                paragonStrength = SPELL_BLOODLETTING;
                break;
            case NPC_RIKKAL_THE_DISSECTOR:
                roleType = 0;
                paragonStrength = SPELL_GENE_SPLICE;
                break;
            case NPC_HISEK_THE_SWARMKEEPER:
                roleType = ROLES_DPS;
                paragonStrength = SPELL_SNIPER_SHOT;
                break;
        }

        if (clicker && hasAnyParagonStrength(clicker)) // prevent multy strength by same time
            return;

        if (roleType)
        {
            if (clicker->ToPlayer()->GetRoleForGroup(clicker->ToPlayer()->GetTalentSpecialization()) == roleType && !hasTriggeredBuff)
            {
                hasTriggeredBuff = true;
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->RemoveAurasDueToSpell(SPELL_CLICK_ME);
                clicker->CastSpell(clicker, paragonStrength);
            }
        }
        else if (!hasTriggeredBuff) // for non-role buffs (rikkal, kaztik)
        {
            hasTriggeredBuff = true;
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->RemoveAurasDueToSpell(SPELL_CLICK_ME);
            clicker->CastSpell(clicker, paragonStrength);
        }
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (hasDeath)
            damage = 0;

        if (damage >= me->GetHealth() && !hasDeath)
        {
            hasDeath = true;
            damage = 0;
            events.Reset();
            scheduler.CancelAll();
            nonCombatEvents.Reset();
            me->RemoveAllAuras();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(OBJECT_FIELD_DYNAMIC_FLAGS, 64); // ref to spellclick (but doesn`t work...)
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            me->PrepareChanneledCast(me->GetOrientation());
            me->SetHealth(1);
            me->SetFaction(35); // cuz ref doesn`t work
            me->SetFlag(UNIT_FIELD_FLAGS_2, 69240832);
            DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);
            DoCast(me, SPELL_CLICK_ME);

            Talk(invParagonDeathYells.find(me->GetEntry())->second);

            if (me->GetInstanceScript())
            {
                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (Creature* paragonController = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI)))
                    paragonController->AI()->DoAction(ACTION_NEXT_PARAGON_MEMBER);

                switch (me->GetEntry())
                {
                    case NPC_KILRUK_THE_WIND_REAVER:
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GOUGE);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TENDERIZING_STRIKES);
                        me->RemoveAllAreasTrigger(); // reaver eff
                        break;
                    case NPC_XARIL_THE_POISONED_MIND:
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPOSED_VEINS);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_RED);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_BLUE);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TOXIN_YELLOW);
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_CAUSTIC_BLOOD);
                        break;
                    case NPC_KORVEN_THE_PRIME:
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SHIELD_BASH);
                        break;
                    case NPC_IYYOKUK_THE_LUCID:
                        for (auto&& calculation : m_CalculationList)
                            instance->DoRemoveAurasDueToSpellOnPlayers(calculation);
                        break;
                    case NPC_SKEER_THE_BLOODSEEKER:
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GENETIC_ALTERATION);
                        break;
                    case NPC_RIKKAL_THE_DISSECTOR:
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_HEWN);
                        break;
                    case NPC_HISEK_THE_SWARMKEEPER:
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_AIM_MARK);
                        break;
                }
            }
        }
    }

    private:
        bool hasAnyParagonStrength(Unit* target)
        {
            for (auto&& itr : paragonStrengthList)
                if (target->HasAura(itr))
                    return true;

            return false;
        }
};

struct boss_kilruk_the_wind_reaver : public soo_paragon_typeAI
{
    boss_kilruk_the_wind_reaver(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MUTILATE, 17 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DEATH_FROM_ABOVE, 26 * IN_MILLISECONDS);
        me->SetAutoattackOverrideSpell(SPELL_RAZOR_SHARP_BLADES, 0);

        if (IsHeroic())
            events.ScheduleEvent(EVENT_REAVE, 40 * IN_MILLISECONDS);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_JUMP_TO_COMBAT:
                    DoCast(me, SPELL_JUMP_TO_CENTER);
                    break;
                case EVENT_REMOVE_ADDITIONAL_ABILITY:
                    me->SetReactState(REACT_AGGRESSIVE);
                    hasMutilateSeq = false;
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) || deathfromAbove || hasMutilateSeq)
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MUTILATE:
                    hasMutilateSeq = true;
                    me->SetReactState(REACT_PASSIVE);

                    if (Unit* vict = me->GetVictim())
                        DoCast(vict, SPELL_GOUGE);

                    events.ScheduleEvent(EVENT_MUTILATE, 25 * IN_MILLISECONDS);
                    nonCombatEvents.ScheduleEvent(EVENT_REMOVE_ADDITIONAL_ABILITY, 3 * IN_MILLISECONDS);
                    break;
                case EVENT_REAVE:

                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    DoCast(me, SPELL_REAVE_SELECTOR);
                    events.ScheduleEvent(EVENT_REAVE, 30 * IN_MILLISECONDS);
                    break;
                case EVENT_DEATH_FROM_ABOVE:
                    deathfromAbove = true;

                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                    {
                        DoCast(target, SPELL_DEATH_FROM_ABOVE_MARK);
                        DoCast(target, SPELL_DEATH_FROM_ABOVE_SUMM);

                        me->OverrideInhabitType(INHABIT_AIR);
                        me->UpdateMovementFlags();
                        GetPositionWithDistInOrientation(me, 10.0f, me->GetAngle(target), x, y);
                       
                        if (Unit* target = me->GetVictim())
                            targetGUID = target->GetGUID();

                        me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ() + 10.0f);

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration() + 1000), [this](TaskContext context)
                        {
                            DoCast(me, SPELL_DEATH_FROM_ABOVE_JUMP_EFF);

                            scheduler
                                .Schedule(Milliseconds(500), [this](TaskContext context)
                            {
                                DoCast(me, SPELL_REAVE_EFF);
                            });

                            scheduler
                                .Schedule(Milliseconds(1200), [this](TaskContext context)
                            {
                                me->OverrideInhabitType(INHABIT_GROUND);
                                me->UpdateMovementFlags();

                                me->RemoveChanneledCast(targetGUID);
                                deathfromAbove = false;
                            });
                        });
                    }
                    else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    {
                        DoCast(target, SPELL_DEATH_FROM_ABOVE_MARK);
                        DoCast(target, SPELL_DEATH_FROM_ABOVE_SUMM);

                        me->OverrideInhabitType(INHABIT_AIR);
                        me->UpdateMovementFlags();
                        GetPositionWithDistInOrientation(me, 5.0f, me->GetAngle(target), x, y);

                        if (Unit* target = me->GetVictim())
                            targetGUID = target->GetGUID();

                        me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ() + 25.0f);

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration() + 1000), [this](TaskContext context)
                        {
                            DoCast(me, SPELL_DEATH_FROM_ABOVE_JUMP_EFF);

                            scheduler
                                .Schedule(Milliseconds(800), [this](TaskContext context)
                            {
                                DoCast(me, SPELL_REAVE_EFF);
                            });

                            scheduler
                                .Schedule(Milliseconds(1200), [this](TaskContext context)
                            {
                                me->OverrideInhabitType(INHABIT_GROUND);
                                me->UpdateMovementFlags();

                                me->RemoveChanneledCast(targetGUID);
                                deathfromAbove = false;
                            });
                        });
                    }

                    events.ScheduleEvent(EVENT_DEATH_FROM_ABOVE, 40 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct boss_xaril_the_poisoned_mind : public soo_paragon_typeAI
{
    boss_xaril_the_poisoned_mind(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_TOXIC_INJECTION);
        events.ScheduleEvent(EVENT_TOXIN_REACT, 17 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CAUSTIC_BLOOD, urand(9 * IN_MILLISECONDS, 10.5 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_TENDERIZING_STRIKES, urand(8 * IN_MILLISECONDS, 8.5*IN_MILLISECONDS));
        me->SetAutoattackOverrideSpell(SPELL_TENDERIZING_STRIKES, 0);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_JUMP_TO_COMBAT)
                DoCast(me, SPELL_JUMP_TO_CENTER);
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_TOXIN_REACT:
                {
                    std::vector<uint32> m_catalysts = { SPELL_CATALYST_BLUE, SPELL_CATALYST_RED, SPELL_CATALYST_YELLOW };

                    if (IsHeroic())
                    {
                        m_catalysts.clear();
                        m_catalysts = { SPELL_CATALYST_PURPLE, SPELL_CATALYST_GREEN , SPELL_CATALYST_ORANGE };
                    }
                
                    // Combo of toxins shouldn`t be repeatable.
                    if (prevToxin)
                        m_catalysts.erase(std::find(m_catalysts.begin(), m_catalysts.end(), prevToxin));
                
                    // Select new toxin catalyst from updated container
                    prevToxin = Trinity::Containers::SelectRandomContainerElement(m_catalysts);
                
                    DoCast(me, prevToxin);
                    events.ScheduleEvent(EVENT_TOXIN_REACT, 33 * IN_MILLISECONDS);
                    break;
                }
                case EVENT_CAUSTIC_BLOOD:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_CAUSTIC_BLOOD);
                
                    events.ScheduleEvent(EVENT_CAUSTIC_BLOOD, urand(9 * IN_MILLISECONDS, 10.5 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct boss_kaztik_the_manipulator : public soo_paragon_typeAI
{
    boss_kaztik_the_manipulator(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SONIC_PROJECTION, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_KUNCHONG, 25 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MESMERIZE, 26 * IN_MILLISECONDS);

        kunchlongCount = Is25ManRaid() ? 4 : 2;

        if (instance && instance->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            kunchlongCount = 2; // min 2, max 4
            uint32 totalPlayersCount = instance->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                kunchlongCount++;

            if (totalPlayersCount > 20)
                kunchlongCount++;
        }

        for (uint8 i = 0; i < kunchlongCount; i++)
            DoCast(me, SPELL_SUMMON_HUNGRY_KUNCHLONG);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_DEVOUR)
            return calculateDone;

        return 0;
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetInCombatWithZone();
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_REMOVE_ADDITIONAL_ABILITY:
                    if (me->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                        break;

                    me->SetReactState(REACT_AGGRESSIVE);

                    if (Unit* itr = ObjectAccessor::GetUnit(*me, targetGUID))
                        me->Attack(itr, true);
                    else if (Player* itr = me->FindNearestPlayer(100.0f))
                        me->Attack(itr, true);
                    break;
                case EVENT_JUMP_TO_COMBAT:
                    DoCast(me, SPELL_JUMP_TO_CENTER);
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
            switch (eventId)
            {
                case EVENT_SONIC_PROJECTION:
                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    me->PrepareChanneledCast(me->GetOrientation(), SPELL_SONIC_PROJECTION);

                    nonCombatEvents.ScheduleEvent(EVENT_REMOVE_ADDITIONAL_ABILITY, 3.5 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_SONIC_PROJECTION, 4 * IN_MILLISECONDS);
                    break;
                case EVENT_KUNCHONG:
                    DoCast(me, SPELL_SUMMON_HUNGRY_KUNCHLONG);
                    events.ScheduleEvent(EVENT_KUNCHONG, 25 * IN_MILLISECONDS);
                    break;
                case EVENT_MESMERIZE:
                    DoCast(me, SPELL_MESMERIZE);
                    events.ScheduleEvent(EVENT_MESMERIZE, 35 * IN_MILLISECONDS);
                    break;
            }
        }
    }
};

struct boss_korven_the_prime : public soo_paragon_typeAI
{
    boss_korven_the_prime(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHIELD_BASH, 18 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_AMBER_REGENERATION, 5 * IN_MILLISECONDS);
    }

    void SpellHit(Unit* caster, SpellInfo const* spell) override
    {
        if (spell->HasEffect(SPELL_EFFECT_ATTACK_ME) || spell->HasAura(SPELL_AURA_MOD_TAUNT))
            me->SetFacingTo(me->GetAngle(caster));
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return amberGUID;
    }

    void KilledUnit(Unit* /*victim*/) override { }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_APPLY_ADITIONAL_ABILITY:
                    DoCast(me, SPELL_VICIOUS_ASSAULT);
                    break;
                case EVENT_REMOVE_ADDITIONAL_ABILITY:
                    me->RemoveChanneledCast(targetGUID);
                    break;
                case EVENT_JUMP_TO_COMBAT:
                    DoCast(me, SPELL_JUMP_TO_CENTER);
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
            switch (eventId)
            {
                case EVENT_SHIELD_BASH:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHIELD_BASH);

                    events.ScheduleEvent(EVENT_SHIELD_BASH, 18 * IN_MILLISECONDS);
                    break;
                case EVENT_AMBER_REGENERATION:
                    if (me->HasAura(SPELL_VICIOUS_ASSAULT)) // channeled issue
                    {
                        events.RescheduleEvent(EVENT_AMBER_REGENERATION, 2 * IN_MILLISECONDS);
                        break;
                    }

                    if (Creature* paragonLowest = ObjectAccessor::GetCreature(*me, LowestKlaxiInCombat(me->GetGUID())))
                    {
                        amberGUID = paragonLowest->GetGUID();
                        DoCast(paragonLowest, SPELL_ENCASE_IN_AMBER);
                    }
                    else
                    {
                        events.RescheduleEvent(EVENT_AMBER_REGENERATION, 1 * IN_MILLISECONDS);
                        break;
                    }

                    events.ScheduleEvent(EVENT_AMBER_REGENERATION, 40 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }

    private:
        ObjectGuid LowestKlaxiInCombat(ObjectGuid casterGUID)
        {
            std::list<Creature*> paragonList;

            for (auto&& itr : eParagonsOfTheKlaaxi)
                GetCreatureListWithEntryInGrid(paragonList, me, itr, 200.0f);

            paragonList.remove_if([=](Creature* target) { return target && (!target->IsInCombat() || target->HasAura(SPELL_PERMANENT_FEIGN_DEATH) || target->GetHealthPct() > 50.0f || target->GetGUID() == me->GetGUID()); });

            if (paragonList.empty())
            {
                if (me->GetHealthPct() <= 50.0f) // self priority if possible
                    return me->GetGUID();
                else
                    return ObjectGuid::Empty;
            }

            paragonList.sort(Trinity::HealthPctOrderPred());

            if (me->GetHealthPct() <= 50.0f) // self priority low
                paragonList.push_back(me);

            return paragonList.front()->GetGUID();
        }
};

// Amber 71407
struct npc_soo_paragon_amber : public ScriptedAI
{
    npc_soo_paragon_amber(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            paragonController->AI()->JustSummoned(me);
    }

    void Reset() override
    {
        me->SetInCombatWithZone();
        DoCast(me, SPELL_AMBER_BARRIER);

        if (Creature* klaxxi = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KORVEN_THE_PRIME) : ObjectGuid::Empty))
        {
            targetGUID = klaxxi->AI()->GetGUID(); // our target
            DoCast(me, SPELL_AMBER_REGENERATION);
        }

        if (IsHeroic())
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return targetGUID;
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* owner = ObjectAccessor::GetCreature(*me, targetGUID))
        {
            owner->RemoveAurasDueToSpell(SPELL_ENCASE_IN_AMBER);
            owner->InterruptNonMeleeSpells(false, SPELL_AMBER_REGENERATION_EFF);
        }

        // Should remove not at expire duration in this case
        if (GameObject* amberCollision = ObjectAccessor::GetGameObject(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(GO_ENCASE_IN_AMBER) : ObjectGuid::Empty))
            amberCollision->Delete();

        me->DisappearAndDie();
    }

private:
    InstanceScript* _instance;
};

struct boss_iyyokuk_the_lucid : public soo_paragon_typeAI
{
    boss_iyyokuk_the_lucid(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DIMINISH, 4 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CALCULATE, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_INSANE, 19 * IN_MILLISECONDS);
    }

    void SetData(uint32 type, uint32 data) override
    {
        calculateDone = data;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_JUMP_TO_COMBAT)
                DoCast(me, SPELL_JUMP_TO_CENTER);
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DIMINISH:
                    DoCast(me, SPELL_DIMINISH);
                    events.ScheduleEvent(EVENT_DIMINISH, 6 * IN_MILLISECONDS);
                    break;
                case EVENT_CALCULATE:
                {
                    calculateDone = 0;

                    std::list<Player*> PlayersInArea;
                    GetPlayerListInGrid(PlayersInArea, me, 200.0f);

                    for (auto&& itr : PlayersInArea)
                        if (uint32 m_mark = Trinity::Containers::SelectRandomContainerElement(m_CalculationList))
                            itr->AddAura(m_mark, itr);
                    break;
                }
                case EVENT_INSANE:
                    DoCast(me, SPELL_INSANE_CALC_FIERY_EDGE);
                    events.ScheduleEvent(EVENT_INSANE, 50 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct boss_karoz_the_locust : public soo_paragon_typeAI
{
    boss_karoz_the_locust(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_STORE_KINETIC_ENERGY, 9 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_PREPARE_TO_JUMP, 45 * IN_MILLISECONDS);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_FLASH_DATA)
            flashDistance = data;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_FLASH_DATA)
            return flashDistance;

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_JUMP_TO_COMBAT:
                    DoCast(me, SPELL_JUMP_TO_CENTER);
                    break;
                case EVENT_APPLY_ADITIONAL_ABILITY:
                    me->SetHomePosition(*me); // for calculation
                    DoCast(me, SPELL_STORE_KINETIC_ENERGY, true);
                    break;
                case EVENT_REMOVE_ADDITIONAL_ABILITY:
                    if (Creature* m_rAmber = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_MANTID_AMBER) : ObjectGuid::Empty))
                        me->GetMotionMaster()->MoveJump(m_rAmber->GetPositionX() + frand(-8.0f, 8.0f), m_rAmber->GetPositionY() + frand(-10.0f, 10.0f), m_rAmber->GetPositionZ(), 20.0f, 25.0f, EVENT_JUMP + 4);
                    break;
                case EVENT_FLASH:
                    DoCast(me, SPELL_FLASH_EFF);
                    break;
                case EVENT_LAST_FLASH:
                    DoCast(me, SPELL_FLASH_EFF);
                    inWhirl = false;
                    whirlCount = 0;
                    me->RemoveChanneledCast(targetGUID);
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
            switch (eventId)
            {
                case EVENT_STORE_KINETIC_ENERGY:
                    if (throwAmberPreparation)
                    {
                        events.RescheduleEvent(EVENT_STORE_KINETIC_ENERGY, 2 * IN_MILLISECONDS);
                        break;
                    }

                    whirlCount = 0;
                    inWhirl = true;
                    me->PrepareChanneledCast(me->GetOrientation());
                    me->SetHomePosition(*me); // for calculation
                    DoCast(me, SPELL_STORE_KINETIC_ENERGY);
                    events.ScheduleEvent(EVENT_STORE_KINETIC_ENERGY, 75 * IN_MILLISECONDS);
                    break;
                case EVENT_PREPARE_TO_JUMP:
                {
                    if (inWhirl)
                    {
                        events.RescheduleEvent(EVENT_PREPARE_TO_JUMP, 2 * IN_MILLISECONDS);
                        break;
                    }

                    throwAmberPreparation = true;

                    me->CastSpell(KazowAmberSpawn[GetNearestAmberPlatformWithAmber()], SPELL_CREATE_AMBER_BOMB, true);
                    events.ScheduleEvent(EVENT_JUMP_TO_AMBER, urand(3 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                    events.ScheduleEvent(EVENT_PREPARE_TO_JUMP, 64 * IN_MILLISECONDS);
                    break;
                }
                case EVENT_JUMP_TO_AMBER:
                {
                    if (Unit* vict = me->GetVictim())
                    {
                        targetGUID = vict->GetGUID();

                        if (Creature* AmberBomb = me->FindNearestCreature(NPC_AMBER_BOMB, 150.0f, true))
                        {
                            me->PrepareChanneledCast(me->GetAngle(AmberBomb));
                            me->GetMotionMaster()->MoveJump(AmberBomb->GetPositionX(), AmberBomb->GetPositionY(), AmberBomb->GetPositionZ(), 30.0f, 25.0f, EVENT_JUMP + 3);
                        }
                    }
                    break;
                }
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }

private:
    uint32 GetNearestAmberPlatformWithAmber()
    {
        float dist = 300.0f;
        uint32 num = 0;

        for (uint32 i = 0; i < 8; i++)
        {
            if (dist > me->GetExactDist2d(KazowAmberSpawn[i].GetPositionX(), KazowAmberSpawn[i].GetPositionY()))
            {
                dist = me->GetExactDist2d(KazowAmberSpawn[i].GetPositionX(), KazowAmberSpawn[i].GetPositionY());
                num = i;
            }
        }

        return num;
    }
};

struct boss_skeer_the_bloodseeker : public soo_paragon_typeAI
{
    boss_skeer_the_bloodseeker(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_BLOODLETTING, 5 * IN_MILLISECONDS);
        me->SetAutoattackOverrideSpell(SPELL_HEW, 0);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
        summon->SetWalk(true);
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        bloodlettingTargetGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return bloodlettingTargetGUID;
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_JUMP_TO_COMBAT)
                DoCast(me, SPELL_JUMP_TO_CENTER);
            break;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_BLOODLETTING)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_BLOODLETTING_PARAGON);

                events.ScheduleEvent(EVENT_BLOODLETTING, urand(42 * IN_MILLISECONDS, 43 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct boss_rikkal_the_dissector : public soo_paragon_typeAI
{
    boss_rikkal_the_dissector(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_INJECTION, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MUTATE, 20 * IN_MILLISECONDS);
        me->SetAutoattackOverrideSpell(SPELL_GENETIC_ALTERATION, 0);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        summon->SetInCombatWithZone();
        summon->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_JUMP_TO_COMBAT)
                DoCast(me, SPELL_JUMP_TO_CENTER);
            break;
        }

        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_INJECTION:
                    if (Unit* victim = me->GetVictim())
                        DoCast(victim, SPELL_INJECTION);

                    events.ScheduleEvent(EVENT_INJECTION, 9 * IN_MILLISECONDS);
                    break;
                case EVENT_MUTATE:
                    DoCast(me, SPELL_MUTATE_SELECTOR);
                    events.ScheduleEvent(EVENT_MUTATE, urand(31 * IN_MILLISECONDS, 32 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
        EnterEvadeIfOutOfCombatArea(diff);
    }
};

struct boss_hisek_the_swarmkeeper : public soo_paragon_typeAI
{
    boss_hisek_the_swarmkeeper(Creature* creature) : soo_paragon_typeAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->HandleEmoteStateCommand(EMOTE_STATE_READY_RIFLE);

        events.ScheduleEvent(EVENT_MULTY_SHOT, 5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_AIM, 35 * IN_MILLISECONDS);

        if (IsHeroic())
            events.ScheduleEvent(EVENT_RAPID_FIRE, 42 * IN_MILLISECONDS);
    }

    void KilledUnit(Unit* /*victim*/) override { }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_KLAXXI_AIM_TARGET:
                return aimTarget;
            case TYPE_KLAXXI_AIM_COUNT:
                return aimCount;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_KLAXXI_AIM_COUNT)
            aimCount = data;
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        aimTargetGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return aimTargetGUID;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_REMOVE_ADDITIONAL_ABILITY:
                    if (me->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                        break;

                    me->SetReactState(REACT_AGGRESSIVE);

                    if (Unit* itr = ObjectAccessor::GetUnit(*me, targetGUID))
                        me->Attack(itr, true);
                    else if (Player* itr = me->FindNearestPlayer(100.0f))
                        me->Attack(itr, true);
                    break;
                case EVENT_JUMP_TO_COMBAT:
                    DoCast(me, SPELL_JUMP_TO_CENTER);
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
            switch (eventId)
            {
                case EVENT_AIM:
                    if (Unit* target = me->GetVictim())
                        targetGUID = target->GetGUID();

                    DoCast(me, SPELL_AIM);
                    events.ScheduleEvent(EVENT_AIM, 35 * IN_MILLISECONDS);
                    break;
                case EVENT_MULTY_SHOT:
                    DoCast(me, SPELL_MULTY_SHOT);
                    events.ScheduleEvent(EVENT_MULTY_SHOT, 4 * IN_MILLISECONDS);
                    break;
                case EVENT_RAPID_FIRE:
                    DoCast(me, SPELL_RAPID_FIRE);
                    events.ScheduleEvent(EVENT_RAPID_FIRE, 50 * IN_MILLISECONDS);
                    nonCombatEvents.ScheduleEvent(EVENT_REMOVE_ADDITIONAL_ABILITY, 8.5 * IN_MILLISECONDS);
                    break;
            }
        }
    }
};

// Mantid amber 71628
struct npc_paragon_mantid_amber : public ScriptedAI
{
    npc_paragon_mantid_amber(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

    bool hasTriggered;

    void Reset() override
    {
        if (me->IsAlive() && me->GetPositionZ() < -1000.0f)
            EnterEvadeMode();

        hasTriggered = false;
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->SetDisplayId(27824);
        DoCast(me, SPELL_COSMETIC_TO_FIGHT);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasTriggered)
            return;

        hasTriggered = true;
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        DoAction(ACTION_KLAXXI_COMBAT);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(31);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_KLAXXI_COMBAT:
                me->RemoveAurasDueToSpell(SPELL_COSMETIC_TO_FIGHT);
            
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* m_Paragon = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI)))
                        m_Paragon->AI()->DoAction(ACTION_KLAXXI_COMBAT);
                break;
        }
    }

private:
    InstanceScript* _instance;
};

// Hungry Kunchong 71420
struct npc_soo_paragon_hungry_kunchong : public ScriptedAI
{
    npc_soo_paragon_hungry_kunchong(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;
    EventMap events;
    float requiredDamage;
    ObjectGuid mesmerizeGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            paragonController->AI()->JustSummoned(me);

        DoCast(me, SPELL_NON_REGENERATE_POWER);
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 0);

        DoCast(me, SPELL_HUNGRY);
        DoCast(me, SPELL_THICK_SHELL);
        me->GetMotionMaster()->MoveRandom(7.5f);
        targetGUID = ObjectGuid::Empty;
        requiredDamage = 100.0f;
        mesmerizeGUID = ObjectGuid::Empty;
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        mesmerizeGUID = guid;
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (!me->HasAura(SPELL_THICK_SHELL) && me->GetHealthPct() <= requiredDamage - 25.0f)
        {
            requiredDamage = me->GetHealthPct();

            if (Unit* target = ObjectAccessor::GetUnit(*me, mesmerizeGUID))
            {
                target->RemoveAurasDueToSpell(SPELL_DEVOUR);
                target->RemoveAurasDueToSpell(SPELL_DEVOUR_P);
                target->RemoveAurasDueToSpell(SPELL_MESMERIZE_AURA);

                me->RemoveAurasDueToSpell(SPELL_DEVOUR);
                me->RemoveAurasDueToSpell(SPELL_DEVOUR_UNK);
            }
        }
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !me->HasAura(SPELL_MOLT))
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, mesmerizeGUID))
            {
                target->RemoveAurasDueToSpell(SPELL_DEVOUR);
                target->RemoveAurasDueToSpell(SPELL_DEVOUR_P);
                target->RemoveAurasDueToSpell(SPELL_MESMERIZE_AURA);
            }

            me->RemoveAurasDueToSpell(SPELL_HUNGRY);
            me->RemoveAurasDueToSpell(SPELL_THICK_SHELL);
            me->RemoveAurasDueToSpell(SPELL_DEVOUR);
            me->RemoveAurasDueToSpell(SPELL_DEVOUR_UNK);
            me->StopMoving();
            me->GetMotionMaster()->Clear();

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            me->SetReactState(REACT_AGGRESSIVE);
            
            if (Player* target = me->FindNearestPlayer(200.0f))
                me->RemoveChanneledCast(target->GetGUID());

            DoCast(me, SPELL_MOLT, true);
            events.ScheduleEvent(EVENT_SWIPE, 6 * IN_MILLISECONDS);
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
            if (eventId == EVENT_SWIPE)
            {
                if (Unit* vict = me->GetVictim())
                {
                    targetGUID = vict->GetGUID();
                    me->PrepareChanneledCast(me->GetAngle(vict), SPELL_SWIPE);

                    scheduler
                        .Schedule(Milliseconds(3500), [this](TaskContext context)
                    {
                        me->RemoveChanneledCast(targetGUID);
                    });
                }

                events.ScheduleEvent(EVENT_SWIPE, 10 * IN_MILLISECONDS);
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Blood 71542
struct npc_blood : public ScriptedAI
{
    npc_blood(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;
    bool reachTarget;

    void Reset() override
    {
        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            paragonController->AI()->JustSummoned(me);

        me->SetInCombatWithZone();
        me->UpdateSpeed(MOVE_RUN, true);
        reachTarget = false;
        targetGUID = ObjectGuid::Empty;

        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
        DoCast(me, SPELL_GROW);

        // Try to get our target from Skeer
        if (Creature* skeer = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SKEER_THE_BLOODSEEKER) : ObjectGuid::Empty))
            targetGUID = skeer->AI()->GetGUID();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->PrepareChanneledCast(me->GetOrientation());

        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (!targetGUID) // if not found (but nvm, this shouldn`t execute)
                targetGUID = getLowestKlaxxiGUID();

            if (Creature* target = ObjectAccessor::GetCreature(*me, targetGUID))
            {
                me->GetMotionMaster()->MoveFollow(target, 2.0f, me->GetAngle(target));

                if (me->GetExactDist2d(target) <= 8.0f && !reachTarget)
                {
                    reachTarget = true;
                    me->GetMotionMaster()->MovementExpired();

                    int32 effValue = (int32)(me->GetHealthPct());

                    if (!target->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                        me->CastCustomSpell(target, SPELL_BLOOD_INFUSION, &effValue, 0, 0, true);
                    else
                        me->CastCustomSpell(me, SPELL_BLOOD_INFUSION, &effValue, 0, 0, true); // just for effect (blizzlike)

                    me->DespawnOrUnsummon(0.2 * IN_MILLISECONDS);
                    return;
                }
            }

            context.Repeat(Seconds(1));
        });
    }

    ObjectGuid getLowestKlaxxiGUID()
    {
        std::list<Creature*> paragonMembers;
        
        for (auto&& itr : eParagonsOfTheKlaaxi)
            GetCreatureListWithEntryInGrid(paragonMembers, me, itr, 300.0f);

        paragonMembers.remove_if([=](Creature* target) { return target && (!target->IsInCombat() || target->HasAura(SPELL_PERMANENT_FEIGN_DEATH)); });

        if (paragonMembers.empty())
            return ObjectGuid::Empty;

        paragonMembers.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = paragonMembers.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Amber Parasite 71578
struct npc_paragon_amber_parasite : public ScriptedAI
{
    npc_paragon_amber_parasite(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid fixateTargetGUID;

    void Reset() override
    {
        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            paragonController->AI()->JustSummoned(me);

        DoCast(me, SPELL_NON_REGENERATE_POWER);
        me->SetPowerType(POWER_ENERGY);
        me->SetMaxPower(POWER_ENERGY, 100);
        me->SetPower(POWER_ENERGY, 0);
        fixateTargetGUID = ObjectGuid::Empty;

        DoCast(me, SPELL_GENETIC_MODIFICATION);

        scheduler
            .Schedule(Seconds(1), Seconds(4), [this](TaskContext context)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
                DoCast(me, SPELL_FEED);

            context.Repeat(Seconds(5));
        });
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        fixateTargetGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return fixateTargetGUID;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Amber Bomb 71691
struct npc_paragon_amber_bomb : public ScriptedAI
{
    npc_paragon_amber_bomb(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            paragonController->AI()->JustSummoned(me);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Eerie Fog 71453
struct npc_paragon_eerie_fog : public ScriptedAI
{
    npc_paragon_eerie_fog(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    Position spawnPos;
    bool hasLaunched;

    void Reset() override
    {
        me->DespawnOrUnsummon(50 * IN_MILLISECONDS);
        hasLaunched = 0;
        spawnPos = { me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation() };

        if (Creature* paragonController = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
            paragonController->AI()->JustSummoned(me);

        me->SetWalk(true);
        events.ScheduleEvent(EVENT_FOG_MOVE, 1000);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_EERIE_FOG_DELAY)
            return hasLaunched ? 1 : 0;

        return 0;
    }

    void UpdateAI(uint32 diff) override 
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_FOG_MOVE)
                GenerateMovement();
            break;
        }
    }

    private:
        void GenerateMovement()
        {
            if (!GetData(TYPE_EERIE_FOG_DELAY))
            {
                DoCast(me, SPELL_EERIE_FOG_AT, true);
                hasLaunched = true;
            }

            float x = spawnPos.GetPositionX() + frand(-12.0f, 12.0f);
            float y = spawnPos.GetPositionY() + frand(-12.0f, 12.0f);
            float z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->GetMotionMaster()->MovePoint(0, x, y, z);
            events.ScheduleEvent(EVENT_FOG_MOVE, me->GetSplineDuration());
        }
};

// Paragons Purpose 143483
class spell_soo_paragon_purpose : public SpellScript
{
    PrepareSpellScript(spell_soo_paragon_purpose);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (GetCaster()->GetMap()->IsHeroic()) // not buff on each death, heroic has another algorythm
            return;

        if (Unit* target = GetHitUnit())
            target->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_paragon_purpose::HandleEffectHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Cosmetic for fight starter 143548
class spell_soo_cosmetic_for_fight_starter : public AuraScript
{
    PrepareAuraScript(spell_soo_cosmetic_for_fight_starter);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* m_owner = GetOwner()->ToCreature())
            m_owner->SetVisible(false);
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* m_owner = GetOwner()->ToCreature())
            m_owner->SetVisible(true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_cosmetic_for_fight_starter::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_soo_cosmetic_for_fight_starter::OnAuraEffectApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

struct ParagonPredicate
{
    bool operator ()(WorldObject const* object) const
    {
        for (auto m_paragon : eParagonsOfTheKlaaxi)
            if (object->GetEntry() == m_paragon)
                return false;

        return true;
    }
};

struct ParagonActivityProtectionPredicate
{
    bool operator ()(WorldObject const* object) const
    {
        for (auto m_vSpell : eProtectiveSpells)
            if (object && object->ToUnit() && object->ToUnit()->HasAura(m_vSpell))
                return false;

        return true;
    }
};

// Berserk 146982
class spell_soo_paragon_berserk : public SpellScript
{
    PrepareSpellScript(spell_soo_paragon_berserk);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(ParagonPredicate());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_paragon_berserk::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_paragon_berserk::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_paragon_berserk::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_paragon_berserk::FilterTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_paragon_berserk::FilterTargets, EFFECT_4, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Gouge 143939
class spell_soo_paragon_gouge : public AuraScript
{
    PrepareAuraScript(spell_soo_paragon_gouge);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (Unit* owner = GetOwner()->ToUnit())
                caster->CastSpell(owner, SPELL_MUTILATE, false);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_soo_paragon_gouge::OnAuraEffectApply, EFFECT_1, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

class PlayerTargetSelector
{
    public:
        PlayerTargetSelector() {}

        bool operator() (WorldObject* object)
        {
            return object && !object->ToPlayer();
        }
};

// Toxic Injection 142528
class spell_soo_paragon_toxic_injection : public SpellScript
{
    PrepareSpellScript(spell_soo_paragon_toxic_injection);

    bool Load() override
    {
        prevToxinID = 0;
        return true;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(PlayerTargetSelector());
    }

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        std::vector<uint32> invToxinReactionType = { SPELL_TOXIN_RED, SPELL_TOXIN_BLUE, SPELL_TOXIN_YELLOW };

        if (prevToxinID && std::find(invToxinReactionType.begin(), invToxinReactionType.end(), prevToxinID) != invToxinReactionType.end())
            invToxinReactionType.erase(std::find(invToxinReactionType.begin(), invToxinReactionType.end(), prevToxinID));

        prevToxinID = Trinity::Containers::SelectRandomContainerElement(invToxinReactionType);

        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, prevToxinID, true);

    }

    private:
        uint32 prevToxinID;

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_paragon_toxic_injection::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_soo_paragon_toxic_injection::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Caustic Blood 142315
class spell_paragon_caustic_blood : public SpellScript
{
    PrepareSpellScript(spell_paragon_caustic_blood);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (GetHitUnit() && HasAnyDefence(GetHitUnit()))
            PreventHitAura();
    }

    bool HasAnyDefence(Unit* target)
    {
        for (auto dItr : eProtectiveSpells)
            if (target->HasAura(dItr))
                return true;

        return false;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_caustic_blood::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

class spell_paragon_caustic_blood_aura : public AuraScript
{
    PrepareAuraScript(spell_paragon_caustic_blood_aura);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 9 && GetOwner() && GetOwner()->ToUnit())
        {
            GetOwner()->ToUnit()->CastSpell(GetOwner()->ToUnit(), SPELL_BLOODY_EXPLOSION, true);
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_paragon_caustic_blood_aura::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

// Catalyst React Blue 142725
class spell_soo_catalyst_react_blue : public SpellScript
{
    PrepareSpellScript(spell_soo_catalyst_react_blue);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_REACTION_BLUE, true);
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) {return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_TOXIN_BLUE); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_catalyst_react_blue::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_catalyst_react_blue::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Catalyst React Red 142726
class spell_soo_catalyst_react_red : public SpellScript
{
    PrepareSpellScript(spell_soo_catalyst_react_red);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_REACTION_RED, true);
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) {return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_TOXIN_RED); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_catalyst_react_red::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_catalyst_react_red::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Catalyst React Yellow 142727
class spell_soo_catalyst_react_yellow : public SpellScript
{
    PrepareSpellScript(spell_soo_catalyst_react_yellow);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_REACTION_YELLOW, true);
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) {return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_TOXIN_YELLOW); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_catalyst_react_yellow::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_catalyst_react_yellow::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Catalyst React Purple 142729
class spell_soo_catalyst_react_purple : public SpellScript
{
    PrepareSpellScript(spell_soo_catalyst_react_purple);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_CANNED_HEAT_LAUNCH, true);
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) {return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_TOXIN_RED) && !target->ToPlayer()->HasAura(SPELL_TOXIN_BLUE); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_catalyst_react_purple::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_catalyst_react_purple::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Catalyst React Orange 142728
class spell_soo_catalyst_react_orange : public SpellScript
{
    PrepareSpellScript(spell_soo_catalyst_react_orange);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            std::vector<uint32> orangeCatalysts = { SPELL_EXPLOSIVE_RING_AT_25, SPELL_EXPLOSIVE_RING_AT_20, SPELL_EXPLOSIVE_RING_AT_15 };
            target->CastSpell(target, Trinity::Containers::SelectRandomContainerElement(orangeCatalysts), true);
        }
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) {return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_TOXIN_RED) && !target->ToPlayer()->HasAura(SPELL_TOXIN_YELLOW); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_catalyst_react_orange::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_catalyst_react_orange::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Catalyst React Green 142730
class spell_soo_catalyst_react_green : public SpellScript
{
    PrepareSpellScript(spell_soo_catalyst_react_green);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_EERIE_FOG_SUMM, true);
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) {return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_TOXIN_BLUE) && !target->ToPlayer()->HasAura(SPELL_TOXIN_YELLOW); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_catalyst_react_green::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_catalyst_react_green::CheckTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Shield Bash 143974
class spell_soo_paragon_shield_bash : public SpellScript
{
    PrepareSpellScript(spell_soo_paragon_shield_bash);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_VICIOUS_ASSAULT);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_paragon_shield_bash::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Vicious Assault 143977
class spell_soo_vicious_assault : public AuraScript
{
    PrepareAuraScript(spell_soo_vicious_assault);

    bool Load() override
    {
        triggerCount = 0;
        return true;
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            ++triggerCount;
            uint32 triggeringSpell = getSpellByTriggeringCount(triggerCount);

            if (triggeringSpell)
                owner->CastSpell(owner, triggeringSpell, true);
        }
    }

    uint32 getSpellByTriggeringCount(uint32 value)
    {
        switch (value)
        {
            case 2:
                return SPELL_VICIOUS_ASSAULT_AURA;
            case 4:
                return SPELL_VICIOUS_ASSAULT_PROC_1;
            case 6:
                return SPELL_VICIOUS_ASSAULT_PROC_2;
            case 8:
                return SPELL_VICIOUS_ASSAULT_PROC_3;
            case 10:
                return SPELL_VICIOUS_ASSAULT_PROC_4;
            case 12:
                return SPELL_VICIOUS_ASSAULT_PROC_5;
        }

        return 0;
    }

    private:
        uint32 triggerCount;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_soo_vicious_assault::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Bloodletting 143280
class spell_soo_bloodletting : public SpellScript
{
    PrepareSpellScript(spell_soo_bloodletting);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->CastSpell(bloodlettingMisslePos[0], SPELL_BLOODLETTING_MISSLE_2, true);

            if (caster->GetMap()->Is25ManRaid() || caster->GetInstanceScript() && caster->GetInstanceScript()->GetData(DATA_FLEX) && caster->GetInstanceScript()->instance->GetPlayersCountExceptGMs() > 17)
            {
                caster->CastSpell(bloodlettingMisslePos[1], SPELL_BLOODLETTING_MISSLE_3, true);
                caster->CastSpell(bloodlettingMisslePos[2], SPELL_BLOODLETTING_MISSLE_4, true);

                if (caster->GetMap()->IsHeroic())
                    caster->CastSpell(bloodlettingMisslePos[3], SPELL_BLOODLETTING_MISSLE_1, true);
            }
            else // a bit randomize
            {
                if (caster->GetMap()->IsHeroic())
                {
                    caster->CastSpell(bloodlettingMisslePos[1], SPELL_BLOODLETTING_MISSLE_3, true);
                    caster->CastSpell(bloodlettingMisslePos[2], SPELL_BLOODLETTING_MISSLE_4, true);
                }
                else
                    caster->CastSpell(bloodlettingMisslePos[2], urand(0, 1) ? SPELL_BLOODLETTING_MISSLE_3 : SPELL_BLOODLETTING_MISSLE_4, true);
            }

            // Calculate blood fixate target now!
            if (ObjectGuid calculatedGUID = getLowestKlaxxiGUID())
                caster->AI()->SetGUID(calculatedGUID);
        }
    }

    private:
        ObjectGuid getLowestKlaxxiGUID()
        {
            std::list<Creature*> paragonMembers;

            for (auto&& itr : eParagonsOfTheKlaaxi)
                GetCreatureListWithEntryInGrid(paragonMembers, GetCaster(), itr, 300.0f);

            paragonMembers.remove_if([=](Creature* target) { return target && (!target->IsInCombat() || target->HasAura(SPELL_PERMANENT_FEIGN_DEATH)); });

            if (paragonMembers.empty())
                return ObjectGuid::Empty;

            paragonMembers.sort(Trinity::HealthPctOrderPred());

            if (Creature* lowestTarget = paragonMembers.front())
                return lowestTarget->GetGUID();

            return ObjectGuid::Empty;
        }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_soo_bloodletting::HandleAfterCast);
    }
};

// Injection 143339
class spell_paragon_injection : public SpellScript
{
    PrepareSpellScript(spell_paragon_injection);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (GetHitUnit() && HasAnyDefence(GetHitUnit()))
            PreventHitAura();
    }

    bool HasAnyDefence(Unit* target)
    {
        for (auto dItr : eProtectiveSpells)
            if (target->HasAura(dItr))
                return true;

        return false;
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_injection::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Injection 143339
class spell_paragon_injection_aura : public AuraScript
{
    PrepareAuraScript(spell_paragon_injection_aura);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_PARAGONS_OF_THE_KLAXXI) != IN_PROGRESS)
                return;

            if (GetCaster() && !GetCaster()->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                GetCaster()->CastSpell(owner, SPELL_INJECTION_INITIALIZE, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_paragon_injection_aura::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Diminish 143666
class spell_soo_paragon_diminish : public SpellScript
{
    PrepareSpellScript(spell_soo_paragon_diminish);

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            uint32 healthValue = target->GetHealth();
            int32 recalculateDamage = (int32)(healthValue * 0.34);
            SetHitDamage(recalculateDamage);
        }
    }

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (GetHitUnit() && GetHitUnit()->GetHealthPct() < 25.0f)
                caster->Kill(GetHitUnit());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_paragon_diminish::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_soo_paragon_diminish::HandleEffectHit, EFFECT_1, SPELL_EFFECT_DUMMY);
    }
};

// Insane Calculation: Fiery Edge 142416
class spell_soo_insane_calculation_fiery_edge : public SpellScript
{
    PrepareSpellScript(spell_soo_insane_calculation_fiery_edge);

    std::vector<ObjectGuid> targetGUIDs;

    void HandleAfterCast()
    {
        if (targetGUIDs.empty() || targetGUIDs.size() < 2)
            return;

        for (uint8 i = 0; i < targetGUIDs.size(); i++)
        {
            if (i == targetGUIDs.size() - 1) // cast to first target
            {
                if (Unit* caster = ObjectAccessor::GetUnit(*GetCaster(), targetGUIDs[i]))
                    if (Unit* target = ObjectAccessor::GetUnit(*GetCaster(), targetGUIDs[0]))
                        caster->CastSpell(target, SPELL_FIERY_EDGE_BEAM, true);
            }
            else
            {
                if (Unit* caster = ObjectAccessor::GetUnit(*GetCaster(), targetGUIDs[i]))
                    if (Unit* target = ObjectAccessor::GetUnit(*GetCaster(), targetGUIDs[i + 1]))
                        caster->CastSpell(target, SPELL_FIERY_EDGE_BEAM, true);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(PlayerTargetSelector());

        // real target calculation is select players with any combined elements (numberic, icon, colour) @Todo

        uint32 maxTargets = GetCaster()->GetMap()->Is25ManRaid() ? 8 : 3;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            maxTargets = 3; // min 3, max 8
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 12)
                maxTargets++;

            if (totalPlayersCount > 14)
                maxTargets++;

            if (totalPlayersCount > 17)
                maxTargets++;

            if (totalPlayersCount > 19)
                maxTargets++;

            if (totalPlayersCount > 22)
                maxTargets++;

            if (targets.size() > maxTargets)
                Trinity::Containers::RandomResizeList(targets, maxTargets); // should be there for flex
        }

        if (GetCaster()->GetMap()->IsHeroic())
            maxTargets += GetCaster()->GetMap()->Is25ManRaid() ? 5 : 3;

        if (targets.size() > maxTargets)
            Trinity::Containers::RandomResizeList(targets, maxTargets);

        for (auto&& itr : targets)
            targetGUIDs.push_back(itr->GetGUID());
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_soo_insane_calculation_fiery_edge::HandleAfterCast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_insane_calculation_fiery_edge::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Aim 142948
class spell_soo_aim_aura : public AuraScript
{
    PrepareAuraScript(spell_soo_aim_aura);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0) // Prevent hit if this cast was interrupted by fake death, invisibility, vanish, amber etc.
        {
            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->AI()->DoAction(ACTION_REMOVE_AIM);

            return;
        }

        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(owner, SPELL_SHOT, true);

                if (caster->ToCreature())
                    caster->ToCreature()->AI()->DoAction(ACTION_REMOVE_AIM);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_aim_aura::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Aim 142948
class spell_soo_aim : public SpellScript
{
    PrepareSpellScript(spell_soo_aim);

    std::list<WorldObject*> m_targets, copyTargets;

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Player* target = GetHitPlayer())
            {
                // pull toward if near (a lower 45y)
                GetPositionWithDistInOrientation(caster, 45.0f, caster->GetAngle(target), x, y);
                float z = target->GetPositionZ();
                target->UpdateAllowedPositionZ(x, y, z);

                if (caster->GetExactDist2d(target) < 45.0f)
                    target->CastSpell(x, y, z, SPELL_POINT_BLANK_SHOT, true);
                else
                    target->CastSpell(target, SPELL_AIM_MARK, true);

                caster->AI()->SetGUID(target->GetGUID());
                caster->PrepareChanneledCast(caster->GetAngle(target));
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_MUTATE); });

        if (targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            copyTargets = targets;
            return;
        }

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    private:
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_aim::HandleEffectHit, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_aim::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_aim::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class AimPredicate 
{
    public:
        AimPredicate(Creature* const caster) : _caster(caster) { }

        bool operator()(WorldObject* object)
        {
            if (Unit* mainTarget = ObjectAccessor::GetUnit(*_caster, _caster->AI()->GetGUID()))
                return object && object->ToPlayer() && (object->ToPlayer()->GetExactDist2d(_caster) > 45.0f || !object->ToPlayer()->IsInBetween(_caster, mainTarget, 2.0f)) && object->ToPlayer()->GetGUID() != mainTarget->GetGUID();

            return false;
        }

    private:
        Creature const* _caster;
};

// Aim effect resonance 142950
class spell_soo_aim_resonance : public SpellScript
{
    PrepareSpellScript(spell_soo_aim_resonance);

    uint8 targetsCount;

    bool Load() override
    {
        targetsCount = 1;
        return true;
    }

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_SONIC_RESONANCE, true);

                if (targetsCount > 0)
                    SetHitDamage(GetHitDamage() / targetsCount);
            }
        }
    }

    void CheckTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(AimPredicate(caster));

        targetsCount = targets.size();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_aim_resonance::HandleEffectHit, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_aim_resonance::CheckTargets, EFFECT_1, TARGET_UNIT_CONE_UNK_130);
    }
};

// Amber Regeneration 144119
class spell_soo_amber_regeneration : public SpellScript
{
    PrepareSpellScript(spell_soo_amber_regeneration);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->DisappearAndDie();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_amber_regeneration::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Devour 142649
class spell_soo_devour : public AuraScript
{
    PrepareAuraScript(spell_soo_devour);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (owner->GetPower(POWER_ENERGY) < 95)
                owner->CastSpell(owner, SPELL_THICK_SHELL, true);
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_THICK_SHELL);
    }

    void OnAuraEffectRemoveDamage(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* m_owner = GetOwner()->ToPlayer())
            {
                if (m_owner->HasForcedMovement())
                    m_owner->SendApplyMovementForce(false, *caster);
            }
        }
    }

    void OnAuraEffectApplyDamage(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* m_owner = GetOwner()->ToPlayer())
            {
                if (m_owner->IsAlive() && !m_owner->HasForcedMovement())
                    m_owner->SendApplyMovementForce(true, *caster, 8.5f);
                else if (!m_owner->IsAlive() && m_owner->HasForcedMovement())
                    m_owner->SendApplyMovementForce(false, *caster);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_devour::OnAuraEffectRemove, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_soo_devour::OnAuraEffectApply, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_devour::OnAuraEffectRemoveDamage, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_soo_devour::OnAuraEffectApplyDamage, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Jump to Center 143545
class spell_paragon_jump_to_center : public SpellScript
{
    PrepareSpellScript(spell_paragon_jump_to_center);

    void SelectTargets(SpellDestination& dest)
    {
        if (Creature* mantidAmber = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_MANTID_AMBER) : ObjectGuid::Empty))
        {
            if (GetCaster()->GetEntry() == NPC_HISEK_THE_SWARMKEEPER)
                newPos = { mantidAmber->GetPositionX(), mantidAmber->GetPositionY(), mantidAmber->GetPositionZ(), 0.0f};
            else
                newPos = { mantidAmber->GetPositionX() + frand(-15.0f, 15.0f), mantidAmber->GetPositionY() + frand(-15.0f, 15.0f), mantidAmber->GetPositionZ(), 0.0f };

            dest.Relocate(newPos);
        }
    }

    private:
        Position newPos;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_paragon_jump_to_center::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// Mutate Selector 143336
class spell_paragon_mutate_selector : public SpellScript
{
    PrepareSpellScript(spell_paragon_mutate_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIdx].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 3 : 1;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() >= targetsCount)
        {
            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 1; // min 1, max 3
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 14)
                targetsCount++;

            if (totalPlayersCount > 19)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_mutate_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_mutate_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Vicious Assault Rapture 143979
class spell_paragon_vicious_assault_rapture : public SpellScript
{
    PrepareSpellScript(spell_paragon_vicious_assault_rapture);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && !GetCaster()->HasInArc(M_PI / 4, target->ToPlayer()); });
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && GetCaster()->GetExactDist2d(target->ToPlayer()) > 10.0f; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_vicious_assault_rapture::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_vicious_assault_rapture::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Reaction: Blue 142735
class spell_paragon_reaction_blue : public SpellScript
{
    PrepareSpellScript(spell_paragon_reaction_blue);

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
        if (Unit* caster = GetCaster())
        {
            uint8 maxSplitTargets = caster->GetMap()->Is25ManRaid() ? 5 : 2;

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                maxSplitTargets = 2; // min 2, max 5
                uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

                if (totalPlayersCount > 13)
                    maxSplitTargets++;

                if (totalPlayersCount > 17)
                    maxSplitTargets++;

                if (totalPlayersCount > 21)
                    maxSplitTargets++;
            }

            if (targetsCount > maxSplitTargets)
                targetsCount = maxSplitTargets;

            if (targetsCount > 0)
                SetHitDamage(GetHitDamage() / targetsCount);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_reaction_blue::CheckTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_paragon_reaction_blue::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Mesmerize 142668
class spell_paragon_mesmerize : public SpellScript
{
    PrepareSpellScript(spell_paragon_mesmerize);

    std::list<WorldObject*> m_targets;
    std::list<ObjectGuid> kunchlongList;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (kunchlongList.empty())
            return;

        if (Unit* caster = GetCaster())
        {
            ObjectGuid kunchlongGUID = Trinity::Containers::SelectRandomContainerElement(kunchlongList);

            if (Creature* hungryKunchlong = ObjectAccessor::GetCreature(*caster, kunchlongGUID))
            {
                if (Unit* target = GetHitUnit())
                {
                    hungryKunchlong->AI()->SetGUID(target->GetGUID());
                    hungryKunchlong->CastSpell(target, SPELL_DEVOUR_P, false);
                    hungryKunchlong->CastSpell(target, SPELL_MESMERIZE_AURA, false);
                    hungryKunchlong->CastSpell(target, SPELL_DEVOUR, false);
                }
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; }); // exlude pets
        targets.remove_if(TankSpecTargetSelector());

        if (targets.size() >= 1)
        {
            Trinity::Containers::RandomResizeList(targets, 1);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; }); // exlude pets

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void SelectKunchlongTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_HUNGRY_KUNCHONG; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->HasAura(SPELL_MOLT); });

        for (auto&& itr : targets)
            kunchlongList.push_back(itr->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_mesmerize::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_mesmerize::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_mesmerize::SelectKunchlongTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Fiery Edge Beam 142811
class spell_paragon_fiery_edge_beam : public AuraScript
{
    PrepareAuraScript(spell_paragon_fiery_edge_beam);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetOwner()->ToUnit())
                caster->CastSpell(target, SPELL_FIERY_EDGE_AURA, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_paragon_fiery_edge_beam::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Fiery Edge Periodic 142808
class spell_paragon_fiery_edge_periodic : public AuraScript
{
    PrepareAuraScript(spell_paragon_fiery_edge_periodic);

    void OnPeriodic(AuraEffect const* aurEff)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetOwner()->ToUnit())
            {
                int32 effValue = sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, caster->GetMap()->GetDifficulty())->Effects[0].BasePoints;
                caster->CastCustomSpell(target, SPELL_FIERY_EDGE_EFF, 0, &effValue, 0, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_paragon_fiery_edge_periodic::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class FieryEdgePredicate 
{
    public:
        FieryEdgePredicate(Unit* const caster, Unit* const target) : _caster(caster), _target(target) { }

        bool operator()(WorldObject* object)
        {
            return object && object->ToPlayer() && (!object->ToPlayer()->IsInBetween(_caster, _target, 1.0f) || object->ToPlayer()->GetGUID() == _caster->GetGUID() || object->ToPlayer()->GetGUID() == _target->GetGUID());

            return false;
        }

    private:
        Unit const* _caster;
        Unit const* _target;
};

// Fiery Edge Eff 142809
class spell_paragon_fiery_edge_eff : public SpellScript
{
    PrepareSpellScript(spell_paragon_fiery_edge_eff);

    bool Load() override
    {
        linkedTargetGUID = ObjectGuid::Empty;
        return true;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Try find our linked target (for dead targets)
        for (auto&& itr : targets)
            if (itr->ToPlayer() && itr->ToPlayer()->HasAura(SPELL_FIERY_EDGE_AURA, GetCaster()->GetGUID()))
                linkedTargetGUID = itr->GetGUID();

        if (Unit* target = ObjectAccessor::GetUnit(*GetCaster(), getLinkedTargetGUID()))
            targets.remove_if(FieryEdgePredicate(GetCaster(), target));
    }

    ObjectGuid getLinkedTargetGUID() // we can`t get it from first hook cuz it`s not synch
    {
        std::list<Player*> targetsList;
        GetPlayerListInGrid(targetsList, GetCaster(), 300.0f);

        targetsList.remove_if([=](Player* target) { return target && !target->HasAura(SPELL_FIERY_EDGE_AURA); });

        for (auto&& itr : targetsList)
            if (itr->HasAura(SPELL_FIERY_EDGE_AURA, GetCaster()->GetGUID()))
                return itr->GetGUID();

        if (linkedTargetGUID)
            return linkedTargetGUID;

        return ObjectGuid::Empty;
    }

    private:
        ObjectGuid linkedTargetGUID;

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_fiery_edge_eff::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// Encase in Amber 142564
class spell_paragon_encase_in_amber : public AuraScript
{
    PrepareAuraScript(spell_paragon_encase_in_amber);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetUnitOwner())
        {
            owner->StopMoving();
            owner->GetMotionMaster()->MovementExpired();
            owner->CastSpell(owner, GetSpellInfo()->Effects[EFFECT_0].BasePoints, false);
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetOwner() && GetOwner()->ToCreature() && GetOwner()->ToCreature()->GetVictim() && GetOwner()->ToCreature()->GetEntry() != NPC_HISEK_THE_SWARMKEEPER && GetOwner()->ToCreature()->GetEntry() != NPC_KAZTIK_THE_MANIPULATOR)
            GetOwner()->ToCreature()->RemoveChanneledCast(GetOwner()->ToCreature()->GetVictim()->GetGUID()); // restore movement
        else if (GetOwner() && GetOwner()->ToCreature() && GetOwner()->ToCreature()->GetEntry() != NPC_HISEK_THE_SWARMKEEPER && GetOwner()->ToCreature()->GetEntry() != NPC_KAZTIK_THE_MANIPULATOR)
            GetOwner()->ToCreature()->RemoveChanneledCast(); // same but in case passive phase (mostly for karoz)
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_paragon_encase_in_amber::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_paragon_encase_in_amber::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Feed 143362
class spell_paragon_feed : public SpellScript
{
    PrepareSpellScript(spell_paragon_feed);

    std::list<WorldObject*> m_targets, copyTargets;

    bool Load() override
    {
        allowPrevTarget = false;
        return true;
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->SetGUID(target->GetGUID());
                caster->AI()->AttackStart(target);
                caster->SetReactState(REACT_PASSIVE);
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Creature* caster = GetCaster()->ToCreature()) // check our target IsAlive or not
        {
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                if (target->IsAlive())
                    allowPrevTarget = true;

            m_targets = targets;
            targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; }); // exlude pets

            if (allowPrevTarget)
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && caster->AI()->GetGUID() != target->ToPlayer()->GetGUID(); });
            else
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_FEED); });

            if (targets.size() < 1)
            {
                targets.clear();

                for (auto&& itr : m_targets)
                    targets.push_back(itr);

                targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; }); // exlude pets

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                copyTargets = targets;
                return;
            }

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            copyTargets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    private:
        bool allowPrevTarget;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_feed::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_feed::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_feed::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Genetic Modification 143355
class spell_paragon_genetic_modification : public AuraScript
{
    PrepareAuraScript(spell_paragon_genetic_modification);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (GetUnitOwner() && GetUnitOwner()->GetPower(POWER_ENERGY) > 99)
        {
            GetUnitOwner()->SetPower(POWER_ENERGY, 0);
            GetUnitOwner()->SetHealth(GetUnitOwner()->GetMaxHealth());
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_paragon_genetic_modification::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
    }
};

// Multi Shot 144839
class spell_paragon_multi_shot : public SpellScript
{
    PrepareSpellScript(spell_paragon_multi_shot);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());

        uint32 targetsCount = sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, GetCaster()->GetMap()->GetDifficulty())->MaxAffectedTargets;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 3; // min 3, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                targetsCount++;

            if (totalPlayersCount > 20)
                targetsCount++;
        }

        if (targets.size() < targetsCount)
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            {
                // Calculate TargetsCount
                targetsCount = 3; // min 3, max 5
                uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

                if (totalPlayersCount > 15)
                    targetsCount++;

                if (totalPlayersCount > 20)
                    targetsCount++;
            }

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount);

            return;
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_multi_shot::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Point Blank Shot 143505
class spell_paragon_point_blank_shot : public SpellScript
{
    PrepareSpellScript(spell_paragon_point_blank_shot);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_AIM_MARK, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_paragon_point_blank_shot::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// Store Kinetic Energy 143709
class spell_paragon_store_kinetic_energy : public SpellScript
{
    PrepareSpellScript(spell_paragon_store_kinetic_energy)

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->SetData(TYPE_FLASH_DATA, (uint32)caster->GetExactDist2d(target));
                caster->CastSpell(target, SPELL_FLASH_CHARGE, true);
            }
        }
    }

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        // Remove whole exclude casters
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        // Exclude near targets
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && target->ToPlayer()->GetExactDist2d(GetCaster()) < 10.5f; });

        if (!targets.empty())
        {
            // We should try select farthest
            targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
            targets.resize(1);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        // if not found any caster
        if (!m_targets.empty())
        {
            m_targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));
            targets.resize(1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_store_kinetic_energy::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_store_kinetic_energy::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Whirling 143701
class spell_paragon_whirling : public SpellScript
{
    PrepareSpellScript(spell_paragon_whirling)

    std::list<WorldObject*> copyTargets;

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && GetCaster() && GetCaster()->ToCreature() && !target->ToPlayer()->IsInBetween(GetCaster(), &GetCaster()->ToCreature()->GetHomePosition(), 2.5f); });
        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_whirling::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_whirling::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_whirling::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Summon Paragons 141804
class spell_paragon_summon_paragons : public SpellScript
{
    PrepareSpellScript(spell_paragon_summon_paragons);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
        {
            for (auto&& key : invParagonSummonsType)
                caster->CastSpell(key.second.GetPositionX(), key.second.GetPositionY(), key.second.GetPositionZ(), key.first, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_summon_paragons::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Bloodletting Proc 143319
class spell_paragon_bloodletting_proc : public AuraScript
{
    PrepareAuraScript(spell_paragon_bloodletting_proc);

    void HandleOnProc(ProcEventInfo& eventInfo)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_BLOODLETTING_MISSLE_AT, true);
    }

    void Register()
    {
        OnProc += AuraProcFn(spell_paragon_bloodletting_proc::HandleOnProc);
    }
};

// Snipe 143217
class spell_paragon_snipe : public SpellScript
{
    PrepareSpellScript(spell_paragon_snipe);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Unit* unit = GetHitUnit())
        {
            // full damage after 45y distance
            float dist = unit->GetExactDist2d(GetCaster());

            if (dist > 45.0f) // same eff
                dist = 45.0f;

            int32 reduction = (int32)((GetSpellInfo()->Effects[eff_idx].BasePoints * dist) / 45.0f);
            SetHitDamage(reduction);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_snipe::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Tenderizing Strikes 142929
class spell_paragon_tenderizing_strikes : public SpellScript
{
    PrepareSpellScript(spell_paragon_tenderizing_strikes);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* killruk = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_KILRUK_THE_WIND_REAVER) : ObjectGuid::Empty))
            if (killruk->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                PreventHitAura();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_tenderizing_strikes::HandleEffectHit, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// Exposed Veins 142931
class spell_paragon_exposed_veins : public SpellScript
{
    PrepareSpellScript(spell_paragon_exposed_veins);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* killruk = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_XARIL_THE_POISONED_MIND) : ObjectGuid::Empty))
            if (killruk->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                PreventHitAura();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_exposed_veins::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Hewn 143275
class spell_paragon_hewn : public SpellScript
{
    PrepareSpellScript(spell_paragon_hewn);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* killruk = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_RIKKAL_THE_DISSECTOR) : ObjectGuid::Empty))
            if (killruk->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                PreventHitAura();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_hewn::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Genetic Alteration 143279
class spell_paragon_genetic_alteration : public SpellScript
{
    PrepareSpellScript(spell_paragon_genetic_alteration);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* killruk = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_SKEER_THE_BLOODSEEKER) : ObjectGuid::Empty))
            if (killruk->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                PreventHitAura();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_genetic_alteration::HandleEffectHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Kunchlong Energize 142645
class spell_paragon_kunchlong_energize : public SpellScript
{
    PrepareSpellScript(spell_paragon_kunchlong_energize);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->GetPower(POWER_ENERGY) > 94)
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_kunchlong_energize::HandleEffectHit, EFFECT_0, SPELL_EFFECT_ENERGIZE);
    }
};

// Hurl Amber Selector 143761
class spell_paragon_hurl_amber_selector : public SpellScript
{
    PrepareSpellScript(spell_paragon_hurl_amber_selector);

    std::list<WorldObject*> m_targets;

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            return;
        }

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_hurl_amber_selector::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_hurl_amber_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Hurl Amber 143759
class spell_paragon_hurl_amber_launch : public AuraScript
{
    PrepareAuraScript(spell_paragon_hurl_amber_launch);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetOwner() && GetOwner()->ToUnit() && (!GetOwner()->ToUnit()->GetMap()->Is25ManRaid() || GetOwner()->ToUnit()->GetMap()->IsHeroic()))
            SetDuration(2 * IN_MILLISECONDS); // in 10 ppl mode it have launch 2 ambers (instead of 3 in 25 ppl mode) - In heroic also 2 in both
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_paragon_hurl_amber_launch::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Devour Instakill 142638
class spell_paragon_devour_instakill : public SpellScript
{
    PrepareSpellScript(spell_paragon_devour_instakill);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (GetCaster() && GetHitUnit() && GetCaster()->ToCreature())
        {
            GetCaster()->ToCreature()->SetPower(POWER_ENERGY, 100);
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_START_INTRO);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_devour_instakill::HandleEffectHit, EFFECT_0, SPELL_EFFECT_INSTAKILL);
    }
};

// Bloodletting Summon 143283
class spell_paragon_bloodletting_summon : public SpellScript
{
    PrepareSpellScript(spell_paragon_bloodletting_summon);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_PARAGONS_OF_THE_KLAXXI) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_paragon_bloodletting_summon::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Prey 144286
class spell_paragon_prey : public SpellScript
{
    PrepareSpellScript(spell_paragon_prey);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetMap()->IsHeroic())
            GetCaster()->RemoveAurasDueToSpell(SPELL_FAULTY_MUTATION);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_prey::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_INSTAKILL);
    }
};

// Rapid Fire 143243
class spell_paragon_rapid_rife : public AuraScript
{
    PrepareAuraScript(spell_paragon_rapid_rife);

    bool Load() override
    {
        sonicEntry = SPELL_SONIC_PULSE_1;
        keyId = 1;
        return true;
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            auto key = invSonicType.find(keyId);
            owner->PrepareChanneledCast(key->second, sonicEntry, true);

            sonicEntry++;
            keyId++;

            // 143235 is nox poison from Fallen Protectors
            if (sonicEntry == 143235)
                sonicEntry++;
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        // Additional tick for hole
        if (Creature* owner = GetOwner()->ToCreature())
            owner->PrepareChanneledCast(3.648f, SPELL_SONIC_PULSE_12, true);
    }

    private:
        uint32 sonicEntry;
        uint32 keyId;

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_paragon_rapid_rife::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_paragon_rapid_rife::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Faulty Mutation 148589
class spell_paragon_faulty_mutation : public AuraScript
{
    PrepareAuraScript(spell_paragon_faulty_mutation);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_FAULTY_MUTATION_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_paragon_faulty_mutation::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Sonic Pulse 143226, 143227, 143228, 143229, 143230, 143231, 143232, 143233, 143234, 143236, 143237, 143238
class spell_paragon_sonic_pulse : public SpellScript
{
    PrepareSpellScript(spell_paragon_sonic_pulse);

    void SelectLeftLong(SpellDestination& dest)
    {
        if (Unit* caster = GetCaster())
        {
            float tempAmount = Position::NormalizeOrientation(caster->GetOrientation() + M_PI / 12);
            GetPositionWithDistInOrientation(caster, 0.5f, tempAmount, x, y);
            newPos = { x, y, dest._position.GetPositionZ() + 2.0f, tempAmount };
            dest.Relocate(newPos);
        }
    }

    void SelectLeftLow(SpellDestination& dest)
    {
        if (Unit* caster = GetCaster())
        {
            float tempAmount = Position::NormalizeOrientation(caster->GetOrientation() + M_PI / 24);
            GetPositionWithDistInOrientation(caster, 0.5f, tempAmount, x, y);
            newPos = { x, y, dest._position.GetPositionZ() + 2.0f, tempAmount };
            dest.Relocate(newPos);
        }
    }

    void SelectRightLow(SpellDestination& dest)
    {
        if (Unit* caster = GetCaster())
        {
            float tempAmount = Position::NormalizeOrientation(caster->GetOrientation() - M_PI / 24);
            GetPositionWithDistInOrientation(caster, 0.5f, tempAmount, x, y);
            newPos = { x, y, dest._position.GetPositionZ() + 2.0f, tempAmount };
            dest.Relocate(newPos);
        }
    }

    void SelectRightLong(SpellDestination& dest)
    {
        if (Unit* caster = GetCaster())
        {
            float tempAmount = Position::NormalizeOrientation(caster->GetOrientation() - M_PI / 12);
            GetPositionWithDistInOrientation(caster, 0.5f, tempAmount, x, y);
            newPos = { x, y, dest._position.GetPositionZ() + 2.0f, tempAmount };
            dest.Relocate(newPos);
        }
    }

    private:
        Position newPos;
        float x, y;

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_paragon_sonic_pulse::SelectLeftLong, EFFECT_0, TARGET_DEST_CASTER_FRONT);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_paragon_sonic_pulse::SelectLeftLow, EFFECT_1, TARGET_DEST_CASTER_FRONT);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_paragon_sonic_pulse::SelectRightLow, EFFECT_3, TARGET_DEST_CASTER_FRONT);
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_paragon_sonic_pulse::SelectRightLong, EFFECT_4, TARGET_DEST_CASTER_FRONT);
    }
};

// Reave 142228
class spell_paragon_reave_eff_heroic : public SpellScript
{
    PrepareSpellScript(spell_paragon_reave_eff_heroic);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* unit = GetHitUnit())
        {
            // 60% damage reduction after 35 yards distance
            float dist = unit->GetExactDist2d(GetCaster());

            int32 reduction = dist >= 35 ? GetHitDamage() * 0.3 : GetHitDamage() * (1 - ((dist * 2) / 100));
            SetHitDamage(reduction);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_reave_eff_heroic::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Reave Selector 148677
class spell_paragon_reave_selector : public SpellScript
{
    PrepareSpellScript(spell_paragon_reave_selector);

    std::list<WorldObject*> m_targets;

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->PrepareChanneledCast(caster->GetOrientation());

            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.empty())
        {
            targets.clear();

            for (auto&& itr : m_targets)
                targets.push_back(itr);

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            return;
        }

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_reave_selector::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_reave_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Reave Launcher 148681
class spell_paragon_reave_launcher : public SpellScript
{
    PrepareSpellScript(spell_paragon_reave_launcher);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->PrepareChanneledCast(caster->GetOrientation(), SPELL_REAVE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_reave_launcher::HandleEffectHit, EFFECT_0, SPELL_EFFECT_JUMP);
    }
};

// Reave Heroic 148676
class spell_paragon_reave_heroic : public AuraScript
{
    PrepareAuraScript(spell_paragon_reave_heroic);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_REMOVE_REAVE);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_paragon_reave_heroic::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Volatile Poultice 142598
class spell_paragon_volatile_poultice : public AuraScript
{
    PrepareAuraScript(spell_paragon_volatile_poultice);

    void HandleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetHeal() && eventInfo.GetActor() && !eventInfo.GetActor()->HasAura(SPELL_VOLATILE_POULTICE_STORAGE))
        {
            int32 healAbsorb = (int32)eventInfo.GetHeal();

            if (Unit* owner = GetOwner()->ToUnit())
                if (Unit* target = eventInfo.GetActor())
                    if (Aura* storage = owner->AddAura(SPELL_VOLATILE_POULTICE_STORAGE, target))
                        storage->GetEffect(EFFECT_0)->SetAmount(healAbsorb);
        }
    }

    void Register()
    {
        OnEffectProc += AuraEffectProcFn(spell_paragon_volatile_poultice::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Volatile Poultice Storage 142877
class spell_paragon_volatile_poultice_storage : public AuraScript
{
    PrepareAuraScript(spell_paragon_volatile_poultice_storage);

    void HandleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        int32 delayedHeal = aurEff->GetAmount();

        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->CastCustomSpell(owner, SPELL_VOLATILE_POULTICE_HEAL_EFF, &delayedHeal, 0, 0, true);
            owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
        }
    }

    void Register()
    {
        OnEffectProc += AuraEffectProcFn(spell_paragon_volatile_poultice_storage::HandleOnProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// Player Reave 142270
class spell_paragon_player_reave : public SpellScript
{
    PrepareSpellScript(spell_paragon_player_reave);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_REAVE_PLAYER_EFF, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_paragon_player_reave::HandleEffectHit, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// Player Reave Eff 142272
class spell_paragon_player_reave_eff : public SpellScript
{
    PrepareSpellScript(spell_paragon_player_reave_eff);

    void HandleEffectHitDamage(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
        {
            int32 damageEff = (int32)caster->GetMaxHealth();
            SetHitDamage(damageEff);
        }
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Creature* target = GetHitCreature())
                caster->Kill(target);
    }

    void SelectTarget(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_HUNGRY_KUNCHONG; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_player_reave_eff::HandleEffectHitDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_paragon_player_reave_eff::HandleEffectHit, EFFECT_1, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paragon_player_reave_eff::SelectTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Sting 144277, Claw 144274, Swipe 144275, Fiery Tail 144182
class spell_paragon_mutation_ability_eff : public SpellScript
{
    PrepareSpellScript(spell_paragon_mutation_ability_eff);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Player* caster = GetCaster()->ToPlayer())
        {
            std::vector<int32> statValue = { int32(caster->GetUInt32Value(UNIT_FIELD_ATTACK_POWER)), int32(caster->GetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER)), int32(caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE)) };
            auto it = std::max_element(statValue.begin(), statValue.end());
            int32 value = *it;

            // Ranged Attack Power and Spell Power has x2 multiplicity
            if (std::distance(statValue.begin(), it) != 0)
                value = value * 2;

            if (GetSpellInfo()->Id == 144182 || GetSpellInfo()->Id == 144275) // 150%
                value = value * 1.5;

            SetHitDamage(value);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_paragon_mutation_ability_eff::HandleEffectHit, EFFECT_0, SPELL_EFFECT_NORMALIZED_WEAPON_DMG);
    }
};

// Encase in Amber 142618
class spell_paragon_encase_in_amber_player : public AuraScript
{
    PrepareAuraScript(spell_paragon_encase_in_amber_player);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_AIM);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_paragon_encase_in_amber_player::OnAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 148652, 148649, 148654, 148653, 148650, 148655, 148651, 148648, 148656
class spell_paragons_award : public AuraScript
{
    PrepareAuraScript(spell_paragons_award);

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Player* target = GetTarget()->ToPlayer();
        if (!target)
            return;

        if (CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(GetSpellInfo()->Effects[EFFECT_0].BasePoints))
            target->SetTitle(title);

        if (auto creat = sObjectMgr->GetCreatureTemplate(GetSpellInfo()->Effects[EFFECT_0].MiscValue))
            target->SetDisplayId(creat->GetModelByIdx(0)->CreatureDisplayID);

    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Player* target = GetTarget()->ToPlayer();
        if (!target)
            return;

        if (CharTitlesEntry const* title = sCharTitlesStore.LookupEntry(GetSpellInfo()->Effects[EFFECT_0].BasePoints))
            target->SetTitle(title, true);

        target->RestoreDisplayId();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_paragons_award::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_paragons_award::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Explosive Ring Visual 144191, 144192, 144193, 144194
class spell_paragon_explosive_ring_visual : public SpellScript
{
    PrepareSpellScript(spell_paragon_explosive_ring_visual);

    void SelectDest(SpellDestination& dest)
    {
        dest._position.RelocateOffset(0.0f, GetSpellInfo()->Effects[EFFECT_0].MaxRadiusEntry->RadiusMax);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_paragon_explosive_ring_visual::SelectDest, EFFECT_0, TARGET_DEST_CASTER);
    }
};

// 1044. Summoned by 143765 - Sonic Projection
class sat_klaxxi_sonic_projection : public IAreaTriggerAura
{
    std::list<uint64> targetGUIDs;

    void OnInit() override
    {
        targetGUIDs.clear();

        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (AreaTrigger* at = GetAreaTrigger())
            {
                float angle = caster->GetOrientation();

                std::vector<Position> path;
                path.push_back(at->GetPosition());

                for (int32 i = 1; i < 21; ++i)
                {
                    Position newPos = path.back();
                    float x = at->GetPositionX() + (2.0f * i * std::cos(angle));
                    float y = at->GetPositionY() + (2.0f * i * std::sin(angle));

                    float z = at->GetPositionZ();
                    at->UpdateAllowedPositionZ(x, y, z);
                    path.push_back({ x, y, z, angle });
                }

                at->InitSpline(path, at->GetDuration());
            }
        }
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 9000 && std::find(targetGUIDs.begin(), targetGUIDs.end(), triggering->ToPlayer()->GetGUID()) == targetGUIDs.end();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                caster->CastSpell(itr, SPELL_SONIC_PROJECTION_EFF, true);
                targetGUIDs.push_back(itr->GetGUID());
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering)  override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SONIC_PROJECTION_EFF);
    }
};

// 998. Summoned by 142737 - Reaction: Yellow
class sat_paragon_reaction_yellow : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetAreaTrigger()->GetDuration() < 28000;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_REACTION_YELLOW_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_REACTION_YELLOW_EFF);
    }
};

// 996. Summoned by 142630 - Hungry
class sat_paragon_hungry : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->HasAura(SPELL_THICK_SHELL);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->Kill(target);
    }
};

// 1039. Summoned by 143740 - Caustic Amber
class sat_paragon_caustic_amber : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_CAUSTIC_AMBER_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_CAUSTIC_AMBER_EFF);
    }
};

// 1037. Summoned by 143701 - Whirling
class sat_paragon_whirling : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_WHIRLING_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_WHIRLING_AT_EFF);
    }
};

// 1022. Summoned by 143321 - Bloodletting Orb
class sat_paragon_bloodletting_orb : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && object->ToPlayer()->GetHealthPct() != 100.0f;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_REGENERATION, true);
    }
};

// 1012, 1014, 1015, 1026, 1027. Summoned by 143226...143238 - Sonic Pulse
class sat_paragon_sonic_pulse : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnInit() override
    {
        if (AreaTrigger* at = GetAreaTrigger())
        {
            float angle = at->GetOrientation();

            std::vector<Position> path;

            path.push_back(at->GetPosition());

            for (int32 i = 1; i < 6; ++i)
            {
                Position newPos = path.back();
                float x = at->GetPositionX() + (16.0f * i * std::cos(angle));
                float y = at->GetPositionY() + (16.0f * i * std::sin(angle));

                float z = at->GetPositionZ();
                at->UpdateAllowedPositionZ(x, y, z);
                path.push_back({ x, y, z + 1.5f });
            }

            int32 time = at->GetDuration() / 2.5; // first wave has duration a lower than 13s (instead 32 in dbc, another faster)
            at->InitSpline(path, time);
        }
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_SONIC_PULSE_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_SONIC_PULSE_EFF);
    }
};

// 1335. Summoned by 148676 - Reave
class sat_paragon_reave : public IAreaTriggerAura
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
                target->SendApplyMovementForce(true, forceMovementPos, 5.5f);
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

// 1033. Summoned by 143575 - Canned Heat
class sat_paragon_canned_heat : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_CANNED_HEAT);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
            if (Player* target = triggering->ToPlayer())
                caster->CastSpell(target, SPELL_CANNED_HEAT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_CANNED_HEAT_EFF);
    }
};

// 1001. Summoned by 142812 - Eerie Fog
class sat_paragon_eerie_fog : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_EERIE_FOG_DELAY);
    }

    void OnInit() override
    {
        GetAreaTrigger()->AttachToTarget();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* caster = GetCaster())
        {
            if (Player* target = triggering->ToPlayer())
            {
                caster->CastSpell(target, SPELL_EERIE_FOG_EFF, true);
                caster->CastSpell(target, SPELL_CHILED_TO_BONE, true);
            }
        }
    }
};

// Areatrigger 9320.
class AreaTrigger_at_soo_forward_paragon_of_the_klaxxi : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_forward_paragon_of_the_klaxxi() : AreaTriggerScript("AreaTrigger_at_soo_forward_paragon_of_the_klaxxi") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (Creature* paragonController = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PARAGON_OF_THE_KLAXXI) : ObjectGuid::Empty))
                paragonController->AI()->DoAction(ACTION_START_INTRO);

            return true;
        }
};

const std::map<uint32, float> invExplosiveRing =
{
    { SPELL_EXPLOSIVE_RING_AT_25, 25.0f },
    { SPELL_EXPLOSIVE_RING_AT_15, 15.0f },
    { SPELL_EXPLOSIVE_RING_AT_20, 20.0f },
    { SPELL_EXPLOSIVE_RING_AT_30, 30.0f },
};

// 1002, 1003, 1004, 1006. Summoned by 142798, 142799, 142800, 142802  - Explosive Ring
class sat_paragon_explosive_ring : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* object) override
    {
        if (!object->ToPlayer() || !object->ToPlayer()->IsAlive())
            return false;

        if (std::abs(object->GetPositionZ() - GetTarget()->GetPositionZ()) > 10.0f)
            return false;

        float coeff = (invExplosiveRing.find(GetSpellInfo()->Id)->second * 1000.0f) / GetAreaTrigger()->GetMaxDuration();
        float dist = (GetAreaTrigger()->GetMaxDuration() - GetAreaTrigger()->GetDuration()) * coeff / 1000.0f;

        if (GetTarget()->GetExactDist2d(object) - object->GetObjectSize() > dist)
            return false;

        if (!GetCaster()->CanSeeOrDetect(object))
            return false;

        if (GetCaster() && GetCaster()->GetGUID() == object->ToPlayer()->GetGUID()) // also not hit self (invisible for caster)
            return false;

        return GetTarget()->GetExactDist2d(object) - object->GetObjectSize() > (dist - 5.0f);
    }

    void OnTriggeringApply(WorldObject* object) override
    {
        if (Player* target = object->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_EXPLOSIVE_RING_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAura(SPELL_EXPLOSIVE_RING_EFF, GetCaster()->GetGUID(), 0, AURA_REMOVE_BY_EXPIRE);
    }
};

void AddSC_paragons_of_the_klaxxi()
{
    new boss_paragon_of_the_klaxxi();
    new creature_script<boss_kilruk_the_wind_reaver>("boss_kilruk_the_wind_reaver");
    new creature_script<boss_xaril_the_poisoned_mind>("boss_xaril_the_poisoned_mind");
    new creature_script<boss_kaztik_the_manipulator>("boss_kaztik_the_manipulator");
    new creature_script<boss_korven_the_prime>("boss_korven_the_prime");
    new creature_script<boss_iyyokuk_the_lucid>("boss_iyyokuk_the_lucid");
    new creature_script<boss_karoz_the_locust>("boss_karoz_the_locust");
    new creature_script<boss_skeer_the_bloodseeker>("boss_skeer_the_bloodseeker");
    new creature_script<boss_rikkal_the_dissector>("boss_rikkal_the_dissector");
    new creature_script<boss_hisek_the_swarmkeeper>("boss_hisek_the_swarmkeeper");
    new creature_script<npc_paragon_mantid_amber>("npc_paragon_mantid_amber");
    new creature_script<npc_soo_paragon_hungry_kunchong>("npc_soo_paragon_hungry_kunchong");
    new creature_script<npc_blood>("npc_blood");
    new creature_script<npc_soo_paragon_amber>("npc_soo_paragon_amber");
    new creature_script<npc_paragon_amber_parasite>("npc_paragon_amber_parasite");
    new creature_script<npc_paragon_amber_bomb>("npc_paragon_amber_bomb");
    new creature_script<npc_paragon_eerie_fog>("npc_paragon_eerie_fog");

    new spell_script<spell_soo_paragon_purpose>("spell_soo_paragon_purpose");
    new aura_script<spell_soo_cosmetic_for_fight_starter>("spell_soo_cosmetic_for_fight_starter");
    new spell_script<spell_soo_paragon_berserk>("spell_soo_paragon_berserk");
    new aura_script<spell_soo_paragon_gouge>("spell_soo_paragon_gouge");
    new spell_script<spell_soo_paragon_toxic_injection>("spell_soo_paragon_toxic_injection");
    new spell_script<spell_paragon_caustic_blood>("spell_paragon_caustic_blood");
    new aura_script<spell_paragon_caustic_blood_aura>("spell_paragon_caustic_blood_aura");
    new spell_script<spell_soo_catalyst_react_blue>("spell_soo_catalyst_react_blue");
    new spell_script<spell_soo_catalyst_react_red>("spell_soo_catalyst_react_red");
    new spell_script<spell_soo_catalyst_react_yellow>("spell_soo_catalyst_react_yellow");
    new spell_script<spell_soo_catalyst_react_purple>("spell_soo_catalyst_react_purple");
    new spell_script<spell_soo_catalyst_react_orange>("spell_soo_catalyst_react_orange");
    new spell_script<spell_soo_catalyst_react_green>("spell_soo_catalyst_react_green");
    new spell_script<spell_soo_paragon_shield_bash>("spell_soo_paragon_shield_bash");
    new aura_script<spell_soo_vicious_assault>("spell_soo_vicious_assault");
    new spell_script<spell_soo_bloodletting>("spell_soo_bloodletting");
    new spell_script<spell_paragon_injection>("spell_paragon_injection");
    new aura_script<spell_paragon_injection_aura>("spell_paragon_injection_aura");
    new spell_script<spell_soo_paragon_diminish>("spell_soo_paragon_diminish");
    new spell_script<spell_soo_insane_calculation_fiery_edge>("spell_soo_insane_calculation_fiery_edge");
    new aura_script<spell_soo_aim_aura>("spell_soo_aim_aura");
    new spell_script<spell_soo_aim>("spell_soo_aim");
    new spell_script<spell_soo_aim_resonance>("spell_soo_aim_resonance");
    new spell_script<spell_soo_amber_regeneration>("spell_soo_amber_regeneration");
    new aura_script<spell_soo_devour>("spell_soo_devour");
    new spell_script<spell_paragon_jump_to_center>("spell_paragon_jump_to_center");
    new spell_script<spell_paragon_mutate_selector>("spell_paragon_mutate_selector");
    new spell_script<spell_paragon_vicious_assault_rapture>("spell_paragon_vicious_assault_rapture");
    new spell_script<spell_paragon_reaction_blue>("spell_paragon_reaction_blue");
    new spell_script<spell_paragon_mesmerize>("spell_paragon_mesmerize");
    new aura_script<spell_paragon_fiery_edge_beam>("spell_paragon_fiery_edge_beam");
    new aura_script<spell_paragon_fiery_edge_periodic>("spell_paragon_fiery_edge_periodic");
    new spell_script<spell_paragon_fiery_edge_eff>("spell_paragon_fiery_edge_eff");
    new aura_script<spell_paragon_encase_in_amber>("spell_paragon_encase_in_amber");
    new spell_script<spell_paragon_feed>("spell_paragon_feed");
    new aura_script<spell_paragon_genetic_modification>("spell_paragon_genetic_modification");
    new spell_script<spell_paragon_multi_shot>("spell_paragon_multi_shot");
    new spell_script<spell_paragon_point_blank_shot>("spell_paragon_point_blank_shot");
    new spell_script<spell_paragon_store_kinetic_energy>("spell_paragon_store_kinetic_energy");
    new spell_script<spell_paragon_whirling>("spell_paragon_whirling");
    new spell_script<spell_paragon_summon_paragons>("spell_paragon_summon_paragons");
    new spell_script<spell_paragon_snipe>("spell_paragon_snipe");
    new aura_script<spell_paragon_bloodletting_proc>("spell_paragon_bloodletting_proc");
    new spell_script<spell_paragon_tenderizing_strikes>("spell_paragon_tenderizing_strikes");
    new spell_script<spell_paragon_exposed_veins>("spell_paragon_exposed_veins");
    new spell_script<spell_paragon_hewn>("spell_paragon_hewn");
    new spell_script<spell_paragon_genetic_alteration>("spell_paragon_genetic_alteration");
    new spell_script<spell_paragon_kunchlong_energize>("spell_paragon_kunchlong_energize");
    new spell_script<spell_paragon_hurl_amber_selector>("spell_paragon_hurl_amber_selector");
    new aura_script<spell_paragon_hurl_amber_launch>("spell_paragon_hurl_amber_launch");
    new spell_script<spell_paragon_devour_instakill>("spell_paragon_devour_instakill");
    new spell_script<spell_paragon_bloodletting_summon>("spell_paragon_bloodletting_summon");
    new spell_script<spell_paragon_prey>("spell_paragon_prey");
    new aura_script<spell_paragon_rapid_rife>("spell_paragon_rapid_rife");
    new aura_script<spell_paragon_faulty_mutation>("spell_paragon_faulty_mutation");
    new spell_script<spell_paragon_sonic_pulse>("spell_paragon_sonic_pulse");
    new spell_script<spell_paragon_reave_eff_heroic>("spell_paragon_reave_eff_heroic");
    new spell_script<spell_paragon_reave_selector>("spell_paragon_reave_selector");
    new spell_script<spell_paragon_reave_launcher>("spell_paragon_reave_launcher");
    new aura_script<spell_paragon_reave_heroic>("spell_paragon_reave_heroic");
    new aura_script<spell_paragon_volatile_poultice>("spell_paragon_volatile_poultice");
    new aura_script<spell_paragon_volatile_poultice_storage>("spell_paragon_volatile_poultice_storage");
    new spell_script<spell_paragon_player_reave>("spell_paragon_player_reave");
    new spell_script<spell_paragon_player_reave_eff>("spell_paragon_player_reave_eff");
    new spell_script<spell_paragon_mutation_ability_eff>("spell_paragon_mutation_ability_eff");
    new aura_script<spell_paragon_encase_in_amber_player>("spell_paragon_encase_in_amber_player");
    new aura_script<spell_paragons_award>("spell_paragons_award");
    new spell_script<spell_paragon_explosive_ring_visual>("spell_paragon_explosive_ring_visual");
    new atrigger_script<sat_klaxxi_sonic_projection>("sat_klaxxi_sonic_projection");
    new atrigger_script<sat_paragon_reaction_yellow>("sat_paragon_reaction_yellow");
    new atrigger_script<sat_paragon_hungry>("sat_paragon_hungry");
    new atrigger_script<sat_paragon_caustic_amber>("sat_paragon_caustic_amber");
    new atrigger_script<sat_paragon_whirling>("sat_paragon_whirling");
    new atrigger_script<sat_paragon_bloodletting_orb>("sat_paragon_bloodletting_orb");
    new atrigger_script<sat_paragon_sonic_pulse>("sat_paragon_sonic_pulse");
    new atrigger_script<sat_paragon_reave>("sat_paragon_reave");
    new atrigger_script<sat_paragon_canned_heat>("sat_paragon_canned_heat");
    new atrigger_script<sat_paragon_eerie_fog>("sat_paragon_eerie_fog");
    new atrigger_script<sat_paragon_explosive_ring>("sat_paragon_explosive_ring");
    new AreaTrigger_at_soo_forward_paragon_of_the_klaxxi();
};
