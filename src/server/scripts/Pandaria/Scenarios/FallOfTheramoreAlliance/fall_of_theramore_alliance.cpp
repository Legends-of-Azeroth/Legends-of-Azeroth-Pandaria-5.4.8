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

#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GridNotifiers.h"
#include "fall_of_theramore_alliance.h"

enum Talks
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
    TALK_SPECIAL_3,
    TALK_SPECIAL_4,
};

struct theramore_invader_typeAI : public ScriptedAI
{
    theramore_invader_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    InstanceScript* instance;
    SummonList summons;
    EventMap events, nonCombatEvents;
    bool hasInterrupted, intro;
    uint32 prevSpellId, delay;
    ObjectGuid targetGUID;

    void Reset() override
    {
        instance = me->GetInstanceScript();
    }

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_ACTIVATE_BY_REGION:
                me->SetVisible(true);

                switch (me->GetEntry())
                {
                    case NPC_LEAKED_OIL_DRUM:
                    case NPC_JAINA_PROUDMOORE:
                        break;
                    default:
                        me->SetFaction(14);
                        break;
                }

                break;
            case ACTION_JAINA_INTRO:
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                Talk(TALK_INTRO);

                delay = 0;
                me->m_Events.Schedule(delay += 4500, 1, [this]()
                {
                    if (TempSummon* bunny = me->SummonCreature(NPC_GENERAL_BUNNY_JMF, BunnyPosition, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        me->SetFacingTo(me->GetAngle(bunny));
                        DoCast(bunny, SPELL_ARCANE_BEAM, true);

                        for (uint8 i = 0; i < 3; i++)
                            me->SummonGameObject(GO_STOLEN_STANDART, StandartsSpawnPos[i].GetPositionX(), StandartsSpawnPos[i].GetPositionY(), StandartsSpawnPos[i].GetPositionZ(), StandartsSpawnPos[i].GetOrientation(), { }, 0);
                    }
                });

                if (instance)
                    instance->SetData(DATA_LADY_PROUDMOORE, DONE);
                break;
            case ACTION_JAINA_LAST_STAND:
            {
                if (intro)
                    break;

                intro = true;
                me->InterruptNonMeleeSpells(true);
                Talk(TALK_SPECIAL_1);
                DoCast(me, SPELL_SUMMON_WATER_ELEMENTALS);

                if (TempSummon* Warlord = me->SummonCreature(NPC_WARLORD_ROHNAH, WarlordSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    Warlord->AI()->Talk(TALK_INTRO);

                    delay = 0;
                    me->m_Events.Schedule(delay += 3000, 1, [Warlord]()
                    {
                        Warlord->AI()->Talk(TALK_INTRO);
                    });

                    delay = 0;
                    me->m_Events.Schedule(delay += 3500, 1, [this]()
                    {
                        Talk(TALK_SPECIAL_2);
                    });

                    me->m_Events.Schedule(delay += 2500, 1, [Warlord]()
                    {
                        Warlord->AI()->Talk(TALK_SPECIAL_1);
                    });

                    me->m_Events.Schedule(delay += 3000, 1, [this]()
                    {
                        Talk(TALK_SPECIAL_3);
                    });

                }

                break;
            }
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetEntry() == NPC_WARLORD_ROHNAH && instance)
            instance->SetData(DATA_LAST_STAND, DONE);
    }

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

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_AIRSHIP_CREWMAN, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_AIRSHIP_MARINE, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ROKNAH_GRUNT, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ROKNAH_HEADHUNTER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ROKNAH_LOA_SINGER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ROKNAH_FELCASTER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ROKNAH_RIDER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_ROKNAH_SKRIMISHER, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }
};

class npc_theramore_invader_base : public CreatureScript
{
    public:
        npc_theramore_invader_base() : CreatureScript("npc_theramore_invader_base") { }

        struct npc_theramore_invader_baseAI : public theramore_invader_typeAI
        {
            npc_theramore_invader_baseAI(Creature* creature) : theramore_invader_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_invader_baseAI(creature);
        }
};

// Sergeant Grud 65785
class npc_theramore_sergeant_grud : public CreatureScript
{
    public:
        npc_theramore_sergeant_grud() : CreatureScript("npc_theramore_sergeant_grud") { }

