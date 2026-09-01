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
#include "bastion_of_twilight.h"

enum ScriptTexts
{
    SAY_CHOGALL_0    = 0,
    SAY_CHOGALL_1    = 1,
    SAY_CHOGALL_2    = 2,
    SAY_CHOGALL_3    = 3,
    SAY_CHOGALL_4    = 4,
    SAY_CHOGALL_5    = 5,
    SAY_CHOGALL_6    = 6,
    SAY_CHOGALL_7    = 7,
    SAY_CHOGALL_8    = 8,
    SAY_CHOGALL_9    = 9,
    SAY_CHOGALL_10   = 10,
    SAY_CHOGALL_11   = 11,
    SAY_CHOGALL_12   = 12,
    SAY_CHOGALL_13   = 13,
};

enum Spells
{
    //twilight portal shaper
    SPELL_SHADOW_BOLT             = 85544,
    SPELL_SHAPE_PORTAL            = 85529,
    SPELL_SHAPE_PORTAL_SUM        = 85528,
    
    //twilight shifter
    SPELL_TWILIGHT_SHIFT          = 85556,

    //twilight shadow mender
    SPELL_MIND_SEAR               = 85643,
    SPELL_MIND_SEAR_DMG           = 85647,
    SPELL_UMBRAL_FLAMES           = 85664,
    SPELL_UMBRAL_FLAMES_DMG       = 85679,  
    SPELL_SHADOW_MENDING          = 85575,
    SPELL_SHADOW_MENDING_HEAL     = 85577,
};

enum Events
{
    // Twilight Portal Shaper
    EVENT_SHADOW_BOLT             = 1,
    EVENT_SHAPE_PORTAL            = 2,

    // Twilight Shifter
    EVENT_TWILIGHT_SHIFT          = 3,
    EVENT_NEXT_TARGET_ON          = 4,
    EVENT_NEXT_TARGET_OFF         = 5,

    // Twilight Shadow Mender
    EVENT_MIND_SEAR               = 6,
    EVENT_UMBRAL_FLAMES           = 7,

    // Chogall dlg
    EVENT_ENTRANCE_DLG            = 111,
    EVENT_HALFUS_DLG_1            = 112,
    EVENT_HALFUS_DLG_2            = 113,
    EVENT_VALIONA_THERALION_DLG_1 = 114,
    EVENT_VALIONA_THERALION_DLG_2 = 115,
    EVENT_COUNCIL_DLG_1           = 116,
    EVENT_COUNCIL_DLG_2           = 117,
    EVENT_COUNCIL_DLG_3           = 118,
    EVENT_CHOGALL_DLG             = 119,
};

class npc_twilight_portal_shaper: public CreatureScript
{
    public:
        npc_twilight_portal_shaper() : CreatureScript("npc_twilight_portal_shaper") { }

        struct npc_twilight_portal_shaperAI : public ScriptedAI
        {
            npc_twilight_portal_shaperAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

            SummonList summons;
            EventMap events;

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);
                if (summon->GetEntry() == NPC_FACELESS_MINION)
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                        summon->AI()->AttackStart(target);
            }

            void SummonedCreatureDespawn(Creature* summon) override
            {
                summons.Despawn(summon);
            }

            void JustDied(Unit* /*killer*/) override
            {
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 1000);
                events.ScheduleEvent(EVENT_SHAPE_PORTAL, urand(10000, 15000));
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
                        case EVENT_SHADOW_BOLT:
                            DoCast(me->GetVictim(), SPELL_SHADOW_BOLT);
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 2000);
                            break;
                        case EVENT_SHAPE_PORTAL:
                            DoCast(me, SPELL_SHAPE_PORTAL);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_portal_shaperAI>(creature);
        }
};

class npc_twilight_shifter: public CreatureScript
{
    public:
        npc_twilight_shifter() : CreatureScript("npc_twilight_shifter") { }

        struct npc_twilight_shifterAI : public ScriptedAI
        {
            npc_twilight_shifterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_TWILIGHT_SHIFT, urand(5000, 15000));
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
                        case EVENT_TWILIGHT_SHIFT:
                            DoCast(me, SPELL_TWILIGHT_SHIFT);
                            events.ScheduleEvent(EVENT_TWILIGHT_SHIFT, urand(30000, 35000));
                            events.ScheduleEvent(EVENT_NEXT_TARGET_ON, 3000);
                            break;
                        case EVENT_NEXT_TARGET_ON:
                            DoResetThreat();
                            events.ScheduleEvent(EVENT_NEXT_TARGET_ON, 3000);
                            break;
                        case EVENT_NEXT_TARGET_OFF:
                            events.CancelEvent(EVENT_NEXT_TARGET_ON);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_shifterAI>(creature);
        }
};

