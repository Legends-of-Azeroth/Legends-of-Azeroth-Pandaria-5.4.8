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
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "BattlegroundIC.h"
#include "Vehicle.h"
#include "CombatAI.h"
#include "Battleground.h"

enum bSpells
{
    SPELL_CRUSHING_LEAP     = 68506,
    SPELL_DAGGER_THROW_1    = 67280,
    SPELL_DAGGER_THROW_2    = 67881,
    SPELL_MORTAL_STRIKE     = 39171,
};

enum Yells
{
    YELL_AGGRO              = 0,
    YELL_EVADE              = 1
};

struct boss_isle_of_conquest : public ScriptedAI
{
    boss_isle_of_conquest(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        uiMortalStrikeTimer         = 8 * IN_MILLISECONDS;
        uiDaggerThrowTimer          = 2 * IN_MILLISECONDS;
        uiCrushingLeapTimer         = 6 * IN_MILLISECONDS;
        uiResetTimer                = 5 * IN_MILLISECONDS;
    }

    void JustEngagedWith(Unit* who) override
    {
        if (!me->IsWithinLOSInMap(who))
            EnterEvadeMode();

        //Talk(YELL_AGGRO);
    }

    void JustAppeared() override
    {
        Reset();
    }

    void UpdateAI(uint32 diff)override
    {
        if (!UpdateVictim())
            return;

        if (uiMortalStrikeTimer < diff)
        {
            DoCast(me->GetVictim(), SPELL_MORTAL_STRIKE);
            uiMortalStrikeTimer = urand(10 * IN_MILLISECONDS, 20 * IN_MILLISECONDS);
        } else uiMortalStrikeTimer -= diff;

        if (uiDaggerThrowTimer < diff)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                DoCast(target, SPELL_DAGGER_THROW_1);
            uiDaggerThrowTimer = urand(7 * IN_MILLISECONDS, 12 * IN_MILLISECONDS);
        } else uiDaggerThrowTimer -= diff;

        if (uiCrushingLeapTimer < diff)
        {
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                DoCast(target, SPELL_CRUSHING_LEAP);
            uiCrushingLeapTimer = urand(12 * IN_MILLISECONDS, 16 * IN_MILLISECONDS);
        } else uiCrushingLeapTimer -= diff;

        // check if creature is not outside of building
        if (uiResetTimer < diff)
        {
            float x,y;
            me->GetPosition(x,y);
            if (me->GetEntry() == NPC_OVERLORD_AGMAR && (x > 1348 || x < 1283 || y < -800 || y > -730))
            {
                me->Kill(me->GetVictim());
                EnterEvadeMode();
            }
            if (me->GetEntry() == NPC_HIGH_COMMANDER_HALFORD_WYRMBANE && (x > 288 || x < 216 || y < -863 || y > -800))
            {
                me->Kill(me->GetVictim());
                EnterEvadeMode();
            }
            uiResetTimer = 200;
        } else uiResetTimer -= diff;

        DoMeleeAttackIfReady();
    }

private:
    uint32 uiMortalStrikeTimer;
    uint32 uiDaggerThrowTimer;
    uint32 uiCrushingLeapTimer;
    uint32 uiResetTimer;
};

// TO-DO: This should be done with SmartAI, but yet it does not correctly support vehicles's AIs.
//        Even adding ReactState Passive we still have issues using SmartAI.
struct npc_four_car_garage : public VehicleAI
{
    npc_four_car_garage(Creature* creature) : VehicleAI(creature) { }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (apply)
        {
            uint32 spellId = 0;

            switch(me->GetEntry())
            {
                case NPC_DEMOLISHER:
                    spellId = SPELL_DRIVING_CREDIT_DEMOLISHER;
                    break;
                case NPC_GLAIVE_THROWER_A:
                case NPC_GLAIVE_THROWER_H:
                    spellId = SPELL_DRIVING_CREDIT_GLAIVE;
                    break;
                case NPC_SIEGE_ENGINE_H:
                case NPC_SIEGE_ENGINE_A:
                    spellId = SPELL_DRIVING_CREDIT_SIEGE;
                    break;
                case NPC_CATAPULT:
                    spellId = SPELL_DRIVING_CREDIT_CATAPULT;
                    break;
                default:
                    return;
            }

            me->CastSpell(who, spellId, true);
        }
    }

    void GetPassengerExitPosition(Unit* passenger, int8 seatId, Position& pos) override
    {
        if (me->GetEntry() == NPC_CATAPULT && passenger && seatId == 1)
        {
            // Eject the player to the location of missile targeting arc start, so that the player's knockback trajectory matches that of the arc
            pos.Relocate(me);
            pos.m_positionZ += 6.0f;
        }
    }
};

enum Events
{
    EVENT_TALK  = 1,
    EVENT_DESPAWN
};

enum Texts
{
    SAY_ONBOARD = 0
};

struct npc_ioc_gunship_captain : public ScriptedAI
{
    npc_ioc_gunship_captain(Creature* creature) : ScriptedAI(creature) { }

