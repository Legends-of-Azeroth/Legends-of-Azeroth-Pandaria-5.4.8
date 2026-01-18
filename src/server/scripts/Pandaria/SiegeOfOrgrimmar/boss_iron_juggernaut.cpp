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

enum Spells
{
    /*Main Phase*/
    SPELL_SCATTER_LASER_SELECTOR     = 144458,
    SPELL_SCATTER_LASER_EFF          = 144459,
    SPELL_LASER_STRIKE               = 144456,
    SPELL_SCATTER_LASER_PRECAST      = 144460,
    SPELL_MORTAR_BLAST_EFF           = 144316,
    SPELL_MORTAR_BLAST               = 146027,
    SPELL_MORTAR_BLAST_SELECTOR      = 144315,
    SPELL_MORTAR_BLAST_PRECAST       = 145407,
    SPELL_FLAME_VENTS                = 144464,
    SPELL_IGNITE_ARMOR               = 144467,
    SPELL_CRAWLER_MINE_SELECTOR      = 144673,
    SPELL_CRAWLER_MINE_SUMM          = 144705,
    SPELL_DETONATION_SEQ             = 144718,
    SPELL_CRAWLER_MINE_BLAST         = 144766,
    SPELL_GROUND_POUND               = 144776,
    SPELL_ENGULFED_EXPLOSION         = 144791,
    SPELL_BORER_DRILL                = 144209,
    SPELL_BORER_DRILL_EFF            = 144218,
    SPELL_BORER_DRILL_AT             = 144220,
    SPELL_BORER_DRILL_MAIN_AT        = 144221, // areatrigger that triggering 144220 AT
    SPELL_BORER_DRILL_UNK            = 144296,
    SPELL_BORER_DRILL_ANN            = 144296,
                                     
    /*Siege Phase*/
    SPELL_DEMOLISHER_CANNONS_EFF     = 144154,
    SPELL_DEMOLISHER_CANNONS         = 144153,
    SPELL_DEMOLISHER_CANNON_SELECTOR = 144198,
    SPELL_DEMOLISHER_CANNON_PROC     = 144199,
    SPELL_SIESMIC_ACTIVITY           = 144483,
    SPELL_SIESMIC_ACTIVITY_EFF       = 144484,
    SPELL_SHOCK_PULSE                = 144485,
    SPELL_EXPLOSIVE_TAR_EFF          = 144498,
    SPELL_EXPLOSIVE_TAR_PROC         = 144492,
    SPELL_EXPLOSIVE_TAR_MISSLE       = 144496,
    SPELL_EXPLOSIVE_TAR_SUMM         = 144497,
    SPELL_EXPLOSIVE_TAR_AT           = 144525,
    SPELL_EXPLOSIVE_TAR_VISUAL       = 146191,
    SPELL_TAR_EXPLOSION              = 144919,
    SPELL_SIEGE_CUTTER_LASER_EFF     = 144918,
    SPELL_SIEGE_CUTTER_LASER_FIXATE  = 146325,
    SPELL_SIEGE_CUTTER_LASER_AT      = 144576,
    SPELL_SIEGE_CUTTER_LASER         = 144573,
    SPELL_SIEGE_CUTTER_LASER_PRECAST = 145190,
                                     
    // Misc                          
    SPELL_MELEE                      = 149356,
    SPELL_BERSERK                    = 26662,
    SPELL_ZERO_POWER                 = 72242,
    SPELL_FREEZE_ANIM                = 16245, // for sawblade at encounter done
    SPELL_SUPERHEATED_CRAWLER_MINE   = 146342,

    // Heroic
    SPELL_RICOCHET_EFF               = 144327,
    SPELL_RICOCHET_AT                = 144375,
    SPELL_RICOCHET_APPEAR_AT         = 144356,
    SPELL_RICOCHET_TRANSFORM         = 149501, // seems like this don`t need... it scale us
    SPELL_RICOCHET_REMOVE            = 149502,
    SPELL_MORTAR_BARRAGE             = 144555,
    SPELL_MORTAR_BARRAGE_PERIODIC    = 144554,
    SPELL_MORTAR_BARRAGE_MISSLE      = 144553,
    SPELL_MORTAR_BARRAGE_EFF         = 144556,
};

enum Events
{
    EVENT_SCATTER_LASTER = 1,
    EVENT_MORTAR_STRIKE,
    EVENT_FLAME_VENTS,
    EVENT_CRAWLER_MINE,
    EVENT_BOREL_DRILL,
    EVENT_BERSERK,
    EVENT_GAIN_POWER,
    EVENT_DISCHARGE_POWER,
    EVENT_SIEGE_MODE,
    EVENT_SHOCK_PULSE,
    EVENT_EXPLOSION_TAR,
    EVENT_CUTTER_LASER,
    EVENT_DEMOLISHER_CANNONS,
    EVENT_CUTTER_LASER_INIT,
    EVENT_ASSSAULT_MODE,
    EVENT_LAUNCH_SAWBLADE,
    EVENT_MORTAR_BARRAGE,
};

enum Yells
{
    TALK_SIEGE_MODE_ANN,
    TALK_ASSAULT_MODE_ANN,
};

std::map<uint32, uint32> invSiegeWeaponsType =
{
    { NPC_IJ_TOP_CANNON, 0 },
    { NPC_IJ_SAWBLADE,   1 },
    { NPC_IJ_CANNON_2,   2 },
    { NPC_IJ_CANNON_1,   3 },
    { NPC_IJ_TAIL_GUN,   4 },
};

class boss_iron_juggernaut : public CreatureScript
{
    public:
        boss_iron_juggernaut() : CreatureScript("boss_iron_juggernaut") { }

