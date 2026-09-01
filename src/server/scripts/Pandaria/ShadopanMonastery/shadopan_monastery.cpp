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
#include "shadopan_monastery.h"

enum Spells
{
    SPELL_FIRE_ARROW_EFF = 131241,
};

class npc_shadopan_ambusher : public CreatureScript
{
    public:
        npc_shadopan_ambusher() : CreatureScript("npc_shadopan_ambusher") { }

        enum eSpells
        {
            SPELL_SHA_CORRUPTION   = 124337,
            SPELL_STEALTH_COSMETIC = 91194,
            SPELL_CRISE            = 128245,
            SPELL_FLIP_OUT_AURA    = 128248,
            SPELL_STEALTH          = 102921,
            SPELL_SHADOWSTEP       = 128766 
        };

        enum eEvents
        {
            EVENT_CRISE      = 1,
            EVENT_SHADOWSTEP = 2
        };

        struct npc_shadopan_ambusherAI : public ScriptedAI
        {
            npc_shadopan_ambusherAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                me->CastSpell(me, SPELL_STEALTH, false);
                me->CastSpell(me, SPELL_STEALTH_COSMETIC, false);
                me->CastSpell(me, SPELL_SHA_CORRUPTION, false);
                me->RemoveAurasDueToSpell(SPELL_FLIP_OUT_AURA);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveAura(SPELL_STEALTH);
                events.ScheduleEvent(EVENT_CRISE, urand(6 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOWSTEP, 0.5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CRISE:
                            me->CastSpell(me, SPELL_CRISE, false);
                            events.ScheduleEvent(EVENT_CRISE, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                        case EVENT_SHADOWSTEP:
                            if (auto const target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                                me->CastSpell(target, SPELL_SHADOWSTEP, true);
                            break;
                    }
                }

               DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_ambusherAI(creature);
        }
};

const float spawnAngles[5] = {M_PI, -M_PI / 2, M_PI / 2, -3 * M_PI / 4, 3 * M_PI / 4};

class npc_shadopan_warden: public CreatureScript
{
    public:
        npc_shadopan_warden() : CreatureScript("npc_shadopan_warden") { }

        enum eSpells
        {
            SPELL_INCAPACITATING_LEAP = 115517
        };

        enum eEvents
        {
            EVENT_INCAPACITATING_LEAP = 1
        };

        enum eTalks
        {
            TALK_AGGRO = 0
        };

        struct npc_shadopan_wardenAI : public ScriptedAI
        {
            npc_shadopan_wardenAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool aggroTalked;
            uint8 aggroTalk;

            void InitializeAI() override
            {
                if (me->IsAlive())
                {
                    for (int i = 0; i < 5; ++i)
                    {
                        float x, y, z;
                        z = me->GetPositionZ();
                        me->GetNearPoint2D(x, y, 3.0f, spawnAngles[i]);
                        me->SummonCreature(NPC_SHADO_PAN_AMBUSHER, x, y, z, me->GetAngle(x, y), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 604800ms);
                    }
                }

                aggroTalked = false;

                Reset();
            }

            void Reset() override
            {
                events.Reset();
            }
     
