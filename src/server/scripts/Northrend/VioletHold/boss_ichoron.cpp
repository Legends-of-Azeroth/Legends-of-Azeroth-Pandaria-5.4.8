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
    SPELL_DRAINED                               = 59820,
    SPELL_FRENZY                                = 54312,
    SPELL_FRENZY_H                              = 59522,
    SPELL_PROTECTIVE_BUBBLE                     = 54306,
    SPELL_WATER_BLAST                           = 54237,
    SPELL_WATER_BLAST_H                         = 59520,
    SPELL_WATER_BOLT_VOLLEY                     = 54241,
    SPELL_WATER_BOLT_VOLLEY_H                   = 59521,
    SPELL_SPLASH                                = 59516,
    SPELL_BURST                                 = 54379,
    SPELL_WATER_GLOBULE_1                       = 54258,
    SPELL_WATER_GLOBULE_VISUAL                  = 54260,
    SPELL_WATER_GLOBULE_2                       = 54264,
    SPELL_WATER_GLOBULE_3                       = 54265,
    SPELL_WATER_GLOBULE_4                       = 54266,
    SPELL_WATER_GLOBULE_5                       = 54267,
    SPELL_WATER_GLOBULE_AURA                    = 54268,
    SPELL_MERGE                                 = 54269,
};

enum IchoronCreatures
{
    NPC_ICHOR_GLOBULE                           = 29321,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_SLAY                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_ENRAGE                                  = 4,
    SAY_SHATTER                                 = 5,
    SAY_BUBBLE                                  = 6,
    EMOTE_SHATTER                               = 7,
};

enum Actions
{
    ACTION_WATER_ELEMENT_KILLED                 = 1,
};

class boss_ichoron : public CreatureScript
{
public:
    boss_ichoron() : CreatureScript("boss_ichoron") { }

    struct boss_ichoronAI : public ScriptedAI
    {
        boss_ichoronAI(Creature* creature) : ScriptedAI(creature), m_waterElements(creature)
        {
            instance  = creature->GetInstanceScript();
        }

        bool bIsExploded;
        bool bIsFrenzy;

        uint32 uiBubbleCheckerTimer;
        uint32 uiWaterBoltVolleyTimer;
        uint32 uiWaterBlastTimer;

        InstanceScript* instance;

        SummonList m_waterElements;

        void Reset() override
        {
            bIsExploded = false;
            bIsFrenzy = false;
            me->GetMap()->SetWorldState(WORLDSTATE_DEHYDRATION, 1);
            uiBubbleCheckerTimer = 1000;
            uiWaterBoltVolleyTimer = urand(10000, 15000);
            uiWaterBlastTimer = urand(10000, 15000);

            me->SetVisible(true);
            DespawnWaterElements();

            if (instance)
            {
                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
            }
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            Talk(SAY_AGGRO);

            DoCast(me, SPELL_PROTECTIVE_BUBBLE);

            if (instance)
            {
                if (GameObject* pDoor = instance->instance->GetGameObject(instance->GetGuidData(DATA_ICHORON_CELL)))
                    if (pDoor->GetGoState() == GO_STATE_READY)
                    {
                        EnterEvadeMode();
                        return;
                    }
                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
            }
        }

        void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
        {
            ScriptedAI::EnterEvadeMode();
            if (instance)
                instance->SetData(DATA_WIPE, 1);
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

        void SpellHit(Unit* /*who*/, SpellInfo const* spell) override
        {
            if (spell->Id == SPELL_MERGE)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                me->ModifyHealth(int32(me->CountPctFromMaxHealth(1)));

                if (bIsExploded)
                    DoExplodeCompleted();

                me->GetMap()->SetWorldState(WORLDSTATE_DEHYDRATION, 0);
            }
        }

        void DoAction(int32 action) override
        {
            if (!me->IsAlive())
                return;

            switch (action)
            {
                case ACTION_WATER_ELEMENT_KILLED:
                    uint32 damage = me->CountPctFromMaxHealth(3);
                    if (me->GetHealth() > damage)
                    {
                        me->ModifyHealth(-int32(damage));
                        me->LowerPlayerDamageReq(damage);
                    }
                    break;
            }
        }

        void DespawnWaterElements()
        {
            m_waterElements.DespawnAll();
        }

        // call when explode shall stop.
        // either when "hit" by a bubble, or when there is no bubble left.
        void DoExplodeCompleted()
        {
            bIsExploded = false;

            if (!HealthBelowPct(25))
            {
                Talk(SAY_BUBBLE);
                DoCast(me, SPELL_PROTECTIVE_BUBBLE, true);
            }

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            me->SetVisible(true);
            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                AttackStart(target);
        }

