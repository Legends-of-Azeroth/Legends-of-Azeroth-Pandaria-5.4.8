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

#include"ScriptPCH.h"
#include"the_vortex_pinnacle.h"

enum Spells
{
    SPELL_CALL_OF_WIND          = 88244,
    SPELL_CALL_OF_WIND_DUMMY_1  = 88276,
    SPELL_CALL_OF_WIND_DUMMY_2  = 88772,
    SPELL_DOWNWIND_OF_ALTAIRUS  = 88286,
    SPELL_UPWIND_OF_ALTAIRUS    = 88282,
    SPELL_CHILLING_BREATH       = 88308,
    SPELL_CHILLING_BREATH_DUMMY = 88322,
    SPELL_LIGHTNING_BLAST       = 88357,
    SPELL_LIGHTNING_BLAST_DUMMY = 88332,
    SPELL_TWISTER_AURA          = 88313,
    SPELL_TWISTER_AURA_DMG      = 88314,

};

enum Events
{
    EVENT_CALL_OF_WIND = 1,
    EVENT_CHILLING_BREATH,
    EVENT_LIGHTNING_BLAST,
    EVENT_CHECK_FACING,
    EVENT_RESET_WIND,
    EVENT_TWISTING_WINDS,
};

class boss_altairus : public CreatureScript
{
    public:
        boss_altairus() : CreatureScript("boss_altairus") { }

        struct boss_altairusAI : public BossAI
        {
            boss_altairusAI(Creature* creature) : BossAI(creature, DATA_ALTAIRUS)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->setActive(true);
            }

            Creature* _aircurrent;
            uint8 _twisternum;

            void Reset() override
            {
                _Reset();
                _twisternum = 0;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                // Should have Fly anim in combat too
                me->OverrideInhabitType(INHABIT_AIR);
                me->SetAnimTier(AnimTier::Fly);
                me->UpdateMovementFlags();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_CHILLING_BREATH, urand(5000, 10000));
                events.ScheduleEvent(EVENT_CALL_OF_WIND, 2000);
                events.ScheduleEvent(EVENT_CHECK_FACING, 2500);

                if (IsHeroic())
                {
                    HandleTwisting(3);
                    events.ScheduleEvent(EVENT_TWISTING_WINDS, 6.5 * IN_MILLISECONDS);
                }

                DoZoneInCombat();
                instance->SetBossState(DATA_ALTAIRUS, IN_PROGRESS);
            }

            void EnterEvadeMode() override
            {
                _EnterEvadeMode();
                BossAI::EnterEvadeMode();

                summons.DespawnAll();

                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetBossState(DATA_ALTAIRUS, FAIL);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            bool CheckOrientation(float player, float creature)
            {
                float _cur, _up, _down;

                if (creature > M_PI)
                    _cur = creature - M_PI;
                else
                    _cur = creature + M_PI;


                _up = _cur + 1.0f;
                _down = _cur - 1.0f;

                if (player > _down && player < _up)
                    return true;
                else
                    return false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetVictim() && me->GetVictim()->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 55.0f)
                {
                    DoCast(me->GetVictim(), SPELL_LIGHTNING_BLAST);
                    return;
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHILLING_BREATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0.0f, true))
                                DoCast(target, SPELL_CHILLING_BREATH);

                            events.ScheduleEvent(EVENT_CHILLING_BREATH, urand(10000, 16000));
                            break;
                        case EVENT_RESET_WIND:
                            if (_aircurrent)
                                _aircurrent->DespawnOrUnsummon();
                            events.DelayEvents(1000);
                            events.ScheduleEvent(EVENT_CALL_OF_WIND, 800);
                            break;
                        case EVENT_CALL_OF_WIND:
                            _aircurrent = me->SummonCreature(NPC_AIR_CURRENT,
                                me->GetPositionX(),
                                me->GetPositionY(),
                                me->GetPositionZ(),
                                orientations[urand(0, 3)]);
                            events.ScheduleEvent(EVENT_RESET_WIND, 18000);
                            break;
                        case EVENT_CHECK_FACING:
                            if (me->GetMap()->GetPlayers().isEmpty() || !_aircurrent)
                                break;

                            for (Map::PlayerList::const_iterator itr = me->GetMap()->GetPlayers().begin(); itr != me->GetMap()->GetPlayers().end(); ++itr) 
                            {
                                if (CheckOrientation(itr->GetSource()->GetOrientation(), _aircurrent->GetOrientation()))
                                {
                                    itr->GetSource()->RemoveAurasDueToSpell(SPELL_DOWNWIND_OF_ALTAIRUS);
                                    me->AddAura(SPELL_UPWIND_OF_ALTAIRUS, itr->GetSource());
                                }
                                else
                                {
                                    itr->GetSource()->RemoveAurasDueToSpell(SPELL_UPWIND_OF_ALTAIRUS);
                                    me->AddAura(SPELL_DOWNWIND_OF_ALTAIRUS, itr->GetSource());
                                }
                            }
                            events.ScheduleEvent(EVENT_CHECK_FACING, 3000);
                            break;
                        case EVENT_TWISTING_WINDS:
                            HandleTwisting(1);
                            events.ScheduleEvent(EVENT_TWISTING_WINDS, urand(8 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

        private:
            void HandleTwisting(uint8 count)
            {
                for (uint8 i = 0; i < count; i++)
                    me->SummonCreature(NPC_TWISTER, AlrairusCenter.GetPositionX() + frand(-20.0f, 25.0f), AlrairusCenter.GetPositionY() + frand(-28.0f, 28.0f), AlrairusCenter.GetPositionZ(), AlrairusCenter.GetOrientation());
            }
        };
        
        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_altairusAI>(creature);
        }
};

// Air Current 47305
struct npc_air_current : public ScriptedAI
{
    npc_air_current(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void Reset() override
    {
        //DoCast(me, SPELL_CALL_OF_WIND_DUMMY_1);
        DoCast(me, SPELL_CALL_OF_WIND_DUMMY_2);
        //DoCast(me, SPELL_CALL_OF_WIND);
    }

    void UpdateAI(uint32 /*diff*/) override { }
};

// Twister 47342
struct npc_altairus_twister : public ScriptedAI
{
    npc_altairus_twister(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        me->OverrideInhabitType(INHABIT_AIR);
        me->SetAnimTier(AnimTier::Fly);
        me->UpdateMovementFlags();

        DoCast(me, SPELL_TWISTER_AURA);
        x = me->GetPositionX(); y = me->GetPositionY();

        scheduler
            .SetValidator([this] { return me->GetInstanceScript() && me->GetInstanceScript()->GetBossState(DATA_ALTAIRUS) == IN_PROGRESS; })
            .Schedule(Milliseconds(1000), [this](TaskContext context)
        {
            float postX = x + frand(-6.0f, 6.0f);
            float postY = y + frand(-6.0f, 6.0f);
            me->GetMotionMaster()->MovePoint(0, postX, postY, me->GetPositionZ(), me->GetOrientation());
            context.Repeat(Milliseconds(me->GetSplineDuration()));
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

void AddSC_boss_altairus()
{
    new boss_altairus();
    new creature_script<npc_air_current>("npc_air_current");
    new creature_script<npc_altairus_twister>("npc_altairus_twister");
}