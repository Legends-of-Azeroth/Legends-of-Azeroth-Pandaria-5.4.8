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
#include "CreatureTextMgr.h"
#include "GameEventMgr.h"

enum eSpells
{
    /*Blackfuse*/
    SPELL_ELECTROSTATIC_CHARGE                 = 143385,
    SPELL_OVERCHARGE                           = 145774,
    SPELL_LAUNCH_SAWBLADE                      = 143265,
    SPELL_LOAD_SAWBLADE                        = 143264,
    SPELL_LAUNCH_SAWBLADE_AAT                  = 143329,
    SPELL_LAUNCH_SAWBLADE_JUMP                 = 143266,
    SPELL_HOLIDAY_SAWBLADE_VISUAL              = 149086,
    SPELL_SERRATED_SLASH_SHREDDERS             = 143328,
    SPELL_SERRATED_SLASH_PLAYERS               = 143327,
    SPELL_SERRATED_SLASH_AURA                  = 145381,
    SPELL_PROTECTIVE_FRENZY                    = 145365,
    SPELL_ENERGIZING_DEFENSIVE_MATRIX          = 148990,
    SPELL_AUTOMATIC_REPAIR_BEAM                = 144213,
    SPELL_AUTOMATIC_REPAIR_BEAM_AT             = 144212,
    SPELL_AUTOMATIC_REPAIR_BEAM_VISUAL         = 145701,
    SPELL_LAUNCH_SAWBLADE_AURA                 = 143266,
    SPELL_BERSERK                              = 64238,
    SPELL_HOLIDAY_DECORATION                   = 149077,
                                               
    /*Shredder*/                               
    SPELL_OVERCHARGE_S                         = 145444,
    SPELL_DEATH_FROM_ABOVE                     = 144210,
    SPELL_DEATH_FROM_ABOVE_MISSLE              = 147010,
    SPELL_DEATH_FROM_ABOVE_INIT                = 144208,
    SPELL_DEATH_FROM_ABOVE_JUMP                = 142264,
                                               
    /*Shockwave Missle Turret*/                
    SPELL_SHOCKWAVE_MISSLE                     = 143640,
    SPELL_SHOCKWAVE_VISUAL                     = 144647,
    SPELL_SHOCKWAVE_INNER_CIRCLE               = 144658,
    SPELL_SHOCKWAVE_CIRCLE_1                   = 144660,
    SPELL_SHOCKWAVE_CIRCLE_2                   = 144661,
    SPELL_SHOCKWAVE_CIRCLE_3                   = 144662,
    SPELL_SHOCKWAVE_CIRCLE_4                   = 144663,
    SPELL_SHOCKWAVE_CIRCLE_5                   = 144664,
    SPELL_DEACTIVATED_ROCKET_TURRET            = 143526,
    SPELL_ACTIVATED_ROCKET_TURRET              = 146380, // base vehicle
                                               
    /*Laser Turret*/                           
    SPELL_SUPERHEATED                          = 144040, // apply aura
    SPELL_SUPERHEATED_EFF                      = 143856,
    SPELL_LASER_TURRET_TARGETING               = 145906,
    SPELL_LASER_TURRET_SELECTOR                = 143833,
    SPELL_LASER_TURRET_TARGETING_BUNNY         = 143838, // summon 71740
    SPELL_LOCKED_ON                            = 143828, // player label
    SPELL_DISINTEGRATION_LASER                 = 143867, // cast to 71740
    SPELL_LASER_GROUND_EFFECT_PERIODIC         = 143829, // create areatrigger each 750 ms
    SPELL_ACTIVATED_LASER_TURRET_BASE          = 143711, // base vehicle
    SPELL_DEACTIVATED_LASER_TURRET             = 143263,
    SPELL_DISINTEGRATION_LASER_VISUAL_HC       = 146680,
                                               
    /*Electromagnet*/                          
    SPELL_ACTIVATED_MAGNET_TURRET              = 143938,
    SPELL_DEACTIVATED_MAGNET_TURRET            = 143936,
    SPELL_MAGNETIC_CRUSH                       = 143487,
    SPELL_MAGNETIC_CRUSH_PERIODIC_DAMAGE       = 144466,
    SPELL_MAGNETIC_PULSE                       = 143945,
    SPELL_MAGNETIC_PULSE_REMOVE_AURA           = 143359,
    SPELL_MAGNETIC_CRUSH_PUSH                  = 147370, // Heroic
    SPELL_MAGNETIC_CRUSH_PULL                  = 147369, // Heroic
    SPELL_MAGNETIC_PULSE_SAW_SELECTOR          = 143357,
                                               
    /*Crawler Mine*/                           
    SPELL_SUPERHEATED_CRAWLER_MINE             = 146342,
    SPELL_DISASSEMBLED_CRAWLER_MINES           = 144012,
    SPELL_ACTIVATED_CRAWLER_MINE_CONTROLER     = 144011, // vehicle base, mine shouldn`t have a movement anim till not at ground
    SPELL_CRAWLER_MINE_IMMUNITY                = 149836, // only effect
    SPELL_CRAWLER_MINE_FIXATE_AURA             = 149147,
    SPELL_CRAWLER_MINE_BREAK_IN_PERIOD         = 145269, // vulnerable to control effects
    SPELL_CRAWLER_MINE_DETONATE                = 143002,
    SPELL_CRAWLER_MINE_READY_TO_GO             = 145580, // inc speed run and size
    SPELL_CRAWLER_MINE_CONTROLLER_PERIODIC     = 143995,
    SPELL_CRAWLER_MINE_SELECTOR                = 149148,
    SPELL_CRAWLER_MINE_HOLIDAY_VISUAL          = 149091,

    /*Deathdealer Laser Turret (Berserker)*/
    SPELL_DEATHDEALER_TURRET_CONTROL_PERIODIC  = 146378,
    SPELL_DEATHDEALER_LASER_TARGETING          = 146372,
    SPELL_DEATHDEALER_TURRET_VISUAL            = 146371,
    SPELL_SUMMON_DEACTIVATED_DEATHDEALER_TURR  = 146381,
    SPELL_SUMMON_ACTIVATED_DEATHDEALER_TURRET  = 146418,
    SPELL_DEATHDEALER                          = 146373,
                                               
    /*Transporter*/                            
    SPELL_CONVEYOR_DEATH_BEAM_AURA             = 144284,
    SPELL_CONVEYOR_DEATH_DEAM                  = 144282, // Areatrigger
    SPELL_CONVEYOR_DEATH_BEAM_HC               = 149016,
    SPELL_ELECTROMAGNETIC_BARRIER_AURA         = 146663,
    SPELL_ELECTROMAGNETIC_BARRIER              = 145154,
    SPELL_CREATE_CONVEYOR_TRIGGER              = 145272, // make areatrigger on conveyor
    SPELL_ON_CONVEYOR                          = 144287,
    SPELL_MATTER_PURIFICATION_BEAM_EFF         = 144335,
    SPELL_PATTERN_RECOGNITION                  = 144236, // Apply only if player used pipe
    SPELL_PIPE_TRANSFER_CONVEYOR_TELEPORT      = 143758, // immune falling damage and trigger pattern
    SPELL_PIPE_TRANSFERT_PLATFORM_TO_UNDERHOLD = 145238,
    SPELL_PIPE_TRANSFERT_UNDERHOLD_TO_PLATFORM = 145224,
    SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM  = 143799,
    SPELL_PIPE_TRANSFERT_PLATFORM_TO_CONVEYOR  = 143738,
    SPELL_PIPE_TRANSFERT_MAIN_TELEPORT         = 145223, // platform - underhold and back
    SPELL_PIPE_TRANSFERT_JUMP                  = 143798,
    SPELL_PIPE_REJECTION_JUMP                  = 148920, // pull back if we have pattern
    SPELL_ON_PLATFORM                          = 145480, // not sure why we need this spell. on conveyor more then enought
    SPELL_LEAP                                 = 146780,

    // Heroic
    SPELL_OVERCHARGE_WEAPON                    = 145774,
    SPELL_OVERCHARGED_WEAPON_VISUAL            = 146092,
    SPELL_OVERCHARGED_LASER_AT                 = 144228,
    SPELL_SHOCKWAVE_MISSLE_LAUNCH_HC           = 146153,
    SPELL_SHOCKWAVE_MISSLE_EFF_HC              = 146152,
    SPELL_SHOCKWAVE_MISSLE_VISUAL_HC           = 146155,
    SPELL_SHOCKWAVE_DESTRUCTION                = 149069,
    SPELL_SHOCKWAVE_MISSLE_HC_SELECTOR         = 146154, // doesn`t know who`s should be selected (no sniff data)
    SPELL_MAGNETIC_CRUSH_PUSH_SAW              = 147458,
    SPELL_MAGNETIC_CRUSH_PULL_SAW              = 147380,
    SPELL_MAGNETIC_PULL_JUMP                   = 147459,

    // Misc
    SPELL_DEMONIC_GATEWAY_TRANSITION           = 120729,
};

enum eEvents
{
    EVENT_SAWBLADE             = 1,
    EVENT_SHREDDER,
    EVENT_ELECTROSTATIC_CHARGE,
    EVENT_INIT_SAWBLADE,
    EVENT_OVERCHARGE,
    EVENT_DEATH_FROM_ABOVE,
    EVENT_BERSERK,
    EVENT_PREPARE_DEACTIVATED_WEAPONS,
    EVENT_LAUNCH_DEACTIVATED_WEAPONS,
    EVENT_LAUNCH_ACTIVATED_WEAPONS,
    EVENT_SW_SHREDDERS,
    EVENT_SW_PLAYERS,
    EVENT_REACH_CONVEYOR,
    EVENT_REMOVE_CONVEYOR,
    EVENT_REMOVE_CONVEYOR_DELAY,
    EVENT_REMOVE,
    EVENT_SPECIAL_LAUNCH,
    EVENT_FIXATE, // only for crawler mine
    EVENT_READY,
    EVENT_MOVE,
    EVENT_FOLLOW,
    EVENT_OVERCHARGE_WEAPON,
    EVENT_LAUNCH_ELECTROMAGNET,
};

enum Says
{
    TALK_AGGRO,
    TALK_CONVEYOR_ANN,
    TALK_WEAPON_DEATH,
    TALK_SHREDDER,
    TALK_SHREDDER_ANN,
    TALK_DEATH,
    TALK_SLAY = 9,
    TALK_OVERLOAD_SHOCKWAVE_MISSLE_TURRET,
    TALK_OVERLOAD_CRAWLER_MINES,
    TALK_OVERLOAD_LASER,
    TALK_OVERLOAD_ELECTROMAGNET,

    TALK_LOCKED_ON_ANN = 0,
    TALK_SAWBLADE_ANN,

    TALK_CRAWLER_MINE_DETONATE = 1,

};

const std::map<uint32, std::array<uint32, 3>> invConveyorType =
{
    { 1,  { SPELL_DEACTIVATED_ROCKET_TURRET,  SPELL_DISASSEMBLED_CRAWLER_MINES, SPELL_DEACTIVATED_LASER_TURRET    } },
    { 2,  { SPELL_DISASSEMBLED_CRAWLER_MINES, SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DEACTIVATED_ROCKET_TURRET   } },
    { 3,  { SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DEACTIVATED_MAGNET_TURRET,  SPELL_DEACTIVATED_ROCKET_TURRET   } },
    { 4,  { SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DEACTIVATED_ROCKET_TURRET,  SPELL_DISASSEMBLED_CRAWLER_MINES  } },
    { 5,  { SPELL_DEACTIVATED_ROCKET_TURRET,  SPELL_DEACTIVATED_MAGNET_TURRET,  SPELL_DISASSEMBLED_CRAWLER_MINES  } },
    { 6,  { SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DISASSEMBLED_CRAWLER_MINES, SPELL_DISASSEMBLED_CRAWLER_MINES  } },
    { 7,  { SPELL_DISASSEMBLED_CRAWLER_MINES, SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DEACTIVATED_ROCKET_TURRET   } },
    { 8,  { SPELL_DEACTIVATED_ROCKET_TURRET,  SPELL_DEACTIVATED_MAGNET_TURRET,  SPELL_DISASSEMBLED_CRAWLER_MINES  } },
    { 9,  { SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DISASSEMBLED_CRAWLER_MINES, SPELL_DEACTIVATED_LASER_TURRET    } },
    { 10, { SPELL_DEACTIVATED_ROCKET_TURRET,  SPELL_DEACTIVATED_LASER_TURRET,   SPELL_DISASSEMBLED_CRAWLER_MINES  } },
    { 11, { SPELL_DEACTIVATED_ROCKET_TURRET,  SPELL_DEACTIVATED_MAGNET_TURRET,  SPELL_DEACTIVATED_ROCKET_TURRET   } },
    { 12, { SPELL_DISASSEMBLED_CRAWLER_MINES, SPELL_DEACTIVATED_MAGNET_TURRET,  SPELL_DEACTIVATED_LASER_TURRET    } }, // after this wave boss will summon 3 mega-turret that`ll wipe raid
    { 13, { SPELL_SUMMON_DEACTIVATED_DEATHDEALER_TURR, SPELL_SUMMON_DEACTIVATED_DEATHDEALER_TURR, SPELL_SUMMON_DEACTIVATED_DEATHDEALER_TURR } },
};

const std::map<uint32, uint32> invOverchargedWeaponType =
{
    { NPC_ACTIVATED_LASER_TURRET,         NPC_OVERCHARGED_LASER_TURRET  },
    { NPC_ACTIVATED_ELECTROMAGNET,        NPC_OVERCHARGED_ELECTROMAGNET },
    { NPC_ACTIVATED_MISSLE_TURRET,        NPC_OVERCHARGED_MISSLE_TURRET },
    { NPC_ACTIVATED_CRAWLER_MINE_VEHICLE, NPC_OVERCHARGED_CRAWLER_MINE  },
};

const std::map<uint32, uint32> invSelectWeaponType =
{
    { NPC_DIACTIVATED_LASER_TURRET,       NPC_ACTIVATED_LASER_TURRET         },
    { NPC_DIACTIVATED_ELECTROMAGNET,      NPC_ACTIVATED_ELECTROMAGNET        },
    { NPC_DIACTIVATED_MISSLE_TURRET,      NPC_ACTIVATED_MISSLE_TURRET        },
    { NPC_DISASSEMBLED_CRAWLER_MINES,     NPC_ACTIVATED_CRAWLER_MINE_VEHICLE },
    { NPC_DEACTIVATED_DEATHDEALER_TURRET, NPC_ACTIVATED_DEATHDEALER_TURRET   },
};

