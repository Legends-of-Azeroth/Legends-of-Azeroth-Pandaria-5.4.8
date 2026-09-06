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
#include "gundrak.h"
#include "../AI/SmartScripts/SmartAI.h"

enum Spells
{
    SPELL_ENRAGE                                  = 55285,
    SPELL_IMPALING_CHARGE                         = 54956,
    SPELL_STOMP                                   = 55292,
    SPELL_PUNCTURE                                = 55276,
    SPELL_PUNCTURE_H                              = 59826,
    SPELL_STAMPEDE                                = 55218,
    SPELL_WHIRLING_SLASH                          = 55250,
    SPELL_IMPALING_CHARGE_VEHICLE                 = 54958,
    SPELL_TRANSFORM                               = 55297,
    SPELL_TRANSFORM_CLEAR                         = 55299,
    SPELL_HEART_BEAM_VISUAL                       = 54988,
    SPELL_CLEAR_PUNCTURE                          = 60022,
    // rhino spirit spells
    SPELL_STAMPEDE_DMG                            = 55220,
    SPELL_STAMPEDE_DMG_H                          = 59823
};

enum Yells
{
    SAY_AGGRO                                     = 0,
    SAY_TRANSFORM                                 = 1,
    SAY_SUMMON_RHINO                              = 2,
    SAY_SLAY                                      = 3,
    SAY_DEATH                                     = 4,
    EMOTE_IMPALE                                  = 5,
};

enum Events
{
    EVENT_STAMPEDE = 1,
    EVENT_PUNCTURE,
    EVENT_WHIRLING_SLASH,
    EVENT_ENRAGE,
    EVENT_STOMP,
    EVENT_IMPALING_CHARGE,
    EVENT_TRANSFORM,
};

enum Phases
{
    PHASE_TROLL = 1,
    PHASE_RHINO,
};

class boss_gal_darah : public CreatureScript
{
    public:
        boss_gal_darah() : CreatureScript("boss_gal_darah") { }

        struct boss_gal_darahAI : public ScriptedAI
        {
            boss_gal_darahAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                events.Reset();
                events.SetPhase(PHASE_TROLL);
                events.ScheduleEvent(EVENT_STAMPEDE,         5 * IN_MILLISECONDS, 0, PHASE_TROLL);
                events.ScheduleEvent(EVENT_PUNCTURE,        10 * IN_MILLISECONDS, 0, PHASE_TROLL);
                events.ScheduleEvent(EVENT_WHIRLING_SLASH,  15 * IN_MILLISECONDS, 0, PHASE_TROLL);
                events.ScheduleEvent(EVENT_TRANSFORM,       31 * IN_MILLISECONDS, 0, PHASE_TROLL);

                impaled.clear();
                oocTimer = 1;

                if (instance)
                    instance->SetData(DATA_GAL_DARAH_EVENT, NOT_STARTED);

                me->GetMap()->SetWorldState(WORLDSTATE_SHARE_THE_LOVE, 0);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);

                me->InterruptNonMeleeSpells(false);

                if (instance)
                    instance->SetData(DATA_GAL_DARAH_EVENT, IN_PROGRESS);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->GetThreatManager().AddThreat(me, 1.0f); // To prevent them from entering evade mode via UpdateVictim()
                summon->SetReactState(REACT_PASSIVE);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                summon->DespawnOrUnsummon(1500);

