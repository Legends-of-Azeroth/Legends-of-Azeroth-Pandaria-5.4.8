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
#include "mogu_shan_palace.h"

#define TYPE_GET_ENTOURAGE 14

const uint32 auiGekkanAdds[4] =
{
    CREATURE_GLINTROK_SKULKER,
    CREATURE_GLINTROK_HEXXER,
    CREATURE_GLINTROK_ORACLE,
    CREATURE_GLINTROK_IRONHIDE,
};

bool NotAtGekkan(Unit* owner)
{
    return owner->GetPositionZ() > -50.0f ? true : false;
}

class boss_gekkan : public CreatureScript
{
    public:
        boss_gekkan() : CreatureScript("boss_gekkan") { }

        enum eSpells
        {
            SPELL_RECKLESS_INSPIRATION     = 118988,
            SPELL_RECKLESS_INSPIRATION_2   = 129262
        };

        enum eActions
        {
            ACTION_ADD_DEATH,
            ACTION_COMBAT
        };

        enum eEvents
        {
            EVENT_RECKLESS_INSPIRATION     = 1,
            EVENT_ENCOUNTER_DONE           = 2,
        };

        enum eTalks
        {
            TALK_INTRO,
            TALK_KILLING,
            TALK_SPELL,
            TALK_AGGRO,
            TALK_DEATH,
        };

        struct boss_gekkan_AI : public BossAI
        {
            boss_gekkan_AI(Creature* creature) : BossAI(creature, DATA_GEKKAN) {}

            std::list<ObjectGuid> m_uilGekkanAdds;
            bool m_bYelled, m_FeignDeath;

            void InitializeAI() override
            {
                Reset();

                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL_TOWARDS_DEST, true);
                m_FeignDeath = false;
                m_bYelled = false;
            }

            void Reset() override
            {
                _Reset();
                events.Reset();
                m_FeignDeath = false;
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }

            void InitializeGekkan()
            {
                for (auto&& itr : auiGekkanAdds)
                    if (Creature* GekkanProtector = GetClosestCreatureWithEntry(me, itr, 50.0f, true))
                        m_uilGekkanAdds.push_back(GekkanProtector->GetGUID());
            }

            void HandleAddDeath()
            {
                Talk(TALK_SPELL);
                DoCast(SPELL_RECKLESS_INSPIRATION_2);
            }

            void JustEngagedWith(Unit* who) override
            {
                DoZoneInCombat();

                if (who)
                {
                    // Get the four adds.
                    for (auto&& guid : m_uilGekkanAdds)
                    {
                        auto const pAdd = ObjectAccessor::GetCreature(*me, guid);
                        if (pAdd && pAdd->AI())
                            pAdd->Attack(who, false);
                    }

                    // Special for orakle cuz he won`t join to combat wtf
                    if (Creature* Orakle = GetClosestCreatureWithEntry(me, CREATURE_GLINTROK_ORACLE, 50.0f, true))
                        Orakle->Attack(who, false);
                }

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

                Talk(TALK_AGGRO);

                events.ScheduleEvent(EVENT_RECKLESS_INSPIRATION, urand(10000, 16000));
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (who && who->GetTypeId() == TYPEID_PLAYER && !m_bYelled)
                {
                    Talk(TALK_INTRO);
                    m_bYelled = true;
                    InitializeGekkan();
                }

                CreatureAI::MoveInLineOfSight(who);
            }

            void JustReachedHome() override
            {
                // Respawn all adds on evade
                for (auto&& guid : m_uilGekkanAdds)
                {
                    if (Creature* pAdd = ObjectAccessor::GetCreature(*me, guid))
                    {
                        if (!pAdd->IsAlive())
                            pAdd->Respawn();

                        pAdd->RemoveAllAuras();
                    }
                }

                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
            }