// Missle should hit only on edge of platform. Nearest by Encounter.
// Main idea check angle our encounter to helper and invert him (helper has middle of platform pos).
Position CalculateMissleThrowPosition(Unit* me, ObjectGuid m_ownerGUID, uint32 count = 0)
{
    Unit* m_owner = ObjectAccessor::GetUnit(*me, m_ownerGUID);
    float x, y;

    if (!m_owner)
        return { 0.0f, 0.0f, 0.0f, 0.0f };

    if (InstanceScript* instance = m_owner->GetInstanceScript())
    {
        if (Creature* Siegecrafter = ObjectAccessor::GetCreature(*m_owner, instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE)))
        {
            if (Creature* Helper = ObjectAccessor::GetCreature(*m_owner, instance->GetGuidData(NPC_SIEGECRAFTER_HELPER)))
            {
                if (float angle = Siegecrafter->GetAngle(Helper))
                {
                    // Invert our ori
                    if (float FixedAngle = Position::NormalizeOrientation(angle + M_PI))
                    {
                        if (count)
                            FixedAngle = Position::NormalizeOrientation(FixedAngle - (count * M_PI) / 4); // for heroic

                        GetPositionWithDistInOrientation(Helper, 55.0f, FixedAngle, x, y);
                        return { x, y, -309.326f, 0.0f };
                    }
                }
            }
        }
    }

    return { 0.0f, 0.0f, 0.0f, 0.0f };
}

// Predicate on select player that not has a tank spec and actually not on conveyor
struct NonTankSpecConveyorPredicate 
{
    public:
        NonTankSpecConveyorPredicate(Creature const* me) : _me(me) { }

        bool operator() (Unit* const& object) const
        {
            return object && object->ToPlayer() && object->ToPlayer()->GetRoleForGroup(object->ToPlayer()->GetTalentSpecialization()) != ROLES_TANK && !object->ToPlayer()->HasAura(SPELL_ON_CONVEYOR);
        }

    private:
        Creature const* _me;
};

class boss_siegecrafter_blackfuse : public CreatureScript
{
    public:
        boss_siegecrafter_blackfuse() : CreatureScript("boss_siegecrafter_blackfuse") { }

        struct boss_siegecrafter_blackfuseAI : public BossAI
        {
            boss_siegecrafter_blackfuseAI(Creature* creature) : BossAI(creature, DATA_SIEGECRAFTER_BLACKFUSE) 
            { 
                me->setActive(true);
            }

            ObjectGuid targetGUID;
            uint32 prevOverchargedValue;
            bool hasEvade;

            void Reset() override
            {
                _Reset();
                events.Reset();
                summons.DespawnAll();
                targetGUID = ObjectGuid::Empty;
                prevOverchargedValue = 0;
                hasEvade = false;
                me->SetCorpseDelay(30 * MINUTE);
                me->SetReactState(REACT_DEFENSIVE);

                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_LASER, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_SHOCKWAVE, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_MAGNET, 1);
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_MINES, 1);

                if (sGameEventMgr->IsActiveEvent(2))
                    DoCast(me, SPELL_HOLIDAY_DECORATION, true);

                // Init delay
                scheduler
                    .Schedule(Milliseconds(1500), [this](TaskContext context)
                {
                    if (Creature* siegecrafterVehicle = me->SummonCreature(NPC_SIEGECRAFTER_BLACKFUSE_1, *me, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        int32 bp = 2;
                        siegecrafterVehicle->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &bp, 0, 0, true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                    }
                });
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_SIEGECRAFTER_BLACKFUSE, FAIL);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim && victim->GetTypeId() == TYPEID_PLAYER)
                    if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                        mySiegecrafter->AI()->Talk(TALK_SLAY);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                me->SetReactState(REACT_AGGRESSIVE);
                DoCast(me, SPELL_ENERGIZING_DEFENSIVE_MATRIX); // remove players from conveyor on encounter combat

                if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                    mySiegecrafter->AI()->Talk(TALK_AGGRO);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->DoAddAuraOnPlayers(SPELL_ON_PLATFORM);

                    if (Creature* helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                        helper->AI()->DoAction(ACTION_CONVEYOR_RUN);
                }

                if (instance && !instance->GetData(DATA_LFR))
                    DoCast(me, SPELL_AUTOMATIC_REPAIR_BEAM_AT);
                else if (instance && instance->GetData(DATA_FLEX)) // in flex too
                    DoCast(me, SPELL_AUTOMATIC_REPAIR_BEAM_AT);

                events.ScheduleEvent(EVENT_INIT_SAWBLADE, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SAWBLADE, 7 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SHREDDER, 35 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ELECTROSTATIC_CHARGE, 0.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() != NPC_SIEGECRAFTER_BLACKFUSE_1)
                    summon->SetInCombatWithZone();

                if (summon->GetEntry() == NPC_SAWBLADE)
                {
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                    summon->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, false);
                }
            }

            void EnterEvadeMode() override
            {
                if (hasEvade)
                    return;

                hasEvade = true;

                if (Creature* helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                    helper->AI()->EnterEvadeMode();

                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_SIEGECRAFTER_BLACKFUSE, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELECTROSTATIC_CHARGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCHARGE_S);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ON_PLATFORM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_CRUSH_PERIODIC_DAMAGE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                HandleResetConveyorBeams();
                summons.DespawnAll();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_OVERCHARGE_WEAPON)
                {
                    scheduler
                        .Schedule(Milliseconds(500), [this](TaskContext context)
                    {
                        if (Creature* target = ObjectAccessor::GetCreature(*me, getOverchargeWeaponGUID()))
                            DoCast(target, SPELL_OVERCHARGE_WEAPON);
                    });
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                    mySiegecrafter->AI()->Talk(TALK_DEATH);

                Talk(TALK_AGGRO); // announce: crossfire door are open

                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_SIEGECRAFTER_BLACKFUSE, DONE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SUPERHEATED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ELECTROSTATIC_CHARGE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OVERCHARGE_S);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ON_PLATFORM);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MAGNETIC_CRUSH_PERIODIC_DAMAGE);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* helper = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_SIEGECRAFTER_HELPER)))
                        helper->AI()->DoAction(ACTION_CONVEYOR_RESET);
                }

                HandleResetConveyorBeams();
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
                        case EVENT_SAWBLADE:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, getSawbladeTargetGUID()))
                            {
                                Talk(TALK_SAWBLADE_ANN, target);
                                DoCast(target, SPELL_LAUNCH_SAWBLADE);
                            }
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                            {
                                Talk(TALK_SAWBLADE_ANN, target);
                                DoCast(target, SPELL_LAUNCH_SAWBLADE);
                            }

                            events.ScheduleEvent(EVENT_SAWBLADE, 15 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_INIT_SAWBLADE, 3.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_ELECTROSTATIC_CHARGE:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_ELECTROSTATIC_CHARGE, false);

                            events.ScheduleEvent(EVENT_ELECTROSTATIC_CHARGE, 16 * IN_MILLISECONDS);
                            break;
                        case EVENT_SHREDDER:
                            if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                            {
                                mySiegecrafter->AI()->Talk(TALK_SHREDDER_ANN);
                                mySiegecrafter->AI()->Talk(TALK_SHREDDER);
                            }

                            me->SummonCreature(NPC_AUTOMATED_SHREDDER, ShredderSpawnPos, TEMPSUMMON_MANUAL_DESPAWN);
                            events.ScheduleEvent(EVENT_SHREDDER, 60 * IN_MILLISECONDS);
                            break;
                        case EVENT_INIT_SAWBLADE:
                            DoCast(me, SPELL_LOAD_SAWBLADE);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                void HandleResetConveyorBeams()
                {
                    std::list<Creature*> conveyorBeams;
                    GetCreatureListWithEntryInGrid(conveyorBeams, me, NPC_LASSER_ARRAY, 300.0f);

                    for (auto&& itr : conveyorBeams)
                        itr->AI()->DoAction(ACTION_CONVEYOR_RESET);
                }

                ObjectGuid getOverchargeWeaponGUID()
                {
                    if (Creature* helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                    {
                        uint32 weaponEntry = helper->AI()->GetData(TYPE_OVERCHARGE_WEAPON);

                        std::list<Creature*> weaponList;
                        GetCreatureListWithEntryInGrid(weaponList, me, weaponEntry, 400.0f);

                        // Remove weapons that already leave from conveyor line
                        weaponList.remove_if([=](Creature* target) { return target && target->AI()->GetData(TYPE_NOT_IN_CONVEYOR_LINE); });

                        if (weaponList.empty())
                            return ObjectGuid::Empty;

                        return Trinity::Containers::SelectRandomContainerElement(weaponList)->GetGUID();
                    }

                    return ObjectGuid::Empty;
                }

                ObjectGuid getSawbladeTargetGUID()
                {
                    ThreatContainer::StorageType const& threatlist = me->GetThreatManager().getThreatList();
                    std::list<Unit*> targetList, tempTargetList;

                    for (ThreatContainer::StorageType::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                        if ((*itr)->getTarget())
                            targetList.push_back((*itr)->getTarget());

                    tempTargetList = targetList;

                    targetList.remove_if(TankSpecTargetSelector()); // tanks not in priority
                    targetList.remove_if([=](Unit* target) { return target && target->ToPlayer() && (target->ToPlayer()->GetExactDist2d(me) < 13.5f || target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR)); });

                    // Exclude pets
                    targetList.remove_if([=](Unit* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });

                    uint32 requredInRangeCount = Is25ManRaid() ? 7 : 3;

                    if (targetList.size() >= requredInRangeCount)
                        return Trinity::Containers::SelectRandomContainerElement(targetList)->GetGUID();

                    targetList.clear();

                    for (auto&& itr : tempTargetList)
                        targetList.push_back(itr);

                    targetList.remove_if([=](Unit* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR); });

                    // Exclude pets
                    targetList.remove_if([=](Unit* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });

                    if (targetList.size() > 1)
                        return Trinity::Containers::SelectRandomContainerElement(targetList)->GetGUID();

                    return ObjectGuid::Empty;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_siegecrafter_blackfuseAI(creature);
        }
};

// Automated Shreder 71591
struct npc_automated_shredder : public ScriptedAI
{
    npc_automated_shredder(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;
    EventMap events;
    ObjectGuid targetGUID;
    InstanceScript* instance;
    bool inAir;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        x = 0.0f;
        y = 0.0f;
        targetGUID = ObjectGuid::Empty;
        inAir = false;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
        me->OverrideInhabitType(INHABIT_GROUND);
        me->UpdateMovementFlags();
        GetPositionWithDistInOrientation(me, 40.0f, me->GetOrientation(), x, y);
        me->GetMotionMaster()->MoveJump(x, y, -309.327f, 25.0f, 15.0f, EVENT_JUMP + 1);
        instance = me->GetInstanceScript();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DEATH_FROM_ABOVE, 19 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_OVERCHARGE, 6 * IN_MILLISECONDS);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
            siegeCrafter->CastSpell(siegeCrafter, SPELL_PROTECTIVE_FRENZY, false);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP + 1)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
            me->SetInCombatWithZone();
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
                case EVENT_OVERCHARGE:
                    if (inAir)
                    {
                        events.RescheduleEvent(EVENT_OVERCHARGE, 1 * IN_MILLISECONDS);
                        break;
                    }
                    DoCast(me, SPELL_OVERCHARGE_S);
                    events.ScheduleEvent(EVENT_OVERCHARGE, 11 * IN_MILLISECONDS);
                    break;
                case EVENT_DEATH_FROM_ABOVE:
                    DoCast(me, SPELL_DEATH_FROM_ABOVE_INIT);
                    me->ClearUnitState(UNIT_STATE_CASTING);

                    scheduler
                        .Schedule(Milliseconds(5200), [this](TaskContext context)
                    {
                        inAir = true;
                        // Movement Type
                        me->OverrideInhabitType(INHABIT_AIR);
                        me->UpdateMovementFlags();

                        if (Unit* vict = me->GetVictim())
                            targetGUID = vict->GetGUID();

                        me->PrepareChanneledCast(me->GetOrientation());
                        me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 20.0f);

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                        {
                            me->CastSpell(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 20.0f, SPELL_DEATH_FROM_ABOVE_MISSLE, false);

                            scheduler
                                .Schedule(Milliseconds(1300), [this](TaskContext context)
                            {
                                me->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - 20.0f, 15.0f, 15.0f);

                                scheduler
                                    .Schedule(Milliseconds(me->GetSplineDuration() + 650), [this](TaskContext context)
                                {
                                    me->OverrideInhabitType(INHABIT_GROUND);
                                    me->UpdateMovementFlags();
                                    me->RemoveChanneledCast(targetGUID);
                                    inAir = false;
                                });
                            });
                        });
                    });

                    events.ScheduleEvent(EVENT_DEATH_FROM_ABOVE, 40 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Sawblade 71532
struct npc_siegecrafter_sawblade : public ScriptedAI
{
    npc_siegecrafter_sawblade(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    uint32 hasLaunched;
    uint32 trajPos;
    ObjectGuid magnetGUID;

    void Reset() override 
    {
        hasLaunched = 0;
        magnetGUID = ObjectGuid::Empty;
        trajPos     = 100; // can`t set 0 cuz it using

        if (sGameEventMgr->IsActiveEvent(2))
            DoCast(me, SPELL_HOLIDAY_SAWBLADE_VISUAL, true);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_LAUNCH_SAWBLADE)
            return hasLaunched;

        switch (type)
        {
            case TYPE_LAUNCH_SAWBLADE:
                return hasLaunched;
            case TYPE_SAW_TRAJ:
                return trajPos;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_LAUNCH_SAWBLADE:
                // Sawblade Shouldn`t pull to magnet in time when it was launch. should have a delay
                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    hasLaunched = 1;
                });
                break;
            case TYPE_SAW_TRAJ:
                trajPos = data;
                break;
        }
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        magnetGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return magnetGUID;
    }

    void EnterEvadeMode() override { }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Siegecrafter Helper Bunny 71520
struct npc_soo_siegecrafter_helper : public ScriptedAI
{
    npc_soo_siegecrafter_helper(Creature* creature) : ScriptedAI(creature), summons(me) { }

    TaskScheduler scheduler;
    EventMap events;
    SummonList summons;
    InstanceScript* instance;
    uint32 prevWeaponType, length_counter;
    uint32 conveyorSeq;
    uint32 hasWeaponDestroyed;
    uint32 prevOvercharged;
    std::vector<uint32> m_deactivatedCombo, m_activatedCombo;
    std::vector<ObjectGuid> m_dGUID;
    std::vector<uint32> tempOvercharged;
    std::map <uint32, uint32> ActivatedConveyorDict;
    bool hasEvade;
    bool hasLaunch; // for HC

    void Reset() override
    {
        instance = me->GetInstanceScript();
        prevWeaponType = 0;
        length_counter = 0;
        conveyorSeq    = 1;
        prevOvercharged = 0;
        hasWeaponDestroyed = 1; // we using matrix on pull, so need this value
        summons.DespawnAll();
        m_deactivatedCombo.clear();
        m_activatedCombo.clear();
        ActivatedConveyorDict.clear();
        m_dGUID.clear();
        tempOvercharged.clear(); // priority - low use > deathdealer > magnet > mine > shockwave > laser
        me->RemoveAllAreasTrigger();
        scheduler.CancelAll();
        events.Reset();
        hasEvade = false;
        hasLaunch = false;

        // Conveyor Dictionary
        ActivatedConveyorDict.insert(std::pair<uint32, uint32>(NPC_DIACTIVATED_ELECTROMAGNET, SPELL_ACTIVATED_MAGNET_TURRET));
        ActivatedConveyorDict.insert(std::pair<uint32, uint32>(NPC_DIACTIVATED_LASER_TURRET, NPC_ACTIVATED_LASER_TURRET));
        ActivatedConveyorDict.insert(std::pair<uint32, uint32>(NPC_DIACTIVATED_MISSLE_TURRET, NPC_ACTIVATED_MISSLE_TURRET));
        ActivatedConveyorDict.insert(std::pair<uint32, uint32>(NPC_DISASSEMBLED_CRAWLER_MINES, NPC_SIEGECRAFTER_CRAWLER_MINE));
        ActivatedConveyorDict.insert(std::pair<uint32, uint32>(NPC_DEACTIVATED_DEATHDEALER_TURRET, SPELL_SUMMON_ACTIVATED_DEATHDEALER_TURRET));
    }

