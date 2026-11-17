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

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_monastery.h"

enum spells
{
    SPELL_BLAZING_FISTS           = 114807,
    SPELL_BLAZING_FISTS_TRIGGERED = 114808, // triggere by 114807 , deal 40% of weapon damage
    SPELL_FIRESTORM_KICK          = 113764,
    SPELL_RISING_FLAME            = 114410,
    SPELL_SCORCHED_EARTH          = 114460,
    SPELL_FLYING_KICK             = 114488,
    SPELL_FLYING_KICK_JUMP        = 114487,
    SPELL_TRIGGER_SCORCHED_EARTH  = 114464,

    SPELL_BURNING_MAN_1           = 125844, // from blazing fists
    SPELL_BURNING_MAN_2           = 125852, // From FireStorm (for achiev)
};

enum events
{
    EVENT_BLAZING_FISTS           = 1,
    EVENT_FLYING_KICK             = 2,
    EVENT_FIRESTORM               = 3,
    EVENT_BLAZING_FIRST_CANCEL    = 4,
};

enum Yells
{
    TALK_AGGRO                    = 0,
    TALK_DEATH                    = 1,
    TALK_SLAY                     = 2,
    TALK_FISTS                    = 3
};

enum phases
{
    PHASE_ONE                     = 1,
    PHASE_TWO                     = 2
};

enum Action
{
    ACTION_FLYING_KICK            = 0,
};

enum Misc
{
    DATA_BURNING_DUMMY_COUNT,
};

class boss_brother_korloff : public CreatureScript
{
    public:
        boss_brother_korloff() : CreatureScript("boss_brother_korloff") { }

        struct boss_brother_korloffAI : public BossAI
        {
            boss_brother_korloffAI(Creature* creature) : BossAI(creature, BOSS_BROTHER_KORLOFF) { }

            uint32 phase;
            ObjectGuid targetGUID;
            uint32 burningDummyCounter;
            float heal;

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
                if (instance)
                {
                    instance->SetData(BOSS_BROTHER_KORLOFF, NOT_STARTED);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_MAN_1);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_MAN_2);
                }
                phase = PHASE_ONE;
                heal = 100.0f;
                targetGUID = ObjectGuid::Empty;
                burningDummyCounter = 0;
                me->GetMap()->SetWorldState(WORLDSTATE_BURNING_MAN, 0);
            }

            void JustDied(Unit* killer) override
            {
                _JustDied();
                Talk(TALK_DEATH);
                if (instance)
                {
                    instance->SetData(BOSS_BROTHER_KORLOFF, DONE);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_MAN_1);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_MAN_2);
                }
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(BOSS_BROTHER_KORLOFF, FAIL);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_MAN_1);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_MAN_2);
                }
                summons.DespawnAll();

                // Clear Traning Dummys
                std::list<Creature*> trainingDummys;
                GetCreatureListWithEntryInGrid(trainingDummys, me, NPC_TRAINING_DUMMY_2, 300.0f);

                for (auto&& itr : trainingDummys)
                    itr->RemoveAurasDueToSpell(SPELL_BURNING_MAN_2);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                {
                    Talk(TALK_SLAY);
                    me->AddAura(SPELL_FIRESTORM_KICK, me);
                }
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (++burningDummyCounter > 5)
                    me->GetMap()->SetWorldState(WORLDSTATE_BURNING_MAN, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_AGGRO);
                if (instance)
                {
                    instance->SetData(BOSS_BROTHER_KORLOFF, IN_PROGRESS);
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }
                heal = me->GetHealthPct();
                events.ScheduleEvent(EVENT_FLYING_KICK, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BLAZING_FISTS, 20 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* killer, uint32 &damage) override
            {
                if (HealthBelowPct(50) && phase == PHASE_ONE)
                {
                    phase = PHASE_TWO;
                    me->CastSpell(me, SPELL_SCORCHED_EARTH, false);
                }
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_FIRESTORM)
                {
                    me->RemoveChanneledCast(targetGUID);
                    events.ScheduleEvent(EVENT_FLYING_KICK, 19 * IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
               
                if (heal - me->GetHealthPct() >= 10)
                {
                    heal = me->GetHealthPct();
                    me->CastSpell(me, SPELL_RISING_FLAME,false);
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLAZING_FISTS:
                            if (Unit* target = me->GetVictim())
                                me->PrepareChanneledCast(me->GetAngle(target), SPELL_BLAZING_FISTS);

                            Talk(TALK_SLAY);
                            events.ScheduleEvent(EVENT_BLAZING_FISTS, 20 * IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_BLAZING_FIRST_CANCEL,6500);
                            break;
                        case EVENT_BLAZING_FIRST_CANCEL:
                            me->RemoveChanneledCast(targetGUID);
                            break;
                        case EVENT_FLYING_KICK:

                            if (Unit* vict = me->GetVictim())
                                targetGUID = vict->GetGUID();

                            if (Unit* FireStormTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, FireStormPredicate(me)))
                            {
                                me->CastSpell(FireStormTarget, SPELL_FLYING_KICK, true);
                                me->PrepareChanneledCast(me->GetAngle(FireStormTarget));
                                me->GetMotionMaster()->MoveCharge(FireStormTarget->GetPositionX(), FireStormTarget->GetPositionY(), FireStormTarget->GetPositionZ(), 35.0f, EVENT_CHARGE);
                            }
                            else // if alive only tank or it`s solo
                            {
                                if (Unit* victim = me->GetVictim())
                                {
                                    Position pos = { me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation() };
                                    pos = me->GetFirstCollisionPosition(frand(6.0f, 8.0f), 0.0f);

                                    me->CastSpell(victim, SPELL_FLYING_KICK, true);
                                    me->PrepareChanneledCast(me->GetAngle(victim));
                                    me->GetMotionMaster()->MoveCharge(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), 35.0f, EVENT_CHARGE);
                                }
                            }
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct FireStormPredicate
                {
                    public:
                        FireStormPredicate(Creature const* me) : _me(me) { }
                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetDistance2d(_me) > 5.0f; // if less then it possible bugged
                        }

                    private:
                        Creature const* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_brother_korloffAI(creature);
        }
};

