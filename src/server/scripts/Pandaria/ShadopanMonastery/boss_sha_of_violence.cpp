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
#include "shadopan_monastery.h"

enum Spells
{
    SPELL_SMOKE_BLADES       = 106826,
    SPELL_SHA_SPIKE          = 106871,
    SPELL_DISORIENTING_SMASH = 106872,
    SPELL_ENRAGE             = 130196,
    SPELL_ICE_TRAP           = 110610,

    SPELL_SHA_FLAY           = 110119,
    SPELL_SHA_BLAST          = 110117,
};

enum Events
{
    EVENT_SMOKE_BLADES = 1,
    EVENT_SHA_SPIKE,
    EVENT_DISORIENTING_SMASH,

    // Misc
    EVENT_SHA_BLAST,
    EVENT_SHA_FLAY,
};

class boss_sha_of_violence : public CreatureScript
{
    public:
        boss_sha_of_violence() : CreatureScript("boss_sha_of_violence") { }

        enum eTalks
        {
            TALK_AGGRO = 0,
            TALK_DEATH = 1,
            TALK_SLAY  = 2
        };

        struct boss_sha_of_violenceAI : public BossAI
        {
            boss_sha_of_violenceAI(Creature* creature) : BossAI(creature, DATA_SHA_VIOLENCE) { }

            bool enrageDone;
            uint32 shaCount;
            std::vector<ObjectGuid> prisonedShaGUIDs;

            void InitializeAI() override
            {
                me->setActive(true);
                SetCombatMovement(false);
                Reset();
            }

            void Reset() override
            {
                _Reset();
                enrageDone = false;
                prisonedShaGUIDs.clear();
                summons.DespawnAll();

                shaCount = 0;
                me->GetMap()->SetWorldState(WORLDSTATE_OBVIOUS_SOLUTION, 0);

                for (auto&& itr : invPrisonedShaType)
                    me->SummonCreature(itr.first, itr.second, TEMPSUMMON_MANUAL_DESPAWN);
            }
             
            void JustEngagedWith(Unit* who) override
            {
                _JustEngagedWith();
                Talk(TALK_AGGRO);
                if (instance)
                {
                    instance->SetData(DATA_SHA_VIOLENCE, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                events.ScheduleEvent(EVENT_SMOKE_BLADES, urand(25000, 35000));
                events.ScheduleEvent(EVENT_SHA_SPIKE, urand(10000, 20000));
                events.ScheduleEvent(EVENT_DISORIENTING_SMASH, 13 * IN_MILLISECONDS);
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_OBVIOUS_SOLUTION && ++shaCount > 4)
                    me->GetMap()->SetWorldState(WORLDSTATE_OBVIOUS_SOLUTION, 1);
            }

            void DoAction(int32 actionId) override
            {
                for (auto&& itr : prisonedShaGUIDs)
                    if (Creature* prisonedSha = ObjectAccessor::GetCreature(*me, itr))
                        prisonedSha->SetInCombatWithZone();
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                summons.DespawnAll();
                Talk(TALK_DEATH);

                if (instance)
                {
                    instance->SetData(DATA_SHA_VIOLENCE, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_SHA_VIOLENCE, FAIL);
                    instance->SetBossState(DATA_SHA_VIOLENCE, FAIL);

                    if (GameObject* prison1 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_SHA_PRISON_1)))
                        prison1->SetGoState(GO_STATE_READY);

                    if (GameObject* prison2 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_SHA_PRISON_2)))
                        prison2->SetGoState(GO_STATE_READY);

