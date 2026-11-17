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
#include "the_stonecore.h"

enum Spells
{
    // Crystalspawn Giant
    SPELL_QUAKE              = 81008,
    H_SPELL_QUAKE            = 92631,

    // Imp
    SPELL_FELL_FIREBALL      = 80344,
    H_SPELL_FELL_FIREBALL    = 92638, 

    // Millhouse Manastorm
    SPELL_BLUR               = 81216,
    SPELL_MILL_FEAR          = 81442,
    SPELL_FROSTBOLT_VOLLEY   = 81440,           
    H_SPELL_FROSTBOLT_VOLLEY = 92642,
    SPELL_IMPENDING_DOOM     = 86830,
    SPELL_SHADOW_BOLT        = 81439,           
    H_SPELL_SHADOW_BOLT      = 92641,
    SPELL_SHADOWFURY         = 81441,
    H_SPELL_SHADOWFURY       = 92644,
    SPELL_TIGULE             = 81220,

    // Stonecore Berserker
    SPELL_SCHARGE            = 81574,
    SPELL_SPINNING_SLASH     = 81568,

    // Stonecore Bruiser
    SPELL_BODY_SLAM          = 80180,
    SPELL_SHOCKWAVE          = 80195,
    H_SPELL_SHOCKWAVE        = 92640,

    // Stonecore Earthshaper 
    SPELL_DUST_STORM         = 81463,
    SPELL_FORCE_OF_EARTH     = 81459,
    SPELL_GROUND_SHOCK       = 81530,       
    H_SPELL_GROUND_SHOCK     = 92628,
    SPELL_LAVA_BURST         = 81576,
    H_SPELL_LAVA_BURST       = 92626, 

    // Stonecore Flayer 
    SPELL_FLAY               = 79922,

    // Stonecore Magmalord (
    SPELL_IGNITE             = 80151,
    H_SPELL_IGNITE           = 92636,
    SPELL_MAGMA_ERUPTION     = 80038,

    // Stonecore Rift Conjurer 
    SPELL_DEMON_PORTAL       = 80308,
    SPELL_SHADOWBOLT         = 80279,             
    H_SPELL_SHADOWBOLT       = 92637,

    //Stonecore Sentry
    
    // Stonecore Warbringer 
    SPELL_CLEAVE             = 15496,
    SPELL_RAGE               = 80158,
};

enum eEvents
{
    EVENT_NONE,
    EVENT_QUAKE,
    EVENT_FELL_FIREBALL,
    EVENT_BLUR,
    EVENT_MILL_FEAR,
    EVENT_FROSTBOLT_VOLLEY,
    EVENT_IMPENDING_DOOM,
    EVENT_SHADOW_BOLT,
    EVENT_SHADOWFURY,
    EVENT_TIGULE,
    EVENT_ROCK_BORE,
    EVENT_SCHARGE,
    EVENT_SPINNING_SLASH,
    EVENT_BODY_SLAM,
    EVENT_SHOCKWAVE,
    EVENT_DUST_STORM,
    EVENT_FORCE_OF_EARTH,
    EVENT_GROUND_SHOCK,
    EVENT_LAVA_BURST,
    EVENT_FLAY,
    EVENT_IGNITE,
    EVENT_MAGMA_ERUPTION,
    EVENT_DEMON_PORTAL,
    EVENT_SHADOWBOLT,
    EVENT_CLEAVE,
    EVENT_RAGE,
    EVENT_COSMETIC_MOVE,
};

class npc_crystalspawn_giant : public CreatureScript
{
    public:
        npc_crystalspawn_giant() : CreatureScript("npc_crystalspawn_giant") { }

        struct npc_crystalspawn_giantAI : public ScriptedAI
        {
            npc_crystalspawn_giantAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_QUAKE, 5000 + rand()%5000);
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
                        case EVENT_QUAKE:
                            DoCast(me->GetVictim(), SPELL_QUAKE);
                            events.RescheduleEvent(EVENT_QUAKE, 5000 + rand()%5000);
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_crystalspawn_giantAI>(creature);
        }
};

class npc_impp : public CreatureScript
{
public:
        npc_impp() : CreatureScript("npc_impp") { }

        struct npc_imppAI : public ScriptedAI
        {
            npc_imppAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FELL_FIREBALL, 1000);
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
                        case EVENT_FELL_FIREBALL:
                            if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FELL_FIREBALL);
                            events.RescheduleEvent(EVENT_FELL_FIREBALL, 1000);
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_imppAI>(creature);
        }
};

enum ScriptTexts
{
    SAY_EVENT_1    = 0,
    SAY_EVENT_2    = 1,
    SAY_EVENT_3    = 2,
    SAY_DOOM       = 3,
};

const Position millhousemanastormscPos[3] = 
{
    { 987.67f,  882.45f, 303.37f, 2.07f },
    { 1075.72f, 862.74f, 291.48f, 2.86f },
    { 1151.45f, 885.74f, 284.96f, 3.36f }
};