    void EnterEvadeMode() override 
    { 
        if (hasEvade)
            return;

        hasEvade = true;
        scheduler.CancelAll();
        me->RemoveAllAreasTrigger();
        summons.DespawnAll();

        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
            siegeCrafter->AI()->EnterEvadeMode();

        ScriptedAI::EnterEvadeMode();

        uint32 corpseDelay = me->GetCorpseDelay();
        uint32 respawnDelay = me->GetRespawnDelay();

        me->SetCorpseDelay(1);
        me->SetRespawnDelay(29);

        me->DespawnOrUnsummon();

        me->SetCorpseDelay(corpseDelay);
        me->SetRespawnDelay(respawnDelay);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_CONVEYOR_RUN:
                me->SetInCombatWithZone();

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    if (instance && instance->IsWipe(100.0f, me))
                    {
                        EnterEvadeMode();
                        return;
                    }

                    context.Repeat(Seconds(1));
                });

                events.ScheduleEvent(EVENT_PREPARE_DEACTIVATED_WEAPONS, 1 * IN_MILLISECONDS);
                break;
            case ACTION_CONVEYOR_RESET:
                Reset();
                me->Kill(me);
                break;
            case ACTION_ANY_DEACTIVATED_HAS_DISSAPEAR:
                // Players can kill only one of three weapons
                for (auto&& itr : m_dGUID)
                    if (Creature* m_dWeapon = ObjectAccessor::GetCreature(*me, itr))
                        if (m_dWeapon->IsAlive())
                            m_dWeapon->CastSpell(m_dWeapon, SPELL_ELECTROMAGNETIC_BARRIER_AURA, false);
            
                // When one of three weapon was dissapeared, encounter cast frenzy in 10s
                if (Creature* Siegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
                    Siegecrafter->CastSpell(Siegecrafter, SPELL_PROTECTIVE_FRENZY, false);
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        m_dGUID.push_back(summon->GetGUID());
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_CONVEYOR_LENGTH:
                length_counter++;
                break;
            case TYPE_CONVEYOR_WEAPON_DESTROYED:
                hasWeaponDestroyed = data;

                // in case when was killed last weapon in distance
                if (m_activatedCombo.size() > 1)
                {
                    // if whole of three weapons reach conveyor, then encounter immune to damage in 20s
                    if (Unit* m_Siegecrafter = ObjectAccessor::GetUnit(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
                        m_Siegecrafter->CastSpell(m_Siegecrafter, SPELL_ENERGIZING_DEFENSIVE_MATRIX, false);

                    events.ScheduleEvent(EVENT_LAUNCH_ACTIVATED_WEAPONS, 7 * IN_MILLISECONDS);
                }
                break;
            default:
                // Init combo of weapons, that react to conveyor back
                m_activatedCombo.push_back(type);

                uint32 requredCount = GetData(TYPE_CONVEYOR_WEAPON_DESTROYED) ? 2 : 3;

                if (m_activatedCombo.size() < requredCount)
                    break;

                // if whole of three weapons reach conveyor, then encounter immune to damage in 20s
                if (Unit* m_Siegecrafter = ObjectAccessor::GetUnit(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
                    m_Siegecrafter->CastSpell(m_Siegecrafter, SPELL_ENERGIZING_DEFENSIVE_MATRIX, false);

                events.ScheduleEvent(EVENT_LAUNCH_ACTIVATED_WEAPONS, 7 * IN_MILLISECONDS);
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_CONVEYOR_WEAPON_DESTROYED:
                return hasWeaponDestroyed;
            case TYPE_CONVEYOR_LENGTH:
                return length_counter;
            case TYPE_OVERCHARGE_WEAPON:
                return prevOvercharged;
        }

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PREPARE_DEACTIVATED_WEAPONS:
                {
                    hasWeaponDestroyed = 0;
                    auto key = invConveyorType.find(conveyorSeq);

                    for (uint8 i = 0; i < 3; i++)
                        m_deactivatedCombo.push_back(key->second[i]);

                    if (++conveyorSeq > 12)
                        conveyorSeq = 13; // Mega Turret!

                    if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                        mySiegecrafter->AI()->Talk(TALK_CONVEYOR_ANN);

                    // Remix Beams (always active 3 in normal, in heroic another algorithm)
                    if (!IsHeroic())
                    {
                        std::list<Creature*> waveBeams;
                        GetCreatureListWithEntryInGrid(waveBeams, me, NPC_LASSER_ARRAY, 300.0f);

                        waveBeams.remove_if([=](Creature* target) { return target && target->ToCreature() && (target->ToCreature()->GetDBTableGUIDLow() < 561190 || target->ToCreature()->GetDBTableGUIDLow() > 561194); });

                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RESET); // disable all before run

                        if (waveBeams.size() > 3)
                            Trinity::Containers::RandomResizeList(waveBeams, 3);

                        // First Wave
                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RUN);

                        waveBeams.clear();
                        GetCreatureListWithEntryInGrid(waveBeams, me, NPC_LASSER_ARRAY, 300.0f);

                        waveBeams.remove_if([=](Creature* target) { return target && target->ToCreature() && (target->ToCreature()->GetDBTableGUIDLow() < 561195 || target->ToCreature()->GetDBTableGUIDLow() > 561199); });

                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RESET); // disable all before run

                        if (waveBeams.size() > 3)
                            Trinity::Containers::RandomResizeList(waveBeams, 3);

                        // Second Wave
                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RUN);

                        waveBeams.clear();
                        GetCreatureListWithEntryInGrid(waveBeams, me, NPC_LASSER_ARRAY, 300.0f);

                        waveBeams.remove_if([=](Creature* target) { return target && target->ToCreature() && (target->ToCreature()->GetDBTableGUIDLow() < 561200 || target->ToCreature()->GetDBTableGUIDLow() > 561204); });

                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RESET); // disable all before run

                        if (waveBeams.size() > 3)
                            Trinity::Containers::RandomResizeList(waveBeams, 3);

                        // Last Wave
                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RUN);
                    }
                    else if (!hasLaunch)
                    {
                        // There we should activate 4 line on each matrix.
                        // Also we should select any 1/5 that will move by left side
                        hasLaunch = true;

                        std::list<Creature*> waveBeams;
                        GetCreatureListWithEntryInGrid(waveBeams, me, NPC_LASSER_ARRAY, 300.0f);

                        waveBeams.remove_if([=](Creature* target) { return target && target->ToCreature() && (target->ToCreature()->GetDBTableGUIDLow() < 561190 || target->ToCreature()->GetDBTableGUIDLow() > 561194); });

                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RESET); // disable all before run

                        ObjectGuid launcherBunnyGUID = ObjectGuid::Empty;

                        if (!waveBeams.empty())
                            launcherBunnyGUID = Trinity::Containers::SelectRandomContainerElement(waveBeams)->GetGUID();

                        waveBeams.remove_if([=](Creature* target) { return target->GetGUID() == launcherBunnyGUID; });

                        // First Wave
                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RUN);

                        if (Creature* selectedLauncher = ObjectAccessor::GetCreature(*me, launcherBunnyGUID))
                            selectedLauncher->CastSpell(selectedLauncher, SPELL_CONVEYOR_DEATH_BEAM_HC, true);

                        waveBeams.clear();
                        GetCreatureListWithEntryInGrid(waveBeams, me, NPC_LASSER_ARRAY, 300.0f);

                        waveBeams.remove_if([=](Creature* target) { return target && target->ToCreature() && (target->ToCreature()->GetDBTableGUIDLow() < 561195 || target->ToCreature()->GetDBTableGUIDLow() > 561199); });

                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RESET); // disable all before run

                        launcherBunnyGUID = ObjectGuid::Empty;

                        if (!waveBeams.empty())
                            launcherBunnyGUID = Trinity::Containers::SelectRandomContainerElement(waveBeams)->GetGUID();

                        waveBeams.remove_if([=](Creature* target) { return target->GetGUID() == launcherBunnyGUID; });

                        scheduler
                            .Schedule(Seconds(1), [this, waveBeams, launcherBunnyGUID](TaskContext context)
                        {
                            if (instance && instance->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == IN_PROGRESS)
                            {
                                // Second Wave
                                for (auto&& itr : waveBeams)
                                    itr->AI()->DoAction(ACTION_CONVEYOR_RUN);

                                if (Creature* selectedLauncher = ObjectAccessor::GetCreature(*me, launcherBunnyGUID))
                                    selectedLauncher->CastSpell(selectedLauncher, SPELL_CONVEYOR_DEATH_BEAM_HC, true);
                            }
                        });

                        waveBeams.clear();
                        GetCreatureListWithEntryInGrid(waveBeams, me, NPC_LASSER_ARRAY, 300.0f);

                        waveBeams.remove_if([=](Creature* target) { return target && target->ToCreature() && (target->ToCreature()->GetDBTableGUIDLow() < 561200 || target->ToCreature()->GetDBTableGUIDLow() > 561204); });

                        for (auto&& itr : waveBeams)
                            itr->AI()->DoAction(ACTION_CONVEYOR_RESET); // disable all before run

                        launcherBunnyGUID = ObjectGuid::Empty;

                        if (!waveBeams.empty())
                            launcherBunnyGUID = Trinity::Containers::SelectRandomContainerElement(waveBeams)->GetGUID();

                        waveBeams.remove_if([=](Creature* target) { return target->GetGUID() == launcherBunnyGUID; });

                        scheduler
                            .Schedule(Seconds(2), [this, waveBeams, launcherBunnyGUID](TaskContext context)
                        {
                            if (instance && instance->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == IN_PROGRESS)
                            {
                                // Last Wave
                                for (auto&& itr : waveBeams)
                                    itr->AI()->DoAction(ACTION_CONVEYOR_RUN);

                                if (Creature* selectedLauncher = ObjectAccessor::GetCreature(*me, launcherBunnyGUID))
                                    selectedLauncher->CastSpell(selectedLauncher, SPELL_CONVEYOR_DEATH_BEAM_HC, true);
                            }
                        });
                    }

                    events.ScheduleEvent(EVENT_PREPARE_DEACTIVATED_WEAPONS, 40 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_LAUNCH_DEACTIVATED_WEAPONS, 1 * IN_MILLISECONDS);
                    break;
                }
                case EVENT_LAUNCH_DEACTIVATED_WEAPONS:
                {
                    if (m_deactivatedCombo.empty())
                        break;

                    // Summon deactivated weapons with interval 2,5s till container not empty
                    if (uint32 selectedType = *m_deactivatedCombo.begin())
                    {
                        me->CastSpell(ConveyorWeaponsPoints[0], selectedType, false);
                        m_deactivatedCombo.erase(std::find(m_deactivatedCombo.begin(), m_deactivatedCombo.end(), selectedType));
                    }
                
                    events.ScheduleEvent(EVENT_LAUNCH_DEACTIVATED_WEAPONS, 3 * IN_MILLISECONDS);

                    break;
                }
                case EVENT_LAUNCH_ACTIVATED_WEAPONS:
                {
                    if (m_activatedCombo.empty())
                    {
                        length_counter = 0;
                        break;
                    }

                    // Summon activated weapons with interval 2,5s till container not empty
                    if (uint32 m_selectedType = *m_activatedCombo.begin())
                    {
                        if (uint32 m_weaponType = ActivatedConveyorDict.find(m_selectedType)->second)
                        {
                            if (m_weaponType == SPELL_ACTIVATED_MAGNET_TURRET)
                                me->CastSpell(ConveyorWeaponsPoints[1], m_weaponType, false);
                            else if (m_weaponType == NPC_SIEGECRAFTER_CRAWLER_MINE)
                                me->CastSpell(ConveyorWeaponsPoints[1], SPELL_ACTIVATED_CRAWLER_MINE_CONTROLER, false);
                            else if (m_weaponType == SPELL_SUMMON_ACTIVATED_DEATHDEALER_TURRET)
                                me->CastSpell(ConveyorWeaponsPoints[1], SPELL_SUMMON_ACTIVATED_DEATHDEALER_TURRET, false);
                            else
                                me->SummonCreature(m_weaponType, ConveyorWeaponsPoints[1], TEMPSUMMON_MANUAL_DESPAWN);
                        }
                        
                        uint32 matchWeaponEntry = invSelectWeaponType.find(m_selectedType)->second;

                        tempOvercharged.push_back(matchWeaponEntry);
                        m_activatedCombo.erase(std::find(m_activatedCombo.begin(), m_activatedCombo.end(), m_selectedType));
                    }

                    if (IsHeroic() && m_activatedCombo.empty()) // set overcharge one of them
                    {
                        // Select weapon that we`ll overcharge:: Remove last used
                        if (prevOvercharged && std::find(tempOvercharged.begin(), tempOvercharged.end(), prevOvercharged) != tempOvercharged.end())
                            tempOvercharged.erase(std::find(tempOvercharged.begin(), tempOvercharged.end(), prevOvercharged));

                        // Set Priority
                        std::vector<uint32> prioritySeq = { NPC_ACTIVATED_ELECTROMAGNET, NPC_ACTIVATED_CRAWLER_MINE_VEHICLE, NPC_ACTIVATED_MISSLE_TURRET, NPC_ACTIVATED_LASER_TURRET, NPC_ACTIVATED_DEATHDEALER_TURRET };

                        for (auto&& itr : prioritySeq)
                        {
                            if (std::find(tempOvercharged.begin(), tempOvercharged.end(), itr) != tempOvercharged.end())
                            {
                                prevOvercharged = itr;
                                break;
                            }
                        }

                        tempOvercharged.clear();

                        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
                            siegeCrafter->AI()->DoAction(ACTION_OVERCHARGE_WEAPON);
                    }

                    events.ScheduleEvent(EVENT_LAUNCH_ACTIVATED_WEAPONS, 3 * IN_MILLISECONDS);
                    break;
                }
            }
        }
    }
};

// Deactivated Weapons 71606, 71751, 71694, 71790, 72904
struct npc_soo_deactivated_weapons : public ScriptedAI
{
    npc_soo_deactivated_weapons(Creature* creature) : ScriptedAI(creature) { }

    InstanceScript* instance;
    EventMap nonCombatEvents;
    float x, y;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        instance = me->GetInstanceScript();
        GetPositionWithDistInOrientation(me, 205.0f, me->GetOrientation(), x, y);

        Movement::MoveSplineInit init(me);
        init.MoveTo(x, y, me->GetPositionZ());
        init.SetVelocity(6.75f);
        init.Launch();
        nonCombatEvents.ScheduleEvent(EVENT_REACH_CONVEYOR, me->GetSplineDuration());