// Scorched Earth 59507
struct npc_trigger_scorched_flame : public ScriptedAI
{
    npc_trigger_scorched_flame(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool trig;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void JustDied(Unit* /*killer*/) override { }

    void IsSummonedBy(Unit* summoner) override
    {
        me->CastSpell(me, SPELL_TRIGGER_SCORCHED_EARTH, false);
        me->DespawnOrUnsummon(30000);
        trig = false;
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);
    }
};

// Scorched Earth 114460
class spell_scorched_earth : public AuraScript
{
    PrepareAuraScript(spell_scorched_earth);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        // Check target moving.
        if (GetOwner() && GetOwner()->ToUnit() && GetOwner()->ToUnit()->isMoving())
            if (Aura* ScorchedEarth = GetOwner()->ToUnit()->GetAura(SPELL_SCORCHED_EARTH))
                GetOwner()->ToUnit()->SummonCreature(NPC_SCORCHED_EARTH_TRIGGER, GetOwner()->ToUnit()->GetPositionX(), GetOwner()->ToUnit()->GetPositionY(), GetOwner()->ToUnit()->GetPositionZ(), GetOwner()->ToUnit()->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_scorched_earth::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// Firestorm Kick 113764
class spell_firestorm_kick : public AuraScript
{
    PrepareAuraScript(spell_firestorm_kick);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->AI()->DoAction(ACTION_FIRESTORM);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_firestorm_kick::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Burning Man 125852
class spell_korloff_burning_man : public AuraScript
{
    PrepareAuraScript(spell_korloff_burning_man);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetCaster() && GetCaster()->ToCreature() && GetOwner() && GetOwner()->GetEntry() == NPC_TRAINING_DUMMY_2)
            GetCaster()->ToCreature()->AI()->SetData(DATA_BURNING_DUMMY_COUNT, 1);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_korloff_burning_man::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_boss_brother_korloff()
{
    new boss_brother_korloff();
    new creature_script<npc_trigger_scorched_flame>("npc_trigger_scorched_flame");

    new aura_script<spell_scorched_earth>("spell_scorched_earth");
    new aura_script<spell_firestorm_kick>("spell_firestorm_kick");
    new aura_script<spell_korloff_burning_man>("spell_korloff_burning_man");
}