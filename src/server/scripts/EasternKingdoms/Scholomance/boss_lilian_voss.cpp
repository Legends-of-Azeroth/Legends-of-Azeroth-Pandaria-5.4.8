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
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "scholomance.h"

enum spells
{
    // Lilian
    SPELL_SHADOW_SHIV         = 111775,
    SPELL_DEATHS_GRASP        = 111570, // dummy
    SPELL_DEATHS_GRASP_EFFECT = 111573,
    SPELL_DARK_BLAZE          = 111585, // apply aura on players after grasp
    SPELL_DARK_BLAZE_DUMMY    = 111587, // apply on dummy
    SPELL_DARK_BLAZE_SUMMON   = 111609, // should summon trigger npc

    // Her soul
    SPELL_BLAZING_SOUL        = 111642,
    SPELL_UNLEASHED_ANGUISH   = 111649,
    SPELL_FIXATE_ANGER        = 115350,
};

enum events
{
    EVENT_SHADOW_SHIV         = 1,
    EVENT_DEATHS_GRASP        = 2,
    EVENT_DARK_BLAZE          = 3,
    EVENT_FIXATE_ANGER        = 4,
    EVENT_SUMMON_SOUL         = 5,
    EVENT_RESSURECT           = 6,
    EVENT_INTRO_DONE          = 7,
    EVENT_SOUL_DISMISS        = 8,
    EVENT_DIE_NECROMANCE      = 9,
    EVENT_TALK_1              = 10,
    EVENT_TALK_2              = 11,
    EVENT_TALK_3              = 12,
    EVENT_TALK_4              = 13,
    EVENT_TALK_5              = 14,
    EVENT_TALK_6              = 15,
    EVENT_TALK_7              = 16,
    EVENT_SPECIAL             = 17,
    EVENT_PURIFIED            = 18,
};

enum Yells
{
    TALK_1                    = 0,
    TALK_2                    = 1,
    TALK_3                    = 2,
    TALK_4                    = 3,
    TALK_5                    = 4,
    TALK_6                    = 5,
    TALK_7_ANN                = 6,
    TALK_8                    = 7,
    TALK_9                    = 8,
    TALK_10                   = 9,
    TALK_11                   = 10,
    TALK_12                   = 11,
    TALK_13                   = 12,
};

class boss_lilian_voss : public CreatureScript
{
    public:
        boss_lilian_voss() : CreatureScript("boss_lilian_voss") { }

        struct boss_lilian_vossAI : public BossAI
        {
            boss_lilian_vossAI(Creature* creature) : BossAI(creature, DATA_LILIAN_VOSS) { }