        enum iSpells
        {
            SPELL_CLEAVE = 15496,
        };

        enum iEvents
        {
            EVENT_CLEAVE = 1,
        };

        struct npc_theramore_sergeant_grudAI : public ScriptedAI
        {
            npc_theramore_sergeant_grudAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8*IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_CLEAVE)
                    {
                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_CLEAVE, false);

                        events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_sergeant_grudAI(creature);
        }
};

// Rok`nah Grunt 64732
class npc_theramore_roknah_grunt : public CreatureScript
{
    public:
        npc_theramore_roknah_grunt() : CreatureScript("npc_theramore_roknah_grunt") { }

        enum iSpells
        {
            SPELL_CLEAVE = 15496,
        };

        enum iEvents
        {
            EVENT_CLEAVE = 1,
        };

        struct npc_theramore_roknah_gruntAI : public theramore_invader_typeAI
        {
            npc_theramore_roknah_gruntAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8*IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_CLEAVE)
                    {
                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_CLEAVE, false);

                        events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_roknah_gruntAI(creature);
        }
};

// Airship Crewman 65610
class npc_theramore_airship_crewman : public CreatureScript
{
    public:
        npc_theramore_airship_crewman() : CreatureScript("npc_theramore_airship_crewman") { }

        enum iSpells
        {
            SPELL_SNAP_KICK = 15618,
        };

        enum iEvents
        {
            EVENT_KICK = 1,
        };

        struct npc_theramore_airship_crewmanAI : public theramore_invader_typeAI
        {
            npc_theramore_airship_crewmanAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_KICK, urand(2.5 * IN_MILLISECONDS, 9.4 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_KICK)
                    {
                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_SNAP_KICK, false);

                        events.ScheduleEvent(EVENT_KICK, urand(2.5 * IN_MILLISECONDS, 9.4 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_airship_crewmanAI(creature);
        }
};

// Sky Captain Dazrip 65609
class npc_theramore_sky_captain_dazrip : public CreatureScript
{
    public:
        npc_theramore_sky_captain_dazrip() : CreatureScript("npc_theramore_sky_captain_dazrip") { }

        enum iSpells
        {
            SPELL_DASHING_SMILE = 114480,
            SPELL_CLEAVE        = 15496,
        };

        enum iEvents
        {
            EVENT_DASHING_SMILE = 1,
            EVENT_CLEAVE        = 2,
        };

        struct npc_theramore_sky_captain_dazripAI : public theramore_invader_typeAI
        {
            npc_theramore_sky_captain_dazripAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DASHING_SMILE, urand(9.5 * IN_MILLISECONDS, 16.5 * IN_MILLISECONDS));
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
                        case EVENT_DASHING_SMILE:
                            DoCast(me, SPELL_DASHING_SMILE);
                            events.ScheduleEvent(EVENT_DASHING_SMILE, urand(12.5 * IN_MILLISECONDS, 16.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_CLEAVE:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_CLEAVE, false);

                            events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_sky_captain_dazripAI(creature);
        }
};

// Rok`nah Felcaster 65507
class npc_theramore_roknah_felcaster : public CreatureScript
{
    public:
        npc_theramore_roknah_felcaster() : CreatureScript("npc_theramore_roknah_felcaster") { }

        enum iSpells
        {
            SPELL_FEL_ARMOR      = 79954,
            SPELL_FEL_IMMOLATE   = 84469,
            SPELL_CHAOS_BOLT     = 79939,
            SPELL_FEL_INCINERATE = 128658,
        };

        enum iEvents
        {
            EVENT_FEL_INCINERATE = 1,
            EVENT_SECOND_ABILITY = 2,
        };

