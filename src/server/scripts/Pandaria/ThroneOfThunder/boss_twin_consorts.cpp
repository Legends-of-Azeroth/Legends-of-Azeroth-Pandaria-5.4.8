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
#include "throne_of_thunder.h"

enum Spells
{
    // Suen
    SPELL_TEAR_OF_THE_SUN            = 137404,
    SPELL_TEAR_OF_THE_SUN_SELECTOR   = 138744,
    SPELL_TEAR_OF_THE_SUN_MISSLE     = 138745, // ok, missle should`ve use ONLY IF ENCOUNTER HAS TIGER FORTITUDE! in else 137405
    SPELL_TEAR_OF_THR_SUN_EFF        = 137405, // selector + eff
    SPELL_DISSIPATE_SUEN             = 137105, // just visual eff
    SPELL_LIGHT_OF_DAY_EFF           = 137403,
    SPELL_LIGHT_OF_DAY_SECOND_EFF    = 137432, // for ice commet
    SPELL_LIGHT_OF_DAY               = 137401,
    SPELL_LIGHT_OF_DAY_SELECTOR      = 138801,
    SPELL_FAN_OF_FLAMES              = 137408,
    SPELL_FLAMES_OF_PASSION_EFF      = 137414,
    SPELL_FLAMES_OF_PASSION_CHARGE   = 137412,
    SPELL_FLAMES_OF_PASSION_CHARGE_2 = 137415,
    SPELL_FLAMES_OF_PASSION_AT       = 137416,
    SPELL_FLAMES_OF_PASSION_AT_EFF   = 137417,
    SPELL_BLAZING_RADIANCE_EFF       = 137410,
    SPELL_BLAZING_RADIANCE           = 137411,
    SPELL_BLAZING_RADIANCE_AURA      = 139412,
    SPELL_INVISIBLE_SUEN             = 137106,
    SPELL_INVIS                      = 137106,

    // Lu`lin
    SPELL_DISSIPATE_LULIN            = 137187,
    SPELL_INVISIBLE_LULIN            = 137189,
    SPELL_MOON_LOTUS_PERIODIC        = 136690, // summon lotus every 4s (looks like lie)
    SPELL_MOON_LOTUS_SUMM            = 136714,
    SPELL_MOON_LOTUS_AURA            = 136721, // green mist
    SPELL_MOON_LOTUS_HIDE_AURA       = 137191,
    SPELL_MOON_LOTUS_BLOOM_AURA      = 137192,
    SPELL_MOON_LOTUS_BLOOM           = 137118,
    SPELL_MOON_LOTUS_HIDE            = 137195, // call in day phase
    SPELL_MOON_LOTUS_REAPPEAR        = 137196, // call in twilight phase
    SPELL_SLUMBER_SPORES             = 136722,
    SPELL_COSMIC_BARRAGE             = 136752,
    SPELL_COSMIC_BARRAGE_SUMM        = 136768,
    SPELL_COSMIC_BARRAGE_SUMM_2      = 137164, // ok, same in alacrity state (slow)
    SPELL_COSMIC_BARRAGE_FORCE       = 137749, // by world trigger (not used)
    SPELL_CRASHING_STAR_JUMP_SLOW    = 139561,
    SPELL_CRASHING_STAR_JUMP         = 137130,
    SPELL_CRASHING_STAR_EFF          = 137129,
    SPELL_CRASHING_STAR_SELECT_AURA  = 139565, // shouldn`t target same player twice (exclude if no more targets)
    SPELL_LAUNCH_STAR                = 137139,
    SPELL_LAUNCH_STAR_EFF            = 137120,
    SPELL_TIDAL_FORCE                = 137531,
    SPELL_TIDAL_FORCE_VISUAL         = 137718,
    SPELL_TIDAL_FORCE_VISUAL_WAVE    = 140863,
    SPELL_TIDAL_FORCE_AT             = 138689, // at realise use 140863
    SPELL_TIDAL_FORCE_EFF            = 138688,
    SPELL_TIDAL_FORCE_AURA_LAUNCH    = 137717, // used by trigger
    SPELL_TIDAL_FORCE_SCRIPT         = 137715,
    SPELL_TIDAL_FORCE_CONE_EFF       = 137716,
    SPELL_BEAST_OF_NIGHTMARES        = 137375,
    SPELL_BEAST_OF_NIGHTMARES_SUMM   = 137332,
    SPELL_BEAST_OF_NIGHTMATRES_AURA  = 137341,
    SPELL_CORRUPTED_HEALING          = 137360, // proc eff
    SPELL_DESPAWN_ICE_COMMET         = 138679,
    SPELL_ICE_COMET_MISSLE           = 137418,
    SPELL_ICE_COMET_AURA             = 137420,
    SPELL_ICE_COMET_SELECTOR         = 138795,
    SPELL_ICE_COMET_MISSLE_TWILIGHT  = 138797,
    SPELL_ICE_COMET_FACE_SUEN        = 137436,
    SPELL_ICE_COMET_FACE_SUEN_EFF    = 137439, // cone AT
    SPELL_ICE_COMET_CHILLED          = 137483,

    // Celestials | Misc
    SPELL_ASTRAL_PLANE               = 139041,
    SPELL_CLEAR_INVOKE               = 140038,
    SPELL_INVOKE_OX_SPIRIT           = 138254,
    SPELL_INVOKE_CRANE_SPIRIT        = 138189,
    SPELL_INVOKE_SERPENT_SPIRIT      = 138267,
    SPELL_INVOKE_TIGER_SPIRIT        = 138264,
    SPELL_STAR_STATE_INACTIVE        = 138039, // also using like model for crashing star
    SPELL_STAR_STATE_ACTIVE          = 137138,
    SPELL_STAR_STATE_UNKNOWN         = 138140, // possible after paint stars should`ve been passive some time.
    SPELL_STARDUST_BEAM              = 138206,
    SPELL_MOONBEAM_STATE_WORLD       = 138114, // init path star
    SPELL_CANCEL_BEAMS               = 138283,
    SPELL_SUMMON_STAR_1              = 138199, // include areatrigger
    SPELL_SUMMON_STAR_2              = 138202,
    SPELL_SUMMON_STAR_3              = 138205,
    SPELL_SUMMON_STAR_4              = 138219,
    SPELL_SUMMON_STAR_5              = 138221,
    SPELL_SUMMON_STAR_6              = 138224,
    SPELL_SUMMON_STAR_7              = 138227,
    SPELL_LINK_STAR_1                = 138200,
    SPELL_LINK_STAR_2                = 138203,
    SPELL_LINK_STAR_3                = 138218,
    SPELL_LINK_STAR_4                = 138220,
    SPELL_LINK_STAR_5                = 138223,
    SPELL_LINK_STAR_6                = 138226,
    SPELL_LINK_STAR_7                = 138204,
    SPELL_OX_CONSTELLATION           = 138260, // used by high trigger (only visual)
    SPELL_TIGER_CONSTELLATION        = 138261,
    SPELL_CRANE_CONSTELLATION        = 138180,
    SPELL_SERPENT_CONSTELLATION      = 138262,
    SPELL_FORTITUDE_OF_THE_OX        = 138298, // selector
    SPELL_FORTITUDE_OF_THE_OX_EFF    = 138300,
    SPELL_CRANE_RUSH_SUMM            = 144495,
    SPELL_CRANE_RUSH_EFF             = 138318,
    SPELL_CRANE_RUSH_AT              = 138314, // target_unk_125?
    SPELL_SERPENT_VITALITY           = 138306,
    SPELL_TIGER_SPIRIT               = 138656,
    SPELL_XUEN_BLESSED_ALACRITY      = 138645, // slow encounter
    SPELL_XUEN_BLESSED_ALACRITY_P    = 138855, // boost players
    SPELL_SUNNY_SKY                  = 137183, // by world trigger | 137141 + This - Night phase
    SPELL_DARK_SKY                   = 138137, // curtain (not used!)
    SPELL_STARY_SKY                  = 137193, // too curtain, make a star sky... | 137141 + This - Star sky
    SPELL_STARY_SKY_DARKNESS         = 137141,
    SPELL_INACTIVE                   = 139752, // scale mod
    SPELL_MOON                       = 136982, // visual moon
    SPELL_SUN                        = 137075, // visual sun
    SPELL_UNINTERACTABLE             = 138185, // in case press to constellation
    SUMMON_ASTRAL_PLANE              = 139043, // portal to back if u won`t DRAWING!
    SPELL_PERMANENT_FEIGN_DEATH      = 70628,

    /*Heroic*/
    SPELL_NUCLEAR_INFERNO            = 137491,
    SPELL_NUCLEAR_INFERNO_DAY_EFF    = 137492,
    SPELL_NUCLEAR_INFERNO_TW_EFF     = 137524,
    SPELL_DARKNESS                   = 137382,
    SPELL_EXPOSE_LURKERS             = 137406,
    SPELL_ICY_SHADOWS_EFF            = 137440,
};

enum Yells
{
    TALK_INTRO,
    TALK_SWITCH_NIGHT_TO_DAY,
    TALK_SWITCH_DAY_TO_TWILIGHT,
    TALK_SUEN_DEATH, // lulin talk only if suen death before
};

enum Phases
{
    PHASE_NONE,
    PHASE_NIGHT,
    PHASE_DAY,
    PHASE_TWILIGHT,
};

enum Actions
{
    ACTION_CELESTIALS_INTERRACT = 1,
    ACTION_CELESTIALS_NOT_INTERRACT,
    ACTION_LULIN_DAY,
    ACTION_LULIN_TWILIGHT,
    ACTION_LAUNCH_STAR,
    ACTION_INIT_CHARGE,
    ACTION_CHARGE_HIT,
    ACTION_LAST_STAND,
    ACTION_CELESTIAL_ASTRAL_FADED,
    ACTION_CELESTIAL_DRAWING_WRONG,
    ACTION_CELESTIAL_DRAWING_DONE,
    ACTION_CELESTIAL_REBIRTH,
    ACTION_LIGHT_OF_DAY_DELAY,
};