        void MoveInLineOfSight(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            if (bIsExploded || !UpdateVictim())
            {
                if (bIsExploded && !bIsFrenzy)
                {
                    if (uiBubbleCheckerTimer <= diff)
                    {
                        if (!m_waterElements.empty())
                            for (auto&& guid : m_waterElements)
                                if (Creature* pTemp = Unit::GetCreature(*me, guid))
                                    if (pTemp->IsAlive())
                                        return;

                        DoExplodeCompleted();
                    }
                    else
                        uiBubbleCheckerTimer -= diff;
                }
                return;
            }

            if (!bIsFrenzy && HealthBelowPct(25) && !bIsExploded)
            {
                Talk(SAY_ENRAGE);
                DoCast(me, DUNGEON_MODE(SPELL_FRENZY,SPELL_FRENZY_H), true);
                bIsFrenzy = true;
            }

            if (!bIsFrenzy)
            {
                if (uiBubbleCheckerTimer <= diff)
                {
                    if (!bIsExploded)
                    {
                        if (!me->HasAura(SPELL_PROTECTIVE_BUBBLE, 0))
                        {
                            Talk(SAY_SHATTER);
                            Talk(EMOTE_SHATTER);
                            DoCast(me, SPELL_DRAINED);
                            DoCast(me, SPELL_BURST, true);
                            bIsExploded = true;
                            me->AttackStop();
                            for (uint8 i = 0; i < 2; i++)
                            {
                                DoCast(me, SPELL_WATER_GLOBULE_1, true);
                                DoCast(me, SPELL_WATER_GLOBULE_2, true);
                                DoCast(me, SPELL_WATER_GLOBULE_3, true);
                                DoCast(me, SPELL_WATER_GLOBULE_4, true);
                                DoCast(me, SPELL_WATER_GLOBULE_5, true);
                            }
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                            me->m_Events.Schedule(500, [this]()
                            {
                                if (me->IsAlive())
                                    me->SetVisible(false);
                            });
                        }
                    }
                    uiBubbleCheckerTimer = 1000;
                }
                else uiBubbleCheckerTimer -= diff;
            }

            if (!bIsExploded)
            {
                if (uiWaterBoltVolleyTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_WATER_BOLT_VOLLEY,SPELL_WATER_BOLT_VOLLEY_H));
                        uiWaterBoltVolleyTimer = urand(10000, 15000);
                    }
                }
                else uiWaterBoltVolleyTimer -= diff;

                if (uiWaterBlastTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me->GetVictim(), DUNGEON_MODE(SPELL_WATER_BLAST,SPELL_WATER_BLAST_H));
                        uiWaterBlastTimer = urand(10000, 15000);
                    }
                }
                else uiWaterBlastTimer -= diff;

                DoMeleeAttackIfReady();
            }
        }

        void JustDied(Unit* /*killer*/) override
        {
            Talk(SAY_DEATH);

            if (bIsExploded)
            {
                bIsExploded = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                me->SetVisible(true);
            }

            DespawnWaterElements();

            if (instance)
            {
                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                {
                    if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
                        me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                    instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
                    instance->SetData(DATA_WAVE_COUNT, 7);
                }
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                {
                    if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
                        me->RemoveFlag(OBJECT_FIELD_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                    instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
                    instance->SetData(DATA_WAVE_COUNT, 13);
                }
            }
        }

        void JustSummoned(Creature* summon) override
        {
            m_waterElements.Summon(summon);
        }

        void SummonedCreatureDespawn(Creature* summon) override
        {
            m_waterElements.Despawn(summon);
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
        {
            m_waterElements.Despawn(summon);
        }

        void KilledUnit(Unit* victim) override
        {
            if (victim->GetTypeId() != TYPEID_PLAYER)
                return;

            Talk(SAY_SLAY);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new boss_ichoronAI(creature);
    }
};

class npc_ichor_globule : public CreatureScript
{
    public:
        npc_ichor_globule() : CreatureScript("npc_ichor_globule") { }

        struct npc_ichor_globuleAI : public ScriptedAI
        {
            npc_ichor_globuleAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiRangeCheck_Timer;

            void Reset() override
            {
                uiRangeCheck_Timer = 1000;
            }

            void IsSummonedBy(Unit* summoner) override
            {
                summoner->CastSpell(me, SPELL_WATER_GLOBULE_VISUAL, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_WATER_GLOBULE_VISUAL)
                {
                    DoCast(me, SPELL_WATER_GLOBULE_AURA, true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                    if (Creature* ichoron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ICHORON)))
                        me->GetMotionMaster()->MoveFollow(ichoron, 0.01f, 0);
                }
            }

            void AttackStart(Unit* /*who*/) override
            {
                return;
            }

            void UpdateAI(uint32 diff) override
            {
                if (uiRangeCheck_Timer < diff)
                {
                    if (instance)
                    {
                        if (Creature* pIchoron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ICHORON)))
                        {
                            if (me->IsWithinDist(pIchoron, 2.0f , false))
                            {
                                if (pIchoron)
                                    pIchoron->SetVisible(true);
                                DoCast(me, SPELL_MERGE, true);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC | UNIT_FLAG_NOT_SELECTABLE);
                                me->DespawnOrUnsummon(1000);
                                uiRangeCheck_Timer = 10000;
                                return;
                            }
                        }
                    }
                    uiRangeCheck_Timer = 1000;
                }
                else uiRangeCheck_Timer -= diff;
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCast(me, SPELL_SPLASH);
                if (Creature* pIchoron = Unit::GetCreature(*me, instance->GetGuidData(DATA_ICHORON)))
                    if (pIchoron->AI())
                        pIchoron->AI()->DoAction(ACTION_WATER_ELEMENT_KILLED);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ichor_globuleAI(creature);
        }
};

void AddSC_boss_ichoron()
{
    new boss_ichoron();
    new npc_ichor_globule();
}
