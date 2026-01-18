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

#include "proving_grounds.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "CreatureAI.h"
#include "MoveSplineInit.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "ScenarioMgr.h"
#include "LFGMgr.h"

enum Spells
{
    SPELL_DEFEND                   = 142174,
    SPELL_AMBER_GLOBULE_LINK       = 142190,
    SPELL_AMBER_GLOBULE            = 142189,
    SPELL_AMBER_GLOBULE_DETONATION = 142191,
    SPELL_AMBER_GLOBULE_VISUAL     = 147287,
    SPELL_AMBER_GLOBULE_AT         = 147292,
    SPELL_HEAL_ILLUSION            = 142238,
    SPELL_BANANASTORM              = 142639,
    SPELL_BANANASTORM_SELECTOR     = 142627,
    SPELL_BANANASTORM_MISSLE       = 142628,
    SPELL_BANANASTORM_EFF          = 142629,
    SPELL_TIMER_10                 = 141577,
    SPELL_TIMER_15                 = 141557,
    SPELL_TIMER_18                 = 141579,
    SPELL_TIMER_20                 = 141580,
    SPELL_TIMER_25                 = 141581,
    SPELL_TIMER_30                 = 141582,
    SPELL_TIMER_35                 = 141583,
    SPELL_TIMER_40                 = 141584,
    SPELL_EXPOSED                  = 142489,
    SPELL_PROTECTION_SHIELD        = 142427,

    // Tank Trial
    SPELL_CHOMP                    = 144113,
    SPELL_FOCUSED_CHI_BURST        = 144103,
    SPELL_CRACKLING_JADE_THUNDER   = 144076,
    SPELL_RENEWING_MIST            = 144080,
    SPELL_PYROBLAST                = 147601,
    SPELL_INVOKE_LAVA              = 144374,
    SPELL_INVOKE_LAVA_MISSLE       = 144376,
    SPELL_INVOKE_LAVA_AT           = 144377,
    SPELL_INVOKE_LAVA_EFF          = 144383,
    SPELL_WING_BLAST               = 144106,
    SPELL_FAN_OF_BANANAS           = 144398,
    SPELL_ENRAGE                   = 144404,
    SPELL_POWERFUL_SLAM            = 144401,

    // Healer Trial
    SPELL_SHIELD_WALL              = 145057,
    SPELL_DEFENSIVE_STANCE         = 145044,
    SPELL_SHIELD_BLOCK             = 145054,
    SPELL_DEVASTATE                = 145060,
    SPELL_TAUNT                    = 145058,
    SPELL_CHARGE_S                 = 145048,
    SPELL_THUNDER_CLAP             = 145046,
    SPELL_SKULL_BASH               = 142657, // not sure
    SPELL_SHOCKWAVE                = 145047,
    SPELL_ENVENOM                  = 145416,
    SPELL_MUTILATE_OFFH            = 145413,
    SPELL_MUTILATE                 = 145414,
    SPELL_RUPTURE                  = 145417,
    SPELL_KICK                     = 145427,
    SPELL_FAN_OF_KNIVES            = 145422,
    SPELL_SLICE_AND_DICE           = 145418,
    SPELL_DEADLY_POISON            = 145421,
    SPELL_STEALTH                  = 145423,
    SPELL_CHEAP_SHOT               = 145424,
    SPELL_ARCANE_MISSLES           = 145436,
    SPELL_ARCANE_BLAST             = 145434,
    SPELL_ARCANE_BARRAGE           = 145437,
    SPELL_COUNTERSPELL             = 145530,
    SPELL_TIME_WARP                = 145534,
    SPELL_COBRA_SHOT               = 145654,
    SPELL_EXPLOSIVE_SHOT           = 145661,
    SPELL_MULTI_SHOT               = 145667,
    SPELL_FEIGN_DEATH              = 145666,
    SPELL_SERPENT_STRING           = 145656,
    SPELL_AUTO_SHOT                = 145759,
    SPELL_COUNTER_SHOT             = 147362,
    SPELL_CALL_SPEEDY              = 145412, // hunter turtle

    // Creatures from Healer Trial
    SPELL_SONIC_BLAST              = 145200,
    SPELL_AQUA_BOMB                = 145206,
    SPELL_AQUA_BOMB_EFF            = 145208,
    SPELL_CHOMP_H                  = 145263,
    SPELL_BURROW                   = 145260,
    SPELL_ENRAGE_H                 = 145411,

    // Misc
    SPELL_BERSERKING_AT            = 147294,
    SPELL_BERSERKING_EFF           = 147293,
};

std::map<uint32, uint32> interruptType =
{
    { NPC_OTO_THE_PROTECTOR,     SPELL_SKULL_BASH   },
    { NPC_SOOLI_THE_SURVIVALIST, SPELL_COUNTER_SHOT },
    { NPC_KAVAN_THE_ARCANIST,    SPELL_COUNTERSPELL },
    { NPC_KI_THE_ASSASSIN,       SPELL_KICK         },
};

struct provingGroundsAI : public customCreatureAI
{
    provingGroundsAI(Creature* creature) : customCreatureAI(creature), summons(me) { }

    TaskScheduler scheduler;
    SummonList summons;
    bool hasReached;
    bool allowCast;
    float x, y;
    ObjectGuid targetGUID;
    uint32 timerID;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        x = 0.0f; y = 0.0f;
        hasReached = false;
        allowCast  = true;
        DoCast(me, invIllusionaryType.find(me->GetEntry())->second);
        me->SetInCombatWithZone();
        targetGUID = ObjectGuid::Empty;
        timerID    = 0;

