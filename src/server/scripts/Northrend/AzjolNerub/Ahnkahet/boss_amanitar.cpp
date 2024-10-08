/*
* This file is part of the Legends of Azeroth Pandaria Project. See THANKS file for Copyright information
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
#include "ahnkahet.h"

enum Spells
{
    SPELL_BASH                                    = 57094, // Victim
    SPELL_ENTANGLING_ROOTS                        = 57095, // Random Victim 100Y
    SPELL_MINI                                    = 57055, // Self
    SPELL_VENOM_BOLT_VOLLEY                       = 57088, // Random Victim 100Y
    SPELL_HEALTHY_MUSHROOM_POTENT_FUNGUS          = 56648, // Killer 3Y
    SPELL_POISONOUS_MUSHROOM_POISON_CLOUD         = 57061, // Self - Duration 8 Sec
    SPELL_POISONOUS_MUSHROOM_VISUAL_AREA          = 61566, // Self
    SPELL_POISONOUS_MUSHROOM_VISUAL_AURA          = 56741, // Self
    SPELL_PUTRID_MUSHROOM                         = 31690, // To make the mushrooms visible
    SPELL_POWER_MUSHROOM_VISUAL_AURA              = 56740,
};

enum Npcs
{
    NPC_HEALTHY_MUSHROOM                          = 30391,
    NPC_POISONOUS_MUSHROOM                        = 30435,
    NPC_TRIGGER                                   = 19656
};

enum Events
{
    EVENT_SPAWN                                   = 1,
    EVENT_MINI,
    EVENT_ROOT,
    EVENT_BASH,
    EVENT_BOLT,
    EVENT_AURA
};

class boss_amanitar : public CreatureScript
{
    public:
        boss_amanitar() : CreatureScript("boss_amanitar") { }

        struct boss_amanitarAI : public BossAI
        {
            boss_amanitarAI(Creature* creature) : BossAI(creature, DATA_AMANITAR)
            {
               me->ApplySpellImmune(0, IMMUNITY_ID, 16857, true);
               me->ApplySpellImmune(0, IMMUNITY_ID, 770, true);
             }

            void Reset() override
            {
                _Reset();

                me->SetMeleeDamageSchool(SPELL_SCHOOL_NATURE);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
                //summons.DespawnAll();

                if (instance)
                {
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MINI);
                    instance->SetData(DATA_AMANITAR_EVENT, NOT_STARTED);
                }
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (instance)
                {
                    _JustDied();
                    instance->SetData(DATA_AMANITAR_EVENT, DONE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MINI);
                    //summons.DespawnAll();
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _JustEngagedWith();

                events.ScheduleEvent(EVENT_ROOT, urand(5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BASH, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BOLT, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MINI, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SPAWN, 5 * IN_MILLISECONDS);

                me->SetInCombatWithZone();
                if (instance)
                    instance->SetData(DATA_AMANITAR_EVENT, IN_PROGRESS);
            }

            void SpawnAdds()
            {
                uint8 u = 0;

                for (uint8 i = 0; i < 30; ++i)
                {
                    Position pos = me->GetPosition();
                    me->GetRandomNearPosition(30.0f);
                    pos.m_positionZ = me->GetMap()->GetHeight(pos.GetPositionX(), pos.GetPositionY(), MAX_HEIGHT) + 2.0f;

                    if (Creature* trigger = me->SummonCreature(NPC_TRIGGER, pos))
                    {
                        Creature* temp1 = trigger->FindNearestCreature(NPC_HEALTHY_MUSHROOM, 4.0f, true);
                        Creature* temp2 = trigger->FindNearestCreature(NPC_POISONOUS_MUSHROOM, 4.0f, true);
                        if (temp1 || temp2)
                            trigger->DisappearAndDie();
                        else
                        {
                            u = 1 - u;
                            trigger->DisappearAndDie();
                            me->SummonCreature(u > 0 ? NPC_POISONOUS_MUSHROOM : NPC_HEALTHY_MUSHROOM, pos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30 * IN_MILLISECONDS);
                        }
                    }
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
                        case EVENT_SPAWN:
                            SpawnAdds();
                            events.ScheduleEvent(EVENT_SPAWN, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_MINI:
                            DoCast(SPELL_MINI);
                            events.ScheduleEvent(EVENT_MINI, urand(25 * IN_MILLISECONDS, 30 * IN_MILLISECONDS));
                            break;
                        case EVENT_ROOT:
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_ENTANGLING_ROOTS, true);
                            events.ScheduleEvent(EVENT_ROOT, urand(10 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case EVENT_BASH:
                            DoCastVictim(SPELL_BASH);
                            events.ScheduleEvent(EVENT_BASH, urand(7 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                        case EVENT_BOLT:
                            DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_VENOM_BOLT_VOLLEY, true);
                            events.ScheduleEvent(EVENT_BOLT, urand(18 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_amanitarAI(creature);
        }
};

class npc_amanitar_mushrooms : public CreatureScript
{
    public:
        npc_amanitar_mushrooms() : CreatureScript("npc_amanitar_mushrooms") { }

        struct npc_amanitar_mushroomsAI : public ScriptedAI
        {
            npc_amanitar_mushroomsAI(Creature* creature) : ScriptedAI(creature)
            {
                SetCombatMovement(false);
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                events.ScheduleEvent(EVENT_AURA, 1 * IN_MILLISECONDS);

                me->SetDisplayFromModel(1);
                DoCast(SPELL_PUTRID_MUSHROOM);

                if (me->GetEntry() == NPC_POISONOUS_MUSHROOM)
                    DoCast(SPELL_POISONOUS_MUSHROOM_VISUAL_AURA);
                else
                    DoCast(SPELL_POWER_MUSHROOM_VISUAL_AURA);
            }

            void DamageTaken(Unit* attacker, uint32& damage) override
            {
                if (damage >= me->GetHealth() && me->GetEntry() == NPC_HEALTHY_MUSHROOM)
                {
                    me->InterruptNonMeleeSpells(false);
                    if (attacker->HasAura(SPELL_MINI))
                        attacker->RemoveAurasDueToSpell(SPELL_MINI);
                    DoCast(me, SPELL_HEALTHY_MUSHROOM_POTENT_FUNGUS, true);
                }

                if (damage >= me->GetHealth() && me->GetEntry() ==  NPC_POISONOUS_MUSHROOM)
                    DoCast(me, SPELL_POISONOUS_MUSHROOM_POISON_CLOUD, true);
            }

            void JustEngagedWith(Unit* /*who*/) override { }

            void AttackStart(Unit* /*who*/) override { }

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
                        case EVENT_AURA:
                            if (me->GetEntry() == NPC_POISONOUS_MUSHROOM)
                            {
                                DoCast(me, SPELL_POISONOUS_MUSHROOM_VISUAL_AREA, true);
                                //DoCastAOE(SPELL_POISONOUS_MUSHROOM_POISON_CLOUD, true);
                            }
                            events.ScheduleEvent(EVENT_AURA, 7 * IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_amanitar_mushroomsAI(creature);
        }
};

void AddSC_boss_amanitar()
{
    new boss_amanitar();
    new npc_amanitar_mushrooms();
}
