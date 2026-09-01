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
#include "arcatraz.h"

enum Spells
{
    SPELL_FEL_IMMOLATION = 36051,
    SPELL_FELFIRE_SHOCK  = 35759,
    SPELL_FELFIRE        = 35769,
};

enum Events
{
    EVENT_FELFIRE_CHARGE     = 1,
    EVENT_FELFIRE_SHOCK      = 2,
    EVENT_FELFIRE_CHARGE_EFF = 3,

    // Cosmetic
    EVENT_CONV_1             = 1,
    EVENT_CONV_2             = 2,
    EVENT_CONV_3             = 3,
    EVENT_CONV_4             = 4,
    EVENT_CONV_5             = 5,
    EVENT_CONV_6             = 6,
};

class boss_wrath_scryer_soccothrates : public CreatureScript
{
    public:
        boss_wrath_scryer_soccothrates() : CreatureScript("boss_wrath_scryer_soccothrates") { }

        struct boss_wrath_scryer_soccothratesAI : public BossAI
        {
            boss_wrath_scryer_soccothratesAI(Creature* creature) : BossAI(creature, TYPE_SOCCOTHRATES) { }

            EventMap nonCombatEvents;
            uint32 felfireCounter, felfireCalculate;
            ObjectGuid targetGUID;
            float x, y;
            bool intro, hasYell;

            void InitializeAI() override
            {
                intro   = false;
                hasYell = false;
                Reset();
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                felfireCounter   = 1;
                felfireCalculate = 0;
                targetGUID = ObjectGuid::Empty;
                x                = 0.0f;
                y                = 0.0f;
                hasYell          = false;

                if (instance)
                    instance->SetData(TYPE_SOCCOTHRATES, NOT_STARTED);
            }

            Creature* Dalliah()
            {
                if (Creature* sDalliah = ObjectAccessor::GetCreature(*me, instance->GetGuidData(TYPE_DALLIAH)))
                    return sDalliah;

                return 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();
                Talk(TALK_SD_AGGRO);

                if (Creature* dalliah = Dalliah())
                    if (dalliah->IsAlive())
                        dalliah->AI()->Talk(TALK_SOCCOTHRATES_IN_COMBAT_AND_ME_ALIVE);

                DoCast(me, SPELL_FEL_IMMOLATION);
                events.ScheduleEvent(EVENT_FELFIRE_SHOCK, 9 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FELFIRE_CHARGE, 30 * IN_MILLISECONDS);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() == TYPEID_PLAYER)
                    Talk(TALK_SD_SLAY);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                _EnterEvadeMode();
                me->GetMotionMaster()->MoveTargetedHome();

                if (instance)
                    instance->SetData(TYPE_SOCCOTHRATES, FAIL);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case TYPE_FELFIRE:
                        return felfireCounter;
                    case TYPE_FELFIRE_COUNT:
                        return felfireCalculate;
                }

