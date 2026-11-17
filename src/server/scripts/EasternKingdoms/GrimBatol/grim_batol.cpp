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

#include "ScriptPCH.h"
#include "grim_batol.h"
    
enum Creatures
{
    NPC_ASCENDED_FLAMESEEKER         = 39415,
    NPC_ASCENDED_ROCKBREAKER         = 40272,
    NPC_FISSURE                      = 41091,
    NPC_ASCENDED_WATERLASHER         = 40273,
    NPC_ASCENDED_WINDWALKER          = 39414,
    NPC_AZUREBORNE_GUARDIAN          = 39854,
    NPC_AZUREBORNE_SEER_1            = 39855,
    NPC_AZUREBORNE_SEER_2            = 40291,
    NPC_AZUREBORNE_WARLORD           = 39909,
    NPC_EMPOWERING_FLAMES            = 41045,
    NPC_ENSLAVED_BURNING_EMBER       = 39892,
    NPC_ENSLAVED_GRONN_BRUTE         = 40166,
    NPC_ENSLAVED_ROCK_ELEMENTAL      = 39900,
    NPC_ENSLAVED_THUNDER_SPIRIT      = 40269,
    NPC_ENSLAVED_WATER_SPIRIT        = 39961,
    NPC_FACELESS_CORRUPTOR           = 39392,
    NPC_HOOKED_NET                   = 48756,
    NPC_NET                          = 42570,
    NPC_TROGG_DWELLER                = 39450,
    NPC_TWILIGHT_ARMSMASTER_1        = 40306,
    NPC_TWILIGHT_ARMSMASTER_2        = 41073,
    NPC_TWILIGHT_BEGUILER            = 40167,
    NPC_TWILIGHT_DRAKE_1             = 41095,
    NPC_TWILIGHT_DRAKE_2             = 39390,
    NPC_TWILIGHT_EARTHSHAPER         = 39890,
    NPC_TWILIGHT_ENFORCER_1          = 40448,
    NPC_TWILIGHT_ENFORCER_2          = 39956,
    NPC_TWILIGHT_FIRECATCHER         = 39870,
    NPC_TWILIGHT_SHADOW_WEAVER       = 39954,
    NPC_KHAAPHOM                     = 40953,
    NPC_TWILIGHT_STORMBREAKER        = 39962,
    NPC_TWILIGHT_THUNDERCALLER       = 40270,
    NPC_TWILIGHT_WAR_MAGE            = 40268,
    NPC_TWILIGHT_WYRMCALLER          = 39873,
    NPC_TWISTED_VISAGE               = 41040,

    
    NPC_FARSEER_THOORANU             = 50385,
    NPC_VELASTRASZA                  = 50390,
    NPC_BALEFLAME                    = 50387,
};

// 94350 summon red drake
enum Spells
{
    // ascended flameseeker
    SPELL_CONFOUNDING_FLAMES         = 76514,
    SPELL_ERUPTING_FIRE              = 76517,

    // ascended rockbreaker
    SPELL_BURNING_FISTS              = 76086,
    SPELL_PETRIFIED_SKIN             = 76792,
    SPELL_ROCK_SMASH                 = 76779,
    SPELL_ROCK_SMASH_DMG             = 76782,
    SPELL_FISSURE_TRIGGER            = 76785,
    SPELL_FISSURE_DMG                = 76786,

    // ascended waterlasher
    SPELL_FOCUSED_GAYSER             = 76797,
    SPELL_ABSORB_THUNDER             = 76095,
    SPELL_LIGHTNING_CLOUD            = 76097, 
    SPELL_LIGHTNING_STRIKE_DMG       = 76101,
    SPELL_WATER_SPOUT                = 76794,

    // ascended windwalker
    SPELL_ABSORB_WATER               = 76029, 
    SPELL_WATER_INFUSIED_BLADES      = 76036, 
    SPELL_TSUNAMI                    = 76045,
    SPELL_WINDWALK                   = 76557,

    // azureborne guardian
    SPELL_ARCANE_INFUSION            = 76378,
    SPELL_ARCANE_SLASH               = 76392,
    SPELL_CURSE_OF_THE_AZUREBORNE    = 76394,

    // azoreborne seer
    SPELL_TWILIGHT_BOLT              = 76369,
    SPELL_TWISTED_ARCANE_TRIGGER     = 79446,
    SPELL_TWISTED_ARCANE             = 76340,
    SPELL_WARPED_TWILIGHT            = 76370,
    SPELL_WARPED_TWILIGHT_DUMMY      = 76373,

    // azureborne warlord
    SPELL_AZURE_BLAST                = 76620,
    SPELL_CONJURE_TWISTED_VISAGE     = 76626,

    // crimsonborne guardian
    SPELL_CRIMSON_CHARGE             = 76404,
    SPELL_CRIMSON_SHOCKWAVE          = 76409,