        if (me->GetEntry() == NPC_DEACTIVATED_DEATHDEALER_TURRET)
            DoCast(me, SPELL_DEATHDEALER_TURRET_VISUAL);

        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
    }

    void Unsummoned() override
    {
        if (instance)
            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (instance)
        {
            if (Creature* helper = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_SIEGECRAFTER_HELPER)))
            {
                helper->AI()->DoAction(ACTION_ANY_DEACTIVATED_HAS_DISSAPEAR);
                helper->AI()->SetData(TYPE_CONVEYOR_WEAPON_DESTROYED, 1);
            }

            if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
                mySiegecrafter->AI()->Talk(TALK_WEAPON_DEATH);

            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        switch (me->GetEntry())
        {
            case NPC_DIACTIVATED_MISSLE_TURRET:
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_SHOCKWAVE, 0);
                break;
            case NPC_DIACTIVATED_LASER_TURRET:
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_LASER, 0);
                break;
            case NPC_DIACTIVATED_ELECTROMAGNET:
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_MAGNET, 0);
                break;
            case NPC_DISASSEMBLED_CRAWLER_MINES:
                me->GetMap()->SetWorldState(WORLDSTATE_LASERS_MAGNITES_NOT_MINES, 0);
                break;
        }

        me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        nonCombatEvents.Update(diff);

        while (uint32 eventId = nonCombatEvents.ExecuteEvent())
        {
            if (eventId == EVENT_REACH_CONVEYOR)
            {
                if (Creature* m_helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                    m_helper->AI()->SetData(me->GetEntry(), DONE);

                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                me->DespawnOrUnsummon();
            }
            break;
        }
    }
};

// Activated Weapons 72015, 71696, 72027, 71795, 72905
struct npc_soo_activated_weapons : public ScriptedAI
{
    npc_soo_activated_weapons(Creature* creature) : ScriptedAI(creature), vehicle(creature->GetVehicleKit()) { }

    TaskScheduler scheduler;
    InstanceScript* instance;
    Vehicle* vehicle;
    EventMap events;
    uint32 m_LengthData;
    uint32 leaveConveyor;
    ObjectGuid m_uWeaponGUID;
    int32 vehicleSeat;

    void Reset() override
    {
        vehicleSeat = 1;
        leaveConveyor = 0;
        instance = me->GetInstanceScript();

        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
            siegeCrafter->AI()->JustSummoned(me);

        // Summon mines on me depend of difficulty
        if (me->GetEntry() == NPC_ACTIVATED_CRAWLER_MINE_VEHICLE)
        {
            if (Is25ManRaid())
                me->SendSetVehicleRecId(2928);

            // Init Delay
            scheduler
                .Schedule(Milliseconds(1500), [this](TaskContext context)
            {
                uint8 mineCount = Is25ManRaid() ? 8 : 3;

                if (me->GetInstanceScript() && me->GetInstanceScript()->GetData(DATA_FLEX))
                {
                    // Calculate TargetsCount
                    mineCount = 3; // min 3, max 8
                    uint32 totalPlayersCount = instance->instance->GetPlayersCountExceptGMs();

                    if (totalPlayersCount > 12)
                        mineCount++;

                    if (totalPlayersCount > 14)
                        mineCount++;

                    if (totalPlayersCount > 17)
                        mineCount++;

                    if (totalPlayersCount > 19)
                        mineCount++;

                    if (totalPlayersCount > 23)
                        mineCount++;
                }

                for (uint8 i = 0; i < mineCount; i++)
                {
                    if (Creature* crawlerMine = me->SummonCreature(NPC_SIEGECRAFTER_CRAWLER_MINE, *me, TEMPSUMMON_MANUAL_DESPAWN))
                        crawlerMine->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &vehicleSeat, 0, 0, true);

                    vehicleSeat++;
                }
            });
        }
        else if (me->GetEntry() == NPC_ACTIVATED_DEATHDEALER_TURRET)
            DoCast(me, SPELL_DEATHDEALER_TURRET_VISUAL);

        if (Creature* helper = ObjectAccessor::GetCreature(*me, instance ? instance->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
        {
            m_LengthData = helper->AI()->GetData(TYPE_CONVEYOR_LENGTH);
            helper->AI()->SetData(TYPE_CONVEYOR_LENGTH, DONE);

            Movement::MoveSplineInit init(me);
            init.MoveTo(ConveyorActivatedPoints[m_LengthData].GetPositionX(), ConveyorActivatedPoints[m_LengthData].GetPositionY(), ConveyorActivatedPoints[m_LengthData].GetPositionZ());
            init.SetVelocity(17.0f);
            init.Launch();
            events.ScheduleEvent(EVENT_REACH_CONVEYOR, me->GetSplineDuration());
        }
    }

    void HandleSetActionOnPassenger(uint32 m_action)
    {
        if (vehicle)
        {
            if (me->GetEntry() != NPC_ACTIVATED_CRAWLER_MINE_VEHICLE)
            {
                if (Unit* passenger = vehicle->GetPassenger(0))
                    if (passenger->ToCreature() && passenger->ToCreature()->AI())
                        passenger->ToCreature()->AI()->DoAction(m_action);
            }
            else
            {
                uint8 seats = Is25ManRaid() ? 9 : 3;

                if (instance && instance->GetData(DATA_FLEX))
                {
                    // Calculate TargetsCount
                    seats = 3; // min 3, max 9 (8)
                    uint32 totalPlayersCount = instance->instance->GetPlayersCountExceptGMs();

                    if (totalPlayersCount > 12)
                        seats++;

                    if (totalPlayersCount > 15)
                        seats++;

                    if (totalPlayersCount > 18)
                        seats++;

                    if (totalPlayersCount > 21)
                        seats++;

                    if (totalPlayersCount > 23)
                        seats++;

                    if (totalPlayersCount > 24)
                        seats++;
                }

                for (uint8 i = 1; i < seats + 1; i++)
                {
                    if (Unit* passenger = vehicle->GetPassenger(seats))
                    {
                        if (passenger->ToCreature() && passenger->ToCreature()->AI())
                            passenger->ToCreature()->AI()->DoAction(m_action);
                    }
                }
            }
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_NOT_IN_CONVEYOR_LINE)
            return leaveConveyor;

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TYPE_NOT_IN_CONVEYOR_LINE)
            leaveConveyor = data;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_REACH_CONVEYOR:
                {
                    HandleSetActionOnPassenger(ACTION_LAUNCH_ACTIVATED_WEAPON);

                    if (me->GetEntry() == NPC_ACTIVATED_ELECTROMAGNET)
                    {
                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_LAUNCH_ELECTROMAGNET, 2 * IN_MILLISECONDS);
                        else
                            DoCast(me, SPELL_MAGNETIC_CRUSH);
                    }
                    else if (me->GetEntry() == NPC_ACTIVATED_CRAWLER_MINE_VEHICLE)
                        DoCast(me, SPELL_CRAWLER_MINE_CONTROLLER_PERIODIC);
                    else if (me->GetEntry() == NPC_ACTIVATED_DEATHDEALER_TURRET)
                    {
                        DoCast(me, SPELL_DEATHDEALER_TURRET_CONTROL_PERIODIC);

                        // Also launch shockwave Missle
                        if (TempSummon* shockwaveMissle = me->SummonCreature(NPC_SHOCKWAVE_MISSLE_CASTER_STALKER, CalculateMissleThrowPosition(me, me->GetGUID()), TEMPSUMMON_MANUAL_DESPAWN))
                            me->CastSpell(shockwaveMissle->GetPositionX(), shockwaveMissle->GetPositionY(), shockwaveMissle->GetPositionZ(), SPELL_SHOCKWAVE_MISSLE, false);
                    }

                    events.ScheduleEvent(EVENT_REMOVE_CONVEYOR, 30 * IN_MILLISECONDS);
                    break;
                }
                case EVENT_REMOVE_CONVEYOR:
                {
                    if (me->GetEntry() == NPC_ACTIVATED_ELECTROMAGNET)
                    {
                        SetData(TYPE_NOT_IN_CONVEYOR_LINE, 1);
                        DoCast(me, SPELL_MAGNETIC_PULSE_SAW_SELECTOR, true);
                        me->RemoveAllAreasTrigger();
                        me->RemoveAurasDueToSpell(SPELL_MAGNETIC_CRUSH);
                        me->RemoveAurasDueToSpell(SPELL_MAGNETIC_CRUSH_PUSH);
                        me->RemoveAurasDueToSpell(SPELL_MAGNETIC_CRUSH_PULL);
                    }
                    else if (me->GetEntry() == NPC_ACTIVATED_DEATHDEALER_TURRET)
                    {
                        SetData(TYPE_NOT_IN_CONVEYOR_LINE, 1);
                        me->RemoveAurasDueToSpell(SPELL_DEATHDEALER_TURRET_CONTROL_PERIODIC);
                    }
                    else if (me->GetEntry() == NPC_ACTIVATED_CRAWLER_MINE_VEHICLE)
                        SetData(TYPE_NOT_IN_CONVEYOR_LINE, 1);
                    else
                    {
                        // Set it on our passengers
                        if (vehicle)
                            if (Unit* passenger = vehicle->GetPassenger(0))
                                if (passenger->ToCreature() && passenger->ToCreature()->AI())
                                    passenger->ToCreature()->AI()->SetData(TYPE_NOT_IN_CONVEYOR_LINE, 1);
                    }

                    me->StopMoving();
                    Movement::MoveSplineInit init(me);
                    init.MoveTo(ConveyorActivatedPoints[3].GetPositionX(), ConveyorActivatedPoints[3].GetPositionY(), ConveyorActivatedPoints[3].GetPositionZ());
                    init.SetVelocity(18.5f);
                    init.Launch();
                    events.ScheduleEvent(EVENT_REMOVE_CONVEYOR_DELAY, me->GetSplineDuration());
                    break;
                }
                case EVENT_REMOVE_CONVEYOR_DELAY:
                    HandleSetActionOnPassenger(ACTION_REMOVE_ACTIVATED_WEAPON);
                    break;
                case EVENT_LAUNCH_ELECTROMAGNET:
                    DoCast(me, me->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL) ? SPELL_MAGNETIC_CRUSH_PUSH : SPELL_MAGNETIC_CRUSH);
                    break;
            }
        }
    }
};

// Activated Weapons Type 71788, 71638, 71752
struct npc_soo_activated_weapons_type : public ScriptedAI
{
    npc_soo_activated_weapons_type(Creature* creature) : ScriptedAI(creature), summons(me)
    {
        SetCombatMovement(false);
    }

    TaskScheduler scheduler;
    uint32 m_templateType, m_laserLOW;
    ObjectGuid targetGUID;
    ObjectGuid summonerGUID;
    uint32 summonerEntry;
    uint32 hasDetonate;
    uint32 hasUsing;
    uint32 launchedCount;
    uint32 leaveConveyor;
    ObjectGuid fixateGUID;
    SummonList summons;
    EventMap events;
    InstanceScript* instance;
    bool allowDetonate;
    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerEntry = summoner->GetEntry();
        summonerGUID = summoner->GetGUID();

        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
            siegeCrafter->AI()->JustSummoned(me);