class npc_twilight_shadow_mender: public CreatureScript
{
    public:
        npc_twilight_shadow_mender() : CreatureScript("npc_twilight_shadow_mender") { }

        struct npc_twilight_shadow_menderAI : public ScriptedAI
        {
            npc_twilight_shadow_menderAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                DoCast(me, SPELL_SHADOW_MENDING);
                events.ScheduleEvent(EVENT_UMBRAL_FLAMES, urand(10000, 13000));
                events.ScheduleEvent(EVENT_MIND_SEAR, urand(3000, 5000));
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
                        case EVENT_UMBRAL_FLAMES:
                            DoCast(me, SPELL_UMBRAL_FLAMES);
                            events.ScheduleEvent(EVENT_UMBRAL_FLAMES, urand(15000, 20000));
                            break;
                        case EVENT_MIND_SEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1, 0.0f, true))
                                DoCast(target, SPELL_MIND_SEAR);
                            events.ScheduleEvent(EVENT_MIND_SEAR, urand(10000, 12000));
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_twilight_shadow_menderAI>(creature);
        }
};

class npc_chogall_dlg: public CreatureScript
{
    public:
        npc_chogall_dlg() : CreatureScript("npc_chogall_dlg") { }

        struct npc_chogall_dlgAI : public ScriptedAI
        {
            npc_chogall_dlgAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_AT_ENTRANCE:
                        events.ScheduleEvent(EVENT_ENTRANCE_DLG, 5000);
                        break;
                    case ACTION_AT_HALFUS_START:
                        events.ScheduleEvent(EVENT_HALFUS_DLG_1, 1000);
                        break;
                    case ACTION_AT_HALFUS_END:
                        events.ScheduleEvent(EVENT_HALFUS_DLG_2, 7000);
                        break;
                    case ACTION_AT_VALIONA_THERALION_START:
                        events.ScheduleEvent(EVENT_VALIONA_THERALION_DLG_1, 1000);
                        break;
                    case ACTION_AT_VALIONA_THERALION_END:
                        events.ScheduleEvent(EVENT_VALIONA_THERALION_DLG_2, 7000);
                        break;
                    case ACTION_AT_COUNCIL_1:
                        events.ScheduleEvent(EVENT_COUNCIL_DLG_1, 1000);
                        break;
                    case ACTION_AT_COUNCIL_2:
                        events.ScheduleEvent(EVENT_COUNCIL_DLG_2, 1000);
                        break;
                    case ACTION_AT_COUNCIL_3:
                        events.ScheduleEvent(EVENT_COUNCIL_DLG_3, 1000);
                        break;
                    case ACTION_AT_CHOGALL:
                        events.ScheduleEvent(EVENT_CHOGALL_DLG, 1000);
                        break;
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENTRANCE_DLG:
                            Talk(SAY_CHOGALL_0);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_HALFUS_DLG_1:
                            Talk(SAY_CHOGALL_1);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_HALFUS_DLG_2:
                            Talk(SAY_CHOGALL_2);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_VALIONA_THERALION_DLG_1:
                            Talk(SAY_CHOGALL_8);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_VALIONA_THERALION_DLG_2:
                            Talk(SAY_CHOGALL_9);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_COUNCIL_DLG_1:
                            Talk(SAY_CHOGALL_10);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_COUNCIL_DLG_2:
                            Talk(SAY_CHOGALL_11);
                            me->DespawnOrUnsummon();
                            break;
                        case EVENT_COUNCIL_DLG_3:
                            Talk(SAY_CHOGALL_12);
                            me->DespawnOrUnsummon();
                        case EVENT_CHOGALL_DLG:
                            Talk(SAY_CHOGALL_13);
                            me->DespawnOrUnsummon();
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_chogall_dlgAI>(creature);
        }
};

struct twilight_casterAI : public ScriptedAI
{
    twilight_casterAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    InstanceScript* instance;
    SummonList summons;
    EventMap events;

    bool HandleRescheduleEventsIfCastAny(uint32 eventId)
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
        {
            events.RescheduleEvent(eventId, urand(1 * IN_MILLISECONDS, 2 * IN_MILLISECONDS));
            return true;
        }

        return false;
    }

    ObjectGuid GetLowestFriendlyGUID()
    {
        std::list<Creature*> tmpTargets;

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_TWILIGHT_SOUL_BLADE, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_TWILIGHT_SHADOW_KNIGHT, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_TWILIGHT_DARK_MENDER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_TWILIGHT_CROSSFIRE, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_WIND_BREAKER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ELEMENTAL_FIRELORD, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_EARTH_RAVAGER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_CRIMSONBORNE_FIRESTARTER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_AZUREBORNE_DESTROYER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_TWILIGHT_BRUTE, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }
};

// Twilight Soul Blade 45265
class npc_twilight_soul_blade : public CreatureScript
{
    public:
        npc_twilight_soul_blade() : CreatureScript("npc_twilight_soul_blade") { }

