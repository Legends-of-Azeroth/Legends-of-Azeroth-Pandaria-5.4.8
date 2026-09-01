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

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "scarlet_halls.h"

enum spells
{
    SPELL_HEROIC_LEAP_DUMMY          = 111219, // emote for script
    SPELL_HEROIC_LEAP                = 111218,
    SPELL_DRAGONS_REACH              = 111217,
    SPELL_BERSERKER_RAGE             = 111221,
    SPELL_LIGHT_BLADES_APPLY_AURA    = 111216, // only visual bladestorm
    SPELL_LIGHT_BLADES               = 111215,
    SPELL_LIGHT_BLADES_AURA          = 112955,
    SPELL_LIGHT_BLADES_TRIGGERED     = 111394,
    SPELL_HEAVY_ARMOR                = 113959,
    SPELL_UNARMORED                  = 113969,
};

enum events
{
    EVENT_DRAGONS_REACH = 1,
    EVENT_HEROIC_LEAP,
    EVENT_LIGHT_BLADES,
    EVENT_BACK,
    EVENT_CALL_FOR_HELP,

    // Misc
    EVENT_INIT_COMBAT,
};

enum Yells
{
    TALK_INTO,
    TALK_DEATH,
    TALK_HEROIC_LEAP,
    TALK_DRAGONS_REACH,
    TALK_CALL_FOR_HELP,
};

enum Actions
{
    ACTION_LIGHT_BLADES,
    ACTION_FOLLOW_ME,
    ACTION_CAUGHT,
};

enum eType
{
    TYPE_LIGHT_BLADES = 1,
};

class boss_armsmaster_harlan : public CreatureScript
{
    public:
        boss_armsmaster_harlan() : CreatureScript("boss_armsmaster_harlan") { }

        struct boss_armsmaster_harlanAI : public BossAI
        {
            boss_armsmaster_harlanAI(Creature* creature) : BossAI(creature, BOSS_ARMSMASTER_HARLAN) { }

            bool bers;
            ObjectGuid targetGuid;
            uint32 wp, m_bSeats;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);

                bers       = false;
                targetGuid = ObjectGuid::Empty;
                wp         = 0;
                m_bSeats   = 1;
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();

                Talk(TALK_DEATH);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_INTO);

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                GetEvents();
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == TYPE_LIGHT_BLADES)
                    return m_bSeats;

                return 0;
            }

            void SetData(uint32 type, uint32 /*data*/) override
            {
                if (type == TYPE_LIGHT_BLADES)
                    m_bSeats++;
            }

            void GetPassengerExitPosition(Unit*, int8, Position& pos) override
            {
                pos = { me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ() };
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_LIGHT_BLADES)
                {
                    Movement::MoveSplineInit init(me);
                    for (auto&& itr : BladeStormPoints)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.Launch();
                    init.SetWalk(false);
                    events.ScheduleEvent(EVENT_BACK, me->GetSplineDuration() + 1000);
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                {
                    DoCast(me, SPELL_HEROIC_LEAP);
                    DoCast(me, SPELL_LIGHT_BLADES_APPLY_AURA);
                    events.Reset();
                }
            }

            void GetEvents()
            {
                events.ScheduleEvent(EVENT_DRAGONS_REACH, urand(5000, 8000));
                events.ScheduleEvent(EVENT_CALL_FOR_HELP, urand(10000, 11000));
                events.ScheduleEvent(EVENT_HEROIC_LEAP, 25000);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override 
            {
                if (!bers && HealthBelowPct(50))
                {
                    bers = true;
                    me->CastSpell(me, SPELL_BERSERKER_RAGE, false);
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
                        case EVENT_DRAGONS_REACH:
                            if (Unit* target = me->GetVictim())
                                me->CastSpell(target, SPELL_DRAGONS_REACH, false);

                            Talk(TALK_DRAGONS_REACH);
                            events.ScheduleEvent(EVENT_DRAGONS_REACH, urand(5000, 8000));
                            break;
                        case EVENT_HEROIC_LEAP:
                            me->CastSpell(me, SPELL_HEROIC_LEAP_DUMMY, false);
                            me->GetMotionMaster()->MoveJump(JumpPoint, 42.0f, 42.0f, EVENT_JUMP);
                            Talk(TALK_HEROIC_LEAP);

                            if (Unit* vict = me->GetVictim())
                                targetGuid = vict->GetGUID();

                            me->GetThreatManager().resetAllAggro();
                            me->AttackStop();
                            me->SetReactState(REACT_PASSIVE);
                            break;
                        case EVENT_BACK:
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveAura(SPELL_LIGHT_BLADES_APPLY_AURA);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIGHT_BLADES);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_LIGHT_BLADES_AURA);
                            m_bSeats = 1;
                            wp       = 0;

                            if (Vehicle* Passengers = me->GetVehicleKit())
                                Passengers->RemoveAllPassengers(/*true*/); // TODO: do something with vehicles

                            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGuid))
                                me->AI()->AttackStart(target);

                            GetEvents();
                            break;
                        case EVENT_CALL_FOR_HELP:
                            Talk(TALK_CALL_FOR_HELP);
                            for (uint32 i = 0; i < 2; i++)
                                me->SummonCreature(NPC_SCARLET_DEFENDER, 1188.38f, 444.03f + frand(2.0, 4.0f), 11.17f, 0.07f, TEMPSUMMON_MANUAL_DESPAWN);

                            events.ScheduleEvent(EVENT_CALL_FOR_HELP, urand(20000, 25000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_armsmaster_harlanAI(creature);
        }
};

