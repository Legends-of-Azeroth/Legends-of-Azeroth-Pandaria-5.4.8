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
#include "forge_of_souls.h"

enum Spells
{
    // Spiteful Apparition
    SPELL_SPITEFUL_APPARITION_SPITE     = 68895,
    SPELL_SPITEFUL_APPARITION_VISUAL_1  = 69136,
    SPELL_SPITEFUL_APPARITION_VISUAL_2  = 69658,
    SPELL_SOUL_HORROR_VISUAL            = 69105,

    // Spectral Warden
    SPELL_VEIL_OF_SHADOWS               = 69633,
    SPELL_WAIL_OF_SOULS                 = 69148,

    // Soulguard Watchman
    SPELL_SHROUD_OF_RUNES               = 69056,
    SPELL_UNHOLY_RAGE                   = 69053,

    // Soulguard Reaper
    SPELL_FROST_NOVA                    = 70209,
    SPELL_SHADOW_LANCE                  = 69058,
    
    // Soulguard Bonecaster
    SPELL_BONE_VOLLEY                   = 69080,
    SPELL_RAISE_DEAD                    = 69562,
    SPELL_SHIELD_OF_BONES               = 69069,
    
    // Soulguard Animator
    // Raise dead 69562
    SPELL_SHADOW_BOLT                   = 69068,
    SPELL_SOUL_SICKNESS                 = 69131,
    SPELL_SOUL_SIPHON                   = 69128,
    
    // Soulguard Adept
    // Raise dead 69562
    // Shadow Bolt 69068/70208
    SPELL_DRAIN_LIFE                    = 69066,
    SPELL_SHADOW_MEND                   = 69564,
    
    // Soul Horror
    SPELL_SOUL_STRIKE                   = 69088,
    SPELL_SOUL_CHANNELING_IMPACT        = 68837,
    
    SPELL_SOULGUARD_CHANNEL_BEAM02      = 68834,
    SPELL_SOULGUARD_CHANNEL             = 68797,
};

enum Events
{
    EVENT_NONE,

    // Jaina/Sylvanas Intro
    EVENT_INTRO_1,
    EVENT_INTRO_2,
    EVENT_INTRO_3,
    EVENT_INTRO_4,
    EVENT_INTRO_5,
    EVENT_INTRO_6,
    EVENT_INTRO_7,
    EVENT_INTRO_8,

    // Spiteful Apparition
    EVENT_SPITEFUL_APPARITION_SPITE,

    // Spectral Warden
    EVENT_VEIL_OF_SHADOWS,
    EVENT_WAIL_OF_SOULS,

    // Soulguard Watchman
    EVENT_SHROUD_OF_RUNES,

    // Soulguard Reaper
    EVENT_FROST_NOVA,
    EVENT_SHADOW_LANCE,

    // Soulguard Bonecaster
    EVENT_BONE_VOLLEY,
    EVENT_RAISE_DEAD,
    EVENT_SHIELD_OF_BONES,

    // Soulguard Animator
    EVENT_SHADOW_BOLT,
    EVENT_SOUL_SICKNESS,
    EVENT_SOUL_SIPHON,

    // Soulguard Adept
    EVENT_DRAIN_LIFE,
    EVENT_SHADOW_MEND,

    // Soul Horror
    EVENT_SOUL_STRIKE,
};

enum Yells
{
    SAY_JAINA_SYLVANAS_INTRO_1              = 0,
    SAY_JAINA_SYLVANAS_INTRO_2              = 1,
    SAY_JAINA_SYLVANAS_INTRO_3              = 2,
    SAY_JAINA_SYLVANAS_INTRO_4              = 3,
    SAY_JAINA_SYLVANAS_INTRO_5              = 4,
    SAY_JAINA_INTRO_6                       = 5,
    SAY_JAINA_INTRO_7                       = 6,
};

enum eSylvanas
{
    GOSSIP_SPEECHINTRO                      = 13525,
    ACTION_INTRO,
};

enum Phase
{
    PHASE_NORMAL,
    PHASE_INTRO,
};

class npc_sylvanas_fos : public CreatureScript
{
    public:
        npc_sylvanas_fos() : CreatureScript("npc_sylvanas_fos") { }

        struct npc_sylvanas_fosAI : public ScriptedAI
        {
            npc_sylvanas_fosAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                player->CLOSE_GOSSIP_MENU();
                if (sender == 10942 && action == 0)
                    StartScene(player);
                return true;
            }

