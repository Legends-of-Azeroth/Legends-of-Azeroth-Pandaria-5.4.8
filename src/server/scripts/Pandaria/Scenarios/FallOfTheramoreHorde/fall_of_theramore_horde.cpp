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
#include "fall_of_theramore_horde.h"

// Misc Spells
enum eSpells
{
    SPELL_RIG_THIS_POWDER_BARREL = 37338,
    SPELL_RIG_TO_EXPLODE         = 111260,
    SPELL_SUMMON_SABOTAGED_TANK  = 111657,

    // Big Bessa
    SPELL_BIG_BESSA_CANNON_SHOT  = 114568,

    // Thalen Songweaver
    SPELL_PORTAL_ORGRIMMAR = 111633,
};

enum eTalks
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
    TALK_SPECIAL_3,
    TALK_SPECIAL_4,
    TALK_SPECIAL_5,
};

// Blastmaster Sparkfuse 58765
class npc_blastmaster_sparkfuse : public CreatureScript
{
    public:
        npc_blastmaster_sparkfuse() : CreatureScript("npc_blastmaster_sparkfuse") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                if (InstanceScript* instance = creature->GetInstanceScript())
                    instance->SetData(DATA_THE_BLASTMASTER, DONE);

                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

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

        struct npc_blastmaster_sparkfuseAI : public ScriptedAI
        {
            npc_blastmaster_sparkfuseAI(Creature* creature) : ScriptedAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blastmaster_sparkfuseAI(creature);
        }
};

struct theramore_guard_typeAI : public ScriptedAI
{
    theramore_guard_typeAI(Creature* creature) : ScriptedAI(creature), summons(me) { }

    InstanceScript* instance;
    SummonList summons;
    EventMap events, nonCombatEvents;
    bool hasInterrupted, intro;
    uint32 prevSpellId, delay;
    ObjectGuid targetGUID;

    void DoAction(int32 actionId) override
    {
        switch (actionId)
        {
            case ACTION_INTRO_SONGWEAVER:
            {
                if (intro)
                    break;

                intro = true;
                Talk(TALK_INTRO);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                delay = 0;
                me->m_Events.Schedule(delay += 3000, 1, [this]()
                {
                    Talk(TALK_SPECIAL_1);
                });
                break;
            }
            case ACTION_SONGWEAVER_CHAIN_BREAK:
            {
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->RemoveAllAuras();
                Talk(TALK_SPECIAL_2);

                if (InstanceScript* _instance = me->GetInstanceScript())
                    _instance->SetData(DATA_FLY_SPY, DONE);

                delay = 0;
                me->m_Events.Schedule(delay += 4000, 1, [this]()
                {
                    Talk(TALK_SPECIAL_3);
                    me->HandleEmoteStateCommand(EMOTE_STATE_CUSTOM_SPELL_01);
                });

                me->m_Events.Schedule(delay += 7500, 1, [this]()
                {
                    Talk(TALK_SPECIAL_4);
                });

                me->m_Events.Schedule(delay += 3000, 1, [this]()
                {
                    Talk(TALK_SPECIAL_5);
                    DoCast(me, SPELL_PORTAL_ORGRIMMAR, false);
                    me->DespawnOrUnsummon(5 * IN_MILLISECONDS);
                });
                break;
            }
            case ACTION_ACTIVATE_BY_REGION:
                me->SetVisible(true);

                switch (me->GetEntry())
                {
                    case NPC_THERAMORE_ARCANIST:
                    case NPC_THERAMORE_FAITHFUL:
                    case NPC_THERAMORE_FOOTMAN:
                    case NPC_THERAMORE_OFFICER:
                    case NPC_THERAMORE_MARKSMAN:
                    case NPC_THERAMORE_GRYPHON:
                    case NPC_KNIGHT_OF_THERAMORE:
                    case NPC_HEDRIC_EVENCANE:
                    case NPC_BIG_BESSA:
                        me->SetFaction(1077); // sniff data
                        break;
                    case NPC_BALDRUC:
                        Talk(TALK_INTRO);
                        me->SetFaction(1077); // sniff data
                        break;
                }
                break;
        }
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

        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_THERAMORE_ARCANIST, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_THERAMORE_FOOTMAN, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_THERAMORE_MARKSMAN, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_THERAMORE_FAITHFUL, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KNIGHT_OF_THERAMORE, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }
};

