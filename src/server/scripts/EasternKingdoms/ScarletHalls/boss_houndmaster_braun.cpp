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
#include "scarlet_halls.h"

enum spells
{
    SPELL_PIERCING_THROW          = 114004,
    SPELL_DEATH_BLOSSOM           = 114242, // damage in 8y
    SPELL_DEATH_BLOSSOM_LEAP      = 114241, // only leap to target
    SPELL_BLOODY_MESS             = 114056,
    SPELL_BLOODY_RAGE             = 116140, // at 50% healh
    SPELL_RAKE_NORMAL             = 24331,
    SPELL_RAKE_HEROIC             = 120560,
    SPELL_HOUNDMASTER_BRAUN_EATEN = 122916
};

enum Yells
{
    TALK_INTO                     = 0,
    TALK_SUDENDEATH_01            = 1,
    TALK_SUDENDEATH_02            = 2,
    TALK_DEATH                    = 3,
    TALK_DOGFAIL                  = 4,
};

enum events
{
    /*Braun*/
    EVENT_PIERCING_THROW          = 1,
    EVENT_DEATH_BLOSSOM           = 2,
    EVENT_CALL_DOG                = 3,
    EVENT_AT_END                  = 4,
    EVENT_CALL_DOG_INTO           = 5,

    /*Dogs*/
    EVENT_RAKE                    = 1,
    EVENT_RAKE_AT_END             = 2
};

enum  Actions
{
    ACTION_ACTIVATE_DOG           = 1,
    ACTION_FINISH_HIM             = 2,
    ACTION_MOVE_TO                = 3,
    ACTION_EATING                 = 4
};

Creature* SelectedObedientHound(WorldObject* owner)
{
    std::list<Creature*> ObediendHounds;
    GetCreatureListWithEntryInGrid(ObediendHounds, owner, NPC_OBEDIEND_HOUND, 200.0f);

    return Trinity::Containers::SelectRandomContainerElement(ObediendHounds);
}

class boss_houndmaster_braun : public CreatureScript
{
    public:
        boss_houndmaster_braun() : CreatureScript("boss_houndmaster_braun") { }

        struct boss_houndmaster_braunAI : public BossAI
        {
            boss_houndmaster_braunAI(Creature* creature) : BossAI(creature, BOSS_HOUNDMASTER_BRAUN) { }

            bool GetRage;
            bool Scenario;
            bool SudenDeath;
            int Phase;

            void InitializeAI() override
            {
                me->setActive(true);
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                me->setRegeneratingHealth(true);
                me->SetReactState(REACT_AGGRESSIVE);
                GetRage = false;
                Scenario = false;
                SudenDeath = false;
                Phase = 0;
                HandleDoors(true);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_DEATH);
                if (instance)
                {
                    instance->SetData(BOSS_HOUNDMASTER_BRAUN, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }
                me->RemoveAllAuras();
                HandleSendActionOnHounds(ACTION_MOVE_TO);
                HandleDoors(true);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(BOSS_HOUNDMASTER_BRAUN, FAIL);
                }
                summons.DespawnAll();
                HandleDoors(true);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_INTO);
                if (instance)
                {
                    instance->SetData(BOSS_HOUNDMASTER_BRAUN, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                events.ScheduleEvent(EVENT_PIERCING_THROW, 7000);
                events.ScheduleEvent(EVENT_DEATH_BLOSSOM, urand(11000, 12000));
                HandleDoors();
            }

            void HandleDoors(bool reset = false)
            {
                if (instance)
                    if (GameObject* Idoor = GetClosestGameObjectWithEntry(me, GO_COMANDER_LINDON_EXIT, 150.0f))
                        instance->HandleGameObject(ObjectGuid::Empty, reset, Idoor);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(50) && !GetRage) 
                { 
                    me->CastSpell(me, SPELL_BLOODY_RAGE, false); 
                    GetRage = true; 
                    Talk(TALK_DOGFAIL); 
                }

                if (HealthBelowPct(15) && !Scenario)
                {
                    damage = 0;
                    Scenario = true;
                    Talk(TALK_SUDENDEATH_01);
                    events.Reset();
                    events.ScheduleEvent(EVENT_AT_END, 5000);
                    me->RemoveAllAuras();
                    HandleSendActionOnHounds(ACTION_FINISH_HIM);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->AttackStop();

                }

                if ((Phase == 0 && HealthBelowPct(90))
                    || (Phase == 1 && HealthBelowPct(80))
                    || (Phase == 2 && HealthBelowPct(70))
                    || (Phase == 3 && HealthBelowPct(60)))
                {
                    Phase++;
                }            
            }