            EventMap nonCombatEvents;
            bool IsSoul, Gandling, fict;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);
                IsSoul   = false;
                Gandling = false;
            }

            void InitializeAI() override
            {
                SetEquipmentSlots(false, LilianSword, LilianSword, EQUIP_NO_CHANGE);;
                me->SetFaction(1665);
                SetCombatMovement(false);
                Reset();
                fict = false;
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                //Talk(TALK_DEATH);
                if (instance)
                {
                    instance->SetData(DATA_LILIAN_VOSS, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }
                if (Creature* LilianSoul = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_LILIAN_SOUL)))
                    LilianSoul->AI()->DoAction(ACTION_REMOVE_LILIAN_SOUL);

                me->Kill(me);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_LILIAN_VOSS, FAIL);
                }
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (!fict)
                    return;

                _JustEngagedWith();
                //Talk(TALK_INTO);
                if (instance)
                {
                    instance->SetData(DATA_LILIAN_VOSS, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                Talk(TALK_2);
                GetEvents();
                me->RemoveAurasDueToSpell(SPELL_COSMETIC_SOUL_RIP);
            }

            void GetEvents()
            {
                events.ScheduleEvent(EVENT_SHADOW_SHIV, urand(3500, 5000));
                events.ScheduleEvent(EVENT_DEATHS_GRASP, 10000);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_LILIAN_RESSURECTED:
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        me->RemoveAllAuras();
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                        GetEvents();

                        if (Player* itr = me->FindNearestPlayer(100.0f))
                            me->AI()->AttackStart(itr);
                        break;
                    case ACTION_INTRO:
                        nonCombatEvents.ScheduleEvent(EVENT_TALK_1, 3000);
                        break;
                    case ACTION_DIFF_INTRO:
                        Talk(TALK_1);
                        nonCombatEvents.ScheduleEvent(EVENT_INTRO_DONE, 6000);
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == EFFECT_MOTION_TYPE && pointId == EVENT_JUMP)
                {
                    me->CastSpell(me, SPELL_SHADOW_SHIV, false);

                    if (Gandling)
                        nonCombatEvents.ScheduleEvent(EVENT_PURIFIED, 9000);
                }

                if (type != POINT_MOTION_TYPE)
                    return;

                if (pointId == 0)
                {
                    Talk(TALK_11);
                    me->CastSpell(me, SPELL_SHADOW_STEP_VISUAL, false);
                    me->NearTeleportTo(me->GetPositionX(), me->GetPositionY() + 10.0f, me->GetPositionZ(), me->GetOrientation());
                    nonCombatEvents.ScheduleEvent(EVENT_SPECIAL, 1500);
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (Gandling)
                {
                    damage = 0;
                    return;
                }

                if (!fict)
                    return;

                if (!IsSoul && HealthBelowPct(50))
                {
                    IsSoul = true;
                    events.Reset();
                    me->RemoveAllAuras();
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->CastSpell(me, SPELL_LILIAN_VOSS_DEAD, false);
                    me->GetMotionMaster()->MovePoint(1, lilianDeathPoint);

                    if (Creature* Gandling = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GRANDMASTER_GANDLING_INTRO)))
                        Gandling->AI()->DoAction(ACTION_DIFF_INTRO);

                    events.ScheduleEvent(EVENT_SOUL_DISMISS, 9000);
                    Talk(TALK_3);
                }
                if (IsSoul && !Gandling && me->GetHealth() <= damage)
                {
                    damage = 0;
                    Gandling = true;
                    events.Reset();
                    me->RemoveAllAuras();
                    me->AttackStop();
                    me->SetFaction(35);
                    me->SetReactState(REACT_PASSIVE);

                    if (Creature* LilianSoul = ObjectAccessor::GetCreature(*me, instance->GetGuidData(NPC_LILIAN_SOUL)))
                        LilianSoul->AI()->DoAction(ACTION_REMOVE_LILIAN_SOUL);

                    nonCombatEvents.ScheduleEvent(EVENT_DIE_NECROMANCE, 2500);
                    Talk(TALK_5);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TALK_1:
                            Talk(TALK_7_ANN);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_2, 10000);
                            break;
                        case EVENT_TALK_2:
                            Talk(TALK_8);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_3, 10000);
                            break;
                        case EVENT_TALK_3:
                            Talk(TALK_9);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_4, 10000);
                            break;
                        case EVENT_TALK_4:
                            Talk(TALK_10);
                            nonCombatEvents.ScheduleEvent(EVENT_TALK_5, 18000);
                            break;
                        case EVENT_TALK_5:
                            Talk(TALK_12);
                            me->AddUnitState(UNIT_STATE_MOVING);
                            me->SetReactState(REACT_PASSIVE);
                            me->CombatStop();
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_PACIFIED);
                            me->SetFaction(35);
                            me->GetMotionMaster()->MovePoint(0, GandlinglilianDoorPoint);
                            break;
                        case EVENT_SPECIAL:
                            if (Creature* Rattlegore = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_RATTLEGORE)))
                                Rattlegore->AI()->DoAction(ACTION_INTRO);

                            me->NearTeleportTo(LilianTeleportPos.GetPositionX(), LilianTeleportPos.GetPositionY(), LilianTeleportPos.GetPositionZ(), LilianTeleportPos.GetOrientation());
                            me->CastSpell(me, SPELL_LILIAN_KNELL, false);
                            fict = true;
                            me->SetHomePosition(LilianTeleportPos.GetPositionX(), LilianTeleportPos.GetPositionY(), LilianTeleportPos.GetPositionZ(), LilianTeleportPos.GetOrientation());
                            SetCombatMovement(false);
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_PACIFIED);

                            if (Creature* Gandling = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GRANDMASTER_GANDLING_INTRO)))
                                Gandling->CastSpell(me, SPELL_COSMETIC_SOUL_RIP, false);
                            break;
                        case EVENT_INTRO_DONE:
                            me->SetFaction(14);
                            break;
                        case EVENT_PURIFIED:
                            JustDied(me);
                            break;
                        case EVENT_DIE_NECROMANCE:
                            if (Creature* Gandling = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_GRANDMASTER_GANDLING_INTRO)))
                            {
                                Gandling->AI()->Talk(TALK_4);
                                me->GetMotionMaster()->MoveJump(Gandling->GetPositionX(), Gandling->GetPositionY(), 107.76f, 20.0f, 30.0f, EVENT_JUMP);
                                Gandling->AI()->DoAction(ACTION_REMOVE_LILIAN_SOUL);
                            }

                            me->setRegeneratingHealth(false);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;


                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_SHIV:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                                me->GetMotionMaster()->MoveJump(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 10.0f, 10.0f, EVENT_JUMP);

                            events.ScheduleEvent(EVENT_SHADOW_SHIV, urand(16700, 20000));
                            break;
                        case EVENT_DEATHS_GRASP:
                            DoCast(me, SPELL_DEATHS_GRASP);
                            events.ScheduleEvent(EVENT_DARK_BLAZE, 3500);
                            events.ScheduleEvent(EVENT_DEATHS_GRASP, 40000);
                            break;
                        case EVENT_DARK_BLAZE:
                            DoCastAOE(SPELL_DARK_BLAZE);
                            break;
                        case EVENT_SUMMON_SOUL:
                            me->SummonCreature(NPC_LILIAN_SOUL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
                            break;
                        case EVENT_SOUL_DISMISS:
                            Talk(TALK_4);
                            Talk(TALK_13);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                            me->SetStandState(UNIT_STAND_STATE_DEAD);
                            events.ScheduleEvent(EVENT_SUMMON_SOUL, 4000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
                EnterEvadeIfOutOfCombatArea(diff, 43.0f);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_lilian_vossAI(creature);
        }
};