        if (summoner->GetEntry() == NPC_SIEGECRAFTER_CRAWLER_MINE)
        {
            DoCast(me, SPELL_CRAWLER_MINE_SELECTOR);
            events.ScheduleEvent(EVENT_FIXATE, 2 * IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_READY, 1 * MINUTE * IN_MILLISECONDS);
        }
    }

    void Reset() override
    {
        m_templateType = 0;
        targetGUID = ObjectGuid::Empty;
        hasDetonate    = 0;
        fixateGUID = ObjectGuid::Empty;
        hasUsing       = 0;
        launchedCount  = 0;
        leaveConveyor  = 0;
        allowDetonate = false;
        x = 0.0f;
        y = 0.0f;
        summons.DespawnAll();
        instance = me->GetInstanceScript();

        switch (me->GetEntry())
        {
            case NPC_ACTIVATED_MISSLE_TURRET:
                m_templateType = SPELL_ACTIVATED_ROCKET_TURRET;
                break;
            case NPC_ACTIVATED_LASER_TURRET:
                m_templateType = SPELL_ACTIVATED_LASER_TURRET_BASE;
                break;
            case NPC_SIEGECRAFTER_CRAWLER_MINE:
                me->ApplySpellImmune(0, IMMUNITY_ID, 3355, true); // allow MECHANIC_FREEZE exclude freezing trap

                if (sGameEventMgr->IsActiveEvent(2))
                    DoCast(me, SPELL_CRAWLER_MINE_HOLIDAY_VISUAL, true);
                break;
        }

        if (m_templateType)
            me->CastSpell(me, m_templateType, false);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_LAUNCH_ACTIVATED_WEAPON:
                me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI / 2));

                if (me->GetEntry() == NPC_ACTIVATED_LASER_TURRET)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

                me->SetInCombatWithZone();

                if (me->GetEntry() != NPC_SIEGECRAFTER_CRAWLER_MINE)
                    events.ScheduleEvent(EVENT_SPECIAL_LAUNCH, 2 * IN_MILLISECONDS);
                break;
            case ACTION_REMOVE_ACTIVATED_WEAPON:
                if (me->GetEntry() != NPC_ACTIVATED_MISSLE_TURRET && me->GetEntry() != NPC_ACTIVATED_LASER_TURRET)
                    summons.DespawnAll();

                me->DespawnOrUnsummon();
                break;
            case ACTION_SIEGECRAFTER_MINE_LAUNCH:
                if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                {
                    GetPositionWithDistInOrientation(me, 40.0f, me->GetAngle(controller), x, y);

                    if (me->IsOnVehicle())
                        me->ExitVehicle();

                    if (Creature* summoner = ObjectAccessor::GetCreature(*me, summonerGUID))
                    {
                        if (summoner->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL))
                        {
                            CreatureTemplate const* normalInfo = sObjectMgr->GetCreatureTemplate(NPC_OVERCHARGED_CRAWLER_MINE);
                            //me->SetName(normalInfo->Name);

                            if (normalInfo)
                                me->SetDisplayId(normalInfo->GetModelByIdx(0)->CreatureDisplayID);

                            me->AddAura(SPELL_OVERCHARGED_WEAPON_VISUAL, me); // not need announe it in DBM
                            uint32 maxHealthByDefault = me->GetMaxHealth();
                            me->SetCreateHealth(maxHealthByDefault / 2);
                            me->SetMaxHealth(maxHealthByDefault / 2);
                            me->SetHealth(maxHealthByDefault / 2);
                            me->ResetPlayerDamageReq();
                        }
                    }

                    me->GetMotionMaster()->MoveJump(x, y, -309.327f, 25.0f, 15.0f, EVENT_JUMP);
                }
                break;
            case ACTION_CRAWLER_MINE_REFIXATE:
                if (GetData(TYPE_CRAWLER_MINE_DETONATE) || HealthBelowPct(5))
                    break;

                allowDetonate = false;

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    DoCast(me, SPELL_CRAWLER_MINE_SELECTOR);
                    me->PrepareChanneledCast(me->GetOrientation());
                    events.ScheduleEvent(EVENT_FIXATE, 2 * IN_MILLISECONDS);
                });
                break;
            case ACTION_CRAWLER_MINE_DISSAPEAR:
                events.Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                if (Unit* target = ObjectAccessor::GetUnit(*me, GetGUID(0)))
                    target->RemoveAurasDueToSpell(SPELL_CRAWLER_MINE_FIXATE_AURA);

                if (me->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL))
                {
                    uint8 count = Is25ManRaid() ? 3 : 2;
                    for (uint8 i = 0; i < count; i++)
                    {
                        x = me->GetPositionX() + 8.0f * std::cos(frand(0.0f, 2 * M_PI));
                        y = me->GetPositionY() + 8.0f * std::sin(frand(0.0f, 2 * M_PI));
                        float z = me->GetPositionZ();

                        Trinity::NormalizeMapCoord(x);
                        Trinity::NormalizeMapCoord(y);
                        me->UpdateGroundPositionZ(x, y, z);
                        if (Creature * crawlerMine = me->SummonCreature(NPC_SIEGECRAFTER_CRAWLER_MINE, x, y, z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            crawlerMine->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                            crawlerMine->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                            crawlerMine->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);
                            crawlerMine->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);

                            crawlerMine->SetInCombatWithZone();
                            crawlerMine->CastSpell(crawlerMine, SPELL_CRAWLER_MINE_BREAK_IN_PERIOD, false);
                        }
                    }
                }

                me->DespawnOrUnsummon(2 * IN_MILLISECONDS);
                break;
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        switch (pointId)
        {
            case EVENT_JUMP:
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);

                me->SetInCombatWithZone();
                DoCast(me, SPELL_CRAWLER_MINE_BREAK_IN_PERIOD);

                DoCast(me, SPELL_CRAWLER_MINE_SELECTOR);
                events.ScheduleEvent(EVENT_FIXATE, 2 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_READY, 1 * MINUTE * IN_MILLISECONDS);
                break;
            case EVENT_JUMP + 1:
            {
                me->CastSpell(me, SPELL_CRAWLER_MINE_DETONATE, false);
                me->DespawnOrUnsummon();
                break;
            }
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        // Set Vehicle on Base
        switch (summon->GetEntry())
        {
            case NPC_ACTIVATED_LASER_TURRET_BASE:
            case NPC_ACTIVATED_MISSLE_TURRET_BASE:
                me->CastSpell(summon, VEHICLE_SPELL_RIDE_HARDCODED, true);
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            target->RemoveAurasDueToSpell(SPELL_CRAWLER_MINE_FIXATE_AURA);

        if (me->GetEntry() == NPC_SIEGECRAFTER_CRAWLER_MINE && me->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL))
        {
            for (uint8 i = 0; i < 2; i++)
            {
                x = me->GetPositionX() + 8.0f * std::cos(frand(0.0f, 2 * M_PI));
                y = me->GetPositionY() + 8.0f * std::sin(frand(0.0f, 2 * M_PI));
                float z = me->GetPositionZ();

                Trinity::NormalizeMapCoord(x);
                Trinity::NormalizeMapCoord(y);
                me->UpdateGroundPositionZ(x, y, z);
                if (Creature * crawlerMine = me->SummonCreature(NPC_SIEGECRAFTER_CRAWLER_MINE, x, y, z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    crawlerMine->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    crawlerMine->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                    crawlerMine->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_THREAT, true);
                    crawlerMine->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);

                    crawlerMine->SetInCombatWithZone();
                    crawlerMine->CastSpell(crawlerMine, SPELL_CRAWLER_MINE_BREAK_IN_PERIOD, false);
                }
            }
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TYPE_LASER_TARGET_GUID:
                return m_laserLOW;
            case TYPE_CRAWLER_MINE_DETONATE:
                return hasDetonate;
            case TYPE_LASSER_TURRET_USING:
                return hasUsing;
            case TYPE_NOT_IN_CONVEYOR_LINE:
                return leaveConveyor;
            case TYPE_ALLOW_DETONATE_CRAWLER_MINE:
                return allowDetonate ? 1 : 0;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_CRAWLER_MINE_DETONATE:
                hasDetonate = data;
                break;
            case TYPE_LASSER_TURRET_USING:
                hasUsing = data;
                break;
            case TYPE_NOT_IN_CONVEYOR_LINE:
                leaveConveyor = data;
                break;
        }
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        fixateGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return fixateGUID;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPECIAL_LAUNCH:
                {
                    switch (me->GetEntry())
                    {
                        case NPC_ACTIVATED_LASER_TURRET:
                        {
                            if (!me->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL))
                            {
                                if (IsHeroic())
                                {
                                    SetData(TYPE_LASSER_TURRET_USING, 0);

                                    if (++launchedCount < 2)
                                        events.ScheduleEvent(EVENT_SPECIAL_LAUNCH, 16.5 * IN_MILLISECONDS);
                                }

                                DoCast(me, SPELL_LASER_TURRET_SELECTOR);
                            }
                            else if (IsHeroic())// 3 arc of fireline
                            {
                                SetData(TYPE_LASSER_TURRET_USING, 1);

                                for (auto&& itr : overchargedLaserBunnyStartPos)
                                    me->CastSpell(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ(), SPELL_LASER_TURRET_TARGETING_BUNNY, true);
                            }
                            break;
                        }
                        case NPC_ACTIVATED_MISSLE_TURRET:
                        {
                            if (TempSummon* shockwaveMissle = me->SummonCreature(NPC_SHOCKWAVE_MISSLE_CASTER_STALKER, CalculateMissleThrowPosition(me, me->GetGUID(), launchedCount), TEMPSUMMON_MANUAL_DESPAWN))
                                me->CastSpell(shockwaveMissle->GetPositionX(), shockwaveMissle->GetPositionY(), shockwaveMissle->GetPositionZ(), me->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL) ? SPELL_SHOCKWAVE_MISSLE_LAUNCH_HC : SPELL_SHOCKWAVE_MISSLE, false);

                            if (me->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL) && ++launchedCount < 4)
                                events.ScheduleEvent(EVENT_SPECIAL_LAUNCH, 12 * IN_MILLISECONDS);
                            else if (IsHeroic() && ++launchedCount < 2)
                                events.ScheduleEvent(EVENT_SPECIAL_LAUNCH, 15 * IN_MILLISECONDS);
                            break;
                        }
                    }
                    break;
                }
                case EVENT_FIXATE:
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetGUID(0)))
                    {
                        if (target->IsAlive())
                        {
                            allowDetonate = true;
                            AttackStart(target);
                            me->RemoveChanneledCast(target->GetGUID());
                            me->SetReactState(REACT_PASSIVE);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                        }
                        else
                            DoAction(ACTION_CRAWLER_MINE_REFIXATE);
                    }
                    else
                        DoAction(ACTION_CRAWLER_MINE_REFIXATE);

                    break;
                }
                case EVENT_READY:
                    me->RemoveAurasDueToSpell(SPELL_CRAWLER_MINE_BREAK_IN_PERIOD);
                    DoCast(me, SPELL_CRAWLER_MINE_READY_TO_GO);
                    break;
            }
        }
    }
};

// Laser target bunny 71740
struct npc_soo_laser_target_bunny : public ScriptedAI
{
    npc_soo_laser_target_bunny(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid targetGUID;
    float x, y;
    bool stopMovement;
    bool summonedByEvent;
    bool byOvercharged;

    void IsSummonedBy(Unit* summoner) override
    {
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == IN_PROGRESS)
        {
            if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
                siegeCrafter->AI()->JustSummoned(me);
        }
        else if (Creature* shanna = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SHANNA_SPARKFIZZ) : ObjectGuid::Empty))
            shanna->AI()->JustSummoned(me);

        targetGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        stopMovement = false;
        byOvercharged = false;
        me->SetInCombatWithZone();
        me->PrepareChanneledCast(me->GetOrientation());
        me->SetSpeed(MOVE_WALK, 0.64f);
        me->SetSpeed(MOVE_RUN, 0.64f);
        x = 0.0f; y = 0.0f;

        // in case if we have 2 turrets on line
        if (me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == IN_PROGRESS)
        {
            if (Creature* laserTurret = ObjectAccessor::GetCreature(*me, getLasserTurretGUID()))
            {
                laserTurret->AI()->SetData(TYPE_LASSER_TURRET_USING, 1);
                laserTurret->CastSpell(me, SPELL_DISINTEGRATION_LASER, false);
            }
        }
        else
        {
            if (Creature* shanna = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SHANNA_SPARKFIZZ) : ObjectGuid::Empty))
                shanna->CastSpell(me, SPELL_DISINTEGRATION_LASER, false);
        }

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            if (Player* owner = ObjectAccessor::GetPlayer(*me, targetGUID))
            {
                Talk(TALK_LOCKED_ON_ANN, owner);
                DoCast(owner, SPELL_LOCKED_ON);
                me->ClearUnitState(UNIT_STATE_CASTING);
            }
            else // Overcharged
            {
                byOvercharged = true;

                if (!me->HasAura(SPELL_LASER_GROUND_EFFECT_PERIODIC))
                    DoCast(me, SPELL_LASER_GROUND_EFFECT_PERIODIC, true);

                if (Creature* lasserTurret = ObjectAccessor::GetCreature(*me, targetGUID))
                    me->CastSpell(lasserTurret, SPELL_DISINTEGRATION_LASER, true);

                if (Creature* siegecrafterHelper = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                {
                    float dist = me->GetExactDist2d(siegecrafterHelper);
                    if (dist < 0.5f)
                    {
                        TC_LOG_ERROR("shitlog", "npc_soo_laser_target_bunny, dist %f", dist);
                        dist = 0.5f;
                    }
                    float mod = dist > 50.0f ? (1.82 * dist) / 15.68 : (1.2 * dist) / 15.68;
                    if (!mod)
                        mod = 0.1f;

                    me->SetWalk(false);
                    me->SetSpeed(MOVE_RUN, mod);
                    me->UpdateSpeed(MOVE_RUN, true);

                    Movement::MoveSplineInit init(me);
                    for (uint8 i = 1; i < 37; ++i)
                    {
                        float step = -(i * M_PI / 20);
                        x = siegecrafterHelper->GetPositionX() + (dist * cos(Position::NormalizeOrientation(siegecrafterHelper->GetAngle(me) + step)));
                        y = siegecrafterHelper->GetPositionY() + (dist * sin(Position::NormalizeOrientation(siegecrafterHelper->GetAngle(me) + step)));

                        init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
                    }

                    init.Launch();
                    init.SetWalk(false);
                    init.SetVelocity(mod);

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        stopMovement = true;
                        me->RemoveAllAuras();
                        me->PrepareChanneledCast(me->GetOrientation());
                        me->DespawnOrUnsummon(50 * IN_MILLISECONDS);
                    });
                }
            }
        });

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            if (stopMovement || byOvercharged)
                return;

            if (!me->HasAura(SPELL_LASER_GROUND_EFFECT_PERIODIC))
                DoCast(me, SPELL_LASER_GROUND_EFFECT_PERIODIC, true);

            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                if (target->IsAlive())
                    me->GetMotionMaster()->MoveFollow(target, 0.0f, me->GetAngle(target));
                else if (Creature* owner = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(me->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) != IN_PROGRESS ? DATA_SHANNA_SPARKFIZZ : DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
                {
                    // Select new target by owner
                    if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector(SPELL_ON_CONVEYOR)))
                    {
                        Talk(TALK_LOCKED_ON_ANN, newTarget);
                        DoCast(newTarget, SPELL_LOCKED_ON);
                        targetGUID = newTarget->GetGUID();
                        me->GetMotionMaster()->MoveFollow(newTarget, 0.0f, me->GetAngle(newTarget));
                    }
                    else if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true, -SPELL_ON_CONVEYOR))
                    {
                        Talk(TALK_LOCKED_ON_ANN, newTarget);
                        DoCast(newTarget, SPELL_LOCKED_ON);
                        targetGUID = newTarget->GetGUID();
                        me->GetMotionMaster()->MoveFollow(newTarget, 0.0f, me->GetAngle(newTarget));
                    }
                }
            }

            context.Repeat(Seconds(1));
        });

        // not remove trigger until areatrigger not expire
        scheduler
            .Schedule(Seconds(16), [this](TaskContext context)
        {
            if (byOvercharged)
                return;

            stopMovement = true;
            me->RemoveAllAuras();
            me->PrepareChanneledCast(me->GetOrientation());
            me->DespawnOrUnsummon(50 * IN_MILLISECONDS);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    private:
        ObjectGuid getLasserTurretGUID()
        {
            std::list<Creature*> lasserTurretsList;
            GetCreatureListWithEntryInGrid(lasserTurretsList, me, NPC_ACTIVATED_LASER_TURRET, 300.0f);
            lasserTurretsList.remove_if([=](Creature* target) { return target && (target->AI()->GetData(TYPE_LASSER_TURRET_USING) || target->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL)); });

            if (lasserTurretsList.empty())
                return ObjectGuid::Empty;

            return Trinity::Containers::SelectRandomContainerElement(lasserTurretsList)->GetGUID();
        }
};

// Laser Array bunny 71910
struct npc_soo_laser_array_bunny : public ScriptedAI
{
    npc_soo_laser_array_bunny(Creature* creature) : ScriptedAI(creature) { }

    float x, y;
    float staticOri;

    void InitializeAI() override
    {
        me->SetDisplayId(11686);
        x = 0.0f;
        y = 0.0f;
        staticOri = me->GetOrientation();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_CONVEYOR_RESET:
                if (me->IsOnVehicle())
                {
                    me->ExitVehicle();
                    me->GetMotionMaster()->MoveTargetedHome();
                }

                me->RemoveAurasDueToSpell(SPELL_CONVEYOR_DEATH_BEAM_AURA);
                me->RemoveAllAreasTrigger();
                me->SetFacingTo(staticOri);
                break;
            case ACTION_CONVEYOR_RUN:
                DoCast(me, SPELL_CONVEYOR_DEATH_BEAM_AURA);
                DoCast(me, SPELL_CONVEYOR_DEATH_DEAM);

                /*GetPositionWithDistInOrientation(me, 14.0f, Position::NormalizeOrientation(me->GetOrientation() + M_PI / 2), x, y);
            
                if (TempSummon* m_vehicle = me->SummonCreature(NPC_LASER_ARRAY_TARGET_VEHICLE, x, y, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
                {
                    //DoCast(me, VEHICLE_SPELL_RIDE_HARDCODED);
                    DoCast(me, SPELL_CONVEYOR_DEATH_BEAM_AURA);
                    DoCast(me, SPELL_CONVEYOR_DEATH_DEAM);
                }*/
                break;
        }
    }
};