    // crimsonborne seer
    SPELL_BLAZING_TWILIGHT_SHIELD    = 76314,
    SPELL_BLAZE                      = 76327,
    SPELL_CORRUPTED_FLAME            = 76332,
    
    // crimsonborne warlord
    SPELL_DISARMING_BLAZE            = 76679,
    SPELL_EMPOWERING_TWILIGHT        = 76685,
    SPELL_EMPOWERING_TWILIGHT_AURA   = 76692,
    SPELL_EMPOWERING_TWILIGHT_DMG    = 76693,

    // enslaved burning ember
    SPELL_FLAME_SHOCK                = 90846,

    // enslaved gronn brute
    SPELL_CRUNCH_ARMOR               = 76703,

    // enslaved rock elemental
    SPELL_JAGGED_ROCK_SHIELD         = 76014,

    // faceless corruptor
    SPELL_SIPHON_ESSENSE             = 75755,

    // trogg dweller
    SPELL_CLAW_PUNCTURE              = 76507,

    // twilight armsmaster
    SPELL_FLURRY_OF_BLOWS            = 76729,
    SPELL_MORTAL_STRIKE              = 76727,

    // twilight beguiler
    SPELL_CHAINED_MIND               = 76711,
    SPELL_DECEITFUL_BLAST            = 76715,

    // twilight drake                
    SPELL_TWILIGHT_BREATH            = 76817,

    // twilight earthshaper
    SPELL_EARTH_SPIKE                = 76603,
    SPELL_STONE_SKIN                 = 76596,
    SPELL_SUMMON_ROCK_ELEMENTAL      = 74552,

    // twilight enforcer
    SPELL_DIZZY                      = 76415,
    SPELL_MEAT_GRINDER               = 76411,
    SPELL_MEAT_GRINDER_DMG           = 76413,
    SPELL_MEAT_GRINDER_TRIGGER       = 76414,

    // twilight firecatcher
    SPELL_MOLTEN_BLAST               = 76765,
    SPELL_FLAME_CONDUIT              = 76766,
    SPELL_FLAME_CONDUIT_DMG          = 76768, 
    SPELL_SUMMON_BURNING_EMBER       = 74551,

    // twilight shadow weaver
    SPELL_SHADOW_BOLT                = 76416,
    SPELL_SHADOW_WEAVE_SCRIPT        = 90673, 
    SPELL_SHADOW_WEAVE_DUMMY         = 90674,
    SPELL_SHADOW_WEAVE_DMG           = 90678, 
    SPELL_SUMMON_FELHUNTER           = 76418, 
    SPELL_SPELL_LOCK                 = 40953, // heroic?

    // twilight stormbreaker
    SPELL_WATER_BOLT                 = 76720,
    SPELL_WATER_SHELL                = 90522,
    SPELL_SUMMON_WATER_SPIRIT        = 74561,

    // twilight thunderbreaker
    SPELL_CHAIN_LIGHTNING            = 76578, // 5m
    SPELL_OVERCHARGE                 = 76579,
    SPELL_ELECTRIC_BLAST             = 82973, // +dmg
    SPELL_SUMMON_THUNDER_SPIRIT      = 75096,

    // twilight war-mage
    SPELL_FIRE_ENCHANT               = 76822,
    SPELL_ICE_ENCHANT                = 76823,
    SPELL_POLYMORPH                  = 76826,

    // twilight wyrmcaller
    SPELL_FEED_PET                   = 76816, 
};

enum Events
{
    EVENT_CONFOUNDING_FLAMES         = 1,
    EVENT_ERUPTING_FIRE              = 2,
    EVENT_BURNING_FISTS              = 3,
    EVENT_PETRIFIED_SKIN             = 4,
    EVENT_ROCK_SMASH                 = 5,
    EVENT_FOCUSED_GAYSER             = 6,
    EVENT_ABSORB_THUNDER             = 7,
    EVENT_WATER_SPOUT                = 8,
    EVENT_ABSORB_WATER               = 9,
    EVENT_ARCANE_INFUSION            = 10,
    EVENT_CURSE_OF_THE_AZUREBORNE    = 11,
    EVENT_TWILIGHT_BOLT              = 12,
    EVENT_WARPED_TWILIGHT            = 13,
    EVENT_AZURE_BLAST                = 14,
    EVENT_CONJURE_TWISTED_VISAGE     = 15,
    EVENT_CRIMSON_CHARGE             = 16,
    EVENT_BLAZING_TWILIGHT_SHIELD    = 17,
    EVENT_CORRUPTED_FLAME            = 18,
    EVENT_DISARMING_BLAZE            = 19,
    EVENT_EMPOWERING_TWILIGHT        = 20,
    EVENT_FLAME_SHOCK                = 21,
    EVENT_JAGGED_ROCK_SHIELD         = 22,
    EVENT_SIPHON_ESSENSE             = 23,
    EVENT_CLAW_PUNCTURE              = 24,
    EVENT_MORTAL_STRIKE              = 25,
    EVENT_FLURRY_OF_BLOWS            = 26,
    EVENT_TWILIGHT_BREATH            = 27,
    EVENT_STONE_SKIN                 = 28,
    EVENT_EARTH_SPIKE                = 29,
    EVENT_CHAINED_MIND               = 30,
    EVENT_DECEITFUL_BLAST            = 31,
    EVENT_CRUNCH_ARMOR               = 32,
    EVENT_MEAT_GRINDER               = 33,
    EVENT_FLAME_CONDUIT              = 34,
    EVENT_MOLTEN_BLAST               = 35,
    EVENT_CALL_WYRM                  = 36,
    EVENT_ENCHANT                    = 37,
    EVENT_POLYMORPH                  = 38,
    EVENT_FEED_PET                   = 39,
    EVENT_SHADOW_BOLT                = 40,
    EVENT_SHADOW_WEAVE               = 41,
    EVENT_WATER_BOLT                 = 42,
    EVENT_WATER_SHELL                = 43,
    EVENT_CHAIN_LIGHTNING            = 44,
    EVENT_OVERCHARGE                 = 45,
};