                return 0;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == TYPE_FELFIRE)
                    felfireCounter = data;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (intro && who && who->GetTypeId() == TYPEID_PLAYER)
                {
                    intro = true;
                    Talk(TALK_CONVERSATION_TO_DALLIAH_1);
                    nonCombatEvents.ScheduleEvent(EVENT_CONV_1, 1.5 * IN_MILLISECONDS);
                }

                BossAI::MoveInLineOfSight(who);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_CHARGE)
                    me->RemoveChanneledCast(targetGUID);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (HealthBelowPct(25) && !hasYell)
                {
                    hasYell = true;

                    if (Creature* dalliah = Dalliah())
                        if (dalliah->IsAlive())
                            dalliah->AI()->Talk(TALK_SOCCOTHRATES_AT_LOW);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(TALK_SOCCOTHRATES_DEATH);

                if (Creature* dalliah = Dalliah())
                    if (dalliah->IsAlive())
                        dalliah->AI()->Talk(TALK_SOCCOTHRATES_DIE);

                if (instance)
                    instance->SetData(TYPE_SOCCOTHRATES, DONE);
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CONV_1:
                            if (Creature* dalliah = Dalliah())
                                dalliah->AI()->Talk(TALK_CONVERSATION_TO_SOCCOTHRATES_1);

                            nonCombatEvents.ScheduleEvent(EVENT_CONV_2, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_CONV_2:
                            Talk(TALK_CONVERSATION_TO_DALLIAH_2);
                            nonCombatEvents.ScheduleEvent(EVENT_CONV_3, 2 * IN_MILLISECONDS);
                            break;
                        case EVENT_CONV_3:
                            if (Creature* dalliah = Dalliah())
                                dalliah->AI()->Talk(TALK_CONVERSATION_TO_SOCCOTHRATES_2);

                            nonCombatEvents.ScheduleEvent(EVENT_CONV_4, 1 * IN_MILLISECONDS);
                            break;
                        case EVENT_CONV_4:
                            Talk(TALK_CONVERSATION_TO_DALLIAH_3);
                            nonCombatEvents.ScheduleEvent(EVENT_CONV_5, 3.5 * IN_MILLISECONDS);
                            break;
                        case EVENT_CONV_5:
                            if (Creature* dalliah = Dalliah())
                                dalliah->AI()->Talk(TALK_CONVERSATION_TO_SOCCOTHRATES_3);

                            nonCombatEvents.ScheduleEvent(EVENT_CONV_6, 2 * IN_MILLISECONDS);
                            break;
                        case EVENT_CONV_6:
                            Talk(TALK_CONVERSATION_TO_DALLIAH_4);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FELFIRE_CHARGE:
                            Talk(TALK_FELFIRE_CHARGE);
                            DoCast(me, SPELL_FELFIRE);
                            felfireCounter = 1;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, FelrushPredicate(me)))
                            {
                                if (Unit* vict = me->GetVictim())
                                    targetGUID = vict->GetGUID();

                                me->PrepareChanneledCast(me->GetAngle(target));

                                if (int32 dive = int32(me->GetExactDist2d(target)) / 5)
                                    felfireCalculate = dive;

                                GetPositionWithDistInOrientation(me, 5.0f, me->GetOrientation(), x, y);
                                me->SummonCreature(NPC_SCRYER_FELFIRE, x, y, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(6 * IN_MILLISECONDS));
                                events.ScheduleEvent(EVENT_FELFIRE_CHARGE_EFF, 100);
                            }
                            events.ScheduleEvent(EVENT_FELFIRE_CHARGE, urand(20 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case EVENT_FELFIRE_CHARGE_EFF:
                            me->GetMotionMaster()->MoveCharge(x, y, me->GetPositionZ(), SPEED_CHARGE, EVENT_CHARGE);
                            break;
                        case EVENT_FELFIRE_SHOCK:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_FELFIRE_SHOCK);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_FELFIRE_SHOCK);

                            events.ScheduleEvent(EVENT_FELFIRE_SHOCK, urand(6 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            private:
                struct FelrushPredicate 
                {
                    public:
                        FelrushPredicate(Creature const* me) : _me(me) { }

                        bool operator()(Unit const* target) const
                        {
                            return target && target->GetTypeId() == TYPEID_PLAYER && target->GetExactDist2d(_me) > 6.0f;
                        }

                    private:
                        Creature const* _me;
                };
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_wrath_scryer_soccothratesAI(creature);
        }
};

// Scryer Felfire 20978
class npc_scryer_felfire : public CreatureScript
{
    public:
        npc_scryer_felfire() : CreatureScript("npc_scryer_felfire") { }

        struct npc_scryer_felfireAI : public ScriptedAI
        {
            npc_scryer_felfireAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            float x, y;
            uint32 felData;

            void Reset() override
            {
                x = 0.0f; y = 0.0f;
                felData = 0;
                DoCast(me, SPELL_FELFIRE);

                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (Creature* Scryer = ObjectAccessor::GetCreature(*me, instance->GetGuidData(TYPE_SOCCOTHRATES)))
                    {
                        if (felData = Scryer->AI()->GetData(TYPE_FELFIRE))
                        {
                            if (uint32 felCalculate = Scryer->AI()->GetData(TYPE_FELFIRE_COUNT))
                            {
                                if (felData <= felCalculate)
                                {
                                    felData++;
                                    Scryer->AI()->SetData(TYPE_FELFIRE, felData);
                                    GetPositionWithDistInOrientation(me, 5.0f, me->GetOrientation(), x, y);
                                    events.ScheduleEvent(EVENT_FELFIRE_CHARGE, 50);
                                }
                            }
                        }
                    }
                }
            }

            void UpdateAI(uint32 diff) override 
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId==EVENT_FELFIRE_CHARGE)
                        me->SummonCreature(NPC_SCRYER_FELFIRE, x, y, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(6 * IN_MILLISECONDS));
                    break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_scryer_felfireAI(creature);
        }
};

void AddSC_boss_wrath_scryer_soccothrates()
{
    new boss_wrath_scryer_soccothrates();
    new npc_scryer_felfire();
}