        struct npc_theramore_roknah_felcasterAI : public theramore_invader_typeAI
        {
            npc_theramore_roknah_felcasterAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                events.Reset();
                hasInterrupted = false;
                prevSpellId = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FEL_INCINERATE, urand(2 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SECOND_ABILITY, urand(7 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                if (int32 sDuration = spell->GetMaxDuration())
                {
                    events.RescheduleEvent(EVENT_FEL_INCINERATE, sDuration + 0.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SECOND_ABILITY, sDuration + 0.5 * IN_MILLISECONDS + 7 * IN_MILLISECONDS);
                }
                else
                {
                    events.RescheduleEvent(EVENT_FEL_INCINERATE, 4 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SECOND_ABILITY, 7 * IN_MILLISECONDS);
                }

                hasInterrupted = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
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
                        case EVENT_FEL_INCINERATE:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (hasInterrupted)
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                hasInterrupted = false;
                            }

                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_FEL_INCINERATE, false);

                            events.ScheduleEvent(EVENT_FEL_INCINERATE, urand(3 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_SECOND_ABILITY:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (hasInterrupted)
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                hasInterrupted = false;
                            }

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, GetAnySecondSpell(), false);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, GetAnySecondSpell(), false);

                            events.ScheduleEvent(EVENT_SECOND_ABILITY, urand(7 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                    }
                }

                if (hasInterrupted)
                    DoMeleeAttackIfReady();
            }

            private:
                uint32 GetAnySecondSpell()
                {
                    std::vector<uint32> SecondComboSpellStorage = { SPELL_FEL_ARMOR, SPELL_FEL_IMMOLATE, SPELL_CHAOS_BOLT };

                    // Remove From Temp container value, that was used before (real random)
                    if (prevSpellId)
                        SecondComboSpellStorage.erase(std::find(SecondComboSpellStorage.begin(), SecondComboSpellStorage.end(), prevSpellId));

                    // Select new spell from updated container
                    prevSpellId = Trinity::Containers::SelectRandomContainerElement(SecondComboSpellStorage);

                    return prevSpellId;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_roknah_felcasterAI(creature);
        }
};

// Rok`nah Headhunter 64735
class npc_theramore_roknah_headhunter : public CreatureScript
{
    public:
        npc_theramore_roknah_headhunter() : CreatureScript("npc_theramore_roknah_headhunter") { }

        enum iSpells
        {
            SPELL_MULTY_SHOT = 102411,
        };

        enum iEvents
        {
            EVENT_SHOT = 1,
        };

        struct npc_theramore_roknah_headhunterAI : public theramore_invader_typeAI
        {
            npc_theramore_roknah_headhunterAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_SHOT, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_SHOT)
                    {
                        if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                            DoCast(target, SPELL_MULTY_SHOT, true);

                        events.ScheduleEvent(EVENT_SHOT, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_roknah_headhunterAI(creature);
        }
};

// Rok`nah rider 64729
class npc_theramore_roknah_rider : public CreatureScript
{
    public:
        npc_theramore_roknah_rider() : CreatureScript("npc_theramore_roknah_rider") { }

        enum iSpells
        {
            SPELL_CLEAVE = 15496,
            SPELL_CHARGE = 22911,
        };

        enum iEvents
        {
            EVENT_CLEAVE = 1,
            EVENT_CHARGE = 2,
        };

        struct npc_theramore_roknah_riderAI : public theramore_invader_typeAI
        {
            npc_theramore_roknah_riderAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHARGE, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
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
                                DoCast(vict, SPELL_CLEAVE, false);

                            events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                            break;
                        case EVENT_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 160.0f, true))
                                DoCast(target, SPELL_CHARGE);

                            events.ScheduleEvent(EVENT_CHARGE, urand(6.5 * IN_MILLISECONDS, 17.5 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_roknah_riderAI(creature);
        }
};

// Rok`nah Loa-Singer 64733
class npc_theramore_roknah_loa_singer : public CreatureScript
{
    public:
        npc_theramore_roknah_loa_singer() : CreatureScript("npc_theramore_roknah_loa_singer") { }

        enum iSpells
        {
            SPELL_HOLLY_SMITE       = 25054,
            SPELL_HEAL              = 22883,
            SPELL_POWER_WORD_SHIELD = 17139,
        };

        enum iEvents
        {
            EVENT_HOLLY_SMITE = 1,
            EVENT_HEAL        = 2,
            EVENT_SHIELD      = 3,
        };