        struct boss_iron_juggernautAI : public BossAI
        {
            boss_iron_juggernautAI(Creature* creature) : BossAI(creature, DATA_IRON_JUGGERNAUT) 
            {
                me->setActive(true);
                DoCast(me, SPELL_MELEE);
            }

            uint32 energyGainTick;
            ObjectGuid targetGUID;
            uint32 crawlerCount; // 3 per phase
            uint32 superheatedMineStomped;
            EventMap berserkerEvents;
            bool hasFirstLaser;

            void Reset() override
            {
                _Reset();

                events.Reset();
                berserkerEvents.Reset();

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    for (auto&& itr : invSiegeWeaponsType)
                        if (Creature* siegeWeapon = me->SummonCreature(itr.first, *me))
                            siegeWeapon->EnterVehicle(me, itr.second);

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                });

                me->SetAnimTier(AnimTier::Ground);
                me->SetReactState(REACT_DEFENSIVE);
                energyGainTick = 0;
                crawlerCount   = 0;
                superheatedMineStomped = 0;
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, 100);
                me->SetPower(POWER_ENERGY, 0);
                hasFirstLaser = true;

                DoCast(me, SPELL_ZERO_POWER);
                me->GetMap()->SetWorldState(WORLDSTATE_FIRE_IN_THE_HALL, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveAurasDueToSpell(SPELL_MELEE);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_VOLJIN)))
                        voljin->AI()->DoAction(ACTION_IRON_JUGGERNAUT_COMBAT_INIT);
                }

                events.ScheduleEvent(EVENT_SCATTER_LASTER, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_MORTAR_STRIKE, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FLAME_VENTS, 6.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CRAWLER_MINE, 30 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BOREL_DRILL, 17 * IN_MILLISECONDS);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_LAUNCH_SAWBLADE, 15 * IN_MILLISECONDS);

                uint32 berserkTimer = IsHeroic() ? 7.5 * MINUTE * IN_MILLISECONDS : 10 * MINUTE * IN_MILLISECONDS;
                berserkerEvents.ScheduleEvent(EVENT_BERSERK, berserkTimer);
                berserkerEvents.ScheduleEvent(EVENT_GAIN_POWER, 1 * IN_MILLISECONDS);
                berserkerEvents.ScheduleEvent(EVENT_DEMOLISHER_CANNONS, 10 * IN_MILLISECONDS);

                SetPassengersInCombat();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (!IsSiegeWeapon(summon->GetEntry()))
                    summon->SetInCombatWithZone();
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_IRON_JUGGERNAUT, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCATTER_LASER_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IGNITE_ARMOR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SIEGE_CUTTER_LASER_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BORER_DRILL_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPLOSIVE_TAR_EFF);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();
                }

                berserkerEvents.Reset();
                summons.DespawnAll();

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_NON_ATTACKABLE);
                _DespawnAtEvade();
                me->GetMap()->CreatureRelocation(me, ironJugernaunRelocate.GetPositionX(), ironJugernaunRelocate.GetPositionY(), ironJugernaunRelocate.GetPositionZ(), ironJugernaunRelocate.GetOrientation());
            }

            void JustReachedHome() override
            {
                _JustReachedHome();

                if (instance)
                    instance->SetBossState(DATA_GENERAL_NAZGRIM, FAIL);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SCATTER_LASER_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IGNITE_ARMOR);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_SIEGE_CUTTER_LASER_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BORER_DRILL_EFF);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_EXPLOSIVE_TAR_EFF);
                    instance->DoRemoveBloodLustDebuffSpellOnPlayers();

                    if (Creature* voljin = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_VOLJIN)))
                        voljin->AI()->DoAction(ACTION_IRON_JUGGERNAUT_DEFEAT);

                    instance->SetData(DATA_IRON_JUGGERNAUT_PAST_EVENT, DONE);
                }

                berserkerEvents.Reset();
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_SUPERHEATED_MINE_STOMP && ++superheatedMineStomped > 5)
                    me->GetMap()->SetWorldState(WORLDSTATE_FIRE_IN_THE_HALL, 1);
            }

            void UpdateAI(uint32 diff) override
            {
                scheduler.Update(diff);

                if (!UpdateVictim())
                    return;

                events.Update(diff);
                berserkerEvents.Update(diff);

                while (uint32 eventId = berserkerEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GAIN_POWER:
                        {
                            if (energyGainTick > 3)
                                energyGainTick = 0;

                            uint32 delay = ++energyGainTick > 3 ? 2 * IN_MILLISECONDS : 1 * IN_MILLISECONDS; // each four tick it has delay 2s instead 1

                            // approximate to timer
                            if (me->GetPower(POWER_ENERGY) > 58)
                                delay = 1 * IN_MILLISECONDS;

                            if (me->GetPower(POWER_ENERGY) + 1 > 99)
                            {
                                me->SetPower(POWER_ENERGY, 100);
                                events.ScheduleEvent(EVENT_SIEGE_MODE, 0.5 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) + 1);

                            berserkerEvents.ScheduleEvent(EVENT_GAIN_POWER, delay);
                            break;
                        }
                        case EVENT_DISCHARGE_POWER:
                        {
                            if (energyGainTick > 2)
                                energyGainTick = 0;

                            uint32 given = ++energyGainTick > 2 ? 1 : 2; // each third tick it has lost 1 energy instead 2

                            if (me->GetPower(POWER_ENERGY) - given <= 0)
                            {
                                me->SetPower(POWER_ENERGY, 0);
                                events.ScheduleEvent(EVENT_ASSSAULT_MODE, 0.5 * IN_MILLISECONDS);
                                break;
                            }
                            else
                                me->SetPower(POWER_ENERGY, me->GetPower(POWER_ENERGY) - given); // 1

                            berserkerEvents.ScheduleEvent(EVENT_DISCHARGE_POWER, 1 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            break;
                        case EVENT_DEMOLISHER_CANNONS:
                        {
                            uint8 passengerID = urand(0, 1) ? 2 : 3;
                            if (me->GetVehicleKit() && me->GetVehicleKit()->GetPassenger(passengerID))
                                me->GetVehicleKit()->GetPassenger(passengerID)->CastSpell(me->GetVehicleKit()->GetPassenger(passengerID), SPELL_DEMOLISHER_CANNON_PROC, true);

                            berserkerEvents.ScheduleEvent(EVENT_DEMOLISHER_CANNONS, 11 * IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_CUTTER_LASER:
                            DoCast(me, SPELL_SIEGE_CUTTER_LASER_PRECAST, true);
                            berserkerEvents.ScheduleEvent(EVENT_CUTTER_LASER, hasFirstLaser ? 46 * IN_MILLISECONDS : 11 * IN_MILLISECONDS);
                            hasFirstLaser = false;
                            break;
                        case EVENT_MORTAR_BARRAGE:
                            DoCast(me, SPELL_MORTAR_BARRAGE, true);
                            berserkerEvents.ScheduleEvent(EVENT_MORTAR_BARRAGE, 30 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SCATTER_LASTER:
                            DoCast(me, SPELL_SCATTER_LASER_PRECAST);
                            events.ScheduleEvent(EVENT_SCATTER_LASTER, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_MORTAR_STRIKE:
                            DoCast(me, SPELL_MORTAR_BLAST_PRECAST);
                            events.ScheduleEvent(EVENT_MORTAR_STRIKE, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_FLAME_VENTS:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FLAME_VENTS);

                            events.ScheduleEvent(EVENT_FLAME_VENTS, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                        case EVENT_BOREL_DRILL:
                            DoCast(me, SPELL_BORER_DRILL_ANN);
                            DoCast(me, SPELL_BORER_DRILL);
                            events.ScheduleEvent(EVENT_BOREL_DRILL, 17 * IN_MILLISECONDS);
                            break;
                        case EVENT_CRAWLER_MINE:
                            if (!me->HasAura(SPELL_SIESMIC_ACTIVITY) && ++crawlerCount > 3)
                                break;

                            DoCast(me, SPELL_CRAWLER_MINE_SELECTOR);
                            events.ScheduleEvent(EVENT_CRAWLER_MINE, me->HasAura(SPELL_SIESMIC_ACTIVITY) ? 21 * IN_MILLISECONDS : 30 * IN_MILLISECONDS);
                            break;
                        case EVENT_SIEGE_MODE:
                            Talk(TALK_SIEGE_MODE_ANN);
                            events.Reset();
                            energyGainTick = 0;
                            crawlerCount   = 0;
                            hasFirstLaser = true;
                            
                            if (Unit* target = me->GetVictim())
                                targetGUID = target->GetGUID();

                            me->PrepareChanneledCast(me->GetOrientation());
                            me->SetAnimTier(AnimTier::Fly); // fly tier for borer to ground.. interesting
                            DoCast(me, SPELL_SIESMIC_ACTIVITY);

                            events.ScheduleEvent(EVENT_SHOCK_PULSE, 18 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_EXPLOSION_TAR, 8 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_CRAWLER_MINE, 28 * IN_MILLISECONDS);
                            berserkerEvents.ScheduleEvent(EVENT_CUTTER_LASER, 16 * IN_MILLISECONDS);
                            berserkerEvents.ScheduleEvent(EVENT_DISCHARGE_POWER, 3 * IN_MILLISECONDS);

                            if (IsHeroic())
                                berserkerEvents.ScheduleEvent(EVENT_MORTAR_BARRAGE, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_SHOCK_PULSE:
                            DoCast(me, SPELL_SHOCK_PULSE);
                            events.ScheduleEvent(EVENT_SHOCK_PULSE, 16 * IN_MILLISECONDS);
                            break;
                        case EVENT_EXPLOSION_TAR:
                            DoCast(me, SPELL_EXPLOSIVE_TAR_PROC);
                            events.ScheduleEvent(EVENT_EXPLOSION_TAR, 39 * IN_MILLISECONDS);
                            break;
                        case EVENT_ASSSAULT_MODE:
                            Talk(TALK_ASSAULT_MODE_ANN);
                            events.Reset();
                            energyGainTick = 0;
                            berserkerEvents.CancelEvent(EVENT_CUTTER_LASER);
                            berserkerEvents.CancelEvent(EVENT_MORTAR_BARRAGE);
                            me->RemoveAurasDueToSpell(SPELL_SIESMIC_ACTIVITY);
                            me->SetAnimTier(AnimTier::Ground);
                            me->RemoveChanneledCast(targetGUID);
                            events.ScheduleEvent(EVENT_SCATTER_LASTER, 10 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_MORTAR_STRIKE, 15 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_FLAME_VENTS, 6.5 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_CRAWLER_MINE, 30 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_BOREL_DRILL, 17 * IN_MILLISECONDS);

                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_LAUNCH_SAWBLADE, 15 * IN_MILLISECONDS);

                            berserkerEvents.ScheduleEvent(EVENT_GAIN_POWER, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_LAUNCH_SAWBLADE:
                            DoCast(me, SPELL_RICOCHET_APPEAR_AT);
                            events.ScheduleEvent(EVENT_LAUNCH_SAWBLADE, 15 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff);
            }

            private:
                bool IsSiegeWeapon(uint32 entry)
                {
                    for (auto&& itr : invSiegeWeaponsType)
                        if (entry == itr.first)
                            return true;
                    
                    return false;
                }

                void SetPassengersInCombat()
                {
                    std::list<TempSummon*> siegeWeaponsList;

                    for (auto&& itr : invSiegeWeaponsType)
                        me->GetSummons(siegeWeaponsList, itr.first);

                    for (auto&& itr : siegeWeaponsList)
                        itr->SetInCombatWithZone();
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_iron_juggernautAI(creature);
        }
};

// Crawler Mine 72050
struct npc_crawler_mine : public ScriptedAI
{
    npc_crawler_mine(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid ownerGUID;
    ObjectGuid targetGUID;
    ObjectGuid explosionGUID;
    bool hasPassenger;

    void IsSummonedBy(Unit* summoner) override
    {
        ownerGUID  = summoner->GetGUID();
        targetGUID = ObjectGuid::Empty;
        explosionGUID = ObjectGuid::Empty;
        hasPassenger = false;

        if (Creature* juggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
            juggernaut->AI()->JustSummoned(me);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            me->SetSpeed(MOVE_RUN, 2.75f);

            if (Unit* target = ObjectAccessor::GetUnit(*me, ownerGUID))
                me->GetMotionMaster()->MovePoint(0, *target);
            else if (Creature* juggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                if (Unit* target = juggernaut->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0.0f, 200.0f, true))
                    me->GetMotionMaster()->MovePoint(0, *target);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->SetFaction(35);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                DoCast(me, SPELL_CRAWLER_MINE_SUMM);
                DoCast(me, SPELL_DETONATION_SEQ, true);
                me->HandleEmoteStateCommand(EMOTE_STATE_SUBMERGED);
            });
        });
    }

    void JustSummoned(Creature* summon) override
    {
        explosionGUID = summon->GetGUID();
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_TAR_EXPLOSION && !me->HasAura(SPELL_SUPERHEATED_CRAWLER_MINE))
            DoCast(me, SPELL_SUPERHEATED_CRAWLER_MINE, true);
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return explosionGUID;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasPassenger)
            return;

        hasPassenger = true;
        targetGUID = clicker->GetGUID();
        clicker->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->InterruptNonMeleeSpells(false, SPELL_DETONATION_SEQ);

        scheduler
            .Schedule(Milliseconds(750), [this](TaskContext context)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
            {
                target->ExitVehicle();
                me->RemoveAurasDueToSpell(SPELL_DETONATION_SEQ);
            }
        });

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                target->CastSpell(target, SPELL_ENGULFED_EXPLOSION, true);

            // Achiev
            if (me->HasAura(SPELL_SUPERHEATED_CRAWLER_MINE))
            {
                if (Creature* ironJuggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                    ironJuggernaut->AI()->SetData(TYPE_SUPERHEATED_MINE_STOMP, 1);
            }

            me->DespawnOrUnsummon(0.5 * IN_MILLISECONDS);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Crawler Mine Clicker 72060
struct npc_juggernaut_crawler_mine_clicker : public ScriptedAI
{
    npc_juggernaut_crawler_mine_clicker(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* summoner) override 
    {
        if (Creature* juggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
            juggernaut->AI()->JustSummoned(me);

        me->SetDisplayFromModel(0);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Borer Drill 71906
struct npc_borer_drill : public ScriptedAI
{
    npc_borer_drill(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid ownerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        ownerGUID = summoner->GetGUID();

        if (Creature* juggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
            juggernaut->AI()->JustSummoned(me);

        me->SetSpeed(MOVE_WALK, 0.64f);
        me->SetSpeed(MOVE_RUN, 0.64f);
    }

    void Reset() override
    {
        me->SetDisplayFromModel(0);
        DoCast(me, SPELL_BORER_DRILL_MAIN_AT, true);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            if (Unit* target = ObjectAccessor::GetUnit(*me, ownerGUID))
            {
                if (target->IsAlive())
                    me->GetMotionMaster()->MoveFollow(target, 0.0f, me->GetAngle(target));
                else if (Creature* owner = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT):ObjectGuid::Empty))
                {
                    // Select new target by owner
                    if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                    {
                        ownerGUID = newTarget->GetGUID();
                        me->GetMotionMaster()->MoveFollow(newTarget, 0.0f, me->GetAngle(newTarget));
                    }
                    else if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                    {
                        ownerGUID = newTarget->GetGUID();
                        me->GetMotionMaster()->MoveFollow(newTarget, 0.0f, me->GetAngle(newTarget));
                    }
                }
            }

            context.Repeat(Milliseconds(500));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Explosive Tar 71950
struct npc_juggernaut_explosive_tar : public ScriptedAI
{
    npc_juggernaut_explosive_tar(Creature* creature) : ScriptedAI(creature) { }

    bool hasDestroyed;

    void Reset() override
    {
        me->SetDisplayFromModel(0);
        DoCast(me, SPELL_EXPLOSIVE_TAR_AT, true);
        DoCast(me, SPELL_EXPLOSIVE_TAR_VISUAL, true);
        hasDestroyed = false;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_TAR_EXPLOSION && !hasDestroyed)
        {
            hasDestroyed = true;
            DoCast(me, SPELL_TAR_EXPLOSION);
            me->DespawnOrUnsummon(1 * IN_MILLISECONDS);
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Cutter Laser 72026
struct npc_juggernaut_cutter_laser : public ScriptedAI
{
    npc_juggernaut_cutter_laser(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    uint32 laserActive;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetDisplayFromModel(0);

        if (Creature* juggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
            juggernaut->AI()->JustSummoned(me);

        laserActive = 0;
        summonerGUID = summoner->GetGUID();
        me->PrepareChanneledCast(me->GetOrientation());

        me->SetSpeed(MOVE_WALK, 0.64f);
        me->SetSpeed(MOVE_RUN, 0.64f);

        if (Unit* owner = ObjectAccessor::GetUnit(*me, summonerGUID))
        {
            DoCast(owner, SPELL_SIEGE_CUTTER_LASER_FIXATE);
            me->ClearUnitState(UNIT_STATE_CASTING);
        }

        events.ScheduleEvent(EVENT_CUTTER_LASER_INIT, 2 * IN_MILLISECONDS);
        me->DespawnOrUnsummon(10 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        damage = 0;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_CUTTER_LASER)
            return laserActive;

        return 0;
    }

    void UpdateAI(uint32 diff) override 
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_CUTTER_LASER_INIT)
            {
                laserActive = 1;

                if (Unit* target = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    if (target->IsAlive())
                        me->GetMotionMaster()->MoveFollow(target, 0.0f, me->GetAngle(target));
                    else if (Creature* owner = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                    {
                        // Select new target by owner
                        if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, CasterSpecTargetSelector()))
                        {
                            DoCast(newTarget, SPELL_SIEGE_CUTTER_LASER_FIXATE);
                            summonerGUID = newTarget->GetGUID();
                            me->GetMotionMaster()->MoveFollow(newTarget, 0.0f, me->GetAngle(newTarget));
                        }
                        else if (Unit* newTarget = owner->AI()->SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, true))
                        {
                            DoCast(newTarget, SPELL_SIEGE_CUTTER_LASER_FIXATE);
                            summonerGUID = newTarget->GetGUID();
                            me->GetMotionMaster()->MoveFollow(newTarget, 0.0f, me->GetAngle(newTarget));
                        }
                    }
                }

                events.ScheduleEvent(EVENT_CUTTER_LASER_INIT, 0.5 * IN_MILLISECONDS);
            }
            break;
        }
    }
};

// Sawblade 71469
struct npc_juggernaut_sawblade : public ScriptedAI
{
    npc_juggernaut_sawblade(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid firstTargetGUID;
    ObjectGuid secondTargetGUID;
    bool hasLaunched;

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            firstTargetGUID = ObjectGuid::Empty;
            secondTargetGUID = ObjectGuid::Empty;
            hasLaunched = false;

            if (Creature* ironJuggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
            {
                me->ExitVehicle();

                float z = ironJuggernaut->GetPositionZ();
                me->UpdateAllowedPositionZ(me->GetPositionX(), me->GetPositionY(), z);
                me->GetMotionMaster()->MoveJump(me->GetPositionX(), me->GetPositionY(), z, 25.0f, 25.0f, EVENT_JUMP);
            }
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == TYPE_ALLOW_SAWBLADE_EFF)
            return hasLaunched ? 1 : 0;

        return 0;
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (pointId == EVENT_JUMP)
        {
            me->SetHomePosition(*me);

            scheduler
                .Schedule(Milliseconds(500), [this](TaskContext context)
            {
                hasLaunched = true;
                me->StopMoving();
                me->SetWalk(false);
                me->GetMotionMaster()->Clear();

                // so, first we should select farthest target on our path, next - any near, but too farthest - in this way we`ll get a loop model.
                // Generate path on init
                std::list<Player*> targets, newTargets;
                GetPlayerListInGrid(targets, me, 150.0f); // no selector spell for this

                if (targets.empty()) // just return to hand
                {
                    hasLaunched = false;

                    if (Creature* ironJuggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                        ironJuggernaut->CastSpell(ironJuggernaut, SPELL_RICOCHET_REMOVE, true);

                    return;
                }

                targets.sort(Trinity::ObjectDistanceOrderPred(me, false));
                firstTargetGUID = targets.front()->GetGUID();

                if (Player* target = ObjectAccessor::GetPlayer(*me, firstTargetGUID))
                {
                    GetPlayerListInGrid(newTargets, target, 30.0f); // no selector spell for this
                    newTargets.remove_if([=](Player* target) { return target && target->GetGUID() == firstTargetGUID; }); // remove old target from selector

                    if (!newTargets.empty())
                        secondTargetGUID = newTargets.front()->GetGUID();

                    Movement::MoveSplineInit init(me);
                    init.Path().push_back(G3D::Vector3(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ()));

                    float z = target->GetPositionZ();
                    me->UpdateAllowedPositionZ(target->GetPositionX(), target->GetPositionY(), z);
                    init.Path().push_back(G3D::Vector3(target->GetPositionX(), target->GetPositionY(), z));

                    if (Player* secondTarget = ObjectAccessor::GetPlayer(*me, secondTargetGUID))
                    {
                        z = secondTarget->GetPositionZ();
                        me->UpdateAllowedPositionZ(secondTarget->GetPositionX(), secondTarget->GetPositionY(), z);
                        init.Path().push_back(G3D::Vector3(secondTarget->GetPositionX(), secondTarget->GetPositionY(), z));

                        init.Launch();

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->MoveTargetedHome();

                            scheduler
                                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                            {
                                hasLaunched = false;

                                if (Creature* ironJuggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                                    ironJuggernaut->CastSpell(ironJuggernaut, SPELL_RICOCHET_REMOVE, true);
                            });
                        });
                    }
                    else // go home if not found second target
                    {
                        init.Launch();

                        scheduler
                            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                        {
                            me->StopMoving();
                            me->GetMotionMaster()->MoveTargetedHome();

                            scheduler
                                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                            {
                                hasLaunched = false;

                                if (Creature* ironJuggernaut = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                                    ironJuggernaut->CastSpell(ironJuggernaut, SPELL_RICOCHET_REMOVE, true);
                            });
                        });
                    }
                }
            });
        }
    }

    void UpdateAI(uint32 diff) override 
    { 
        scheduler.Update(diff);
    }
};

// Scatter Laser 144460
class spell_juggernaut_scatter_laser_init : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_scatter_laser_init);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IJ_TAIL_GUN; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_scatter_laser_init::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Laser Strike 144456
class spell_juggernaut_laser_strike : public AuraScript
{
    PrepareAuraScript(spell_juggernaut_laser_strike);

    void OnApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetMap()->Is25ManRaid())
            SetDuration(1500); // 3 ticks in 25 mode

        // Calculate Ticks for Flex Difficulty
        if (GetOwner()->ToUnit() && GetOwner()->ToUnit()->GetInstanceScript() && GetOwner()->ToUnit()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate Duration
            int32 tickAmount = 500; // min 500, max 1500
            uint32 totalPlayersCount = GetOwner()->ToUnit()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 15)
                tickAmount += 500;

            if (totalPlayersCount > 20)
                tickAmount += 500;

            SetDuration(tickAmount);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_juggernaut_laser_strike::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Scatter Laser Selector 144458
class spell_juggernaut_scatter_laser_selector : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_scatter_laser_selector);

    std::list<WorldObject*> m_targets, copyTargets;

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector()); // Try not select targets affected by scatter laser if it possible
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_SCATTER_LASER_EFF); });

        if (targets.size() > 1)
        {
            Trinity::Containers::RandomResizeList(targets, 1);
            copyTargets = targets;
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);

        copyTargets = targets;
    }

    void CopyTargets(std::list<WorldObject*>&targets)
    {
        targets = copyTargets;
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_scatter_laser_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_scatter_laser_selector::CopyTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Mortar Blast Selector 144315
class spell_juggernaut_mortar_blast_selector : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_mortar_blast_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.size() > 1)
        {
            Trinity::Containers::RandomResizeList(targets, 1);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_juggernaut_mortar_blast_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_mortar_blast_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Crawler Mine Selector 144673
class spell_juggernaut_crawler_mine_selector : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_crawler_mine_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                target->SummonCreature(NPC_CRAWLER_MINE, *caster, TEMPSUMMON_MANUAL_DESPAWN);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.size() > 3)
        {
            Trinity::Containers::RandomResizeList(targets, 3);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });

        if (targets.size() > 3)
            Trinity::Containers::RandomResizeList(targets, 3);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_juggernaut_crawler_mine_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_crawler_mine_selector::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Mortar Blast Init 145407