            void OnQuestAccept(Player* player, Quest const* /*quest*/) override
            {
                StartScene(player, 5000);
            }

        private:
            bool started = false;

            void StartScene(Player* player, uint32 firstDelay = 0)
            {
                if (started)
                    return;

                started = true;
                me->SetGossip(true, 10971);
                me->SetFacingToObject(player);
                uint32 delay = firstDelay;
                me->m_Events.Schedule(delay +=   200, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_1); });
                me->m_Events.Schedule(delay += 11500, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_2); });
                me->m_Events.Schedule(delay += 10500, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_3); });
                me->m_Events.Schedule(delay += 10500, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_4); });
                me->m_Events.Schedule(delay += 12000, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_5); });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_sylvanas_fosAI(creature);
        }
};

class npc_jaina_fos : public CreatureScript
{
    public:
        npc_jaina_fos() : CreatureScript("npc_jaina_fos") { }

        struct npc_jaina_fosAI: public ScriptedAI
        {
            npc_jaina_fosAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {
                player->CLOSE_GOSSIP_MENU();
                if (sender == 10943 && action == 0)
                    StartScene(player);
                return true;
            }

            void OnQuestAccept(Player* player, Quest const* /*quest*/) override
            {
                StartScene(player, 5000);
            }

        private:
            bool started = false;

            void StartScene(Player* player, uint32 firstDelay = 0)
            {
                if (started)
                    return;

                started = true;
                me->SetGossip(true, 10970);
                me->SetFacingToObject(player);
                uint32 delay = firstDelay;
                me->m_Events.Schedule(delay +=   200, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_1); });
                me->m_Events.Schedule(delay +=  8500, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_2); });
                me->m_Events.Schedule(delay += 12000, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_3); });
                me->m_Events.Schedule(delay +=  8500, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_4); });
                me->m_Events.Schedule(delay += 11000, [this]() { Talk(SAY_JAINA_SYLVANAS_INTRO_5); });
                me->m_Events.Schedule(delay +=  8500, [this]() { Talk(SAY_JAINA_INTRO_6); });
                me->m_Events.Schedule(delay += 12000, [this]() { Talk(SAY_JAINA_INTRO_7); });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jaina_fosAI(creature);
        }
};

class npc_spiteful_apparition : public CreatureScript
{
    public:
        npc_spiteful_apparition() : CreatureScript("npc_spiteful_apparition") { }

        struct npc_spiteful_apparitionAI: public ScriptedAI
        {
            npc_spiteful_apparitionAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (damage >= me->GetHealth() && !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    DoCast(me, SPELL_SPITEFUL_APPARITION_SPITE);
                    damage = 0;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                DoCast(me, SPELL_SOUL_HORROR_VISUAL, true);
                DoCast(me, SPELL_SPITEFUL_APPARITION_VISUAL_1, true);
                events.ScheduleEvent(EVENT_SPITEFUL_APPARITION_SPITE, urand(3000, 5000));
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
                        case EVENT_SPITEFUL_APPARITION_SPITE:
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveIdle();
                            DoCast(me, SPELL_SPITEFUL_APPARITION_SPITE);
                            events.CancelEvent(EVENT_SPITEFUL_APPARITION_SPITE);
                            me->m_Events.Schedule(3500, [this]()
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->RemoveAurasDueToSpell(SPELL_SOUL_HORROR_VISUAL);
                                me->RemoveAurasDueToSpell(SPELL_SPITEFUL_APPARITION_VISUAL_1);
                            });
                            me->DespawnOrUnsummon(6000);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spiteful_apparitionAI(creature);
        }
};

class npc_spectral_warden : public CreatureScript
{
    public:
        npc_spectral_warden() : CreatureScript("npc_spectral_warden") { }

        struct npc_spectral_wardenAI: public ScriptedAI
        {
            npc_spectral_wardenAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_VEIL_OF_SHADOWS, urand(3000, 5000));
                events.ScheduleEvent(EVENT_WAIL_OF_SOULS, 10000);
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
                        case EVENT_VEIL_OF_SHADOWS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_VEIL_OF_SHADOWS);
                            events.RescheduleEvent(EVENT_VEIL_OF_SHADOWS, urand(5000, 10000));
                            break;
                        case EVENT_WAIL_OF_SOULS:
                            DoCastVictim(SPELL_WAIL_OF_SOULS);
                            events.RescheduleEvent(EVENT_WAIL_OF_SOULS, 5000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spectral_wardenAI(creature);
        }
};

