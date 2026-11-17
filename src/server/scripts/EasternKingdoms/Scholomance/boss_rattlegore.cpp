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

enum Spells
{
    SPELL_RUSTING             = 113763,
    SPELL_BONE_SPIKE          = 113999,
    SPELL_SOULFLAME           = 114007,
    SPELL_SOULFLAME_DMG       = 114009,
    SPELL_BONE_ARMOR          = 113996,

    // cosmetic
    SPELL_BONE_PILE           = 113995,
    SPELL_SOUL_FLAME_COSMETIC = 114057,
};

enum Actions
{
    ACTION_ACTIVATE           = 0,
    ACTION_DEACTIVATE         = 1
};

enum Events
{
    EVENT_SOULFLAME           = 1,
    EVENT_BONE_SPIKE          = 2,
    EVENT_CHECK_BONEWEAVER    = 3,
    EVENT_SPECIAL             = 4,
    EVENT_REMOVE_SOULFLAME    = 5,
};

enum Texts
{
    TALK_SUMMONED = 0,
    TALK_BONE_PILE_ANN,
};

class boss_rattlegore : public CreatureScript
{
    public:
        boss_rattlegore() : CreatureScript("boss_rattlegore") { }

        struct boss_rattlegoreAI : public BossAI
        {
            boss_rattlegoreAI(Creature* creature) : BossAI(creature, DATA_RATTLEGORE) { }

            std::list <Creature*> Stalker;
            std::list<ObjectGuid> m_uSoul;
            EventMap nonCombatEvents;

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);

                if (instance)
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONE_ARMOR);

                RemoveSoulFlame();
                Stalker.clear();
                m_uSoul.clear();
                HandleDoors(true);
                me->GetMap()->SetWorldState(WORLDSTATE_RATTLE_NO_MORE, 1);
            }

            void InitializeAI() override 
            {
                me->SetDisplayId(11686); // invisible
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->GetMap()->SetWorldState(WORLDSTATE_RATTLE_NO_MORE, 1);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                //Talk(TALK_DEATH);
                if (instance)
                {
                    instance->SetData(DATA_RATTLEGORE, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONE_ARMOR);
                }
                SetBonePile();
                RemoveSoulFlame();
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_RATTLEGORE, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BONE_ARMOR);
                }
                summons.DespawnAll();
                RemoveSoulFlame();
                HandleDoors(true);
            }

            void HandleDoors(bool reset = false)
            {
                if (instance)
                    if (GameObject* Idoor = GetClosestGameObjectWithEntry(me, GO_JANDICE_TO_RATTLEGORE_DOOR, 150.0f))
                        instance->HandleGameObject(ObjectGuid::Empty, reset, Idoor);
            }

            void SetBonePile(bool activate = false)
            {
                std::list<Creature*> BonePilles;
                GetCreatureListWithEntryInGrid(BonePilles, me, NPC_BONE_PILE, 200.0f);

                for (auto&& itr : BonePilles)
                    itr->SetVisible(activate);
            }

            void RemoveSoulFlame()
            {
                for (auto&& itr : m_uSoul)
                    if (Creature* u_SoulFlame = ObjectAccessor::GetCreature(*me, itr))
                        u_SoulFlame->RemoveAura(SPELL_SOULFLAME);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                SetBonePile(true);
                Talk(TALK_BONE_PILE_ANN);
                if (instance)
                {
                    instance->SetData(DATA_RATTLEGORE, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                me->CastSpell(me, SPELL_RUSTING, false);
                events.ScheduleEvent(EVENT_SOULFLAME, urand(3500, 6000));
                events.ScheduleEvent(EVENT_BONE_SPIKE, 10000);
                HandleDoors();
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                Talk(TALK_SUMMONED);
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTRO)
                    nonCombatEvents.ScheduleEvent(EVENT_CHECK_BONEWEAVER, 1000);
            }
        
            void MovementInform(uint32 /*type*/, uint32 /*pointId*/) override { }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override { }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_BONEWEAVER:
                            if (Creature* Boneweaver = GetClosestCreatureWithEntry(me, NPC_BONEWEAVER, 60.0f, true))
                            {
                                nonCombatEvents.ScheduleEvent(EVENT_CHECK_BONEWEAVER, urand(100, 200));
                                return;
                            }
                            me->CastSpell(me, SPELL_BONE_WHIRL_WIND, false);
                            me->CastSpell(me, SPELL_BONE_STORM_COSMETIC, false);
                            nonCombatEvents.ScheduleEvent(EVENT_SPECIAL, 5000);
                            break;
                        case EVENT_SPECIAL:
                            me->RemoveAllAuras();
                            me->SetReactState(REACT_AGGRESSIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            me->SetDisplayId(me->GetNativeDisplayId());
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
                        case EVENT_SOULFLAME:
                        {
                            std::list<Creature*> soul_flame_triggers;
                            me->GetCreatureListWithEntryInGrid(soul_flame_triggers, NPC_SOULFLAME_ACTIVATED_STALKER, 150.0F);
                            m_uSoul.clear();
                            for (uint32 i = 0; i < 2; i++)
                            {
                                auto it = soul_flame_triggers.begin();
                                if ((*it)->HasAura(SPELL_SOULFLAME))
                                    soul_flame_triggers.erase(it);
                                else
                                    std::advance(it, urand(0, soul_flame_triggers.size() - 1));

                                (*it)->AddAura(SPELL_SOULFLAME, (*it));
                                m_uSoul.push_back((*it)->GetGUID());
                            }
                            events.ScheduleEvent(EVENT_REMOVE_SOULFLAME, 15000);
                            events.ScheduleEvent(EVENT_SOULFLAME, urand(18000, 22000));
                            break;
                        }
                        case EVENT_BONE_SPIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_BONE_SPIKE, false);
                            events.ScheduleEvent(EVENT_BONE_SPIKE, 15000);
                            break;
                        case EVENT_REMOVE_SOULFLAME:
                            RemoveSoulFlame();
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_rattlegoreAI(creature);
        }
};

