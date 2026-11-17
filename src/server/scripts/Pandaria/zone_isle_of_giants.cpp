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

enum Spells
{
    SPELL_CRUSH              = 138765,
    SPELL_SKYCALL            = 138817,
    SPELL_DINO_MENDING       = 138770,
    SPELL_DINO_FORM          = 138771,
    SPELL_TRIPPLE_PUNCTURE   = 138768,
    SPELL_TRIHORN_CHARGE     = 138769,
    SPELL_TRIHORN_CHARGE_EFF = 138777,
    SPELL_DOUBLE_SWIPE       = 138772,
};

enum Events
{
    EVENT_CRUSH = 1,
    EVENT_SKYCALL,
    EVENT_DINO_MENDING,
    EVENT_TRIPPLE_PUNCTURE,
    EVENT_TRIHORN_CHARGE,
    EVENT_DOUBLE_SWIPE,
};

// Young Primal Devilsaur 70005
struct npc_young_primal_devilsaur : public customCreatureAI
{
    npc_young_primal_devilsaur(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
        me->SetCombatDistance(5.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetCombatDistance(220.0f);
        events.ScheduleEvent(EVENT_SKYCALL, urand(3.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_SKYCALL, urand(7 * IN_MILLISECONDS, 13 * IN_MILLISECONDS), EVENT_SKYCALL, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Pterrorwing Skyscreamer 70020
struct npc_pterrorwing_skyscreamer : public customCreatureAI
{
    npc_pterrorwing_skyscreamer(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
        me->SetCombatDistance(5.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetCombatDistance(220.0f);
        events.ScheduleEvent(EVENT_CRUSH, 2.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_CRUSH, urand(4.5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS), EVENT_CRUSH, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Zandalari Dinomancer 69925
struct npc_zandalari_dinomancer_2 : public customCreatureAI
{
    npc_zandalari_dinomancer_2(Creature* creature) : customCreatureAI(creature) { }

    bool hasTriggered;

    void Reset() override
    {
        hasTriggered = false;
        events.Reset();
        me->RemoveAurasDueToSpell(SPELL_DINO_FORM);
        me->SetCombatDistance(5.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetCombatDistance(220.0f);
        events.ScheduleEvent(EVENT_DINO_MENDING, 8.5 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(60) && !hasTriggered && !me->HasUnitState(UNIT_STATE_CASTING))
        {
            hasTriggered = true;
            DoCast(me, SPELL_DINO_FORM, true);
        }
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
            ExecuteTargetEvent(SPELL_DINO_MENDING, urand(10.5 * IN_MILLISECONDS, 14.3 * IN_MILLISECONDS), EVENT_DINO_MENDING, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Primal Direhorn Hatchling 70014
struct npc_primal_direhorn_hatchling : public customCreatureAI
{
    npc_primal_direhorn_hatchling(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
        me->SetCombatDistance(5.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetCombatDistance(220.0f);
        events.ScheduleEvent(EVENT_TRIPPLE_PUNCTURE, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_TRIHORN_CHARGE, 8 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_TRIPPLE_PUNCTURE, urand(8.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_TRIPPLE_PUNCTURE, eventId);
            ExecuteTargetEvent(SPELL_TRIHORN_CHARGE, urand(12 * IN_MILLISECONDS, 25 * IN_MILLISECONDS), EVENT_TRIHORN_CHARGE, eventId, PRIORITY_NOT_VICTIM);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Primal Direhorn 70018
struct npc_primal_direhorn : public customCreatureAI
{
    npc_primal_direhorn(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
        me->SetCombatDistance(5.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetCombatDistance(220.0f);
        events.ScheduleEvent(EVENT_TRIPPLE_PUNCTURE, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_TRIHORN_CHARGE, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_DOUBLE_SWIPE, 6 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_TRIPPLE_PUNCTURE, urand(8.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_TRIPPLE_PUNCTURE, eventId);
            ExecuteTargetEvent(SPELL_TRIHORN_CHARGE, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS), EVENT_TRIHORN_CHARGE, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_DOUBLE_SWIPE, urand(12 * IN_MILLISECONDS, 14 * IN_MILLISECONDS), EVENT_DOUBLE_SWIPE, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Trihorn Charge 138769
class spell_trihorn_charge : public SpellScript
{
    PrepareSpellScript(spell_trihorn_charge);

    void HandleEffectHitTarget(SpellEffIndex /*eff_idx*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_TRIHORN_CHARGE_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_trihorn_charge::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_CHARGE);
    }
};

void AddSC_isle_of_giants()
{
    new creature_script<npc_young_primal_devilsaur>("npc_young_primal_devilsaur");
    new creature_script<npc_pterrorwing_skyscreamer>("npc_pterrorwing_skyscreamer");
    new creature_script<npc_zandalari_dinomancer_2>("npc_zandalari_dinomancer_2");
    new creature_script<npc_primal_direhorn_hatchling>("npc_primal_direhorn_hatchling");
    new creature_script<npc_primal_direhorn>("npc_primal_direhorn");
    new spell_script<spell_trihorn_charge>("spell_trihorn_charge");
}