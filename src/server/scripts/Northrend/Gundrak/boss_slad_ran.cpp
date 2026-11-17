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
#include "SpellAuras.h"
#include "gundrak.h"

//Spells
enum Spells
{
    // Slad'ran
    SPELL_POISON_NOVA                             = 55081,
    SPELL_POWERFULL_BITE                          = 48287,
    SPELL_VENOM_BOLT                              = 54970,
    SPELL_SUMMON_VIPER                            = 55060,
    SPELL_SUMMON_CONSTRICTOR                      = 54969,

    // Slad'ran Constrictor
    SPELL_GRIP_OF_SLAD_RAN                        = 55093,
    SPELL_SNAKE_WRAP                              = 55099,
    SPELL_SNAKE_WRAP_SUMMON                       = 55126,
    SPELL_SNAKE_WRAP_KILL_SNAKES                  = 55127,
    SPELL_SNAKE_WRAP_SCRIPT                       = 55128,

    // Slad'ran Viper
    SPELL_VENOMOUS_BITE                           = 54987,
};

enum Yells
{
    SAY_SUMMON_SNAKES                             = 0,
    SAY_SUMMON_CONSTRICTORS                       = 1,
    EMOTE_POISON_NOVA                             = 2,
    EMOTE_DEATH                                   = 3,
    SAY_AGGRO                                     = 4,
    SAY_SLAY                                      = 5,
    SAY_DEATH                                     = 6,
};

enum Creatures
{
    NPC_SLADRAN_SUMMON_TARGET                     = 29682,
    NPC_SNAKE                                     = 29680,
    NPC_CONSTRICTORS                              = 29713
};

enum Events
{
    EVENT_VENOM_BOLT                              = 1,
    EVENT_POISON_NOVA,
    EVENT_POWERFUL_BITE,

    EVENT_GRIP_OF_SLADRAN,

    EVENT_VENOMOUS_BITE,

    EVENT_GROUP_SUMMON_SNAKES                     = 1,
};

static Position SpawnLoc[] =
{
    { 1783.81f, 646.637f, 133.948f, 3.71755f  },
    { 1775.03f, 606.586f, 134.165f, 1.43117f  },
    { 1717.39f, 630.041f, 129.282f, 5.96903f  },
    { 1765.66f, 646.542f, 134.02f,  5.11381f  },
    { 1716.76f, 635.159f, 129.282f, 0.191986f }
};

#define DATA_SNAKES_WHYD_IT_HAVE_TO_BE_SNAKES 1

class SnakeSummonEvent : public GroupedEvent
{
    public:
        SnakeSummonEvent(Creature* summoner, uint32 spell, uint32 repeatMin, uint32 repeatMax) : GroupedEvent(EVENT_GROUP_SUMMON_SNAKES), m_summoner(summoner), m_spell(spell), m_repeatMin(repeatMin), m_repeatMax(repeatMax) { }

        bool Execute(uint64 time, uint32 diff) override
        {
            m_summoner->CastSpell(m_summoner, m_spell);
            m_summoner->m_Events.Schedule(urand(m_repeatMin, m_repeatMax), this);
            return false;
        }

    private:
        Creature* m_summoner;
        uint32 m_spell;
        uint32 m_repeatMin;
        uint32 m_repeatMax;
};

class boss_slad_ran : public CreatureScript
{
    public:
        boss_slad_ran() : CreatureScript("boss_slad_ran") { }

        struct boss_slad_ranAI : public ScriptedAI
        {
            boss_slad_ranAI(Creature* creature) : ScriptedAI(creature), summons(me), instance(me->GetInstanceScript()) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_VENOM_BOLT, 8000);
                events.ScheduleEvent(EVENT_POISON_NOVA, 17000);
                events.ScheduleEvent(EVENT_POWERFUL_BITE, 24000);
                summons.DespawnAll();
                phase = 0;
                wrappedPlayers.clear();

                StopSummoning();