// Bone pile 59304
class npc_bone_pile : public CreatureScript
{
    public:
        npc_bone_pile() : CreatureScript("npc_bone_pile") { }

        struct npc_bone_pileAI : public ScriptedAI
        {
            npc_bone_pileAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override { }

            void InitializeAI() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void MovementInform(uint32 /*type*/, uint32 /*pointId*/) override { }

            void DoAction(int32 /*actionId*/) override { }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (!clicker->HasAura(SPELL_BONE_ARMOR))
                    clicker->AddAura(SPELL_BONE_ARMOR, clicker);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/) override { }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (!UpdateVictim())
                    return;
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bone_pileAI(creature);
        }
};

// Bone Spike 113999
class spell_bone_spike : public SpellScriptLoader
{
    public:
        spell_bone_spike() : SpellScriptLoader("spell_bone_spike") { }

        class spell_bone_spike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bone_spike_SpellScript);

            void RecalculateDamage(SpellEffIndex /*effIndex*/)
            {
                if (GetSpellInfo() && GetSpellInfo()->Id == SPELL_BONE_SPIKE)
                {
                    if (GetHitUnit() && GetHitUnit()->HasAura(SPELL_BONE_ARMOR))
                    {
                        SetHitDamage(0);
                        GetHitUnit()->RemoveAura(SPELL_BONE_ARMOR);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_bone_spike_SpellScript::RecalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bone_spike_SpellScript();
        }
};

// Soul flame dmg 114009
class spell_soul_flame : public SpellScriptLoader
{
    public:
        spell_soul_flame() : SpellScriptLoader("spell_soul_flame") { }

        class spell_soul_flame_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_soul_flame_SpellScript);

            void CorrectRange(std::list<WorldObject*>& targets)
            {
                targets.clear();

                float scale = GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE);
                float MaxDist = 2.7f + scale;
                float MinDist = 0.0f;

                Map::PlayerList const& players = GetCaster()->GetMap()->GetPlayers();
                if (!players.isEmpty())
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        if (Player* player = itr->GetSource())
                            if (player->GetExactDist2d(GetCaster()->GetPositionX(), GetCaster()->GetPositionY()) <= MaxDist &&
                                player->GetExactDist2d(GetCaster()->GetPositionX(), GetCaster()->GetPositionY()) >= MinDist)
                                targets.push_back(player);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_soul_flame_SpellScript::CorrectRange, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_soul_flame_SpellScript();
        }
};

// Rusting 113765
class spell_rattlegore_rusting : public AuraScript
{
    PrepareAuraScript(spell_rattlegore_rusting);

    void HandleAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (GetStackAmount() > 5 && GetOwner() && GetOwner()->ToUnit())
            GetOwner()->ToUnit()->GetMap()->SetWorldState(WORLDSTATE_RATTLE_NO_MORE, 0);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_rattlegore_rusting::HandleAuraEffectApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAPPLY);
    }
};

void AddSC_boss_rattlegore()
{
    new boss_rattlegore();
    new npc_bone_pile();
    new spell_bone_spike();
    new spell_soul_flame();
    new aura_script<spell_rattlegore_rusting>("spell_rattlegore_rusting");
}