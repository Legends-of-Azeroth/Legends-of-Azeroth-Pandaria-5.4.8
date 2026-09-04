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
    SPELL_BLOODLUST                             = 54516,
    SPELL_BREAK_BONDS                           = 59463,
    SPELL_CHAIN_HEAL                            = 54481,
    SPELL_CHAIN_HEAL_H                          = 59473,
    SPELL_EARTH_SHIELD                          = 54479,
    SPELL_EARTH_SHIELD_H                        = 59471,
    SPELL_EARTH_SHOCK                           = 54511,
    SPELL_LIGHTNING_BOLT                        = 53044,
    SPELL_STORMSTRIKE                           = 51876,
    SPELL_WINDFURY                              = 54493
};

static uint32 const EREKEM_CC_MECHANIC_MASK =
      (1 << MECHANIC_SNARE)
    | (1 << MECHANIC_ROOT)
    | (1 << MECHANIC_FEAR)
    | (1 << MECHANIC_STUN)
    | (1 << MECHANIC_SLEEP)
    | (1 << MECHANIC_CHARM)
    | (1 << MECHANIC_SAPPED)
    | (1 << MECHANIC_HORROR)
    | (1 << MECHANIC_POLYMORPH)
    | (1 << MECHANIC_DISORIENTED)
    | (1 << MECHANIC_FREEZE)
    | (1 << MECHANIC_TURN);

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_ADD_KILLED                              = 4,
    SAY_BOTH_ADDS_KILLED                        = 5,
};

class boss_erekem : public CreatureScript
{
    public:
        boss_erekem() : CreatureScript("boss_erekem") { }

        struct boss_erekemAI : public ScriptedAI
        {
            boss_erekemAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()), _phase(0), _addsKilled(0)
            {
                _scheduler.SetValidator([this]
                {
                    return !me->HasUnitState(UNIT_STATE_CASTING);
                });
            }

