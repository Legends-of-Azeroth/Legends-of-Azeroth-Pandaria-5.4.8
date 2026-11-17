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

/* ScriptData
SDName: Boss_Mekgineer_Steamrigger
SD%Complete: 60
SDComment: Mechanics' interrrupt heal doesn't work very well, also a proper movement needs to be implemented -> summon further away and move towards target to repair.
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "steam_vault.h"

enum Yells
{
    SAY_MECHANICS               = 0,
    SAY_AGGRO                   = 1,
    SAY_SLAY                    = 2,
    SAY_DEATH                   = 3
};

enum Spells
{
    SPELL_SUPER_SHRINK_RAY      = 31485,
    SPELL_SAW_BLADE             = 31486,
    SPELL_ELECTRIFIED_NET       = 35107,

    SPELL_DISPEL_MAGIC          = 17201,
    SPELL_REPAIR                = 31532,
    H_SPELL_REPAIR              = 37936
};

enum Events
{
    EVENT_SHRINK   = 1,
    EVENT_NET      = 2,
    EVENT_SAWBLADE = 3,
};

enum Creatures
{
    NPC_STREAMRIGGER_MECHANIC   = 17951
};

class boss_mekgineer_steamrigger : public CreatureScript
{
    public:
        boss_mekgineer_steamrigger() : CreatureScript("boss_mekgineer_steamrigger") { }

        struct boss_mekgineer_steamriggerAI : public ScriptedAI
        {
            boss_mekgineer_steamriggerAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;
            EventMap events;
            uint32 enginerPhase;

            void InitializeAI() override
            {
                me->setActive(true);
                instance = me->GetInstanceScript();
                Reset();
            }

            void Reset() override
            {
                events.Reset();
                enginerPhase = 75;

                if (instance)
                    instance->SetBossState(DATA_MEKGINEER_STEAMRIGGER, NOT_STARTED);
            }

            void JustDied(Unit* /*killer*/) override
            {
                Talk(SAY_DEATH);

                if (instance)
                {
                    instance->SetBossState(DATA_MEKGINEER_STEAMRIGGER, DONE);

                    if (instance)
                        if (GameObject* goMekDoor = ObjectAccessor::GetGameObject(*me, instance->GetGuidData(GO_ACCESS_PANEL_MEK)))
                            goMekDoor->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_SLAY);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_SHRINK, 20 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SAWBLADE, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_NET, 10 * IN_MILLISECONDS);

                if (instance)
                    instance->SetBossState(DATA_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
            }

            //no known summon spells exist
            void SummonMechanichs()
            {
                Talk(SAY_MECHANICS);

                DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, 5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000ms);
                DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, -5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000ms);
                DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, -5, -5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000ms);

                if (rand()%2)
                    DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, 5, -7, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000ms);
                if (rand()%2)
                    DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, 7, -5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000ms);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(enginerPhase) && enginerPhase >= 25)
                {
                    enginerPhase -= 25;
                    SummonMechanichs();
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHRINK:
                            DoCast(me, SPELL_SUPER_SHRINK_RAY);
                            events.ScheduleEvent(EVENT_SHRINK, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_SAWBLADE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_SAW_BLADE);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_SAW_BLADE);

                            events.ScheduleEvent(EVENT_SAWBLADE, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_NET:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_ELECTRIFIED_NET);

                            events.ScheduleEvent(EVENT_NET, urand(11.5 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_mekgineer_steamriggerAI(creature);
        }
};

#define MAX_REPAIR_RANGE            (13.0f)                 //we should be at least at this range for repair
#define MIN_REPAIR_RANGE            (7.0f)                  //we can stop movement at this range to repair but not required

class npc_steamrigger_mechanic : public CreatureScript
{
    public:
        npc_steamrigger_mechanic() : CreatureScript("npc_steamrigger_mechanic") { }

        struct npc_steamrigger_mechanicAI : public ScriptedAI
        {
            npc_steamrigger_mechanicAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 Repair_Timer;

            void Reset() override
            {
                Repair_Timer = 2000;
            }

            void MoveInLineOfSight(Unit* /*who*/) override
            {
                //react only if attacked
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (Repair_Timer <= diff)
                {
                    if (instance && instance->GetBossState(DATA_MEKGINEER_STEAMRIGGER) == IN_PROGRESS)
                    {
                        if (Unit* pMekgineer = Unit::GetUnit(*me, instance->GetGuidData(DATA_MEKGINEER_STEAMRIGGER)))
                        {
                            if (me->IsWithinDistInMap(pMekgineer, MAX_REPAIR_RANGE))
                            {
                                //are we already channeling? Doesn't work very well, find better check?
                                if (!me->GetUInt32Value(UNIT_FIELD_CHANNEL_SPELL))
                                {
                                    //me->GetMotionMaster()->MovementExpired();
                                    //me->GetMotionMaster()->MoveIdle();

                                    DoCast(me, SPELL_REPAIR, true);
                                }
                                Repair_Timer = 5000;
                            }
                            else
                            {
                                //me->GetMotionMaster()->MovementExpired();
                                //me->GetMotionMaster()->MoveFollow(pMekgineer, 0, 0);
                            }
                        }
                    } else Repair_Timer = 5000;
                } else Repair_Timer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_steamrigger_mechanicAI(creature);
        }
};

void AddSC_boss_mekgineer_steamrigger()
{
    new boss_mekgineer_steamrigger();
    new npc_steamrigger_mechanic();
}