// Shockwave missle 72052, 72832
struct npc_soo_shockwave_missle : public ScriptedAI
{
    npc_soo_shockwave_missle(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
            siegeCrafter->AI()->JustSummoned(me);
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        SetCombatMovement(false);
        me->CastSpell(me, me->GetEntry() == NPC_SHOCKWAVE_MISSLE_HC ? SPELL_SHOCKWAVE_MISSLE_VISUAL_HC : SPELL_SHOCKWAVE_VISUAL, true);
        me->SetInCombatWithZone();

        if (me->GetEntry() == NPC_SHOCKWAVE_MISSLE_HC) // unk real hitbox
        {
            me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 0.76f);
            me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10.0f);
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        // Required: visible for first inner circle
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            DoCast(me, SPELL_SHOCKWAVE_INNER_CIRCLE);
        });

        if (Creature* helper = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
            me->PrepareChanneledCast(me->GetAngle(helper)); // required
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_SHOCKWAVE_DESTRUCTION);
        me->DespawnOrUnsummon(2 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Shockwave missle 72710, Lasser Array Vehicle 71912
struct npc_siegecrafter_shockwave_missle_trigger : public ScriptedAI
{
    npc_siegecrafter_shockwave_missle_trigger(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override
    {
        if (Creature* siegeCrafter = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_SIEGECRAFTER_BLACKFUSE) : ObjectGuid::Empty))
            siegeCrafter->AI()->JustSummoned(me);
    }

    void Reset() override
    {
        me->SetDisplayFromModel(me->GetEntry() == NPC_LASER_ARRAY_TARGET_VEHICLE ? 1 : 0);
    }
};

// This spells have a radius hit box, idea in exclude by type range where players shouldn`t affect
class ShockwaveRangePredicate 
{
    public:
        ShockwaveRangePredicate(uint32 const& spell_id, Unit* const m_caster) : _spell_id(spell_id), _caster(m_caster) { }

        bool operator()(WorldObject* object)
        {
            switch (_spell_id)
            {
                case SPELL_SHOCKWAVE_CIRCLE_1:
                    range = 10.0f;
                    break;
                case SPELL_SHOCKWAVE_CIRCLE_2:
                    range = 25.0f;
                    break;
                case SPELL_SHOCKWAVE_CIRCLE_3:
                    range = 45.0f;
                    break;
                case SPELL_SHOCKWAVE_CIRCLE_4:
                    range = 65.0f;
                    break;
                case SPELL_SHOCKWAVE_CIRCLE_5:
                    range = 85.0f;
                    break;
            }

            if (range)
                if (_caster->GetExactDist2d(object) < range)
                    return true;

            return false;
        }

    private:
        uint32 const& _spell_id;
        Unit const* _caster;
        float range;
};

class PlayerSelectorPredicate
{
    public:
        PlayerSelectorPredicate() {}

        bool operator() (WorldObject* object)
        {
            return object && !object->ToPlayer();
        }
};

// Shockwave Missle 144658, 144660, 144661, 144662, 144663, 144664
class spell_soo_shockwave_missle : public SpellScript
{
    PrepareSpellScript(spell_soo_shockwave_missle);

    void HandleAfterCast()
    {
        if (SpellInfo const* m_spellType = GetSpellInfo())
        {
            if (Unit* caster = GetCaster())
            {
                switch (m_spellType->Id)
                {
                    case SPELL_SHOCKWAVE_INNER_CIRCLE:
                        caster->CastSpell(caster, SPELL_SHOCKWAVE_CIRCLE_1, false);
                        break;
                    case SPELL_SHOCKWAVE_CIRCLE_1:
                    case SPELL_SHOCKWAVE_CIRCLE_2:
                    case SPELL_SHOCKWAVE_CIRCLE_3:
                    case SPELL_SHOCKWAVE_CIRCLE_4:
                        caster->CastSpell(caster, m_spellType->Id + 1, false);
                        break;
                    case SPELL_SHOCKWAVE_CIRCLE_5:
                        if (caster->ToCreature() && caster->ToCreature()->GetEntry() != NPC_SHOCKWAVE_MISSLE_HC)
                        {
                            caster->RemoveAurasDueToSpell(SPELL_SHOCKWAVE_VISUAL);
                            caster->CastSpell(caster, SPELL_SHOCKWAVE_DESTRUCTION, true);
                            caster->ToCreature()->DespawnOrUnsummon(2 * IN_MILLISECONDS);
                        }
                        else if (caster->GetEntry() == NPC_SHOCKWAVE_MISSLE_HC) // endless until death
                            caster->CastSpell(caster, SPELL_SHOCKWAVE_INNER_CIRCLE, false);
                        break;
                }
            }
        }
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(PlayerSelectorPredicate());

        if (SpellInfo const* m_spellInfo = GetSpellInfo())
            if (Unit* caster = GetCaster())
                targets.remove_if(ShockwaveRangePredicate(m_spellInfo->Id, caster));
    }

    void FilterShredderTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_AUTOMATED_SHREDDER; });

        if (SpellInfo const* m_spellInfo = GetSpellInfo())
            if (Unit* caster = GetCaster())
                targets.remove_if(ShockwaveRangePredicate(m_spellInfo->Id, caster));
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_soo_shockwave_missle::HandleAfterCast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_shockwave_missle::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_shockwave_missle::FilterShredderTargets, EFFECT_2, TARGET_UNIT_DEST_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_shockwave_missle::FilterTargets, EFFECT_3, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Electromagnetic Barrier 146663
class spell_soo_electromagnetic_barrier : public AuraScript
{
    PrepareAuraScript(spell_soo_electromagnetic_barrier);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetOwner()->ToUnit())
            caster->CastSpell(caster, SPELL_ELECTROMAGNETIC_BARRIER, true);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_soo_electromagnetic_barrier::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Electromagnetic Barrier Effect 145154
class spell_soo_electromagnetic_barrier_eff : public SpellScript
{
    PrepareSpellScript(spell_soo_electromagnetic_barrier_eff);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_DISASSEMBLED_CRAWLER_MINES && target->GetEntry() != NPC_DIACTIVATED_ELECTROMAGNET && target->GetEntry() != NPC_DIACTIVATED_LASER_TURRET && target->GetEntry() != NPC_DIACTIVATED_MISSLE_TURRET; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->IsAlive(); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_electromagnetic_barrier_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_electromagnetic_barrier_eff::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Shockwave Missle 143641
class spell_soo_shockwave_missle_throw : public SpellScript
{
    PrepareSpellScript(spell_soo_shockwave_missle_throw);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(PlayerSelectorPredicate());
    }

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_shockwave_missle_throw::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
        OnEffectHit += SpellEffectFn(spell_soo_shockwave_missle_throw::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// AreaTrigger 9179
class AreaTrigger_at_soo_conveyor : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_conveyor() : AreaTriggerScript("AreaTrigger_at_soo_conveyor") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->RemoveAurasDueToSpell(SPELL_CRAWLER_MINE_FIXATE_AURA);

            if (!player->HasAura(SPELL_ON_CONVEYOR))
                player->CastSpell(player, SPELL_ON_CONVEYOR, true);

            return false;
        }
};

// Break in Period 145269
class spell_soo_crawler_mine_break_in_period : public AuraScript
{
    PrepareAuraScript(spell_soo_crawler_mine_break_in_period);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* m_owner = GetOwner()->ToCreature())
            m_owner->CastSpell(m_owner, SPELL_CRAWLER_MINE_READY_TO_GO, false);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_soo_crawler_mine_break_in_period::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ready to Go! 145580
class spell_soo_crawler_mine_ready_to_go : public AuraScript
{
    PrepareAuraScript(spell_soo_crawler_mine_ready_to_go);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->CastSpell(owner, SPELL_CRAWLER_MINE_IMMUNITY, true);

            // Apply full mask
            owner->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
            owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
            owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
            owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
            owner->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_soo_crawler_mine_ready_to_go::OnAuraEffectApply, EFFECT_1, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Launch Sawblade 143265
class spell_soo_launch_sawblade : public SpellScript
{
    PrepareSpellScript(spell_soo_launch_sawblade);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
        {
            if (Vehicle* veh = GetCaster()->GetVehicleKit())
            {
                if (Unit* passenger=veh->GetPassenger(0))
                {
                    passenger->ExitVehicle();
                    passenger->CastSpell(target->GetPositionX(), target->GetPositionY(), GetCaster()->GetPositionZ() + 2.0f, SPELL_LAUNCH_SAWBLADE_AURA, false);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_soo_launch_sawblade::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Shredder Overcharge 145444
class spell_soo_shredder_overcharge : public SpellScript
{
    PrepareSpellScript(spell_soo_shredder_overcharge);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(PlayerSelectorPredicate());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soo_shredder_overcharge::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Launch Sawblade Jump 143266
class spell_siegecrafter_launch_sawblade_jump : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_launch_sawblade_jump);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->SetData(TYPE_LAUNCH_SAWBLADE, 1);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_siegecrafter_launch_sawblade_jump::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// Laser Turret Selector 143833
class spell_siegecrafter_laser_turret_selector : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_laser_turret_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_LASER_TURRET_TARGETING_BUNNY, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // not tanks and victim affected by conveyor
        targets.remove_if(MeeleSpecTargetSelector());
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && (target->ToUnit()->HasAura(SPELL_ON_CONVEYOR) || target->ToUnit()->HasAura(SPELL_LOCKED_ON)); });

        if (!targets.empty())
        {
            if (targets.size() > 1)
                Trinity::Containers::RandomResizeList(targets, 1);

            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_ON_CONVEYOR); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_laser_turret_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_laser_turret_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Energized Defensive Matrix 148990
class spell_siegecrafter_energized_defensive_matrix : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_energized_defensive_matrix);

    std::list<WorldObject*> m_targets;

    void HandleOnMatrixHit(SpellEffIndex effIdx)
    {
        // Not apply this aura if any weapon on conveyor was destroyed
        if (Creature* helper = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->GetInstanceScript() ? GetCaster()->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
            if (helper->AI()->GetData(TYPE_CONVEYOR_WEAPON_DESTROYED) || GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_LFR) && !GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
                PreventHitAura();
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            target->GetMotionMaster()->MoveJump(conveyorPull, 25.0f, 25.0f);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && !target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_energized_defensive_matrix::HandleOnMatrixHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_energized_defensive_matrix::HandleOnEffectHit, EFFECT_1, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_energized_defensive_matrix::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Crawler Mine Controller Periodic 143995
class spell_siegecrafter_crawler_mine_controller_periodic : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_crawler_mine_controller_periodic);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (Vehicle* base = owner->GetVehicleKit())
            {
                for (uint8 i = 0; i < 10; i++)
                {
                    if (Unit* passenger = base->GetPassenger(i))
                    {
                        if (passenger->ToCreature())
                            passenger->ToCreature()->AI()->DoAction(ACTION_SIEGECRAFTER_MINE_LAUNCH);

                        break;
                    }
                }
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_siegecrafter_crawler_mine_controller_periodic::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// Magnetic Pulse Selector 143357
class spell_siegecrafter_magnetic_pulse_selector : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_magnetic_pulse_selector);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_MAGNETIC_PULSE, true);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SAWBLADE; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->AI()->GetData(TYPE_LAUNCH_SAWBLADE); });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_magnetic_pulse_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_magnetic_pulse_selector::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Magnetic Pulse Past Eff 143945
class spell_siegecrafter_magnetic_pulse_past_eff : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_magnetic_pulse_past_eff);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (Creature* magnet = ObjectAccessor::GetCreature(*owner, owner->AI()->GetGUID()))
                if (magnet->HasAura(SPELL_OVERCHARGED_WEAPON_VISUAL) && !magnet->AI()->GetData(TYPE_NOT_IN_CONVEYOR_LINE))
                    return;

            owner->DespawnOrUnsummon();
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_siegecrafter_magnetic_pulse_past_eff::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Crawler Mine Selector 149148
class spell_siegecrafter_crawler_mine_selector : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_crawler_mine_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIdx].BasePoints, false);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (Creature* caster = GetCaster()->ToCreature()) // so, we have issue that Channeled spells interrupted by any control taken, but it`s wrong for mostly fixates.
        {
            if (Unit* fixatedTarget = ObjectAccessor::GetUnit(*caster, caster->AI()->GetGUID()))
            {
                if (fixatedTarget->IsAlive() && !fixatedTarget->HasAura(SPELL_ON_CONVEYOR))
                {
                    targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->GetGUID() != fixatedTarget->GetGUID(); });
                }
                else
                {
                    m_targets = targets;
                    targets.remove_if(TankSpecTargetSelector());
                    targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR) || target->ToPlayer()->HasAura(SPELL_CRAWLER_MINE_FIXATE_AURA)); });

                    if (targets.empty())
                    {
                        targets.clear();

                        for (auto&& itr : m_targets)
                            targets.push_back(itr);

                        targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR); });

                        if (targets.size() > 1)
                            Trinity::Containers::RandomResizeList(targets, 1);

                        return;
                    }

                    if (targets.size() > 1)
                        Trinity::Containers::RandomResizeList(targets, 1);
                }
            }
            else
            {
                m_targets = targets;
                targets.remove_if(TankSpecTargetSelector());
                targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && (target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR) || target->ToPlayer()->HasAura(SPELL_CRAWLER_MINE_FIXATE_AURA)); });

                if (targets.empty())
                {
                    targets.clear();

                    for (auto&& itr : m_targets)
                        targets.push_back(itr);

                    targets.remove_if([=](WorldObject* target) { return target && target->ToPlayer() && target->ToPlayer()->HasAura(SPELL_ON_CONVEYOR); });

                    if (targets.size() > 1)
                        Trinity::Containers::RandomResizeList(targets, 1);

                    return;
                }

                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_crawler_mine_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_crawler_mine_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Crawler Mine Fixate 149147
class spell_siegecrafter_crawler_mine_fixate : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_crawler_mine_fixate);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetOwner()->ToUnit())
            {
                caster->AI()->SetGUID(target->GetGUID());
                caster->AI()->Talk(0, target);
            }
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetCaster() && GetCaster()->ToCreature())
            GetCaster()->ToCreature()->AI()->DoAction(ACTION_CRAWLER_MINE_REFIXATE);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_siegecrafter_crawler_mine_fixate::OnAuraEffectApply, EFFECT_0, SPELL_AURA_FIXATE_TARGET, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_siegecrafter_crawler_mine_fixate::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_FIXATE_TARGET, AURA_EFFECT_HANDLE_REAL);
    }
};

// Detonate 143002
class spell_siegecrafter_crawler_mine_detonate : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_crawler_mine_detonate);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
    }

    void SelectShredderTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_AUTOMATED_SHREDDER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_crawler_mine_detonate::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_crawler_mine_detonate::SelectShredderTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Pipe Transfert platform to underhold (script for creatures only!) 145238
class spell_siegecrafter_pipe_transfert_platform_to_underhold : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_pipe_transfert_platform_to_underhold);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            uint8 posIdx = urand(0, 2);
            owner->CastSpell(platformToUnderhold[posIdx].GetPositionX(), platformToUnderhold[posIdx].GetPositionY(), platformToUnderhold[posIdx].GetPositionZ(), SPELL_PIPE_TRANSFERT_MAIN_TELEPORT, true);
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            float additionalValue = owner->GetEntry() == NPC_SHANNA_SPARKFIZZ ? 8.0f : 0.0f;
            owner->CastSpell(pipelineTransfertPos[0].GetPositionX() + frand(-15.0f, 15.0f), pipelineTransfertPos[0].GetPositionY() + frand(-15.0f, 15.0f), pipelineTransfertPos[0].GetPositionZ() + additionalValue, SPELL_PIPE_TRANSFERT_JUMP, true);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_siegecrafter_pipe_transfert_platform_to_underhold::OnAuraEffectApply, EFFECT_1, SPELL_AURA_SAFE_FALL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_siegecrafter_pipe_transfert_platform_to_underhold::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_SAFE_FALL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Deathdealer Laser Targeting 146372