        enum iEvents
        {
            EVENT_DARK_POOL=1,
        };

        enum iSpells
        {
            SPELL_DARK_POOL     = 84853,
            SPELL_SOUL_BLADE    = 84849,
            SPELL_BLADE_BARRIER = 84567,
        };

        struct npc_twilight_soul_bladeAI : public ScriptedAI
        {
            npc_twilight_soul_bladeAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool HasBarrier;

            void Reset() override
            {
                events.Reset();
                HasBarrier = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_SOUL_BLADE);
                events.ScheduleEvent(EVENT_DARK_POOL, 6.5 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(35) && !HasBarrier)
                {
                    HasBarrier = true;
                    DoCast(me, SPELL_BLADE_BARRIER, true);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_DARK_POOL)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(target, SPELL_DARK_POOL);
                        else if (Unit* target=me->GetVictim())
                            DoCast(target, SPELL_DARK_POOL);

                        events.ScheduleEvent(EVENT_DARK_POOL, urand(15 * IN_MILLISECONDS, 28 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_soul_bladeAI(creature);
        }
};

// Twilight Shadow Knight 45261
class npc_twilight_shadow_knight : public CreatureScript
{
    public:
        npc_twilight_shadow_knight() : CreatureScript("npc_twilight_shadow_knight") { }

        enum iEvents
        {
            EVENT_DISMANTLE    = 1,
            EVENT_SUNDEN_ARMOR = 2,
            EVENT_DEVASTATE    = 3,
        };

        enum iSpells
        {
            SPELL_DISMANTLE      = 84832,
            SPELL_SHIELD_MASTERY = 84827,
            SPELL_SUNDEN_ARMOR   = 76622,
            SPELL_DEVASTATE      = 78660,
        };

        struct npc_twilight_shadow_knightAI : public ScriptedAI
        {
            npc_twilight_shadow_knightAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool HasBlock;

            void Reset() override
            {
                events.Reset();
                HasBlock = false;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_DEVASTATE, 6.5 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DISMANTLE, urand(14.5 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SUNDEN_ARMOR, 8 * IN_MILLISECONDS);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (HealthBelowPct(50) && !HasBlock)
                {
                    HasBlock = true;
                    DoCast(me, SPELL_SHIELD_MASTERY, true);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEVASTATE:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_DEVASTATE);

                            events.ScheduleEvent(SPELL_DEVASTATE, 7 * IN_MILLISECONDS);
                            break;
                        case EVENT_DISMANTLE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_DISMANTLE, true);

                            events.ScheduleEvent(EVENT_DISMANTLE, urand(14.5 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                            break;
                        case EVENT_SUNDEN_ARMOR:
                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_SUNDEN_ARMOR);

                            events.ScheduleEvent(EVENT_SUNDEN_ARMOR, 8 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_shadow_knightAI(creature);
        }
};

// Twilight Dark Mender 45266
class npc_twilight_dark_mender : public CreatureScript
{
    public:
        npc_twilight_dark_mender() : CreatureScript("npc_twilight_dark_mender") { }

        enum iEvents
        {
            EVENT_HUNGERING_SHADOWS = 1,
            EVENT_DARK_MENDING      = 2,
        };

        enum iSpells
        {
            SPELL_HUNGERING_SHADOWS = 84856,
            SPELL_DARK_MENDING      = 84855,
        };

