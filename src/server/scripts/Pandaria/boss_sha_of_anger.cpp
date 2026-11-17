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
#include "SpellScript.h"

enum ShaOfAngerSpellData
{
    SPELL_SHADOW_BOLT_ANGER                 = 119487,
    SPELL_OVERCOME_BY_ANGER                 = 129356,
    SPELL_ENDLESS_RAGE                      = 119446,
    SPELL_BITTER_THOUGHTS                   = 119601,
    SPELL_BERSERK                           = 47008,
    SPELL_DOMINATE_MIND_WARNING             = 119622,
    SPELL_DOMINATE_MIND                     = 119626,
    SPELL_SEETHE_AURA                       = 119487,
    SPELL_BITTER_THOUGHTS_EFF               = 119610,
    SPELL_UNLEASHED_WRATH                   = 119488
};

enum ShaOfAngerTexts
{
    TALK_INTRO                              = 0,
    TALK_ANGER                              = 1,
    TALK_SPAWN                              = 2,
    TALK_RESET                              = 3,
    TALK_SLAY                               = 4,
    TALK_AGGRO                              = 5
};

enum ShaOfAngerEvents
{
    EVENT_GROWING_ANGER_WARNING             = 1,
    EVENT_GROWING_ANGER                     = 2,
    EVENT_UNLEASHED_WRATH                   = 3,
    EVENT_BERSERK                           = 4,
    EVENT_DESPAWN                           = 5,
    EVENT_SPAWN                             = 6,
    EVENT_UPDATE_RAGE                       = 7,
    EVENT_RANGE_ATTACK                      = 8
};

enum ShaOfAngerCreatures
{
    NPC_SHA_OF_ANGER                        = 56439,
    NPC_SHA_OF_ANGER_TRIGGER                = 61523
};

class boss_sha_of_anger : public CreatureScript
{
    public:
        boss_sha_of_anger() : CreatureScript("boss_sha_of_anger") { }

        struct boss_sha_of_anger_AI : public ScriptedAI
        {
            boss_sha_of_anger_AI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            int _targetCount;
            int _cloudCount;
            int _maxTargetCount;
            uint8 _dominateMindCount;
            uint32 timer;
            bool phase1;
            bool range;
            std::list<ObjectGuid> targetedDominationPlayerGuids;
            EventMap events;
            SummonList summons;

            void Reset() override
            {
                SetCombatMovement(false);
                me->SetPowerType(POWER_RAGE);
                me->SetPower(POWER_RAGE, 0);

                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 37.0f);
                me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 37.0f);

                summons.DespawnAll();

                phase1 = true;
                range = false;
                _dominateMindCount = 2;
                _cloudCount = 3;
                _targetCount = 0;
                _maxTargetCount = 5;
                timer = 0;

                Talk(TALK_RESET);

                events.Reset();
                events.ScheduleEvent(EVENT_GROWING_ANGER_WARNING, 19s);
                events.ScheduleEvent(EVENT_SPAWN, 8s);
                events.ScheduleEvent(EVENT_UNLEASHED_WRATH, 1min + 12s);
                events.ScheduleEvent(EVENT_BERSERK, 15min);
                events.ScheduleEvent(EVENT_UPDATE_RAGE, 4s);

                targetedDominationPlayerGuids.clear();

                std::list<Player*> playerList;
                GetPlayerListInGrid(playerList, me, 100.0f);