enum Events
{
    EVENT_INTRO = 1,
    EVENT_SWITCH_DAY,
    EVENT_SWITCH_TWILIGHT,
    EVENT_TEAR_OF_THE_SUN,
    EVENT_LIGHT_OF_DAY,
    EVENT_MOON_LOTUS,
    EVENT_COSMIC_BARRAGE,
    EVENT_BEAST_OF_NIGHTMARES,
    EVENT_BERSERK,
    EVENT_FAN_OF_FLAMES,
    EVENT_FLAMES_OF_PASSION,
    EVENT_ICE_COMET,
    EVENT_TIDAL_FORCE,
    EVENT_SPAWN_LURKER,
    EVENT_NUCLEAR_INFERNO,
};

const std::list<uint32> starSumList =
{
    SPELL_SUMMON_STAR_1,
    SPELL_SUMMON_STAR_2,
    SPELL_SUMMON_STAR_3,
    SPELL_SUMMON_STAR_4,
    SPELL_SUMMON_STAR_5,
    SPELL_SUMMON_STAR_6,
    SPELL_SUMMON_STAR_7,
};

const std::map<uint32, uint32> celestialsList =
{
    { NPC_FADED_IMAGE_OF_CHI_JI, SPELL_CRANE_RUSH_AT       },
    { NPC_FADED_IMAGE_OF_NIUZAO, SPELL_FORTITUDE_OF_THE_OX },
    { NPC_FADED_IMAGE_OF_XUEN,   SPELL_TIGER_SPIRIT        },
    { NPC_FADED_IMAGE_OF_YULON,  SPELL_SERPENT_VITALITY    },
};

const std::map<uint32,uint32> invCelestialsType=
{
    { NPC_FADED_IMAGE_OF_NIUZAO, SPELL_INVOKE_OX_SPIRIT      },
    { NPC_FADED_IMAGE_OF_CHI_JI, SPELL_INVOKE_CRANE_SPIRIT   },
    { NPC_FADED_IMAGE_OF_YULON,  SPELL_INVOKE_SERPENT_SPIRIT },
    { NPC_FADED_IMAGE_OF_XUEN,   SPELL_INVOKE_TIGER_SPIRIT   },
};

const std::map<uint32, std::array<uint32, 2>> invStarSequenceType =
{
    { 1, { SPELL_SUMMON_STAR_1, SPELL_LINK_STAR_1 } },
    { 2, { SPELL_SUMMON_STAR_2, SPELL_LINK_STAR_2 } },
    { 3, { SPELL_SUMMON_STAR_3, SPELL_LINK_STAR_3 } },
    { 4, { SPELL_SUMMON_STAR_4, SPELL_LINK_STAR_4 } },
    { 5, { SPELL_SUMMON_STAR_5, SPELL_LINK_STAR_5 } },
    { 6, { SPELL_SUMMON_STAR_6, SPELL_LINK_STAR_6 } },
    { 7, { SPELL_SUMMON_STAR_7, SPELL_LINK_STAR_7 } },
};

const std::map<uint32, uint32> invOxType =
{
    { 5, 7 },
    { 7, 2 },
    { 2, 3 },
    { 3, 6 },
};

const std::map<uint32, uint32> invCraneType =
{
    { 6, 2 },
    { 2, 3 },
    { 3, 6 },
    { 6, 5 },
};

const std::map<uint32, uint32> invTigerType =
{
    { 2, 6 },
    { 6, 3 },
    { 3, 5 },
    { 5, 2 },
};

const std::map<uint32, uint32> invSerpentType =
{
    { 4, 3 },
    { 3, 7 },
    { 7, 2 },
    { 2, 1 },
};

// Suen 68904 will controler of encounter
class boss_suen : public CreatureScript
{
    public:
        boss_suen() : CreatureScript("boss_suen") { }

        struct boss_suenAI : public BossAI
        {
            boss_suenAI(Creature* creature) : BossAI(creature, DATA_TWIN_CONSORTS) 
            {
                me->setActive(true);
            }

            uint8 phase;
            uint32 targetLowGUID;
            uint32 currentCelestialArea;
            uint32 celestialCounter;
            ObjectGuid targetGUID;
            EventMap berserkEvents;
            bool atEvade;
            bool feignDeath;
            bool isTwilight;
            bool isDefeat;

            void Reset() override
            {
                events.Reset();
                berserkEvents.Reset();
                atEvade = false;
                feignDeath = false;
                isTwilight = false;
                isDefeat = false;
                _Reset();
                phase = PHASE_NONE;
                targetGUID = ObjectGuid::Empty;
                targetLowGUID = ObjectGuid::Empty;
                celestialCounter = 0;
                currentCelestialArea = PHASE_NONE;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_AGGRESSIVE);

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 219.8m in 10 normal
                    me->SetCreateHealth(219800000);
                    me->SetMaxHealth(219800000);
                    me->SetHealth(219800000);
                    me->ResetPlayerDamageReq();
                }

                // if feign death remove
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                me->GetMap()->SetWorldState(WORLDSTATE_FROM_DUSK_TILL_DOWN, 0);

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            ObjectGuid GetGUID(int32 /*type*/) const override
            {
                return targetGUID;
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(TALK_INTRO);

                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                {
                    lulin->SetInCombatWithZone();
                    lulin->AI()->DoAction(ACTION_START_INTRO);
                }

                SetPhase(PHASE_NIGHT);
                SendActionToImages(ACTION_CELESTIALS_INTERRACT);
                events.ScheduleEvent(EVENT_SWITCH_DAY, 3 * MINUTE * IN_MILLISECONDS); // temp 1m
                events.ScheduleEvent(EVENT_SWITCH_TWILIGHT, 6 * MINUTE * IN_MILLISECONDS);
                
                berserkEvents.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                // Feign Death
                if (feignDeath)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && !feignDeath && hasLulinAlive())
                {
                    feignDeath = true;
                    damage = 0;

                    events.Reset();
                    me->RemoveAllAuras();
                    me->SetHealth(1);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->PrepareChanneledCast(me->GetOrientation());
                    DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);

