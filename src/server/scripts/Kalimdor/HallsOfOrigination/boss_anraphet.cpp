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
    SAY_DEATH                  = 0,
    SAY_INTRO                  = 1,
    SAY_KILL                   = 2,
    SAY_OMEGA                  = 3,
    SAY_AGGRO                  = 4,
};

enum Spells
{
    SPELL_ALPHA_BEAMS           = 76184,
    SPELL_ALPHA_BEAMS_AOE       = 76904, 
    SPELL_ALPHA_BEAM_CHANNELED  = 76912,
    SPELL_ALPHA_BEAM_AURA_EFF   = 91205, // void and damage
    SPELL_ALPHA_BEAM_SUMM       = 76907,
    SPELL_ALPHA_BEAM_DMG        = 76956, 
    SPELL_CRUMBLING_RUIN        = 75609, 
    SPELL_DESTRUCTION_PROTOCOL  = 77437,
    SPELL_NEMESIS_STRIKE        = 75604,
    SPELL_OMEGA_STANCE          = 75622,
    SPELL_OMEGA_STANCE_BUNNY    = 77106,
    SPELL_OMEGA_STANCE_GROWTH   = 77117,
    SPELL_OMEGA_STANCE_SPIDER   = 77121,
};

enum Events
{
    EVENT_ALPHA_BEAMS          = 1,
    EVENT_CRUMBLING_RUIN       = 2,
    EVENT_DESTRUCTION_PROTOCOL = 3,
    EVENT_NEMESIS_STRIKE       = 4,
    EVENT_INTRO                = 5,
    EVENT_OMEGA_STANCE         = 6,
};

enum Actions
{
    ACTION_REMOVE_AMEGA = 0,
    ACTION_INTRO        = 1,
};

class boss_anraphet : public CreatureScript
{
    public:
        boss_anraphet() : CreatureScript("boss_anraphet") { }

        struct boss_anraphetAI : public BossAI
        {
            boss_anraphetAI(Creature* creature) : BossAI(creature, DATA_ANRAPHET)
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
                //me->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
            }

            ObjectGuid targetGUID;

            void Reset() override
            {
                _Reset();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                DespawnCreatures(NPC_ALPHA_BEAM);
                DespawnCreatures(NPC_OMEGA_STANCE);
                targetGUID = ObjectGuid::Empty;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_ALPHA_BEAMS, 7000, 10000);
                events.ScheduleEvent(EVENT_NEMESIS_STRIKE, urand(3000, 7000));
                events.ScheduleEvent(EVENT_CRUMBLING_RUIN, 20000);
                events.ScheduleEvent(EVENT_OMEGA_STANCE, urand(30 * IN_MILLISECONDS, 40 * IN_MILLISECONDS));

                DoZoneInCombat();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_ANRAPHET, DONE);
                }

                DespawnCreatures(NPC_ALPHA_BEAM);
                DespawnCreatures(NPC_OMEGA_STANCE);
                me->SummonGameObject(GO_ORIGINATION_ELEVATOR, -505.522f, 193.411f, 79.2668f, 3.14159f, { }, 7200);
            }
            
            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() == NPC_ALPHA_BEAM)
                    me->CastSpell(summon, SPELL_ALPHA_BEAM_CHANNELED, true);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_INTRO:
                        Talk(SAY_INTRO);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetHomePosition(-203.93f, 368.71f, 75.92f, me->GetOrientation());
                        //DoTeleportTo(-203.93f, 368.71f, 75.92f);
                        me->GetMotionMaster()->MovePoint(0, -203.93f, 368.71f, 75.92f);
                        break;
                    case ACTION_REMOVE_AMEGA:
                        me->RemoveAurasDueToSpell(SPELL_OMEGA_STANCE_SPIDER);
                        me->RemoveChanneledCast(targetGUID);
                        break;
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
                        case EVENT_ALPHA_BEAMS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                me->CastSpell(target, SPELL_ALPHA_BEAMS, false);

                            events.ScheduleEvent(EVENT_ALPHA_BEAMS, 15000);
                            break;
                        case EVENT_CRUMBLING_RUIN:
                            DoCastAOE(SPELL_CRUMBLING_RUIN);
                            events.ScheduleEvent(EVENT_CRUMBLING_RUIN, 40000);
                            break;
                        case EVENT_NEMESIS_STRIKE:
                            DoCastVictim(SPELL_NEMESIS_STRIKE);
                            events.ScheduleEvent(EVENT_NEMESIS_STRIKE, urand(15000, 20000));
                            break;
                        case EVENT_OMEGA_STANCE:
                            Talk(SAY_OMEGA);
                            if (Unit* m_victim = me->GetVictim())
                            {
                                targetGUID = m_victim->GetGUID();
                                me->PrepareChanneledCast(me->GetAngle(m_victim), SPELL_OMEGA_STANCE, false);
                            }
                            events.ScheduleEvent(EVENT_OMEGA_STANCE, urand(30 * IN_MILLISECONDS, 40 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            void DespawnCreatures(uint32 entry)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, 1000.0f);

                if (creatures.empty())
                   return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                     (*iter)->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_anraphetAI>(creature);
        }
};