        struct npc_theramore_roknah_loa_singerAI : public theramore_invader_typeAI
        {
            npc_theramore_roknah_loa_singerAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_HOLLY_SMITE, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL, 9 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SHIELD, 1 * IN_MILLISECONDS);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                if (int32 sDuration = spell->GetMaxDuration())
                {
                    events.RescheduleEvent(EVENT_HOLLY_SMITE, sDuration + 0.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, sDuration + 0.5 * IN_MILLISECONDS + 7 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, sDuration + 0.5 * IN_MILLISECONDS + 4.5 * IN_MILLISECONDS);
                }
                else
                {
                    events.RescheduleEvent(EVENT_HOLLY_SMITE, 3.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, 7 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, 4.5 * IN_MILLISECONDS);
                }

                hasInterrupted = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
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
                    case EVENT_HOLLY_SMITE:
                        if (HandleRescheduleEventsIfCastAny(eventId))
                            break;

                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_HOLLY_SMITE, false);

                        events.RescheduleEvent(EVENT_HOLLY_SMITE, 3.5 * IN_MILLISECONDS);
                        break;
                    case EVENT_HEAL:
                        if (HandleRescheduleEventsIfCastAny(eventId))
                            break;

                        if (Unit* lowTarget = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                            DoCast(lowTarget, SPELL_HEAL);

                        events.ScheduleEvent(EVENT_HEAL, urand(9000, 14000));
                        break;
                    case EVENT_SHIELD:
                        if (HandleRescheduleEventsIfCastAny(eventId))
                            break;

                        if (Unit* lowTarget = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                            DoCast(lowTarget, SPELL_HEAL);

                        events.ScheduleEvent(EVENT_SHIELD, urand(9000, 14000));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_roknah_loa_singerAI(creature);
        }
};

// Rok`nah Wave-Caller 65510
class npc_theramore_roknah_wave_caller : public CreatureScript
{
    public:
        npc_theramore_roknah_wave_caller() : CreatureScript("npc_theramore_roknah_wave_caller") { }

        enum iSpells
        {
            SPELL_FROST_SHOCK     = 79925,
            SPELL_HEALING_WAVE    = 15982,
            SPELL_EARTHBIND_TOTEM = 129411,
            SPELL_EARTH_SHIELD    = 79927,
        };

        enum iEvents
        {
            EVENT_OFFENSIVE_ABILITY = 1,
            EVENT_HEALING_WAVE      = 2,
            EVENT_SHIELD            = 3,
        };

        struct npc_theramore_roknah_wave_callerAI : public theramore_invader_typeAI
        {
            npc_theramore_roknah_wave_callerAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                SetCombatMovement(false);
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_OFFENSIVE_ABILITY, urand(2 * IN_MILLISECONDS, 4 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEALING_WAVE, 9 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SHIELD, 1 * IN_MILLISECONDS);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                if (int32 sDuration = spell->GetMaxDuration())
                {
                    events.RescheduleEvent(EVENT_OFFENSIVE_ABILITY, sDuration + 0.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, sDuration + 0.5 * IN_MILLISECONDS + 7 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, sDuration + 0.5 * IN_MILLISECONDS + 4.5 * IN_MILLISECONDS);
                }
                else
                {
                    events.RescheduleEvent(EVENT_OFFENSIVE_ABILITY, 3.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, 7 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SHIELD, 4.5 * IN_MILLISECONDS);
                }

                hasInterrupted = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
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
                    case EVENT_OFFENSIVE_ABILITY:
                        if (HandleRescheduleEventsIfCastAny(eventId))
                            break;

                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, urand(0, 1) ? SPELL_FROST_SHOCK : SPELL_EARTHBIND_TOTEM, false);

                        events.RescheduleEvent(EVENT_OFFENSIVE_ABILITY, 8.5 * IN_MILLISECONDS);
                        break;
                    case EVENT_HEALING_WAVE:
                        if (HandleRescheduleEventsIfCastAny(eventId))
                            break;