        // Send Attack us by player friends
        for (auto&& itr : invHealerTrialFriends)
            if (Creature* helper = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr.first) : ObjectGuid::Empty))
                helper->AI()->AttackStart(me);

        if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
            sikari->AI()->AttackStart(me);

        if (IsDpsTrial())
            me->PrepareChanneledCast(me->GetOrientation());
        else // force to attack our friends
        {
            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                AttackStart(sikari);

            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
                AttackStart(oto);
        }

        HandleUpscaleEndless();
    }

    void DamageDealt(Unit* victim, uint32& damage, DamageEffectType damageType) override
    {
        // Hackfix for blocks (Creature block doesn`t implement in core)
        if (victim && victim->GetEntry() == NPC_OTO_THE_PROTECTOR && damageType == DIRECT_DAMAGE && victim->HasAura(SPELL_SHIELD_BLOCK))
            damage = 0;
    }

    bool IsDpsTrial()
    {
        for (auto&& itr : illusions)
            if (me->HasAura(itr))
                return true;

        return false;
    }

    // 1% per each wave, depend of role (increase health or damage done)
    void HandleUpscaleEndless()
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
        {
            if (controller->AI()->GetData(TRIAL_DATA) < TRIAL_DD_ENDLESS)
                return;

            uint32 waveId = controller->AI()->GetData(WAVE_DATA);
            if (!waveId)
                return;

            for (uint32 i = 0; i < waveId; i++)
                DoCast(me, controller->AI()->GetData(TRIAL_DATA) == TRIAL_TANK_ENDLESS ? SPELL_ENDLESS_WAVE_DMG : SPELL_ENDLESS_WAVE_HP, true);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        scheduler.CancelAll();
    }

    void HandleInterruptMySpell()
    {
        std::list<Creature*> protectorsList;

        for (auto&& itr : invHealerTrialFriends)
            GetCreatureListWithEntryInGrid(protectorsList, me, itr.first, 300.0f);

        protectorsList.remove_if([=](Creature* target) { return target && !target->AI()->GetData(INTERRUPT_DATA); });

        if (protectorsList.empty())
            return;

        ObjectGuid interruptMemberGUID = Trinity::Containers::SelectRandomContainerElement(protectorsList)->GetGUID();

        scheduler
            .Schedule(Milliseconds(500), [this, interruptMemberGUID](TaskContext context)
        {
            if (Creature* ourMember = ObjectAccessor::GetCreature(*me, interruptMemberGUID))
            {
                if (!me->HasUnitState(UNIT_STATE_CASTING)) // prevent if already interrupted by smth
                    return;

                switch (ourMember->GetEntry())
                {
                    case NPC_KAVAN_THE_ARCANIST:
                        ourMember->AI()->Talk(TALK_KAVAN_INTERRUPT, me);
                        break;
                    case NPC_KI_THE_ASSASSIN:
                        ourMember->AI()->Talk(TALK_KI_INTERRUPT, me);
                        break;
                }

                ourMember->AI()->SetData(INTERRUPT_DATA, 1);
                ourMember->CastSpell(me, interruptType.find(ourMember->GetEntry())->second, true);
            }
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Large Illusionary Slayer 71062, Small Illusionary Slayer 71080, Small Illusionary Varmint 71078, Large Illusionary Varmint 71067
struct npc_proving_grounds_base : public provingGroundsAI
{
    npc_proving_grounds_base(Creature* creature) : provingGroundsAI(creature) { }
};

// Illusionary Guardian 71079, 71064
struct npc_proving_grounds_illusionary_guardian : public provingGroundsAI
{
    npc_proving_grounds_illusionary_guardian(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            me->PrepareChanneledCast(frand(0.0f, 2 * M_PI), SPELL_DEFEND);
            context.Repeat(Seconds(15));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Illusionary Amber-Weaver 71077, 71068
struct npc_proving_grounds_illusionary_amber_weaver : public provingGroundsAI
{
    npc_proving_grounds_illusionary_amber_weaver(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (Player* target = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
            {
                DoCast(target, SPELL_AMBER_GLOBULE_LINK); // tracking
                DoCast(me, SPELL_AMBER_GLOBULE);
            }

            context.Repeat(Seconds(11));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Illusionary Mystic 71069, 71076
struct npc_proving_grounds_illusionary_mystic : public provingGroundsAI
{
    npc_proving_grounds_illusionary_mystic(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(7), [this](TaskContext context)
        {
            if (Creature* target = ObjectAccessor::GetCreature(*me, getLowestFriendlyGUID()))
                DoCast(target, SPELL_HEAL_ILLUSION);

            context.Repeat(Seconds(14));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    private:
        ObjectGuid getLowestFriendlyGUID()
        {
            std::list<Creature*> tmpTargets;
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_SLAYER, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_SLAYER, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_GUARDIAN, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_GUARDIAN, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_VARMINT, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_VARMINT, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_AMBER_WEAVER, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_AMBER_WEAVER, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_MYSTIC, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_MYSTIC, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_BANANA_TOSSER, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_BANANA_TOSSER, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_BANSHE, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_BANSHE, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_SMALL_ILLUSIONARY_SHA, 70.0f);
            GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_LARGE_ILLUSIONARY_SHA, 70.0f);

            if (tmpTargets.empty())
                return ObjectGuid::Empty;

            tmpTargets.sort(Trinity::HealthPctOrderPred());

            if (Creature* lowestTarget = tmpTargets.front())
                return lowestTarget->GetGUID();
            return ObjectGuid::Empty;
        }
};

// Illusionary Banana-Tosser 71414, 71415
struct npc_proving_grounds_illusionary_banana_tosser : public provingGroundsAI
{
    npc_proving_grounds_illusionary_banana_tosser(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (hasReached && !me->HasAura(SPELL_BANANASTORM)) // use this on reach first destination
                DoCast(me, SPELL_BANANASTORM);

            // Generate Path
            x = arenaCenterPos.GetPositionX() + frand(-30.0f, 30.0f);
            y = arenaCenterPos.GetPositionY() + frand(-30.0f, 30.0f);
            float z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            hasReached = true;
            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Illusionary Banshe 71075, 71070
struct npc_proving_grounds_illusionary_banshe : public provingGroundsAI
{
    npc_proving_grounds_illusionary_banshe(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (hasReached) // use this on reach first destination
            {
                if (timerID && !me->HasAura(timerID))
                    DoCast(me, timerID, true);
                else if (!timerID)
                {
                    timerID = me->GetEntry() == NPC_LARGE_ILLUSIONARY_BANSHE ? SPELL_TIMER_20 : SPELL_TIMER_10;
                    DoCast(me, timerID, true);
                }
            }

            // Generate Path
            x = arenaCenterPos.GetPositionX() + frand(-10.0f, 10.0f);
            y = arenaCenterPos.GetPositionY() + frand(-10.0f, 10.0f);
            float z = me->GetPositionZ();
            me->UpdateAllowedPositionZ(x, y, z);
            me->GetMotionMaster()->MovePoint(0, x, y, z);

            hasReached = true;
            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == TIMER_DATA)
            timerID = data;
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Illusionary Sha 71074, 71072
struct npc_proving_grounds_illusionary_sha : public provingGroundsAI
{
    npc_proving_grounds_illusionary_sha(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(me, SPELL_PROTECTION_SHIELD);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Illusionary Forager 71831, 71832
struct npc_proving_grounds_illusionary_forager : public provingGroundsAI
{
    npc_proving_grounds_illusionary_forager(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(3), Seconds(7), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
                DoCast(target, SPELL_CHOMP);

            context.Repeat(Seconds(6), Seconds(12));
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

// Illusionary Flamecaller 71835, 71836
struct npc_proving_grounds_illusionary_flamecaller : public provingGroundsAI
{
    npc_proving_grounds_illusionary_flamecaller(Creature* creature) : provingGroundsAI(creature) 
    {
        SetCombatMovement(false);
    }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
                DoCast(target, SPELL_INVOKE_LAVA);
        });

        scheduler
            .Schedule(Seconds(5), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
                DoCast(target, SPELL_PYROBLAST);

            context.Repeat(Milliseconds(1600));
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

// Illusionary Wind Guard 71833, 71834
struct npc_proving_grounds_illusionary_wind_guard : public provingGroundsAI
{
    npc_proving_grounds_illusionary_wind_guard(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(5), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
            {
                targetGUID = target->GetGUID();
                me->PrepareChanneledCast(me->GetAngle(target), SPELL_WING_BLAST);

                scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                {
                    me->RemoveChanneledCast(targetGUID);
                });
            }

            context.Repeat(Seconds(8), Seconds(12));
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

// Illusionary Ambusher 71838, 71839
struct npc_proving_grounds_illusionary_ambusher : public provingGroundsAI
{
    npc_proving_grounds_illusionary_ambusher(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            DoCast(me, SPELL_FAN_OF_BANANAS);
            context.Repeat(Seconds(2), Seconds(4));
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

// Illusionary Conqueror 71841, 71842
struct npc_proving_grounds_illusionary_conqueror : public provingGroundsAI
{
    npc_proving_grounds_illusionary_conqueror(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(5), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
            {
                targetGUID = target->GetGUID();
                me->PrepareChanneledCast(me->GetAngle(target), SPELL_POWERFUL_SLAM);

                scheduler
                    .Schedule(Seconds(2), [this](TaskContext context)
                {
                    me->RemoveChanneledCast(targetGUID);
                });
            }

            context.Repeat(Seconds(10));
        });

        scheduler
            .Schedule(Seconds(10), [this](TaskContext context)
        {
            DoCast(me, SPELL_ENRAGE);
            context.Repeat(Seconds(50));
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

// Illusionary Hive-Singer 72343, 72342
struct npc_proving_grounds_illusionary_hive_singer : public provingGroundsAI
{
    npc_proving_grounds_illusionary_hive_singer(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            HandleInterruptMySpell();
            DoCast(me, SPELL_SONIC_BLAST);
            context.Repeat(Seconds(6), Seconds(10));
        });
    }

    void AttackStart(Unit* target) override
    {
        if (!target || target && target->ToPlayer())
            return;

        if (target && me->Attack(target, true))
            me->GetMotionMaster()->MoveChase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Illusionary Flamecaller 72338, 72339
struct npc_proving_grounds_illusionary_flamecaller_healer : public provingGroundsAI
{
    npc_proving_grounds_illusionary_flamecaller_healer(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(3), [this](TaskContext context)
        {
            if (Unit* target = me->GetVictim())
                DoCast(target, SPELL_INVOKE_LAVA);
        });

        scheduler
            .Schedule(Seconds(5), [this](TaskContext context)
        {
            if (allowCast)
            {
                me->StopMoving();
                me->GetMotionMaster()->MovementExpired();

                if (Unit* vict = me->GetVictim())
                {
                    if (me->GetExactDist2d(vict) > 35.0f)
                        DoStartMovement(vict);
                    else
                        DoStartNoMovement(vict);
                }

                HandleInterruptMySpell();

                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_PYROBLAST);
            }

            context.Repeat(Milliseconds(1600));
        });
    }

    void CastInterrupted(SpellInfo const* spell) override
    {
        if (Unit* vict = me->GetVictim())
        {
            allowCast = false;
            me->GetMotionMaster()->MoveChase(vict);

            scheduler
                .Schedule(Seconds(4), [this](TaskContext context)
            {
                allowCast = true;
            });
        }
    }

    void AttackStart(Unit* target) override
    {
        if (!target || target && target->ToPlayer())
            return;

        if (target && me->Attack(target, true))
            me->GetMotionMaster()->MoveChase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Illusionary Aqualyte 72346, 72344
struct npc_proving_grounds_illusionary_aqualyte : public provingGroundsAI
{
    npc_proving_grounds_illusionary_aqualyte(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 200.0f, false))
                DoCast(target, SPELL_AQUA_BOMB);

            context.Repeat(Seconds(10), Seconds(14));
        });
    }

    void AttackStart(Unit* target) override
    {
        if (!target || target && target->ToPlayer())
            return;

        if (target && me->Attack(target, true))
            me->GetMotionMaster()->MoveChase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Illusionary Tunneler 72387, 72386
struct npc_proving_grounds_illusionary_tunneler : public provingGroundsAI
{
    npc_proving_grounds_illusionary_tunneler(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(2), [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [=](Unit const* conditionTarget) { return conditionTarget && !conditionTarget->ToPlayer(); })) // no target player for this
                DoCast(target, SPELL_BURROW);

            context.Repeat(Seconds(12), Seconds(16));
        });
    }

    void AttackStart(Unit* target) override
    {
        if (!target || target && target->ToPlayer())
            return;

        if (target && me->Attack(target, true))
            me->GetMotionMaster()->MoveChase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Illusionary Conqueror 72341, 72340
struct npc_proving_grounds_illusionary_conqueror_healer : public provingGroundsAI
{
    npc_proving_grounds_illusionary_conqueror_healer(Creature* creature) : provingGroundsAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(8), [this](TaskContext context)
        {
            // Send Oto use shield wall not depend of CD or not
            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
            {
                oto->AI()->Talk(TALK_OTO_SHIELDWALL);
                oto->CastSpell(oto, SPELL_SHIELD_WALL, true);
            }

            DoCast(me, SPELL_ENRAGE_H);
            context.Repeat(Seconds(15), Seconds(25));
        });
    }

    void AttackStart(Unit* target) override
    {
        if (!target || target && target->ToPlayer())
            return;

        if (target && me->Attack(target, true))
            me->GetMotionMaster()->MoveChase(target);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

enum frienlyEvents
{
    EVENT_ARCANE_BLAST = 1, // by default
    EVENT_ARCANE_MISSLES, // after 4td cast of blast
    EVENT_ARCANE_BARRAGE, // by CD

    EVENT_MUTILATE, // by default
    EVENT_ENVENOM, // on reach 5 CP (but for us - on 5td use mutilate)
    EVENT_FAN_OF_KNIVES, // AOE only
    EVENT_SLICE_AND_DICE, // by CD
    EVENT_RUPTURE, // by CD

    EVENT_SHIELD_BLOCK, // by CD, but at low HP (less 75 pct)
    EVENT_DEVASTATE, // by default
    EVENT_SHOCKWAVE, // AOE
    EVENT_THUNDER_CLAP, // AOE by CD

    EVENT_AUTO_SHOT, // autoattack
    EVENT_COBRA_SHOT, // default
    EVENT_EXPLOSIVE_SHOT, // by CD
    EVENT_MULTI_SHOT, // AOE
    EVENT_SERPENT_STING, // by CD
};

// Kavan the Arcanist 72221
struct npc_proving_grounds_kavan_the_arcanist : public customCreatureAI
{
    npc_proving_grounds_kavan_the_arcanist(Creature* creature) : customCreatureAI(creature) 
    { 
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            if (controller->AI()->GetData(TRIAL_DATA) == TRIAL_HEAL_SILVER)
                Talk(TALK_HEALER_SILVER_INTRO);
    }

    TaskScheduler scheduler;
    uint32 blastCount;
    bool allowInterrupt;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        blastCount = 0;
        allowInterrupt = true;
    }

    void AttackStart(Unit* target) override
    {
        if (!target)
            return;

        if (me->Attack(target, false))
            DoStartNoMovement(target);
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_ARCANE_BLAST, 1 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_ARCANE_BARRAGE, 3.5 * IN_MILLISECONDS);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == INTERRUPT_DATA)
            return allowInterrupt ? 1 : 0;

        return 0;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(90) && attacker && attacker->GetTarget() == me->GetGUID())
        {
            // Re-taunt if available
            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
            {
                if (!oto->AI()->GetData(TAUNT_DATA))
                    return;

                oto->AI()->SetData(TAUNT_DATA, 1);
                oto->CastSpell(attacker, SPELL_TAUNT, true);
            }
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == INTERRUPT_DATA)
        {
            allowInterrupt = false;

            scheduler
                .Schedule(Seconds(10), [this](TaskContext context)
            {
                allowInterrupt = true;
            });
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            controller->AI()->DoAction(ACTION_FAIL_TRIAL);
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
                case EVENT_ARCANE_BLAST:
                    me->StopMoving();
                    me->GetMotionMaster()->MovementExpired();

                    if (Unit* vict = me->GetVictim())
                    {
                        if (me->GetExactDist2d(vict) > 35.0f)
                            DoStartMovement(vict);
                        else
                            DoStartNoMovement(vict);
                    }

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_ARCANE_BLAST);

                    if (++blastCount > 3)
                    {
                        blastCount = 0;
                        events.ScheduleEvent(EVENT_ARCANE_MISSLES, 2.1 * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_ARCANE_BLAST, 2.5 * IN_MILLISECONDS);
                    }
                    else
                        events.ScheduleEvent(EVENT_ARCANE_BLAST, 2.1 * IN_MILLISECONDS);
                    break;
                case EVENT_ARCANE_MISSLES:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_ARCANE_MISSLES);
                    break;
                case EVENT_ARCANE_BARRAGE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_ARCANE_BARRAGE);

                    events.ScheduleEvent(EVENT_ARCANE_BARRAGE, 8 * IN_MILLISECONDS);
                    break;
            }
        }
    }
};

// Ki the Assassin 72219
struct npc_proving_grounds_ki_the_assassin : public customCreatureAI
{
    npc_proving_grounds_ki_the_assassin(Creature* creature) : customCreatureAI(creature) 
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            if (controller->AI()->GetData(TRIAL_DATA) == TRIAL_HEAL_GOLD)
                Talk(TALK_HEALER_GOLD_INTRO);
    }

    TaskScheduler scheduler;
    uint8 mutilateCount;
    bool allowInterrupt;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        mutilateCount  = 0;
        allowInterrupt = true;

        //DoCast(me, SPELL_STEALTH);
        DoCast(me, SPELL_DEADLY_POISON);
    }

    void DamageDealt(Unit* victim, uint32& /*damage*/, DamageEffectType damageType) override
    {
        if (damageType == DIRECT_DAMAGE && me->HasAura(SPELL_STEALTH) && victim)
            DoCast(victim, SPELL_CHEAP_SHOT, true);
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_MUTILATE, 2 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_FAN_OF_KNIVES, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SLICE_AND_DICE, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_RUPTURE, 6 * IN_MILLISECONDS);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == INTERRUPT_DATA)
            return allowInterrupt ? 1 : 0;

        return 0;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(90) && attacker && attacker->GetTarget() == me->GetGUID())
        {
            // Re-taunt if available
            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
            {
                if (!oto->AI()->GetData(TAUNT_DATA))
                    return;

                oto->AI()->SetData(TAUNT_DATA, 1);
                oto->CastSpell(attacker, SPELL_TAUNT, true);
            }
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == INTERRUPT_DATA)
        {
            allowInterrupt = false;

            scheduler
                .Schedule(Seconds(10), [this](TaskContext context)
            {
                allowInterrupt = true;
            });
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            controller->AI()->DoAction(ACTION_FAIL_TRIAL);
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
                case EVENT_MUTILATE:
                    if (me->GetVictim() && me->GetExactDist2d(me->GetVictim()) < 5.5f)
                    {
                        DoCastVictim(SPELL_MUTILATE);
                        DoCastVictim(SPELL_MUTILATE_OFFH);
                        mutilateCount += 2;

                        if (mutilateCount > 4)
                        {
                            mutilateCount -= 5;
                            events.ScheduleEvent(EVENT_ENVENOM, 0.5 * IN_MILLISECONDS);
                        }
                    }

                    events.ScheduleEvent(EVENT_MUTILATE, 3.5 * IN_MILLISECONDS);
                    break;
                case EVENT_ENVENOM:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_ENVENOM, true);
                    break;
                case EVENT_SLICE_AND_DICE:
                    DoCast(me, SPELL_SLICE_AND_DICE, true);
                    events.ScheduleEvent(EVENT_SLICE_AND_DICE, 15 * IN_MILLISECONDS);
                    break;
                case EVENT_FAN_OF_KNIVES:
                    if (getTotalVictims() > 2)
                        DoCast(me, SPELL_FAN_OF_KNIVES, true);

                    events.ScheduleEvent(EVENT_FAN_OF_KNIVES, 3 * IN_MILLISECONDS);
                    break;
                case EVENT_RUPTURE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_RUPTURE, true);

                    events.ScheduleEvent(EVENT_RUPTURE, 7.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    private:
        uint32 getTotalVictims()
        {
            uint32 count = 0;
            for (auto&& itr : me->GetThreatManager().getThreatList())
                if (Creature* target = ObjectAccessor::GetCreature(*me, itr->getUnitGuid()))
                    count++;

            return count;
        }
};

// Oto the Protector 72218
struct npc_proving_grounds_oto_the_protector : public customCreatureAI
{
    npc_proving_grounds_oto_the_protector(Creature* creature) : customCreatureAI(creature) 
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            if (controller->AI()->GetData(TRIAL_DATA) == TRIAL_HEAL_BRONZE)
                Talk(TALK_HEALER_BRONZE_INTRO);
    }

    TaskScheduler scheduler;
    bool canUseShieldWall;
    bool allowInterrupt;
    bool allowTaunt;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        canUseShieldWall = true;
        allowInterrupt   = true;
        allowTaunt       = true;
        events.Reset();
        DoCast(me, SPELL_DEFENSIVE_STANCE, true);
    }

    void JustEngagedWith(Unit* who) override
    {
        DoCast(who, SPELL_CHARGE_S);
        events.ScheduleEvent(EVENT_DEVASTATE, 2 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHIELD_BLOCK, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SHOCKWAVE, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_THUNDER_CLAP, 6 * IN_MILLISECONDS);
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case INTERRUPT_DATA:
                return allowInterrupt ? 1 : 0;
            case TAUNT_DATA:
                return allowTaunt ? 1 : 0;
        }

        return 0;
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case INTERRUPT_DATA:
                allowInterrupt = false;

                scheduler
                    .Schedule(Seconds(10), [this](TaskContext context)
                {
                    allowInterrupt = true;
                });
                break;
            case TAUNT_DATA:
                allowTaunt = false;

                scheduler
                    .Schedule(Seconds(8), [this](TaskContext context)
                {
                    allowTaunt = true;
                });
                break;
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            controller->AI()->DoAction(ACTION_FAIL_TRIAL);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        // 30s recovery
        if (HealthBelowPct(51) && canUseShieldWall)
        {
            Talk(TALK_OTO_SHIELDWALL);
            canUseShieldWall = false;
            DoCast(me, SPELL_SHIELD_WALL);

            scheduler
                .Schedule(Seconds(30), [this](TaskContext context)
            {
                canUseShieldWall = true;
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
            switch (eventId)
            {
                case EVENT_DEVASTATE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_DEVASTATE);

                    events.ScheduleEvent(EVENT_DEVASTATE, 1.5 * IN_MILLISECONDS);
                    break;
                case EVENT_SHIELD_BLOCK:
                    if (HealthAbovePct(75))
                    {
                        events.ScheduleEvent(EVENT_SHIELD_BLOCK, 1 * IN_MILLISECONDS);
                        break;
                    }

                    DoCast(me, SPELL_SHIELD_BLOCK, true);
                    events.ScheduleEvent(EVENT_SHIELD_BLOCK, 9 * IN_MILLISECONDS);
                    break;
                case EVENT_SHOCKWAVE:
                    if (getTotalVictims() < 3)
                    {
                        events.ScheduleEvent(EVENT_SHOCKWAVE, 1 * IN_MILLISECONDS);
                        break;
                    }

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHOCKWAVE);

                    events.ScheduleEvent(EVENT_SHOCKWAVE, 15 * IN_MILLISECONDS);
                    break;
                case EVENT_THUNDER_CLAP:
                    DoCast(me, SPELL_THUNDER_CLAP);
                    events.ScheduleEvent(EVENT_THUNDER_CLAP, 7.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    private:
        uint32 getTotalVictims()
        {
            uint32 count = 0;
            for (auto&& itr : me->GetThreatManager().getThreatList())
                if (Creature* target = ObjectAccessor::GetCreature(*me, itr->getUnitGuid()))
                    count++;

            return count;
        }
};

// Sooli the Survivalist 72220
struct npc_proving_grounds_sooli_the_survivalist : public customCreatureAI
{
    npc_proving_grounds_sooli_the_survivalist(Creature* creature) : customCreatureAI(creature) 
    { 
        SetCombatMovement(false);
        DoCast(me, SPELL_CALL_SPEEDY);
    }

    TaskScheduler scheduler;
    uint32 allowInterrupt;
    bool allowFeignDeath;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        allowInterrupt = true;
        allowFeignDeath = true;
    }

    void JustEngagedWith(Unit* who) override
    {
        events.ScheduleEvent(EVENT_AUTO_SHOT, 0.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_COBRA_SHOT, 1.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_MULTI_SHOT, 10 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SERPENT_STING, 6 * IN_MILLISECONDS);
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == INTERRUPT_DATA)
            return allowInterrupt ? 1 : 0;

        return 0;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(90) && attacker && attacker->GetTarget() == me->GetGUID())
        {
            // Re-taunt if available
            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
            {
                if (!oto->AI()->GetData(TAUNT_DATA))
                    return;

                oto->AI()->SetData(TAUNT_DATA, 1);
                oto->CastSpell(attacker, SPELL_TAUNT, true);
            }
        }

        if (HealthBelowPct(60) && allowFeignDeath)
        {
            allowFeignDeath = false;
            DoCast(me, SPELL_FEIGN_DEATH);

            scheduler
                .Schedule(Seconds(1), [this](TaskContext context)
            {
                me->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);
            });

            scheduler
                .Schedule(Seconds(30), [this](TaskContext context)
            {
                allowFeignDeath = true;
            });
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == INTERRUPT_DATA)
        {
            allowInterrupt = false;

            scheduler
                .Schedule(Seconds(10), [this](TaskContext context)
            {
                allowInterrupt = true;
            });
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            controller->AI()->DoAction(ACTION_FAIL_TRIAL);
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
                case EVENT_AUTO_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_AUTO_SHOT, true);

                    events.ScheduleEvent(EVENT_AUTO_SHOT, 1.5 * IN_MILLISECONDS);
                    break;
                case EVENT_EXPLOSIVE_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_EXPLOSIVE_SHOT, true);

                    events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, 8 * IN_MILLISECONDS);
                    break;
                case EVENT_COBRA_SHOT:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_COBRA_SHOT);

                    events.ScheduleEvent(EVENT_COBRA_SHOT, 2.5 * IN_MILLISECONDS);
                    break;
                case EVENT_MULTI_SHOT:
                    if (getTotalVictims() > 2)
                        DoCast(me, SPELL_MULTI_SHOT, true);

                    events.ScheduleEvent(EVENT_MULTI_SHOT, 3 * IN_MILLISECONDS);
                    break;
                case EVENT_SERPENT_STING:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SERPENT_STRING, true);

                    events.ScheduleEvent(EVENT_SERPENT_STING, 7.5 * IN_MILLISECONDS);
                    break;
            }
        }
    }

    private:
        uint32 getTotalVictims()
        {
            uint32 count = 0;
            for (auto&& itr : me->GetThreatManager().getThreatList())
                if (Creature* target = ObjectAccessor::GetCreature(*me, itr->getUnitGuid()))
                    count++;

            return count;
        }
};