class npc_theramore_guard_base : public CreatureScript
{
    public:
        npc_theramore_guard_base() : CreatureScript("npc_theramore_guard_base") { }

        struct npc_theramore_guard_baseAI : public theramore_guard_typeAI
        {
            npc_theramore_guard_baseAI(Creature* creature) : theramore_guard_typeAI(creature) { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_guard_baseAI(creature);
        }
};

// Theramore Footman 58612, 58913
class npc_theramore_footman : public CreatureScript
{
    public:
        npc_theramore_footman() : CreatureScript("npc_theramore_footman") { }

        enum iSpells
        {
            SPELL_CLEAVE = 15496,
        };

        enum iEvents
        {
            EVENT_CLEAVE = 1,
        };

        struct npc_theramore_footmanAI : public theramore_guard_typeAI
        {
            npc_theramore_footmanAI(Creature* creature) : theramore_guard_typeAI(creature) { }

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
            return new npc_theramore_footmanAI(creature);
        }
};

// Theramore Arcanist 59596
class npc_theramore_arcanist : public CreatureScript
{
    public:
        npc_theramore_arcanist() : CreatureScript("npc_theramore_arcanist") { }

        enum iSpells
        {
            SPELL_AURA_OF_ARCANE_HASTE = 50182,
            SPELL_SLOW                 = 79880,
            SPELL_POLYMORPH            = 13323,
            SPELL_ARCANE_BLAST         = 114683,
            SPELL_ARCANE_EXPLOSION     = 33860,
        };

        enum iEvents
        {
            EVENT_ARCANE_BLAST   = 1,
            EVENT_SECOND_ABILITY = 2,
        };