                for (auto&& itr : playerList)
                    itr->RemoveAura(SPELL_DOMINATE_MIND);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SLAY);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_AGGRO);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                // Clouds
                if (summon->GetEntry() == NPC_SHA_OF_ANGER_TRIGGER)
                {
                    summon->CastSpell(summon, SPELL_BITTER_THOUGHTS, true);
                    summon->DespawnOrUnsummon(60 * IN_MILLISECONDS);
                }
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GROWING_ANGER_WARNING:
                        {
                            Talk(TALK_ANGER);

                            for (uint8 i = 0; i < _dominateMindCount; ++i)
                            {
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                                {
                                    targetedDominationPlayerGuids.push_back(target->GetGUID());
                                    me->CastSpell(target, SPELL_DOMINATE_MIND_WARNING, true);
                                }
                            }

                            events.ScheduleEvent(EVENT_GROWING_ANGER, 6s);
                            break;
                        }
                        case EVENT_GROWING_ANGER:
                        {
                            for (auto&& guid : targetedDominationPlayerGuids)
                                if (Player* target = ObjectAccessor::GetPlayer(*me, guid))
                                    if (me->GetVictim() || target != me->GetVictim())
                                        me->CastSpell(target, SPELL_DOMINATE_MIND, false);

                            events.ScheduleEvent(EVENT_GROWING_ANGER_WARNING, 19s);
                            break;
                        }
                        case EVENT_SPAWN:
                        {
                            Talk(TALK_SPAWN);

                            for (uint8 i = 0; i < _cloudCount; ++i)
                                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
                                    me->CastSpell(target, SPELL_ENDLESS_RAGE, false);

                            events.ScheduleEvent(EVENT_SPAWN, 15s);
                            break;
                        }
                        case EVENT_UPDATE_RAGE:
                        {
                            if (phase1)
                                timer += 20;
                            else
                                timer -= 20;

                            switch (me->GetPower(POWER_RAGE))
                            {
                                case 0:
                                    phase1 = true;
                                    break;
                                case 1000:
                                    if (!phase1)
                                        break;

                                    phase1 = false;
                                    DoCast(me, SPELL_UNLEASHED_WRATH);
                                    break;
                            }

                            me->SetPower(POWER_RAGE, timer > 100 ? 100 : timer < 0 ? 0 : timer);
                            events.ScheduleEvent(EVENT_UPDATE_RAGE, 1s);
                            break;
                        }
                        case EVENT_BERSERK:
                        {
                            me->CastSpell(me, SPELL_BERSERK, false);
                            break;
                        }
                        case EVENT_RANGE_ATTACK:
                        {
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                me->CastSpell(target, SPELL_SHADOW_BOLT_ANGER, false);
                                me->AddAura(SPELL_SEETHE_AURA, target);
                            }

                            range = false;
                            break;
                        }
                        default:
                            break;
                    }
                }

                if (me->GetVictim() && !me->IsWithinMeleeRange(me->GetVictim(), 37.5f) && !range)
                {
                    range = true;
                    events.ScheduleEvent(EVENT_RANGE_ATTACK, 2s);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_sha_of_anger_AI(creature);
        }
};

struct npc_sha_of_anger_bunny : public ScriptedAI
{
    npc_sha_of_anger_bunny(Creature* creature) : ScriptedAI(creature)
    {
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
        creature->GetMotionMaster()->Clear();
    }

    void Reset() override
    {
        me->GetMotionMaster()->Clear();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

        if (me->AI())
            me->AI()->SetCanSeeEvenInPassiveMode(true);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!who->IsWithinDist(me, 50.0f))
            return;

        if (who->IsWithinDist(me, 30.0f))
            who->AddAura(SPELL_OVERCOME_BY_ANGER, who);
        else if (who->HasAura(SPELL_OVERCOME_BY_ANGER))
            who->RemoveAura(SPELL_OVERCOME_BY_ANGER);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (me->AI() && !me->AI()->CanSeeEvenInPassiveMode())
            me->AI()->SetCanSeeEvenInPassiveMode(true);

        if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

        if (me->isMoving())
            me->StopMoving();
    }
};

class spell_sha_of_anger_aggressive_behaviour : public AuraScript
{
    PrepareAuraScript(spell_sha_of_anger_aggressive_behaviour);

    void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        if (Unit* target = GetTarget())
            if (target->GetHealthPct() < 50.0f)
                this->Remove(AURA_REMOVE_BY_DEFAULT);
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
        {
            if (!target->ToPlayer())
                return;

            target->SetPvP(true);
            target->SetFaction(16);
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
        {
            target->RestoreFaction();
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_sha_of_anger_aggressive_behaviour::HandlePeriodicTick, EFFECT_5, SPELL_AURA_PERIODIC_DUMMY);
        OnEffectApply += AuraEffectApplyFn(spell_sha_of_anger_aggressive_behaviour::OnApply, EFFECT_5, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_sha_of_anger_aggressive_behaviour::OnRemove, EFFECT_5, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class spell_sha_of_anger_overcome_by_anger : public AuraScript
{
    PrepareAuraScript(spell_sha_of_anger_overcome_by_anger);

    void OnUpdate(uint32 /*diff*/)
    {
        if (Unit* target = GetTarget())
        {
            if (Unit* caster = GetCaster())
            {
                if (target->GetMapId() != caster->GetMapId() || !target->IsWithinDist(caster, 30.0f))
                    target->RemoveAura(SPELL_OVERCOME_BY_ANGER);
            }
        }
    }

    void Register() override
    {
        OnAuraUpdate += AuraUpdateFn(spell_sha_of_anger_overcome_by_anger::OnUpdate);
    }
};

void AddSC_boss_sha_of_anger()
{
    new boss_sha_of_anger();
    new creature_script<npc_sha_of_anger_bunny>("npc_sha_of_anger_bunny");
    new aura_script<spell_sha_of_anger_aggressive_behaviour>("spell_sha_of_anger_aggressive_behaviour");
    new aura_script<spell_sha_of_anger_overcome_by_anger>("spell_sha_of_anger_overcome_by_anger");   
}