class spell_juggernaut_mortar_blast_init : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_mortar_blast_init);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IJ_TOP_CANNON; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_mortar_blast_init::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Cutter Laser Init 145190
class spell_juggernaut_cutter_laser_init : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_cutter_laser_init);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IJ_TAIL_GUN; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_cutter_laser_init::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Borer Drill Selector 144209
class spell_juggernaut_borer_drill_selector : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_borer_drill_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                target->SummonCreature(NPC_BORER_DRILL, *caster, TEMPSUMMON_TIMED_DESPAWN, 13 * IN_MILLISECONDS);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 3;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 3; // min 3, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
        {
            Trinity::Containers::RandomResizeList(targets, targetsCount);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 3; // min 3, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_juggernaut_borer_drill_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_borer_drill_selector::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Demolisher Cannons Selector 144198
class spell_juggernaut_demolisher_cannons_selector : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_demolisher_cannons_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, GetSpellInfo()->Effects[EFFECT_0].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        uint8 targetsCount = GetCaster()->GetMap()->Is25ManRaid() ? 5 : 3;

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 3; // min 3, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
        {
            Trinity::Containers::RandomResizeList(targets, targetsCount);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        if (GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetData(DATA_FLEX))
        {
            // Calculate TargetsCount
            targetsCount = 3; // min 3, max 5
            uint32 totalPlayersCount = GetCaster()->GetInstanceScript()->instance->GetPlayersCountExceptGMs();

            if (totalPlayersCount > 16)
                targetsCount++;

            if (totalPlayersCount > 21)
                targetsCount++;

            if (targets.size() > targetsCount)
                Trinity::Containers::RandomResizeList(targets, targetsCount); // should be there for flex
        }

        if (targets.size() > targetsCount)
            Trinity::Containers::RandomResizeList(targets, targetsCount);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_juggernaut_demolisher_cannons_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_demolisher_cannons_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Explosive Tar 144492 �
class spell_juggernaut_explosive_tar : public AuraScript
{
    PrepareAuraScript(spell_juggernaut_explosive_tar);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            float dist = 95.0f;

            if (Player* farthestTarget = GetFarthestPlayerInArea(owner, 200.0f))
            {
                dist = owner->GetExactDist2d(farthestTarget) + 5.0f;

                if (dist > 95.0f) // not more
                    dist = 95.0f;
                else if (dist < 25.0f) // not lower
                    dist = 25.0f;
            }

            for (uint8 i = 0; i < 5; i++)
            {
                GetNearTarPoint(owner, m_pos, dist);
                owner->CastSpell(m_pos.GetPositionX(), m_pos.GetPositionY(), m_pos.GetPositionZ(), SPELL_EXPLOSIVE_TAR_MISSLE, true);
            }
        }
    }

    private:
        Position m_pos;

        void GetNearTarPoint(Unit* caster, Position &pos, float distance) const
        {
            // angle to face `obj` to `this`
            float angle = frand(0.0f, 2 * M_PI);
            float new_dist = frand(12.0f, distance);

            float rand_x = caster->GetPositionX() + new_dist * std::cos(angle);
            float rand_y = caster->GetPositionY() + new_dist * std::sin(angle);
            float rand_z = caster->GetPositionZ();

            Trinity::NormalizeMapCoord(rand_x);
            Trinity::NormalizeMapCoord(rand_y);
            caster->UpdateGroundPositionZ(rand_x, rand_y, rand_z);
            pos.Relocate(rand_x, rand_y, rand_z, caster->GetOrientation());
        }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_juggernaut_explosive_tar::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Cutter Laser Script 144575
class spell_juggernaut_cutter_laser_script : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_cutter_laser_script);

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_SIEGE_CUTTER_LASER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_juggernaut_cutter_laser_script::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Cutter Laser Selector 144573
class spell_juggernaut_cutter_laser_selector : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_cutter_laser_selector);

    std::list<WorldObject*> m_targets;

    void HandleOnEffectHit(SpellEffIndex effIdx)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                if (Creature* beam = target->SummonCreature(NPC_CUTTER_LASER, *target, TEMPSUMMON_MANUAL_DESPAWN))
                    caster->CastSpell(beam, GetSpellInfo()->Effects[EFFECT_0].BasePoints);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        m_targets = targets;

        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });
        targets.remove_if(TankSpecTargetSelector());
        targets.remove_if(MeeleSpecTargetSelector());

        if (targets.size() > 1)
        {
            Trinity::Containers::RandomResizeList(targets, 1);
            return;
        }

        targets.clear();

        for (auto&& itr : m_targets)
            targets.push_back(itr);

        targets.remove_if([=](WorldObject* target) { return target && !target->ToPlayer(); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_juggernaut_cutter_laser_selector::HandleOnEffectHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_cutter_laser_selector::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Detonation Sequence 144718
class spell_juggernaut_detonation_sequence : public AuraScript
{
    PrepareAuraScript(spell_juggernaut_detonation_sequence);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            if (aurEff->GetBase()->GetDuration() > 0)
                return;

            if (Creature* explosionTarget = ObjectAccessor::GetCreature(*owner, owner->AI()->GetGUID()))
            {
                explosionTarget->CastSpell(explosionTarget, SPELL_CRAWLER_MINE_BLAST, true);
                explosionTarget->DespawnOrUnsummon(1 * IN_MILLISECONDS);

                owner->DespawnOrUnsummon(1 * IN_MILLISECONDS);
            }
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_juggernaut_detonation_sequence::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Explosive Tar Summon 144497
class spell_juggernaut_explosive_tar_summon : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_explosive_tar_summon);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (GetCaster() && GetCaster()->GetInstanceScript() && GetCaster()->GetInstanceScript()->GetBossState(DATA_IRON_JUGGERNAUT) != IN_PROGRESS)
            PreventHitEffect(eff_idx);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_juggernaut_explosive_tar_summon::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// Ricochet Launcher 144356
class spell_juggernaut_ricochet_launch : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_ricochet_launch);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IJ_SAWBLADE; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_ricochet_launch::SelectTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Ricochet Transform 149501
class spell_juggernaut_ricochet_transform : public AuraScript
{
    PrepareAuraScript(spell_juggernaut_ricochet_transform);

    void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->CastSpell(owner, SPELL_RICOCHET_AT, true);
            owner->SetObjectScale(0.5f);
            owner->AI()->DoAction(ACTION_START_INTRO);
        }
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
        {
            owner->SetObjectScale(1.0f);
            owner->RemoveAurasDueToSpell(SPELL_RICOCHET_AT);

            if (Creature* juggernaut = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(DATA_IRON_JUGGERNAUT) : ObjectGuid::Empty))
                owner->EnterVehicle(juggernaut, 1);
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_juggernaut_ricochet_transform::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_juggernaut_ricochet_transform::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
    }
};