        struct npc_twilight_dark_menderAI : public twilight_casterAI
        {
            npc_twilight_dark_menderAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_HUNGERING_SHADOWS, urand(6.5 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DARK_MENDING, urand(10 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HUNGERING_SHADOWS:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_HUNGERING_SHADOWS);
                            events.ScheduleEvent(EVENT_HUNGERING_SHADOWS, urand(6.5 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_DARK_MENDING:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                                DoCast(target, SPELL_DARK_MENDING);

                            events.ScheduleEvent(EVENT_DARK_MENDING, urand(10 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_dark_menderAI(creature);
        }
};

// Twilight Crossfire 45264
class npc_twilight_crossfire : public CreatureScript
{
    public:
        npc_twilight_crossfire() : CreatureScript("npc_twilight_crossfire") { }

        enum iEvents
        {
            EVENT_RAPID_FIRE   = 1,
            EVENT_SHOT         = 2,
            EVENT_MULTI_SHOT   = 3,
            EVENT_WYVERN_STING = 4,
        };

        enum iSpells
        {
            SPELL_RAPID_FIRE   = 36828,
            SPELL_SHOT         = 84837,
            SPELL_MULTI_SHOT   = 84836,
            SPELL_WYVERN_STING = 90488,
        };

        struct npc_twilight_crossfireAI : public ScriptedAI
        {
            npc_twilight_crossfireAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHOT, urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RAPID_FIRE, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MULTI_SHOT, urand(7 * IN_MILLISECONDS, 26.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WYVERN_STING, 15 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_SHOT);

                            events.ScheduleEvent(EVENT_SHOT, me->HasAura(SPELL_RAPID_FIRE) ? urand(1 * IN_MILLISECONDS, 1.5 * IN_MILLISECONDS) : urand(2 * IN_MILLISECONDS, 3 * IN_MILLISECONDS));
                            break;
                        case EVENT_RAPID_FIRE:
                            DoCast(me, SPELL_RAPID_FIRE);
                            events.ScheduleEvent(EVENT_RAPID_FIRE, urand(10 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                        case EVENT_MULTI_SHOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                                DoCast(target, SPELL_MULTI_SHOT);

                            events.ScheduleEvent(EVENT_MULTI_SHOT, urand(7 * IN_MILLISECONDS, 26.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_WYVERN_STING:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_WYVERN_STING);

                            events.ScheduleEvent(EVENT_WYVERN_STING, 15 * IN_MILLISECONDS);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_crossfireAI(creature);
        }
};

// Twilight Phase Twister 45267
class npc_twilight_phase_twister : public CreatureScript
{
    public:
        npc_twilight_phase_twister() : CreatureScript("npc_twilight_phase_twister") { }

        enum iEvents
        {
            EVENT_PHASE_TWIST = 1,
        };

        enum iSpells
        {
            SPELL_TWIST_PHASE_COSMETIC = 84737,
            SPELL_TWIST_PHASE          = 84838,
        };

        struct npc_twilight_phase_twisterAI : public ScriptedAI
        {
            npc_twilight_phase_twisterAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            uint32 delay;

            void Reset() override
            {
                events.Reset();

                delay = 0;
                me->m_Events.Schedule(delay += 1000, 2, [this]()
                {
                    DoCast(me, SPELL_TWIST_PHASE_COSMETIC);
                });
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_PHASE_TWIST, urand(6.5 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_PHASE_TWIST)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            DoCast(target, SPELL_TWIST_PHASE);

                        events.ScheduleEvent(SPELL_TWIST_PHASE, urand(6.5 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_phase_twisterAI(creature);
        }
};

// Chosen Seer 46952
class npc_chosen_seer : public CreatureScript
{
    public:
        npc_chosen_seer() : CreatureScript("npc_chosen_seer") { }

        enum iEvents
        {
            EVENT_LAVA_BOLT  = 1,
            EVENT_FIRE_SHOCK = 2,
        };

        enum iSpells
        {
            SPELL_LAVA_BOLT  = 87710,
            SPELL_FIRE_SHOCK = 87707,
        };

        struct npc_chosen_seerAI : public twilight_casterAI
        {
            npc_chosen_seerAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LAVA_BOLT, urand(6.5 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FIRE_SHOCK, urand(3.5 * IN_MILLISECONDS, 13.8 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LAVA_BOLT:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_LAVA_BOLT);

                            events.ScheduleEvent(EVENT_LAVA_BOLT, urand(6.5 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_FIRE_SHOCK:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_FIRE_SHOCK);
                            else if (Unit* target=me->GetVictim())
                                DoCast(target, SPELL_FIRE_SHOCK);

                            events.ScheduleEvent(EVENT_FIRE_SHOCK, urand(3.5 * IN_MILLISECONDS, 13.8 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chosen_seerAI(creature);
        }
};

// Chosen Warrior 46951
class npc_chosen_warrior : public CreatureScript
{
    public:
        npc_chosen_warrior() : CreatureScript("npc_chosen_warrior") { }

        enum iEvents
        {
            EVENT_HAMSTRING = 1,
        };

        enum iSpells
        {
            SPELL_HAMSTRING = 87704,
        };

        struct npc_chosen_warriorAI : public ScriptedAI
        {
            npc_chosen_warriorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_HAMSTRING, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_HAMSTRING)
                    {
                        if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_HAMSTRING);

                        events.ScheduleEvent(EVENT_HAMSTRING, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_chosen_warriorAI(creature);
        }
};

// Crimsonborne Firestarter 47086
class npc_crimsonborne_firestarter : public CreatureScript
{
    public:
        npc_crimsonborne_firestarter() : CreatureScript("npc_crimsonborne_firestarter") { }

        enum iEvents
        {
            EVENT_CRIMSON_FLAMES   = 1,
            EVENT_BURNING_TWILIGHT = 2,
        };

        enum iSpells
        {
            SPELL_CRIMSON_FLAMES      = 88226,
            SPELL_BURNING_TWILIGHT    = 88219,
            SPELL_CRIMSON_FLAMES_AURA = 88231,
        };

        struct npc_crimsonborne_firestarterAI : public twilight_casterAI
        {
            npc_crimsonborne_firestarterAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CRIMSON_FLAMES, urand(11 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BURNING_TWILIGHT, urand(3.5 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                summon->CastSpell(summon, SPELL_CRIMSON_FLAMES_AURA, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CRIMSON_FLAMES:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                                DoCast(target, SPELL_CRIMSON_FLAMES);

                            events.ScheduleEvent(EVENT_CRIMSON_FLAMES, urand(11 * IN_MILLISECONDS, 18.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_BURNING_TWILIGHT:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_BURNING_TWILIGHT);

                            events.ScheduleEvent(EVENT_BURNING_TWILIGHT, urand(3.5 * IN_MILLISECONDS, 23 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_crimsonborne_firestarterAI(creature);
        }
};

// Azureborne Destroyer 47087
class npc_azureborne_destroyer : public CreatureScript
{
    public:
        npc_azureborne_destroyer() : CreatureScript("npc_azureborne_destroyer") { }

        enum iEvents
        {
            EVENT_STASIS_STRIKE = 1,
            EVENT_ARCANE_FISTS  = 2,
        };

        enum iSpells
        {
            SPELL_STASIS_STRIKE = 88178,
            SPELL_ARCANE_FISTS  = 88204,
        };

        struct npc_azureborne_destroyerAI : public ScriptedAI
        {
            npc_azureborne_destroyerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARCANE_FISTS, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STASIS_STRIKE, 16 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STASIS_STRIKE:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_STASIS_STRIKE);

                            events.ScheduleEvent(EVENT_STASIS_STRIKE, 16 * IN_MILLISECONDS);
                            break;
                        case EVENT_ARCANE_FISTS:
                            DoCast(me, SPELL_ARCANE_FISTS);
                            events.ScheduleEvent(EVENT_ARCANE_FISTS, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_azureborne_destroyerAI(creature);
        }
};

// Twilight Brute 47161
class npc_twilight_brute : public CreatureScript
{
    public:
        npc_twilight_brute() : CreatureScript("npc_twilight_brute") { }

        enum iEvents
        {
            EVENT_WHIRLING_BLADES = 1,
        };

        enum iSpells
        {
            SPELL_WHIRLING_BLADES = 88133,
        };

        struct npc_twilight_bruteAI : public ScriptedAI
        {
            npc_twilight_bruteAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WHIRLING_BLADES, 12 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_WHIRLING_BLADES)
                    {
                        DoCast(me, SPELL_WHIRLING_BLADES);
                        me->ClearUnitState(UNIT_STATE_CASTING);
                        events.ScheduleEvent(EVENT_WHIRLING_BLADES, 19 * IN_MILLISECONDS);
                    }
                    break;
                }

                if (!me->HasAura(SPELL_WHIRLING_BLADES))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_bruteAI(creature);
        }
};

// Twilight Elementalist 47152
class npc_twilight_elementalist : public CreatureScript
{
    public:
        npc_twilight_elementalist() : CreatureScript("npc_twilight_elementalist") { }

        enum iEvents
        {
            EVENT_FROST_FIRE_BOLT   = 1,
            EVENT_EMPOWER_ELEMENTAL = 2,
        };

        enum iSpells
        {
            SPELL_FROST_FIRE_BOLT   = 88079,
            SPELL_EMPOWER_ELEMENTAL = 88065,
        };

        struct npc_twilight_elementalistAI : public twilight_casterAI
        {
            npc_twilight_elementalistAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FROST_FIRE_BOLT, urand(4.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EMPOWER_ELEMENTAL, urand(12 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROST_FIRE_BOLT:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_FROST_FIRE_BOLT);
                            else if (Unit* target=me->GetVictim())
                                DoCast(target, SPELL_FROST_FIRE_BOLT);

                            events.ScheduleEvent(EVENT_FROST_FIRE_BOLT, urand(4.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                            break;
                        case EVENT_EMPOWER_ELEMENTAL:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                                DoCast(target, SPELL_EMPOWER_ELEMENTAL);

                            events.ScheduleEvent(EVENT_EMPOWER_ELEMENTAL, urand(12 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_twilight_elementalistAI(creature);
        }
};

// Elemental Firelord 47081
class npc_elemental_firelord : public CreatureScript
{
    public:
        npc_elemental_firelord() : CreatureScript("npc_elemental_firelord") { }

        enum iEvents
        {
            EVENT_MOLTEN_BURST   = 1,
            EVENT_VOLCANIC_WRATH = 2,
        };

        enum iSpells
        {
            SPELL_MOLTEN_BURST   = 87898,
            SPELL_VOLCANIC_WRATH = 87903,
        };

        struct npc_elemental_firelordAI : public twilight_casterAI
        {
            npc_elemental_firelordAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_MOLTEN_BURST, urand(4.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VOLCANIC_WRATH, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOLTEN_BURST:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_MOLTEN_BURST);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_MOLTEN_BURST);

                            events.ScheduleEvent(EVENT_MOLTEN_BURST, urand(4.5 * IN_MILLISECONDS, 5 * IN_MILLISECONDS));
                            break;
                        case EVENT_VOLCANIC_WRATH:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_VOLCANIC_WRATH);
                            events.ScheduleEvent(EVENT_VOLCANIC_WRATH, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_elemental_firelordAI(creature);
        }
};

// Wind Breaker 47151
class npc_wind_breaker : public CreatureScript
{
    public:
        npc_wind_breaker() : CreatureScript("npc_wind_breaker") { }

        enum iEvents
        {
            EVENT_VIOLET_GALE_STORM = 1,
            EVENT_GALE_WIND         = 2,
        };

        enum iSpells
        {
            SPELL_VIOLET_GALE_STORM = 87875,
            SPELL_GALE_WIND         = 87870,
        };

        struct npc_wind_breakerAI : public twilight_casterAI
        {
            npc_wind_breakerAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_GALE_WIND, urand(5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VIOLET_GALE_STORM, 20 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GALE_WIND:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_GALE_WIND);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_GALE_WIND);

                            events.ScheduleEvent(EVENT_GALE_WIND, urand(5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                            break;
                        case EVENT_VIOLET_GALE_STORM:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_VIOLET_GALE_STORM);
                            events.ScheduleEvent(EVENT_VIOLET_GALE_STORM, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wind_breakerAI(creature);
        }
};

// Earth Ravager 47150
class npc_earth_ravager : public CreatureScript
{
    public:
        npc_earth_ravager() : CreatureScript("npc_earth_ravager") { }

        enum iEvents
        {
            EVENT_TREMORS      = 1,
            EVENT_PETRIFY_SKIN = 2,
        };

        enum iSpells
        {
            SPELL_TREMORS      = 87920,
            SPELL_TREMORS_AURA = 87932,
            SPELL_PETRIFY_SKIN = 87917,
        };

        struct npc_earth_ravagerAI : public twilight_casterAI
        {
            npc_earth_ravagerAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_PETRIFY_SKIN, urand(5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_TREMORS, 20 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                summon->CastSpell(summon, SPELL_TREMORS_AURA, true);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TREMORS:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_TREMORS);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_TREMORS);

                            events.ScheduleEvent(EVENT_TREMORS, 20 * IN_MILLISECONDS);
                            break;
                        case EVENT_PETRIFY_SKIN:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_PETRIFY_SKIN);

                            events.ScheduleEvent(EVENT_PETRIFY_SKIN, urand(5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_earth_ravagerAI(creature);
        }
};

// Evolved Drakonaar 49813
class npc_evolved_drakonaar : public CreatureScript
{
    public:
        npc_evolved_drakonaar() : CreatureScript("npc_evolved_drakonaar") { }

        enum iEvents
        {
            EVENT_WHIRLING_BLADES = 1,
            EVENT_CLEAVE          = 2,
        };

        enum iSpells
        {
            SPELL_WHIRLING_BLADES = 93373,
            SPELL_CLEAVE          = 40504,
        };

        struct npc_evolved_drakonaarAI : public ScriptedAI
        {
            npc_evolved_drakonaarAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_WHIRLING_BLADES, 12 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CLEAVE, 6 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CLEAVE:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_CLEAVE);

                            events.ScheduleEvent(EVENT_CLEAVE, 6 * IN_MILLISECONDS);
                            break;
                        case EVENT_WHIRLING_BLADES:
                            DoCast(me, SPELL_WHIRLING_BLADES);
                            me->ClearUnitState(UNIT_STATE_CASTING);
                            events.ScheduleEvent(EVENT_WHIRLING_BLADES, 25 * IN_MILLISECONDS);
                            break;
                    }
                }

                if (!me->HasAura(SPELL_WHIRLING_BLADES))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_evolved_drakonaarAI(creature);
        }
};

// Bound Rumbler 49826
class npc_bound_rumbler : public CreatureScript
{
    public:
        npc_bound_rumbler() : CreatureScript("npc_bound_rumbler") { }

        enum iEvents
        {
            EVENT_ENTOMB    = 1,
            EVENT_SHOCKWAVE = 2,
        };

        enum iSpells
        {
            SPELL_EMTOMB    = 93327,
            SPELL_SHOCKWAVE = 93325,
        };

        struct npc_bound_rumblerAI : public twilight_casterAI
        {
            npc_bound_rumblerAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ENTOMB, urand(5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHOCKWAVE, urand(15 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENTOMB:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_EMTOMB);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_EMTOMB);

                            events.ScheduleEvent(EVENT_ENTOMB, urand(5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                            break;
                        case EVENT_SHOCKWAVE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_SHOCKWAVE);
                            events.ScheduleEvent(EVENT_SHOCKWAVE, urand(15 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bound_rumblerAI(creature);
        }
};

// Bound Inferno 49817
class npc_bound_inferno : public CreatureScript
{
    public:
        npc_bound_inferno() : CreatureScript("npc_bound_inferno") { }

        enum iEvents
        {
            EVENT_FLAMESTRIKE = 1,
        };

        enum iSpells
        {
            SPELL_WARD_OF_COMBUSTION = 93336,
            SPELL_FLAMESTRIKE        = 93362,
        };

        struct npc_bound_infernoAI : public ScriptedAI
        {
            npc_bound_infernoAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_WARD_OF_COMBUSTION);
                events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_FLAMESTRIKE)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            DoCast(target, SPELL_FLAMESTRIKE);
                        else if (Unit* target = me->GetVictim())
                            DoCast(target, SPELL_FLAMESTRIKE);

                        events.ScheduleEvent(EVENT_FLAMESTRIKE, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bound_infernoAI(creature);
        }
};

// Bound Deluge 49825
class npc_bound_deluge : public CreatureScript
{
    public:
        npc_bound_deluge() : CreatureScript("npc_bound_deluge") { }

        enum iEvents
        {
            EVENT_COLD_TOUCHED = 1,
            EVENT_FROST_WHIRL  = 2,
        };

        enum iSpells
        {
            SPELL_ICY_SHROUD   = 93335,
            SPELL_COLD_TOUCHED = 93381,
            SPELL_FROST_WHIRL  = 93340,
        };

        struct npc_bound_delugeAI : public twilight_casterAI
        {
            npc_bound_delugeAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                DoCast(me, SPELL_ICY_SHROUD);
                events.ScheduleEvent(EVENT_FROST_WHIRL, 10 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_COLD_TOUCHED, urand(12 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_COLD_TOUCHED:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_COLD_TOUCHED);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_COLD_TOUCHED);

                            events.ScheduleEvent(EVENT_COLD_TOUCHED, urand(12 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                        case EVENT_FROST_WHIRL:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            DoCast(me, SPELL_FROST_WHIRL);
                            events.ScheduleEvent(EVENT_FROST_WHIRL, 13 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bound_delugeAI(creature);
        }
};

// Bound Zephyr 49821
class npc_bound_zephyr : public CreatureScript
{
    public:
        npc_bound_zephyr() : CreatureScript("npc_bound_zephyr") { }

        enum iEvents
        {
            EVENT_LIGHTNING_SHOCK = 1,
            EVENT_RENDING_GALE    = 2,
            EVENT_VAPORIZE        = 3,
        };

        enum iSpells
        {
            SPELL_LIGHTNING_SHOCK = 93278,
            SPELL_RENDING_GALE    = 93277,
            SPELL_VAPORIZE        = 93306,
        };

        struct npc_bound_zephyrAI : public twilight_casterAI
        {
            npc_bound_zephyrAI(Creature* creature) : twilight_casterAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_LIGHTNING_SHOCK, urand(4.5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RENDING_GALE, urand(12 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VAPORIZE, 10 * IN_MILLISECONDS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LIGHTNING_SHOCK:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_LIGHTNING_SHOCK);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_LIGHTNING_SHOCK);

                            events.ScheduleEvent(EVENT_LIGHTNING_SHOCK, urand(4.5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
                            break;
                        case EVENT_RENDING_GALE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_RENDING_GALE);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_RENDING_GALE);

                            events.ScheduleEvent(EVENT_RENDING_GALE, urand(12 * IN_MILLISECONDS, 22 * IN_MILLISECONDS));
                            break;
                        case EVENT_VAPORIZE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_VAPORIZE);

                            events.ScheduleEvent(EVENT_VAPORIZE, 12 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_bound_zephyrAI(creature);
        }
};

class at_bt_entrance : public AreaTriggerScript
{
    public:
        at_bt_entrance() : AreaTriggerScript("at_bt_entrance") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_ENTRANCE) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                    {
                        pChogall->AI()->DoAction(ACTION_AT_ENTRANCE);
                    }
                    instance->SetData(DATA_DLG_ENTRANCE, DONE);
                }
            }
            return false;
        }
};

class at_bt_halfus: public AreaTriggerScript
{
    public:
        at_bt_halfus() : AreaTriggerScript("at_bt_halfus") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_HALFUS) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                        pChogall->AI()->DoAction(ACTION_AT_HALFUS_START);
                    instance->SetData(DATA_DLG_HALFUS, DONE);
                }
            }
            return true;
        }
};

class at_bt_valiona_theralion : public AreaTriggerScript
{
    public:
        at_bt_valiona_theralion() : AreaTriggerScript("at_bt_valiona_theralion") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_VALIONA_THERALION) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                        pChogall->AI()->DoAction(ACTION_AT_VALIONA_THERALION_START);
                    instance->SetData(DATA_DLG_VALIONA_THERALION, DONE);
                }
            }
            return true;
        }
};

class at_bt_council_1 : public AreaTriggerScript
{
    public:
        at_bt_council_1() : AreaTriggerScript("at_bt_council_1") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_COUNCIL_1) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                        pChogall->AI()->DoAction(ACTION_AT_COUNCIL_1);
                    instance->SetData(DATA_DLG_COUNCIL_1, DONE);
                }
            }
            return true;
        }
};

class at_bt_council_2 : public AreaTriggerScript
{
    public:
        at_bt_council_2() : AreaTriggerScript("at_bt_council_2") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_COUNCIL_2) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                        pChogall->AI()->DoAction(ACTION_AT_COUNCIL_2);
                    instance->SetData(DATA_DLG_COUNCIL_2, DONE);
                }
            }
            return true;
        }
};

class at_bt_council_3 : public AreaTriggerScript
{
    public:
        at_bt_council_3() : AreaTriggerScript("at_bt_council_3") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_COUNCIL_3) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                        pChogall->AI()->DoAction(ACTION_AT_COUNCIL_3);
                    instance->SetData(DATA_DLG_COUNCIL_3, DONE);
                }
            }
            return true;
        }
};

class at_bt_chogall : public AreaTriggerScript
{
    public:
        at_bt_chogall() : AreaTriggerScript("at_bt_chogall") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_CHOGALL) != DONE)
                {
                    if (Creature* pChogall    = player->SummonCreature(NPC_CHOGALL_DLG,
                        player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f))
                        pChogall->AI()->DoAction(ACTION_AT_CHOGALL);
                    instance->SetData(DATA_DLG_CHOGALL, DONE);
                }
            }
            return true;
        }
};

class at_bt_sinestra : public AreaTriggerScript
{
    public:
        at_bt_sinestra() : AreaTriggerScript("at_bt_sinestra") { }

