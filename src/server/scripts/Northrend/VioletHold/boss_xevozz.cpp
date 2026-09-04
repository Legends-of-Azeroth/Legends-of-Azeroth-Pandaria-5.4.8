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
#include "violet_hold.h"

enum Spells
{
    SPELL_ARCANE_BARRAGE_VOLLEY               = 54202,
    SPELL_ARCANE_BARRAGE_VOLLEY_H             = 59483,
    SPELL_SUMMON_ETHEREAL_SPHERE_1            = 54102,
    SPELL_SUMMON_ETHEREAL_SPHERE_2            = 54137,
    SPELL_SUMMON_ETHEREAL_SPHERE_3            = 54138,
    SPELL_ARCANE_BOLT                           = 13748
};

enum NPCs
{
    NPC_ETHEREAL_SPHERE                       = 29271,
};

enum CreatureSpells
{
    SPELL_ARCANE_POWER                             = 54160,
    SPELL_ARCANE_POWER_H                           = 59474,
    SPELL_ARCANE_BUFFED                            = 54226,
    SPELL_ARCANE_BUFFED_H                          = 59485,
    SPELL_SUMMON_PLAYERS                           = 54164,
    SPELL_POWER_BALL_VISUAL                        = 54141,
};

enum Yells
{
    // Xevozz
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_CHARGED                                 = 4,
    SAY_REPEAT_SUMMON                           = 5,
    SAY_SUMMON_ENERGY                           = 6,

    // Ethereal Sphere
    SAY_ETHEREAL_SPHERE_SUMMON                  = 0,
};

class boss_xevozz : public CreatureScript
{
    public:
        boss_xevozz() : CreatureScript("boss_xevozz") { }

        struct boss_xevozzAI : public ScriptedAI
        {
            boss_xevozzAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
            {
                _scheduler.SetValidator([this]
                {
                    return !me->HasUnitState(UNIT_STATE_CASTING);
                });
            }

            void Reset() override
            {
                _scheduler.CancelAll();

                if (_instance)
                {
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                        _instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                        _instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }

                DespawnSphere();
            }

            void DespawnSphere()
            {
                std::list<Creature*> assistList;
                GetCreatureListWithEntryInGrid(assistList, me, NPC_ETHEREAL_SPHERE, 150.0f);

                if (assistList.empty())
                    return;

                for (auto&& pSphere : assistList)
                    if (pSphere)
                        pSphere->Kill(pSphere, false);
            }

            void JustSummoned(Creature* summon) override
            {
                summon->SetSpeed(MOVE_RUN, 0.5f);
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                {
                    summon->AddThreat(target, 0.00f);
                    summon->AI()->AttackStart(target);
                }
            }

            void AttackStart(Unit* who) override
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who);
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                if (_instance)
                {
                    if (GameObject* pDoor = _instance->instance->GetGameObject(_instance->GetGuidData(DATA_XEVOZZ_CELL)))
                        if (pDoor->GetGoState() == GO_STATE_READY)
                        {
                            EnterEvadeMode();
                            return;
                        }
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                        _instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                        _instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
                }

                _scheduler
                    .Schedule(Seconds(8), Seconds(10), [this](TaskContext context)
                    {
                        DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_BARRAGE_VOLLEY, SPELL_ARCANE_BARRAGE_VOLLEY_H));
                        context.Repeat(Seconds(8), Seconds(10));
                    })
                    .Schedule(Seconds(10), Seconds(11), [this](TaskContext context)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 45, true))
                            DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BUFFED, SPELL_ARCANE_BUFFED_H));
                        context.Repeat(Seconds(15), Seconds(20));
                    })
                    .Schedule(Seconds(5), [this](TaskContext context)
                    {
                        Talk(SAY_REPEAT_SUMMON);
                        uint32 summonSpell = SPELL_SUMMON_ETHEREAL_SPHERE_1 + rand() % 3;
                        DoCast(me, summonSpell);
                        context.Repeat(Seconds(45), Seconds(47));
                    });
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                ScriptedAI::EnterEvadeMode();
                if (_instance)
                    _instance->SetData(DATA_WIPE, 1);
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _scheduler.Update(diff, [this]
                {
                    DoMeleeAttackIfReady();
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                _scheduler.CancelAll();
                Talk(SAY_DEATH);

                DespawnSphere();

                if (_instance)
                {
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                    {
                        if (IsHeroic() && _instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        _instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                        _instance->SetData(DATA_WAVE_COUNT, 7);
                    }
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                    {
                        if (IsHeroic() && _instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
                            me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                        _instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                        _instance->SetData(DATA_WAVE_COUNT, 13);
                    }
                }
            }

            void SpellHit(Unit* /*who*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_ARCANE_POWER || spell->Id == SPELL_ARCANE_POWER_H)
                    Talk(SAY_SUMMON_ENERGY);
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(SAY_SLAY);
            }

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_xevozzAI(creature);
        }
};

class npc_ethereal_sphere : public CreatureScript
{
    public:
        npc_ethereal_sphere() : CreatureScript("npc_ethereal_sphere") { }

        struct npc_ethereal_sphereAI : public ScriptedAI
        {
            npc_ethereal_sphereAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
            {
                _scheduler.SetValidator([this]
                {
                    return !me->HasUnitState(UNIT_STATE_CASTING);
                });
            }

            void Reset() override
            {
                _scheduler.CancelAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _scheduler
                    .Schedule(Seconds(1), [this](TaskContext context)
                    {
                        if (!me->HasAura(SPELL_POWER_BALL_VISUAL))
                            DoCast(me, SPELL_POWER_BALL_VISUAL);

                        if (_instance)
                        {
                            if (Creature* pXevozz = Unit::GetCreature(*me, _instance->GetGuidData(DATA_XEVOZZ)))
                            {
                                float fDistance = me->GetDistance2d(pXevozz);
                                if (fDistance <= 3)
                                    DoCast(pXevozz, SPELL_ARCANE_POWER);
                                else
                                    DoCast(me, 35845);
                            }
                        }
                        context.Repeat(Seconds(1));
                    })
                    .Schedule(Seconds(33), Seconds(35), [this](TaskContext context)
                    {
                        Talk(SAY_ETHEREAL_SPHERE_SUMMON);
                        DoCast(me, SPELL_SUMMON_PLAYERS);

                        Map* map = me->GetMap();
                        if (map && map->IsDungeon())
                        {
                            Map::PlayerList const& PlayerList = map->GetPlayers();
                            if (!PlayerList.isEmpty())
                                for (auto&& ref : PlayerList)
                                    if (ref.GetSource()->IsAlive())
                                        DoTeleportPlayer(ref.GetSource(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), ref.GetSource()->GetOrientation());
                        }

                        context.Repeat(Seconds(33), Seconds(35));
                    });
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _scheduler.Update(diff);
            }

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ethereal_sphereAI(creature);
        }
};

void AddSC_boss_xevozz()
{
    new boss_xevozz();
    new npc_ethereal_sphere();
}