                    if (instance)
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        // If not Twilight Phase
                        if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_LULIN)))
                            lulin->AI()->DoAction(ACTION_LAST_STAND);
                    }

                    return;
                }
            }

            void SetPhase(uint8 newPhase)
            {
                uint8 oldPhase = phase;
                phase = newPhase;
                events.SetPhase(newPhase);

                switch (newPhase)
                {
                    case PHASE_NIGHT:
                        me->AttackStop();
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(me, SPELL_DISSIPATE_SUEN);

                        // Visible Night
                        if (Creature* worldTrigger_1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                        {
                            worldTrigger_1->CastSpell(worldTrigger_1, SPELL_STARY_SKY_DARKNESS, true);
                            worldTrigger_1->CastSpell(worldTrigger_1, SPELL_SUNNY_SKY, true);
                        }

                        // Visible Star Sky
                        if (Creature* worldTrigger_2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER + 1) : ObjectGuid::Empty))
                        {
                            worldTrigger_2->SetObjectScale(0.1f);
                            worldTrigger_2->CastSpell(worldTrigger_2, SPELL_STARY_SKY_DARKNESS, true);
                            worldTrigger_2->CastSpell(worldTrigger_2, SPELL_STARY_SKY, true);
                        }

                        // Fly moon
                        if (Creature* moon = me->SummonCreature(NPC_INVISIBLE_MAN, sunMoonPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            moon->OverrideInhabitType(INHABIT_AIR);
                            moon->UpdateMovementFlags();
                            moon->SetObjectScale(5.0f);

                            moon->CastSpell(moon, SPELL_MOON, true);

                            Movement::MoveSplineInit init(moon);
                            for (auto&& itr : sunMoonPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.SetCyclic();
                            init.Launch();
                        }

                        events.ScheduleEvent(EVENT_TEAR_OF_THE_SUN, 23 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_LIGHT_OF_DAY, 14 * IN_MILLISECONDS);
                        break;
                    case PHASE_DAY:
                        summons.DespawnEntry(NPC_INVISIBLE_MAN);

                        // Visible Night
                        if (Creature* worldTrigger_1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                        {
                            worldTrigger_1->RemoveAurasDueToSpell(SPELL_STARY_SKY_DARKNESS);
                            worldTrigger_1->CastSpell(worldTrigger_1, SPELL_SUNNY_SKY, true);
                        }

                        // Visible Star Sky
                        if (Creature* worldTrigger_2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER + 1) : ObjectGuid::Empty))
                        {
                            worldTrigger_2->RemoveAurasDueToSpell(SPELL_STARY_SKY_DARKNESS);
                            worldTrigger_2->RemoveAurasDueToSpell(SPELL_STARY_SKY);
                        }

                        // Fly Sun
                        if (Creature* sun = me->SummonCreature(NPC_SUN, sunMoonPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            sun->OverrideInhabitType(INHABIT_AIR);
                            sun->UpdateMovementFlags();
                            sun->SetObjectScale(5.0f);

                            sun->CastSpell(sun, SPELL_SUN, true);

                            Movement::MoveSplineInit init(sun);
                            for (auto&& itr : sunMoonPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.SetCyclic();
                            init.Launch();
                        }

                        // Set Lulin in Inactive State
                        if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                            lulin->AI()->DoAction(ACTION_LULIN_DAY);

                        // Set Seun in Active State
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVISIBLE_SUEN);
                        DoCast(me, SPELL_BLAZING_RADIANCE, true);

                        events.CancelEvent(EVENT_TEAR_OF_THE_SUN);
                        events.CancelEvent(EVENT_LIGHT_OF_DAY);

                        events.ScheduleEvent(EVENT_INTRO, 3 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_FAN_OF_FLAMES, 3.5 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_FLAMES_OF_PASSION, 7 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_LIGHT_OF_DAY, 6.5 * IN_MILLISECONDS);
                        
                        if (IsHeroic())
                        {
                            events.ScheduleEvent(EVENT_NUCLEAR_INFERNO, 45 * IN_MILLISECONDS);

                            std::list<Creature*> lurkerList;
                            GetCreatureListWithEntryInGrid(lurkerList, me, NPC_LURKER_IN_THE_NIGHT, 200.0f);

                            for (auto&& itr : lurkerList)
                            {
                                itr->RemoveAurasDueToSpell(SPELL_INVIS);
                                itr->AI()->DoAction(ACTION_INIT_CHARGE);
                            }
                        }
                        break;
                    case PHASE_TWILIGHT:
                        summons.DespawnEntry(NPC_SUN);

                        Talk(TALK_SWITCH_DAY_TO_TWILIGHT);

                        SendActionToImages(ACTION_CELESTIAL_REBIRTH);

                        // Twilight
                        if (Creature* worldTrigger_1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                            worldTrigger_1->RemoveAurasDueToSpell(SPELL_SUNNY_SKY);

                        if (Creature* worldTrigger_2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER + 1) : ObjectGuid::Empty))
                        {
                            worldTrigger_2->SetObjectScale(1.0f);
                            worldTrigger_2->CastSpell(worldTrigger_2, SPELL_STARY_SKY_DARKNESS, true);
                            worldTrigger_2->CastSpell(worldTrigger_2, SPELL_STARY_SKY, true);
                        }

                        // Fly moon
                        if (Creature* moon = me->SummonCreature(NPC_INVISIBLE_MAN, sunMoonPath[0], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            moon->OverrideInhabitType(INHABIT_AIR);
                            moon->UpdateMovementFlags();
                            moon->SetObjectScale(5.0f);

                            moon->CastSpell(moon, SPELL_MOON, true);

                            Movement::MoveSplineInit init(moon);
                            for (auto&& itr : sunMoonPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.SetCyclic();
                            init.Launch();
                        }

                        // Fly Sun
                        if (Creature* sun = me->SummonCreature(NPC_SUN, sunMoonPath[16], TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            sun->OverrideInhabitType(INHABIT_AIR);
                            sun->UpdateMovementFlags();
                            sun->SetObjectScale(5.0f);

                            sun->CastSpell(sun, SPELL_SUN, true);

                            Movement::MoveSplineInit init(sun);
                            for (auto&& itr : sunMoonPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.SetCyclic();
                            init.Launch();
                        }

                        isTwilight = true;
                        events.CancelEvent(EVENT_FAN_OF_FLAMES);

                        // Set Lulin in Active State
                        if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                            lulin->AI()->DoAction(ACTION_LULIN_TWILIGHT);

                        break;
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INIT_CHARGE:
                    {
                        if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                        {
                            uint32 flamesCount = me->GetExactDist2d(target) / 3;

                            for (uint32 i = 0; i < flamesCount; i++)
                            {
                                float x, y;
                                GetPositionWithDistInOrientation(me, 3.0f + 3.5f * i, me->GetAngle(target), x, y);
                                me->CastSpell(x, y, me->GetPositionZ(), SPELL_FLAMES_OF_PASSION_AT, true);
                            }

                        }

                        me->RemoveChanneledCast(targetGUID);
                        break;
                    }
                    case ACTION_LAST_STAND:
                        if (isTwilight || !me->HasAura(SPELL_INVISIBLE_SUEN))
                            break;

                        events.Reset();

                        // Set Seun in Active State
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVISIBLE_SUEN);
                        DoCast(me, SPELL_BLAZING_RADIANCE, true);

                        events.ScheduleEvent(EVENT_FLAMES_OF_PASSION, 7 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_LIGHT_OF_DAY, 6.5 * IN_MILLISECONDS);

                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_NUCLEAR_INFERNO, 45 * IN_MILLISECONDS);
                        break;
                    case ACTION_CELESTIAL_DRAWING_DONE:
                        currentCelestialArea = 0;
                        celestialCounter = 0;
                        break;
                    case ACTION_CELESTIAL_DRAWING_WRONG:
                        celestialCounter = 0;
                        break;
                    case ACTION_LIGHT_OF_DAY_DELAY:
                        events.RescheduleEvent(EVENT_LIGHT_OF_DAY, 5.5 * IN_MILLISECONDS);
                        break;
                }
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_FLAME_OF_PASSION_TARGET:
                        return targetLowGUID;
                    case TYPE_IN_TWILIGHT:
                        return isTwilight;
                    case TYPE_SELECTED_CELESTIAL:
                        return currentCelestialArea;
                    case TYPE_ACTIVE_STAR_COUNTER:
                        return celestialCounter;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case TYPE_ACTIVE_STAR_COUNTER:
                        celestialCounter = data;
                        break;
                    case TYPE_SELECTED_CELESTIAL:
                        currentCelestialArea = data;
                        break;
                }
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_TWIN_CONSORTS, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAN_OF_FLAMES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BEAST_OF_NIGHTMATRES_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ICY_SHADOWS_EFF);
                }

                SendActionToImages(ACTION_CELESTIALS_NOT_INTERRACT);

                if (Creature* worldTrigger_1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                    worldTrigger_1->RemoveAllAuras();

                if (Creature* worldTrigger_2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER + 1) : ObjectGuid::Empty))
                    worldTrigger_2->RemoveAllAuras();

                if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                    lulin->AI()->EnterEvadeMode();

                HandleInactivityCreatures();
                DoCast(me, SPELL_DESPAWN_ICE_COMMET, true);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                scheduler.CancelAll();
                _DespawnAtEvade();
                me->GetMotionMaster()->MoveTargetedHome();

                me->RemoveAurasDueToSpell(SPELL_DISSIPATE_SUEN);
                me->RemoveAllAreasTrigger();
            }

            void JustDied(Unit* /*killer*/) override 
            {
                if (isDefeat)
                    return;

                isDefeat = true;

                // Encounter Done
                if (Player* killer = me->FindNearestPlayer(200.0f))
                {
                    if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                    {
                        lulin->RemoveAura(SPELL_PERMANENT_FEIGN_DEATH);
                        killer->Kill(lulin);
                    }

                    me->RemoveAura(SPELL_PERMANENT_FEIGN_DEATH);
                    killer->Kill(me);
                }

                _JustDied();

                Talk(TALK_SUEN_DEATH);

                if (instance)
                {
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FAN_OF_FLAMES);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BEAST_OF_NIGHTMATRES_AURA);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ICY_SHADOWS_EFF);

                    if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_LULIN)))
                        if (lulin->IsAlive())
                            lulin->AI()->Talk(TALK_SUEN_DEATH);
                }

                SendActionToImages(ACTION_CELESTIALS_NOT_INTERRACT);

                if (Creature* worldTrigger_1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                    worldTrigger_1->RemoveAllAuras();

                if (Creature* worldTrigger_2 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER + 1) : ObjectGuid::Empty))
                    worldTrigger_2->RemoveAllAuras();

                HandleInactivityCreatures();
                DoCast(me, SPELL_DESPAWN_ICE_COMMET, true);
                me->RemoveAllAreasTrigger();
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkEvents.Update(diff);

                while (uint32 eventId = berserkEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_BERSERK)
                    {
                        DoCast(me, SPELL_BERSERK, true);

                        if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                            lulin->CastSpell(lulin, SPELL_BERSERK, true);

                    }
                    break;
                }

                // Handle check tear of the sun cuz we`d remove this state for moving
                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_TEAR_OF_THE_SUN))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SWITCH_DAY:
                            SetPhase(PHASE_DAY);
                            break;
                        case EVENT_SWITCH_TWILIGHT:
                            SetPhase(PHASE_TWILIGHT);
                            break;
                        case EVENT_LIGHT_OF_DAY:
                            DoCast(me, SPELL_LIGHT_OF_DAY_SELECTOR);
                            events.ScheduleEvent(EVENT_LIGHT_OF_DAY, 6 * IN_MILLISECONDS);
                            break;
                        case EVENT_TEAR_OF_THE_SUN:
                            DoCast(me, SPELL_TEAR_OF_THE_SUN);
                            events.ScheduleEvent(EVENT_TEAR_OF_THE_SUN, 23 * IN_MILLISECONDS);
                            break;
                        case EVENT_INTRO:
                            Talk(TALK_SWITCH_NIGHT_TO_DAY);
                            break;
                        case EVENT_FLAMES_OF_PASSION:
                            if (Unit* vict = me->GetVictim())
                            {
                                targetGUID = vict->GetGUID();
                                targetLowGUID = vict->GetGUID().GetCounter();
                            }

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                            {
                                me->PrepareChanneledCast(target->GetGUID());
                                DoCast(target, SPELL_FLAMES_OF_PASSION_CHARGE, true);
                            }
                            else if (Unit* target = GetFarthestPlayerInArea(me, 200.0f))
                            {
                                me->PrepareChanneledCast(target->GetGUID());
                                DoCast(target, SPELL_FLAMES_OF_PASSION_CHARGE, true);
                            }

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_TWIN_CONSORTS) == IN_PROGRESS && !me->HasAura(SPELL_PERMANENT_FEIGN_DEATH); })
                                .Schedule(Milliseconds(3000), [this](TaskContext context)
                            {
                                me->RemoveChanneledCast(targetGUID);
                            });

                            events.ScheduleEvent(EVENT_FLAMES_OF_PASSION, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_FAN_OF_FLAMES:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FAN_OF_FLAMES);

                            events.ScheduleEvent(EVENT_FAN_OF_FLAMES, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_NUCLEAR_INFERNO:
                            DoCast(me, SPELL_NUCLEAR_INFERNO);
                            events.RescheduleEvent(EVENT_LIGHT_OF_DAY, 15 * IN_MILLISECONDS);
                            events.RescheduleEvent(EVENT_FLAMES_OF_PASSION, 16 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_NUCLEAR_INFERNO, 50 * IN_MILLISECONDS);
                            break;
                    }
                }

                EnterEvadeIfOutOfCombatArea(diff);
                DoMeleeAttackIfReady();
            }

            private:
                void SendActionToImages(uint32 actionId)
                {
                    std::list<Creature*> fadedImagesList;
                    GetCreatureListWithEntryInGrid(fadedImagesList, me, NPC_FADED_IMAGE_OF_CHI_JI, 200.0f);
                    GetCreatureListWithEntryInGrid(fadedImagesList, me, NPC_FADED_IMAGE_OF_NIUZAO, 200.0f);
                    GetCreatureListWithEntryInGrid(fadedImagesList, me, NPC_FADED_IMAGE_OF_XUEN, 200.0f);
                    GetCreatureListWithEntryInGrid(fadedImagesList, me, NPC_FADED_IMAGE_OF_YULON, 200.0f);

                    for (auto&& itr : fadedImagesList)
                        itr->AI()->DoAction(actionId);
                }

                void HandleInactivityCreatures()
                {
                    std::list<Creature*> lotusList;
                    GetCreatureListWithEntryInGrid(lotusList, me, NPC_MOON_LOTUS, 200.0f);

                    for (auto&& itr : lotusList)
                        itr->RemoveAllAuras();
                }

                bool hasLulinAlive()
                {
                    if (Creature* lulin = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
                        if (lulin->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                            return false;

                    return true;
                }
        };
    
        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_suenAI(creature);
        }
};