        bool OnTrigger(Player* player, const AreaTriggerEntry* /*trigger*/) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DLG_SINESTRA) != DONE)
                {
                    instance->SetData(DATA_DLG_SINESTRA, DONE);
                }
            }
            return true;
        }
};

class npc_bt_instance_portal: public CreatureScript
{
    public:
        npc_bt_instance_portal() : CreatureScript("npc_bt_instance_portal") { }

        struct npc_bt_instance_portal_AI : public ScriptedAI
        {
            npc_bt_instance_portal_AI(Creature* creature) : ScriptedAI(creature) { }

            void OnSpellClick(Unit* clicker, bool& /*result*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                {
                    AreaTriggerEntry const* at = nullptr;
                    if (instance->GetBossState(DATA_CHOGALL) == DONE)
                        at = sAreaTriggerStore.LookupEntry(6444);
                    else if (instance->GetBossState(DATA_COUNCIL) == DONE)
                        at = sAreaTriggerStore.LookupEntry(6626);
                    else if (instance->GetBossState(DATA_VALIONA_THERALION) == DONE)
                        at = sAreaTriggerStore.LookupEntry(6442);
                    else if (instance->GetBossState(DATA_HALFUS) == DONE)
                        at = sAreaTriggerStore.LookupEntry(6437);
                    
                    if (at)
                        clicker->NearTeleportTo(at->Pos.X, at->Pos.Y, at->Pos.Z, clicker->GetOrientation(), false);
                }
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetInstanceAI<npc_bt_instance_portal_AI>(creature);
        }
};

void AddSC_bastion_of_twilight()
{
    new npc_twilight_portal_shaper();
    new npc_twilight_shifter();
    new npc_twilight_shadow_mender();
    new npc_chogall_dlg();
    new npc_twilight_soul_blade();
    new npc_twilight_shadow_knight();
    new npc_twilight_dark_mender();
    new npc_twilight_crossfire();
    new npc_twilight_phase_twister();
    new npc_chosen_seer();
    new npc_chosen_warrior();
    new npc_crimsonborne_firestarter();
    new npc_azureborne_destroyer();
    new npc_twilight_brute();
    new npc_twilight_elementalist();
    new npc_elemental_firelord();
    new npc_wind_breaker();
    new npc_earth_ravager();
    new npc_evolved_drakonaar();
    new npc_bound_rumbler();
    new npc_bound_inferno();
    new npc_bound_deluge();
    new npc_bound_zephyr();
    new at_bt_entrance();
    new at_bt_halfus();
    new at_bt_valiona_theralion();
    new at_bt_council_1();
    new at_bt_council_2();
    new at_bt_council_3();
    new at_bt_chogall();
    new npc_bt_instance_portal();
}