                    if (GameObject* prison3 = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_SHA_PRISON_3)))
                        prison3->SetGoState(GO_STATE_READY);

                    if (GameObject* button = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(DATA_CONTAINMENT_OVERRIDE)))
                    {
                        button->SetGoState(GO_STATE_READY);
                        button->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_IN_USE);
                        button->SetLootState(GO_NOT_READY);
                    }
                }
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                if (summon->GetEntry() != NPC_VOLATILE_ENERGY && summon->GetEntry() != NPC_DESTROYING_SHA && summon->GetEntry() != NPC_SHA_MANIFESTATION)
                    summon->CastSpell(summon, SPELL_ICE_TRAP, true);
                else
                    prisonedShaGUIDs.push_back(summon->GetGUID());
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (!enrageDone && me->HealthBelowPctDamaged(20, damage))
                {
                    enrageDone = true;
                    me->CastSpell(me, SPELL_ENRAGE, true);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_SMOKE_BLADES:
                        me->CastSpell(me, SPELL_SMOKE_BLADES, false);
                        events.ScheduleEvent(EVENT_SMOKE_BLADES, urand(25000, 35000));
                        break;
                    case EVENT_SHA_SPIKE:
                        if (auto const target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            me->CastSpell(target, SPELL_SHA_SPIKE, false);
                        events.ScheduleEvent(EVENT_SHA_SPIKE, urand(10000, 20000));
                        break;
                    case EVENT_DISORIENTING_SMASH:
                        if (auto const target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                            me->CastSpell(target, SPELL_DISORIENTING_SMASH, false);
                        events.ScheduleEvent(EVENT_DISORIENTING_SMASH, 13 * IN_MILLISECONDS);
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sha_of_violenceAI(creature);
        }
};

class npc_sv_lesser : public CreatureScript
{
    public:
        npc_sv_lesser() : CreatureScript("npc_sv_lesser") { }

        enum eSpells
        {
            SPELL_EXPLOSION = 106966
        };

        enum eEvents
        {
            EVENT_DIE = 1
        };

        struct npc_sv_lesser_AI : public ScriptedAI
        {
            npc_sv_lesser_AI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;
            bool canDead;

            void InitializeAI() override
            {
                canDead = false;
                events.ScheduleEvent(EVENT_DIE, 30 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (me->GetHealth() <= damage)
                {
                    damage = 0;
                    if (!canDead)
                    {
                        canDead = true;
                        me->CastSpell((Unit*)NULL, SPELL_EXPLOSION, false);
                        attacker->Kill(me);
                        me->DespawnOrUnsummon(2 * IN_MILLISECONDS);
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_DIE)
                    me->DealDamage(me, me->GetHealth());

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sv_lesser_AI(creature);
        }
};

// Prisoned Sha:: Volatile Energy 56766, Sha Manifestation 58201
struct npc_achiev_prisoned_sha : public ScriptedAI
{
    npc_achiev_prisoned_sha(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (me->GetEntry() == NPC_SHA_MANIFESTATION)
        {
            events.ScheduleEvent(EVENT_SHA_BLAST, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            events.ScheduleEvent(EVENT_SHA_FLAY, 6 * IN_MILLISECONDS);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetDBTableGUIDLow())
            return;

        if (Creature* shaOfViolence = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SHA_VIOLENCE) : ObjectGuid::Empty))
            shaOfViolence->AI()->SetData(TYPE_OBVIOUS_SOLUTION, 1);
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
                case EVENT_SHA_BLAST:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHA_BLAST);

                    events.ScheduleEvent(EVENT_SHA_BLAST, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                    break;
                case EVENT_SHA_FLAY:
                    DoCast(me, SPELL_SHA_FLAY);
                    events.ScheduleEvent(EVENT_SHA_FLAY, 15.5 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

class SmokeBladesTargetSelector
{
    public:
        bool operator()(WorldObject* object) const
        {
            if (object->GetTypeId() == TYPEID_PLAYER && !object->ToPlayer()->HasAura(SPELL_SMOKE_BLADES))
                return false;

            return true;
        }

    private:
        enum eSpells
        {
            SPELL_SMOKE_BLADES = 106827
        };
};

class spell_sv_smoke_blades : public SpellScriptLoader
{
    public:
        spell_sv_smoke_blades() : SpellScriptLoader("spell_sv_smoke_blades") { }

        enum eSpells
        {
            SPELL_PARTING_SMOKE = 127576
        };

        class spell_sv_smoke_blades_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sv_smoke_blades_AuraScript);

            void HanleOnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {      
                if (auto const caster = eventInfo.GetActor())
                {
                    if (auto const smoke = caster->GetAura(aurEff->GetId()))
                    {
                        if (smoke->GetDuration() > 5 * IN_MILLISECONDS)
                            smoke->SetDuration(smoke->GetDuration() - 5 * IN_MILLISECONDS);
                        else
                            caster->RemoveAura(aurEff->GetId());
                    }
                }
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (auto const owner = GetOwner()->ToPlayer())
                {
                    owner->CastSpell(owner, SPELL_PARTING_SMOKE, false);
                }
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_sv_smoke_blades_AuraScript::HanleOnProc, EFFECT_1, SPELL_AURA_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_sv_smoke_blades_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_sv_smoke_blades_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sv_smoke_blades_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (SmokeBladesTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_sv_smoke_blades_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_sv_smoke_blades_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_sv_smoke_blades_SpellScript();
        }
};

// Containment Override 213457
class go_containment_override : public GameObjectScript
{
    public:
        go_containment_override() : GameObjectScript("go_containment_override") { }

        void OnGameObjectStateChanged(GameObject* go, uint32 state) override
        {
            if (state != GO_STATE_ACTIVE)
                return;

            if (GameObject* prison1 = ObjectAccessor::GetGameObject(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(DATA_SHA_PRISON_1) : ObjectGuid::Empty))
                prison1->SetGoState(GO_STATE_ACTIVE);

            if (GameObject* prison2 = ObjectAccessor::GetGameObject(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(DATA_SHA_PRISON_2) : ObjectGuid::Empty))
                prison2->SetGoState(GO_STATE_ACTIVE);

            if (GameObject* prison3 = ObjectAccessor::GetGameObject(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(DATA_SHA_PRISON_3) : ObjectGuid::Empty))
                prison3->SetGoState(GO_STATE_ACTIVE);

            // Force sha to attack players
            if (Creature* shaOfViolence = ObjectAccessor::GetCreature(*go, go->GetInstanceScript() ? go->GetInstanceScript()->GetGuidData(NPC_SHA_VIOLENCE) : ObjectGuid::Empty))
                shaOfViolence->AI()->DoAction(0);
        }
};

void AddSC_boss_sha_of_violence()
{
    new boss_sha_of_violence();
    new npc_sv_lesser();
    new creature_script<npc_achiev_prisoned_sha>("npc_achiev_prisoned_sha");
    new spell_sv_smoke_blades();
    new go_containment_override();
}