// Trial Master Rotun 61636
class npc_proving_grounds_trial_master_rotun : public CreatureScript
{
    public:
        npc_proving_grounds_trial_master_rotun() : CreatureScript("npc_proving_grounds_trial_master_rotun") { }
    
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_DD_BRONZE_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_TANK_BRONZE_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_HEALER_BRONZE_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_DD_SILVER_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_DD_GOLD_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 10:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_TANK_SILVER_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 9:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_TANK_GOLD_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 14:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_HEALER_SILVER_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 13:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_HEALER_GOLD_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 6:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_DD_ENDLESS_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 8:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_TANK_ENDLESS_TRIAL);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 12:
                    if (Creature* controller = ObjectAccessor::GetCreature(*player, player->GetInstanceScript() ? player->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                        controller->AI()->DoAction(ACTION_HEALER_ENDLESS_TRIAL);
                    break;
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            // by default for each only Bronze, next check achievements complete
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

            // Need check achievement there
            for (auto&& itr : invAllowTrialType)
                if (player->HasAchieved(itr.first, false))
                    player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), itr.second, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3 + itr.second);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

// Proving Grounds Controller 73334
struct npc_proving_grounds_controller : public ScriptedAI
{
    npc_proving_grounds_controller(Creature* creature) : ScriptedAI(creature), summons(me) { }