// Scarlet defender 58998
struct npc_scarlet_defender : public ScriptedAI
{
    npc_scarlet_defender(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    uint32 curPos;

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        SetEquipmentSlots(false, 98316, 58998, 0);
        me->CastSpell(me, SPELL_HEAVY_ARMOR, false);
        me->SetReactState(REACT_PASSIVE);
        me->AttackStop();
        curPos = urand(0, 1);

        auto& path = curPos ? DefenderPath1 : DefenderPath2;
        Movement::MoveSplineInit init(me);
        for (auto&& itr : path)
            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

        init.Launch();
        init.SetWalk(false);
        events.ScheduleEvent(EVENT_INIT_COMBAT, me->GetSplineDuration());

    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_INIT_COMBAT)
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetInCombatWithZone();
            }
            break;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// Heavy armor 113959
class spell_heavy_armor : public AuraScript
{
    PrepareAuraScript(spell_heavy_armor);

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_UNARMORED, false);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_heavy_armor::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// Light Blades 111216
class spell_harlan_light_blades : public SpellScript
{
    PrepareSpellScript(spell_harlan_light_blades);

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->AI()->DoAction(ACTION_LIGHT_BLADES);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_harlan_light_blades::HandleAfterCast);
    }
};

// Light Blades 111394
class spell_harlan_light_blades_proc : public SpellScript
{
    PrepareSpellScript(spell_harlan_light_blades_proc);

    void HandleDummyEffect(SpellEffIndex effIndex)
    {
        if (Creature* caster = GetCaster()->ToCreature())
        {
            if (Unit* target = GetHitUnit())
            {
                if (uint32 seat = caster->AI()->GetData(TYPE_LIGHT_BLADES))
                {
                    // Vehicle Aura
                    if (!target->HasAura(SPELL_LIGHT_BLADES_AURA) && seat < 8)
                    {
                        if (const SpellInfo* m_spell = sSpellMgr->GetSpellInfo(VEHICLE_SPELL_RIDE_HARDCODED))
                        {
                            int32 m_bp = m_spell->Effects[EFFECT_0].BasePoints + (int32)seat;
                            target->CastSpell(caster, SPELL_LIGHT_BLADES_AURA, true);
                            target->CastCustomSpell(caster, VEHICLE_SPELL_RIDE_HARDCODED, &m_bp, 0, 0, true);
                            caster->AI()->SetData(TYPE_LIGHT_BLADES, 0);
                        }
                    }

                    caster->CastSpell(target, SPELL_LIGHT_BLADES, true);
                }
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_harlan_light_blades_proc::HandleDummyEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Blades of Light Eff 111215
class spell_harlan_light_blades_eff : public SpellScript
{
    PrepareSpellScript(spell_harlan_light_blades_eff);

    void HandleEffectHit(SpellEffIndex effIndex)
    {
        uint32 damage = GetHitDamage();

        if (Player* target = GetHitPlayer())
        {
            if (damage >= target->GetHealth() && target->HasAura(SPELL_LIGHT_BLADES_AURA)) // hackfix, vehicle issue
            {
                SetHitDamage(0);
                target->RemoveAurasDueToSpell(SPELL_LIGHT_BLADES_AURA);
                target->_ExitVehicle();

                // Delay last hit
                target->m_Events.Schedule(200, [=]()
                {
                    if (Creature* harlan = ObjectAccessor::GetCreature(*target, target->GetInstanceScript() ? target->GetInstanceScript()->GetGuidData(BOSS_ARMSMASTER_HARLAN) : ObjectGuid::Empty))
                        harlan->CastSpell(target, SPELL_LIGHT_BLADES, true);
                });
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_harlan_light_blades_eff::HandleEffectHit, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
    }
};

void AddSC_boss_armsmaster_harlan()
{
    new boss_armsmaster_harlan();
    new creature_script<npc_scarlet_defender>("npc_scarlet_defender");

    new aura_script<spell_heavy_armor>("spell_heavy_armor");
    new spell_script<spell_harlan_light_blades>("spell_harlan_light_blades");
    new spell_script<spell_harlan_light_blades_proc>("spell_harlan_light_blades_proc");
    new spell_script<spell_harlan_light_blades_eff>("spell_harlan_light_blades_eff");
}
