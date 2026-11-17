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
#include "blackrock_caverns.h"

//todo: сделать спелл трансформации

enum ScriptTexts
{
    SAY_AGGRO                   = 0,
    SAY_KILL                    = 1,
    SAY_TRANSFORMATION          = 2,
    SAY_DEATH                   = 3,
};
enum Spells
{
    SPELL_STONEBLOW             = 76185,
    SPELL_TWILIGHT_CORRUPTION   = 76188,
    SPELL_THUNDERCLAP           = 76186,
    SPELL_TWITCHY               = 76167,
    SPELL_SHADOW_OF_OBSIDIUS    = 76164,
    SPELL_CREPUSCULAR_VEIL      = 76189,
};

enum Events
{
    EVENT_STONEBLOW             = 1,
    EVENT_TWILIGHT_CORRUPTION   = 2,
    EVENT_THUNDERCLAP           = 3,
    EVENT_CREPUSCULAR_VEIL      = 4,
};

enum Adds
{
    NPC_SHADOW_OF_OBSIDIUS  = 40817,
};

const Position shadowofobsidiusPos[3] = 
{
    {328.19f, 561.97f, 66.0f, 4.50f},
    {335.24f, 556.08f, 66.0f, 1.64f},
    {330.25f, 549.84f, 66.f, 3.10f}
};
class boss_ascendant_lord_obsidius : public CreatureScript
{
    public:
        boss_ascendant_lord_obsidius() : CreatureScript("boss_ascendant_lord_obsidius") { }
     
        struct boss_ascendant_lord_obsidiusAI : public BossAI
        {
            boss_ascendant_lord_obsidiusAI(Creature* creature) : BossAI(creature, DATA_ASCENDANT_LORD_OBSIDIUS)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->setActive(true);
            }

            void Reset() override
            {
                _Reset();
                summons.DespawnAll();
                for (uint8 i = (IsHeroic() ? 0 : 1); i < 3; i++)
                    shadows[i] = me->SummonCreature(NPC_SHADOW_OF_OBSIDIUS, shadowofobsidiusPos[i]);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->GetMap()->SetWorldState(WORLDSTATE_ASCENDANT_DESCENDING, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_STONEBLOW, 6000);
                events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 20000);
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_THUNDERCLAP, 6000);
                Talk(SAY_AGGRO);
                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
                summons.DespawnAll();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }
     
            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                if (me->IsInCombat())
                    summon->SetInCombatWithZone();
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->GetDistance(me->GetHomePosition()) > 60.0f)
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STONEBLOW:
                            DoCast(me->GetVictim(), SPELL_STONEBLOW);
                            events.ScheduleEvent(EVENT_STONEBLOW, 6000);
                            break;
                        case EVENT_TWILIGHT_CORRUPTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            DoCast(target, SPELL_TWILIGHT_CORRUPTION);
                            events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 20000);
                            break;
                        case EVENT_THUNDERCLAP:
                            DoCast(me, SPELL_THUNDERCLAP);
                            events.ScheduleEvent(EVENT_THUNDERCLAP, 15000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                Creature* shadows[3];
        };
     
        CreatureAI* GetAI(Creature* creature) const
        {
            return GetInstanceAI<boss_ascendant_lord_obsidiusAI>(creature);
        }
};

class npc_shadow_of_obsidius : public CreatureScript
{
    public:
        npc_shadow_of_obsidius() : CreatureScript("npc_shadow_of_obsidius") { }

        struct npc_shadow_of_obsidiusAI : public ScriptedAI
        {
            npc_shadow_of_obsidiusAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            }

            EventMap events;
            
            void Reset() override
            {
                DoCast(me, SPELL_TWITCHY);
                DoCast(me, SPELL_SHADOW_OF_OBSIDIUS);
            }   

            void JustEngagedWith(Unit* attacker) override
            {
                events.ScheduleEvent(EVENT_CREPUSCULAR_VEIL, 3900);
            }

            void DamageTaken(Unit* attacker, uint32& /*damage*/) override
            {
                if (me->GetVictim() != attacker)
                {
                    DoResetThreat();
                    me->GetThreatManager().AddThreat(attacker, 1000000.0f);
                    me->Attack(attacker, true);
                    me->GetMotionMaster()->MoveChase(attacker);
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
                    switch (eventId)
                    {
                        case EVENT_CREPUSCULAR_VEIL:
                            DoCast(me->GetVictim(), SPELL_CREPUSCULAR_VEIL);
                            events.ScheduleEvent(EVENT_CREPUSCULAR_VEIL, 3900);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_shadow_of_obsidiusAI>(creature);
        }
};

// Crepuscular Veil 76189
class spell_crepuscular_veil : public AuraScript
{
    PrepareAuraScript(spell_crepuscular_veil);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 3 && GetOwner() && GetOwner()->ToUnit())
            GetOwner()->ToUnit()->GetMap()->SetWorldState(WORLDSTATE_ASCENDANT_DESCENDING, 0);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_crepuscular_veil::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

void AddSC_boss_ascendant_lord_obsidius()
{
    new boss_ascendant_lord_obsidius();
    new npc_shadow_of_obsidius();
    new aura_script<spell_crepuscular_veil>("spell_crepuscular_veil");
}