class spell_siegecrafter_deathdealer_laser_targeting : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_deathdealer_laser_targeting);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_DEATHDEALER, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (targets.size() > 6)
            Trinity::Containers::RandomResizeList(targets, 6);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_deathdealer_laser_targeting::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_deathdealer_laser_targeting::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// On Conveyor 144287
class spell_siegecrafter_on_conveyor : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_on_conveyor);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (owner->GetPositionZ() < -305.12f) // remove if not in conveyor
            {
                owner->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                return;
            }

            GetPositionWithDistInOrientation(owner, 200.0f, 5.283260f, x, y); // pull player toward depend of his pos at static ori
            forcePos = { x, y, owner->GetPositionZ(), 0.0f };
            owner->SendApplyMovementForce(true, forcePos, 5.0f);
        }
    }

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Player* owner = GetOwner()->ToPlayer())
        {
            if (owner->HasForcedMovement())
                owner->SendApplyMovementForce(false, forcePos);
        }
    }

    private:
        Position forcePos;
        float x, y;

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_siegecrafter_on_conveyor::OnUpdate, EFFECT_0, SPELL_AURA_INTERFERE_TARGETTING);
        OnEffectRemove += AuraEffectRemoveFn(spell_siegecrafter_on_conveyor::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_INTERFERE_TARGETTING, AURA_EFFECT_HANDLE_REAL);
    }
};

// Conveyor Death Beam Heroic 149016
class spell_siegecrafter_conveyor_death_beam_hc : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_conveyor_death_beam_hc);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            if (owner->GetInstanceScript() && owner->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) != IN_PROGRESS)
                return;

            owner->CastSpell(owner, SPELL_CONVEYOR_DEATH_BEAM_AURA, true);
            owner->CastSpell(owner, SPELL_CONVEYOR_DEATH_DEAM, true);

            // Launch next left if possible
            {
                owner->SetFacingTo(Position::NormalizeOrientation(owner->GetOrientation() + M_PI / 2));

                std::list<Creature*> bunnyList;
                GetCreatureListWithEntryInGrid(bunnyList, owner, owner->GetEntry(), 30.0f);
                bunnyList.remove_if([=](Creature* target) { return target && !owner->HasInArc(M_PI / 6, target); });
                bunnyList.remove_if([=](Creature* target) { return target->GetGUID() == owner->GetGUID(); });
                bunnyList.sort(Trinity::ObjectDistanceOrderPred(owner, true));

                if (bunnyList.empty()) // select first
                {
                    owner->SetFacingTo(Position::NormalizeOrientation(owner->GetOrientation() - M_PI));
                    bunnyList.clear();
                    GetCreatureListWithEntryInGrid(bunnyList, owner, owner->GetEntry(), 30.0f);
                    bunnyList.remove_if([=](Creature* target) { return target && !owner->HasInArc(M_PI / 6, target); });
                    bunnyList.remove_if([=](Creature* target) { return target->GetGUID() == owner->GetGUID(); });
                    bunnyList.sort(Trinity::ObjectDistanceOrderPred(owner, false));

                    if (!bunnyList.empty())
                    {
                        if (Creature* farthestBunny = *bunnyList.begin())
                            farthestBunny->CastSpell(farthestBunny, GetSpellInfo()->Id, true);
                    }

                    owner->SetFacingTo(Position::NormalizeOrientation(owner->GetOrientation() + M_PI / 2));
                }
                else
                {
                    if (Creature* nearBunny = *bunnyList.begin())
                        nearBunny->CastSpell(nearBunny, GetSpellInfo()->Id, true);

                    owner->SetFacingTo(Position::NormalizeOrientation(owner->GetOrientation() - M_PI / 2));
                }
            }
        }
    }

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->RemoveAurasDueToSpell(SPELL_CONVEYOR_DEATH_BEAM_AURA);
            owner->RemoveAllAreasTrigger();
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_siegecrafter_conveyor_death_beam_hc::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_siegecrafter_conveyor_death_beam_hc::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Overcharge Weapon 145774
class spell_siegecrafter_overcharge_weapon : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_overcharge_weapon);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* target = GetHitCreature())
        {
            uint32 replaceEntry = invOverchargedWeaponType.find(target->GetEntry())->second;
            CreatureTemplate const* normalInfo = sObjectMgr->GetCreatureTemplate(replaceEntry);

            if (replaceEntry != NPC_OVERCHARGED_CRAWLER_MINE)
            {
                //target->SetName(normalInfo->Name);

                if (normalInfo)
                    target->SetDisplayId(normalInfo->GetModelByIdx(0)->CreatureDisplayID);
            }

            // Speical Announce about overcharged weapon
            if (Creature* mySiegecrafter = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_BLACKFUSE_1) : ObjectGuid::Empty))
            {
                switch (replaceEntry)
                {
                    case NPC_OVERCHARGED_LASER_TURRET:
                        mySiegecrafter->AI()->Talk(TALK_OVERLOAD_LASER);
                        break;
                    case NPC_OVERCHARGED_ELECTROMAGNET:
                        mySiegecrafter->AI()->Talk(TALK_OVERLOAD_ELECTROMAGNET);
                        break;
                    case NPC_OVERCHARGED_MISSLE_TURRET:
                        mySiegecrafter->AI()->Talk(TALK_OVERLOAD_SHOCKWAVE_MISSLE_TURRET);
                        break;
                    case NPC_OVERCHARGED_CRAWLER_MINE:
                        mySiegecrafter->AI()->Talk(TALK_OVERLOAD_CRAWLER_MINES);
                        break;
                }
            }


            target->CastSpell(target, SPELL_OVERCHARGED_WEAPON_VISUAL, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_overcharge_weapon::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Magnetic Crush Push Saw 147380
class spell_siegecrafter_magnetic_crush_push_saw : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_magnetic_crush_push_saw);

    std::list<WorldObject*> m_targets;

    bool Load() override
    {
        useCount = 0;
        step = 0.0f;
        return true;
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* target = GetHitCreature())
        {
            if (!target->AI()->GetGUID())
                target->AI()->SetGUID(GetCaster()->GetGUID());

            uint32 tempCount = target->AI()->GetData(TYPE_SAW_TRAJ) == 100 ? useCount : target->AI()->GetData(TYPE_SAW_TRAJ);
            // Calculate Arc position depend of this will form traj in next time at pull
            float angle = Position::NormalizeOrientation(sawformPos.GetOrientation() + tempCount * step);
            x = sawformPos.GetPositionX() + (12.5f * cos(angle));
            y = sawformPos.GetPositionY() + (12.5f * sin(angle));

            if (target->AI()->GetData(TYPE_SAW_TRAJ) == 100)
            {
                target->AI()->SetData(TYPE_SAW_TRAJ, useCount); // save it for correct traj
                useCount++;
            }

            target->CastSpell(x, y, -309.326f + 2.0f, SPELL_MAGNETIC_PULL_JUMP, true);
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SAWBLADE; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->AI()->GetData(TYPE_LAUNCH_SAWBLADE); });

        // Calculate Step
        if (targets.size() > 1)
            step = M_PI / (targets.size() - 1);
        else if (targets.size() > 0)
            step = M_PI / targets.size();

        m_targets = targets; // calculate last using for traj
        m_targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_SAW_TRAJ) == 100; });
        useCount = m_targets.size();
    }

    private:
        uint32 useCount;
        float step;
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_magnetic_crush_push_saw::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_magnetic_crush_push_saw::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Magnetic Crush Pull Saw 147458
class spell_siegecrafter_magnetic_crush_pull_saw : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_magnetic_crush_pull_saw);

    std::list<WorldObject*> m_targets;

    bool Load() override
    {
        useCount = 0;
        step = 0.0f;
        return true;
    }

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Creature* target = GetHitCreature())
        {
            if (!target->AI()->GetGUID())
                target->AI()->SetGUID(GetCaster()->GetGUID());

            // Calculate Pull Pos
            if (Creature* helper = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
            {
                uint32 tempCount = target->AI()->GetData(TYPE_SAW_TRAJ) == 100 ? useCount : target->AI()->GetData(TYPE_SAW_TRAJ);
                // Calculate Arc position depend of this will form traj in next time at pull
                float angle = Position::NormalizeOrientation(sawformPos.GetOrientation() + tempCount * step);
                x = helper->GetPositionX() + (55.0f * cos(angle));
                y = helper->GetPositionY() + (55.0f * sin(angle));

                if (target->AI()->GetData(TYPE_SAW_TRAJ) == 100)
                {
                    target->AI()->SetData(TYPE_SAW_TRAJ, useCount); // save it for correct traj
                    useCount++;
                }

                target->CastSpell(x, y, -309.326f + 2.0f, SPELL_MAGNETIC_PULL_JUMP, true);
            }

        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_SAWBLADE; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && !target->ToCreature()->AI()->GetData(TYPE_LAUNCH_SAWBLADE); });

        // Calculate Step
        if (targets.size() > 0)
            step = M_PI / targets.size();

        m_targets = targets; // calculate last using for traj
        m_targets.remove_if([=](WorldObject* target) { return target && target->ToCreature() && target->ToCreature()->AI()->GetData(TYPE_SAW_TRAJ) == 100; });
        useCount = m_targets.size();
    }

    private:
        uint32 useCount;
        float step;
        float x, y;

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_magnetic_crush_pull_saw::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siegecrafter_magnetic_crush_pull_saw::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Magnetic Crush Invert 147370, 147369
class spell_siegecrafter_magnetic_crush_invert : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_magnetic_crush_invert);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        SetDuration(5100); // it set 4500, but triggering eff 5000
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_siegecrafter_magnetic_crush_invert::OnAuraEffectApply, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Disintegration Laser 143867
class spell_siegecrafter_disintegration_laser : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_disintegration_laser);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetOwner() && GetOwner()->ToCreature() && GetOwner()->ToCreature()->GetEntry() == NPC_ACTIVATED_LASER_TURRET && !GetOwner()->ToCreature()->AI()->GetData(TYPE_NOT_IN_CONVEYOR_LINE) && GetCaster())
            GetCaster()->CastSpell(GetOwner()->ToCreature(), GetSpellInfo()->Id, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_siegecrafter_disintegration_laser::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Superheated 144040
class spell_siegecrafter_superheated_periodic : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_superheated_periodic);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (GetOwner() && GetOwner()->ToUnit() && (GetOwner()->ToUnit()->HasAuraType(SPELL_AURA_DEFLECT_SPELLS) || GetOwner()->ToUnit()->HasAura(31224))) // hack for deterance and cloak of shadow (using byself)
            PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_siegecrafter_superheated_periodic::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Pattern Recognition 144236
class spell_siegecrafter_pattern_recognition : public SpellScript
{
    PrepareSpellScript(spell_siegecrafter_pattern_recognition);

    void HandleOnJumpInPipe(SpellEffIndex effIdx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
            PreventHitAura();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_siegecrafter_pattern_recognition::HandleOnJumpInPipe, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Electrostatic Charge 143385
class spell_siegecrafter_electrostatic_charge : public AuraScript
{
    PrepareAuraScript(spell_siegecrafter_electrostatic_charge);

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Player* owner = GetOwner()->ToPlayer()) // hackfix for dancing rune weapon
        {
            if (owner->GetTalentSpecialization() != SPEC_DEATH_KNIGHT_BLOOD && owner->GetTalentSpecialization() != SPEC_PALADIN_PROTECTION && owner->GetTalentSpecialization() != SPEC_MONK_BREWMASTER)
                return;

            std::list<TempSummon*> runicWeapons;
            owner->GetSummons(runicWeapons, NPC_DANCING_RUNIC_WEAPON);
            owner->GetSummons(runicWeapons, NPC_MONK_XUEN);
            owner->GetSummons(runicWeapons, NPC_GUARDIAN_OF_ANCIENT_KINGS);

            for (auto&& itr : runicWeapons)
                if (!itr->HasAura(GetSpellInfo()->Id))
                    if (Aura* staticCharge = itr->AddAura(GetSpellInfo()->Id, itr))
                        staticCharge->SetStackAmount(GetStackAmount());
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_siegecrafter_electrostatic_charge::OnPeriodic, EFFECT_3, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// AreaTriggers 9240, 9241, 9189, 9236, 9500
class AreaTrigger_at_soo_pipelines_to_conveyor : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_pipelines_to_conveyor() : AreaTriggerScript("AreaTrigger_at_soo_pipelines_to_conveyor") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            // Prevent conveyor jump if encounter not started
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) != IN_PROGRESS)
            {
                // Pull back
                if (!player->HasAura(SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM))
                    player->CastSpell(player, SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM, true);

                return true;
            }

            if (player->HasAura(SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM) || player->HasAura(SPELL_PIPE_TRANSFERT_PLATFORM_TO_CONVEYOR))
                return true;

            // if player has pattern - pull back to platform
            if (player->HasAura(SPELL_PATTERN_RECOGNITION) && !player->HasAura(SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM))
            {
                player->CastSpell(player, SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM, true);
                return true;
            }

            if (!player->HasAura(SPELL_PIPE_TRANSFERT_PLATFORM_TO_CONVEYOR))
                player->CastSpell(player, SPELL_PIPE_TRANSFERT_PLATFORM_TO_CONVEYOR, true);

            return false;
        }
};

// AreaTriggers 9238, 9239, 9194, 9216
class AreaTrigger_at_soo_pipelines_from_conveyor : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_pipelines_from_conveyor() : AreaTriggerScript("AreaTrigger_at_soo_pipelines_from_conveyor") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (!player->HasAura(SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM))
                player->CastSpell(player, SPELL_PIPE_TRANSFERT_CONVEYOR_TO_PLATFORM, true);

            return false;
        }
};

// AreaTrigger 9225, + Deadlines of conveyor 9259, 9258
class AreaTrigger_at_soo_pipelines_down : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_pipelines_down() : AreaTriggerScript("AreaTrigger_at_soo_pipelines_down") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            player->Kill(player);

            // Teleport corpse to center of area for rebirth possibility
            if (Creature* helper = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_SIEGECRAFTER_HELPER) : ObjectGuid::Empty))
                player->NearTeleportTo(helper->GetPositionX(), helper->GetPositionY(), helper->GetPositionZ(), helper->GetOrientation());

            return false;
        }
};

