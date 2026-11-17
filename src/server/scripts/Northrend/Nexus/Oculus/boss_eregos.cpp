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

#include "ScriptPCH.h"
#include "oculus.h"

// Types of drake mounts: Ruby (Tank),  Amber (DPS),  Emerald (Healer)
// Two Repeating phases

enum Events
{
    EVENT_ARCANE_BARRAGE                          = 1,
    EVENT_ARCANE_VOLLEY                           = 2,
    EVENT_ENRAGED_ASSAULT                         = 3,
    EVENT_SUMMON_LEY_WHELP                        = 4
};

enum Says
{
    SAY_SPAWN                                     = 0,
    SAY_AGGRO                                     = 1,
    SAY_ENRAGE                                    = 2,
    SAY_KILL                                      = 3,
    SAY_DEATH                                     = 4,
    SAY_SHIELD                                    = 5,
};

enum Spells
{
    SPELL_ARCANE_BARRAGE                          = 50804,
    SPELL_ARCANE_BARRAGE_H                        = 59381,
    SPELL_ARCANE_VOLLEY                           = 51153,
    SPELL_ARCANE_VOLLEY_H                         = 59382,
    SPELL_ENRAGED_ASSAULT                         = 51170,
    SPELL_PLANAR_ANOMALIES                        = 57959,
    SPELL_PLANAR_SHIFT                            = 51162,
    SPELL_SUMMON_LEY_WHELP                        = 51175,
    SPELL_SUMMON_PLANAR_ANOMALIES                 = 57963,
    SPELL_PLANAR_BLAST                            = 57976
};

enum Npcs
{
    NPC_PLANAR_ANOMALY                            = 30879
};

enum Phases
{
    PHASE_NORMAL                                  = 1,
    PHASE_FIRST_PLANAR                            = 2,
    PHASE_SECOND_PLANAR                           = 3
};

enum Actions
{
    ACTION_SET_NORMAL_EVENTS                      = 1
};

enum EregosData
{
    DATA_RUBY_VOID                                = 0,      // http://www.wowhead.com/achievement=2044
    DATA_EMERALD_VOID                             = 1,      // http://www.wowhead.com/achievement=2045
    DATA_AMBER_VOID                               = 2       // http://www.wowhead.com/achievement=2046
};

class boss_eregos : public CreatureScript
{
    public:
        boss_eregos() : CreatureScript("boss_eregos") { }

        struct boss_eregosAI : public BossAI
        {
            boss_eregosAI(Creature* creature) : BossAI(creature, DATA_EREGOS)
            {
                Initialize();
            }

            void Initialize()
            {
                _phase = PHASE_NORMAL;

                _rubyVoid = true;
                _emeraldVoid = true;
                _amberVoid = true;
            }

            void Reset() override
            {
                _Reset();
                Initialize();

                DoAction(ACTION_SET_NORMAL_EVENTS);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_KILL);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                Talk(SAY_AGGRO);
                /* Checks for present drakes vehicles from each type and deactivate achievement that corresponds to each found
                   The checks are so big in case some party try weird things like pulling boss down or hiding out of check range, the only thing player need is to get the boss kill credit after the check /even if he or his drake die/
                   Drakes mechanic would despawn all after unmount and also drakes should be auto mounted after item use, item use after Eregos is engaged leads to his despawn - based on retail data. */
                if (me->FindNearestCreature(NPC_RUBY_DRAKE_VEHICLE, 500.0f, true))
                    _rubyVoid = false;
                if (me->FindNearestCreature(NPC_EMERALD_DRAKE_VEHICLE, 500.0f, true))
                    _emeraldVoid = false;
                if (me->FindNearestCreature(NPC_AMBER_DRAKE_VEHICLE, 500.0f, true))
                    _amberVoid = false;
            }

            bool CanAIAttack(Unit const* who) const override
            {
                if (Unit* vehicle = who->GetVehicleBase())
                    if (vehicle->GetEntry() == NPC_RUBY_DRAKE_VEHICLE || vehicle->GetEntry() == NPC_AMBER_DRAKE_VEHICLE || vehicle->GetEntry() == NPC_EMERALD_DRAKE_VEHICLE)
                        return true;

                return who->GetPositionZ() >= 400.0f;
            }

            uint32 GetData(uint32 type) const override
            {
               switch (type)
               {
                   case DATA_RUBY_VOID:
                        return _rubyVoid;
                   case DATA_EMERALD_VOID:
                        return _emeraldVoid;
                   case DATA_AMBER_VOID:
                        return _amberVoid;
                    default:
                        break;
                }
                return 0;
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_SET_NORMAL_EVENTS)
                    return;

                DoResetThreat();

