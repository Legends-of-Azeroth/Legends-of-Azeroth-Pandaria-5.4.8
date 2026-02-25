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

#include "battle_on_the_high_seas.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_RIDE_VEHICLE                    = 61421, // on transport cannon click
    SPELL_CANNON_LAUNCH                   = 141812,
    SPELL_CANNON_MISC                     = 132808,
    SPELL_CANNON_MISC_EFF                 = 132807,
    SPELL_SWASHBUCKLING                   = 132942,
    SPELL_SWASHBUCKLING_EFF               = 132941,
    SPELL_CANNON_BARRAGE_PERIODIC         = 142058, // req for achiev
    SPELL_CANNONEERS_TORCH                = 132831,
    SPELL_CANNONEERS_TORCH_EFF            = 132830,
    SPELL_SHOT                            = 132943,
    SPELL_COUNTER_SHOT                    = 142699,
    SPELL_WATER_BOLT                      = 32011,
    SPELL_CHAIN_HEAL                      = 136462,
    SPELL_ROPE_COSMETIC                   = 141148,
    SPELL_THROW_BOMB                      = 132995,
    SPELL_ACTIVATE_RANDOM_BARREL          = 141847,
    SPELL_ACTIVATE_RANDOM_BARREL_SELECTOR = 141099,
    SPELL_SUMMON_FUSE                     = 141101,
    SPELL_BOMB                            = 132991,
    SPELL_BOMB_EFF                        = 132993,
    SPELL_FUSE_VISUAL                     = 141075,
    SPELL_FUSE_VISUAL_LINK                = 141068, // by explosive barrel to fuse stalker!
    SPELL_EXPLOSIVE_BARREL                = 142262,
    SPELL_FIRE                            = 143222,
    SPELL_FIRE_EFF                        = 143219,
    SPELL_BARREL_EXPLOSION_EFF            = 141465,
    SPELL_STICKY_BOMB                     = 132998,
    SPELL_STICKY_BOMB_EFF                 = 132999,
    SPELL_VOLATILE_CONCOCTION             = 141327,
    SPELL_SUMMON_EXPLOSIVE_BARREL         = 141644,
    SPELL_GATHER_EXPLOSIVES               = 141640, // take barrel for update criteria
    SPELL_SHOTGUN_TARGETING               = 142176,
    SPELL_SHOTGUN_AMMO                    = 141389,
    SPELL_SHOTGUN                         = 141381,
    SPELL_RELOADING                       = 141391,
    SPELL_PRIMING                         = 132977,
    SPELL_VERTICAL_SLASH                  = 141187,
};

enum Events
{
    EVENT_SWASHBUCKLING = 1,
    EVENT_CANNONEER_TORCH,
    EVENT_SHOT,
    EVENT_WATER_BOLT,
    EVENT_CHAIN_HEAL,
    EVENT_THROW_BOMB,
    EVENT_ACTIVATE_BARREL,
    EVENT_STICKY_BOMB,
    EVENT_VOLATILE_CONCOCTION,
    EVENT_SHOTGUN,
    EVENT_RELOAD_SHOTGUN,
    EVENT_VERTICAL_SLASH,
    EVENT_CALL_BOMBARDMENT,
};