// AreaTrigger 9251, 9252, 9250
class AreaTrigger_at_soo_pipelines_from_underhold_to_platform : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_pipelines_from_underhold_to_platform() : AreaTriggerScript("AreaTrigger_at_soo_pipelines_from_underhold_to_platform") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetInstanceScript() && (player->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == IN_PROGRESS || player->GetInstanceScript()->GetData(DATA_SIEGECRAFTER_PRE_EVENT) != DONE))
            {
                // Pull back
                if (!player->HasAura(SPELL_PIPE_TRANSFERT_PLATFORM_TO_UNDERHOLD))
                    player->CastSpell(player, SPELL_PIPE_TRANSFERT_PLATFORM_TO_UNDERHOLD, true);

                return true;
            }

            if (!player->HasAura(SPELL_PIPE_TRANSFERT_UNDERHOLD_TO_PLATFORM))
                player->CastSpell(player, SPELL_PIPE_TRANSFERT_UNDERHOLD_TO_PLATFORM, true);

            return false;
        }
};

// AreaTrigger 9371, 9253, 9257, 9254, 9255, 9256
class AreaTrigger_at_soo_pipelines_from_platform_to_underhold : public AreaTriggerScript
{
    public:
        AreaTrigger_at_soo_pipelines_from_platform_to_underhold() : AreaTriggerScript("AreaTrigger_at_soo_pipelines_from_platform_to_underhold") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetInstanceScript() && player->GetInstanceScript()->GetBossState(DATA_SIEGECRAFTER_BLACKFUSE) == IN_PROGRESS)
            {
                // pull back
                if (!player->HasAura(SPELL_PIPE_TRANSFERT_UNDERHOLD_TO_PLATFORM))
                    player->CastSpell(player, SPELL_PIPE_TRANSFERT_UNDERHOLD_TO_PLATFORM, true);

                return true;
            }

            if (!player->HasAura(SPELL_PIPE_TRANSFERT_PLATFORM_TO_UNDERHOLD))
                player->CastSpell(player, SPELL_PIPE_TRANSFERT_PLATFORM_TO_UNDERHOLD, true);

            return false;
        }
};

// 1038, 1286. Summoned by 143487, 147369, 147370 - Magnetic Crush
class sat_magnetic_crush : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAura(SPELL_ON_CONVEYOR);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            itr->CastSpell(itr, SPELL_MAGNETIC_CRUSH_PERIODIC_DAMAGE, true);

            if (Unit* caster = GetCaster())
            {
                forcePos = { caster->GetPositionX(), caster->GetPositionY(), itr->GetPositionZ(), 0.0f };
                itr->SendApplyMovementForce(true, forcePos, GetSpellInfo()->Id == SPELL_MAGNETIC_CRUSH_PULL ? -2.5f : 2.5f);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            itr->RemoveAurasDueToSpell(SPELL_MAGNETIC_CRUSH_PERIODIC_DAMAGE);

            if (itr->HasForcedMovement())
                itr->SendApplyMovementForce(false, forcePos);
        }
    }

    private:
        Position forcePos;
};

// 1057. Summoned by 144212 - Automatic Repair Beam
class sat_siegecrafter_automatic_repair_beam : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToCreature() && triggering->ToCreature()->GetEntry() == NPC_AUTOMATED_SHREDDER;
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_AUTOMATIC_REPAIR_BEAM, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_AUTOMATIC_REPAIR_BEAM);
    }
};

// 1021. Summoned by 143329 - Launch Sawblade
class sat_siegecrafter_launch_sawblade : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToCreature() && triggering->ToCreature()->GetEntry() == NPC_AUTOMATED_SHREDDER || triggering->ToPlayer() && !triggering->ToPlayer()->HasAura(SPELL_DEMONIC_GATEWAY_TRANSITION)) && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_LAUNCH_SAWBLADE);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->CastSpell(itr, SPELL_SERRATED_SLASH_SHREDDERS, true);
        else if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
            {
                // Pull towards (not random, cuz it triggered multi-triggering
                if (caster->ToCreature() && !caster->ToCreature()->AI()->GetGUID()) // not break movement
                    caster->SetFacingTo(caster->GetAngle(itr));

                float x = caster->GetPositionX() + 4.0f * std::cos(itr->GetAngle(caster));
                float y = caster->GetPositionY() + 4.0f * std::sin(itr->GetAngle(caster));
                itr->CastSpell(x, y, itr->GetPositionZ(), SPELL_SERRATED_SLASH_PLAYERS, true);
            }
        }
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Creature* itr = triggering->ToCreature())
            itr->RemoveAurasDueToSpell(SPELL_SERRATED_SLASH_SHREDDERS);
    }
};

// 1059. Summoned by 144282 - Conveyor Death Beam
class sat_siegecrafter_conveyor_death_beam : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && !triggering->ToPlayer()->HasAuraType(SPELL_AURA_DEFLECT_SPELLS);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_MATTER_PURIFICATION_BEAM_EFF, true);
    }
};

// 1046. Summoned by 143830 - Laser Ground Eff
class sat_siegecrafter_laser_ground_eff : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToPlayer() && !triggering->ToPlayer()->HasAura(SPELL_DEMONIC_GATEWAY_TRANSITION) && triggering->ToPlayer()->IsAlive() || triggering->ToCreature() && triggering->ToCreature()->GetEntry() == NPC_AUTOMATED_SHREDDER);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* itr = triggering->ToUnit())
            itr->CastSpell(itr, SPELL_SUPERHEATED, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Unit* itr = triggering->ToUnit())
            itr->RemoveAurasDueToSpell(SPELL_SUPERHEATED);
    }
};

// 1359. Summoned by 149147 - Crawler Mine Fixate
class sat_siegecrafter_crawler_mine_fixate : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->ToCreature() && !GetCaster()->ToCreature()->AI()->GetData(TYPE_CRAWLER_MINE_DETONATE) && triggering->ToPlayer()->GetGUID() == GetCaster()->ToCreature()->AI()->GetGUID() && GetCaster()->ToCreature()->AI()->GetData(TYPE_ALLOW_DETONATE_CRAWLER_MINE);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* itr = triggering->ToUnit())
        {
            if (Creature* caster = GetCaster()->ToCreature())
            {
                caster->AI()->Talk(TALK_CRAWLER_MINE_DETONATE, itr);
                caster->AI()->SetData(TYPE_CRAWLER_MINE_DETONATE, 1);
                caster->PrepareChanneledCast(caster->GetOrientation());
                caster->CastSpell(caster, SPELL_CRAWLER_MINE_DETONATE, false);
                caster->AI()->DoAction(ACTION_CRAWLER_MINE_DISSAPEAR);
            }
        }
    }
};

// SceneId 224
class scene_blackfuse_underhold_to_platform : public SceneScript
{
    public:
        scene_blackfuse_underhold_to_platform() : SceneScript("scene_blackfuse_underhold_to_platform") { }

        void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            uint8 posIdx = urand(0, 5);
            player->CastSpell(underholdToPlatform[posIdx].GetPositionX(), underholdToPlatform[posIdx].GetPositionY(), underholdToPlatform[posIdx].GetPositionZ(), SPELL_PIPE_TRANSFERT_MAIN_TELEPORT, true);

            player->m_Events.Schedule(2000, [=]()
            {
                player->CastSpell(pipelineTransfertPos[1].GetPositionX(), pipelineTransfertPos[1].GetPositionY(), pipelineTransfertPos[1].GetPositionZ(), SPELL_PIPE_TRANSFERT_JUMP, true);
            });
        }
};

// SceneId 223
class scene_blackfuse_platform_to_underhold : public SceneScript
{
    public:
        scene_blackfuse_platform_to_underhold() : SceneScript("scene_blackfuse_platform_to_underhold") { }

        void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            uint8 posIdx = urand(0, 2);
            player->CastSpell(platformToUnderhold[posIdx].GetPositionX(), platformToUnderhold[posIdx].GetPositionY(), platformToUnderhold[posIdx].GetPositionZ(), SPELL_PIPE_TRANSFERT_MAIN_TELEPORT, true);

            player->m_Events.Schedule(2000, [=]()
            {
                player->CastSpell(pipelineTransfertPos[0].GetPositionX(), pipelineTransfertPos[0].GetPositionY(), pipelineTransfertPos[0].GetPositionZ(), SPELL_PIPE_TRANSFERT_JUMP, true);
            });
        }
};

// SceneId 208
class scene_blackfuse_platform_to_conveyor : public SceneScript
{
    public:
        scene_blackfuse_platform_to_conveyor() : SceneScript("scene_blackfuse_platform_to_conveyor") { }

        void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            player->m_Events.Schedule(2000, [=]()
            {
                player->CastSpell(pipelineTransfertPos[2].GetPositionX(), pipelineTransfertPos[2].GetPositionY(), pipelineTransfertPos[2].GetPositionZ(), SPELL_PIPE_TRANSFER_CONVEYOR_TELEPORT, true);
            });
        }
};

// SceneId 210
class scene_blackfuse_conveyor_to_platform : public SceneScript
{
    public:
        scene_blackfuse_conveyor_to_platform() : SceneScript("scene_blackfuse_conveyor_to_platform") { }

        void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            uint8 posIdx = urand(0, 4);
            player->CastSpell(conveyorToPlatform[posIdx].GetPositionX(), conveyorToPlatform[posIdx].GetPositionY(), conveyorToPlatform[posIdx].GetPositionZ(), SPELL_PIPE_TRANSFERT_MAIN_TELEPORT, true);

            player->m_Events.Schedule(2000, [=]()
            {
                player->CastSpell(pipelineTransfertPos[3].GetPositionX(), pipelineTransfertPos[3].GetPositionY(), pipelineTransfertPos[3].GetPositionZ(), SPELL_PIPE_TRANSFERT_JUMP, true);
            });
        }
};

void AddSC_siegecrafter_blackfuse()
{
    new boss_siegecrafter_blackfuse();
    new creature_script<npc_automated_shredder>("npc_automated_shredder");
    new creature_script<npc_siegecrafter_sawblade>("npc_siegecrafter_sawblade");
    new creature_script<npc_soo_siegecrafter_helper>("npc_soo_siegecrafter_helper");
    new creature_script<npc_soo_deactivated_weapons>("npc_soo_deactivated_weapons");
    new creature_script<npc_soo_activated_weapons>("npc_soo_activated_weapons");
    new creature_script<npc_soo_activated_weapons_type>("npc_soo_activated_weapons_type");
    new creature_script<npc_soo_laser_target_bunny>("npc_soo_laser_target_bunny");
    new creature_script<npc_soo_laser_array_bunny>("npc_soo_laser_array_bunny");
    new creature_script<npc_soo_shockwave_missle>("npc_soo_shockwave_missle");
    new creature_script<npc_siegecrafter_shockwave_missle_trigger>("npc_siegecrafter_shockwave_missle_trigger");

    new spell_script<spell_soo_shockwave_missle>("spell_soo_shockwave_missle");
    new aura_script<spell_soo_electromagnetic_barrier>("spell_soo_electromagnetic_barrier");
    new spell_script<spell_soo_electromagnetic_barrier_eff>("spell_soo_electromagnetic_barrier_eff");
    new spell_script<spell_soo_shockwave_missle_throw>("spell_soo_shockwave_missle_throw");
    new aura_script<spell_soo_crawler_mine_break_in_period>("spell_soo_crawler_mine_break_in_period");
    new aura_script<spell_soo_crawler_mine_ready_to_go>("spell_soo_crawler_mine_ready_to_go");
    new spell_script<spell_soo_launch_sawblade>("spell_soo_launch_sawblade");
    new spell_script<spell_soo_shredder_overcharge>("spell_soo_shredder_overcharge");
    new spell_script<spell_siegecrafter_launch_sawblade_jump>("spell_siegecrafter_launch_sawblade_jump");
    new spell_script<spell_siegecrafter_laser_turret_selector>("spell_siegecrafter_laser_turret_selector");
    new spell_script<spell_siegecrafter_energized_defensive_matrix>("spell_siegecrafter_energized_defensive_matrix");
    new aura_script<spell_siegecrafter_crawler_mine_controller_periodic>("spell_siegecrafter_crawler_mine_controller_periodic");
    new spell_script<spell_siegecrafter_magnetic_pulse_selector>("spell_siegecrafter_magnetic_pulse_selector");
    new aura_script<spell_siegecrafter_magnetic_pulse_past_eff>("spell_siegecrafter_magnetic_pulse_past_eff");
    new aura_script<spell_siegecrafter_on_conveyor>("spell_siegecrafter_on_conveyor");
    new spell_script<spell_siegecrafter_crawler_mine_selector>("spell_siegecrafter_crawler_mine_selector");
    new aura_script<spell_siegecrafter_crawler_mine_fixate>("spell_siegecrafter_crawler_mine_fixate");
    new spell_script<spell_siegecrafter_crawler_mine_detonate>("spell_siegecrafter_crawler_mine_detonate");
    new aura_script<spell_siegecrafter_pipe_transfert_platform_to_underhold>("spell_siegecrafter_pipe_transfert_platform_to_underhold");
    new spell_script<spell_siegecrafter_deathdealer_laser_targeting>("spell_siegecrafter_deathdealer_laser_targeting");
    new aura_script<spell_siegecrafter_conveyor_death_beam_hc>("spell_siegecrafter_conveyor_death_beam_hc");
    new spell_script<spell_siegecrafter_overcharge_weapon>("spell_siegecrafter_overcharge_weapon");
    new spell_script<spell_siegecrafter_magnetic_crush_push_saw>("spell_siegecrafter_magnetic_crush_push_saw");
    new spell_script<spell_siegecrafter_magnetic_crush_pull_saw>("spell_siegecrafter_magnetic_crush_pull_saw");
    new aura_script<spell_siegecrafter_magnetic_crush_invert>("spell_siegecrafter_magnetic_crush_invert");
    new aura_script<spell_siegecrafter_disintegration_laser>("spell_siegecrafter_disintegration_laser");
    new aura_script<spell_siegecrafter_superheated_periodic>("spell_siegecrafter_superheated_periodic");
    new spell_script<spell_siegecrafter_pattern_recognition>("spell_siegecrafter_pattern_recognition");
    new aura_script<spell_siegecrafter_electrostatic_charge>("spell_siegecrafter_electrostatic_charge");
    new AreaTrigger_at_soo_conveyor();
    new AreaTrigger_at_soo_pipelines_to_conveyor();
    new AreaTrigger_at_soo_pipelines_from_conveyor();
    new AreaTrigger_at_soo_pipelines_down();
    new AreaTrigger_at_soo_pipelines_from_underhold_to_platform();
    new AreaTrigger_at_soo_pipelines_from_platform_to_underhold();
    new atrigger_script<sat_magnetic_crush>("sat_magnetic_crush");
    new atrigger_script<sat_siegecrafter_automatic_repair_beam>("sat_siegecrafter_automatic_repair_beam");
    new atrigger_script<sat_siegecrafter_launch_sawblade>("sat_siegecrafter_launch_sawblade");
    new atrigger_script<sat_siegecrafter_conveyor_death_beam>("sat_siegecrafter_conveyor_death_beam");
    new atrigger_script<sat_siegecrafter_laser_ground_eff>("sat_siegecrafter_laser_ground_eff");
    new atrigger_script<sat_siegecrafter_crawler_mine_fixate>("sat_siegecrafter_crawler_mine_fixate");
    new scene_blackfuse_underhold_to_platform();
    new scene_blackfuse_platform_to_underhold();
    new scene_blackfuse_platform_to_conveyor();
    new scene_blackfuse_conveyor_to_platform();
};