    SummonList summons;
    TaskScheduler scheduler;
    bool hasTrialStarted;
    uint32 currentTimer;
    uint32 waveId;
    uint32 trialId;
    uint32 endlessWaveId;
    std::vector<ObjectGuid> illusionGUIDs;
    bool hasYellSlayer;
    bool hasYellVarmint;
    bool hasYellGuardian;

    void Reset() override
    {
        me->SetVisible(false);
        hasTrialStarted = false;
        currentTimer  = 0;
        waveId        = 0;
        trialId       = 0;
        endlessWaveId = 0;
        scheduler.CancelAll();
        illusionGUIDs.clear();
        summons.DespawnAll();

        if (me->GetInstanceScript())
            me->GetInstanceScript()->SetData(BARRICADE_DATA, DONE);
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_DD_BRONZE_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_DD_BRONZE;

                if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                    rotun->AI()->Talk(TALK_TRIAL_BEGINNING);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 20 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (!illusionGUIDs.empty()) // fail trial
                    {
                        DoAction(ACTION_FAIL_TRIAL);
                        return;
                    }

                    if (++waveId > 5) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 1:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_SLAYER);
                            break;
                        case 3:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_GUARDIAN);
                            break;
                        case 4:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_VARMINT);

                            me->SummonCreature(NPC_BERSERKING, berserkSpawnPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                    }

                    // Clear corpses before set new
                    summons.DespawnAll();

                    auto itr = invBronzeTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second, TEMPSUMMON_MANUAL_DESPAWN);

                    currentTimer += 10 * IN_MILLISECONDS;


                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 5, ChallengeMedals::MedalTypeBronze, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_DD_SILVER_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_DD_SILVER;

                if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                    rotun->AI()->Talk(TALK_TRIAL_BEGINNING_SILVER);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 25 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (!illusionGUIDs.empty()) // fail trial
                    {
                        DoAction(ACTION_FAIL_TRIAL);
                        return;
                    }

                    if (++waveId > 8) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 3:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_AMBER_WEAVER);
                            break;
                        case 4:
                            me->SummonCreature(NPC_BERSERKING, berserkSpawnPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                        case 5:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_MYSTIC);
                            break;
                        case 7:
                            me->SummonCreature(NPC_BERSERKING, berserkSpawnPos[1], TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                    }

                    // Clear corpses before set new
                    summons.DespawnAll();

                    auto itr = invSilverTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second, TEMPSUMMON_MANUAL_DESPAWN);

                    currentTimer += 5 * IN_MILLISECONDS;

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 8, ChallengeMedals::MedalTypeSilver, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_DD_GOLD_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_DD_GOLD;

                if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                    rotun->AI()->Talk(TALK_TRIAL_BEGINNING_GOLD);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 30 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (!illusionGUIDs.empty()) // fail trial
                    {
                        DoAction(ACTION_FAIL_TRIAL);
                        return;
                    }

                    if (++waveId > 10) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    summons.DespawnAll();

                    auto itr = invGoldTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second, TEMPSUMMON_MANUAL_DESPAWN);

                    currentTimer += 5 * IN_MILLISECONDS;

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 10, ChallengeMedals::MedalTypeGold, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 2:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_BANANA_TOSSER);
                            break;
                        case 4:
                            me->SummonCreature(NPC_BERSERKING, berserkSpawnPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                        case 5:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_BANSHEE);

                            scheduler
                                .Schedule(Milliseconds(currentTimer - 10 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_15);
                            });

                            scheduler
                                .Schedule(Milliseconds(currentTimer - 25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_15);
                            });

                            scheduler
                                .Schedule(Milliseconds(currentTimer - 40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_10);
                            });
                            break;
                        case 7:
                            me->SummonCreature(NPC_BERSERKING, berserkSpawnPos[1], TEMPSUMMON_MANUAL_DESPAWN);

                            scheduler
                                .Schedule(Milliseconds(currentTimer - 45 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_18);
                            });

                            scheduler
                                .Schedule(Milliseconds(currentTimer - 20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_LARGE_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_30);
                            });
                            break;
                        case 8:
                            if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                                rotun->AI()->Talk(TALK_ILLUSIONARY_SHA);
                            break;
                        case 9:
                            scheduler
                                .Schedule(Milliseconds(currentTimer - 50 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_10);
                            });

                            scheduler
                                .Schedule(Milliseconds(currentTimer - 75 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                if (Creature* banshe = me->SummonCreature(NPC_LARGE_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                                    banshe->AI()->SetData(TIMER_DATA, SPELL_TIMER_25);
                            });
                            break;
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_TANK_BRONZE_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_TANK_BRONZE;

                if (me->GetInstanceScript())
                {
                    me->GetInstanceScript()->SendChallengeStartTimer(10);
                    me->GetInstanceScript()->SetData(BARRICADE_DATA, IN_PROGRESS);
                }

                if (Creature* sikari = me->SummonCreature(NPC_SIKARI_THE_MISTWEAVER, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                    sikari->AI()->Talk(TALK_SIKARI_BRONZE_INTRO);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 30 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (++waveId > 5) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invTankBronzeTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                        sikari->SetInCombatWithZone();

                    currentTimer += 10 * IN_MILLISECONDS;

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 5, ChallengeMedals::MedalTypeBronze, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 1:
                            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                                sikari->AI()->Talk(TALK_ILLUSIONARY_RIPPER);
                            break;
                        case 3:
                            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                                sikari->AI()->Talk(TALK_ILLUSIONARY_FORAGER);
                            break;
                        case 5:
                            scheduler
                                .Schedule(Milliseconds(30 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                // Spawn Illusion for current wave
                                for (auto&& itr : invTankBronzeAdditionalWave5)
                                    me->SummonCreature(itr.first, itr.second.GetPositionX() + frand(-2.5f, 2.5f), itr.second.GetPositionY() + frand(-2.5f, 2.5f), itr.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_TANK_SILVER_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_TANK_SILVER;

                if (me->GetInstanceScript())
                {
                    me->GetInstanceScript()->SendChallengeStartTimer(10);
                    me->GetInstanceScript()->SetData(BARRICADE_DATA, IN_PROGRESS);
                }

                if (Creature* sikari = me->SummonCreature(NPC_SIKARI_THE_MISTWEAVER, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                    sikari->AI()->Talk(TALK_SIKARI_SILVER_INTRO);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 40 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (++waveId > 8) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invTankSilverTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                        sikari->SetInCombatWithZone();

                    if (waveId > 3)
                        currentTimer += 5 * IN_MILLISECONDS;

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 8, ChallengeMedals::MedalTypeSilver, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 3:
                            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                                sikari->AI()->Talk(TALK_ILLUSIONARY_FLAMECALLER);
                            break;
                        case 5:
                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                // Spawn Illusion for current wave
                                for (auto&& itr : invTankSilverAdditionalWave5)
                                    me->SummonCreature(itr.first, itr.second.GetPositionX() + frand(-2.5f, 2.5f), itr.second.GetPositionY() + frand(-2.5f, 2.5f), itr.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 6:
                            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                                sikari->AI()->Talk(TALK_ILLUSIONARY_WIND_GUARD);
                            break;
                        case 8:
                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                // Spawn Illusion for current wave
                                for (auto&& itr : invTankSilverAdditionalWave8)
                                    me->SummonCreature(itr.first, itr.second.GetPositionX() + frand(-2.5f, 2.5f), itr.second.GetPositionY() + frand(-2.5f, 2.5f), itr.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_TANK_GOLD_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_TANK_GOLD;

                if (me->GetInstanceScript())
                {
                    me->GetInstanceScript()->SendChallengeStartTimer(10);
                    me->GetInstanceScript()->SetData(BARRICADE_DATA, IN_PROGRESS);
                }

                if (Creature* sikari = me->SummonCreature(NPC_SIKARI_THE_MISTWEAVER, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                    sikari->AI()->Talk(TALK_SIKARI_GOLD_INTRO);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 40 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (++waveId > 10) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invTankGoldTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                        sikari->SetInCombatWithZone();

                    switch (waveId)
                    {
                        case 4:
                        case 5:
                            currentTimer = 50 * IN_MILLISECONDS;
                            break;
                        case 6:
                            currentTimer = 60 * IN_MILLISECONDS;
                            break;
                        case 7:
                            currentTimer = 70 * IN_MILLISECONDS;
                            break;
                        case 8:
                            currentTimer = 80 * IN_MILLISECONDS;
                            break;
                        case 9:
                            currentTimer = 110 * IN_MILLISECONDS;
                            break;
                        case 10:
                            currentTimer = 120 * IN_MILLISECONDS;
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 10, ChallengeMedals::MedalTypeGold, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 3:
                            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                                sikari->AI()->Talk(TALK_ILLUSIONARY_AMBUSHER);

                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[3], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 4:
                            scheduler
                                .Schedule(Milliseconds(5 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 5:
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_WIND_GUARD, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[1], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 6:
                            if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                                sikari->AI()->Talk(TALK_ILLUSIONARY_CONQUEROR);
                            break;
                        case 7:
                            scheduler
                                .Schedule(Milliseconds(35 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[3], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 8:
                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 9:
                            scheduler
                                .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_WIND_GUARD, innerPillarsPos[0].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_CONQUEROR, innerPillarsPos[0].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(50 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(60 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 10:
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_WIND_GUARD, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_CONQUEROR, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_CONQUEROR, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(60 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[2].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[2].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_HEALER_BRONZE_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_HEAL_BRONZE;

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                // Summon our party
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : invHealerTrialFriends)
                    {
                        // No way to creature group with creature right now (available only for Players)
                        if (Creature* healerMember = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (me->GetInstanceScript())
                                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, healerMember);
                        }
                    }

                    // For self
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 25 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (++waveId > 5) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invHealerBronzeTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 1:
                            if (Creature* ki = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KI_THE_ASSASSIN) : ObjectGuid::Empty))
                                ki->AI()->Talk(TALK_ILLUSIONARY_RIPPER_H);
                            break;
                        case 2:
                            currentTimer += 5 * IN_MILLISECONDS;
                            break;
                        case 3:
                            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
                                oto->AI()->Talk(TALK_ILLUSIONARY_HIVE_SINGER);

                            currentTimer += 15 * IN_MILLISECONDS;
                            break;
                        case 4:
                            currentTimer += 15 * IN_MILLISECONDS;
                            break;
                        case 5:
                            currentTimer += 30 * IN_MILLISECONDS;

                            // Use time warp on group at last wave
                            if (Creature* kavan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KAVAN_THE_ARCANIST) : ObjectGuid::Empty))
                                kavan->CastSpell(kavan, SPELL_TIME_WARP, true);

                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                // Spawn Illusion for current wave
                                for (uint8 i = 0; i < 2; i++)
                                    me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 5, ChallengeMedals::MedalTypeBronze, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_HEALER_SILVER_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_HEAL_SILVER;

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                // Summon our party
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : invHealerTrialFriends)
                    {
                        // No way to creature group with creature right now (available only for Players)
                        if (Creature* healerMember = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (me->GetInstanceScript())
                                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, healerMember);
                        }
                    }

                    // For self
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 35 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (++waveId > 8) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invHealerSilverTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 3:
                            currentTimer += 5 * IN_MILLISECONDS;

                            if (Creature* ki = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KI_THE_ASSASSIN) : ObjectGuid::Empty))
                                ki->AI()->Talk(TALK_ILLUSIONARY_FLAMECALLER_H);
                            break;
                        case 4:
                            currentTimer += 10 * IN_MILLISECONDS;
                            break;
                        case 5:
                            currentTimer += 10 * IN_MILLISECONDS;

                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_FLAMECALLER_H, innerPillarsPos[1], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 6:
                            currentTimer -= 5 * IN_MILLISECONDS;

                            if (Creature* kavan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KAVAN_THE_ARCANIST) : ObjectGuid::Empty))
                                kavan->AI()->Talk(TALK_ILLUSIONARY_AQUALYTE);
                            break;
                        case 7:
                            currentTimer += 5 * IN_MILLISECONDS;
                            break;
                        case 8:
                            currentTimer += 5 * IN_MILLISECONDS;

                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                // Use time warp on group at last wave
                                if (Creature* kavan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KAVAN_THE_ARCANIST) : ObjectGuid::Empty))
                                    kavan->CastSpell(kavan, SPELL_TIME_WARP, true);
                            });

                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_FLAMECALLER_H, innerPillarsPos[1], TEMPSUMMON_MANUAL_DESPAWN);

                                for (uint8 i = 0; i < 2; i++)
                                    me->SummonCreature(NPC_SMALL_ILLUSIONARY_RIPPER_H, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 8, ChallengeMedals::MedalTypeSilver, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_HEALER_GOLD_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_HEAL_GOLD;

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                // Summon our party
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : invHealerTrialFriends)
                    {
                        // No way to creature group with creature right now (available only for Players)
                        if (Creature* healerMember = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (me->GetInstanceScript())
                                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, healerMember);
                        }
                    }

                    // For self
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 35 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (++waveId > 10) // complete trial
                    {
                        DoAction(ACTION_COMPLETE_TRIAL);
                        return;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invHealerGoldTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 2:
                            currentTimer += 10 * IN_MILLISECONDS;
                            break;
                        case 3:
                            currentTimer -= 5 * IN_MILLISECONDS;

                            if (Creature* sooli = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SOOLI_THE_SURVIVALIST) : ObjectGuid::Empty))
                                sooli->AI()->Talk(TALK_ILLUSIONARY_TUNNELER);
                            break;
                        case 4:
                            currentTimer += 15 * IN_MILLISECONDS;
                            break;
                        case 5:
                            currentTimer += 10 * IN_MILLISECONDS;

                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_FLAMECALLER_H, innerPillarsPos[1], TEMPSUMMON_MANUAL_DESPAWN);

                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 6:
                            currentTimer -= 10 * IN_MILLISECONDS;

                            if (Creature* oto = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_OTO_THE_PROTECTOR) : ObjectGuid::Empty))
                                oto->AI()->Talk(TALK_ILLUSIONARY_CONQUEROR_H);
                            break;
                        case 7:
                            currentTimer += 10 * IN_MILLISECONDS;
                            break;
                        case 9:
                            currentTimer += 15 * IN_MILLISECONDS;

                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AQUALYTE, innerPillarsPos[3], TEMPSUMMON_MANUAL_DESPAWN);

                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[3].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[3].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[3].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 10:
                            currentTimer += 10 * IN_MILLISECONDS;

                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AQUALYTE, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);

                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_FLAMECALLER_H, innerPillarsPos[4].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_CONQUEROR_H, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);

                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[4].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });

                            scheduler
                                .Schedule(Milliseconds(45 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                // Use time warp on group at last wave
                                if (Creature* kavan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KAVAN_THE_ARCANIST) : ObjectGuid::Empty))
                                    kavan->CastSpell(kavan, SPELL_TIME_WARP, true);
                            });
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(waveId, 10, ChallengeMedals::MedalTypeGold, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_DD_ENDLESS_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_DD_ENDLESS;

                if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                    rotun->AI()->Talk(TALK_TRIAL_BEGINNING_GOLD);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 30 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    if (!illusionGUIDs.empty()) // fail trial
                    {
                        DoAction(ACTION_FAIL_TRIAL); // complete trial on fail there (ofc started within 10td wave)
                        return;
                    }

                    endlessWaveId++;

                    if (++waveId > 10) // re-combine trial with upscale
                    {
                        waveId = 1;
                        currentTimer = 30 * IN_MILLISECONDS;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    summons.DespawnAll();

                    auto itr = invGoldTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second, TEMPSUMMON_MANUAL_DESPAWN);

                    currentTimer += 5 * IN_MILLISECONDS;

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 5:
                            scheduler
                                .Schedule(Milliseconds(currentTimer - 10 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 7:
                            scheduler
                                .Schedule(Milliseconds(currentTimer - 45 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(currentTimer - 20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 9:
                            scheduler
                                .Schedule(Milliseconds(currentTimer - 50 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_BANSHE, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(endlessWaveId, 0, ChallengeMedals::MaxMedalType, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_TANK_ENDLESS_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_TANK_ENDLESS;

                if (me->GetInstanceScript())
                {
                    me->GetInstanceScript()->SendChallengeStartTimer(10);
                    me->GetInstanceScript()->SetData(BARRICADE_DATA, IN_PROGRESS);
                }

                if (Creature* sikari = me->SummonCreature(NPC_SIKARI_THE_MISTWEAVER, arenaCenterPos, TEMPSUMMON_MANUAL_DESPAWN))
                    sikari->AI()->Talk(TALK_SIKARI_GOLD_INTRO);

                // For self
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 40 * IN_MILLISECONDS;
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    endlessWaveId++;

                    if (++waveId > 10) // re-combine trial with upscale
                    {
                        waveId = 1;
                        currentTimer = 40 * IN_MILLISECONDS;
                    }

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invTankGoldTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                        sikari->SetInCombatWithZone();

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 3:
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[3], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 4:
                            currentTimer = 50 * IN_MILLISECONDS;
                        
                            scheduler
                                .Schedule(Milliseconds(5 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 5:
                            currentTimer = 50 * IN_MILLISECONDS;
                        
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_WIND_GUARD, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[1], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 6:
                            currentTimer += 10 * IN_MILLISECONDS;
                            break;
                        case 7:
                            currentTimer += 10 * IN_MILLISECONDS;
                        
                            scheduler
                                .Schedule(Milliseconds(35 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[3], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 8:
                            currentTimer += 10 * IN_MILLISECONDS;
                        
                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[0], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 9:
                            currentTimer += 30 * IN_MILLISECONDS;
                        
                            scheduler
                                .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_WIND_GUARD, innerPillarsPos[0].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_CONQUEROR, innerPillarsPos[0].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[0].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(50 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(60 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 10:
                            currentTimer += 10 * IN_MILLISECONDS;
                        
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_WIND_GUARD, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_CONQUEROR, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_CONQUEROR, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(25 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AMBUSHER, innerPillarsPos[5], TEMPSUMMON_MANUAL_DESPAWN);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(60 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[2].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                                me->SummonCreature(NPC_LARGE_ILLUSIONARY_AMBUSHER, innerPillarsPos[2].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[2].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(endlessWaveId, 0, ChallengeMedals::MaxMedalType, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_HEALER_ENDLESS_TRIAL:
                if (hasTrialStarted) // prevent multi-calling
                    return;

                hasTrialStarted = true;
                trialId = TRIAL_HEAL_ENDLESS;

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStartTimer(10);

                // Summon our party
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                {
                    for (auto&& itr : invHealerTrialFriends)
                    {
                        // No way to creature group with creature right now (available only for Players)
                        if (Creature* healerMember = me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            if (me->GetInstanceScript())
                                me->GetInstanceScript()->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, healerMember);
                        }
                    }

                    // For self
                    for (auto&& itr : raidBuffs)
                        trialOwner->CastSpell(trialOwner, itr, true);

                    sScenarioMgr->SendScenarioState(trialOwner, 1148, 0, 0, 33614, 248);
                }

                illusionGUIDs.clear();
                waveId = 0;
                currentTimer = 60 * IN_MILLISECONDS; // static timer for endless here
                scheduler
                    .Schedule(Milliseconds(10 * IN_MILLISECONDS), [this](TaskContext context)
                {
                    endlessWaveId++;

                    if (++waveId > 10) // re-combine trial with upscale
                        waveId = 1;

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Clear corpses before set new
                    for (auto&& itr : illusionGUIDs)
                        if (Creature* illusion = ObjectAccessor::GetCreature(*me, itr))
                            if (!illusion->IsAlive())
                                illusion->DespawnOrUnsummon();

                    auto itr = invHealerGoldTrial.find(waveId);

                    // Spawn Illusion for current wave
                    for (auto&& illusions : itr->second)
                        me->SummonCreature(illusions.first, illusions.second.GetPositionX() + frand(-2.5f, 2.5f), illusions.second.GetPositionY() + frand(-2.5f, 2.5f), illusions.second.GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                    // Special Announce about each of illusion
                    switch (waveId)
                    {
                        case 5:
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_FLAMECALLER_H, innerPillarsPos[1], TEMPSUMMON_MANUAL_DESPAWN);
                        
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[1].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[1].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 9:
                            scheduler
                                .Schedule(Milliseconds(15 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AQUALYTE, innerPillarsPos[3], TEMPSUMMON_MANUAL_DESPAWN);
                        
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[3].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[3].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[3].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                        case 10:
                            scheduler
                                .Schedule(Milliseconds(20 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_AQUALYTE, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                        
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_FLAMECALLER_H, innerPillarsPos[4].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);

                                // Use time warp on group at last wave
                                if (Creature* kavan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_KAVAN_THE_ARCANIST) : ObjectGuid::Empty))
                                    kavan->CastSpell(kavan, SPELL_TIME_WARP, true);
                            });
                        
                            scheduler
                                .Schedule(Milliseconds(40 * IN_MILLISECONDS), [this](TaskContext context)
                            {
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_CONQUEROR_H, innerPillarsPos[4], TEMPSUMMON_MANUAL_DESPAWN);
                        
                                me->SummonCreature(NPC_SMALL_ILLUSIONARY_HIVE_SINGER, innerPillarsPos[4].GetPositionX() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionY() + frand(-2.5f, 2.5f), innerPillarsPos[4].GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN);
                            });
                            break;
                    }

                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    {
                        ScenarioChallenge waveData = ScenarioChallenge(endlessWaveId, 0, ChallengeMedals::MaxMedalType, currentTimer / IN_MILLISECONDS);
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, 1, 0, 33614, 248, 0, &waveData);
                    }

                    context.Repeat(Milliseconds(currentTimer));
                });
                break;
            case ACTION_FAIL_TRIAL:
                if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                    rotun->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                    sikari->DespawnOrUnsummon();

                for (auto&& itr : invHealerTrialFriends)
                    if (Creature* healerMember = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr.first) : ObjectGuid::Empty))
                        healerMember->DespawnOrUnsummon();

                if (me->GetInstanceScript())
                    me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                if (GetData(TRIAL_DATA) > TRIAL_TANK_GOLD && me->GetInstanceScript()) // complete endless trial on fail
                    me->GetInstanceScript()->SetData(DATA_TRIAL, DONE);
                else if (me->GetInstanceScript())
                    me->GetInstanceScript()->SetData(DATA_TRIAL, FAIL);

                // Repeat Scenario
                if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                    sScenarioMgr->SendScenarioState(trialOwner, 1148, DATA_SELECT_A_TRIAL, 0);

                Reset();
                break;
            case ACTION_COMPLETE_TRIAL:
                if (Creature* rotun = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_TRIAL_MASTER_ROTUN) : ObjectGuid::Empty))
                {
                    rotun->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(DATA_TRIAL, DONE);

                    if (Creature* sikari = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SIKARI_THE_MISTWEAVER) : ObjectGuid::Empty))
                        sikari->DespawnOrUnsummon();

                    for (auto&& itr : invHealerTrialFriends)
                        if (Creature* healerMember = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(itr.first) : ObjectGuid::Empty))
                            healerMember->DespawnOrUnsummon(3 * IN_MILLISECONDS);

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SendChallengeStopElapsedTimer(TIMER_SCENARIO);

                    // Repeat Scenario
                    if (Player* trialOwner = ObjectAccessor::GetPlayer(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(PLAYER_DATA) : ObjectGuid::Empty))
                        sScenarioMgr->SendScenarioState(trialOwner, 1148, DATA_SELECT_A_TRIAL, 0);
                }

                Reset();
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case TRIAL_DATA:
                return trialId;
            case WAVE_DATA:
                return endlessWaveId ? endlessWaveId : waveId;
        }

        return 0;
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() != NPC_SIKARI_THE_MISTWEAVER && summon->GetEntry() != NPC_OTO_THE_PROTECTOR && summon->GetEntry() != NPC_SOOLI_THE_SURVIVALIST && summon->GetEntry() != NPC_KAVAN_THE_ARCANIST && summon->GetEntry() != NPC_KI_THE_ASSASSIN && summon->GetEntry() != NPC_BERSERKING)
        {
            summons.Summon(summon);
            illusionGUIDs.push_back(summon->GetGUID());
        }
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (std::find(illusionGUIDs.begin(), illusionGUIDs.end(), summon->GetGUID()) != illusionGUIDs.end())
            illusionGUIDs.erase(std::find(illusionGUIDs.begin(), illusionGUIDs.end(), summon->GetGUID()));
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Sikari the Mistweaver 71828
struct npc_proving_grounds_sikari_the_mistweaver : public customCreatureAI
{
    npc_proving_grounds_sikari_the_mistweaver(Creature* creature) : customCreatureAI(creature)
    { 
        SetCombatMovement(false);
    }

    TaskScheduler scheduler;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        events.Reset();
        scheduler.CancelAll();
    }

    void JustEngagedWith(Unit* who) override
    {
        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            if (!me->HasUnitState(UNIT_STATE_CASTING))
            {
                if (Unit* target = me->GetVictim())
                {
                    if (getTotalVictims() > 3)
                        DoCast(target, SPELL_CRACKLING_JADE_THUNDER);
                    else
                        DoCast(target, SPELL_FOCUSED_CHI_BURST);
                }
            }

            context.Repeat(Seconds(2));
        });

        scheduler
            .Schedule(Seconds(6), [this](TaskContext context)
        {
            DoCast(me, SPELL_RENEWING_MIST, true);
            context.Repeat(Seconds(16));
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* controller = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
            controller->AI()->DoAction(ACTION_FAIL_TRIAL);
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;
    }

    private:
        uint32 getTotalVictims()
        {
            uint32 count = 0;
            for (auto&& itr : me->GetThreatManager().getThreatList())
                if (Creature* target = ObjectAccessor::GetCreature(*me, itr->getUnitGuid()))
                    count++;

            return count;
        }
};

// Trial Master Rotun Queue 72536
class npc_proving_grounds_trial_master_rotun_queue : public CreatureScript
{
    public:
        npc_proving_grounds_trial_master_rotun_queue() : CreatureScript("npc_proving_grounds_trial_master_rotun_queue") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                lfg::LfgDungeonSet scenario = { 658 };
                sLFGMgr->JoinLfg(player, lfg::PLAYER_ROLE_DAMAGE, scenario, "");
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (!player->GetGroup() && !player->IsUsingLfg())
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
};

// Berserking 73333
struct npc_proving_grounds_berserking : public customCreatureAI
{
    npc_proving_grounds_berserking(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;

    void Reset() override
    {
        scheduler.CancelAll();

        scheduler
            .Schedule(Seconds(1), [this](TaskContext context)
        {
            DoCast(me, SPELL_BERSERKING_AT);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// Bananastorm 142627
class spell_proving_grounds_bananastorm_selector : public SpellScript
{
    PrepareSpellScript(spell_proving_grounds_bananastorm_selector);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (GetCaster() && GetCaster()->HasAura(SPELL_AMBER_GLOBULE_DETONATION)) // prevent if stunned in amber globule
            return;

        if (Unit* target = GetHitUnit())
            GetCaster()->CastSpell(target, SPELL_BANANASTORM_MISSLE, true);
    }

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && GetCaster()->GetExactDist2d(target->ToUnit()) < 10.0f; });
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_proving_grounds_bananastorm_selector::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_proving_grounds_bananastorm_selector::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// Banshe Timer 141579, 141580, 141580, 141581, 141582, 141583, 141584, 141557
class spell_proving_grounds_banshe_timer : public AuraScript
{
    PrepareAuraScript(spell_proving_grounds_banshe_timer);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
            if (Creature* controller = ObjectAccessor::GetCreature(*owner, owner->GetInstanceScript() ? owner->GetInstanceScript()->GetGuidData(NPC_PROVING_GROUNDS_2) : ObjectGuid::Empty))
                controller->AI()->DoAction(ACTION_FAIL_TRIAL);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_proving_grounds_banshe_timer::OnAuraEffectRemove, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Amber Globule Detonation 142191
class spell_proving_grounds_amber_globule_detonation : public SpellScript
{
    PrepareSpellScript(spell_proving_grounds_amber_globule_detonation);

    void SelectTargets(std::list<WorldObject*>&targets)
    {
        targets.remove_if([=](WorldObject* target) { return target && target->ToUnit() && !target->ToPlayer() && !IsIllusion(target->ToUnit()); });
    }

    private:
        bool IsIllusion(Unit* target)
        {
            for (auto&& itr : illusions)
                if (target->HasAura(itr))
                    return true;

            return false;
        }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_proving_grounds_amber_globule_detonation::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_proving_grounds_amber_globule_detonation::SelectTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Protection Shield 142427
class spell_proving_grounds_protection_shield : public AuraScript
{
    PrepareAuraScript(spell_proving_grounds_protection_shield);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_EXPOSED, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_proving_grounds_protection_shield::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Exposed 142489
class spell_proving_grounds_exposed : public AuraScript
{
    PrepareAuraScript(spell_proving_grounds_exposed);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0)
            return;

        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_PROTECTION_SHIELD, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_proving_grounds_exposed::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
    }
};

// Aqua Bomb 145206
class spell_proving_grounds_aqua_bomb : public AuraScript
{
    PrepareAuraScript(spell_proving_grounds_aqua_bomb);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (aurEff->GetBase()->GetDuration() > 0) // explode only if not dispeled
            return;

        if (Unit* caster = GetCaster())
            if (Unit* target = GetOwner()->ToUnit())
                caster->CastSpell(target, SPELL_AQUA_BOMB_EFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_proving_grounds_aqua_bomb::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Renewing Mist 144080
class spell_proving_grounds_renewing_mist : public AuraScript
{
    PrepareAuraScript(spell_proving_grounds_renewing_mist);

    void CalculateTick(AuraEffect const*, int32& damage)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            damage = int32(owner->GetMaxHealth() * 0.1f);
    }

    void HandleEffectCalcAmount(AuraEffect const* /*aurEff*/, float& amount, bool& canBeRecalculated)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            amount = int32(owner->GetMaxHealth() * 0.1f);

        canBeRecalculated = true;
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_proving_grounds_renewing_mist::HandleEffectCalcAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
        DoEffectCalcTickAmount += AuraEffectCalcTickAmountFn(spell_proving_grounds_renewing_mist::CalculateTick, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
    }
};

// Wing Blast 144106
class spell_proving_grounds_wing_blast : public SpellScript
{
    PrepareSpellScript(spell_proving_grounds_wing_blast);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Player* affectedTarget = GetHitPlayer())
            GetCaster()->GetThreatManager().modifyThreatPercent(affectedTarget, -100);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_proving_grounds_wing_blast::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// Spell Chomp 145263
class spell_proving_grounds_chomp : public AuraScript
{
    PrepareAuraScript(spell_proving_grounds_chomp);

    void OnUpdate(uint32 /*diff*/, AuraEffect* aurEff)
    {
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->HealthAbovePct(90))
            GetOwner()->ToUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        OnEffectUpdate += AuraEffectUpdateFn(spell_proving_grounds_chomp::OnUpdate, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE);
    }
};

// 1281. Summoned by 147292 - Volatile Amber Globule
class sat_proving_grounds_volatile_amber_globule : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToUnit() && (object->ToPlayer() || IsIllusion(object->ToUnit())) && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 8500;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Unit* itr = target->ToUnit())
            if (Unit* caster = GetCaster())
                caster->CastSpell(caster, SPELL_AMBER_GLOBULE_DETONATION, true);
    }

    private:
        bool IsIllusion(Unit* target)
        {
            for (auto&& itr : illusions)
                if (target->HasAura(itr))
                    return true;

            return false;
        }
};

// 1067. Summoned by 144377 - Invoke Lava
class sat_proving_grounds_invoke_lava : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToUnit() && (triggering->ToPlayer() && triggering->ToPlayer()->IsAlive() || IsIllusion(triggering->ToUnit()));
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* itr = triggering->ToUnit())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_INVOKE_LAVA_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Unit* itr = triggering->ToUnit())
            itr->RemoveAurasDueToSpell(SPELL_INVOKE_LAVA_EFF);
    }

    private:
        bool IsIllusion(Unit* target)
        {
            for (auto&& itr : tankTrialIllusions)
                if (target->HasAura(itr))
                    return true;

            return false;
        }
};