// Alliance Cannoneer 69194, Horde Cannoneer 67360
struct npc_high_seas_cannoneer : public customCreatureAI
{
    npc_high_seas_cannoneer(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    float x, y, z;

    void Reset() override
    {
        events.Reset();
        x = 0.0f; y = 0.0f; z = 0.0f;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->RemoveAurasDueToSpell(SPELL_CANNON_LAUNCH);
        DoCast(me, SPELL_CANNONEERS_TORCH);

        scheduler
            .Schedule(Milliseconds(7000), Milliseconds(10000), [this](TaskContext context)
        {
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, me, 60.0f);

            if (!targets.empty())
            {
                Player* target = Trinity::Containers::SelectRandomContainerElement(targets);

                if (!me->IsAlive() || !me->IsValidAttackTarget(target))
                    return;

                if (Creature* bunny = me->SummonCreature(NPC_GENERAL_BUNNY_JMF, *target, TEMPSUMMON_TIMED_DESPAWN, 8 * IN_MILLISECONDS))
                {
                    x = bunny->GetPositionX() + frand(-2.0f, 2.0f);
                    y = bunny->GetPositionY() + frand(-2.0f, 2.0f);
                    z = bunny->GetPositionZ();

                    if (Aura* targeting = bunny->AddAura(SPELL_BOMBS_AWAY_TARGETING, bunny))
                        targeting->SetDuration(5 * IN_MILLISECONDS);

                    scheduler
                        .Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        me->CastSpell(x, y, z, SPELL_BOMBS_AWAY, true);
                    });
                }
            }

            context.Repeat(Milliseconds(4800));
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(85) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

            scheduler
                .Schedule(Milliseconds(1000), [this](TaskContext context)
            {
                std::list<Creature*> transportCannons;
                GetCreatureListWithEntryInGrid(transportCannons, me, NPC_TRANSPORT_CANNON_A_H_1, 80.0f);
                GetCreatureListWithEntryInGrid(transportCannons, me, NPC_TRANSPORT_CANNON_A_H_2, 80.0f);
                GetCreatureListWithEntryInGrid(transportCannons, me, NPC_TRANSPORT_CANNON_A_H_3, 80.0f);

                if (transportCannons.empty())
                    return;

                transportCannons.sort(Trinity::ObjectDistanceOrderPred(me, true));

                if (Creature* transportCannon = transportCannons.front())
                    DoCast(transportCannon, SPELL_RIDE_VEHICLE);

                scheduler
                    .Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    if (Unit* vBase = me->GetVehicleBase())
                    {
                        me->ExitVehicle();
                        me->AddAura(SPELL_CANNON_LAUNCH, me);

                        if (vBase->GetEntry() != NPC_TRANSPORT_CANNON_A_H_2)
                            me->CastSpell(transportCannonsPos[1].GetPositionX(), transportCannonsPos[1].GetPositionY(), transportCannonsPos[1].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                        else
                            me->CastSpell(transportCannonsPos[2].GetPositionX(), transportCannonsPos[2].GetPositionY(), transportCannonsPos[2].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                    }
                });
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

        DoMeleeAttackIfReady();
    }
};

// Alliance Swashbuckler 69196, Horde Swashbuckler 67361, Lieutenant Parker 71326
struct npc_high_seas_swashbuckler : public customCreatureAI
{
    npc_high_seas_swashbuckler(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    float x, y, z;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->RemoveAurasDueToSpell(SPELL_CANNON_LAUNCH);
        events.ScheduleEvent(EVENT_SWASHBUCKLING, 4.5 * IN_MILLISECONDS);

        scheduler
            .Schedule(Milliseconds(7000), Milliseconds(10000), [this](TaskContext context)
        {
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, me, 60.0f);

            if (!targets.empty())
            {
                Player* target = Trinity::Containers::SelectRandomContainerElement(targets);

                if (!me->IsAlive() || !me->IsValidAttackTarget(target))
                    return;

                if (Creature* bunny = me->SummonCreature(NPC_GENERAL_BUNNY_JMF, *target, TEMPSUMMON_TIMED_DESPAWN, 8 * IN_MILLISECONDS))
                {
                    x = bunny->GetPositionX() + frand(-2.0f, 2.0f);
                    y = bunny->GetPositionY() + frand(-2.0f, 2.0f);
                    z = bunny->GetPositionZ();

                    if (Aura* targeting = bunny->AddAura(SPELL_BOMBS_AWAY_TARGETING, bunny))
                        targeting->SetDuration(5 * IN_MILLISECONDS);

                    scheduler
                        .Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        me->CastSpell(x, y, z, SPELL_BOMBS_AWAY, true);
                    });
                }
            }

            context.Repeat(Milliseconds(4800));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetEntry() == NPC_LIEUTENANT_PARKER && me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_BOARDING_PARTY, DONE);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(85) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);

