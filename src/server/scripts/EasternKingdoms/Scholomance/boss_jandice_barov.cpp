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
#include "scholomance.h"
#include "Group.h"

enum spells
{
    SPELL_WONDROUS_RAPIDITY        = 114062,
    SPELL_GRAVITY_FLUX             = 114035,
    SPELL_GRAVITY_FLUX_EFF         = 114038,
    SPELL_WHIRL_OF_ILLUSION        = 114048,
    SPELL_FLASH_BANG               = 113866,
};

enum events
{
    EVENT_WONDROUS_RAPIDITY        = 1,
    EVENT_WONDROUS_RAPIDITY_CANCEL = 2,
    EVENT_GRAVITY_FLUX             = 3,
    EVENT_ILLUSION                 = 4,
};

enum Yells
{
    TALK_AGGRO                     = 0,
    TALK_DEATH                     = 1,
    TALK_RAPIDITY                  = 2,
    TALK_ILLUSION                  = 3,
};

enum Phases
{
    PHASE_DEFAULT                  = 0,
    PHASE_ILLUSIONS                = 1
}; 

class boss_jandice_barov : public CreatureScript
{
    public:
        boss_jandice_barov() : CreatureScript("boss_jandice_barov") { }

        struct boss_jandice_barovAI : public BossAI
        {
            boss_jandice_barovAI(Creature* creature) : BossAI(creature, DATA_JANDICE_BAROV) { }

            uint32 Pos, phase, CurHealth, CurPcnt, Max;
            ObjectGuid TargetGUID;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                phase = PHASE_DEFAULT;
                CurHealth = 0;
                CurPcnt = 66;
                me->SetMaxHealth(Max);
                me->SetReactState(REACT_AGGRESSIVE);
                summons.DespawnAll();
                RemoveIllusions();
                me->SetVisible(true);
                HandleDoors(true);
                me->GetMap()->SetWorldState(WORLDSTATE_ATTENTION_TO_DETAIL, 1);
            }

            void InitializeAI() override 
            {
                CurHealth = 0;
                phase = PHASE_DEFAULT;
                CurPcnt = 66;
                Max = me->GetMaxHealth(); 
                TargetGUID = ObjectGuid::Empty;
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                //Talk(TALK_DEATH);
                if (instance)
                {
                    instance->SetData(DATA_JANDICE_BAROV, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }
                RemoveIllusions();
                Talk(TALK_DEATH);
            }

            void JustSummoned(Creature* summon) override
            {
                switch (summon->GetEntry())
                {
                    case NPC_GRAVITY_FLUX_STALKER:
                        summon->CastSpell(summon, SPELL_GRAVITY_FLUX, false);
                        summon->DespawnOrUnsummon(30000);
                        break;
                    case NPC_JANDICE_ILLUSION:
                        summon->SetInCombatWith(me);
                        DoZoneInCombat(summon);
                        summon->SetReactState(REACT_PASSIVE);
                        summon->AttackStop();
                        summon->StopMoving();
                        summon->AddAura(SPELL_WHIRL_OF_ILLUSION, summon);
                        summon->SetMaxHealth(IsHeroic() ? 200000 : 42900);
                        summon->SetHealth(IsHeroic() ? 200000 : 42900);
                        break;
                    default:
                        break;
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_JANDICE_BAROV, FAIL);
                }
                summons.DespawnAll();
                RemoveIllusions();
                HandleDoors(true);
            }

            void HandleDoors(bool reset = false)
            {
                if (instance)
                    if (GameObject* Idoor = GetClosestGameObjectWithEntry(me, GO_JANDICE_INTRO_DOOR, 150.0f))
                        instance->HandleGameObject(ObjectGuid::Empty, reset, Idoor);
            }