            Creature* GetRandomAliveAdd()
            {
                decltype(m_uilGekkanAdds) uiTempGuidList;

                for (auto&& guid : m_uilGekkanAdds)
                {
                    auto const pAdd = ObjectAccessor::GetCreature(*me, guid);
                    if (pAdd && pAdd->IsAlive())
                        uiTempGuidList.push_back(guid);
                }

                if (uiTempGuidList.empty())
                    return nullptr;

                auto itr = uiTempGuidList.cbegin();
                std::advance(itr, urand(0, uiTempGuidList.size() - 1));

                return ObjectAccessor::GetCreature(*me, *itr);
            }

            bool HasAnyProtectorAlive()
            {
                if (!m_bYelled)
                    return true;

                if (m_uilGekkanAdds.empty())
                    return false;

                for (auto&& itr : m_uilGekkanAdds)
                    if (Creature* prt = ObjectAccessor::GetCreature(*me, itr))
                        if (prt->IsAlive())
                            return true;

                return false;
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                if (auto const script = me->GetInstanceScript())
                    script->SetData(TYPE_GEKKAN, DONE);
            }

            void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
            {
                BossAI::EnterEvadeMode();
                if (instance)
                {
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    instance->SetData(DATA_GEKKAN, FAIL);
                }
                summons.DespawnAll();

                for (auto&& guid : m_uilGekkanAdds)
                {
                    auto const pAdd = ObjectAccessor::GetCreature(*me, guid);
                    if (pAdd && pAdd->AI())
                        pAdd->AI()->EnterEvadeMode();
                }
            }

            void KilledUnit(Unit* /*victim*/) override
            {
                Talk(TALK_KILLING);
            }

            void DoAction(int32 actionId) override
            {
                switch (actionId)
                {
                    case ACTION_ADD_DEATH:
                        if (m_bYelled && !m_FeignDeath) // use this spell and talk only if alive
                            HandleAddDeath();

                        if (!HasAnyProtectorAlive() && m_FeignDeath)
                            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        break;
                    case ACTION_COMBAT:
                        DoZoneInCombat();
                        break;
                }
            }

            void DamageTaken(Unit* /*killer*/, uint32 &damage) override
            {
                if (m_FeignDeath && HasAnyProtectorAlive())
                    damage = 0;

                if (!m_FeignDeath && me->GetHealth() <= damage && HasAnyProtectorAlive())
                {
                    damage = 0;
                    me->RemoveAllAuras();
                    me->ClearAllReactives();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                    me->SetReactState(REACT_PASSIVE);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->AttackStop();
                    m_FeignDeath = true;
                    events.Reset();

                    // If we die, all remaining adds should get buff
                    for (auto&& guid : m_uilGekkanAdds)
                    {
                        auto const pAdd = ObjectAccessor::GetCreature(*me, guid);
                        if (pAdd && pAdd->IsAlive())
                            pAdd->CastSpell(pAdd, SPELL_RECKLESS_INSPIRATION_2, false);
                    }

                    Talk(TALK_DEATH);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (Unit* m_taunt = me->GetVictim())
                    if (VictimOnBalcony(m_taunt->GetGUID()))
                        me->AI()->EnterEvadeMode();

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_RECKLESS_INSPIRATION)
                    {
                        Talk(TALK_SPELL);

                        if (Creature* pAdd = GetRandomAliveAdd())
                            DoCast(pAdd, SPELL_RECKLESS_INSPIRATION, false);

                        events.ScheduleEvent(EVENT_RECKLESS_INSPIRATION, urand(15000, 21000));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }

            private:
                bool VictimOnBalcony(ObjectGuid vict) // If someone try to taunt us at balcony, not at our battle area
                {
                    if (Unit* victim = ObjectAccessor::GetUnit(*me, vict))
                        return victim->GetPositionZ() > -50.0f ? true : false;

                    return true;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_gekkan_AI(creature);
        }
};

class npc_glintrok_skulker : public CreatureScript
{
    public:
        npc_glintrok_skulker() : CreatureScript("npc_glintrok_skulker") { }

        enum eSpells
        {
            SPELL_SHANK        = 118963,
            SPELL_STEALTH      = 118969
        };