enum Yells
{
    SAY_CALL_WYRM                    = 0,
};

class npc_ascended_flameseeker : public CreatureScript
{
    public:
        npc_ascended_flameseeker() : CreatureScript("npc_ascended_flameseeker") { }

        struct npc_ascended_flameseekerAI : public ScriptedAI
        {
            npc_ascended_flameseekerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CONFOUNDING_FLAMES, urand(5000, 10000));
                events.ScheduleEvent(EVENT_ERUPTING_FIRE, urand(7000, 12000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_CONFOUNDING_FLAMES:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_CONFOUNDING_FLAMES);
                            events.ScheduleEvent(EVENT_CONFOUNDING_FLAMES, urand(7000, 12000));
                            break;
                        case EVENT_ERUPTING_FIRE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_CONFOUNDING_FLAMES);
                            events.ScheduleEvent(EVENT_ERUPTING_FIRE, urand(9000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ascended_flameseekerAI>(creature);
        }
};

class npc_ascended_rockbreaker : public CreatureScript
{
    public:
        npc_ascended_rockbreaker() : CreatureScript("npc_ascended_rockbreaker") { }

        struct npc_ascended_rockbreakerAI : public ScriptedAI
        {
            npc_ascended_rockbreakerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_BURNING_FISTS);
                events.ScheduleEvent(EVENT_BURNING_FISTS, 45000);
                events.ScheduleEvent(EVENT_PETRIFIED_SKIN, urand(5000, 7000));
                events.ScheduleEvent(EVENT_ROCK_SMASH, urand(8000, 12000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_BURNING_FISTS:
                            DoCast(me, SPELL_BURNING_FISTS);
                            events.ScheduleEvent(EVENT_BURNING_FISTS, 45000);
                            break;
                        case EVENT_PETRIFIED_SKIN:
                            DoCast(me->GetVictim(), SPELL_PETRIFIED_SKIN);
                            events.ScheduleEvent(EVENT_PETRIFIED_SKIN, urand(8000, 12000));
                            break;
                        case EVENT_ROCK_SMASH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 1, 0.0f, true))
                                DoCast(target, SPELL_ROCK_SMASH);
                            events.ScheduleEvent(EVENT_ROCK_SMASH, urand(9000, 14000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ascended_rockbreakerAI>(creature);
        }
};

class npc_ascended_waterlasher : public CreatureScript
{
    public:
        npc_ascended_waterlasher() : CreatureScript("npc_ascended_waterlasher") { }

        struct npc_ascended_waterlasherAI : public ScriptedAI
        {
            npc_ascended_waterlasherAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ABSORB_THUNDER, urand(25000, 30000));
                events.ScheduleEvent(EVENT_FOCUSED_GAYSER, urand(5000, 9000));
                events.ScheduleEvent(EVENT_WATER_SPOUT, urand(3000, 10000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_FOCUSED_GAYSER:
                            DoCast(me, SPELL_FOCUSED_GAYSER);
                            events.ScheduleEvent(EVENT_FOCUSED_GAYSER, urand(15000, 20000));
                            break;
                        case EVENT_WATER_SPOUT:
                            DoCast(me->GetVictim(), SPELL_WATER_SPOUT);
                            events.ScheduleEvent(EVENT_WATER_SPOUT, urand(15000, 20000));
                            break;
                        case EVENT_ABSORB_THUNDER:
                            if (Creature* _windwalker = me->FindNearestCreature(NPC_ASCENDED_WINDWALKER,  100.0f, true))
                                DoCast(_windwalker, SPELL_ABSORB_THUNDER);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ascended_waterlasherAI>(creature);
        }
};

class npc_ascended_windwalker: public CreatureScript
{
    public:
        npc_ascended_windwalker() : CreatureScript("npc_ascended_windwalker") { }