            void HandleSendActionOnHounds(uint32 actionId)
            {
                std::list<Creature*> ObedientHounds;
                GetCreatureListWithEntryInGrid(ObedientHounds, me, NPC_OBEDIEND_HOUND, 200.0f);

                for (auto&& itr : ObedientHounds)
                    itr->AI()->DoAction(actionId);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PIERCING_THROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                me->CastSpell(target, SPELL_PIERCING_THROW, false);
                            events.ScheduleEvent(EVENT_PIERCING_THROW, 7000);
                            break;
                        case EVENT_DEATH_BLOSSOM:
                            if (Unit* target = me->SelectNearestTarget())
                                me->CastSpell(target, SPELL_DEATH_BLOSSOM_LEAP, false);
                            events.ScheduleEvent(EVENT_DEATH_BLOSSOM, urand(11000, 12000));
                            break;
                        case EVENT_AT_END:
                            if (!SudenDeath)
                            {
                                me->CastSpell(me, SPELL_HOUNDMASTER_BRAUN_EATEN, false);
                                Talk(TALK_SUDENDEATH_02);
                                SudenDeath = true;
                                HandleSendActionOnHounds(ACTION_EATING);
                                events.ScheduleEvent(EVENT_AT_END, urand(7000, 10000));
                                return;
                            }

                            if (SudenDeath)
                                if (Player* originalCaster = me->FindNearestPlayer(50.0f))
                                {
                                    me->SetLootRecipient(originalCaster);
                                    originalCaster->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                                    events.Reset();
                                }
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_houndmaster_braunAI(creature);
        }
};

// Obediend Hound 59309
class npc_obediend_hound : public CreatureScript
{
    public:
        npc_obediend_hound() : CreatureScript("npc_obediend_hound") { }

        struct npc_obediend_houndAI : public ScriptedAI
        {
            npc_obediend_houndAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            EventMap events;

            void IsSummonedBy(Unit* summoner) override { }

            void Reset() override 
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (type == POINT_MOTION_TYPE && pointId == 1)
                    KillThemAll();
            }

            void KillThemAll()
            {
                std::list<Creature*> Guardians;
                GetCreatureListWithEntryInGrid(Guardians, me, NPC_SCARLET_GUARDIAN, 15.0f);

                for (auto&& itr : Guardians)
                    itr->DealDamage(itr, itr->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                if (Creature* Verdone = GetClosestCreatureWithEntry(me, NPC_SERGEANT_VERDONE, 20.0f, true))
                    Verdone->DealDamage(Verdone, Verdone->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->DespawnOrUnsummon(1500);
            }

            void JustDied(Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_HUMANE_SOCIETY, 0);
            }

            void DoAction(int32 actionId) override 
            {
                switch (actionId)
                {
                    case ACTION_ACTIVATE_DOG:
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Unit* target = me->SelectNearestTarget(100.0f))
                        {
                            me->GetMotionMaster()->MoveChase(target);
                            me->Attack(target, true);
                        }
                        break;
                    case ACTION_FINISH_HIM:
                        events.Reset();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
                        me->SetFaction(35);

                        if (_instance)
                        {
                            if (Unit* Braun = ObjectAccessor::GetUnit(*me, _instance->GetGuidData(BOSS_HOUNDMASTER_BRAUN)))
                            {
                                me->Attack(Braun, true);
                                me->GetMotionMaster()->MoveChase(Braun);
                            }
                        }

                        events.ScheduleEvent(EVENT_RAKE, 3500);
                        break;
                    case ACTION_MOVE_TO:
                        events.Reset();
                        me->GetMotionMaster()->MovePoint(1, EndScenario);
                        break;
                    case ACTION_EATING:
                        if (_instance)
                            if (Unit* Braun = ObjectAccessor::GetUnit(*me, _instance->GetGuidData(BOSS_HOUNDMASTER_BRAUN)))
                                me->CastSpell(Braun, IsHeroic() ? SPELL_RAKE_HEROIC : SPELL_RAKE_NORMAL, true);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override 
            {
                events.Update(diff);

                if (!UpdateVictim())
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                    if (eventId == EVENT_RAKE)
                    {
                        if (Unit* target = me->GetVictim())
                            me->CastSpell(target, IsHeroic() ? SPELL_RAKE_HEROIC : SPELL_RAKE_NORMAL, true);

                        events.ScheduleEvent(EVENT_RAKE, 3500);
                    }
                   
                DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_obediend_houndAI(creature);
        }
};

// Bloody mess 114020
class spell_bloody_mess : public SpellScriptLoader
{
    public:
        spell_bloody_mess() : SpellScriptLoader("spell_bloody_mess") { }

        class spell_bloody_mess_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_bloody_mess_SpellScript);

            void HandleOnHit()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;
        
                std::list<Player*> PlayersInArea;
                GetPlayerListInGrid(PlayersInArea, caster, 100.0f);

                for (auto&& itr : PlayersInArea)
                {
                    if (itr->IsInBetween(caster, target, 3.0f) && itr->GetGUID() != target->GetGUID())
                        caster->AddAura(SPELL_BLOODY_MESS, itr);
                }
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_bloody_mess_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_bloody_mess_SpellScript();
        }
};

// Crushing Leap Visual 108481
class spell_crushing_leap_visual : public SpellScriptLoader
{
    public:
        spell_crushing_leap_visual() : SpellScriptLoader("spell_crushing_leap_visual") { }

        class spell_crushing_leap_visual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_crushing_leap_visual_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* m_caster = GetCaster())
                    m_caster->CastSpell(m_caster, SPELL_DEATH_BLOSSOM, false);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_crushing_leap_visual_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_MELEE_HASTE_2, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_crushing_leap_visual_AuraScript();
        }
};

void AddSC_boss_houndmaster_braun()
{
    new boss_houndmaster_braun();
    new npc_obediend_hound(); 
    new spell_bloody_mess();
    new spell_crushing_leap_visual();
}