            scheduler
                .Schedule(Milliseconds(1000), [this](TaskContext context)
            {
                std::list<Creature*> transportCannons;
                GetCreatureListWithEntryInGrid(transportCannons, me, NPC_TRANSPORT_CANNON_A_H_1, 80.0f);
                GetCreatureListWithEntryInGrid(transportCannons, me, NPC_TRANSPORT_CANNON_A_H_2, 80.0f);
                GetCreatureListWithEntryInGrid(transportCannons, me, NPC_TRANSPORT_CANNON_A_H_3, 80.0f);

                if (transportCannons.empty())
                    return;

                transportCannons.sort(Trinity::ObjectDistanceOrderPred(me, true));

                if (Creature* transportCannon = transportCannons.front())
                    DoCast(transportCannon, SPELL_RIDE_VEHICLE);

                scheduler
                    .Schedule(Milliseconds(2000), [this](TaskContext context)
                {
                    if (Unit* vBase = me->GetVehicleBase())
                    {
                        me->ExitVehicle();
                        me->AddAura(SPELL_CANNON_LAUNCH, me);

                        if (vBase->GetEntry() != NPC_TRANSPORT_CANNON_A_H_2)
                            me->CastSpell(transportCannonsPos[1].GetPositionX(), transportCannonsPos[1].GetPositionY(), transportCannonsPos[1].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                        else
                            me->CastSpell(transportCannonsPos[2].GetPositionX(), transportCannonsPos[2].GetPositionY(), transportCannonsPos[2].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                    }
                });
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
            ExecuteTargetEvent(SPELL_SWASHBUCKLING, 11.5 * IN_MILLISECONDS, EVENT_SWASHBUCKLING, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Cannon Exploder 67341
struct npc_high_seas_cannon_exploder : public ScriptedAI
{
    npc_high_seas_cannon_exploder(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler.CancelAll();

        uint32 miscDelay = urand(1000, 2500);
        scheduler
            .Schedule(Milliseconds(miscDelay), [this](TaskContext context)
        {
            DoCast(me, SPELL_CANNON_MISC_EFF);
            context.Repeat(Seconds(4));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Admiral Hagman 71436
struct npc_high_seas_admiral_hagman : public ScriptedAI
{
    npc_high_seas_admiral_hagman(Creature* creature) : ScriptedAI(creature), summons(me) 
    { 
        isFirstWave = true;
        isSecondWave = false;
    }

    SummonList summons;
    std::vector<ObjectGuid> allianceDogGUIDs;
    bool isFirstWave;
    bool isSecondWave;

    void Reset() override { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(65) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() != NPC_LIEUTENANT_PARKER)
        {
            allianceDogGUIDs.push_back(summon->GetGUID());
            summons.Summon(summon);
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (std::find(allianceDogGUIDs.begin(), allianceDogGUIDs.end(), summon->GetGUID()) != allianceDogGUIDs.end())
            allianceDogGUIDs.erase(std::find(allianceDogGUIDs.begin(), allianceDogGUIDs.end(), summon->GetGUID()));

        if (allianceDogGUIDs.empty() && isSecondWave)
        {
            isSecondWave = false;

            // Summon Lieutenant Parker
            if (Creature* parker = me->SummonCreature(NPC_LIEUTENANT_PARKER, parketSumPos, TEMPSUMMON_MANUAL_DESPAWN))
                parker->AI()->DoAction(ACTION_START_INTRO);
        }

        if (allianceDogGUIDs.empty() && isFirstWave)
        {
            isFirstWave = false;
            isSecondWave = true;

            // Summon Second Wave
            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(HORDE_ASSAULT_DATA, IN_PROGRESS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    };
};

// Transport Cannon 71331, 71364, 71365, 71367, 71167
struct npc_high_seas_transport_cannon : public ScriptedAI
{
    npc_high_seas_transport_cannon(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    bool hasClicked;
    ObjectGuid clickerGUID;

    void Reset() override
    {
        hasClicked = false;
        clickerGUID = ObjectGuid::Empty;
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        if (hasClicked)
            return;

        hasClicked = true;
        clickerGUID = clicker->GetGUID();
        clicker->CastSpell(me, SPELL_RIDE_VEHICLE, true);

        scheduler
            .Schedule(Milliseconds(2000), [this](TaskContext context)
        {
            if (Unit* oldClicker = ObjectAccessor::GetUnit(*me, clickerGUID))
            {
                oldClicker->ExitVehicle();
                oldClicker->AddAura(SPELL_CANNON_LAUNCH, oldClicker);

                switch (me->GetEntry())
                {
                    case NPC_TRANSPORT_CANNON_H_A_1:
                        oldClicker->CastSpell(transportCannonsPos[0].GetPositionX(), transportCannonsPos[0].GetPositionY(), transportCannonsPos[0].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                        break;
                    case NPC_TRANSPORT_CANNON_A_H_1:
                    case NPC_TRANSPORT_CANNON_A_H_3:
                        oldClicker->CastSpell(transportCannonsPos[1].GetPositionX(), transportCannonsPos[1].GetPositionY(), transportCannonsPos[1].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                        break;
                    case NPC_TRANSPORT_CANNON_A_H_2:
                        oldClicker->CastSpell(transportCannonsPos[2].GetPositionX(), transportCannonsPos[2].GetPositionY(), transportCannonsPos[2].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                        break;
                    case NPC_TRANSPORT_CANNON_2_H_A_1:
                        oldClicker->CastSpell(transportCannonsPos[3].GetPositionX(), transportCannonsPos[3].GetPositionY(), transportCannonsPos[3].GetPositionZ(), SPELL_CANNON_LAUNCH, true);
                        break;
                }
            }
        });

        scheduler
            .Schedule(Milliseconds(3000), [this](TaskContext context)
        {
            hasClicked = false;
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Horde Captain 71445
struct npc_high_seas_horde_captain : public customCreatureAI
{
    npc_high_seas_horde_captain(Creature* creature) : customCreatureAI(creature)
    {
        triggered = false;
        delay = 0;
        calculationCount = 13;
        yellEntry = TALK_SPECIAL_5;
    }

    EventMap events;
    TaskScheduler scheduler;
    std::vector<ObjectGuid> allianceDogGUIDs;
    bool triggered;
    uint32 delay;
    uint32 calculationCount;
    uint32 yellEntry;

    void Reset() override  { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHOT, 1.5 * IN_MILLISECONDS);
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(65) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_START_INTRO:
                delay = 9000;
                scheduler
                    .Schedule(Milliseconds(delay), [this](TaskContext context)
                {
                    Talk(TALK_INTRO);
                });

                scheduler
                    .Schedule(Milliseconds(delay += 12000), [this](TaskContext context)
                {
                    Talk(TALK_SPECIAL_1);
                });
                break;
            case ACTION_EXPLOSIVE_SHIP:
                scheduler
                    .Schedule(Milliseconds(1000), [this](TaskContext context)
                {
                    if (calculationCount)
                    {
                        calculationCount--;
                        Talk(yellEntry);
                        yellEntry++;
                        context.Repeat(Seconds(1));
                    }
                    else
                    {
                        // Explosive ship there
                        if (me->GetInstanceScript())
                            me->GetInstanceScript()->SetData(DATA_SMITHEREENS, DONE);

                        scheduler
                            .Schedule(Milliseconds(2500), [this](TaskContext context)
                        {
                            Talk(TALK_SPECIAL_18);
                        });
                    }
                });
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        allianceDogGUIDs.push_back(summon->GetGUID());
        summons.Summon(summon);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (std::find(allianceDogGUIDs.begin(), allianceDogGUIDs.end(), summon->GetGUID()) != allianceDogGUIDs.end())
            allianceDogGUIDs.erase(std::find(allianceDogGUIDs.begin(), allianceDogGUIDs.end(), summon->GetGUID()));

        if (allianceDogGUIDs.empty() && !triggered)
        {
            triggered = true;
            Talk(TALK_SPECIAL_2);

            // Allow to use Zip line
            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(DATA_EXPLOSIVES_ACQUICITION, IN_PROGRESS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_COUNTER_SHOT, 10.6 * IN_MILLISECONDS, EVENT_SHOT, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    };
};

// Horde Squalsharper 71367
struct npc_high_seas_horde_squalsharper : public customCreatureAI
{
    npc_high_seas_horde_squalsharper(Creature* creature) : customCreatureAI(creature) 
    {
        SetCombatMovement(false);
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_WATER_BOLT, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_CHAIN_HEAL, 10 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(85) && attacker && attacker->GetTypeId() != TYPEID_PLAYER)
            damage = 0;
    }

    ObjectGuid GetLowestFriendGUID() override
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ADMIRAL_HAGMAN, 30.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_HORDE_SQUALSHAPER, 30.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_HORDE_CANNONEER, 30.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_HORDE_SWASHBUCKLER, 30.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_HORDE_CAPTAIN, 30.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
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
            ExecuteTargetEvent(SPELL_WATER_BOLT, 2.6 * IN_MILLISECONDS, EVENT_WATER_BOLT, eventId);
            ExecuteTargetEvent(SPELL_CHAIN_HEAL, 14 * IN_MILLISECONDS, EVENT_CHAIN_HEAL, eventId, PRIORITY_ALLY);
            break;
        }
    }
};

// Zip 70779
struct npc_high_seas_zip : public ScriptedAI
{
    npc_high_seas_zip(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
        summoner->CastSpell(summoner, SPELL_ROPE_COSMETIC, true);
    }

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            Movement::MoveSplineInit init(me);

            for (auto&& itr : zipLinePath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                if (Unit* passenger = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    // Remove rope anim
                    std::list<Creature*> ropeList;
                    GetCreatureListWithEntryInGrid(ropeList, me, NPC_ROPE_ANCHOR, 200.0f);

                    for (auto&& itr : ropeList)
                        if (itr->HasAura(SPELL_ROPE_COSMETIC, passenger->GetGUID()))
                            itr->RemoveAurasDueToSpell(SPELL_ROPE_COSMETIC);

                    passenger->RemoveAurasDueToSpell(SPELL_ROPE_COSMETIC);
                    passenger->ExitVehicle();
                }

                me->DespawnOrUnsummon(1000);
            });
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Zip Back 67327
struct npc_high_seas_zip_back : public ScriptedAI
{
    npc_high_seas_zip_back(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        summoner->CastSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, true);
        summoner->CastSpell(summoner, SPELL_ROPE_COSMETIC, true);
    }

    void Reset() override
    {
        scheduler
            .Schedule(Milliseconds(1500), [this](TaskContext context)
        {
            Movement::MoveSplineInit init(me);

            for (auto&& itr : zipLineBackPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                if (Unit* passenger = ObjectAccessor::GetUnit(*me, summonerGUID))
                {
                    // Remove rope anim
                    std::list<Creature*> ropeList;
                    GetCreatureListWithEntryInGrid(ropeList, me, NPC_ROPE_ANCHOR, 200.0f);

                    for (auto&& itr : ropeList)
                        if (itr->HasAura(SPELL_ROPE_COSMETIC, passenger->GetGUID()))
                            itr->RemoveAurasDueToSpell(SPELL_ROPE_COSMETIC);

                    passenger->RemoveAurasDueToSpell(SPELL_ROPE_COSMETIC);
                    passenger->ExitVehicle();
                }

                me->DespawnOrUnsummon(1000);
            });
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    };
};

// Bomb 67403
struct npc_high_seas_bomb : public ScriptedAI
{
    npc_high_seas_bomb(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->PlayOneShotAnimKitId(992);
        DoCast(me, SPELL_BOMB);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Fuse 70754
struct npc_high_seas_fuse : public ScriptedAI
{
    npc_high_seas_fuse(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    ObjectGuid barrelGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        barrelGUID = ObjectGuid::Empty;
        summonerGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        DoCast(me, SPELL_FUSE_VISUAL);

        scheduler
            .Schedule(Milliseconds(500), [this](TaskContext context)
        {
            if (Creature* lieutenant = ObjectAccessor::GetCreature(*me, summonerGUID))
            {
                if (Creature* barrel = ObjectAccessor::GetCreature(*me, lieutenant->AI()->GetGUID()))
                {
                    barrelGUID = barrel->GetGUID();
                    barrel->CastSpell(me, SPELL_FUSE_VISUAL_LINK, true);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, *barrel);

                    scheduler
                        .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
                    {
                        if (Creature* oldBarrel = ObjectAccessor::GetCreature(*me, barrelGUID))
                        {
                            oldBarrel->CastSpell(oldBarrel, SPELL_BARREL_EXPLOSION_EFF, true);
                            me->RemoveAurasDueToSpell(SPELL_FUSE_VISUAL_LINK);
                            me->RemoveAurasDueToSpell(SPELL_FUSE_VISUAL);
                            oldBarrel->RemoveAurasDueToSpell(SPELL_EXPLOSIVE_BARREL);
                            oldBarrel->AI()->Reset(); // restore default model or it`ll switch to rnd
                            DoCast(me, SPELL_FIRE, true);
                        }
                    });
                }
            }
        });
    }

    void UpdateAI(uint32 diff) override 
    {
        scheduler.Update(diff);
    }
};

// Triad of Lieutenants: Fuseblast 71328, Fizzboil 71330, Boltblaster 71329
struct npc_high_seas_triad_of_lieutenants : public customCreatureAI
{
    npc_high_seas_triad_of_lieutenants(Creature* creature) : customCreatureAI(creature) { }

    ObjectGuid barrelGUID;

    void Reset() override
    {
        barrelGUID = ObjectGuid::Empty;
        events.Reset();
    }

    void SetGUID(ObjectGuid guid, int32 /*type*/) override
    {
        barrelGUID = guid;
    }

    ObjectGuid GetGUID(int32 /*type*/) const override
    {
        return barrelGUID;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_THROW_BOMB, 4 * IN_MILLISECONDS);
        
        switch (me->GetEntry())
        {
            case NPC_LIEUTENANT_FUSEBLAST:
                events.ScheduleEvent(EVENT_ACTIVATE_BARREL, 5 * IN_MILLISECONDS);
                break;
            case NPC_LIEUTENANT_FIZZBOIL:
                events.ScheduleEvent(EVENT_STICKY_BOMB, 5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_VOLATILE_CONCOCTION, 9.5 * IN_MILLISECONDS);
                break;
            case NPC_LIUTENANT_BOLTBLASTER:
                DoCast(me, SPELL_SHOTGUN_AMMO);
                events.ScheduleEvent(EVENT_SHOTGUN, 2.5 * IN_MILLISECONDS);
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCast(me, SPELL_SUMMON_EXPLOSIVE_BARREL);
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
                case EVENT_THROW_BOMB:
                    Talk(TALK_INTRO);
                    break;
                case EVENT_ACTIVATE_BARREL:
                    Talk(TALK_SPECIAL_1);
                    break;
                case EVENT_VOLATILE_CONCOCTION:
                    Talk(TALK_SPECIAL_2); // Announce
                    Talk(TALK_SPECIAL_3);
                    break;
            }

            ExecuteTargetEvent(SPELL_THROW_BOMB, 6.8 * IN_MILLISECONDS, EVENT_THROW_BOMB, eventId);
            ExecuteTargetEvent(SPELL_ACTIVATE_RANDOM_BARREL_SELECTOR, 5.5 * IN_MILLISECONDS, EVENT_ACTIVATE_BARREL, eventId);
            ExecuteTargetEvent(SPELL_STICKY_BOMB, 10 * IN_MILLISECONDS, EVENT_STICKY_BOMB, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_VOLATILE_CONCOCTION, 28.5 * IN_MILLISECONDS, EVENT_VOLATILE_CONCOCTION, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_SHOTGUN_TARGETING, 3.5 * IN_MILLISECONDS, EVENT_SHOTGUN, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Explosive Barrel Trigger 70755
struct npc_high_seas_explosive_barrel_trigger : public ScriptedAI
{
    npc_high_seas_explosive_barrel_trigger(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetDisplayFromModel(1);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Explosive Barrel 71106, Plant Explosives 67394
struct npc_high_seas_explosive_barrel : public ScriptedAI
{
    npc_high_seas_explosive_barrel(Creature* creature) : ScriptedAI(creature) { }

    bool hasTriggered;

    void Reset() override
    {
        hasTriggered = false;
    }

    void DoAction(int32 actionId) override
    {
        if (actionId == ACTION_START_INTRO && !hasTriggered)
        {
            hasTriggered = true;

            if (me->GetEntry() == NPC_PLANT_EXPLOSIVES)
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
            else
                me->DespawnOrUnsummon();

            if (me->GetInstanceScript())
                me->GetInstanceScript()->SetData(me->GetEntry() == NPC_EXPLOSIVE_BARREL ? EXPLOSIVES_COUNT_DATA : PLANT_COUNT_DATA, 1);
        }
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Admiral Hodgson 71327
struct npc_high_seas_admiral_hodgson : public customCreatureAI
{
    npc_high_seas_admiral_hodgson(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    bool hasTriggered;
    float x, y, z;

    void Reset() override
    {
        events.Reset();
        scheduler.CancelAll();
        hasTriggered = false;
        x = 0.0f; y = 0.0f; z = 0.0f;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_INTRO);

        scheduler
            .Schedule(Seconds(7), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_2); // just yell sometimes this for nothing
            context.Repeat(Seconds(8), Seconds(12));
        });

        // Call Bombardment

        // Call for help
        std::list<Creature*> deckhandList;
        GetCreatureListWithEntryInGrid(deckhandList, me, NPC_ALLIANCE_DECKHAND, 65.0f);

        for (auto&& itr : deckhandList)
            itr->SetInCombatWithZone();

        events.ScheduleEvent(EVENT_VERTICAL_SLASH, 4.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHOT, 10 * IN_MILLISECONDS);

        scheduler
            .Schedule(Milliseconds(4000), [this](TaskContext context)
        {
            std::list<Player*> targets;
            GetPlayerListInGrid(targets, me, 60.0f);

            if (!targets.empty())
            {
                Player* target = Trinity::Containers::SelectRandomContainerElement(targets);

                if (Creature* bunny = me->SummonCreature(NPC_GENERAL_BUNNY_JMF, *target, TEMPSUMMON_TIMED_DESPAWN, 8 * IN_MILLISECONDS))
                {
                    x = bunny->GetPositionX() + frand(-2.0f, 2.0f);
                    y = bunny->GetPositionY() + frand(-2.0f, 2.0f);
                    z = bunny->GetPositionZ();

                    if (Aura* targeting = bunny->AddAura(SPELL_BOMBS_AWAY_TARGETING, bunny))
                        targeting->SetDuration(5 * IN_MILLISECONDS);

                    scheduler
                        .Schedule(Milliseconds(4000), [this](TaskContext context)
                    {
                        me->CastSpell(x, y, z, SPELL_BOMBS_AWAY, true);
                    });
                }
            }

            context.Repeat(Milliseconds(4800));
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(50) && !hasTriggered)
        {
            hasTriggered = true;
            Talk(TALK_SPECIAL_1);
            Talk(TALK_SPECIAL_3); // Announce

            // Allow to use Rapire
            if (GameObject* rapier = ObjectAccessor::GetGameObject(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(GO_RAPIER) : ObjectGuid::Empty))
                rapier->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_INTERACT_COND);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(DATA_DEFEAT_THE_ADMIRAL, DONE);
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
            ExecuteTargetEvent(SPELL_COUNTER_SHOT, 15.6 * IN_MILLISECONDS, EVENT_SHOT, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_VERTICAL_SLASH, 9.5 * IN_MILLISECONDS, EVENT_VERTICAL_SLASH, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Net Climber 67331
struct npc_high_seas_net_climber : public ScriptedAI
{
    npc_high_seas_net_climber(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;
    uint8 posId;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();
        posId = 0;

        if (me->GetPositionX() > 2400.0f)
            return;

        if (me->GetPositionX() < 2200.0f)
        {
            posId = 3;
            return;
        }

        posId = me->GetPositionX() > 2320.0f ? 1 : 2;
    }

    void Reset() override
    {
        me->SetDisplayFromModel(1);

        scheduler
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            Movement::MoveSplineInit init(me);

            init.MoveTo(G3D::Vector3(netScalePos[posId].GetPositionX(), netScalePos[posId].GetPositionY(), netScalePos[posId].GetPositionZ()));
            init.Launch();

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                if (Unit* passenger = ObjectAccessor::GetUnit(*me, summonerGUID))
                    passenger->ExitVehicle();

                me->DespawnOrUnsummon(1000);
            });
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Cannon Launch 141812
class spell_high_seas_cannon_launch : public SpellScript
{
    PrepareSpellScript(spell_high_seas_cannon_launch);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            caster->SetHomePosition(*caster);
            caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
        }
        else if (Player* caster = GetCaster()->ToPlayer())
        {
            caster->m_Events.Schedule(500, [=]()
            {
                caster->RemoveAurasDueToSpell(SPELL_CANNON_LAUNCH);
            });
        }
    }

    void Register()
    {
        OnEffectHit += SpellEffectFn(spell_high_seas_cannon_launch::HandleHit, EFFECT_0, SPELL_EFFECT_JUMP_DEST);
    }
};

// Activate Random Barrel Selector 141099
class spell_high_seas_activate_random_barrel_selector : public SpellScript
{
    PrepareSpellScript(spell_high_seas_activate_random_barrel_selector);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Unit* caster = GetCaster())
            if (Creature* target = GetHitCreature())
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_EXPLOSIVE_BARREL_TRIGGER; });
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && target->ToUnit()->HasAura(SPELL_EXPLOSIVE_BARREL); });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_activate_random_barrel_selector::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_high_seas_activate_random_barrel_selector::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Bomb 132991
class spell_high_seas_bomb : public AuraScript
{
    PrepareAuraScript(spell_high_seas_bomb);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_BOMB_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_high_seas_bomb::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Bomb Eff 132993
class spell_high_seas_bomb_eff : public SpellScript
{
    PrepareSpellScript(spell_high_seas_bomb_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->RemoveAurasDueToSpell(SPELL_BOMB);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() == NPC_LIEUTENANT_FUSEBLAST || target->GetEntry() == NPC_LIEUTENANT_FIZZBOIL || target->GetEntry() == NPC_LIUTENANT_BOLTBLASTER || target->GetEntry() == NPC_EXPLOSIVE_BARREL_TRIGGER; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_bomb_eff::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_high_seas_bomb_eff::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Activate Random Barrel Eff 141847
class spell_high_seas_activate_random_barrel_eff : public SpellScript
{
    PrepareSpellScript(spell_high_seas_activate_random_barrel_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                caster->AI()->SetGUID(target->GetGUID());
                target->CastSpell(target, SPELL_EXPLOSIVE_BARREL, true);
                caster->CastSpell(caster, SPELL_SUMMON_FUSE, true);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_activate_random_barrel_eff::HandleHit, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// Sticky Bomb 132998
class spell_high_seas_sticky_bomb : public SpellScript
{
    PrepareSpellScript(spell_high_seas_sticky_bomb);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Unit* target = GetHitUnit())
                caster->AI()->Talk(TALK_SPECIAL_1, target);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_sticky_bomb::HandleHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Sticky Bomb 132998
class spell_high_seas_sticky_bomb_aura : public AuraScript
{
    PrepareAuraScript(spell_high_seas_sticky_bomb_aura);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetOwner()->ToUnit())
                caster->CastSpell(target, SPELL_STICKY_BOMB_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_high_seas_sticky_bomb_aura::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Barrel Explosion Eff 141465
class spell_high_seas_barrel_explosion_eff : public SpellScript
{
    PrepareSpellScript(spell_high_seas_barrel_explosion_eff);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() == NPC_LIEUTENANT_FUSEBLAST || target->GetEntry() == NPC_LIEUTENANT_FIZZBOIL || target->GetEntry() == NPC_LIUTENANT_BOLTBLASTER || target->GetEntry() == NPC_EXPLOSIVE_BARREL_TRIGGER || target->GetEntry() == NPC_FUSE; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_high_seas_barrel_explosion_eff::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Shotgun Targeting 142176
class spell_high_seas_shotgun_targeting : public SpellScript
{
    PrepareSpellScript(spell_high_seas_shotgun_targeting);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                if (caster->HasAura(SPELL_SHOTGUN_AMMO))
                    caster->CastSpell(target, SPELL_SHOTGUN, false);
                else
                {
                    caster->AI()->Talk(TALK_SPECIAL_1);
                    caster->AI()->Talk(TALK_SPECIAL_2); // announce
                    caster->CastSpell(caster, SPELL_RELOADING);
                }
            }
        }
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_shotgun_targeting::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_high_seas_shotgun_targeting::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Shotgun 141381
class spell_high_seas_shotgun : public SpellScript
{
    PrepareSpellScript(spell_high_seas_shotgun);

    void HandleAfterCast()
    {
        if (Aura* ammo = GetCaster()->GetAura(SPELL_SHOTGUN_AMMO))
        {
            if (ammo->GetStackAmount() > 1)
                ammo->SetStackAmount(ammo->GetStackAmount() - 1);
            else
                GetCaster()->RemoveAurasDueToSpell(SPELL_SHOTGUN_AMMO);
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_high_seas_shotgun::HandleAfterCast);
    }
};

// Shotgun Ammo 141389
class spell_high_seas_shotgun_ammo : public AuraScript
{
    PrepareAuraScript(spell_high_seas_shotgun_ammo);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        SetStackAmount(6);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_high_seas_shotgun_ammo::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Gather Explosives 141640
class spell_high_seas_gather_explosives : public SpellScript
{
    PrepareSpellScript(spell_high_seas_gather_explosives);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_gather_explosives::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Priming 132977
class spell_high_seas_priming : public SpellScript
{
    PrepareSpellScript(spell_high_seas_priming);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
            target->AI()->DoAction(ACTION_START_INTRO);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_priming::HandleHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// Bombs Away Eff 132938
class spell_high_seas_bombs_away_eff : public SpellScript
{
    PrepareSpellScript(spell_high_seas_bombs_away_eff);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Player* target = GetHitPlayer())
            target->GetMap()->SetWorldState(WORLDSTATE_KEEP_THOSE_BOMBS_AWAY, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_high_seas_bombs_away_eff::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 1009. Summoned by 143222 - Fire!
class sat_high_seas_fire : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_FIRE_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_FIRE_EFF);
    }
};

void AddSC_battle_on_the_high_seas()
{
    new creature_script<npc_high_seas_cannoneer>("npc_high_seas_cannoneer");
    new creature_script<npc_high_seas_swashbuckler>("npc_high_seas_swashbuckler");
    new creature_script<npc_high_seas_cannon_exploder>("npc_high_seas_cannon_exploder");
    new creature_script<npc_high_seas_admiral_hagman>("npc_high_seas_admiral_hagman");
    new creature_script<npc_high_seas_transport_cannon>("npc_high_seas_transport_cannon");
    new creature_script<npc_high_seas_horde_captain>("npc_high_seas_horde_captain");
    new creature_script<npc_high_seas_horde_squalsharper>("npc_high_seas_horde_squalsharper");
    new creature_script<npc_high_seas_zip>("npc_high_seas_zip");
    new creature_script<npc_high_seas_zip_back>("npc_high_seas_zip_back");
    new creature_script<npc_high_seas_bomb>("npc_high_seas_bomb");
    new creature_script<npc_high_seas_fuse>("npc_high_seas_fuse");
    new creature_script<npc_high_seas_triad_of_lieutenants>("npc_high_seas_triad_of_lieutenants");
    new creature_script<npc_high_seas_explosive_barrel_trigger>("npc_high_seas_explosive_barrel_trigger");
    new creature_script<npc_high_seas_explosive_barrel>("npc_high_seas_explosive_barrel");
    new creature_script<npc_high_seas_admiral_hodgson>("npc_high_seas_admiral_hodgson");
    new creature_script<npc_high_seas_net_climber>("npc_high_seas_net_climber");

    new spell_script<spell_high_seas_cannon_launch>("spell_high_seas_cannon_launch");
    new spell_script<spell_high_seas_activate_random_barrel_selector>("spell_high_seas_activate_random_barrel_selector");
    new aura_script<spell_high_seas_bomb>("spell_high_seas_bomb");
    new spell_script<spell_high_seas_bomb_eff>("spell_high_seas_bomb_eff");
    new spell_script<spell_high_seas_activate_random_barrel_eff>("spell_high_seas_activate_random_barrel_eff");
    new spell_script<spell_high_seas_sticky_bomb>("spell_high_seas_sticky_bomb");
    new aura_script<spell_high_seas_sticky_bomb_aura>("spell_high_seas_sticky_bomb_aura");
    new spell_script<spell_high_seas_barrel_explosion_eff>("spell_high_seas_barrel_explosion_eff");
    new spell_script<spell_high_seas_shotgun_targeting>("spell_high_seas_shotgun_targeting");
    new spell_script<spell_high_seas_shotgun>("spell_high_seas_shotgun");
    new aura_script<spell_high_seas_shotgun_ammo>("spell_high_seas_shotgun_ammo");
    new spell_script<spell_high_seas_gather_explosives>("spell_high_seas_gather_explosives");
    new spell_script<spell_high_seas_priming>("spell_high_seas_priming");
    new spell_script<spell_high_seas_bombs_away_eff>("spell_high_seas_bombs_away_eff");
    new atrigger_script<sat_high_seas_fire>("sat_high_seas_fire");
}