// Ricochet Remove 149502
class spell_juggernaut_ricochet_remove : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_ricochet_remove);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IJ_SAWBLADE; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_ricochet_remove::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Mortar Barrage Launcher 144555
class spell_juggernaut_mortar_barrage_launcher : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_mortar_barrage_launcher);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_IJ_CANNON_1 && target->GetEntry() != NPC_IJ_CANNON_2 && target->GetEntry() != NPC_IJ_TOP_CANNON; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_mortar_barrage_launcher::SelectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// Mortar Barrage Triggering 144554
class spell_juggernaut_mortar_barrage_triggering : public AuraScript
{
    PrepareAuraScript(spell_juggernaut_mortar_barrage_triggering);

    void OnTrigger(AuraEffect const* /*aurEff*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            float dist = 65.0f;

            if (Player* farthestTarget = GetFarthestPlayerInArea(owner, 200.0f))
            {
                dist = owner->GetExactDist2d(farthestTarget) + 5.0f;

                if (dist > 65.0f) // not more
                    dist = 65.0f;
                else if (dist < 25.0f) // not lower
                    dist = 25.0f;
            }

            GetNearTarPoint(owner, m_pos, dist);
            owner->CastSpell(m_pos.GetPositionX(), m_pos.GetPositionY(), m_pos.GetPositionZ(), SPELL_MORTAR_BARRAGE_MISSLE, true);
        }
    }

    private:
        Position m_pos;

        void GetNearTarPoint(Unit* caster, Position &pos, float distance) const
        {
            // angle to face `obj` to `this`
            float angle = frand(0.0f, 2 * M_PI);
            float new_dist = frand(12.0f, distance);

            float rand_x = caster->GetPositionX() + new_dist * std::cos(angle);
            float rand_y = caster->GetPositionY() + new_dist * std::sin(angle);
            float rand_z = caster->GetPositionZ();

            Trinity::NormalizeMapCoord(rand_x);
            Trinity::NormalizeMapCoord(rand_y);
            caster->UpdateGroundPositionZ(rand_x, rand_y, rand_z);
            pos.Relocate(rand_x, rand_y, rand_z, caster->GetOrientation());
        }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_juggernaut_mortar_barrage_triggering::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Seismic Activity Eff 144484
class spell_juggernaut_seismic_activity_eff : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_seismic_activity_eff);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_seismic_activity_eff::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// Shock Pulse 144485
class spell_juggernaut_shock_pulse : public SpellScript
{
    PrepareSpellScript(spell_juggernaut_shock_pulse);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->GetTypeId() != TYPEID_PLAYER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_shock_pulse::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_juggernaut_shock_pulse::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 1110. Summoned by 144221, 144220 - Borer Drill
class sat_borer_drill : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (!itr->HasAura(SPELL_BORER_DRILL_EFF))
                itr->CastSpell(itr, SPELL_BORER_DRILL_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_BORER_DRILL_EFF);
    }
};

// 1083. Summoned by 144525 - Explosive Tar
class sat_explosive_tar : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_EXPLOSIVE_TAR_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_EXPLOSIVE_TAR_EFF);
    }
};

// 1097. Summoned by 144576 - Cutter Laser
class sat_cutter_laser : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && (triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() || triggering->ToCreature() && (triggering->ToCreature()->GetEntry() == NPC_EXPLOSIVE_TAR || triggering->ToCreature()->GetEntry() == NPC_CRAWLER_MINE))
            && GetTarget() && GetTarget()->ToCreature() && GetTarget()->ToCreature()->AI()->GetData(TYPE_CUTTER_LASER);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_SIEGE_CUTTER_LASER_EFF, true);
        }
        else if (Creature* itr = triggering->ToCreature())
            itr->AI()->DoAction(ACTION_TAR_EXPLOSION);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_SIEGE_CUTTER_LASER_EFF);
    }
};

// 1065. Summoned by 144375 - Ricochet
class sat_ricochet : public IAreaTriggerAura
{
    void OnInit() override
    {
        GetAreaTrigger()->AttachToTarget();
    }

    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() && GetCaster() && GetCaster()->ToCreature() && GetCaster()->ToCreature()->AI()->GetData(TYPE_ALLOW_SAWBLADE_EFF);
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_RICOCHET_EFF, true);
    }
};

void AddSC_iron_juggernaut()
{
    new boss_iron_juggernaut();
    new creature_script<npc_crawler_mine>("npc_crawler_mine");
    new creature_script<npc_borer_drill>("npc_borer_drill");
    new creature_script<npc_juggernaut_explosive_tar>("npc_juggernaut_explosive_tar");
    new creature_script<npc_juggernaut_cutter_laser>("npc_juggernaut_cutter_laser");
    new creature_script<npc_juggernaut_crawler_mine_clicker>("npc_juggernaut_crawler_mine_clicker");
    new creature_script<npc_juggernaut_sawblade>("npc_juggernaut_sawblade");

    new spell_script<spell_juggernaut_scatter_laser_init>("spell_juggernaut_scatter_laser_init");
    new aura_script<spell_juggernaut_laser_strike>("spell_juggernaut_laser_strike");
    new spell_script<spell_juggernaut_scatter_laser_selector>("spell_juggernaut_scatter_laser_selector");
    new spell_script<spell_juggernaut_mortar_blast_selector>("spell_juggernaut_mortar_blast_selector");
    new spell_script<spell_juggernaut_crawler_mine_selector>("spell_juggernaut_crawler_mine_selector");
    new spell_script<spell_juggernaut_mortar_blast_init>("spell_juggernaut_mortar_blast_init");
    new spell_script<spell_juggernaut_cutter_laser_init>("spell_juggernaut_cutter_laser_init");
    new spell_script<spell_juggernaut_borer_drill_selector>("spell_juggernaut_borer_drill_selector");
    new spell_script<spell_juggernaut_demolisher_cannons_selector>("spell_juggernaut_demolisher_cannons_selector");
    new aura_script<spell_juggernaut_explosive_tar>("spell_juggernaut_explosive_tar");
    new spell_script<spell_juggernaut_cutter_laser_script>("spell_juggernaut_cutter_laser_script");
    new spell_script<spell_juggernaut_cutter_laser_selector>("spell_juggernaut_cutter_laser_selector");
    new aura_script<spell_juggernaut_detonation_sequence>("spell_juggernaut_detonation_sequence");
    new spell_script<spell_juggernaut_explosive_tar_summon>("spell_juggernaut_explosive_tar_summon");
    new spell_script<spell_juggernaut_ricochet_launch>("spell_juggernaut_ricochet_launch");
    new aura_script<spell_juggernaut_ricochet_transform>("spell_juggernaut_ricochet_transform");
    new spell_script<spell_juggernaut_ricochet_remove>("spell_juggernaut_ricochet_remove");
    new spell_script<spell_juggernaut_mortar_barrage_launcher>("spell_juggernaut_mortar_barrage_launcher");
    new aura_script<spell_juggernaut_mortar_barrage_triggering>("spell_juggernaut_mortar_barrage_triggering");
    new spell_script<spell_juggernaut_seismic_activity_eff>("spell_juggernaut_seismic_activity_eff");
    new spell_script<spell_juggernaut_shock_pulse>("spell_juggernaut_shock_pulse");
    new atrigger_script<sat_borer_drill>("sat_borer_drill");
    new atrigger_script<sat_explosive_tar>("sat_explosive_tar");
    new atrigger_script<sat_cutter_laser>("sat_cutter_laser");
    new atrigger_script<sat_ricochet>("sat_ricochet");
}