                        if (Unit* lowTarget = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                            DoCast(lowTarget, SPELL_HEALING_WAVE);

                        events.ScheduleEvent(EVENT_HEALING_WAVE, urand(9000, 14000));
                        break;
                    case EVENT_SHIELD:
                        if (HandleRescheduleEventsIfCastAny(eventId))
                            break;

                        if (Unit* lowTarget = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                            DoCast(lowTarget, SPELL_EARTH_SHIELD);

                        events.ScheduleEvent(EVENT_SHIELD, urand(9000, 14000));
                        break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_roknah_wave_callerAI(creature);
        }
};

// Captain Korthok 65152
class npc_theramore_captain_korthok : public CreatureScript
{
    public:
        npc_theramore_captain_korthok() : CreatureScript("npc_theramore_captain_korthok") { }

        enum iSpells
        {
            SPELL_DANCING_BLADES = 114449,
            SPELL_CLEAVE         = 15496,
        };

        enum iEvents
        {
            EVENT_DANCING_BLADES = 1,
            EVENT_CLEAVE         = 2,
        };

        struct npc_theramore_captain_korthokAI : public theramore_invader_typeAI
        {
            npc_theramore_captain_korthokAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DANCING_BLADES, urand(9.5 * IN_MILLISECONDS, 16.5 * IN_MILLISECONDS));
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
                        case EVENT_DANCING_BLADES:
                            DoCast(me, SPELL_DANCING_BLADES);
                            events.ScheduleEvent(EVENT_DANCING_BLADES, urand(12.5 * IN_MILLISECONDS, 16.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_CLEAVE:
                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_CLEAVE, false);

                            events.ScheduleEvent(EVENT_CLEAVE, urand(5 * IN_MILLISECONDS, 12.8 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_captain_korthokAI(creature);
        }
};

// Captain Mousson 65444
class npc_theramore_captain_mousson : public CreatureScript
{
    public:
        npc_theramore_captain_mousson() : CreatureScript("npc_theramore_captain_mousson") { }

        enum iSpells
        {
            SPELL_AURA_OF_ARCANE_HASTE   = 50182,
            SPELL_FROST_NOVA             = 79850,
            SPELL_FROST_BOLT             = 79858,
            SPELL_ICE_LANCE              = 79859,
            SPELL_SUMMON_WATER_ELEMENTAL = 17162,
            SPELL_BLIZZARD               = 79860,
        };

        enum iEvents
        {
            EVENT_FROST_BOLT     = 1,
            EVENT_BLIZZARD       = 2,
            EVENT_SECOND_ABILITY = 3,
        };

        struct npc_theramore_captain_moussonAI : public theramore_invader_typeAI
        {
            npc_theramore_captain_moussonAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                events.Reset();
                hasInterrupted = false;
                prevSpellId = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_FROST_BOLT, urand(2 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SECOND_ABILITY, urand(7 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLIZZARD, 15 * IN_MILLISECONDS);
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                if (int32 sDuration = spell->GetMaxDuration())
                {
                    events.RescheduleEvent(EVENT_FROST_BOLT, sDuration + 0.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SECOND_ABILITY, sDuration + 0.5 * IN_MILLISECONDS + 7 * IN_MILLISECONDS);
                }
                else
                {
                    events.RescheduleEvent(EVENT_FROST_BOLT, 4 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SECOND_ABILITY, 7 * IN_MILLISECONDS);
                }

                hasInterrupted = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
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
                        case EVENT_FROST_BOLT:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (hasInterrupted)
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                hasInterrupted = false;
                            }

                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_FROST_BOLT, false);

                            events.ScheduleEvent(EVENT_FROST_BOLT, urand(3 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_SECOND_ABILITY:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (hasInterrupted)
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                hasInterrupted = false;
                            }

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, GetAnySecondSpell(), false);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, GetAnySecondSpell(), false);

                            events.ScheduleEvent(EVENT_SECOND_ABILITY, urand(7 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                            break;
                        case EVENT_BLIZZARD:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BLIZZARD, false);
                            else if (Unit* target = me->GetVictim())
                                DoCast(target, SPELL_BLIZZARD, false);

                            events.ScheduleEvent(EVENT_BLIZZARD, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS));
                            break;
                    }
                }

                if (hasInterrupted)
                    DoMeleeAttackIfReady();
            }