            void RemoveIllusions()
            {
                std::list <Creature*> Illusions;
                GetCreatureListWithEntryInGrid(Illusions, me, NPC_JANDICE_ILLUSION, 150.0f);

                for (auto&& itr : Illusions)
                    itr->DespawnOrUnsummon();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                //Talk(TALK_INTO);
                if (instance)
                {
                    instance->SetData(DATA_JANDICE_BAROV, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                GetEvents();
                Talk(TALK_AGGRO);
                HandleDoors();

                if (Unit* TalkingSkull = ObjectAccessor::GetUnit(*me, instance->GetGuidData(NPC_TALKING_SKULL)))
                    TalkingSkull->ToCreature()->AI()->Talk(TALK_JANDICE_COMBAT);
            }

            void GetEvents()
            {
                events.ScheduleEvent(EVENT_WONDROUS_RAPIDITY, 2500);
                events.ScheduleEvent(EVENT_GRAVITY_FLUX, urand(8000, 12000));
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override  
            {
                if (CurPcnt == 0 && phase == PHASE_DEFAULT)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (HealthBelowPct(CurPcnt) && phase == PHASE_DEFAULT)
                {
                    phase = PHASE_ILLUSIONS;
                    events.Reset();
                    me->CastSpell(me, SPELL_FLASH_BANG, true);
                    Talk(TALK_ILLUSION);
                    me->RemoveAllAuras();

                    me->SetVisible(false);
                    CurHealth = me->GetHealth();
                    me->PrepareChanneledCast(me->GetOrientation());
                    events.ScheduleEvent(EVENT_ILLUSION, 2 * IN_MILLISECONDS);

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    CurPcnt -= 33;
                }

                if (phase != PHASE_ILLUSIONS)
                    return;

                if (me->GetHealth() <= damage)
                {
                    damage = 0;
                    me->RemoveAllAuras();
                    phase = PHASE_DEFAULT;
                    me->SetMaxHealth(Max);
                    me->SetHealth(CurHealth);
                    me->RemoveAura(SPELL_WHIRL_OF_ILLUSION);
                    RemoveIllusions();
                    me->RemoveChanneledCast(TargetGUID);
                    GetEvents();
                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
            }

            void SummonIllusions(uint32 fake)
            {
                for (uint32 i = 0; i < MAX_ILLUSIONS; i++)
                    if (i != fake)
                        me->SummonCreature(NPC_JANDICE_ILLUSION, IllusionsPoints[i], TEMPSUMMON_MANUAL_DESPAWN);
            }

            void SendClearTargetToPlayers()
            {
                std::list <Player*> TargetClear;
                GetPlayerListInGrid(TargetClear, me, 150.0f);

                for (auto&& itr : TargetClear)
                    itr->SendClearTarget();
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING) && phase == PHASE_DEFAULT)
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WONDROUS_RAPIDITY:
                        {
                            if (Unit* vic = me->GetVictim())
                            {
                                Talk(TALK_RAPIDITY);
                                me->PrepareChanneledCast(me->GetAngle(vic), SPELL_WONDROUS_RAPIDITY);
                                events.ScheduleEvent(EVENT_WONDROUS_RAPIDITY_CANCEL, 7700);
                                events.ScheduleEvent(EVENT_WONDROUS_RAPIDITY, urand(18000, 22000));
                            }
                        }
                            break;
                        case EVENT_WONDROUS_RAPIDITY_CANCEL:
                        {
                            me->RemoveChanneledCast(TargetGUID);
                        }
                            break;
                        case EVENT_ILLUSION:
                        {
                            Pos = urand(0, MAX_ILLUSIONS - 1);
                            SummonIllusions(Pos);
                            me->SetPosition(IllusionsPoints[Pos].GetPositionX(), IllusionsPoints[Pos].GetPositionY(), IllusionsPoints[Pos].GetPositionZ(), IllusionsPoints[Pos].GetOrientation());
                            me->RemoveAllAuras();
                            me->SetMaxHealth(IsHeroic() ? 200000 : 3432);
                            me->SetHealth(IsHeroic() ? 200000 : 3432);
                            me->SetVisible(true);
                            me->AddAura(SPELL_WHIRL_OF_ILLUSION, me);
                        }
                            break;
                        case EVENT_GRAVITY_FLUX:
                        {
                            if (Unit* itr = SelectTarget(SELECT_TARGET_RANDOM))
                                me->SummonCreature(NPC_GRAVITY_FLUX_STALKER, itr->GetPositionX() + frand(-5.0f, 5.0f), itr->GetPositionY() + frand(-10.0f, 10.0f), me->GetPositionZ(), TEMPSUMMON_MANUAL_DESPAWN);
                            events.ScheduleEvent(EVENT_GRAVITY_FLUX, urand(15000, 16000));
                        }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 86.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_jandice_barovAI(creature);
        }
};

// Jandice Illusion 59220
class npc_jandice_illusion : public CreatureScript
{
    public:
        npc_jandice_illusion() : CreatureScript("npc_jandice_illusion") { }

        struct npc_jandice_illusionAI : public ScriptedAI
        {
            npc_jandice_illusionAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap nonCombatEvents;
            bool toBeRemoved = false;

            void Reset() override { }

            void IsSummonedBy(Unit* summoner) override
            {
                SetEquipmentSlots(false, JandiceStuff, 0, EQUIP_NO_CHANGE);
            }

            void DoAction(int32 actionId) override { }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (me->GetHealth() <= damage && !toBeRemoved)
                {
                    toBeRemoved = true;
                    damage = 0;

                    if (IsHeroic())
                        DoCastAOE(SPELL_FLASH_BANG);

                    me->GetMap()->SetWorldState(WORLDSTATE_ATTENTION_TO_DETAIL, 0);
                    me->DespawnOrUnsummon(500);
                }
            }

            void UpdateAI(uint32 diff) override { }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jandice_illusionAI(creature);
        }
};

// Whirl of Illusion 113775
// Not sure how it should work, but seems like each arrow has ~15% chance to deal damage at hit
class spell_whirl_of_illusion : public SpellScriptLoader
{
    public:
        spell_whirl_of_illusion() : SpellScriptLoader("spell_whirl_of_illusion") { }

        class spell_whirl_of_illusion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_whirl_of_illusion_SpellScript);

            void HandleHitEffect(SpellEffIndex /*effIndex*/)
            {
                if (Creature* caster = GetCaster()->ToCreature())
                    if (caster->GetEntry() != NPC_JANDICE_BAROV)
                        SetHitDamage(0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_whirl_of_illusion_SpellScript::HandleHitEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_whirl_of_illusion_SpellScript();
        }
};

// 204. Summoned by 114035 - Gravity Flux
class sat_scholomance_gravity_flux : public IAreaTriggerOnce
{
    bool CheckTriggering(WorldObject* object) override
    {
        return object && object->ToPlayer() && object->ToPlayer()->IsAlive() && GetAreaTrigger() && GetAreaTrigger()->GetDuration() < 28 * IN_MILLISECONDS;
    }

    void OnTrigger(WorldObject* target) override
    {
        if (Player* itr = target->ToPlayer())
            itr->CastSpell(itr, SPELL_GRAVITY_FLUX_EFF, true);
    }
};

void AddSC_boss_jandice_barov()
{
    new boss_jandice_barov();
    new npc_jandice_illusion();
    new spell_whirl_of_illusion();
    new atrigger_script<sat_scholomance_gravity_flux>("sat_scholomance_gravity_flux");
}