                if (instance)
                    instance->SetData(DATA_SLAD_RAN_EVENT, NOT_STARTED);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                if (instance)
                    instance->SetData(DATA_SLAD_RAN_EVENT, IN_PROGRESS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (viperSummonerGuids.size() != 2 || constrictorSummonerGuids.size() != 3)
                {
                    viperSummonerGuids.clear();
                    constrictorSummonerGuids.clear();
                    std::list<Creature*> targets;
                    GetCreatureListWithEntryInGrid(targets, me, NPC_SLADRAN_SUMMON_TARGET, 100.0f);
                    if (targets.size() == 5)
                        for (auto&& target : targets)
                            (target->GetPositionX() < 1730 ? viperSummonerGuids : constrictorSummonerGuids).push_back(target->GetGUID());
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_POISON_NOVA:
                            DoCastVictim(SPELL_POISON_NOVA);
                            Talk(EMOTE_POISON_NOVA);
                            events.ScheduleEvent(EVENT_POISON_NOVA, urand(20000, 42000));
                            break;
                        case EVENT_POWERFUL_BITE:
                            DoCastVictim(SPELL_POWERFULL_BITE);
                            events.ScheduleEvent(EVENT_POWERFUL_BITE, urand(12000, 42000));
                            break;
                        case EVENT_VENOM_BOLT:
                            DoCastVictim(SPELL_VENOM_BOLT);
                            events.ScheduleEvent(EVENT_VENOM_BOLT, urand(9000, 18000));
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                if (phase == 0 && HealthBelowPct(90))
                {
                    phase = 1;
                    Talk(SAY_SUMMON_SNAKES);
                    for (auto&& guid : viperSummonerGuids)
                        if (Creature* target = ObjectAccessor::GetCreature(*me, guid))
                            if (SummonTimer const* timer = GetSummonTimer(target))
                                target->m_Events.Schedule(urand(timer->InitialMin, timer->InitialMax), new SnakeSummonEvent(target, SPELL_SUMMON_VIPER, timer->RepeatMin, timer->RepeatMax));
                }

                if (phase == 1 && HealthBelowPct(75))
                {
                    Talk(SAY_SUMMON_CONSTRICTORS);
                    phase = 2;
                    for (auto&& guid : constrictorSummonerGuids)
                        if (Creature* target = ObjectAccessor::GetCreature(*me, guid))
                            if (SummonTimer const* timer = GetSummonTimer(target))
                                target->m_Events.Schedule(urand(timer->InitialMin, timer->InitialMax), new SnakeSummonEvent(target, SPELL_SUMMON_CONSTRICTOR, timer->RepeatMin, timer->RepeatMax));
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);
                Talk(EMOTE_DEATH);

                StopSummoning();
                summons.DespawnAll();

                if (instance)
                    instance->SetData(DATA_SLAD_RAN_EVENT, DONE);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustSummoned(Creature* summon) override
            {
                DoZoneInCombat(summon, 100.0f);
                summon->CallAssistance();
                summons.Summon(summon);
            }

            void SetGUID(ObjectGuid guid, int32 type) override
            {
                if (type == DATA_SNAKES_WHYD_IT_HAVE_TO_BE_SNAKES)
                    wrappedPlayers.insert(guid);
            }

            bool WasWrapped(uint64 guid)
            {
                return wrappedPlayers.count(guid);
            }

        private:
            InstanceScript* instance;
            EventMap events;
            SummonList summons;
            uint8 phase = 0;
            std::set<uint64> wrappedPlayers;
            std::vector<ObjectGuid> viperSummonerGuids;
            std::vector<ObjectGuid> constrictorSummonerGuids;

            void StopSummoning()
            {
                for (auto&& guid : viperSummonerGuids)
                    if (Creature* target = ObjectAccessor::GetCreature(*me, guid))
                        target->m_Events.KillEventsByGroup(EVENT_GROUP_SUMMON_SNAKES);

                for (auto&& guid : constrictorSummonerGuids)
                    if (Creature* target = ObjectAccessor::GetCreature(*me, guid))
                        target->m_Events.KillEventsByGroup(EVENT_GROUP_SUMMON_SNAKES);
            }

            struct SummonTimer
            {
                uint32 InitialMin, InitialMax, RepeatMin, RepeatMax;
            };

            SummonTimer const* GetSummonTimer(Creature* summoner) const
            {
                static std::map<uint32, SummonTimer> const timerTable =
                {
                    { 122973, SummonTimer {     1,     1, 16000, 20000 } }, // Viper
                    { 122971, SummonTimer { 25000, 31000, 25000, 31000 } }, // Viper
                    { 122969, SummonTimer {  5000,  6000,  5000,  6000 } }, // Constrictor
                    { 122970, SummonTimer {     1,     1,  5000,  6000 } }, // Constrictor
                    { 122972, SummonTimer {     1,     1,  6000,  7000 } }, // Constrictor
                };
                auto itr = timerTable.find(summoner->GetDBTableGUIDLow());
                return itr == timerTable.end() ? NULL : &itr->second;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_slad_ranAI(creature);
        }
};

class npc_slad_ran_constrictor : public CreatureScript
{
    public:
        npc_slad_ran_constrictor() : CreatureScript("npc_slad_ran_constrictor") { }

        struct npc_slad_ran_constrictorAI : public ScriptedAI
        {
            npc_slad_ran_constrictorAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_GRIP_OF_SLADRAN, 1000);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (Creature* sladran = me->FindNearestCreature(NPC_SLAD_RAN, 200.0f))
                    sladran->AI()->JustSummoned(me);
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
                        case EVENT_GRIP_OF_SLADRAN:
                            if (Unit* target = me->GetVictim())
                            {
                                if (!target->HasAura(SPELL_SNAKE_WRAP_SUMMON))
                                {
                                    DoCast(target, SPELL_GRIP_OF_SLAD_RAN);

                                    if (target->GetAuraCount(SPELL_GRIP_OF_SLAD_RAN) == 5)
                                    {
                                        target->RemoveAurasDueToSpell(SPELL_GRIP_OF_SLAD_RAN);
                                        target->CastSpell(target, SPELL_SNAKE_WRAP, true);

                                        if (Creature* sladran = me->FindNearestCreature(NPC_SLAD_RAN, 200.0f))
                                            sladran->AI()->SetGUID(target->GetGUID(), DATA_SNAKES_WHYD_IT_HAVE_TO_BE_SNAKES);
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_GRIP_OF_SLADRAN, 5000);
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_slad_ran_constrictorAI(creature);
        }
};

class npc_slad_ran_viper : public CreatureScript
{
    public:
        npc_slad_ran_viper() : CreatureScript("npc_slad_ran_viper") { }

        struct npc_slad_ran_viperAI : public ScriptedAI
        {
            npc_slad_ran_viperAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_VENOMOUS_BITE, 2000);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                if (Creature* sladran = me->FindNearestCreature(NPC_SLAD_RAN, 200.0f))
                    sladran->AI()->JustSummoned(me);
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
                        case EVENT_VENOMOUS_BITE:
                            DoCastVictim(SPELL_VENOMOUS_BITE);
                            events.ScheduleEvent(EVENT_VENOMOUS_BITE, urand(6000, 18000));
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_slad_ran_viperAI(creature);
        }
};

class npc_snake_wrap : public CreatureScript
{
    public:
        npc_snake_wrap() : CreatureScript("npc_snake_wrap") { }

        struct npc_snake_wrapAI : public ScriptedAI
        {
            npc_snake_wrapAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*summoner*/) override
            {
                me->m_Events.Schedule(10000, [this]()
                {
                    DoCast(me, SPELL_SNAKE_WRAP_KILL_SNAKES);
                    DoCast(me, SPELL_SNAKE_WRAP_SCRIPT);
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (TempSummon* summon = me->ToTempSummon())
                    if (Unit* summoner = summon->GetSummoner())
                        summoner->RemoveAurasDueToSpell(SPELL_SNAKE_WRAP_SUMMON);
            }

            void UpdateAI(uint32 diff) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_snake_wrapAI(creature);
        }
};

class spell_snake_wrap : public SpellScriptLoader
{
    public:
        spell_snake_wrap() : SpellScriptLoader("spell_snake_wrap") { }

        class spell_snake_wrap_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_snake_wrap_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastSpell(GetTarget(), SPELL_SNAKE_WRAP_SUMMON, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_snake_wrap_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_ROOT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_snake_wrap_AuraScript();
        }
};

class achievement_snakes_whyd_it_have_to_be_snakes : public AchievementCriteriaScript
{
    public:
        achievement_snakes_whyd_it_have_to_be_snakes() : AchievementCriteriaScript("achievement_snakes_whyd_it_have_to_be_snakes") { }

        bool OnCheck(Player* source, Unit* target) override
        {
            if (!target)
                return false;

            if (boss_slad_ran::boss_slad_ranAI* sladRanAI = CAST_AI(boss_slad_ran::boss_slad_ranAI, target->GetAI()))
                return !sladRanAI->WasWrapped(source->GetGUID());

            return false;
        }
};

void AddSC_boss_slad_ran()
{
    new boss_slad_ran();
    new npc_slad_ran_constrictor();
    new npc_slad_ran_viper();
    new npc_snake_wrap();
    new spell_snake_wrap();
    new achievement_snakes_whyd_it_have_to_be_snakes();
}