            private:
                uint32 GetAnySecondSpell()
                {
                    std::vector<uint32> SecondComboSpellStorage = { SPELL_AURA_OF_ARCANE_HASTE, SPELL_SUMMON_WATER_ELEMENTAL, SPELL_FROST_BOLT, SPELL_ICE_LANCE };

                    // Remove From Temp container value, that was used before (real random)
                    if (prevSpellId)
                        SecondComboSpellStorage.erase(std::find(SecondComboSpellStorage.begin(), SecondComboSpellStorage.end(), prevSpellId));

                    // Select new spell from updated container
                    prevSpellId = Trinity::Containers::SelectRandomContainerElement(SecondComboSpellStorage);

                    return prevSpellId;
                }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_captain_moussonAI(creature);
        }
};

// Gashnul 64900
class npc_theramore_gashnul : public CreatureScript
{
    public:
        npc_theramore_gashnul() : CreatureScript("npc_theramore_gashnul") { }

        enum iSpells
        {
            SPELL_STORM_TOTEM            = 114569,
            SPELL_STORM_TOTEM_PERIODICAL = 114674,
            SPELL_LIGHTNING_SHIELD       = 131844,
        };

        enum iEvents
        {
            EVENT_STORM_TOTEM      = 1,
            EVENT_LIGHTNING_SHIELD = 2,
        };

        struct npc_theramore_gashnulAI : public theramore_invader_typeAI
        {
            npc_theramore_gashnulAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                me->GetMap()->SetWorldState(WORLDSTATE_KITE_FIGHT, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                //Talk(TALK_SPECIAL_1);
                events.ScheduleEvent(EVENT_STORM_TOTEM, urand(4 * IN_MILLISECONDS, 9.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_LIGHTNING_SHIELD, 1 * IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                summon->CastSpell(summon, SPELL_STORM_TOTEM_PERIODICAL, true);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_KITE_FIGHT, 0);
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
                        case EVENT_STORM_TOTEM:
                            DoCast(me, SPELL_STORM_TOTEM);
                            events.ScheduleEvent(EVENT_STORM_TOTEM, urand(10 * IN_MILLISECONDS, 15.5 * IN_MILLISECONDS));
                            break;
                        case EVENT_LIGHTNING_SHIELD:
                            DoCast(me, SPELL_LIGHTNING_SHIELD);
                            events.ScheduleEvent(EVENT_LIGHTNING_SHIELD, 8 * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_gashnulAI(creature);
        }
};

// Vicious Wyvern 64957
class npc_theramore_vicious_wyvern : public CreatureScript
{
    public:
        npc_theramore_vicious_wyvern() : CreatureScript("npc_theramore_vicious_wyvern") { }

        enum iSpells
        {
            SPELL_PRIDEWING_POISON = 81376,
        };

        enum iEvents
        {
            EVENT_POISON = 1,
        };

        struct npc_theramore_vicious_wyvernAI : public theramore_invader_typeAI
        {
            npc_theramore_vicious_wyvernAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_POISON, urand(5 * IN_MILLISECONDS, 7.5 * IN_MILLISECONDS));
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_POISON)
                    {
                        if (Unit* vict = me->GetVictim())
                            DoCast(vict, SPELL_PRIDEWING_POISON, false);

                        events.ScheduleEvent(EVENT_POISON, urand(5 * IN_MILLISECONDS, 7.5 * IN_MILLISECONDS));
                    }
                    break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_vicious_wyvernAI(creature);
        }
};

// Jaina Proudmoore 64727
class npc_theramore_jaina_proudmoore : public CreatureScript
{
    public:
        npc_theramore_jaina_proudmoore() : CreatureScript("npc_theramore_jaina_proudmoore") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                creature->AI()->DoAction(ACTION_JAINA_INTRO);

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            // don`t know real text
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Placeholder:", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(19183, creature->GetGUID());
            return true;
        }

        struct npc_theramore_jaina_proudmooreAI : public theramore_invader_typeAI
        {
            npc_theramore_jaina_proudmooreAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                instance = me->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                intro = false;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_jaina_proudmooreAI(creature);
        }
};

// Gatecrusher 64479
class npc_theramore_gatecrusher : public CreatureScript
{
    public:
        npc_theramore_gatecrusher() : CreatureScript("npc_theramore_gatecrusher") { }

