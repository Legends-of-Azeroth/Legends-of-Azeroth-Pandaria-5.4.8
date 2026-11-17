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
#include "halls_of_origination.h"

enum ScriptTexts
{
    SAY_DEATH   = 0,
    SAY_AGGRO   = 1,
    SAY_KILL    = 2,
};

enum Spells
{
    SPELL_FLAME_BOLT                = 77370,
    SPELL_FLAME_BOLT_DMG            = 75540, 
    SPELL_RAGING_SMASH              = 83650,
    SPELL_EARTH_SPIKE               = 94974,
    SPELL_EARTH_SPIKE_DMG           = 75339,
    SPELL_EARTH_SPIKE_KILL          = 89398,
    SPELL_QUICKSAND                 = 75546,
    SPELL_QUICKSAND_AOE             = 75547,
    SPELL_QUICKSAND_DIS             = 89396,

    SPELL_SAND_VORTEX_DUMMY1        = 79441,
    SPELL_SAND_VORTEX_DUMMY2        = 93570,
    SPELL_SAND_VORTEX               = 83097,
    SPELL_SAND_VORTEX_DMG           = 83096, 

    SPELL_SUBMERGE                  = 53421,

    SPELL_SUMMON_DUSTBONE_HORROR    = 75521,
    SPELL_SUMMON_JEWELED_SCARAB     = 75462,
    SPELL_BEETLE_BURROW             = 75463,

    SPELL_SMASH                     = 75453,
};
 
enum Events
{
    EVENT_FLAME_BOLT    = 1,
    EVENT_RAGING_SMASH  = 2,
    EVENT_EARTH_POINT   = 3,
    EVENT_SUBMERGE      = 4,
    EVENT_SUMMON        = 5,      
    EVENT_STORM_MOVE    = 6,
    EVENT_VORTEX_DUST   = 7,
    EVENT_SMASH         = 8,
    EVENT_MERGE         = 9,
};
 
enum Adds
{
    NPC_QUICKSAND               = 40503, // 75546
    NPC_DUSTBONE_HORROR         = 40450,
    NPC_JEWELED_SCARAB          = 40458,
    NPC_TUMULTUOUS_EARTHSTORM   = 40406,
};
 
class boss_earthrager_ptah : public CreatureScript
{
    public:
        boss_earthrager_ptah() : CreatureScript("boss_earthrager_ptah") { }

        struct boss_earthrager_ptahAI : public BossAI
        {
            boss_earthrager_ptahAI(Creature* creature) : BossAI(creature, DATA_EARTHRAGER_PTAH)
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
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
            }

            uint8 count;
            uint8 phase;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                
                phase = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_FLAME_BOLT, urand(5000, 8000));
                events.ScheduleEvent(EVENT_RAGING_SMASH, urand(7000, 10000));
                events.ScheduleEvent(EVENT_EARTH_POINT, urand(12000, 15000));
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    // Hackfix, default method won`t check this criteria
                    if (AchievementEntry const* achievCamel = sAchievementStore.LookupEntry(ACHIEV_STRAW_THAT_BROKE_CAMEL))
                    {
                        for (auto&& itr : instance->instance->GetPlayers())
                            if (Player* target = itr.GetSource())
                                if (target->IsOnVehicle())
                                    target->CompletedAchievement(achievCamel);
                    }
                }

                Talk(SAY_DEATH);
            }
            
            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void HandleSetBurrow()
            {
                std::list<Creature*> m_burrowStalker;
                GetCreatureListWithEntryInGrid(m_burrowStalker, me, NPC_BEETLE_STALKER, 200.0f);

                if (!m_burrowStalker.empty())
                    for (auto itr : m_burrowStalker)
                        itr->CastSpell(itr, SPELL_BEETLE_BURROW, false);
            }
      
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;
                                       
                events.Update(diff);
 
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->HealthBelowPct(50) && phase == 0)
                {
                    phase = 1;
                    me->RemoveAllAuras();
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    DoCast(me, SPELL_SUBMERGE);
                    me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                    events.Reset();
                    events.ScheduleEvent(EVENT_SUBMERGE, 2000);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUBMERGE:           
                            HandleSetBurrow();
                            DoCast(me, SPELL_QUICKSAND, true);
                            DoCast(me, SPELL_SAND_VORTEX, true);
                            events.ScheduleEvent(EVENT_MERGE, 30000);
                            break;
                        case EVENT_MERGE:
                            me->RemoveAllAuras();
                            me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            events.ScheduleEvent(EVENT_FLAME_BOLT, urand(5000, 8000));
                            events.ScheduleEvent(EVENT_RAGING_SMASH, urand(7000, 10000));
                            events.ScheduleEvent(EVENT_EARTH_POINT, urand(12000, 15000));
                            break;
                        case EVENT_FLAME_BOLT:
                            DoCast(me, SPELL_FLAME_BOLT);
                            events.ScheduleEvent(EVENT_FLAME_BOLT, urand(16000, 20000));
                            break;
                        case EVENT_RAGING_SMASH:
                            DoCastVictim(SPELL_RAGING_SMASH);
                            events.ScheduleEvent(EVENT_RAGING_SMASH, urand(10000, 15000));
                            break;
                        case EVENT_EARTH_POINT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                DoCast(target->GetVehicleBase() ? target->GetVehicleBase() : target, SPELL_EARTH_SPIKE, true);
                            events.ScheduleEvent(EVENT_EARTH_POINT, urand(20000, 25000));
                            break;                            
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_earthrager_ptahAI>(creature);
        }
};

struct earthrager_typeAI : public ScriptedAI
{
    earthrager_typeAI(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        DoZoneInCombat();
    }
};

class npc_earthrager_base : public CreatureScript
{
    public:
        npc_earthrager_base() : CreatureScript("npc_earthrager_base") { }

        struct npc_earthrager_baseAI : public earthrager_typeAI
        {
            npc_earthrager_baseAI(Creature* creature) : earthrager_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_earthrager_baseAI(creature);
        }
};

class npc_ptah_dustbone_horror: public CreatureScript
{
    public:
        npc_ptah_dustbone_horror() : CreatureScript("npc_ptah_dustbone_horror") { }

        struct npc_ptah_dustbone_horrorAI : public earthrager_typeAI
        {
            npc_ptah_dustbone_horrorAI(Creature* creature) : earthrager_typeAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SMASH, urand(2000, 8000));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SMASH:
                            DoCast(me->GetVictim(), SPELL_SMASH);
                            events.ScheduleEvent(EVENT_SMASH, urand(5000, 10000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_ptah_dustbone_horrorAI>(creature);
        }
};

// Beetle Burrow 75463
class spell_beetle_burrow : public SpellScriptLoader
{
    public:
        spell_beetle_burrow() : SpellScriptLoader("spell_beetle_burrow") { }

        class spell_beetle_burrow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_beetle_burrow_AuraScript);

            void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* m_caster = GetCaster())
                    m_caster->CastSpell(m_caster, urand(0, 1) ? SPELL_SUMMON_DUSTBONE_HORROR : SPELL_SUMMON_JEWELED_SCARAB, false);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_beetle_burrow_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_beetle_burrow_AuraScript();
        }
};

void AddSC_boss_earthrager_ptah()
{
    new boss_earthrager_ptah();
    new npc_earthrager_base();
    new npc_ptah_dustbone_horror();
    new spell_beetle_burrow();
}