        struct npc_glintrok_skulker_AI : public ScriptedAI
        {
            npc_glintrok_skulker_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                if (!me->HasAura(SPELL_STEALTH))
                    me->CastSpell(me, SPELL_STEALTH, false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(1, 2000);

                if (NotAtGekkan(me))
                    return;

                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(1);
                }
            }

            void Reset() override
            {
                DoCast(SPELL_STEALTH);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (NotAtGekkan(me))
                    return;

                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(0);
                }
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
                    if (eventId == 1)
                    {
                        me->CastSpell(me->GetVictim(), SPELL_SHANK, false);
                        events.ScheduleEvent(1, 7000);
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_glintrok_skulker_AI(creature);
        }
};

class npc_glintrok_ironhide : public CreatureScript
{
    public:
        npc_glintrok_ironhide() : CreatureScript("npc_glintrok_ironhide") { }

        enum eSpells
        {
            SPELL_IRON_PROTECTOR        = 118958,
            SPELL_IRON_DEFENCE          = 118960,
        };

        struct npc_glintrok_ironhide_AI : public ScriptedAI
        {
            npc_glintrok_ironhide_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                DoCast(me, SPELL_IRON_PROTECTOR);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->ClearUnitState(UNIT_STATE_CASTING);
                events.ScheduleEvent(1, 2000);

                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(1);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(0);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_glintrok_ironhide_AI(creature);
        }
};

class npc_glintrok_oracle : public CreatureScript
{
    public:
        npc_glintrok_oracle() : CreatureScript("npc_glintrok_oracle") { }

        enum eSpells
        {
            SPELL_CLEANSING_FLAME        = 118940,
            SPELL_FIRE_BOLT              = 118936,
        };

        struct npc_glintrok_oracle_AI : public ScriptedAI
        {
            npc_glintrok_oracle_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                if (!NotAtGekkan(me))
                    SetCombatMovement(false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(1, 16000);
                events.ScheduleEvent(2, 4000);

                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(1);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(0);
                }
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
                        case 1:
                            me->CastSpell(me, SPELL_CLEANSING_FLAME, false);
                            events.ScheduleEvent(1, 25000);
                            break;
                        case 2:
                            me->CastSpell(me->GetVictim(), SPELL_FIRE_BOLT, false);
                            events.ScheduleEvent(2, 7000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_glintrok_oracle_AI(creature);
        }
};

class npc_glintrok_hexxer : public CreatureScript
{
    public:
        npc_glintrok_hexxer() : CreatureScript("npc_glintrok_hexxer") { }

        enum eSpells
        {
            SPELL_HEX_OF_LETHARGY        = 118903,
            SPELL_DARK_BOLT              = 118917,
        };

        struct npc_glintrok_hexxer_AI : public ScriptedAI
        {
            npc_glintrok_hexxer_AI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                if (!NotAtGekkan(me))
                    SetCombatMovement(false);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(1, 10000);
                events.ScheduleEvent(2, 4000);

                if (NotAtGekkan(me))
                    return;

                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(1);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (NotAtGekkan(me))
                    return;

                if (Creature* pGekkan = GetClosestCreatureWithEntry(me, CREATURE_GEKKAN, 100.0f))
                {
                    if (pGekkan->AI())
                        pGekkan->AI()->DoAction(0);
                }
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
                        case 1:
                            me->CastSpell(me->GetVictim(), SPELL_HEX_OF_LETHARGY, false);
                            events.ScheduleEvent(1, 20000);
                            break;
                        case 2:
                            me->CastSpell(me->GetVictim(), SPELL_DARK_BOLT, false);
                            events.ScheduleEvent(2, 5000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_glintrok_hexxer_AI(creature);
        }
};

void AddSC_boss_gekkan()
{
    new boss_gekkan();
    new npc_glintrok_hexxer();
    new npc_glintrok_skulker();
    new npc_glintrok_oracle();
    new npc_glintrok_ironhide();
}
