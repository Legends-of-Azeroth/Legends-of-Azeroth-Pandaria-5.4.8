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
#include <ctgmath>

enum ScriptTexts
{
    SAY_AGGRO = 0,
    SAY_KILL  = 1,
};

enum Spells
{
    SPELL_CYCLONE_SHIELD     = 86267,
    SPELL_CYCLONE_SHIELD_DMG = 86292,
    SPELL_SUMMON_TEMPEST     = 86340,
    SPELL_STORM_EDGE         = 86309,
    SPELL_LIGHTNING_BOLT     = 86331,
};

enum Events
{
    EVENT_LIGHTNING_BOLT = 1,
    EVENT_STORM_EDGE,
    EVENT_CALL_VORTEX,
    EVENT_RESET_VORTEX,
    EVENT_SUMMON_TEMPEST,

};

enum Actions
{
    ACTION_VORTEX_PULL_FORWARD,
    ACTION_VORTEX_PULL_BACK,
};

class boss_grand_vizier_ertan : public CreatureScript
{
    public:
        boss_grand_vizier_ertan() : CreatureScript("boss_grand_vizier_ertan") { }
    
        struct boss_grand_vizier_ertanAI : public BossAI
        {
            boss_grand_vizier_ertanAI(Creature* creature) : BossAI(creature, DATA_ERTAN)
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

            std::vector<ObjectGuid> _vortexes;
            uint32 m_cPos;
            float _distance, x, y;

            void Reset() override
            {
                _Reset();
                _vortexes.clear();
                m_cPos = 0;
                x = 0.0f;
                y = 0.0f;

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                for (uint8 i = 0; i < 8; i++)
                    if (TempSummon* m_vortex = me->SummonCreature(NPC_ERTAN_VORTEX, ertanvortexPos_1[i]))
                        _vortexes.push_back(m_vortex->GetGUID());

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 3000);
                events.ScheduleEvent(EVENT_CALL_VORTEX, urand(18000, 21000));
                events.ScheduleEvent(EVENT_STORM_EDGE, 5000);

                if (IsHeroic())
                    events.ScheduleEvent(EVENT_SUMMON_TEMPEST, 10 * IN_MILLISECONDS);

                Talk(SAY_AGGRO);
                DoZoneInCombat();
                instance->SetBossState(DATA_ERTAN, IN_PROGRESS);
            }

            void AttackStart(Unit* target) override
            {
                if (target)
                    me->Attack(target, false);
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(SAY_KILL);                
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void HandleVortexMovement(int32 actionId)
            {
                for (auto&& itr : _vortexes)
                    if (Creature* vortex = ObjectAccessor::GetCreature(*me, itr))
                        vortex->AI()->DoAction(actionId);
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
                        case EVENT_LIGHTNING_BOLT:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_LIGHTNING_BOLT);
                        
                            events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 2000);
                            break;
                        case EVENT_CALL_VORTEX:
                            HandleVortexMovement(ACTION_VORTEX_PULL_FORWARD);
                            events.ScheduleEvent(EVENT_RESET_VORTEX, urand(14000, 17000));
                            break;
                        case EVENT_RESET_VORTEX:
                            HandleVortexMovement(ACTION_VORTEX_PULL_BACK);
                            events.ScheduleEvent(EVENT_CALL_VORTEX, urand(20000, 25000));
                            break;
                        case EVENT_STORM_EDGE:
                            if (_vortexes.empty())
                                break;

                            if (Creature* m_vortex = ObjectAccessor::GetCreature(*me, Trinity::Containers::SelectRandomContainerElement(_vortexes)))
                                _distance = me->GetDistance2d(m_vortex);

                            if (me->GetMap()->GetPlayers().isEmpty())
                                return;

                            for (auto&& itr : instance->instance->GetPlayers())
                            {
                                if (Player* player = itr.GetSource())
                                {
                                    if (me->GetDistance2d(player) > _distance)
                                    {
                                        if (ObjectGuid m_vGuid = Trinity::Containers::SelectRandomContainerElement(_vortexes))
                                            if (Creature* m_vortex = ObjectAccessor::GetCreature(*me, m_vGuid))
                                                m_vortex->CastSpell(player, SPELL_STORM_EDGE, true);

                                        DoCast(player, SPELL_STORM_EDGE, true);
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_STORM_EDGE, 2000);
                            break;
                        case EVENT_SUMMON_TEMPEST:
                            DoCast(me, SPELL_SUMMON_TEMPEST);
                            events.ScheduleEvent(EVENT_SUMMON_TEMPEST, urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<boss_grand_vizier_ertanAI>(creature);
        }
};

// Ertan`t Vortex 46007
struct npc_ertan_vortex : public ScriptedAI
{
    npc_ertan_vortex(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    float x, y, dist, prevX, prevY;

    void IsSummonedBy(Unit* /*summoner*/) override
    {
        x = 0.0f; y = 0.0f;
        SetCombatMovement(false);
        DoCast(me, SPELL_CYCLONE_SHIELD);

        dist = 35.8f;
        SetHandleCycleMovement();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_VORTEX_PULL_FORWARD:
                me->StopMoving();
                prevX = me->GetPositionX();
                prevY = me->GetPositionY();

                if (Creature* ertan = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(DATA_ERTAN) : ObjectGuid::Empty))
                {
                    GetPositionWithDistInOrientation(me, dist - 5.0f, me->GetAngle(ertan), x, y);
                    me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), me->GetAngle(ertan));
                }
                break;
            case ACTION_VORTEX_PULL_BACK:
                me->StopMoving();

                me->GetMotionMaster()->MovePoint(0, prevX, prevY, me->GetPositionZ(), 0.0f);
                scheduler
                    .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext /*context*/)
                {
                    me->StopMoving();
                    SetHandleCycleMovement();
                });
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }

    private:
        void SetHandleCycleMovement()
        {
            Movement::MoveSplineInit init(me);
            for (uint8 i = 1; i < 25; ++i)
            {
                x = EranCenterPos.GetPositionX() + dist * cos(Position::NormalizeOrientation(me->GetAngle(EranCenterPos.GetPositionX(), EranCenterPos.GetPositionY()) + (i * M_PI / 12) - M_PI));
                y = EranCenterPos.GetPositionY() + dist * sin(Position::NormalizeOrientation(me->GetAngle(EranCenterPos.GetPositionX(), EranCenterPos.GetPositionY()) + (i * M_PI / 12) - M_PI));
                init.Path().push_back(G3D::Vector3(x, y, me->GetPositionZ()));
            }

            init.SetSmooth();
            init.SetCyclic();
            init.Launch();
        }
};

// Summon Lurker Tempest 86340
class spell_summon_lurker_tempest : public SpellScript
{
    PrepareSpellScript(spell_summon_lurker_tempest);

    void SelectTargets(SpellDestination& dest)
    {
        if (Unit* caster = GetCaster())
        {
            // In Description it should summon on edge of area only
            GetPositionWithDistInOrientation(caster, 35.0f, frand(0, 2 * M_PI), x, y);
            Position newPos = { x, y, caster->GetPositionZ(), 0 };
            dest.Relocate(newPos);
        }
    }

    private:
        float x, y;

        void Register() override
        {
            OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_summon_lurker_tempest::SelectTargets, EFFECT_0, TARGET_DEST_DEST_RADIUS);
        }
};

void AddSC_boss_grand_vizier_ertan()
{
    new boss_grand_vizier_ertan();
    new creature_script<npc_ertan_vortex>("npc_ertan_vortex");
    new spell_script<spell_summon_lurker_tempest>("spell_summon_lurker_tempest");
}