// 1282. Summoned by 147294 - Berserking
class sat_proving_grounds_berserking : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive();
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Unit* itr = target->ToUnit())
            if (Unit* caster = GetCaster())
                caster->CastSpell(itr, SPELL_BERSERKING_EFF, true);
    }
};

void AddSC_proving_grounds()
{
    new creature_script<npc_proving_grounds_base>("npc_proving_grounds_base");
    new creature_script<npc_proving_grounds_illusionary_guardian>("npc_proving_grounds_illusionary_guardian");
    new creature_script<npc_proving_grounds_illusionary_amber_weaver>("npc_proving_grounds_illusionary_amber_weaver");
    new creature_script<npc_proving_grounds_illusionary_mystic>("npc_proving_grounds_illusionary_mystic");
    new creature_script<npc_proving_grounds_illusionary_banana_tosser>("npc_proving_grounds_illusionary_banana_tosser");
    new creature_script<npc_proving_grounds_illusionary_banshe>("npc_proving_grounds_illusionary_banshe");
    new creature_script<npc_proving_grounds_illusionary_sha>("npc_proving_grounds_illusionary_sha");
    new creature_script<npc_proving_grounds_illusionary_forager>("npc_proving_grounds_illusionary_forager");
    new creature_script<npc_proving_grounds_illusionary_flamecaller>("npc_proving_grounds_illusionary_flamecaller");
    new creature_script<npc_proving_grounds_illusionary_wind_guard>("npc_proving_grounds_illusionary_wind_guard");
    new creature_script<npc_proving_grounds_illusionary_ambusher>("npc_proving_grounds_illusionary_ambusher");
    new creature_script<npc_proving_grounds_illusionary_conqueror>("npc_proving_grounds_illusionary_conqueror");
    new creature_script<npc_proving_grounds_illusionary_hive_singer>("npc_proving_grounds_illusionary_hive_singer");
    new creature_script<npc_proving_grounds_illusionary_flamecaller_healer>("npc_proving_grounds_illusionary_flamecaller_healer");
    new creature_script<npc_proving_grounds_illusionary_aqualyte>("npc_proving_grounds_illusionary_aqualyte");
    new creature_script<npc_proving_grounds_illusionary_tunneler>("npc_proving_grounds_illusionary_tunneler");
    new creature_script<npc_proving_grounds_illusionary_conqueror_healer>("npc_proving_grounds_illusionary_conqueror_healer");
    new creature_script<npc_proving_grounds_kavan_the_arcanist>("npc_proving_grounds_kavan_the_arcanist");
    new creature_script<npc_proving_grounds_ki_the_assassin>("npc_proving_grounds_ki_the_assassin");
    new creature_script<npc_proving_grounds_oto_the_protector>("npc_proving_grounds_oto_the_protector");
    new creature_script<npc_proving_grounds_sooli_the_survivalist>("npc_proving_grounds_sooli_the_survivalist");
    new npc_proving_grounds_trial_master_rotun();
    new creature_script<npc_proving_grounds_controller>("npc_proving_grounds_controller");
    new creature_script<npc_proving_grounds_sikari_the_mistweaver>("npc_proving_grounds_sikari_the_mistweaver");
    new npc_proving_grounds_trial_master_rotun_queue();
    new creature_script<npc_proving_grounds_berserking>("npc_proving_grounds_berserking");

    new spell_script<spell_proving_grounds_bananastorm_selector>("spell_proving_grounds_bananastorm_selector");
    new aura_script<spell_proving_grounds_banshe_timer>("spell_proving_grounds_banshe_timer");
    new spell_script<spell_proving_grounds_amber_globule_detonation>("spell_proving_grounds_amber_globule_detonation");
    new aura_script<spell_proving_grounds_protection_shield>("spell_proving_grounds_protection_shield");
    new aura_script<spell_proving_grounds_exposed>("spell_proving_grounds_exposed");
    new aura_script<spell_proving_grounds_aqua_bomb>("spell_proving_grounds_aqua_bomb");
    new aura_script<spell_proving_grounds_renewing_mist>("spell_proving_grounds_renewing_mist");
    new spell_script<spell_proving_grounds_wing_blast>("spell_proving_grounds_wing_blast");
    new aura_script<spell_proving_grounds_chomp>("spell_proving_grounds_chomp");
    new atrigger_script<sat_proving_grounds_volatile_amber_globule>("sat_proving_grounds_volatile_amber_globule");
    new atrigger_script<sat_proving_grounds_invoke_lava>("sat_proving_grounds_invoke_lava");
    new atrigger_script<sat_proving_grounds_berserking>("sat_proving_grounds_berserking");
}