            void Reset() override
            {
                _scheduler.CancelAll();
                _phase = 0;
                _addsKilled = 0;
                me->SetCanDualWield(false);

                if (_instance)
                {
                    if (_instance->GetData(DATA_WAVE_COUNT) == 6)
                        _instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                    else if (_instance->GetData(DATA_WAVE_COUNT) == 12)
                        _instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
                }

                if (Creature* pGuard1 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_1) : ObjectGuid::Empty))
                {
                    if (!pGuard1->IsAlive())
                        pGuard1->Respawn();
                }
                if (Creature* pGuard2 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_2) : ObjectGuid::Empty))
                {
                    if (!pGuard2->IsAlive())
                        pGuard2->Respawn();
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

                    if (Creature* pGuard1 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_1) : ObjectGuid::Empty))
                    {
                        pGuard1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        if (!pGuard1->GetVictim() && pGuard1->AI())
                            pGuard1->AI()->AttackStart(who);
                    }
                    if (Creature* pGuard2 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_2) : ObjectGuid::Empty))
                    {
                        pGuard2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                        if (!pGuard2->GetVictim() && pGuard2->AI())
                            pGuard2->AI()->AttackStart(who);
                    }
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(SAY_AGGRO);
                DoCast(me, SPELL_EARTH_SHIELD);

                if (_instance)
                {
                    if (GameObject* pDoor = _instance->instance->GetGameObject(_instance->GetGuidData(DATA_EREKEM_CELL)))
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
                    .Schedule(Seconds(20), [this](TaskContext context)
                    {
                        if (Unit* ally = DoSelectLowestHpFriendly(30.0f))
                            DoCast(ally, DUNGEON_MODE(SPELL_EARTH_SHIELD, SPELL_EARTH_SHIELD_H));
                        context.Repeat(Seconds(20));
                    })
                    .Schedule(Seconds(2), [this](TaskContext context)
                    {
                        DoCast(me, SPELL_BLOODLUST);
                        context.Repeat(Seconds(35), Seconds(45));
                    })
                    .Schedule(Seconds(2), [this](TaskContext context)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
                            DoCast(target, SPELL_LIGHTNING_BOLT);
                        context.Repeat(Milliseconds(2500));
                    })
                    .Schedule(Seconds(10), [this](TaskContext context)
                    {
                        if (Unit* ally = DoSelectLowestHpFriendly(40.0f))
                            DoCast(ally, DUNGEON_MODE(SPELL_CHAIN_HEAL, SPELL_CHAIN_HEAL_H));

                        Creature* pGuard1 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_1) : ObjectGuid::Empty);
                        Creature* pGuard2 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_2) : ObjectGuid::Empty);
                        bool bothDead = !CheckGuardAlive();
                        if (bothDead)
                            context.Repeat(Milliseconds(3000));
                        else
                            context.Repeat(Seconds(8), Seconds(11));
                    })
                    .Schedule(Milliseconds(2000), Milliseconds(8000), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), SPELL_EARTH_SHOCK);
                        context.Repeat(Milliseconds(8000), Milliseconds(13000));
                    })
                    .Schedule(Milliseconds(500), [this](TaskContext context)
                    {
                        bool ccDetected = false;
                        Creature* pGuard1 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_1) : ObjectGuid::Empty);
                        Creature* pGuard2 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_2) : ObjectGuid::Empty);
                        if (pGuard1 && pGuard1->IsAlive() && CheckGuardAuras(pGuard1))
                            ccDetected = true;
                        if (pGuard2 && pGuard2->IsAlive() && CheckGuardAuras(pGuard2))
                            ccDetected = true;

                        if (ccDetected)
                        {
                            DoCastAOE(SPELL_BREAK_BONDS);
                            context.Repeat(Seconds(10));
                        }
                        else
                            context.Repeat(Milliseconds(500));
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

                if (_phase == 0 && !CheckGuardAlive())
                {
                    _phase = 1;
                    me->SetCanDualWield(true);
                    DoCast(me, SPELL_WINDFURY);
                }

                _scheduler.Update(diff, [this]
                {
                    if (_phase == 1)
                        DoSpellAttackIfReady(SPELL_STORMSTRIKE);
                    else
                        DoMeleeAttackIfReady();
                });
            }

            void JustDied(Unit* /*killer*/) override
            {
                _scheduler.CancelAll();
                Talk(SAY_DEATH);

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

                        _instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
                        _instance->SetData(DATA_WAVE_COUNT, 13);
                    }
                }
            }

            void KilledUnit(Unit* victim) override
            {
                if (victim->GetTypeId() != TYPEID_PLAYER)
                    return;
                Talk(SAY_SLAY);
            }

            void DoAction(int32 eventType) override
            {
                if (eventType == DATA_EREKEM_GUARD_1)
                {
                    ++_addsKilled;
                    if (_addsKilled == 1)
                        Talk(SAY_ADD_KILLED);
                    else if (_addsKilled == 2)
                        Talk(SAY_BOTH_ADDS_KILLED);
                }
            }

            bool CheckGuardAlive() const
            {
                if (Creature* pGuard1 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_1) : ObjectGuid::Empty))
                    if (pGuard1->IsAlive())
                        return true;
                if (Creature* pGuard2 = Unit::GetCreature(*me, _instance ? _instance->GetGuidData(DATA_EREKEM_GUARD_2) : ObjectGuid::Empty))
                    if (pGuard2->IsAlive())
                        return true;
                return false;
            }

            bool CheckGuardAuras(Creature* guard) const
            {
                if (guard->HasAuraWithMechanic(EREKEM_CC_MECHANIC_MASK))
                    return true;

                static AuraType const AuraImmunityList[] =
                {
                    SPELL_AURA_MOD_STUN,
                    SPELL_AURA_MOD_DECREASE_SPEED,
                    SPELL_AURA_MOD_ROOT,
                    SPELL_AURA_MOD_CONFUSE,
                    SPELL_AURA_MOD_FEAR
                };

                for (AuraType type : AuraImmunityList)
                    if (guard->HasAuraType(type))
                        return true;

                return false;
            }

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
            uint8 _phase;
            uint8 _addsKilled;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_erekemAI(creature);
        }
};

enum GuardSpells
{
    SPELL_GUSHING_WOUND                   = 39215,
    SPELL_HOWLING_SCREECH                 = 54462,
    SPELL_STRIKE                          = 14516
};

class npc_erekem_guard : public CreatureScript
{
    public:
        npc_erekem_guard() : CreatureScript("npc_erekem_guard") { }

        struct npc_erekem_guardAI : public ScriptedAI
        {
            npc_erekem_guardAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
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
                _scheduler
                    .Schedule(Milliseconds(4000), Milliseconds(8000), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), SPELL_STRIKE);
                        context.Repeat(Milliseconds(4000), Milliseconds(8000));
                    })
                    .Schedule(Milliseconds(8000), Milliseconds(13000), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), SPELL_HOWLING_SCREECH);
                        context.Repeat(Milliseconds(8000), Milliseconds(13000));
                    })
                    .Schedule(Milliseconds(1000), Milliseconds(3000), [this](TaskContext context)
                    {
                        DoCast(me->GetVictim(), SPELL_GUSHING_WOUND);
                        context.Repeat(Milliseconds(7000), Milliseconds(12000));
                    });
            }

            void JustDied(Unit* /*killer*/) override
            {
                _scheduler.CancelAll();

                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    if (Creature* boss = Unit::GetCreature(*me, instance->GetGuidData(DATA_EREKEM)))
                        if (boss->AI())
                            boss->AI()->DoAction(DATA_EREKEM_GUARD_1);
                }
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

        private:
            InstanceScript* _instance;
            TaskScheduler _scheduler;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_erekem_guardAI(creature);
        }
};

void AddSC_boss_erekem()
{
    new boss_erekem();
    new npc_erekem_guard();
}