class npc_alpha_beam : public CreatureScript
{
    public:
        npc_alpha_beam() : CreatureScript("npc_alpha_beam") { }

        struct npc_alpha_beamAI : public ScriptedAI
        {
            npc_alpha_beamAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                SetCombatMovement(false);
           }

            void Reset() override { }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_alpha_beamAI>(creature);
        }
};

struct AmegaPredicate
{
    bool operator ()(WorldObject const* object) const
    {
        if (object->GetEntry() == NPC_OMEGA_STANCE)
            return false;

        return true;
    }
};

// Omega Stance 75622
class spell_omega_stance : public SpellScriptLoader
{
    public:
        spell_omega_stance() : SpellScriptLoader("spell_omega_stance") { }

        class spell_omega_stance_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_omega_stance_AuraScript);

            void OnTrigger(AuraEffect const* /*aurEff*/)
            {
                if (Unit* m_owner = GetCaster())
                    m_owner->CastSpell(m_owner->GetPositionX() + frand(-6.0f, 6.0f), m_owner->GetPositionY() + frand(-3.5f, 3.5f), m_owner->GetPositionZ(), SPELL_OMEGA_STANCE_BUNNY, true);
            }

            void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* m_caster = GetCaster())
                    m_caster->AddAura(SPELL_OMEGA_STANCE_SPIDER, m_caster);
            }

            void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* m_caster = GetCaster())
                    if (m_caster->ToCreature() && m_caster->ToCreature()->AI())
                        m_caster->ToCreature()->AI()->DoAction(ACTION_REMOVE_AMEGA);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_omega_stance_AuraScript::OnTrigger, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectApply += AuraEffectApplyFn(spell_omega_stance_AuraScript::OnAuraEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_omega_stance_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_omega_stance_AuraScript();
        }
};

// Omega Stance 77127
class spell_omega_stance_spider : public SpellScriptLoader
{
    public:
        spell_omega_stance_spider() : SpellScriptLoader("spell_omega_stance_spider") { }

        class spell_omega_stance_spider_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_omega_stance_spider_SpellScript);

            void SelectTargets(SpellDestination& dest)
            {
                if (Unit* m_caster = GetCaster())
                {
                    GetPositionWithDistInOrientation(m_caster, frand(2.0f, 40.0f), frand(0.0f, 2 * M_PI), x, y);
                    Position newPos = { x, y, m_caster->GetPositionZ(), 0 };
                    dest.Relocate(newPos);
                }
            }

            void Register() override
            {
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_omega_stance_spider_SpellScript::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RANDOM);
            }

            private:
                float x, y;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_omega_stance_spider_SpellScript();
        }
};

// Alpha Beam 76912
class spell_alpha_beam : public SpellScriptLoader
{
    public:
        spell_alpha_beam() : SpellScriptLoader("spell_alpha_beam") { }

        class spell_alpha_beam_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_alpha_beam_AuraScript);

            void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* m_caster = GetOwner()->ToCreature())
                    m_caster->CastSpell(m_caster, SPELL_ALPHA_BEAM_AURA_EFF, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_alpha_beam_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_alpha_beam_AuraScript();
        }
};

void AddSC_boss_anraphet()
{
    new boss_anraphet();
    new npc_alpha_beam();
    new spell_omega_stance();
    new spell_omega_stance_spider();
    new spell_alpha_beam();
}