// Lu`lin 68905
class boss_lulin : public CreatureScript
{
    public:
        boss_lulin() : CreatureScript("boss_lulin") { }
    
        struct boss_lulinAI : public ScriptedAI
        {
            boss_lulinAI(Creature* creature) : ScriptedAI(creature), summons(me)
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
            }

            EventMap events;
            SummonList summons;
            InstanceScript* instance;
            TaskScheduler scheduler;
            bool atEvade;
            bool feignDeath;
            bool isTwilight;
            bool isDefeat;
            ObjectGuid targetGUID;

            void Reset() override
            {
                events.Reset();
                scheduler.CancelAll();
                atEvade = false;
                feignDeath = false;
                isDefeat = false;
                isTwilight = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_AGGRESSIVE);
                targetGUID = ObjectGuid::Empty;

                // Temp Hackfix
                if (instance && instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL)
                {
                    // should be 219.8m in 10 normal
                    me->SetCreateHealth(219800000);
                    me->SetMaxHealth(219800000);
                    me->SetHealth(219800000);
                    me->ResetPlayerDamageReq();
                }

                // if feign death remove
                me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });
            }

            void JustEngagedWith(Unit* who) override
            {
                me->setActive(true, ActiveFlags::InCombat);
                DoZoneInCombat();

                if (Creature* suen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                    DoZoneInCombat(suen, 200.0f);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_MOON_LOTUS, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_COSMIC_BARRAGE, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BEAST_OF_NIGHTMARES, 20 * IN_MILLISECONDS);

                if (me->GetMap()->IsHeroic())
                    events.ScheduleEvent(EVENT_SPAWN_LURKER, 10 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_DEEP_SUBMERGE)
                {
                    summon->SetInCombatWithZone();
                    summon->CastSpell(summon, SPELL_TIDAL_FORCE_AURA_LAUNCH, true);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                // Feign Death
                if (feignDeath)
                {
                    damage = 0;
                    return;
                }

                if (damage >= me->GetHealth() && !feignDeath && hasSuenAlive())
                {
                    feignDeath = true;
                    damage = 0;

                    events.Reset();
                    me->RemoveAllAuras();
                    me->SetHealth(1);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->PrepareChanneledCast(me->GetOrientation());
                    DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);

                    if (Creature* suen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                        if (suen->GetHealthPct() > 30.0f)
                            me->GetMap()->SetWorldState(WORLDSTATE_FROM_DUSK_TILL_DOWN, 1);

                    if (instance)
                    {
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                        // If not Twilight Phase
                        if (Creature* suen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_TWIN_CONSORTS)))
                            suen->AI()->DoAction(ACTION_LAST_STAND);
                    }

                    return;
                }
            }

            void EnterEvadeMode() override
            {
                if (atEvade)
                    return;

                atEvade = true;

                ScriptedAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    if (Creature* suen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_TWIN_CONSORTS)))
                        suen->AI()->EnterEvadeMode();
                }

                summons.DespawnAll();
                scheduler.CancelAll();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);

                uint32 corpseDelay = me->GetCorpseDelay();
                uint32 respawnDelay = me->GetRespawnDelay();

                me->SetCorpseDelay(1);
                me->SetRespawnDelay(29);

                me->DespawnOrUnsummon();

                me->SetCorpseDelay(corpseDelay);
                me->SetRespawnDelay(respawnDelay);
                me->GetMotionMaster()->MoveTargetedHome();

                me->RemoveAurasDueToSpell(SPELL_DISSIPATE_LULIN);
            }

            void JustDied(Unit* killer) override 
            {
                if (isDefeat)
                    return;

                isDefeat = true;

                events.Reset();
                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    if (Creature* suen = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_TWIN_CONSORTS)))
                        suen->AI()->JustDied(killer);
                }
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_START_INTRO:
                        events.ScheduleEvent(EVENT_INTRO, 3 * IN_MILLISECONDS);
                        break;
                    case ACTION_LULIN_DAY:
                    {
                        Talk(TALK_SWITCH_NIGHT_TO_DAY);
                        DoCast(me, SPELL_MOON_LOTUS_HIDE, true);

                        // Remove wave from Lotis
                        std::list<Creature*> lotusWaveList;
                        GetCreatureListWithEntryInGrid(lotusWaveList, me, NPC_MOON_LOTUS_WAVE, 200.0f);

                        for (auto&& itr : lotusWaveList)
                            itr->DespawnOrUnsummon();

                        events.Reset();
                        me->PrepareChanneledCast(me->GetOrientation());
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        DoCast(me, SPELL_DISSIPATE_LULIN, true);
                        events.ScheduleEvent(EVENT_ICE_COMET, 16 * IN_MILLISECONDS);
                        break;
                    }
                    case ACTION_LULIN_TWILIGHT:
                        isTwilight = true;

                        Talk(TALK_SWITCH_DAY_TO_TWILIGHT);
                        // Set Lulin in Active State
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVISIBLE_LULIN);
                        events.ScheduleEvent(EVENT_TIDAL_FORCE, 11 * IN_MILLISECONDS);
                        break;
                    case ACTION_LAST_STAND:
                        events.CancelEvent(EVENT_TIDAL_FORCE);
                        events.CancelEvent(EVENT_ICE_COMET);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_INVISIBLE_LULIN);
                        events.ScheduleEvent(EVENT_COSMIC_BARRAGE, 7 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BEAST_OF_NIGHTMARES, 20 * IN_MILLISECONDS);

                        if (me->GetMap()->IsHeroic())
                            events.ScheduleEvent(EVENT_SPAWN_LURKER, 10 * IN_MILLISECONDS);

                        DoCast(me, SPELL_MOON_LOTUS_REAPPEAR, true);
                        break;
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
                    switch (eventId)
                    {
                        case EVENT_INTRO:
                            Talk(TALK_INTRO);
                            break;
                        case EVENT_COSMIC_BARRAGE:
                            DoCast(me, SPELL_COSMIC_BARRAGE);
                            events.ScheduleEvent(EVENT_COSMIC_BARRAGE, 23 * IN_MILLISECONDS);
                            break;
                        case EVENT_MOON_LOTUS:
                            DoCast(me, SPELL_MOON_LOTUS_BLOOM);
                            events.ScheduleEvent(EVENT_MOON_LOTUS, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_BEAST_OF_NIGHTMARES:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_BEAST_OF_NIGHTMARES);

                            events.ScheduleEvent(EVENT_BEAST_OF_NIGHTMARES, 60 * IN_MILLISECONDS);
                            break;
                        case EVENT_ICE_COMET:
                            if (Creature* suen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                            {
                                uint32 currentSpellId = suen->GetCurrentSpell(CURRENT_CHANNELED_SPELL) ? suen->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->m_spellInfo->Id : 0;

                                if (currentSpellId == SPELL_NUCLEAR_INFERNO)
                                {
                                    events.RescheduleEvent(EVENT_ICE_COMET, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                                    break;
                                }
                            }

                            if (Creature* suen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                                suen->AI()->DoAction(ACTION_LIGHT_OF_DAY_DELAY);

                            DoCast(me, SPELL_ICE_COMET_SELECTOR, true);
                            events.ScheduleEvent(EVENT_ICE_COMET, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_TIDAL_FORCE:
                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation(), SPELL_TIDAL_FORCE);
                            events.ScheduleEvent(EVENT_TIDAL_FORCE, 71 * IN_MILLISECONDS);

                            scheduler
                                .SetValidator([this] { return instance && instance->GetBossState(DATA_TWIN_CONSORTS) == IN_PROGRESS; })
                                .Schedule(Milliseconds(18500), [this](TaskContext context)
                            {
                                me->RemoveChanneledCast(targetGUID);
                            });
                            break;
                        case EVENT_SPAWN_LURKER:
                            if (Creature* worldTrigger_1 = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                                me->SummonCreature(NPC_LURKER_IN_THE_NIGHT, worldTrigger_1->GetPositionX() + frand(-50.0f, 50.0f), worldTrigger_1->GetPositionY() + frand(-50.0f, 50.0f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);

                            events.ScheduleEvent(EVENT_SPAWN_LURKER, 10 * IN_MILLISECONDS);
                            break;
                    }
                }

                EnterEvadeIfOutOfCombatArea(diff);
                DoMeleeAttackIfReady();
            }

            private:
                bool hasSuenAlive()
                {
                    if (Creature* suen = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                        if (suen->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                            return false;

                    return true;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lulinAI(creature);
        }
};

// Faded Image of Celestial 69777, 69774, 69771, 69779
struct npc_faded_image_of_celestial : public ScriptedAI
{
    npc_faded_image_of_celestial(Creature* creature) : ScriptedAI(creature) { }

    bool hasActivated;
    uint32 seqValue;
    ObjectGuid clickerGUID;

    void Reset() override
    {
        hasActivated = false;
        seqValue = 1;
        clickerGUID = ObjectGuid::Empty;
        me->SetVisible(true);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_CELESTIALS_NOT_INTERRACT:
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                me->RemoveAurasDueToSpell(SPELL_UNINTERACTABLE);
                Reset();
                break;
            case ACTION_CELESTIALS_INTERRACT:
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                Reset();
                break;
            case ACTION_CELESTIAL_ASTRAL_FADED:
                if (!hasActivated)
                {
                    me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                    seqValue = 1;
                    clickerGUID = ObjectGuid::Empty;

                    // Reset Drawing Seq
                    if (Creature* sulin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                        sulin->AI()->DoAction(ACTION_CELESTIAL_DRAWING_DONE);
                }
                break;
            case ACTION_CELESTIAL_DRAWING_DONE:
                hasActivated = true;

                if (Unit* clicker = ObjectAccessor::GetUnit(*me, clickerGUID))
                    clicker->RemoveAurasDueToSpell(invCelestialsType.find(me->GetEntry())->second);

                if (Creature* worldTrigger = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_WORLD_TRIGGER) : ObjectGuid::Empty))
                    worldTrigger->CastSpell(worldTrigger, celestialsList.find(me->GetEntry())->second, true);

                me->SetVisible(false);
                break;
            case ACTION_CELESTIAL_REBIRTH:
                Reset();
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_STAR_IN_SEQ)
            return seqValue;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        seqValue = data;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (Creature* sulin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
            sulin->AI()->SetData(TYPE_SELECTED_CELESTIAL, me->GetEntry());

        DoCast(me, SPELL_UNINTERACTABLE, true);
        clickerGUID = clicker->GetGUID();
        clicker->AddAura(invCelestialsType.find(me->GetEntry())->second, clicker);

        for (auto&& itr : starSumList)
            DoCast(me, itr, true);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Celestial Star 70124
struct npc_celestial_star : public ScriptedAI
{
    npc_celestial_star(Creature* creature) : ScriptedAI(creature) { }

    uint32 sequenceValue;
    uint32 isNext;
    uint32 ownerEntry;
    ObjectGuid starGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        isNext = 0;
        starGUID = ObjectGuid::Empty;
        me->SetPhaseMask(2, true);

        if (summoner && summoner->ToCreature())
        {
            sequenceValue = summoner->ToCreature()->AI()->GetData(TYPE_STAR_IN_SEQ);
            summoner->ToCreature()->AI()->SetData(TYPE_STAR_IN_SEQ, ++sequenceValue);

            uint32 tempDraw = 0;

            switch (summoner->GetEntry())
            {
                case NPC_FADED_IMAGE_OF_NIUZAO:
                    tempDraw = 5;
                    break;
                case NPC_FADED_IMAGE_OF_CHI_JI:
                    tempDraw = 6;
                    break;
                case NPC_FADED_IMAGE_OF_XUEN:
                    tempDraw = 2;
                    break;
                case NPC_FADED_IMAGE_OF_YULON:
                    tempDraw = 4;
                    break;
            }

            if (sequenceValue - 1 == tempDraw)
            {
                DoCast(me, SPELL_MOONBEAM_STATE_WORLD, true);
                isNext = 1;
            }

            me->SetDisplayFromModel(1);
            DoCast(me, SPELL_STAR_STATE_INACTIVE, true);
            DoCast(me, invStarSequenceType.find(sequenceValue - 1)->second[0], true);
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_STAR_IN_SEQ:
                return sequenceValue - 1;
            case TYPE_NEXT_IN_LINK:
                return isNext;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_NEXT_IN_LINK)
            isNext = data;
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        starGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return starGUID;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_CELESTIAL_DRAWING_WRONG)
        {
            starGUID = ObjectGuid::Empty;

            if (!me->HasAura(SPELL_MOONBEAM_STATE_WORLD))
                isNext = 0;

            me->RemoveAurasDueToSpell(SPELL_STAR_STATE_ACTIVE);
            DoCast(me, SPELL_STAR_STATE_INACTIVE, true);

            // Reset counter of right lines
            if (Creature* sulin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                sulin->AI()->DoAction(ACTION_CELESTIAL_DRAWING_WRONG);
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Moon Lotus Wave 69340
struct npc_moon_lotus_wave : public ScriptedAI
{
    npc_moon_lotus_wave(Creature* creature) : ScriptedAI(creature) { }

    float x, y;

    void Reset() override
    {
        if (Creature* lulin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
            lulin->AI()->JustSummoned(me);

        me->SetSpeed(MOVE_RUN, 0.35f);
        x = 0.0f; y = 0.0f;
        DoCast(me, SPELL_MOON_LOTUS_AURA, true);

        GetPositionWithDistInOrientation(me, 120.0f, frand(0.0f, 2 * M_PI), x, y);

        Movement::MoveSplineInit init(me);
        init.MoveTo(x, y, me->GetPositionZ());

        init.SetSmooth();
        init.SetUncompressed();
        init.Launch();
        me->DespawnOrUnsummon(me->GetSplineDuration());
    }

    void UpdateAI(uint32 diff) override { }
};

// Crashing Star 69365, 69477
struct npc_crashing_star : public ScriptedAI
{
    npc_crashing_star(Creature* creature) : ScriptedAI(creature) { }

    // 69365 used by celestial shaper too
    void IsSummonedBy(Unit* summoner) override
    {
        if (summoner->GetEntry() == NPC_SHANZE_CELESTIAL_SHAPER)
            DoCast(me, SPELL_LAUNCH_STAR, true);
    }

    void Reset() override
    {
        me->SetDisplayFromModel(1);
        DoCast(me, SPELL_STAR_STATE_INACTIVE, true);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_LAUNCH_STAR)
            DoCast(me, SPELL_LAUNCH_STAR, true);
    }

    void UpdateAI(uint32 diff) override { }
};

// Beast of Nightmares 69479
struct npc_beast_of_nightmares : public ScriptedAI
{
    npc_beast_of_nightmares(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid summonerGUID;
    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();

        DoCast(me, SPELL_BEAST_OF_NIGHTMATRES_AURA);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (Creature* lulin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
            lulin->AI()->JustSummoned(me);

        me->ClearUnitState(UNIT_STATE_CASTING);
        AttackStart(summoner);

        scheduler.Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            me->SetPhaseMask(2, true); // hacky, should`ve visible for whole, but attackable only by victim
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Unit* summoner = ObjectAccessor::GetUnit(*me, summonerGUID))
            summoner->RemoveAurasDueToSpell(SPELL_BEAST_OF_NIGHTMATRES_AURA);
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Ice Comet 69596
struct npc_ice_comet : public ScriptedAI
{
    npc_ice_comet(Creature* creature) : ScriptedAI(creature) { }

    ObjectGuid tombGUID;
    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        // this creatures shouldn`t take collisions
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);

        me->setRegeneratingHealth(false);
        DoCast(me, SPELL_ICE_COMET_AURA);

        if (GameObject* tomb = me->SummonGameObject(GO_ICE_TOMB, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), { }, 0))
            tombGUID = tomb->GetGUID();

        if (Creature* suen = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
            DoCast(suen, SPELL_ICE_COMET_FACE_SUEN);

        // Areatrigger have some issue with low duration and LoS
        scheduler.Schedule(Milliseconds(500), [this](TaskContext context)
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, me, 10.0f);

            if (Creature* suen = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
            {
                for (auto&& itr : pList)
                    if (me->GetExactDist2d(itr) < 5.5f && !itr->IsWithinLOS(suen->GetPositionX(), suen->GetPositionY(), suen->GetPositionZ()))
                        DoCast(itr, SPELL_ICY_SHADOWS_EFF, true);
                    else
                        itr->RemoveAurasDueToSpell(SPELL_ICY_SHADOWS_EFF, me->GetGUID());
            }

            context.Repeat(Milliseconds(500));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (GameObject* tomb = ObjectAccessor::GetGameObject(*me, tombGUID))
            tomb->Delete();
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Lurker in the Night 69591
struct npc_lurker_in_the_night : public ScriptedAI
{
    npc_lurker_in_the_night(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* lulin = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
            lulin->AI()->JustSummoned(me);

        DoCast(me, SPELL_INVIS, true);
        me->SetInCombatWithZone();

        scheduler
            .SetValidator([this] { return me->HasAura(SPELL_INVIS); })
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            DoCast(me, SPELL_DARKNESS);

            context.Repeat(Milliseconds(2500));
        });
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_INIT_CHARGE)
            if (Unit* vict = me->GetVictim())
                DoStartMovement(vict);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        if (!me->HasAura(SPELL_INVIS))
            DoMeleeAttackIfReady();
    }
};

// Tear of the Sun Selector 138744
class spell_tear_of_the_sun_selector : public SpellScript
{
    PrepareSpellScript(spell_tear_of_the_sun_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_TEAR_OF_THE_SUN_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove tanks if possible
            pList.remove_if(TankSpecTargetSelector());

            // Get real victim size
            uint32 maxVictims = caster->GetInstanceScript()->instance->Is25ManRaid() ? urand(3, 4) : urand(1, 2);

            // if we have enough players without tank
            if (pList.size() >= maxVictims)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > maxVictims)
                    Trinity::Containers::RandomResizeList(targets, maxVictims);

                return;
            }

            // if we haven`t enough players without tank, then just push whole raid to container
            if (targets.size() > maxVictims)
                Trinity::Containers::RandomResizeList(targets, maxVictims);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_tear_of_the_sun_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tear_of_the_sun_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Tear of the Sun Eff 137405
class spell_tear_of_the_sun_eff : public SpellScript
{
    PrepareSpellScript(spell_tear_of_the_sun_eff);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove tanks if possible
            pList.remove_if(TankSpecTargetSelector());

            // Get real victim size
            uint32 maxVictims = caster->GetInstanceScript()->instance->Is25ManRaid() ? urand(3, 4) : urand(1, 2);

            // if we have enough players without tank
            if (pList.size() >= maxVictims)
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > maxVictims)
                    Trinity::Containers::RandomResizeList(targets, maxVictims);

                return;
            }

            // if we haven`t enough players without tank, then just push whole raid to container
            if (targets.size() > maxVictims)
                Trinity::Containers::RandomResizeList(targets, maxVictims);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_tear_of_the_sun_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Tear of the Sun 137404
class spell_tear_of_the_sun : public AuraScript
{
    PrepareAuraScript(spell_tear_of_the_sun);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->ClearUnitState(UNIT_STATE_CASTING);
            owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            owner->SetReactState(REACT_AGGRESSIVE);
        }
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            owner->CastSpell(owner, SPELL_DISSIPATE_SUEN, true);
            owner->SetReactState(REACT_PASSIVE);
            owner->AttackStop();
        }
    }

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, owner->HasAura(SPELL_XUEN_BLESSED_ALACRITY) ? SPELL_TEAR_OF_THE_SUN_SELECTOR : SPELL_TEAR_OF_THR_SUN_EFF, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_tear_of_the_sun::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_tear_of_the_sun::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_tear_of_the_sun::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Light of Day Selector 138801
class spell_light_of_day_selector : public SpellScript
{
    PrepareSpellScript(spell_light_of_day_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_LIGHT_OF_DAY, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove meelee and tanks if possible
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // if we have enough players without tanks and meelee
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players without tank and meelee, then just push whole raid to container
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_light_of_day_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_light_of_day_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Blazing Radiance 137411
class spell_blazing_radiance : public AuraScript
{
    PrepareAuraScript(spell_blazing_radiance);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->CastSpell(owner, SPELL_BLAZING_RADIANCE_EFF, true);

            if (!owner->HasAura(SPELL_ICE_COMET_CHILLED)) // Prevent stackable if in front comet 
                owner->CastSpell(owner, SPELL_BLAZING_RADIANCE_AURA, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_blazing_radiance::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Moon Lotus Bloom Aura 137192
class spell_moon_lotus_bloom_aura : public AuraScript
{
    PrepareAuraScript(spell_moon_lotus_bloom_aura);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_MOON_LOTUS_PERIODIC, true);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->RemoveAurasDueToSpell(SPELL_MOON_LOTUS_PERIODIC);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_moon_lotus_bloom_aura::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_moon_lotus_bloom_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Moon Lotus Bloom Aura 137192
class spell_moon_lotus_bloom_aura_eff : public SpellScript
{
    PrepareSpellScript(spell_moon_lotus_bloom_aura_eff);

    void HandleOnHit(SpellEffIndex effIndex)
    {
        if (Unit* target = GetHitUnit())
            if (target->GetInstanceScript() && target->GetInstanceScript()->GetBossState(DATA_TWIN_CONSORTS) == IN_PROGRESS) // prevent missle effect after evade
                return;

        PreventHitEffect(effIndex);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_moon_lotus_bloom_aura_eff::HandleOnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Moon Lotus Bloom 137118
class spell_moon_lotus_bloom : public SpellScript
{
    PrepareSpellScript(spell_moon_lotus_bloom);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_MOON_LOTUS_BLOOM_AURA, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (target->GetEntry() != NPC_MOON_LOTUS || (target->ToUnit() && target->ToUnit()->HasAura(SPELL_MOON_LOTUS_BLOOM_AURA))); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_moon_lotus_bloom::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_moon_lotus_bloom::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Crashing Star Jump 139561 (slow version)
class spell_crashing_star_jump : public SpellScript
{
    PrepareSpellScript(spell_crashing_star_jump);

    void HandleJump(SpellEffIndex /*effIdx*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_CRASHING_STAR_EFF, true);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_crashing_star_jump::HandleJump, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// Summon Star 138199, 138202, 138205, 138219, 138221, 138224, 138227
class spell_celestial_summon_star : public SpellScript
{
    PrepareSpellScript(spell_celestial_summon_star);

    void HandleEffectAreaTrigger(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (castedByCelestial(caster->GetEntry()))
                PreventEffect(effIndex);
    }

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (!castedByCelestial(caster->GetEntry()))
                PreventEffect(effIndex);
    }

    bool castedByCelestial(uint32 casterEntry)
    {
        for (auto&& itr : celestialsList)
            if (itr.first == casterEntry)
                return true;

        return false;
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_celestial_summon_star::HandleEffectAreaTrigger, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectHit += SpellEffectFn(spell_celestial_summon_star::HandleEffectHit, EFFECT_1, SPELL_EFFECT_SUMMON);
    }
};

// Twin Consort Dissipate 137105, 137187
class spell_twin_consort_dissipate : public AuraScript
{
    PrepareAuraScript(spell_twin_consort_dissipate);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (owner->IsAlive())
                owner->CastSpell(owner, owner->GetEntry() == NPC_SUEN ? SPELL_INVISIBLE_SUEN : SPELL_INVISIBLE_LULIN, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_twin_consort_dissipate::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Moon Lotus Periodic 136690
class spell_moon_lotus_periodic : public AuraScript
{
    PrepareAuraScript(spell_moon_lotus_periodic);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            if (!owner->HasAura(SPELL_MOON_LOTUS_HIDE_AURA))
                owner->CastSpell(owner, SPELL_MOON_LOTUS_SUMM, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_moon_lotus_periodic::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Moon Lotus Hide 137195
class spell_moon_lotus_hide_selector : public SpellScript
{
    PrepareSpellScript(spell_moon_lotus_hide_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_MOON_LOTUS_HIDE_AURA, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetEntry() != NPC_MOON_LOTUS; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_moon_lotus_hide_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_moon_lotus_hide_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Reappear Moon Lotus 137196
class spell_moon_lotus_reappear_selector : public SpellScript
{
    PrepareSpellScript(spell_moon_lotus_reappear_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->RemoveAurasDueToSpell(SPELL_MOON_LOTUS_HIDE_AURA);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetEntry() != NPC_MOON_LOTUS; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_moon_lotus_reappear_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_moon_lotus_reappear_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Cosmic Barrage 136752
class spell_cosmic_barrage : public AuraScript
{
    PrepareAuraScript(spell_cosmic_barrage);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, owner->HasAura(SPELL_XUEN_BLESSED_ALACRITY) ? SPELL_COSMIC_BARRAGE_SUMM_2 : SPELL_COSMIC_BARRAGE_SUMM, true);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        // Handle Set Stars in Launch State
        if (Unit* owner = GetOwner()->ToUnit())
        {
            std::list<Creature*> starsList;
            GetCreatureListWithEntryInGrid(starsList, owner, NPC_CRASHING_STAR, 100.0f);
            GetCreatureListWithEntryInGrid(starsList, owner, NPC_CRASHING_STAR_SLOW, 100.0f);

            for (auto&& itr : starsList)
                itr->AI()->DoAction(ACTION_LAUNCH_STAR);
        }
    }

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            if (owner->GetMap()->Is25ManRaid()) // in 25 mode it should`ve summon 12 stars
                SetDuration(6 * IN_MILLISECONDS);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_cosmic_barrage::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectRemove += AuraEffectRemoveFn(spell_cosmic_barrage::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_cosmic_barrage::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Cosmic Barrage Summ 136768, 137164
class spell_cosmic_barrage_summon : public SpellScript
{
    PrepareSpellScript(spell_cosmic_barrage_summon);

    void SelectTargets(SpellDestination& dest)
    {
        Position newPos = { dest._position.GetPositionX(), dest._position.GetPositionY(), frand(153.25f, 158.0f), dest._position.GetOrientation() };
        dest.Relocate(newPos);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_cosmic_barrage_summon::SelectTargets, EFFECT_1, TARGET_DEST_DEST_RADIUS);
    }
};

// Launch Star 137139
class spell_launch_star : public AuraScript
{
    PrepareAuraScript(spell_launch_star);

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_LAUNCH_STAR_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_launch_star::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Launch Star Eff 137120
class spell_launch_star_eff : public SpellScript
{
    PrepareSpellScript(spell_launch_star_eff);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->CastSpell(target, SPELL_CRASHING_STAR_SELECT_AURA, true);
                caster->CastSpell(target, caster->GetEntry() == NPC_CRASHING_STAR ? SPELL_CRASHING_STAR_JUMP : SPELL_CRASHING_STAR_JUMP_SLOW, false);
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove meelee, tanks and affected by select aura if possible
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());
            pList.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_CRASHING_STAR_SELECT_AURA); });

            // if we have enough players without tanks, meelee and affected by aura 
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players without tanks, meelee and aura affected, then just select anyone
            targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_CRASHING_STAR_SELECT_AURA); });

            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_launch_star_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_launch_star_eff::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Flames of Passion Charge Back 137415
class spell_flames_of_passion_charge_back : public SpellScript
{
    PrepareSpellScript(spell_flames_of_passion_charge_back);

    void HandleBeforeCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_INIT_CHARGE);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_flames_of_passion_charge_back::HandleBeforeCast);
    }
};

// Flames of Passion Eff 137414
class spell_flames_of_passion_eff : public SpellScript
{
    PrepareSpellScript(spell_flames_of_passion_eff);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                caster->CastSpell(target, SPELL_FLAMES_OF_PASSION_CHARGE_2, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_flames_of_passion_eff::HandleAfterCast);
    }
};

// Flames of Passion Charge 137412
class spell_flames_of_passion_charge : public SpellScript
{
    PrepareSpellScript(spell_flames_of_passion_charge);

    void HandleCharge(SpellEffIndex /*effIdx*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_FLAMES_OF_PASSION_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_flames_of_passion_charge::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
    }
};

// Ice Comet Selector 138795
class spell_ice_comet_selector : public SpellScript
{
    PrepareSpellScript(spell_ice_comet_selector);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_ICE_COMET_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Player*> pList;
            GetPlayerListInGrid(pList, caster, 200.0f);

            // Remove meelee, tanks and affected by select aura if possible
            pList.remove_if(TankSpecTargetSelector());
            pList.remove_if(MeeleSpecTargetSelector());

            // if we have enough players without tanks, meelee and affected by aura 
            if (!pList.empty())
            {
                targets.clear();

                for (auto&& itr : pList)
                    targets.push_back(itr);

                if (pList.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);

                return;
            }

            // if we haven`t enough players without tanks and meelee, then just select anyone
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ice_comet_selector::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ice_comet_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Despawn Ice Comet 138679
class spell_despawn_ice_comet : public SpellScript
{
    PrepareSpellScript(spell_despawn_ice_comet);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (GameObject* target = GetHitGObj())
            target->Delete();
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToGameObject() && target->ToGameObject()->GetEntry() != GO_ICE_TOMB; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_despawn_ice_comet::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_ACTIVATE_OBJECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_despawn_ice_comet::SelectTargets, EFFECT_0, TARGET_GAMEOBJECT_SRC_AREA);
    }
};

// Tidal Force 137715
class spell_tidal_force : public SpellScript
{
    PrepareSpellScript(spell_tidal_force);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Creature* lulin = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(NPC_LULIN) : ObjectGuid::Empty))
            {
                // Prevent Trigger Proc if Lulin has dead state
                if (lulin->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                    return;

                float ori = frand(0.0f, 2 * M_PI);
                caster->SetFacingTo(ori);
                caster->CastSpell(caster, SPELL_TIDAL_FORCE_VISUAL_WAVE, true);
                caster->CastSpell(caster, SPELL_TIDAL_FORCE_CONE_EFF, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_tidal_force::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Ice Comet Chilled 137483
class spell_ice_comet_chilled : public SpellScript
{
    PrepareSpellScript(spell_ice_comet_chilled);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            target->RemoveAuraFromStack(SPELL_BLAZING_RADIANCE_AURA);

        if (Unit* caster = GetCaster())
            caster->DealDamage(caster, caster->GetMaxHealth() * 0.05, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetEntry() != NPC_SUEN; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ice_comet_chilled::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ice_comet_chilled::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_ice_comet_chilled::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Beast of Nightmares Aura 137341
class spell_beast_of_nightmares : public AuraScript
{
    PrepareAuraScript(spell_beast_of_nightmares);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->SetPhaseMask(3, true);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->SetPhaseMask(1, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_beast_of_nightmares::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_FORCE_REACTION, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_beast_of_nightmares::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_FORCE_REACTION, AURA_EFFECT_HANDLE_REAL);
    }
};

// Expose Lurkers 137406
class spell_expose_lurkers : public SpellScript
{
    PrepareSpellScript(spell_expose_lurkers);

    void HandleEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
        {
            target->RemoveAurasDueToSpell(SPELL_INVIS);
            target->AI()->DoAction(ACTION_INIT_CHARGE);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_LURKER_IN_THE_NIGHT; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_expose_lurkers::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_expose_lurkers::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Darkness 137382
class spell_lurker_darkness : public SpellScript
{
    PrepareSpellScript(spell_lurker_darkness);

    std::list<WorldObject*> m_targets, m_copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        if (Unit* caster = GetCaster())
        {
            uint8 targetCount = caster->GetMap()->Is25ManRaid() ? 3 : 1;

            targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
            targets.remove_if(TankSpecTargetSelector());

            if (targets.size() >= targetCount)
                Trinity::Containers::RandomResizeList(targets, targetCount);
            else if (!m_targets.empty())
            {
                targets.clear();

                for (auto&& itr : m_targets)
                    targets.push_back(itr);

                targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });

                if (targets.size() >= targetCount)
                    Trinity::Containers::RandomResizeList(targets, targetCount);
            }

            m_copyTargets = targets;
        }
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lurker_darkness::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_lurker_darkness::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Nuclear Inferno 137491
class spell_nuclear_inferno : public AuraScript
{
    PrepareAuraScript(spell_nuclear_inferno);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_NUCLEAR_INFERNO_DAY_EFF, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_nuclear_inferno::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Nuclear Inferno Eff 137492, 137524
class spell_nuclear_inferno_eff : public SpellScript
{
    PrepareSpellScript(spell_nuclear_inferno_eff);

    void RecalculateDamage(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
            SetHitDamage(target->HasAura(SPELL_ICY_SHADOWS_EFF) ? (int32)(sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, target->GetMap()->GetDifficulty())->Effects[0].BasePoints * 0.4f) : sSpellMgr->GetSpellInfo(GetSpellInfo()->Id, target->GetMap()->GetDifficulty())->Effects[0].BasePoints);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_nuclear_inferno_eff::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Lurker Invis 137106
class spell_lurker_invis : public AuraScript
{
    PrepareAuraScript(spell_lurker_invis);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_lurker_invis::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_lurker_invis::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// Uninteractible 138185
class spell_celestial_uninteractible : public SpellScript
{
    PrepareSpellScript(spell_celestial_uninteractible);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_FADED_IMAGE_OF_NIUZAO && target->GetEntry() != NPC_FADED_IMAGE_OF_CHI_JI && target->GetEntry() != NPC_FADED_IMAGE_OF_XUEN && target->GetEntry() != NPC_FADED_IMAGE_OF_YULON; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_celestial_uninteractible::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Uninteractible 138185
class spell_celestial_uninteractible_aura : public AuraScript
{
    PrepareAuraScript(spell_celestial_uninteractible_aura);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void HandleAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_CELESTIAL_ASTRAL_FADED);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_celestial_uninteractible_aura::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_celestial_uninteractible_aura::HandleAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class StarSequencePredicate 
{
    public:
        StarSequencePredicate(Creature* const m_caster) : _caster(m_caster) { }
    
        bool operator()(WorldObject* object)
        {
            if (Creature* sulin = ObjectAccessor::GetCreature(*_caster, object->GetInstanceScript() ? object->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
            {
                switch (sulin->AI()->GetData(TYPE_SELECTED_CELESTIAL))
                {
                    case NPC_FADED_IMAGE_OF_CHI_JI:
                        celestialMap = invCraneType;
                        break;
                    case NPC_FADED_IMAGE_OF_NIUZAO:
                        celestialMap = invOxType;
                        break;
                    case NPC_FADED_IMAGE_OF_XUEN:
                        celestialMap = invTigerType;
                        break;
                    case NPC_FADED_IMAGE_OF_YULON:
                        celestialMap = invSerpentType;
                        break;
                }

                uint32 currentKey = _caster->AI()->GetData(TYPE_STAR_IN_SEQ);
                
                return sulin->AI()->GetData(TYPE_SELECTED_CELESTIAL) == NPC_FADED_IMAGE_OF_CHI_JI && sulin->AI()->GetData(TYPE_ACTIVE_STAR_COUNTER) >= 4 ? object && object->ToCreature() && object->ToCreature()->AI()->GetData(TYPE_STAR_IN_SEQ) != 5 :
                    object && object->ToCreature() && object->ToCreature()->AI()->GetData(TYPE_STAR_IN_SEQ) != celestialMap.find(currentKey)->second;
            }

            return true;
        }
    
    private:
        Creature const* _caster;
        std::map<uint32, uint32> celestialMap;
};

// Celestial Link Star 138200, 138203, 138218, 138220, 138223, 138226, 138204
class spell_celestial_link_star : public SpellScript
{
    PrepareSpellScript(spell_celestial_link_star);

    void HandleDummyHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
        {
            if (Unit* caster = GetCaster())
            {
                target->AI()->SetData(TYPE_NEXT_IN_LINK, 1);
                target->AI()->SetGUID(caster->GetGUID());
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_CELESTIAL_STAR; });

        if (Creature* caster = GetCaster()->ToCreature())
            targets.remove_if(StarSequencePredicate(caster));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_celestial_link_star::HandleDummyHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_celestial_link_star::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Cancel Beams 138283
class spell_celestial_cancel_beams : public SpellScript
{
    PrepareSpellScript(spell_celestial_cancel_beams);

    void HandleDummyHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_CELESTIAL_DRAWING_WRONG);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_CELESTIAL_STAR; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_celestial_cancel_beams::HandleDummyHit, EFFECT_0, SPELL_EFFECT_REMOVE_AURA);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_celestial_cancel_beams::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Xuen Blessed Alacrity 138645
class spell_huen_blessed_alacrity : public SpellScript
{
    PrepareSpellScript(spell_huen_blessed_alacrity);

    std::list<WorldObject*> m_targets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && (target->ToPlayer() || target->ToUnit() && target->ToUnit()->IsPet()); });
        m_targets = targets;
    }

    void CopyTargets(std::list<WorldObject*>& targets)
    {
        targets = m_targets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_huen_blessed_alacrity::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_huen_blessed_alacrity::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_huen_blessed_alacrity::CopyTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_huen_blessed_alacrity::CopyTargets, EFFECT_3, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Fortitude of the Ox 138298
class spell_fortitude_of_the_ox : public SpellScript
{
    PrepareSpellScript(spell_fortitude_of_the_ox);

    void HandleDummyHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* target = GetHitUnit())
            {
                int32 bp = (int32)(target->GetMaxHealth() * 0.5f);
                caster->CastCustomSpell(target, SPELL_FORTITUDE_OF_THE_OX_EFF, &bp, 0, 0, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_fortitude_of_the_ox::HandleDummyHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Fortitude of the Ox Eff 138300
class spell_fortitude_of_the_ox_eff : public AuraScript
{
    PrepareAuraScript(spell_fortitude_of_the_ox_eff);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            owner->SetHealth(owner->GetMaxHealth()); // Temp hack:: SPELL_AURA_MOD_INCREASE_HEALTH not right calculated at remove
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_fortitude_of_the_ox_eff::OnRemove, EFFECT_0, SPELL_AURA_MOD_INCREASE_HEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

// Light of Day Second Eff 137432
class spell_light_of_day_second_eff : public SpellScript
{
    PrepareSpellScript(spell_light_of_day_second_eff);

    void HandleDummyHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->Kill(target);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_ICE_COMET; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_light_of_day_second_eff::HandleDummyHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_light_of_day_second_eff::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// 740. Summoned by 137416 - Flames of Passion
class sat_flames_of_passion : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        Player* itr = triggering->ToPlayer();
        return itr && itr->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->CastSpell(target, SPELL_FLAMES_OF_PASSION_AT_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_FLAMES_OF_PASSION_AT_EFF);
    }
};

// 742. Summoned by 137439 - Ice Shadows
class sat_icy_shadows : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Creature* suen = ObjectAccessor::GetCreature(*itr, itr->GetInstanceScript() ? itr->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                if (!itr->IsWithinLOS(suen->GetPositionX(), suen->GetPositionY(), suen->GetPositionZ()))
                    return true;

        return false;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, SPELL_ICY_SHADOWS_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            target->RemoveAurasDueToSpell(SPELL_ICY_SHADOWS_EFF);
    }
};

// 697. Summoned by 136721 - Moon Lotus
class sat_moon_lotus : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
            if (GetCaster() && !target->HasAura(SPELL_SLUMBER_SPORES))
                GetCaster()->CastSpell(target, SPELL_SLUMBER_SPORES, true);
    }
};

// 787, 788, 789, 790, 791, 792, 793. Summoned by x - Star
class sat_celestial_star : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && 
            (triggering->ToPlayer()->HasAura(SPELL_INVOKE_OX_SPIRIT) || triggering->ToPlayer()->HasAura(SPELL_INVOKE_CRANE_SPIRIT) || triggering->ToPlayer()->HasAura(SPELL_INVOKE_SERPENT_SPIRIT) || triggering->ToPlayer()->HasAura(SPELL_INVOKE_TIGER_SPIRIT));
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* target = triggering->ToPlayer())
        {
            if (Creature* caster = GetCaster()->ToCreature())
            {
                if (caster->HasAura(SPELL_STAR_STATE_ACTIVE))
                    return;

                if (!caster->AI()->GetData(TYPE_NEXT_IN_LINK) && caster->AI()->GetData(TYPE_STAR_IN_SEQ) != 7)
                {
                    caster->CastSpell(caster, SPELL_CANCEL_BEAMS, true);
                    return;
                }

                if (!caster->HasAura(SPELL_MOONBEAM_STATE_WORLD))
                {
                    caster->RemoveAurasDueToSpell(SPELL_STAR_STATE_INACTIVE);
                    caster->CastSpell(caster, SPELL_STAR_STATE_ACTIVE, true);
                }

                if (Creature* sulin = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript() ? caster->GetInstanceScript()->GetGuidData(DATA_TWIN_CONSORTS) : ObjectGuid::Empty))
                {
                    if (sulin->AI()->GetData(TYPE_SELECTED_CELESTIAL) != NPC_FADED_IMAGE_OF_NIUZAO && sulin->AI()->GetData(TYPE_SELECTED_CELESTIAL) != NPC_FADED_IMAGE_OF_YULON && caster->AI()->GetData(TYPE_STAR_IN_SEQ) == 7)
                        return;

                    sulin->AI()->SetData(TYPE_ACTIVE_STAR_COUNTER, sulin->AI()->GetData(TYPE_ACTIVE_STAR_COUNTER) + 1);

                    if (sulin->AI()->GetData(TYPE_ACTIVE_STAR_COUNTER) >= 5)
                    {
                        if (Creature* selectedCelestial = ObjectAccessor::GetCreature(*caster, caster->GetInstanceScript()->GetGuidData(sulin->AI()->GetData(TYPE_SELECTED_CELESTIAL))))
                            selectedCelestial->AI()->DoAction(ACTION_CELESTIAL_DRAWING_DONE);

                        sulin->AI()->DoAction(ACTION_CELESTIAL_DRAWING_DONE);

                        return;
                    }
                }

                caster->CastSpell(caster, invStarSequenceType.find(caster->AI()->GetData(TYPE_STAR_IN_SEQ))->second[1], true);

                if (Unit* prevCaster = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
                    prevCaster->CastSpell(caster, SPELL_STARDUST_BEAM, true);
            }
        }
    }
};

void AddSC_boss_twin_consorts()
{
    new boss_suen();
    new boss_lulin();
    new creature_script<npc_faded_image_of_celestial>("npc_faded_image_of_celestial");
    new creature_script<npc_celestial_star>("npc_celestial_star");
    new creature_script<npc_moon_lotus_wave>("npc_moon_lotus_wave");
    new creature_script<npc_crashing_star>("npc_crashing_star");
    new creature_script<npc_beast_of_nightmares>("npc_beast_of_nightmares");
    new creature_script<npc_ice_comet>("npc_ice_comet");
    new creature_script<npc_lurker_in_the_night>("npc_lurker_in_the_night");

    new spell_script<spell_tear_of_the_sun_selector>("spell_tear_of_the_sun_selector");
    new spell_script<spell_tear_of_the_sun_eff>("spell_tear_of_the_sun_eff");
    new aura_script<spell_tear_of_the_sun>("spell_tear_of_the_sun");
    new spell_script<spell_light_of_day_selector>("spell_light_of_day_selector");
    new aura_script<spell_blazing_radiance>("spell_blazing_radiance");
    new aura_script<spell_moon_lotus_bloom_aura>("spell_moon_lotus_bloom_aura");
    new spell_script<spell_moon_lotus_bloom_aura_eff>("spell_moon_lotus_bloom_aura_eff");
    new spell_script<spell_moon_lotus_bloom>("spell_moon_lotus_bloom");
    new spell_script<spell_crashing_star_jump>("spell_crashing_star_jump");
    new spell_script<spell_celestial_summon_star>("spell_celestial_summon_star");
    new aura_script<spell_twin_consort_dissipate>("spell_twin_consort_dissipate");
    new aura_script<spell_moon_lotus_periodic>("spell_moon_lotus_periodic");
    new spell_script<spell_moon_lotus_hide_selector>("spell_moon_lotus_hide_selector");
    new spell_script<spell_moon_lotus_reappear_selector>("spell_moon_lotus_reappear_selector");
    new aura_script<spell_cosmic_barrage>("spell_cosmic_barrage");
    new spell_script<spell_cosmic_barrage_summon>("spell_cosmic_barrage_summon");
    new aura_script<spell_launch_star>("spell_launch_star");
    new spell_script<spell_launch_star_eff>("spell_launch_star_eff");
    new spell_script<spell_flames_of_passion_charge_back>("spell_flames_of_passion_charge_back");
    new spell_script<spell_flames_of_passion_eff>("spell_flames_of_passion_eff");
    new spell_script<spell_flames_of_passion_charge>("spell_flames_of_passion_charge");
    new spell_script<spell_ice_comet_selector>("spell_ice_comet_selector");
    new spell_script<spell_despawn_ice_comet>("spell_despawn_ice_comet");
    new spell_script<spell_ice_comet_chilled>("spell_ice_comet_chilled");
    new spell_script<spell_tidal_force>("spell_tidal_force");
    new aura_script<spell_beast_of_nightmares>("spell_beast_of_nightmares");
    new spell_script<spell_expose_lurkers>("spell_expose_lurkers");
    new spell_script<spell_lurker_darkness>("spell_lurker_darkness");
    new aura_script<spell_nuclear_inferno>("spell_nuclear_inferno");
    new spell_script<spell_nuclear_inferno_eff>("spell_nuclear_inferno_eff");
    new aura_script<spell_lurker_invis>("spell_lurker_invis");
    new spell_script<spell_celestial_uninteractible>("spell_celestial_uninteractible");
    new aura_script<spell_celestial_uninteractible_aura>("spell_celestial_uninteractible_aura");
    new spell_script<spell_celestial_link_star>("spell_celestial_link_star");
    new spell_script<spell_celestial_cancel_beams>("spell_celestial_cancel_beams");
    new spell_script<spell_huen_blessed_alacrity>("spell_huen_blessed_alacrity");
    new spell_script<spell_fortitude_of_the_ox>("spell_fortitude_of_the_ox");
    new spell_script<spell_light_of_day_second_eff>("spell_light_of_day_second_eff");
    new aura_script<spell_fortitude_of_the_ox_eff>("spell_fortitude_of_the_ox_eff");
    new atrigger_script<sat_flames_of_passion>("sat_flames_of_passion");
    //new atrigger_script<sat_icy_shadows>("sat_icy_shadows");
    new atrigger_script<sat_moon_lotus>("sat_moon_lotus");
    new atrigger_script<sat_celestial_star>("sat_celestial_star");
}