            void JustEngagedWith(Unit* /*who*/) override
            {
                if (!aggroTalked)
                {
                    aggroTalked = true;
                    Talk(TALK_AGGRO);
                }

                events.ScheduleEvent(EVENT_INCAPACITATING_LEAP, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_INCAPACITATING_LEAP)
                {
                    if (auto const target = SelectTarget(SELECT_TARGET_RANDOM))
                        me->CastSpell(target, SPELL_INCAPACITATING_LEAP, false);
                    events.ScheduleEvent(EVENT_INCAPACITATING_LEAP, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_wardenAI(creature);
        }
};

class npc_shadopan_stormbringer : public CreatureScript
{
    public:
        npc_shadopan_stormbringer() : CreatureScript("npc_shadopan_stormbringer") { }

        enum eSpells
        {
            SPELL_FOCUSING_ENERGY = 115009,
            SPELL_FOCUSING_COMBAT = 115433,
            SPELL_ARC_LIGHTNING   = 115026 
        };

        enum eEvents
        {
            EVENT_FOCUSING_COMBAT = 1
        };

        struct npc_shadopan_stormbringerAI : public ScriptedAI
        {
            npc_shadopan_stormbringerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                SetCombatMovement(false);

                Reset();
            }

            void Reset() override
            {
                events.Reset();
                if (auto const energy = me->FindNearestCreature(NPC_UNSTABLE_ENERGY, 10.0f))
                    if (energy->HasAura(SPELL_FOCUSING_COMBAT))
                        energy->RemoveAura(SPELL_FOCUSING_COMBAT);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FOCUSING_COMBAT, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }

            void JustDied(Unit* killer) override
            {
                if (auto const energy = me->FindNearestCreature(NPC_UNSTABLE_ENERGY, 10.0f))
                    if (energy->IsAIEnabled)
                        energy->AI()->DoAction(0); // ACTION_STORMBRINGER_KILLED
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_FOCUSING_COMBAT)
                {
                    if (auto const sTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
                        target->CastSpell(sTarget, SPELL_ARC_LIGHTNING, false);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!me->HasUnitState(UNIT_STATE_CASTING))
                {
                    if (auto const energy = me->FindNearestCreature(NPC_UNSTABLE_ENERGY, 10.0f))
                        me->CastSpell(energy, SPELL_FOCUSING_ENERGY, false);
                }        

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_FOCUSING_COMBAT)
                {
                    if (auto const energy = me->FindNearestCreature(NPC_UNSTABLE_ENERGY, 10.0f))
                        me->CastSpell(energy, SPELL_FOCUSING_COMBAT, true);
                    events.ScheduleEvent(EVENT_FOCUSING_COMBAT, urand(6 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                }

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_stormbringerAI(creature);
        }
};

class npc_shadopan_disciple : public CreatureScript
{
    public:
        npc_shadopan_disciple() : CreatureScript("npc_shadopan_disciple") { }

        enum eSpells
        {
            SPELL_PALM_STRIKE       = 106421,
            SPELL_ROUNDHOUSE_STRIKE = 128304
        };

        enum eEvents
        {
            EVENT_PALM_STRIKE       = 1,
            EVENT_ROUNDHOUSE_STRIKE = 2
        };

        struct npc_shadopan_discipleAI : public ScriptedAI
        {
            npc_shadopan_discipleAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_PALM_STRIKE, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ROUNDHOUSE_STRIKE, urand(6 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PALM_STRIKE:
                            me->CastSpell((Unit*)NULL, SPELL_PALM_STRIKE, false);
                            events.ScheduleEvent(EVENT_PALM_STRIKE, urand(16 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                            break;
                        case EVENT_ROUNDHOUSE_STRIKE:
                            me->CastSpell((Unit*)NULL, SPELL_ROUNDHOUSE_STRIKE, false);
                            events.ScheduleEvent(EVENT_ROUNDHOUSE_STRIKE, urand(13 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_discipleAI(creature);
        }
};

class npc_shadopan_unstable_energy : public CreatureScript
{
    public:
        npc_shadopan_unstable_energy() : CreatureScript("npc_shadopan_unstable_energy") { }

        enum eActions
        {
            ACTION_STORMBRINGER_KILLED = 0
        };

        struct npc_shadopan_unstable_energyAI : public ScriptedAI
        {
            npc_shadopan_unstable_energyAI(Creature* creature) : ScriptedAI(creature) { }

            uint8 stormbringerKilled;

            void InitializeAI() override
            {
                stormbringerKilled = 0;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_STORMBRINGER_KILLED)
                {
                    if (++stormbringerKilled == 6)
                        me->DespawnOrUnsummon(2 * IN_MILLISECONDS);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_unstable_energyAI(creature);
        }
};

class npc_ethereal_sha : public CreatureScript
{
    public:
        npc_ethereal_sha() : CreatureScript("npc_ethereal_sha") { }

        enum eSpells
        {
            SPELL_SHA_BLAST              = 110117,
            SPELL_REACH_THROUGH_THE_VOID = 128339
        };

        enum eEvents
        {
            EVENT_SHA_BLAST              = 1,
            EVENT_REACH_THROUGH_THE_VOID = 2
        };

        struct npc_ethereal_shaAI : public ScriptedAI
        {
            npc_ethereal_shaAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHA_BLAST, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REACH_THROUGH_THE_VOID, urand(11 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHA_BLAST:
                            me->CastSpell(me->GetVictim(), SPELL_SHA_BLAST, false);
                            events.ScheduleEvent(EVENT_SHA_BLAST, urand(12 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                        case EVENT_REACH_THROUGH_THE_VOID:
                            me->CastSpell(me, SPELL_REACH_THROUGH_THE_VOID, false);
                            events.ScheduleEvent(EVENT_REACH_THROUGH_THE_VOID, urand(18 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ethereal_shaAI(creature);
        }
};

class npc_spm_void_sha : public CreatureScript
{
    public:
        npc_spm_void_sha() : CreatureScript("npc_spm_void_sha") { }

        EventMap events;

        enum eSpells
        {
            SPELL_ON_SPAWN    = 128342,
            SPELL_VOID_ENERGY = 128343
        };

        enum eEvents
        {
            EVENT_VOID_ENERGY = 1
        };

        struct npc_spm_void_shAI : public ScriptedAI
        {
            npc_spm_void_shAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void InitializeAI() override
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->CastSpell(me, SPELL_ON_SPAWN, false);

                Reset();
            }

            void Reset() override
            {
                DoZoneInCombat();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_VOID_ENERGY, 5 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_VOID_ENERGY)
                {
                    me->CastSpell((Unit*)NULL, SPELL_VOID_ENERGY, false);
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spm_void_shAI(creature);
        }
};

class npc_consuming_sha : public CreatureScript
{
    public:
        npc_consuming_sha() : CreatureScript("npc_consuming_sha") { }

        enum eSpells
        {
            SPELL_CONSUMING_BITE = 106925,
            SPELL_CONSUMPTION    = 106929
        };

        enum eEvents
        {
            EVENT_CONSUMING_BITE = 1,
            EVENT_CONSUMPTION    = 2
        };

        struct npc_consuming_shaAI : public ScriptedAI
        {
            npc_consuming_shaAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CONSUMING_BITE, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CONSUMPTION, urand(6 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CONSUMING_BITE:
                            me->CastSpell((Unit*)NULL, SPELL_CONSUMING_BITE, false);
                            events.ScheduleEvent(EVENT_CONSUMING_BITE, urand(10 * IN_MILLISECONDS, 14 * IN_MILLISECONDS));
                            break;
                        case EVENT_CONSUMPTION:
                            me->CastSpell((Unit*)NULL, SPELL_CONSUMPTION, false);
                            events.ScheduleEvent(EVENT_CONSUMPTION, urand(15 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_consuming_shaAI(creature);
        }
    };

class npc_destroying_sha : public CreatureScript
{
    public:
        npc_destroying_sha() : CreatureScript("npc_destroying_sha") { }

        enum eSpells
        {
            SPELL_SHADOWS_OF_DESTRUCTION = 106942
        };

        enum eEvents
        {
            EVENT_SHADOWS_OF_DESTRUCTION = 1
        };

        struct npc_destroying_shaAI : public ScriptedAI
        {
            npc_destroying_shaAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHADOWS_OF_DESTRUCTION, urand(4 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetDBTableGUIDLow())
                    return;

                if (Creature* shaOfViolence = ObjectAccessor::GetCreature(*me, me->GetInstanceScript() ? me->GetInstanceScript()->GetGuidData(NPC_SHA_VIOLENCE) : ObjectGuid::Empty))
                    shaOfViolence->AI()->SetData(TYPE_OBVIOUS_SOLUTION, 1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SHADOWS_OF_DESTRUCTION)
                {
                    me->CastSpell((Unit*)NULL, SPELL_SHADOWS_OF_DESTRUCTION, false);
                    events.ScheduleEvent(EVENT_SHADOWS_OF_DESTRUCTION, urand(16 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_destroying_shaAI(creature);
        }
};

class npc_shadopan_novice : public CreatureScript
{
    public:
        npc_shadopan_novice() : CreatureScript("npc_shadopan_novice") { }

        struct npc_shadopan_noviceAI : public ScriptedAI
        {
            npc_shadopan_noviceAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                if (auto const training = me->FindNearestCreature(NPC_TRAINING_TARGET, 10.0f))
                    AttackStart(training);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_noviceAI(creature);
        }
};

class npc_shadopan_archery : public CreatureScript
{
    public:
        npc_shadopan_archery() : CreatureScript("npc_shadopan_archery") { }

        enum eEvents
        {
            EVENT_ICE_ARROW  = 1,
            EVENT_FIRE_ARROW = 2
        };

        enum eSpells
        {
            SPELL_ICE_ARROW = 126114,
            SPELL_FIRE_ARROW = 106992
        };

        struct npc_shadopan_archeryAI : public ScriptedAI
        {
            npc_shadopan_archeryAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            InstanceScript* instance;      

            void InitializeAI() override
            {
                instance = me->GetInstanceScript();
                me->setActive(true);
                switch (me->GetEntry())
                {
                    case NPC_ARCHERY_FIRST:
                        if (instance)
                            events.ScheduleEvent(EVENT_ICE_ARROW, 2 * IN_MILLISECONDS);
                        break;
                    case NPC_ARCHERY_SECOND:
                        if (instance)
                            events.ScheduleEvent(EVENT_FIRE_ARROW, 2 * IN_MILLISECONDS);
                        break;
                }
            }

            ObjectGuid ArcheryTarget()
            {
                std::list<Creature*> ArcheryTargets;
                GetCreatureListWithEntryInGrid(ArcheryTargets, me, NPC_ARCHERY_TARGET, 200.0f);

                if (ArcheryTargets.empty())
                    return ObjectGuid::Empty;

                return Trinity::Containers::SelectRandomContainerElement(ArcheryTargets)->GetGUID();
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ICE_ARROW:
                            me->UpdateObjectVisibility();

                            if (Creature* ArcherTarget = ObjectAccessor::GetCreature(*me, ArcheryTarget()))
                                me->CastSpell(ArcherTarget, SPELL_ICE_ARROW, false);

                            events.ScheduleEvent(EVENT_ICE_ARROW, urand(1 * IN_MILLISECONDS, 2 * IN_MILLISECONDS));
                            break;
                        case EVENT_FIRE_ARROW:
                            me->CastSpell(me, SPELL_FIRE_ARROW, false);
                            events.ScheduleEvent(EVENT_FIRE_ARROW, urand(14 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadopan_archeryAI(creature);
        }
};

class npc_residual_hatred : public CreatureScript
{
    public:
        npc_residual_hatred() : CreatureScript("npc_residual_hatred") { }

        enum eSpells
        {
            SPELL_CURSE_OF_AGONY    = 112999,
            SPELL_RING_OF_MALICE    = 112932,
            SPELL_SHADOW_BOLT       = 112998,
            SPELL_APPARATIONS       = 111698,
        };

        enum eEvents
        {
            EVENT_CURSE_OF_AGONY = 1,
            EVENT_RING_OF_MALICE = 2,
            EVENT_SHADOW_BOLT    = 3
        };

        struct npc_residual_hatredAI : public ScriptedAI
        {
            npc_residual_hatredAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RING_OF_MALICE, urand(6 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 1.3 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && me->HasAura(SPELL_APPARATIONS))
                    damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CURSE_OF_AGONY:
                            me->CastSpell((Unit*)NULL, SPELL_CURSE_OF_AGONY, false);
                            events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                        case EVENT_RING_OF_MALICE:
                            me->CastSpell(me, SPELL_RING_OF_MALICE, false);
                            events.ScheduleEvent(EVENT_RING_OF_MALICE, urand(21 * IN_MILLISECONDS, 26 * IN_MILLISECONDS));
                            break;
                        case EVENT_SHADOW_BOLT:
                            me->CastSpell((Unit*)NULL, SPELL_SHADOW_BOLT, false);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 1.3 * IN_MILLISECONDS);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_residual_hatredAI(creature);
        }
};

class npc_vistige_of_hatred : public CreatureScript
{
    public:
        npc_vistige_of_hatred() : CreatureScript("npc_vistige_of_hatred") { }

        enum eSpells
        {
            SPELL_BLACK_CLEAVE      = 113020,
            SPELL_DEATH_GRIP        = 113021,
            SPELL_TOUCH_OF_WEAKNESS = 113022,
            SPELL_APPARATIONS       = 111698,
        };

        enum eEvents
        {
            EVENT_BLACK_CLEAVE      = 1,
            EVENT_TOUCH_OF_WEAKNESS = 2,
            EVENT_DEATH_GRIP        = 3
        };

        struct npc_vistige_of_hatredAI : public ScriptedAI
        {
            npc_vistige_of_hatredAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_BLACK_CLEAVE, 15 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_TOUCH_OF_WEAKNESS, urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEATH_GRIP, urand(7 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && me->HasAura(SPELL_APPARATIONS))
                    damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLACK_CLEAVE:
                            me->CastSpell((Unit*)NULL, SPELL_BLACK_CLEAVE, false);
                            events.ScheduleEvent(EVENT_BLACK_CLEAVE, 15 * IN_MILLISECONDS);
                            break;
                        case EVENT_TOUCH_OF_WEAKNESS:
                            me->CastSpell((Unit*)NULL, SPELL_TOUCH_OF_WEAKNESS, false);
                            events.ScheduleEvent(EVENT_TOUCH_OF_WEAKNESS, urand(20 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                        case EVENT_DEATH_GRIP:
                            if (me->GetVictim() && !me->IsWithinMeleeRange(me->GetVictim()))
                                me->CastSpell(me->GetVictim(), SPELL_DEATH_GRIP, false);
                            events.ScheduleEvent(EVENT_DEATH_GRIP, urand(7 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_vistige_of_hatredAI(creature);
        }
};

class npc_fragment_of_hatred : public CreatureScript
{
    public:
        npc_fragment_of_hatred() : CreatureScript("npc_fragment_of_hatred") { }

        enum eSpells
        {
            SPELL_SINISTER_STRIKE  = 112931,
            SPELL_APPARATIONS      = 111698,
        };

        enum eEvents
        {
            EVENT_SINISTER_STRIKE = 1
        };

        struct npc_fragment_of_hatredAI : public ScriptedAI
        {
            npc_fragment_of_hatredAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SINISTER_STRIKE, urand(3 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && me->HasAura(SPELL_APPARATIONS))
                    damage = 0;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_SINISTER_STRIKE)
                {
                    me->CastSpell((Unit*)NULL, SPELL_SINISTER_STRIKE, false);
                    events.ScheduleEvent(EVENT_SINISTER_STRIKE, urand(3 * IN_MILLISECONDS, 10 * IN_MILLISECONDS));
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_fragment_of_hatredAI(creature);
        }
};

class npc_hateful_essence : public CreatureScript
{
    public:
        npc_hateful_essence() : CreatureScript("npc_hateful_essence") { }

        struct npc_hateful_essenceAI : public ScriptedAI
        {
            npc_hateful_essenceAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                me->SetReactState(REACT_PASSIVE);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hateful_essenceAI(creature);
        }
};

class spell_shadopan_explosion : public SpellScriptLoader
{
    public:
        spell_shadopan_explosion() : SpellScriptLoader("spell_shadopan_explosion") { }

        enum eSpells
        {
            SPELL_EXPLOSION_DAMAGE = 106966
        };

        class spell_shadopan_explosion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shadopan_explosion_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
                   if (auto const caster = GetCaster())
                       caster->CastSpell(caster, SPELL_EXPLOSION_DAMAGE, true);
            }

            void Register() override
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_shadopan_explosion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_shadopan_explosion_AuraScript();
        }
};

class ApparitionsTargetSelector
{
    public:
        bool operator()(WorldObject* object) const
        {
            if (object->ToCreature() && (object->GetEntry() == 58807 || object->GetEntry() == 58810 || object->GetEntry() == 58803))
                return false;

            return true;
        }
};

class spell_shadopan_apparitions : public SpellScriptLoader
{
    public:
        spell_shadopan_apparitions() : SpellScriptLoader("spell_shadopan_apparitions") { }

        class spell_shadopan_apparitions_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shadopan_apparitions_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (ApparitionsTargetSelector());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shadopan_apparitions_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        class spell_shadopan_apparitions_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shadopan_apparitions_AuraScript);

            void CalculateAmount(AuraEffect const* AuraEffect, float& amount, bool& /*canBeRecalculated*/)
            {
                if (auto const owner = GetOwner()->ToCreature())
                    amount = owner->GetMaxHealth();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_shadopan_apparitions_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_shadopan_apparitions_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_shadopan_apparitions_AuraScript();
        }
};

class spell_purification_ritual : public SpellScriptLoader
{
    public:
        spell_purification_ritual() : SpellScriptLoader("spell_purification_ritual") { }

        enum eSpells
        {
            SPELL_APPARITIONS = 111698
        };

        class spell_purification_ritual_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_purification_ritual_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto const owner = GetOwner()->ToCreature();

                if (owner)
                {
                    owner->RemoveAura(SPELL_APPARITIONS);
                    owner->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_purification_ritual_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        class spell_purification_ritual_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_purification_ritual_SpellScript);

            void HandleAfterCast()
            {
                if (Unit* owner = GetCaster())
                {
                    std::list<Creature*> Hatred;
                    GetCreatureListWithEntryInGrid(Hatred, owner, NPC_FRAGMENT_OF_HATRED, 50.0f);
                    GetCreatureListWithEntryInGrid(Hatred, owner, NPC_VESTIGE_OF_HATRED, 50.0f);
                    GetCreatureListWithEntryInGrid(Hatred, owner, NPC_RESIDUAL_OF_HATRED, 50.0f);

                    for (auto&& itr : Hatred)
                        if (itr->HasAura(SPELL_APPARITIONS))
                            itr->RemoveAura(SPELL_APPARITIONS);
                }
            }

            void Register() override
            {
                AfterCast += SpellCastFn(spell_purification_ritual_SpellScript::HandleAfterCast);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_purification_ritual_AuraScript();
        }

        SpellScript* GetSpellScript() const override
        {
            return new spell_purification_ritual_SpellScript();
        }
};

struct PlayerTargetSelector
{
    bool operator ()(WorldObject const* object) const
    {
        if (object->GetTypeId() == TYPEID_PLAYER)
            return false;
        return true;
    }
};

class spell_spm_fire_arrow : public SpellScriptLoader
{
    public:
        spell_spm_fire_arrow() : SpellScriptLoader("spell_spm_fire_arrow") { }

        class spell_spm_fire_arrow_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spm_fire_arrow_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (PlayerTargetSelector());
                if (!targets.empty() && targets.size() > 1)
                    targets.resize(1);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                auto const caster = GetCaster();
                auto const target = GetHitUnit();
                if (!caster || !target)
                    return;

                caster->UpdateObjectVisibility();
                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_spm_fire_arrow_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spm_fire_arrow_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_spm_fire_arrow_SpellScript();
        }
};

class spell_spm_flip_out : public SpellScriptLoader
{
    public:
        spell_spm_flip_out() : SpellScriptLoader("spell_spm_flip_out") { }

        class spell_spm_flip_out_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spm_flip_out_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if (PlayerTargetSelector());
                if (!targets.empty() && targets.size() > 1)
                    Trinity::Containers::SelectRandomContainerElement(targets);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                auto const caster = GetCaster();
                auto const target = GetHitUnit();
                if (!caster || !target)
                    return;

                caster->CastSpell(target, GetSpellInfo()->Effects[effIndex].BasePoints, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_spm_flip_out_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_spm_flip_out_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_spm_flip_out_SpellScript();
        }
};

class spell_spm_shadows_of_destruction : public SpellScriptLoader
{
    public:
        spell_spm_shadows_of_destruction() : SpellScriptLoader("spell_spm_shadows_of_destruction") { }

        class spell_spm_shadows_of_destruction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spm_shadows_of_destruction_SpellScript);

            void HandleBeforeCast()
            {
                auto const caster = GetCaster();
                if (!caster)
                    return;

                if (caster->ToCreature())
                    caster->ToCreature()->SetReactState(REACT_PASSIVE);
            }

            void Register() override
            {
                BeforeCast += SpellCastFn(spell_spm_shadows_of_destruction_SpellScript::HandleBeforeCast);
            }
        };

        class spell_spm_shadows_of_destruction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_spm_shadows_of_destruction_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                auto const owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                owner->SetReactState(REACT_AGGRESSIVE);
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_spm_shadows_of_destruction_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_MOD_PACIFY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_spm_shadows_of_destruction_SpellScript();
        }

        AuraScript* GetAuraScript() const override
        {
            return new spell_spm_shadows_of_destruction_AuraScript();
        }
};

class AreaTrigger_at_shadopan_archery : public AreaTriggerScript
{
    public:
        AreaTrigger_at_shadopan_archery() : AreaTriggerScript("AreaTrigger_at_shadopan_archery") { }

        enum areaTrigger
        {
            AREATRIGGER_ARCHERY_FIRST_BEGIN  = 8271, // ice archers begin
            AREATRIGGER_ARCHERY_FIRST_END    = 8272, // ice archers end
            AREATRIGGER_ARCHERY_SECOND_FIRST = 7121, // fire archers begin
            AREATRIGGER_ARCHERY_SECOND_END   = 7126  // fire archers end
        };

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                switch (trigger->ID)
                {
                    case AREATRIGGER_ARCHERY_FIRST_BEGIN:
                        if (instance->GetData(DATA_ARCHERY) != IN_PROGRESS && instance->GetData(DATA_ARCHERY) != DONE)
                            instance->SetData(DATA_ARCHERY, IN_PROGRESS);
                        break;
                    case AREATRIGGER_ARCHERY_FIRST_END:
                        if (instance->GetData(DATA_ARCHERY) != DONE)
                            instance->SetData(DATA_ARCHERY, DONE);
                        break;
                    case AREATRIGGER_ARCHERY_SECOND_FIRST:
                        if (instance->GetData(DATA_FIRE_ARCHERY) != IN_PROGRESS && instance->GetData(DATA_FIRE_ARCHERY) != DONE)
                            instance->SetData(DATA_FIRE_ARCHERY, IN_PROGRESS);
                        break;
                    case AREATRIGGER_ARCHERY_SECOND_END:
                        if (instance->GetData(DATA_FIRE_ARCHERY) != DONE)
                            instance->SetData(DATA_FIRE_ARCHERY, DONE);
                        break;
                }
            }

            return false;
        }
};

class spell_spm_death_grip : public SpellScriptLoader
{
    public:
        spell_spm_death_grip() : SpellScriptLoader("spell_spm_death_grip") { }

        class spell_spm_death_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_spm_death_grip_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                if (auto const caster = GetCaster())
                    if (auto const target = GetHitUnit())
                         target->CastSpell(caster, GetSpellInfo()->Effects[effIndex].BasePoints, true);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_spm_death_grip_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_spm_death_grip_SpellScript();
        }
};

// 474. Summoned by 106979 - Fire Arrow
class sat_spm_fire_arrow : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering && triggering->ToPlayer() && triggering->ToPlayer()->IsAlive();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->CastSpell(itr, SPELL_FIRE_ARROW_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Player* itr = triggering->ToPlayer())
            itr->RemoveAurasDueToSpell(SPELL_FIRE_ARROW_EFF);
    }
};

void AddSC_shadopan_monastery()
{
    new npc_shadopan_ambusher();
    new npc_shadopan_warden();
    new npc_shadopan_stormbringer();
    new npc_shadopan_disciple();
    new npc_shadopan_unstable_energy();
    new npc_ethereal_sha();
    new npc_spm_void_sha();
    new npc_consuming_sha();
    new npc_destroying_sha();
    new npc_shadopan_novice();
    new npc_shadopan_archery();
    new npc_residual_hatred();
    new npc_vistige_of_hatred();
    new npc_fragment_of_hatred();
    new npc_hateful_essence();
    new spell_shadopan_explosion();
    new spell_purification_ritual();
    new spell_shadopan_apparitions();
    new spell_spm_fire_arrow();
    new spell_spm_flip_out();
    new spell_spm_shadows_of_destruction();
    new spell_spm_death_grip();
    new AreaTrigger_at_shadopan_archery();
    new atrigger_script<sat_spm_fire_arrow>("sat_spm_fire_arrow");
}