        struct npc_ascended_windwalkerAI : public ScriptedAI
        {
            npc_ascended_windwalkerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ABSORB_WATER, urand(25000, 30000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_ABSORB_WATER:
                            if (Creature* _waterlasher = me->FindNearestCreature(NPC_ASCENDED_WATERLASHER,  100.0f, true))
                                DoCast(_waterlasher, SPELL_ABSORB_WATER);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ascended_windwalkerAI>(creature);
        }
};

class npc_azureborne_guardian: public CreatureScript
{
    public:
        npc_azureborne_guardian() : CreatureScript("npc_azureborne_guardian") { }

        struct npc_azureborne_guardianAI : public ScriptedAI
        {
            npc_azureborne_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARCANE_INFUSION, urand(2000, 4000));
                events.ScheduleEvent(EVENT_CURSE_OF_THE_AZUREBORNE, urand(6000, 7000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_ARCANE_INFUSION:
                            DoCast(me, SPELL_ARCANE_INFUSION);
                            events.ScheduleEvent(EVENT_ARCANE_INFUSION, urand(15000, 20000));
                            break;
                        case EVENT_CURSE_OF_THE_AZUREBORNE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_CURSE_OF_THE_AZUREBORNE);
                            events.ScheduleEvent(EVENT_CURSE_OF_THE_AZUREBORNE, urand(10000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_azureborne_guardianAI>(creature);
        }
};

class npc_azureborne_seer: public CreatureScript
{
    public:
        npc_azureborne_seer() : CreatureScript("npc_azureborne_seer") { }

        struct npc_azureborne_seerAI : public ScriptedAI
        {
            npc_azureborne_seerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WARPED_TWILIGHT, urand(15000, 20000));
                events.ScheduleEvent(EVENT_TWILIGHT_BOLT, urand(1000, 2000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->HasAura(SPELL_TWISTED_ARCANE_TRIGGER) &&
                    !me->HasAura(SPELL_TWISTED_ARCANE))
                {
                    DoCast(me, SPELL_TWISTED_ARCANE_TRIGGER);
                    return;
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_TWILIGHT_BOLT:
                            DoCast(me->GetVictim(), SPELL_TWILIGHT_BOLT);
                            events.ScheduleEvent(EVENT_TWILIGHT_BOLT, 3500);
                            break;
                        case EVENT_WARPED_TWILIGHT:
                            DoCast(me, SPELL_WARPED_TWILIGHT);
                            events.ScheduleEvent(EVENT_WARPED_TWILIGHT, urand(20000, 30000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_azureborne_seerAI>(creature);
        }
};

class npc_azureborne_warlord: public CreatureScript
{
    public:
        npc_azureborne_warlord() : CreatureScript("npc_azureborne_warlord") { }

        struct npc_azureborne_warlordAI : public ScriptedAI
        {
            npc_azureborne_warlordAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_AZURE_BLAST, urand(5000, 10000));
                events.ScheduleEvent(EVENT_CONJURE_TWISTED_VISAGE, urand(7000, 12000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_AZURE_BLAST:
                            DoCast(me, SPELL_AZURE_BLAST);
                            events.ScheduleEvent(EVENT_AZURE_BLAST, urand(10000, 15000));
                            break;
                        case EVENT_CONJURE_TWISTED_VISAGE:
                            DoCast(me, SPELL_CONJURE_TWISTED_VISAGE);
                            events.ScheduleEvent(EVENT_CONJURE_TWISTED_VISAGE, urand(15000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_azureborne_warlordAI>(creature);
        }
};

class npc_crimsonborne_guardian: public CreatureScript
{
    public:
        npc_crimsonborne_guardian() : CreatureScript("npc_crimsonborne_guardian") { }

        struct npc_crimsonborne_guardianAI : public ScriptedAI
        {
            npc_crimsonborne_guardianAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CRIMSON_CHARGE, urand(3000, 5000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_CRIMSON_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 0.0f, true))
                                DoCast(target, SPELL_CRIMSON_CHARGE);
                            events.ScheduleEvent(EVENT_CRIMSON_CHARGE, urand(10000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crimsonborne_guardianAI>(creature);
        }
};

class npc_crimsonborne_seer: public CreatureScript
{
    public:
        npc_crimsonborne_seer() : CreatureScript("npc_crimsonborne_seer") { }

        struct npc_crimsonborne_seerAI : public ScriptedAI
        {
            npc_crimsonborne_seerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BLAZING_TWILIGHT_SHIELD, urand(2000, 5000));
                events.ScheduleEvent(EVENT_CORRUPTED_FLAME, urand(6000, 10000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_BLAZING_TWILIGHT_SHIELD:
                            DoCast(me, SPELL_BLAZING_TWILIGHT_SHIELD);
                            events.ScheduleEvent(EVENT_BLAZING_TWILIGHT_SHIELD, urand(12000, 15000));
                            break;
                        case EVENT_CORRUPTED_FLAME:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_CORRUPTED_FLAME);
                            events.ScheduleEvent(EVENT_CORRUPTED_FLAME, urand(8000, 14000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crimsonborne_seerAI>(creature);
        }
};

class npc_crimsonborne_warlord: public CreatureScript
{
    public:

        npc_crimsonborne_warlord() : CreatureScript("npc_crimsonborne_warlord") { }

        struct npc_crimsonborne_warlordAI : public ScriptedAI
        {
            npc_crimsonborne_warlordAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_DISARMING_BLAZE, urand(3000, 5000));
                events.ScheduleEvent(EVENT_EMPOWERING_TWILIGHT, urand(9000, 12000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_DISARMING_BLAZE:
                            DoCast(me->GetVictim(), SPELL_DISARMING_BLAZE);
                            events.ScheduleEvent(EVENT_DISARMING_BLAZE, urand(10000, 12000));
                            break;
                        case EVENT_EMPOWERING_TWILIGHT:
                            DoCast(me, SPELL_EMPOWERING_TWILIGHT);
                            events.ScheduleEvent(EVENT_EMPOWERING_TWILIGHT, urand(25000, 30000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crimsonborne_warlordAI>(creature);
        }
};

class npc_enslaved_burning_ember: public CreatureScript
{
    public:
        npc_enslaved_burning_ember() : CreatureScript("npc_enslaved_burning_ember") { }

        struct npc_enslaved_burning_emberAI : public ScriptedAI
        {
            npc_enslaved_burning_emberAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FLAME_SHOCK, urand(3000, 4000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_FLAME_SHOCK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_FLAME_SHOCK);
                            events.ScheduleEvent(EVENT_FLAME_SHOCK, urand(10000, 12000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_enslaved_burning_emberAI>(creature);
        }
};

class npc_enslaved_rock_elemental: public CreatureScript
{
    public:
        npc_enslaved_rock_elemental() : CreatureScript("npc_enslaved_rock_elemental") { }

        struct npc_enslaved_rock_elementalAI : public ScriptedAI
        {
            npc_enslaved_rock_elementalAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_JAGGED_ROCK_SHIELD, 10000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_JAGGED_ROCK_SHIELD:
                            DoCast(me, SPELL_JAGGED_ROCK_SHIELD);
                            events.ScheduleEvent(EVENT_JAGGED_ROCK_SHIELD, urand(40000, 45000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_enslaved_rock_elementalAI>(creature);
        }
};

class npc_enslaved_gronn_brute: public CreatureScript
{
    public:
        npc_enslaved_gronn_brute() : CreatureScript("npc_enslaved_gronn_brute") { }

        struct npc_enslaved_gronn_bruteAI : public ScriptedAI
        {
            npc_enslaved_gronn_bruteAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CRUNCH_ARMOR, urand(2000, 4000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_CRUNCH_ARMOR:
                            DoCast(me->GetVictim(), SPELL_CRUNCH_ARMOR);
                            events.ScheduleEvent(EVENT_CRUNCH_ARMOR, urand(10000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_enslaved_gronn_bruteAI>(creature);
        }
};

class npc_faceless_corruptor : public CreatureScript
{
    public:
        npc_faceless_corruptor() : CreatureScript("npc_faceless_corruptor") { }

        struct npc_faceless_corruptorAI : public ScriptedAI
        {
            npc_faceless_corruptorAI(Creature* creature) : ScriptedAI(creature)
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
            }
            
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SIPHON_ESSENSE, urand(5000, 7000));
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
                        case EVENT_SIPHON_ESSENSE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_SIPHON_ESSENSE);
                            events.ScheduleEvent(EVENT_SIPHON_ESSENSE, urand(7000, 10000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_faceless_corruptorAI>(creature);
        }
};

class npc_trogg_dweller : public CreatureScript
{
    public:
        npc_trogg_dweller() : CreatureScript("npc_trogg_dweller") { }

        struct npc_trogg_dwellerAI : public ScriptedAI
        {
            npc_trogg_dwellerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLAW_PUNCTURE, urand(5000, 7000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;


                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while(uint32 eventId = events.ExecuteEvent())
                {
                    switch(eventId)
                    {
                        case EVENT_CLAW_PUNCTURE:
                            DoCast(me->GetVictim(), SPELL_CLAW_PUNCTURE);
                            events.ScheduleEvent(EVENT_CLAW_PUNCTURE, urand(5000, 7000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_trogg_dwellerAI>(creature);
        }
};

class npc_twilight_armsmaster : public CreatureScript
{
    public:
        npc_twilight_armsmaster() : CreatureScript("npc_twilight_armsmaster") { }

        struct npc_twilight_armsmasterAI : public ScriptedAI
        {
            npc_twilight_armsmasterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(3000, 4000));
                events.ScheduleEvent(EVENT_FLURRY_OF_BLOWS, urand(8000, 10000));
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
                        case EVENT_MORTAL_STRIKE:
                            DoCast(me->GetVictim(), SPELL_MORTAL_STRIKE);
                            events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(6000, 8000));
                            break;
                        case EVENT_FLURRY_OF_BLOWS:
                            DoCast(me, SPELL_FLURRY_OF_BLOWS);
                            events.ScheduleEvent(EVENT_FLURRY_OF_BLOWS, urand(13000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_armsmasterAI>(creature);
        }
};

class npc_twilight_beguiler : public CreatureScript
{
    public:
        npc_twilight_beguiler() : CreatureScript("npc_twilight_beguiler") { }

        struct npc_twilight_beguilerAI : public ScriptedAI
        {
            npc_twilight_beguilerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHAINED_MIND, urand(5000, 9000));
                events.ScheduleEvent(EVENT_DECEITFUL_BLAST, urand(2000, 4000));
                
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
                        case EVENT_CHAINED_MIND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_CHAINED_MIND);
                            events.ScheduleEvent(EVENT_CHAINED_MIND, urand(8000, 12000));
                            break;
                        case EVENT_DECEITFUL_BLAST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_DECEITFUL_BLAST);
                            events.ScheduleEvent(EVENT_DECEITFUL_BLAST, urand(2000, 3000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_beguilerAI>(creature);
        }
};

class npc_twilight_drake : public CreatureScript
{
    public:
        npc_twilight_drake() : CreatureScript("npc_twilight_drake") { }

        struct npc_twilight_drakeAI : public ScriptedAI
        {
            npc_twilight_drakeAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_TWILIGHT_BREATH, urand(5000, 7000));
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
                        case EVENT_TWILIGHT_BREATH:
                            DoCast(me, SPELL_TWILIGHT_BREATH);
                            events.ScheduleEvent(EVENT_TWILIGHT_BREATH, urand(10000, 12000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_drakeAI>(creature);
        }
};

class npc_twilight_earthshaper : public CreatureScript
{
    public:
        npc_twilight_earthshaper() : CreatureScript("npc_twilight_earthshaper") { }

        struct npc_twilight_earthshaperAI : public ScriptedAI
        {
            npc_twilight_earthshaperAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                DoCast(me, SPELL_SUMMON_ROCK_ELEMENTAL);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_EARTH_SPIKE, urand(3000, 5000));
                events.ScheduleEvent(EVENT_STONE_SKIN, urand(4000, 7000));
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
                    case EVENT_EARTH_SPIKE:
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                            DoCast(target, SPELL_EARTH_SPIKE);
                        events.ScheduleEvent(EVENT_EARTH_SPIKE, urand(8000, 10000));
                        break;
                    case EVENT_STONE_SKIN:
                        DoCast(me, SPELL_STONE_SKIN);
                        events.ScheduleEvent(EVENT_STONE_SKIN, urand(25000, 31000));
                        break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_earthshaperAI>(creature);
        }
};

class npc_twilight_enforcer : public CreatureScript
{
    public:
        npc_twilight_enforcer() : CreatureScript("npc_twilight_enforcer") { }

        struct npc_twilight_enforcerAI : public ScriptedAI
        {
            npc_twilight_enforcerAI(Creature* creature) : ScriptedAI(creature) { }
            
            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MEAT_GRINDER, urand(5000, 6000));
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
                        case EVENT_MEAT_GRINDER:
                            DoCast(me, SPELL_MEAT_GRINDER_TRIGGER);
                            DoCast(me, SPELL_MEAT_GRINDER);
                            events.ScheduleEvent(EVENT_MEAT_GRINDER, urand(12000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_enforcerAI>(creature);
        }

};

class npc_twilight_firecatcher : public CreatureScript
{
    public:
        npc_twilight_firecatcher() : CreatureScript("npc_twilight_firecatcher") { }

        struct npc_twilight_firecatcherAI : public ScriptedAI
        {
            npc_twilight_firecatcherAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                DoCast(me, SPELL_SUMMON_BURNING_EMBER);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MOLTEN_BLAST, urand(2000, 3000));
                events.ScheduleEvent(EVENT_FLAME_CONDUIT, urand(6000, 9000));
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
                        case EVENT_MOLTEN_BLAST:
                            DoCast(me->GetVictim(), SPELL_MOLTEN_BLAST);
                            events.ScheduleEvent(EVENT_MOLTEN_BLAST, urand(8000, 10000));
                            break;
                        case EVENT_FLAME_CONDUIT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_FLAME_CONDUIT);
                            events.ScheduleEvent(EVENT_FLAME_CONDUIT, urand(12000, 14000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_firecatcherAI>(creature);
        }
};

class npc_twilight_shadow_weaver : public CreatureScript
{
    public:
        npc_twilight_shadow_weaver() : CreatureScript("npc_twilight_shadow_weaver") { }

        struct npc_twilight_shadow_weaverAI : public ScriptedAI
        {
            npc_twilight_shadow_weaverAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                if (IsHeroic())
                    DoCast(me, SPELL_SUMMON_FELHUNTER);

                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(1000, 2000));
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SHADOW_WEAVE, urand(5000, 6000));
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
                        case EVENT_SHADOW_BOLT:
                            DoCast(me->GetVictim(), SPELL_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 3000));
                            break;
                        case EVENT_SHADOW_WEAVE:
                            DoCast(me, SPELL_SHADOW_WEAVE_SCRIPT);
                            events.ScheduleEvent(EVENT_SHADOW_WEAVE, urand(9000, 14000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_shadow_weaverAI>(creature);
        }
};

class npc_twilight_stormbreaker : public CreatureScript
{
    public:
        npc_twilight_stormbreaker() : CreatureScript("npc_twilight_stormbreaker") { }

        struct npc_twilight_stormbreakerAI : public ScriptedAI
        {
            npc_twilight_stormbreakerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                DoCast(me, SPELL_SUMMON_WATER_SPIRIT);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WATER_BOLT, urand(1000, 2000));

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_WATER_SHELL, urand(5000, 8000));
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
                        case EVENT_WATER_BOLT:
                            DoCast(me->GetVictim(), SPELL_WATER_BOLT);
                            events.ScheduleEvent(EVENT_WATER_BOLT, urand(2500, 3000));
                            break;
                        case EVENT_WATER_SHELL:
                            DoCast(me, SPELL_WATER_SHELL);
                            events.ScheduleEvent(EVENT_WATER_SHELL, urand(15000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_stormbreakerAI>(creature);
        }
};

class npc_twilight_thundercaller : public CreatureScript
{
    public:
        npc_twilight_thundercaller() : CreatureScript("npc_twilight_thundercaller") { }

        struct npc_twilight_thundercallerAI : public ScriptedAI
        {
            npc_twilight_thundercallerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                DoCast(me, SPELL_SUMMON_THUNDER_SPIRIT);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(2000, 3000));
                events.ScheduleEvent(EVENT_OVERCHARGE, urand(9000, 12000));
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
                        case EVENT_CHAIN_LIGHTNING:
                            DoCast(me->GetVictim(), SPELL_CHAIN_LIGHTNING);
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(5000, 7000));
                            break;
                        case EVENT_OVERCHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_OVERCHARGE);
                            events.ScheduleEvent(EVENT_OVERCHARGE, urand(10000, 12000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_thundercallerAI>(creature);
        }
};

class npc_twilight_wyrmcaller : public CreatureScript
{
    public:
        npc_twilight_wyrmcaller() : CreatureScript("npc_twilight_wyrmcaller") { }

        struct npc_twilight_wyrmcallerAI : public ScriptedAI
        {
            npc_twilight_wyrmcallerAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            EventMap events;
            SummonList summons;

            void Reset() override
            {
                summons.DespawnAll();
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CALL_WYRM, 3000);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
            }
            
            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
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
                        case EVENT_CALL_WYRM:
                            Talk(SAY_CALL_WYRM);
                            if (Creature* _wyrm = me->SummonCreature(NPC_TWILIGHT_DRAKE_1, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 20.0f, me->GetOrientation()))
                                _wyrm->GetMotionMaster()->MovePoint(0, me->GetPosition());
                            events.ScheduleEvent(EVENT_FEED_PET, urand(3000, 5000));
                            break;
                        case EVENT_FEED_PET:
                            if (Creature* _wyrm = me->FindNearestCreature(NPC_TWILIGHT_DRAKE_1, 100.0f, true))
                                DoCast(_wyrm, SPELL_FEED_PET);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_wyrmcallerAI>(creature);
        }
};

class npc_twilight_war_mage : public CreatureScript
{
    public:
        npc_twilight_war_mage() : CreatureScript("npc_twilight_war_mage") { }

        struct npc_twilight_war_mageAI : public ScriptedAI
        {
            npc_twilight_war_mageAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ENCHANT, 1000);
                events.ScheduleEvent(EVENT_POLYMORPH, urand(5000, 7000));
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
                        case EVENT_ENCHANT:
                            DoCast(me, urand(0, 1)? SPELL_FIRE_ENCHANT: SPELL_ICE_ENCHANT);
                            break;
                        case EVENT_POLYMORPH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_POLYMORPH);
                            events.ScheduleEvent(EVENT_POLYMORPH, urand(10000, 15000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_war_mageAI>(creature);
        }
};

class npc_ascended_rockbreaker_fissure : public CreatureScript
{
    public:
        npc_ascended_rockbreaker_fissure() : CreatureScript("npc_ascended_rockbreaker_fissure") { }

        struct npc_ascended_rockbreaker_fissureAI : public ScriptedAI
        {
            npc_ascended_rockbreaker_fissureAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                DoCast(me, SPELL_FISSURE_TRIGGER);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ascended_rockbreaker_fissureAI>(creature);
        }
};

class npc_crimsonborne_warlord_empowering_flames : public CreatureScript
{
    public:
        npc_crimsonborne_warlord_empowering_flames() : CreatureScript("npc_crimsonborne_warlord_empowering_flames") { }

        struct npc_crimsonborne_warlord_empowering_flamesAI : public ScriptedAI
        {
            npc_crimsonborne_warlord_empowering_flamesAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                DoCast(me, SPELL_EMPOWERING_TWILIGHT_AURA);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crimsonborne_warlord_empowering_flamesAI>(creature);
        }
};

class spell_twilight_enforcer_meat_grinder : public SpellScriptLoader
{
    public:
        spell_twilight_enforcer_meat_grinder() : SpellScriptLoader("spell_twilight_enforcer_meat_grinder") { }

        class spell_twilight_enforcer_meat_grinder_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_twilight_enforcer_meat_grinder_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_DIZZY, true);
            }
            
            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_twilight_enforcer_meat_grinder_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_twilight_enforcer_meat_grinder_AuraScript();
        }
};

class npc_battered_red_drake : public CreatureScript
{
    public:
        npc_battered_red_drake() : CreatureScript("npc_battered_red_drake") { }

        struct npc_battered_red_drakeAI : public ScriptedAI
        {
            npc_battered_red_drakeAI(Creature* creature) : ScriptedAI(creature) { }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (instance->GetData(DATA_FORGEMASTER_THRONGUS) != DONE)
                        return;

                if (me->GetEntry() == 42571)
                    clicker->NearTeleportTo(-549.354f, -582.390f, 276.597f, 2.73476f, false);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_battered_red_drakeAI>(creature);
        }
};


class spell_twilight_shadow_weaver_shadow_weave : public SpellScriptLoader
{
    public:
        spell_twilight_shadow_weaver_shadow_weave() : SpellScriptLoader("spell_twilight_shadow_weaver_shadow_weave") { }

        class spell_twilight_shadow_weaver_shadow_weave_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_twilight_shadow_weaver_shadow_weave_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetCaster())
                    return;

                GetCaster()->CastSpell(GetCaster(), SPELL_SHADOW_WEAVE_DUMMY, true);
                GetCaster()->CastSpell(GetCaster(), SPELL_SHADOW_WEAVE_DMG, false);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_twilight_shadow_weaver_shadow_weave_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_twilight_shadow_weaver_shadow_weave_SpellScript();
        }
};

class spell_twilight_thundercaller_electric_blast: public SpellScriptLoader
{
    public:
        spell_twilight_thundercaller_electric_blast() : SpellScriptLoader("spell_twilight_thundercaller_electric_blast") { }

        class spell_twilight_thundercaller_electric_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_twilight_thundercaller_electric_blast_SpellScript);

            void HandleScript()
            {
                if (!GetCaster())
                    return;

                uint8 _stacks = 0;

                if (GetCaster()->HasAura(SPELL_OVERCHARGE))
                    if (Aura* _overcharge = GetCaster()->GetAura(SPELL_OVERCHARGE))
                        _stacks = _overcharge->GetStackAmount();

                if (_stacks > 0)
                    SetHitDamage(GetHitDamage() * _stacks);
            }

            void Register() override
            {
                BeforeHit += SpellHitFn(spell_twilight_thundercaller_electric_blast_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_twilight_thundercaller_electric_blast_SpellScript();
        }
};

void AddSC_grim_batol()
{
    new npc_ascended_flameseeker();
    new npc_ascended_rockbreaker();
    new npc_ascended_waterlasher();
    new npc_ascended_windwalker();
    new npc_azureborne_guardian();
    new npc_azureborne_seer();
    new npc_azureborne_warlord();
    new npc_crimsonborne_guardian();
    new npc_crimsonborne_seer();
    new npc_crimsonborne_warlord();
    new npc_enslaved_burning_ember();
    new npc_enslaved_rock_elemental();
    new npc_enslaved_gronn_brute();
    new npc_faceless_corruptor();
    new npc_trogg_dweller();
    new npc_twilight_armsmaster();
    new npc_twilight_beguiler();
    new npc_twilight_drake();
    new npc_twilight_earthshaper();
    new npc_twilight_enforcer();
    new npc_twilight_firecatcher();
    new npc_twilight_shadow_weaver();
    new npc_twilight_stormbreaker();
    new npc_twilight_thundercaller();
    new npc_twilight_war_mage();
    new npc_twilight_wyrmcaller();
    new npc_ascended_rockbreaker_fissure();
    new npc_crimsonborne_warlord_empowering_flames();
    new npc_battered_red_drake();
    new spell_twilight_enforcer_meat_grinder();
    new spell_twilight_shadow_weaver_shadow_weave();
    new spell_twilight_thundercaller_electric_blast();
}