                events.SetPhase(PHASE_NORMAL);
                events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                events.ScheduleEvent(EVENT_ARCANE_VOLLEY, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                events.ScheduleEvent(EVENT_ENRAGED_ASSAULT, urand(35 * IN_MILLISECONDS, 50 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                events.ScheduleEvent(EVENT_SUMMON_LEY_WHELP, urand(15 * IN_MILLISECONDS, 30 * IN_MILLISECONDS), 0, PHASE_NORMAL);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (me->HasAura(SPELL_PLANAR_SHIFT))
                    damage = 0;

                if (!me->GetMap()->IsHeroic())
                    return;

                if ( (me->GetHealthPct() < 60.0f  && me->GetHealthPct() > 20.0f && _phase < PHASE_FIRST_PLANAR)
                    || (me->GetHealthPct() < 20.0f && _phase < PHASE_SECOND_PLANAR) )
                {
                    events.Reset();
                    _phase = (me->GetHealthPct() < 60.0f  && me->GetHealthPct() > 20.0f) ? PHASE_FIRST_PLANAR : PHASE_SECOND_PLANAR;

                    Talk(SAY_SHIELD);
                    DoCast(SPELL_PLANAR_SHIFT);

                    // not sure about the amount, and if we should despawn previous spawns (dragon trashs)
                    summons.DespawnAll();
                    for (uint8 i = 0; i < 6; i++)
                        DoCast(SPELL_PLANAR_ANOMALIES);
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
                        case EVENT_ARCANE_BARRAGE:
                            DoCastVictim(DUNGEON_MODE(SPELL_ARCANE_BARRAGE, SPELL_ARCANE_BARRAGE_H));
                            events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(3 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                            break;
                        case EVENT_ARCANE_VOLLEY:
                            DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_VOLLEY, SPELL_ARCANE_VOLLEY_H));
                            events.ScheduleEvent(EVENT_ARCANE_VOLLEY, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                            break;
                        case EVENT_ENRAGED_ASSAULT:
                            Talk(SAY_ENRAGE);
                            DoCast(SPELL_ENRAGED_ASSAULT);
                            events.ScheduleEvent(EVENT_ENRAGED_ASSAULT, urand(35 * IN_MILLISECONDS, 50 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                            break;
                        case EVENT_SUMMON_LEY_WHELP:
                            for (uint8 i = 0; i < 3; i++)
                                DoCast(SPELL_SUMMON_LEY_WHELP);
                            events.ScheduleEvent(EVENT_SUMMON_LEY_WHELP, urand(15 * IN_MILLISECONDS, 30 * IN_MILLISECONDS), 0, PHASE_NORMAL);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                _JustDied();
                me->SummonCreature(NPC_IMAGE_OF_BELGARISTRASZ, 1022.352f, 1051.535f, 605.7099f, 0.03490658f);
            }

        private:
            uint8 _phase;
            bool _rubyVoid;
            bool _emeraldVoid;
            bool _amberVoid;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetOculusAI<boss_eregosAI>(creature);
        }
};

class npc_planar_anomaly : public CreatureScript
{
    public:
        npc_planar_anomaly() : CreatureScript("npc_planar_anomaly") { }

        struct npc_planar_anomalyAI : public ScriptedAI
        {
            npc_planar_anomalyAI(Creature* creature) : ScriptedAI(creature)
            {
                timer = 0;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MoveFollow(summoner, 0, 0);
                timer = 15000;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!timer)
                    return;

                if (timer <= diff)
                {
                    timer = 0;
                    DoCastAOE(SPELL_PLANAR_BLAST);
                }
                else
                    timer -= diff;
            }

        private:
            uint32 timer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetOculusAI<npc_planar_anomalyAI>(creature);
        }
};

class spell_eregos_planar_shift : public SpellScriptLoader
{
    public:
        spell_eregos_planar_shift() : SpellScriptLoader("spell_eregos_planar_shift") { }

        class spell_eregos_planar_shift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_eregos_planar_shift_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Creature* caster = GetTarget()->ToCreature())
                    caster->AI()->DoAction(ACTION_SET_NORMAL_EVENTS);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_eregos_planar_shift_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_eregos_planar_shift_AuraScript();
        }
};

class achievement_gen_eregos_void : public AchievementCriteriaScript
{
    public:
        achievement_gen_eregos_void(char const* name, uint32 data) : AchievementCriteriaScript(name), _data(data) { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(_data);
        }

    private:
        uint32 _data;
};

// criteria_id = 7179
class achievement_experienced_drake_rider_ruby : public AchievementCriteriaScript
{
    public:
        achievement_experienced_drake_rider_ruby() : AchievementCriteriaScript("achievement_experienced_drake_rider_ruby") { }

        bool OnCheck(Player* source, Unit* /*target*/) override
        {
            if (!source)
                return false;

            if (Creature* mount = source->GetVehicleCreatureBase())
                if (mount->GetEntry() == NPC_RUBY_DRAKE_VEHICLE)
                    return true;

            return false;
        }
};

// criteria_id = 7177
class achievement_experienced_drake_rider_amber : public AchievementCriteriaScript
{
    public:
        achievement_experienced_drake_rider_amber() : AchievementCriteriaScript("achievement_experienced_drake_rider_amber") { }

        bool OnCheck(Player* source, Unit* /*target*/) override
        {
            if (!source)
                return false;

            if (Creature* mount = source->GetVehicleCreatureBase())
                if (mount->GetEntry() == NPC_AMBER_DRAKE_VEHICLE)
                    return true;

            return false;
        }
};

// criteria_id = 7178
class achievement_experienced_drake_rider_emerald : public AchievementCriteriaScript
{
    public:
        achievement_experienced_drake_rider_emerald() : AchievementCriteriaScript("achievement_experienced_drake_rider_emerald") { }

        bool OnCheck(Player* source, Unit* /*target*/) override
        {
            if (!source)
                return false;

            if (Creature* mount = source->GetVehicleCreatureBase())
                if (mount->GetEntry() == NPC_EMERALD_DRAKE_VEHICLE)
                    return true;

            return false;
        }
};

void AddSC_boss_eregos()
{
    new boss_eregos();
    new npc_planar_anomaly();
    new spell_eregos_planar_shift();
    new achievement_gen_eregos_void("achievement_ruby_void", DATA_RUBY_VOID);
    new achievement_gen_eregos_void("achievement_emerald_void", DATA_EMERALD_VOID);
    new achievement_gen_eregos_void("achievement_amber_void", DATA_AMBER_VOID);
    new achievement_experienced_drake_rider_ruby();
    new achievement_experienced_drake_rider_amber();
    new achievement_experienced_drake_rider_emerald();
}