        struct npc_theramore_arcanistAI : public theramore_guard_typeAI
        {
            npc_theramore_arcanistAI(Creature* creature) : theramore_guard_typeAI(creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                events.Reset();
                hasInterrupted = false;
                prevSpellId = 0;
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(2 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SECOND_ABILITY, urand(7 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
            }

            void CastInterrupted(SpellInfo const* spell) override
            {
                if (int32 sDuration = spell->GetMaxDuration())
                {
                    events.RescheduleEvent(EVENT_ARCANE_BLAST, sDuration + 0.5 * IN_MILLISECONDS);
                    events.RescheduleEvent(EVENT_SECOND_ABILITY, sDuration + 0.5 * IN_MILLISECONDS + 7 * IN_MILLISECONDS);
                }
                else
                {
                    events.RescheduleEvent(EVENT_ARCANE_BLAST, 4 * IN_MILLISECONDS);
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
                        case EVENT_ARCANE_BLAST:
                            if (HandleRescheduleEventsIfCastAny(eventId))
                                break;

                            if (hasInterrupted)
                            {
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                                hasInterrupted = false;
                            }

                            if (Unit* vict = me->GetVictim())
                                DoCast(vict, SPELL_ARCANE_BLAST, false);

                            events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(3 * IN_MILLISECONDS, 3.5 * IN_MILLISECONDS));
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
                    std::vector<uint32> SecondComboSpellStorage = { SPELL_AURA_OF_ARCANE_HASTE, SPELL_SLOW, SPELL_POLYMORPH, SPELL_ARCANE_EXPLOSION };

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
            return new npc_theramore_arcanistAI(creature);
        }
};

// Theramore Marksman 59317
class npc_theramore_marksman : public CreatureScript
{
    public:
        npc_theramore_marksman() : CreatureScript("npc_theramore_marksman") { }

        enum iSpells
        {
            SPELL_MULTY_SHOT = 102411,
        };

        enum iEvents
        {
            EVENT_SHOT = 1,
        };

        struct npc_theramore_marksmanAI : public theramore_guard_typeAI
        {
            npc_theramore_marksmanAI(Creature* creature) : theramore_guard_typeAI(creature) { }

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
            return new npc_theramore_marksmanAI(creature);
        }
};

// Knight of Theramore 59654
class npc_knight_of_theramore : public CreatureScript
{
    public:
        npc_knight_of_theramore() : CreatureScript("npc_knight_of_theramore") { }

        enum iSpells
        {
            SPELL_HEAL_ANOTHER = 33910,
            SPELL_CHARGE       = 22911,
        };

        enum iEvents
        {
            EVENT_HEAL_ANOTHER = 1,
            EVENT_CHARGE       = 2,
        };

        struct npc_knight_of_theramoreAI : public theramore_guard_typeAI
        {
            npc_knight_of_theramoreAI(Creature* creature) : theramore_guard_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                events.ScheduleEvent(EVENT_CHARGE, 1 * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEAL_ANOTHER, urand(8 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
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
                        case EVENT_HEAL_ANOTHER:
                            if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                                DoCast(target, SPELL_HEAL_ANOTHER);

                            events.ScheduleEvent(EVENT_HEAL_ANOTHER, urand(9.5 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
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
            return new npc_knight_of_theramoreAI(creature);
        }
};

// Theramore Faithful 59595
class npc_theramore_faithful : public CreatureScript
{
    public:
        npc_theramore_faithful() : CreatureScript("npc_theramore_faithful") { }

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

        struct npc_theramore_faithfulAI : public theramore_guard_typeAI
        {
            npc_theramore_faithfulAI(Creature* creature) : theramore_guard_typeAI(creature) { }

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
            return new npc_theramore_faithfulAI(creature);
        }
};

// Baldruc 58777
class npc_baldruc : public CreatureScript
{
    public:
        npc_baldruc() : CreatureScript("npc_baldruc") { }

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

        struct npc_baldrucAI : public theramore_guard_typeAI
        {
            npc_baldrucAI(Creature* creature) : theramore_guard_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                me->GetMap()->SetWorldState(WORLDSTATE_KITE_FIGHT, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_SPECIAL_1);
                events.ScheduleEvent(EVENT_STORM_TOTEM, urand(4 * IN_MILLISECONDS, 9.5 * IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_LIGHTNING_SHIELD, 1 * IN_MILLISECONDS);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
            {
                me->GetMap()->SetWorldState(WORLDSTATE_KITE_FIGHT, 0);
            }

            void JustSummoned(Creature* summon) override
            {
                summons.Summon(summon);

                summon->CastSpell(summon, SPELL_STORM_TOTEM_PERIODICAL, true);
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
            return new npc_baldrucAI(creature);
        }
};

// Big Bessa 58787
class npc_big_bessa : public CreatureScript
{
    public:
        npc_big_bessa() : CreatureScript("npc_big_bessa") { }

        enum iSpells
        {
            SPELL_WAR_ENGINE            = 131847,
            SPELL_WAR_ENGINE_SIGHTS     = 114570,
            SPELL_BIG_BESSA_CANNON      = 114565,
            SPELL_BEAM_TARGET_STATE     = 114581,
        };

        enum iEvents
        {
            EVENT_SIGHTS = 1,
            EVENT_CANNON = 2,
        };

        struct npc_big_bessaAI : public theramore_guard_typeAI
        {
            npc_big_bessaAI(Creature* creature) : theramore_guard_typeAI(creature) { }

            void Reset() override
            {
                events.Reset();
                summons.DespawnAll();
                targetGUID = ObjectGuid::Empty;
                me->GetMap()->SetWorldState(WORLDSTATE_NO_KITE_YOU, 1);
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                Talk(TALK_INTRO);
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
            return new npc_big_bessaAI(creature);
        }
};

// Captain Drok 58870
class npc_theramore_captain_drok : public CreatureScript
{
    public:
        npc_theramore_captain_drok() : CreatureScript("npc_theramore_captain_drok") { }

        struct npc_theramore_captain_drokAI : public ScriptedAI
        {
            npc_theramore_captain_drokAI(Creature* creature) : ScriptedAI(creature) { }

            bool IntroEnable;
            uint32 delay;

            void Reset() override
            {
                IntroEnable = false;
            }

            void DoAction(int32 actionId) override
            {
                if (actionId == ACTION_INTRO_SHIP && !IntroEnable)
                {
                    IntroEnable = true;
                    Talk(TALK_INTRO);

                    delay = 0;
                    me->m_Events.Schedule(delay += 2500, 1, [this]()
                    {
                        Talk(TALK_SPECIAL_1);
                    });

                    me->m_Events.Schedule(delay += 3500, 1, [this]()
                    {
                        Talk(TALK_SPECIAL_2);
                    });
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_captain_drokAI(creature);
        }
};

// Talen SongWeaver 58816
class npc_theramore_thalen_songweaver : public CreatureScript
{
    public:
        npc_theramore_thalen_songweaver() : CreatureScript("npc_theramore_thalen_songweaver") { }

        enum iItems
        {
            ITEM_PRISON_KEY = 79261,
        };

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                if (!player->HasItemCount(ITEM_PRISON_KEY))
                    return false;

                player->DestroyItemCount(ITEM_PRISON_KEY, 1, true);

                creature->AI()->DoAction(ACTION_SONGWEAVER_CHAIN_BREAK);
            }

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

        struct npc_theramore_thalen_songweaverAI : public theramore_guard_typeAI
        {
            npc_theramore_thalen_songweaverAI(Creature* creature) : theramore_guard_typeAI(creature) { }

            void Reset() override
            {
                intro = false;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_theramore_thalen_songweaverAI(creature);
        }
};

// Portal : Orgrimmar 215852 . Also on click send cinematic
class go_theramore_horde_portal : public GameObjectScript
{
    public:
        go_theramore_horde_portal() : GameObjectScript("go_theramore_horde_portal") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            player->CastSpell(player, SPELL_THERAMORE_EXPLOSION_SCENE, false);
            return true;
        }

    private:
        uint32 delay;
};

// Rig to explode 111260
class spell_theramore_rig_to_explode : public SpellScript
{
    PrepareSpellScript(spell_theramore_rig_to_explode);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            switch (target->GetEntry())
            {
                case NPC_RIG_THIS_POWDER_BARREL:
                    if (!target->HasAura(SPELL_RIG_THIS_POWDER_BARREL))
                        break;

                    if (InstanceScript* instance = target->GetInstanceScript())
                    {
                        target->RemoveAurasDueToSpell(SPELL_RIG_THIS_POWDER_BARREL);
                        instance->SetData(DATA_TO_THE_WATERLINE, instance->GetData(DATA_TO_THE_WATERLINE) + 1);
                        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                    break;
                case NPC_UNMANNED_TANK:
                    if (!target->IsAlive())
                        break;

                    if (InstanceScript* instance = target->GetInstanceScript())
                    {
                        instance->SetData(DATA_TANKS_FOR_NOTHING, IN_PROGRESS);
                        target->CastSpell(target, SPELL_SUMMON_SABOTAGED_TANK, true);
                        
                        if (target->ToCreature())
                            target->ToCreature()->DespawnOrUnsummon();
                    }
                    break;
                case NPC_LEAKED_OIL_DRUM:
                    if (!target->HasAura(SPELL_RIG_THIS_POWDER_BARREL))
                        break;

                    if (InstanceScript* instance = target->GetInstanceScript())
                    {
                        target->RemoveAurasDueToSpell(SPELL_RIG_THIS_POWDER_BARREL);
                        instance->SetData(DATA_SWEEP_THE_HARBOR, SPECIAL);
                        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                    break;
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_theramore_rig_to_explode::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// War Engine 131847
class spell_theramore_war_engine : public AuraScript
{
    PrepareAuraScript(spell_theramore_war_engine);

    void OnAuraEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Unit* owner = GetOwner()->ToUnit())
        {
            owner->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR, true);
            owner->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_FEAR_2, true);
            owner->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STUN, true);
            owner->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ROOT, true);
            owner->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_PULL, true);;
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_theramore_war_engine::OnAuraEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Big Bessa Cannon 114565
class spell_big_bessa_cannon : public SpellScript
{
    PrepareSpellScript(spell_big_bessa_cannon);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            target->CastSpell(target, SPELL_BIG_BESSA_CANNON_SHOT, true);
            
            if (target->ToCreature())
                target->ToCreature()->DespawnOrUnsummon();
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_big_bessa_cannon::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Big Bessa Cannon Shot 114568
class spell_big_bessa_cannon_shot : public SpellScript
{
    PrepareSpellScript(spell_big_bessa_cannon_shot);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Creature* caster = GetCaster()->ToCreature())
            if (Player* target = GetHitPlayer())
                caster->GetMap()->SetWorldState(WORLDSTATE_NO_KITE_YOU, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_big_bessa_cannon_shot::HandleEffectHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// AreaTrigger 8461
class AreaTrigger_at_theramore_songweaver_prison : public AreaTriggerScript
{
    public:
        AreaTrigger_at_theramore_songweaver_prison() : AreaTriggerScript("AreaTrigger_at_theramore_songweaver_prison") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (instance->GetData(DATA_TANKS_FOR_NOTHING) != DONE)
                    return false;

                if (Creature* Talen = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_THALEN_SONGWEAVER)))
                    Talen->AI()->DoAction(ACTION_INTRO_SONGWEAVER);
            }

            return false;
        }
};

// AreaTrigger 7442
class AreaTrigger_at_captain_drok_ship : public AreaTriggerScript
{
    public:
        AreaTrigger_at_captain_drok_ship() : AreaTriggerScript("AreaTrigger_at_captain_drok_ship") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (Creature* Drok = ObjectAccessor::GetCreature(*player, instance->GetGuidData(NPC_CAPTAIN_DROK)))
                    Drok->AI()->DoAction(ACTION_INTRO_SHIP);
            }

            return false;
        }
};

// Scene ID 46, Package 190
class scene_theramore_explosion_horde : public SceneScript
{
    public:
        scene_theramore_explosion_horde() : SceneScript("scene_theramore_explosion_horde") { }

        void OnSceneComplete(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            CreditScene(player);
        }

        void OnSceneCancel(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            CreditScene(player);
        }

        void CreditScene(Player* player)
        {
            if (player->GetTeam() == HORDE)
                player->TeleportTo(1, 2032.06f, -4385.29f, 98.61f, 0.49f, TELE_TO_NOT_UNSUMMON_PET);
        }
};

void AddSC_fall_of_theramore_horde_scenario()
{
    new npc_blastmaster_sparkfuse();
    new npc_theramore_guard_base();
    new npc_theramore_footman();
    new npc_theramore_arcanist();
    new npc_theramore_marksman();
    new npc_knight_of_theramore();
    new npc_theramore_faithful();
    new npc_baldruc();
    new npc_big_bessa();
    new npc_theramore_captain_drok();
    new npc_theramore_thalen_songweaver();
    new go_theramore_horde_portal();
    new spell_script<spell_theramore_rig_to_explode>("spell_theramore_rig_to_explode");
    new aura_script<spell_theramore_war_engine>("spell_theramore_war_engine");
    new spell_script<spell_big_bessa_cannon>("spell_big_bessa_cannon");
    new spell_script<spell_big_bessa_cannon_shot>("spell_big_bessa_cannon_shot");
    new AreaTrigger_at_theramore_songweaver_prison();
    new AreaTrigger_at_captain_drok_ship();
    new scene_theramore_explosion_horde();
}