class npc_sc_millhouse_manastorm : public CreatureScript
{
    public:
        npc_sc_millhouse_manastorm() : CreatureScript("npc_sc_millhouse_manastorm") { }

        struct npc_sc_millhouse_manastormAI : public ScriptedAI
        {
            npc_sc_millhouse_manastormAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events, nonCombatEvents;
            uint8 stage;

            void InitializeAI() override
            {
                stage = 1;
                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MILL_FEAR, 10000);
                events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, 7000 + rand()%10000);
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 1000);
                events.ScheduleEvent(EVENT_SHADOWFURY, 5000 + rand()%15000);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if ((damage >= me->GetHealth() || me->HealthBelowPct(51)) && !me->HasAura(SPELL_BLUR) && stage < 4)
                {
                    damage = 0;
                    me->RemoveAllAuras();
                    Talk(stage - 1);
                    DoCast(me, SPELL_BLUR);
                    me->PrepareChanneledCast(me->GetOrientation());
                    stage++;

                    Movement::MoveSplineInit init{ me };

                    for (uint8 i = stage < 3 ? 0 : stage - 1; i < stage; i++)
                    {
                        if (stage < 3)
                            init.Path().push_back(G3D::Vector3(ManaStormPathEvent[i].GetPositionX(), ManaStormPathEvent[i].GetPositionY(), ManaStormPathEvent[i].GetPositionZ()));
                        else
                            init.MoveTo(ManaStormPathEvent[i].GetPositionX(), ManaStormPathEvent[i].GetPositionY(), ManaStormPathEvent[i].GetPositionZ());
                    }

                    init.SetSmooth();
                    init.Launch();
                    nonCombatEvents.ScheduleEvent(EVENT_COSMETIC_MOVE, me->GetSplineDuration());
                }
            }

            void MovementInform(uint32 type, uint32 pointId) override
            {
                if (pointId == EVENT_JUMP)
                    me->DespawnOrUnsummon();
            }

            void UpdateAI(uint32 diff) override
            {
                nonCombatEvents.Update(diff);

                while (uint32 eventId = nonCombatEvents.ExecuteEvent())
                {
                    if (eventId == EVENT_COSMETIC_MOVE)
                    {
                        me->SetHomePosition(*me);
                        me->RemoveAurasDueToSpell(SPELL_BLUR);
                        me->StopMoving();
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetFacingTo(Position::NormalizeOrientation(me->GetOrientation() - M_PI));
                        EnterEvadeMode();
                    }
                    break;
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
                        case EVENT_IMPENDING_DOOM:
                            me->Kill(me);
                            break;
                        case EVENT_MILL_FEAR:
                            if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_MILL_FEAR);
                            events.RescheduleEvent(EVENT_MILL_FEAR, 10000);
                            return;
                        case EVENT_SHADOW_BOLT:
                            DoCast(me->GetVictim(), SPELL_SHADOW_BOLT);
                            events.RescheduleEvent(EVENT_SHADOWBOLT, 1000);
                            return;
                        case EVENT_FROSTBOLT_VOLLEY:
                            if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FROSTBOLT_VOLLEY);
                            events.RescheduleEvent(EVENT_FROSTBOLT_VOLLEY, rand()%15000);
                            return;
                        case EVENT_SHADOWFURY:
                            if (Unit *target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SHADOWFURY);
                            events.RescheduleEvent(SPELL_SHADOWFURY, 5000 + rand()%15000);
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_sc_millhouse_manastormAI>(creature);
        }
};

class npc_stonecore_instance_portal: public CreatureScript
{
    public:
        npc_stonecore_instance_portal() : CreatureScript("npc_stonecore_instance_portal") { }

        struct npc_stonecore_instance_portal_AI : public ScriptedAI
        {
           npc_stonecore_instance_portal_AI(Creature* creature) : ScriptedAI(creature) { }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (instance->GetBossState(DATA_SLABHIDE) != DONE)
                        return;

                if (me->GetEntry() == 51396)
                    clicker->NearTeleportTo(1313.197f, 1236.351f, 246.957f, 4.733236f, false);
                else if (me->GetEntry() == 51397)
                    clicker->NearTeleportTo(853.575f, 999.710f, 317.326f, 4.591864f, false);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_stonecore_instance_portal_AI>(creature);
        }
};

// AreaTrigger 6076
class AreaTrigger_at_behind_corborus : public AreaTriggerScript
{
    public:
        AreaTrigger_at_behind_corborus() : AreaTriggerScript("AreaTrigger_at_behind_corborus") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                instance->SetData(DATA_CORBORUS, SPECIAL);

            return false;
        }
};

void AddSC_the_stonecore()
{
    new npc_crystalspawn_giant();
    new npc_impp();
    new npc_sc_millhouse_manastorm();
    new npc_stonecore_instance_portal();
    new AreaTrigger_at_behind_corborus();
}
