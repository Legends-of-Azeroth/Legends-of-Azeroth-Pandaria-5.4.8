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

#include "ragefire_chasm.h"

enum Spells
{
    SPELL_INFERNO_CHARGE_EFF = 119299, // triggered by 119405, but actually by handle charge...
};

class boss_adarogg : public CreatureScript
{
    public:
        boss_adarogg() : CreatureScript("boss_adarogg") { }

        enum iSpells
        {
            SPELL_INFERNO_CHARGE     = 119405,
            SPELL_FLAME_BREATH       = 119420, // on victim
        };

        enum iEvents
        {
            EVENT_INFERNO_CHARGE = 1,
            EVENT_FLAME_BREATH   = 2,
        };

        struct boss_adaroggAI : public BossAI
        {
            boss_adaroggAI(Creature* creature) : BossAI(creature, BOSS_ADAROGG) { }

            uint32 m_InfernoTarget;

            void InitializeAI() override 
            {
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                summons.DespawnAll();
                m_InfernoTarget = 0;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* who) override
            {
                // @TODO: Set in combat for other protectors
                _JustEngagedWith();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                    instance->SetBossState(BOSS_ADAROGG, IN_PROGRESS);
                }

                events.ScheduleEvent(EVENT_INFERNO_CHARGE, urand(11 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                    if (uint32 m_inferno = GetData(TYPE_TARGET_ID))
                        if (Player* InfernoTarget = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, m_inferno)))
                            DoCast(InfernoTarget, SPELL_INFERNO_CHARGE_EFF);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(BOSS_ADAROGG, DONE);
                }
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_TARGET_ID)
                    return m_InfernoTarget;
                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_TARGET_ID)
                    m_InfernoTarget = data;
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
                        case EVENT_INFERNO_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            {
                                //Talk(TALK_ANN, target);
                                me->CastSpell(target, SPELL_INFERNO_CHARGE, false);
                                m_InfernoTarget = target->GetGUID();
                            }

                            events.ScheduleEvent(EVENT_INFERNO_CHARGE, urand(11 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            events.ScheduleEvent(EVENT_FLAME_BREATH, urand(4 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                            break;
                        case EVENT_FLAME_BREATH:
                            if (Unit* vict = me->GetVictim())
                                me->CastSpell(vict, SPELL_FLAME_BREATH, false);
                            break;
                    }
                }

                if (!me->HasAura(SPELL_INFERNO_CHARGE))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_adaroggAI(creature);
        }
};

// Inferno Charge 119405
class spell_ragefire_inferno_charge : public SpellScriptLoader
{
    public:
        spell_ragefire_inferno_charge() : SpellScriptLoader("spell_ragefire_inferno_charge") { }

        class spell_ragefire_inferno_charge_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ragefire_inferno_charge_AuraScript);

            void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
            {
                if (Unit* caster = GetCaster())
                    if (caster->ToCreature() && caster->ToCreature()->AI())
                        if (uint32 m_inferno = caster->ToCreature()->AI()->GetData(TYPE_TARGET_ID))
                            if (Player* InfernoTarget = ObjectAccessor::FindPlayer(ObjectGuid(HighGuid::Player, m_inferno)))
                                caster->GetMotionMaster()->MoveCharge(InfernoTarget->GetPositionX(), InfernoTarget->GetPositionY(), InfernoTarget->GetPositionZ(), 42.0f, EVENT_CHARGE);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_ragefire_inferno_charge_AuraScript::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ragefire_inferno_charge_AuraScript();
        }
};

void AddSC_boss_adarogg()
{
    new boss_adarogg();
    new spell_ragefire_inferno_charge();
}