class npc_soulguard_watchman : public CreatureScript
{
    public:
        npc_soulguard_watchman() : CreatureScript("npc_soulguard_watchman") { }

        struct npc_soulguard_watchmanAI: public ScriptedAI
        {
            npc_soulguard_watchmanAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool rage;

            void Reset() override
            {
                rage = false;
                events.Reset();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!rage && me->HealthBelowPctDamaged(25, damage))
                {
                    DoCast(me, SPELL_UNHOLY_RAGE);
                    rage = true;
                }
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHROUD_OF_RUNES, urand(5000, 15000));
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
                        case EVENT_SHROUD_OF_RUNES:
                            DoCast(me, SPELL_SHROUD_OF_RUNES);
                            events.CancelEvent(EVENT_SHROUD_OF_RUNES);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soulguard_watchmanAI(creature);
        }
};

void CommonJustReachedHome(Creature* me)
{
    Creature* horror = NULL;
    Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, NPC_SOUL_HORROR, true, 40.0f);
    Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, horror, checker);
    me->VisitNearbyObject(40.0f, searcher, true); // Load grids

    if (horror)
    {
        if (horror->isMoving() || horror->IsInEvadeMode())
            me->m_Events.Schedule(1000, [me]() { CommonJustReachedHome(me); });
        else
            me->CastSpell(horror, SPELL_SOULGUARD_CHANNEL_BEAM02);
    }
    else if (me->GetDefaultMovementType() != WAYPOINT_MOTION_TYPE)
        me->CastSpell(me, SPELL_SOULGUARD_CHANNEL);
}

class npc_soulguard_reaper : public CreatureScript
{
    public:
        npc_soulguard_reaper() : CreatureScript("npc_soulguard_reaper") { }