                if (Unit* target = ObjectAccessor::GetUnit(*me, stampedeTarget))
                    summon->CastSpell(target, DUNGEON_MODE(SPELL_STAMPEDE_DMG, SPELL_STAMPEDE_DMG_H), true);
            }

            void JustReachedHome() override
            {
                ScriptedAI::JustReachedHome();
                oocTimer = 1;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_TRANSFORM_CLEAR)
                    me->RemoveAurasDueToSpell(SPELL_TRANSFORM);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_CLEAR_PUNCTURE)
                    target->RemoveAurasDueToSpell(DUNGEON_MODE(SPELL_PUNCTURE, SPELL_PUNCTURE_H));
                else if (spell->Id == SPELL_IMPALING_CHARGE)
                {
                    Talk(EMOTE_IMPALE, target);
                    target->CastSpell(me, SPELL_IMPALING_CHARGE_VEHICLE, true);
                    impaled.insert(target->GetGUID());

                    if (impaled.size() >= 5)
                        me->GetMap()->SetWorldState(WORLDSTATE_SHARE_THE_LOVE, 1);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                {
                    if (!me->IsInCombat())
                    {
                        if (oocTimer <= diff)
                        {
                            DoCast(me, SPELL_HEART_BEAM_VISUAL);
                            oocTimer = 11000;
                        }
                        else
                            oocTimer -= diff;
                    }
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STAMPEDE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                            {
                                stampedeTarget = target->GetGUID();
                                DoCast(target, SPELL_STAMPEDE);
                            }
                            Talk(SAY_SUMMON_RHINO);
                            events.ScheduleEvent(EVENT_STAMPEDE, urand(16 * IN_MILLISECONDS, 17 * IN_MILLISECONDS), 0, PHASE_TROLL);
                            break;
                        case EVENT_PUNCTURE:
                            DoCastVictim(DUNGEON_MODE(SPELL_PUNCTURE, SPELL_PUNCTURE_H));
                            events.ScheduleEvent(EVENT_PUNCTURE, urand(13 * IN_MILLISECONDS, 16 * IN_MILLISECONDS), 0, PHASE_TROLL);
                            break;
                        case EVENT_WHIRLING_SLASH:
                            DoCast(me, SPELL_WHIRLING_SLASH);
                            break;
                        case EVENT_ENRAGE:
                            DoCast(me, SPELL_ENRAGE);
                            events.ScheduleEvent(EVENT_ENRAGE, 15 * IN_MILLISECONDS, 0, PHASE_RHINO);
                            break;
                        case EVENT_STOMP:
                            DoCast(me, SPELL_STOMP);
                            events.ScheduleEvent(EVENT_STOMP, 11 * IN_MILLISECONDS, 0, PHASE_RHINO);
                            break;
                        case EVENT_IMPALING_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 60, true))
                                DoCast(target, SPELL_IMPALING_CHARGE);
                            break;
                        case EVENT_TRANSFORM:
                            if (events.IsInPhase(PHASE_TROLL))
                            {
                                events.Reset();
                                events.SetPhase(PHASE_RHINO);
                                events.ScheduleEvent(EVENT_ENRAGE,           4 * IN_MILLISECONDS, 0, PHASE_RHINO);
                                events.ScheduleEvent(EVENT_STOMP,            5 * IN_MILLISECONDS, 0, PHASE_RHINO);
                                events.ScheduleEvent(EVENT_IMPALING_CHARGE, 14 * IN_MILLISECONDS, 0, PHASE_RHINO);
                                events.ScheduleEvent(EVENT_TRANSFORM,       32 * IN_MILLISECONDS, 0, PHASE_RHINO);
                                DoCast(SPELL_TRANSFORM);
                                Talk(SAY_TRANSFORM);
                            }
                            else
                            {
                                events.Reset();
                                events.SetPhase(PHASE_TROLL);
                                events.ScheduleEvent(EVENT_STAMPEDE,               7 * IN_MILLISECONDS,                        0, PHASE_TROLL);
                                events.ScheduleEvent(EVENT_PUNCTURE,        urand(12 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), 0, PHASE_TROLL);
                                events.ScheduleEvent(EVENT_WHIRLING_SLASH,  urand(13 * IN_MILLISECONDS, 16 * IN_MILLISECONDS), 0, PHASE_TROLL);
                                events.ScheduleEvent(EVENT_TRANSFORM,             32 * IN_MILLISECONDS,                        0, PHASE_TROLL);
                                DoCast(SPELL_TRANSFORM_CLEAR);
                            }
                            break;
                        default:
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        break;
                }

                DoMeleeAttackIfReady();

                if (me->GetDistance(me->GetHomePosition()) > 80.0f)
                {
                    me->SetHealth(uint32(me->GetMaxHealth()));
                    EnterEvadeMode();
                    return;
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                DoCastAOE(SPELL_CLEAR_PUNCTURE, true);

                if (instance)
                    instance->SetData(DATA_GAL_DARAH_EVENT, DONE);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

        private:
            InstanceScript* instance;
            EventMap events;
            std::set<uint64> impaled;
            ObjectGuid stampedeTarget;
            uint32 oocTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_gal_darahAI(creature);
        }
};

class npc_drakkari_rhino : public CreatureScript
{
    public:
        npc_drakkari_rhino() : CreatureScript("npc_drakkari_rhino") { }

        struct npc_drakkari_rhinoAI : public SmartAI
        {
            npc_drakkari_rhinoAI(Creature* creature) : SmartAI(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                me->RemoveAurasDueToSpell(VEHICLE_SPELL_RIDE_HARDCODED);

                SmartAI::JustEngagedWith(who);
            }

            void GetPassengerExitPosition(Unit* passenger, int8 seatId, Position& pos) override
            {
                if (me->GetPositionX() < 1788.0f)
                {
                    switch (seatId)
                    {
                        case 0: pos.Relocate(1773.924f, 748.7017f, 119.3996f, 3.132604f); break;
                        case 1: pos.Relocate(1771.291f, 738.6677f, 119.3996f, 3.132604f); break;
                        case 2: pos.Relocate(1769.334f, 743.6854f, 119.3996f, 3.132604f); break;
                        default: return;
                    }
                }

                if (Creature* creature = passenger->ToCreature())
                    creature->SetHomePosition(pos);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_drakkari_rhinoAI(creature);
        }
};

void AddSC_boss_gal_darah()
{
    new boss_gal_darah();
    new npc_drakkari_rhino();
}
