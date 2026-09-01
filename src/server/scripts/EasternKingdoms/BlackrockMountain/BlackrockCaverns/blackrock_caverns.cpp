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
#include "blackrock_caverns.h"

enum eTalks
{
    TALK_INTRO     = 0,
};

// Raz the Crazed 39670
class npc_raz_the_crazed : public CreatureScript
{
    public:
        npc_raz_the_crazed() : CreatureScript("npc_raz_the_crazed") { }

        enum iEvents
        {
            EVENT_CLEAVE       = 1,
            EVENT_BREAK_PRISON = 2,
            EVENT_MOVE         = 3,
        };

        enum iSpells
        {
            SPELL_EVIL_CLEAVE   = 80606,
            SPELL_SHADOW_PRISON = 79725,
            SPELL_FURIOUS_RAGE  = 80218,
        };

        struct npc_raz_the_crazedAI : public ScriptedAI
        {
            npc_raz_the_crazedAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;
            uint32 wp;
            bool SecondEvent;

            void InitializeAI() override
            {
                wp = 0;
                SecondEvent = false;
                DoCast(me, SPELL_SHADOW_PRISON);
            }

            void Reset() override
            {
                events.Reset();
                nonCombatEvents.Reset();
                nonCombatEvents.ScheduleEvent(EVENT_MOVE, 1500);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_RAZ_BREAK_PRISON:
                        me->SetAnimTier(AnimTier::Fly);
                        me->OverrideInhabitType(INHABIT_AIR);
                        me->UpdateMovementFlags();
                        nonCombatEvents.ScheduleEvent(EVENT_BREAK_PRISON, 2.5 * IN_MILLISECONDS);
                        break;
                    case ACTION_RAZ_NEXT_BRIDGE:
                        if (SecondEvent)
                            break;

                        wp = 0;
                        SecondEvent = true;
                        me->GetMotionMaster()->MoveJump(RazSecondBridgePath[wp], 25.0f, 20.0f, EVENT_JUMP + 2);
                        break;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, 0.5 * IN_MILLISECONDS);
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                switch (pointId)
                {
                    case EVENT_JUMP:
                        me->SetHomePosition(*me);
                        me->SetFaction(1665);
                        Talk(TALK_INTRO);
                        me->SetAnimTier(AnimTier::Ground);
                        me->OverrideInhabitType(INHABIT_GROUND);
                        me->UpdateMovementFlags();
                        nonCombatEvents.ScheduleEvent(EVENT_MOVE, 2 * IN_MILLISECONDS);
                        break;
                    case EVENT_JUMP + 1:
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        break;
                    case EVENT_JUMP + 2:
                        me->SetHomePosition(*me);
                        Talk(TALK_INTRO);
                        nonCombatEvents.ScheduleEvent(EVENT_MOVE, 2 * IN_MILLISECONDS);
                        break;
                    case EVENT_JUMP + 3:
                        me->DespawnOrUnsummon(1000);
                        break;
                }

                if (type != POINT_MOTION_TYPE)
                    return;

                wp++;

                switch (pointId)
                {
                    case 7:
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveJump(RazFirstBridgePath[wp], 25.0f, 20.0f, EVENT_JUMP + 1);
                        break;
                    case 3:
                        if (SecondEvent)
                        {
                            me->SetFaction(35);
                            me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveJump(RazSecondBridgePath[wp], 45.0f, 45.0f, EVENT_JUMP + 3);
                        }
                        else
                            nonCombatEvents.ScheduleEvent(EVENT_MOVE, urand(100, 200));
                        break;
                    default:
                        nonCombatEvents.ScheduleEvent(EVENT_MOVE, urand(100, 200));
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BREAK_PRISON:
                            me->RemoveAurasDueToSpell(SPELL_SHADOW_PRISON);
                            me->GetMotionMaster()->MoveJump(RazFirstBridgePath[0], 25.0f, 20.0f, EVENT_JUMP);
                            break;
                        case EVENT_MOVE:
                            if (me->IsInCombat())
                                return;
                            me->GetMotionMaster()->MovementExpired();
                            me->GetMotionMaster()->Clear();

                            if (me->GetUnitMovementFlags() == MOVEMENTFLAG_WALKING)
                                me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);

                            me->GetMotionMaster()->MovePoint(wp, SecondEvent ? RazSecondBridgePath[wp] : RazFirstBridgePath[wp]);
                            break;
                    }
                }

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_CLEAVE)
                    {
                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_EVIL_CLEAVE, true);

                        DoCast(me, SPELL_FURIOUS_RAGE, true);
                        events.ScheduleEvent(EVENT_CLEAVE, urand(1.5 * IN_MILLISECONDS, 2.5 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_raz_the_crazedAI(creature);
        }
};

class blackrock_caverns_teleport: public CreatureScript
{
    public:
        blackrock_caverns_teleport() : CreatureScript("blackrock_caverns_teleport") { }

        struct blackrock_caverns_teleport_AI : public ScriptedAI
        {
           blackrock_caverns_teleport_AI(Creature* creature) : ScriptedAI(creature) { }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (instance->GetData(DATA_KARSH) != DONE)
                        return;

                if (me->GetDBTableGUIDLow() == 327777)
                    clicker->NearTeleportTo(284.838f, 840.364f, 95.920f, 3.881242f, false);
                else if (me->GetDBTableGUIDLow() == 327776)
                    clicker->NearTeleportTo(233.555f, 1128.875f, 205.569f, 3.551373f, false);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<blackrock_caverns_teleport_AI>(creature);
        }
};

// Aggro Nearby Targets 80196
class spell_aggro_nearby_target : public SpellScript
{
    PrepareSpellScript(spell_aggro_nearby_target);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (Unit* caster = GetCaster())
            targets.remove_if([=](WorldObject* target) { return !target->ToPlayer(); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_aggro_nearby_target::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// AreaTrigger 6014
class AreaTrigger_at_second_bridge_blackrock_caverns : public AreaTriggerScript
{
    public:
        AreaTrigger_at_second_bridge_blackrock_caverns() : AreaTriggerScript("AreaTrigger_at_second_bridge_blackrock_caverns") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (Creature* Raz = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_RAZ_THE_CRAZED)))
                    Raz->AI()->DoAction(ACTION_RAZ_NEXT_BRIDGE);

            return false;
        }
};

void AddSC_blackrock_caverns()
{
    new npc_raz_the_crazed();
    new blackrock_caverns_teleport();
    new spell_script<spell_aggro_nearby_target>("spell_aggro_nearby_target");
    new AreaTrigger_at_second_bridge_blackrock_caverns();
}