// Lilian soul 58791
class npc_lilian_soul : public CreatureScript
{
    public:
        npc_lilian_soul() : CreatureScript("npc_lilian_soul") { }

        struct npc_lilian_soulAI : public ScriptedAI
        {
            npc_lilian_soulAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap events;
            InstanceScript* instance;
            bool OnlyMe, BreakDamage;

            void Reset() override { }

            void IsSummonedBy(Unit* summoner) override 
            {
                SetEquipmentSlots(false, LilianSword, LilianSword, EQUIP_NO_CHANGE);
                DoZoneInCombat();
                OnlyMe = false;
                BreakDamage = false;
                DoCast(me, SPELL_UNLEASHED_ANGUISH);
                events.ScheduleEvent(EVENT_FIXATE_ANGER, urand(4000, 5000));
                instance = me->GetInstanceScript();

                if (Player* itr = me->FindNearestPlayer(100.0f))
                    me->Attack(itr, true);
            }

            void DoAction(int32 actionId) override 
            {
                if (actionId == ACTION_REMOVE_LILIAN_SOUL)
                    me->DespawnOrUnsummon();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (BreakDamage)
                    damage = 0;

                if (!OnlyMe && me->GetHealth() <= damage)
                {
                    damage = 0;
                    OnlyMe = true;
                    BreakDamage = true;
                    me->GetMotionMaster()->MoveRandom(4.0f);
                    events.Reset();
                    me->RemoveAllAuras();
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                    events.ScheduleEvent(EVENT_RESSURECT, 5000);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIXATE_ANGER:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f, true))
                            {
                                me->GetThreatManager().addThreat(target, 10000.0f);
                                me->AddAura(SPELL_FIXATE_ANGER, target);
                            }
                            events.ScheduleEvent(EVENT_FIXATE_ANGER, 10000);
                            break;
                        case EVENT_RESSURECT:
                            me->SetHealth(me->GetMaxHealth());
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->GetMotionMaster()->MoveIdle();
                            me->GetMotionMaster()->Clear();

                            if (instance)
                                if (Creature* Lilian = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LILIAN_VOSS)))
                                    Lilian->AI()->DoAction(ACTION_LILIAN_RESSURECTED);

                            DoCast(me, SPELL_UNLEASHED_ANGUISH);

                            if (Player* itr = me->FindNearestPlayer(100.0f))
                            {
                                me->GetMotionMaster()->MoveChase(itr);
                                me->Attack(itr, true);
                            }
                            events.ScheduleEvent(EVENT_FIXATE_ANGER, urand(4000, 5000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lilian_soulAI(creature);
        }
};

// Dark Blaze 111609
class spell_dark_blaze : public SpellScriptLoader
{
    public:
        spell_dark_blaze() : SpellScriptLoader("spell_dark_blaze") { }

        class spell_dark_blaze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dark_blaze_SpellScript);

            void HandleOnCast()
            {
                if (GetCaster())
                    if (Creature* Trigger = GetCaster()->SummonCreature(NPC_DARK_BLAZE, GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ(), GetCaster()->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * IN_MILLISECONDS)))
                        GetCaster()->AddAura(SPELL_DARK_BLAZE_DUMMY, Trigger);
            }

            void Register() override
            {
                OnCast += SpellCastFn(spell_dark_blaze_SpellScript::HandleOnCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_dark_blaze_SpellScript();
        }
};

class TargetNotPlayer
{
    public:
        TargetNotPlayer() { }
        bool operator()(WorldObject* object)
        {
            if (object->GetTypeId() != TYPEID_PLAYER)
                return true;
            else
                return false;
        }
};

// Blazing soul 111642
class spell_blazing_soul : public SpellScriptLoader
{
    public:
        spell_blazing_soul() : SpellScriptLoader("spell_blazing_soul") { }

        class spell_blazing_soul_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blazing_soul_SpellScript);

            void CorrectTargets(std::list<WorldObject*>& targets)
            {
                if (targets.empty())
                    return;

                targets.remove_if (TargetNotPlayer());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_blazing_soul_SpellScript::CorrectTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_blazing_soul_SpellScript();
        }
};

// Deaths grasp 111570
class spell_deaths_grasp : public SpellScriptLoader
{
    public:
        spell_deaths_grasp() : SpellScriptLoader("spell_deaths_grasp") { }

        class spell_deaths_grasp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deaths_grasp_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DEATHS_GRASP_EFFECT))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (GetCaster() && GetHitUnit())
                    GetHitUnit()->CastSpell(GetCaster(), SPELL_DEATHS_GRASP_EFFECT, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_deaths_grasp_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_deaths_grasp_SpellScript();
        }
};

void AddSC_boss_lilian_voss()
{
    new boss_lilian_voss();
    new npc_lilian_soul();
    new spell_dark_blaze();
    new spell_blazing_soul();
    new spell_deaths_grasp();
}