        enum iSpells
        {
            SPELL_WAR_ENGINE = 131847,
            SPELL_WAR_ENGINE_SIGHTS = 114570,
            SPELL_BIG_BESSA_CANNON = 114565,
            SPELL_BEAM_TARGET_STATE = 114581,
        };

        enum iEvents
        {
            EVENT_SIGHTS = 1,
            EVENT_CANNON = 2,
        };

        struct npc_theramore_gatecrusherAI : public theramore_invader_typeAI
        {
            npc_theramore_gatecrusherAI(Creature* creature) : theramore_invader_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                targetGUID = ObjectGuid::Empty;
                me->GetMap()->SetWorldState(WORLDSTATE_NO_TANK_YOU, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                //Talk(TALK_INTRO);
                events.ScheduleEvent(EVENT_SIGHTS, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CANNON, urand(4.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                summon->CastSpell(summon, SPELL_BEAM_TARGET_STATE, true);
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
                        case EVENT_SIGHTS:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetNextFixateTarget()))
                            {
                                me->GetThreatManager().addThreat(target, 10000.0f);
                                me->CastSpell(target, SPELL_WAR_ENGINE_SIGHTS, true);

                                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                            }

                            events.ScheduleEvent(EVENT_SIGHTS, 10 * IN_MILLISECONDS);
                            break;
                        case EVENT_CANNON:
                            if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST, 0, 150.0f, true))
                                if (TempSummon* cannonTarget = me->SummonCreature(NPC_TANK_TARGET, *target, TEMPSUMMON_TIMED_DESPAWN, Milliseconds(4 * IN_MILLISECONDS)))
                                    DoCast(target, SPELL_BIG_BESSA_CANNON, false);

                            events.ScheduleEvent(EVENT_CANNON, urand(11.5 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            ObjectGuid GetNextFixateTarget()
            {
                std::list<Player*> pList;
                GetPlayerListInGrid(pList, me, 150.0f);

                if (targetGUID)
                    pList.remove_if([=](Player* target) { return target->GetGUID() == targetGUID; });

                if (!pList.empty())
                    return Trinity::Containers::SelectRandomContainerElement(pList)->GetGUID();

                return ObjectGuid::Empty;
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_gatecrusherAI(creature);
        }
};

// Standard of Theramore 214672
class go_standard_of_theramore : public GameObjectScript
{
    public:
        go_standard_of_theramore() : GameObjectScript("go_standard_of_theramore") { }

        enum iSpells
        {
            SPELL_STANDARD_OF_THERAMORE = 105690,
        };

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            player->CastSpell(player, SPELL_STANDARD_OF_THERAMORE, false);

            if (InstanceScript* instance = player->GetInstanceScript())
                instance->SetData(DATA_DESTROY_THE_DESTROYER, SPECIAL + 1);

            go->Delete();
            return true;
        }
};

// AreaTrigger 8419
class AreaTrigger_at_jaina_theramore : public AreaTriggerScript
{
    public:
        AreaTrigger_at_jaina_theramore() : AreaTriggerScript("AreaTrigger_at_jaina_theramore") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_DESTROY_THE_DESTROYER) != DONE)
                    return false;

                if (Creature* jaina = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_JAINA_PROUDMOORE)))
                    jaina->AI()->DoAction(ACTION_JAINA_LAST_STAND);
            }

            return false;
        }
};

void AddSC_fall_of_theramore_alliance_scenario()
{
    new npc_theramore_invader_base();
    new npc_theramore_sergeant_grud();
    new npc_theramore_roknah_grunt();
    new npc_theramore_airship_crewman();
    new npc_theramore_sky_captain_dazrip();
    new npc_theramore_roknah_felcaster();
    new npc_theramore_roknah_headhunter();
    new npc_theramore_roknah_rider();
    new npc_theramore_roknah_loa_singer();
    new npc_theramore_roknah_wave_caller();
    new npc_theramore_captain_korthok();
    new npc_theramore_captain_mousson();
    new npc_theramore_gashnul();
    new npc_theramore_vicious_wyvern();
    new npc_theramore_jaina_proudmoore();
    new npc_theramore_gatecrusher();
    new go_standard_of_theramore();
    new AreaTrigger_at_jaina_theramore();
}