    void DoAction(int32 action) override
    {
        if (action == ACTION_GUNSHIP_READY)
        {
            DoCast(me, SPELL_SIMPLE_TELEPORT);
            _events.ScheduleEvent(EVENT_TALK, 3000);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_TALK:
                    _events.ScheduleEvent(EVENT_DESPAWN, 1000);
                    Talk(SAY_ONBOARD);
                    DoCast(me, SPELL_TELEPORT_VISUAL_ONLY);
                    break;
                case EVENT_DESPAWN:
                    if (me->GetMap()->ToBattlegroundMap())
                        if (Battleground* bgIoC = me->GetMap()->ToBattlegroundMap()->GetBG())
                            bgIoC->DelCreature(BG_IC_NPC_GUNSHIP_CAPTAIN_1);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
};

enum KeepCannonEnums
{
    SPELL_EJECT_ALL_PASSENGERS      = 50630,
    SPELL_PERMANENT_FEIGN_DEATH_2   = 114371,
    SPELL_PERMANENT_FEIGN_DEATH     = 29266,
    SPELL_REPAIR_CANNON             = 68077,
    SPELL_REPAIR_CANNON_TRIGGERED   = 68078,
    SPELL_FULL_HEAL                 = 43978,
    SPELL_FACING_SAVER              = 75925,

    NPC_BROKEN_KEEP_CANNON          = 35819,
};

struct npc_ioc_keep_cannon : public VehicleAI
{
    npc_ioc_keep_cannon(Creature* creature) : VehicleAI(creature) { }

    void DamageTaken(Unit* /*attacker*/, uint32& damage) override
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH))
            damage = 0;

        if (me->GetHealth() <= damage)
        {
            damage = me->GetHealth() - 1;
            DoCast(me, SPELL_EJECT_ALL_PASSENGERS, true);
            DoCast(me, SPELL_PERMANENT_FEIGN_DEATH_2, true);
            DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // Not used in sniffs
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            me->SetEntry(NPC_BROKEN_KEEP_CANNON); // Not Creature::UpdateEntry because we don't want its stats to change
        }
    }

    void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
    {
        if (spell->Id == SPELL_REPAIR_CANNON_TRIGGERED)
        {
            me->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH_2);
            me->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, false);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_MAX_HEALTH, false);
            DoCast(me, SPELL_FULL_HEAL, true);
            DoCast(me, SPELL_FACING_SAVER, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
            me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_MAX_HEALTH, true);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC); // Not used in sniffs
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC);
            me->SetEntry(me->GetOriginalEntry());
        }
    }
};

// 66630 - Alliance Gunship Portal
// 66637 - Horde Gunship Portal
class spell_ioc_gunship_portal : public SpellScript
{
    PrepareSpellScript(spell_ioc_gunship_portal);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (Battleground* bg = caster->GetBattleground())
            if (bg->GetTypeID(true) == BATTLEGROUND_IC)
                bg->DoAction(ACTION_TELEPORT_PLAYER_TO_TRANSPORT, caster->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ioc_gunship_portal::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66656 - Parachute
class spell_ioc_parachute_ic : public AuraScript
{
    PrepareAuraScript(spell_ioc_parachute_ic)

    void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();

        if (!target->ToPlayer())
            return;

        if (target->ToPlayer()->m_movementInfo.jump.fallTime > 2000)
            target->CastSpell(target, SPELL_PARACHUTE_IC, true);
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (target->GetTypeId() == TYPEID_PLAYER && target->HasUnitMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR) && !target->IsOnGround() && !target->HasAura(SPELL_PARACHUTE_IC))
                target->CastSpell(target, SPELL_PARACHUTE_IC, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_ioc_parachute_ic::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        OnEffectRemove += AuraEffectRemoveFn(spell_ioc_parachute_ic::HandleEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

enum Launch
{
    SPELL_LAUNCH_NO_FALLING_DAMAGE_AURA = 66227,
};

// 66218 - Launch
class spell_ioc_launch : public SpellScript
{
    PrepareSpellScript(spell_ioc_launch);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        SpellInfo const* const spell = GetSpellInfo();

        if (Player* player = GetHitPlayer())
        {
            GetCaster()->CastSpell(GetCaster(), SPELL_LAUNCH_NO_FALLING_DAMAGE_AURA, true);

            ObjectGuid vehicleGUID = GetCaster()->GetGUID();
            SpellCastTargets targets = GetSpell()->m_targets;
            player->m_Events.Schedule(1500, [player, vehicleGUID, targets]()
            {
                Unit* vehicle = ObjectAccessor::GetUnit(*player, vehicleGUID);
                if (!vehicle || !player->IsOnVehicle(vehicle) || vehicle->GetVehicleKit()->GetPassenger(1) != player)
                    return;

                player->ExitVehicle();
                player->GetMotionMaster()->MoveJump(targets.GetDstPos()->GetPosition(), targets.GetSpeedXY(), targets.GetSpeedZ());
            });
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_ioc_launch::HandleScript, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
    }
};

// 68077 - Repair Cannon
class spell_ioc_repair_cannon : public AuraScript
{
    PrepareAuraScript(spell_ioc_repair_cannon);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                caster->CastSpell(GetTarget(), SPELL_REPAIR_CANNON_TRIGGERED, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_ioc_repair_cannon::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_isle_of_conquest()
{
    new creature_script<boss_isle_of_conquest>("boss_isle_of_conquest");
    new creature_script<npc_four_car_garage>("npc_four_car_garage");
    new creature_script<npc_ioc_gunship_captain>("npc_ioc_gunship_captain");
    new creature_script<npc_ioc_keep_cannon>("npc_ioc_keep_cannon");
    new spell_script<spell_ioc_gunship_portal>("spell_ioc_gunship_portal");
    new aura_script<spell_ioc_parachute_ic>("spell_ioc_parachute_ic");
    new spell_script<spell_ioc_launch>("spell_ioc_launch");
    new aura_script<spell_ioc_repair_cannon>("spell_ioc_repair_cannon");
}