        struct npc_soulguard_reaperAI: public ScriptedAI
        {
            npc_soulguard_reaperAI(Creature* creature) : ScriptedAI(creature)
            {
                if (me->IsAlive())
                    me->m_Events.Schedule(1000, [this]() { JustReachedHome(); });
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustReachedHome() override
            {
                CommonJustReachedHome(me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(false);
                events.ScheduleEvent(EVENT_FROST_NOVA, urand(5000, 10000));
                events.ScheduleEvent(EVENT_SHADOW_LANCE, urand(5000, 10000));
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
                        case EVENT_FROST_NOVA:
                            DoCast(me, SPELL_FROST_NOVA);
                            events.RescheduleEvent(EVENT_FROST_NOVA, urand(10000, 20000));
                            break;
                        case EVENT_SHADOW_LANCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SHADOW_LANCE);
                            events.RescheduleEvent(EVENT_SHADOW_LANCE, urand(5000, 10000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soulguard_reaperAI(creature);
        }
};

class npc_soulguard_bonecaster : public CreatureScript
{
    public:
        npc_soulguard_bonecaster() : CreatureScript("npc_soulguard_bonecaster") { }

        struct npc_soulguard_bonecasterAI: public ScriptedAI
        {
            npc_soulguard_bonecasterAI(Creature* creature) : ScriptedAI(creature)
            {
                if (me->IsAlive())
                    me->m_Events.Schedule(1000, [this]() { JustReachedHome(); });
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustReachedHome() override
            {
                CommonJustReachedHome(me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(false);
                events.ScheduleEvent(EVENT_BONE_VOLLEY, urand(100, 3000));
                events.ScheduleEvent(EVENT_RAISE_DEAD, urand(25000, 35000));
                events.ScheduleEvent(EVENT_SHIELD_OF_BONES, urand(4000, 8000));

                me->CallForHelp(20);
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
                        case EVENT_BONE_VOLLEY:
                            DoCastAOE(SPELL_BONE_VOLLEY);
                            events.RescheduleEvent(EVENT_BONE_VOLLEY, urand(5000, 15000));
                            break;
                        case EVENT_RAISE_DEAD:
                            DoCast(me, SPELL_RAISE_DEAD);
                            events.RescheduleEvent(EVENT_RAISE_DEAD, urand(25000, 35000));
                            break;
                        case EVENT_SHIELD_OF_BONES:
                            DoCast(me, SPELL_SHIELD_OF_BONES);
                            events.RescheduleEvent(EVENT_SHIELD_OF_BONES, urand(10000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soulguard_bonecasterAI(creature);
        }
};

class npc_soulguard_animator : public CreatureScript
{
    public:
        npc_soulguard_animator() : CreatureScript("npc_soulguard_animator") { }

        struct npc_soulguard_animatorAI : public ScriptedAI
        {
            npc_soulguard_animatorAI(Creature* creature) : ScriptedAI(creature)
            {
                if (me->IsAlive())
                    me->m_Events.Schedule(1000, [this]() { JustReachedHome(); });
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustReachedHome() override
            {
                CommonJustReachedHome(me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(false);
                events.ScheduleEvent(EVENT_RAISE_DEAD, urand(25000, 35000));
                events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(100, 5000));
                events.ScheduleEvent(EVENT_SOUL_SICKNESS, urand(10000, 20000));
                events.ScheduleEvent(EVENT_SOUL_SIPHON, urand(10000, 20000));
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
                        case EVENT_RAISE_DEAD:
                            DoCast(me, SPELL_RAISE_DEAD);
                            events.RescheduleEvent(EVENT_RAISE_DEAD, urand(25000, 35000));
                            break;
                        case EVENT_SHADOW_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SHADOW_BOLT);
                            events.RescheduleEvent(EVENT_SHADOW_BOLT, urand(5000, 10000));
                            break;
                        case EVENT_SOUL_SICKNESS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SOUL_SICKNESS);
                            events.RescheduleEvent(EVENT_SOUL_SICKNESS, urand(10000, 20000));
                            break;
                        case EVENT_SOUL_SIPHON:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SOUL_SIPHON);
                            events.RescheduleEvent(EVENT_SOUL_SIPHON, urand(10000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soulguard_animatorAI(creature);
        }
};

class npc_soulguard_adept : public CreatureScript
{
    public:
        npc_soulguard_adept() : CreatureScript("npc_soulguard_adept") { }

        struct npc_soulguard_adeptAI: public ScriptedAI
        {
            npc_soulguard_adeptAI(Creature* creature) : ScriptedAI(creature)
            {
                if (me->IsAlive())
                    me->m_Events.Schedule(1000, [this]() { JustReachedHome(); });
            }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustReachedHome() override
            {
                CommonJustReachedHome(me);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(false);
                events.ScheduleEvent(EVENT_RAISE_DEAD, urand(25000, 35000));
                events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(100, 3000));
                events.ScheduleEvent(EVENT_DRAIN_LIFE, urand(5000, 15000));
                events.ScheduleEvent(EVENT_SHADOW_MEND, urand(15000, 25000));
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
                        case EVENT_RAISE_DEAD:
                            DoCast(me, SPELL_RAISE_DEAD);
                            events.RescheduleEvent(EVENT_RAISE_DEAD, urand(25000, 35000));
                            break;
                        case EVENT_SHADOW_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_SHADOW_BOLT);
                            events.RescheduleEvent(EVENT_SHADOW_BOLT, urand(5000, 10000));
                            break;
                        case EVENT_DRAIN_LIFE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(target, SPELL_DRAIN_LIFE);
                            events.RescheduleEvent(EVENT_DRAIN_LIFE, urand(10000, 20000));
                            break;
                        case EVENT_SHADOW_MEND:
                            if (Unit* target = DoSelectLowestHpFriendly(39, 11000))
                                DoCast(target, SPELL_SHADOW_MEND);
                            else
                                DoCast(me, SPELL_SHADOW_MEND);
                            events.RescheduleEvent(EVENT_SHADOW_MEND, urand(10000, 20000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soulguard_adeptAI(creature);
        }
};

class npc_soul_horror : public CreatureScript
{
    public:
        npc_soul_horror() : CreatureScript("npc_soul_horror") { }

        struct npc_soul_horrorAI : public ScriptedAI
        {
            npc_soul_horrorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
                JustReachedHome();
            }

            void JustReachedHome() override
            {
                DoCast(me, SPELL_SOUL_CHANNELING_IMPACT);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->InterruptNonMeleeSpells(false);
                events.ScheduleEvent(EVENT_SOUL_STRIKE, urand(100, 3000));
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
                        case EVENT_SOUL_STRIKE:
                            DoCast(me->GetVictim(), SPELL_SOUL_STRIKE);
                            events.RescheduleEvent(EVENT_SOUL_STRIKE, urand(3000, 8000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_soul_horrorAI(creature);
        }
};

void AddSC_forge_of_souls()
{
    new npc_sylvanas_fos;
    new npc_jaina_fos;
    new npc_spiteful_apparition;
    new npc_spectral_warden;
    new npc_soulguard_watchman;
    new npc_soulguard_reaper;
    new npc_soulguard_bonecaster;
    new npc_soulguard_animator;
    new npc_soulguard_adept;
    new npc_soul_horror;
}
