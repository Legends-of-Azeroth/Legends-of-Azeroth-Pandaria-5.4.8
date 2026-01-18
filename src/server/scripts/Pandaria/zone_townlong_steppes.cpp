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
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "SpellScript.h"

enum eQuests
{
    QUEST_THE_TORCHES                      = 30787,
    QUEST_ORBISS_FADES                     = 30792,
    QUEST_FALLEN_SENTINELS                 = 30953,
    QUEST_THE_SEARCH_OF_RESTLESS_LENG      = 31688,
    QUEST_RANGER_RESCUE                    = 30774,
};

enum eCreatures
{
    NPC_WOUNDED_NIUZAO_SENTINEL = 61570,
    NPC_SENTINEL_HEALED_CREDIT  = 61569,
    NPC_SRATHIK_WAR_WAGON       = 61510,
    NPC_RESTLESS_LENG           = 65586,
    NPC_LONGYING_RANGER         = 60730,
    NPC_LONGYING_RANGER_HELPER  = 60763,
    NPC_SUNA_SILENTSTRIKE       = 60901,
    NPC_MIST_SHAMANS_TORCH      = 60698,
    NPC_SUNA_SILENTSTRIKE_2     = 61055,
};

enum eMisc
{
    OBJECTIVE_SIKTHIK_CAGES_SEARCHED       = 268906,
    QUEST_OBJECTIVE_LONGYIN_RANGER_RESCUED = 263418,
    QUEST_OBJECTIVE_FREE_LIN_SILENTSTRIKE  = 263419,
    GO_DRYWOOD_CAGE                        = 211511,
};

enum eLithIkSpells
{
    SPELL_BLADE_FURY       = 125370,
    SPELL_TORNADO          = 125398,
    SPELL_TORNADO_DMG      = 131693,
    SPELL_WINDSONG         = 125373,
};

enum eLithIkEvents
{
    EVENT_BLADE_FURY       = 1,
    EVENT_TORNADO          = 2,
    EVENT_WINDSONG         = 3,
};

class npc_lith_ik : public CreatureScript
{
    public:
        npc_lith_ik() : CreatureScript("npc_lith_ik") { }

        struct npc_lith_ikAI : public ScriptedAI
        {
            npc_lith_ikAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_TORNADO,       5000);
                events.ScheduleEvent(EVENT_BLADE_FURY,   25000);
                events.ScheduleEvent(EVENT_WINDSONG,     30000);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == 64267)
                {
                    summon->DespawnOrUnsummon(15000);
                    summon->AddAura(SPELL_TORNADO_DMG, summon);
                    summon->SetReactState(REACT_PASSIVE);
                    summon->GetMotionMaster()->MoveRandom(20.0f);
                }

            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TORNADO:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TORNADO, false);
                            events.ScheduleEvent(EVENT_TORNADO,      70000);
                            break;
                        case EVENT_BLADE_FURY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BLADE_FURY, false);
                            events.ScheduleEvent(EVENT_BLADE_FURY,      30000);
                            break;
                        case EVENT_WINDSONG:
                            me->CastSpell(me, SPELL_WINDSONG, false);
                            events.ScheduleEvent(EVENT_WINDSONG,      25000);
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
            return new npc_lith_ikAI(creature);
        }
};

enum eDarkwoodsFaerieSpells
{
    SPELL_DISGUISE         = 121308,
    SPELL_FAE_SPIRIT       = 122567,
    SPELL_NIGHT_SKY        = 123318,
    SPELL_STARSURGE        = 123330,
};

enum eDarkwoodsFaerieEvents
{
    EVENT_DISGUISE          = 1,
    EVENT_FAE_SPIRIT        = 2,
    EVENT_NIGHT_SKY         = 3,
    EVENT_STARSURGE         = 4,
};

class npc_darkwoods_faerie : public CreatureScript
{
    public:
        npc_darkwoods_faerie() : CreatureScript("npc_darkwoods_faerie") { }

        struct npc_darkwoods_faerieAI : public ScriptedAI
        {
            npc_darkwoods_faerieAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_DISGUISE,       5000);
                events.ScheduleEvent(EVENT_FAE_SPIRIT,    15000);
                events.ScheduleEvent(EVENT_NIGHT_SKY,     22000);
                events.ScheduleEvent(EVENT_STARSURGE,     30000);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == 64267)
                {
                    summon->DespawnOrUnsummon(15000);
                    summon->AddAura(SPELL_TORNADO_DMG, summon);
                    summon->SetReactState(REACT_PASSIVE);
                    summon->GetMotionMaster()->MoveRandom(20.0f);
                }

            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DISGUISE:
                            if (SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(me, SPELL_DISGUISE, false);
                            events.ScheduleEvent(EVENT_DISGUISE,      70000);
                            break;
                        case EVENT_FAE_SPIRIT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_FAE_SPIRIT, false);
                            events.ScheduleEvent(EVENT_FAE_SPIRIT,      15000);
                            break;
                        case EVENT_NIGHT_SKY:
                            me->CastSpell(me, SPELL_NIGHT_SKY, false);
                            events.ScheduleEvent(EVENT_NIGHT_SKY,      22000);
                            break;
                        case EVENT_STARSURGE:
                            me->CastSpell(me, SPELL_STARSURGE, false);
                            events.ScheduleEvent(EVENT_STARSURGE,      30000);
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
            return new npc_darkwoods_faerieAI(creature);
        }
};

enum eHeiFengSpells
{
    SPELL_DEEP_BREATH          = 125030,
    SPELL_SERPENT_SWEEP        = 125063,
    SPELL_SHADOW_DETONATION    = 124956,
};

enum eHeiFengEvents
{
    EVENT_DEEP_BREATH          = 1,
    EVENT_SERPENT_SWEEP        = 2,
    EVENT_SHADOW_DETONATION    = 3,
};

class npc_hei_feng : public CreatureScript
{
    public:
        npc_hei_feng() : CreatureScript("npc_hei_feng") { }

        struct npc_hei_fengAI : public ScriptedAI
        {
            npc_hei_fengAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_DEEP_BREATH,       5000);
                events.ScheduleEvent(EVENT_SERPENT_SWEEP,    15000);
                events.ScheduleEvent(EVENT_SHADOW_DETONATION,     22000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEEP_BREATH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_DEEP_BREATH, false);
                            events.ScheduleEvent(EVENT_DEEP_BREATH,      30000);
                            break;
                        case EVENT_SERPENT_SWEEP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SERPENT_SWEEP, false);
                            events.ScheduleEvent(EVENT_SERPENT_SWEEP,      15000);
                            break;
                        case EVENT_SHADOW_DETONATION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHADOW_DETONATION, false);
                            events.ScheduleEvent(EVENT_SHADOW_DETONATION,      22000);
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
            return new npc_hei_fengAI(creature);
        }
};

enum eCreatureSpells
{
    SPELL_CLAW_ATTACK          = 124839,
    SPELL_CORROSIVE_STING      = 124859,
    SPELL_DOUBLE_BLADE_LUNGE_1 = 125286,
    SPELL_DOUBLE_BLADE_LUNGE_2 = 125288,
    SPELL_TIMBERHUSK           = 122134,
    SPELL_BROKEN_SHELL         = 126035,
    SPELL_PROTECTIVE_SHELL     = 130728,
    SPELL_RANK_BITE            = 125353,
    SPELL_BLOOD_RAGE           = 124019,
    SPELL_GORED                = 124015,
    SPELL_SPITBALING           = 125358,
    SPELL_BELLY_FLOP           = 125384,
    SPELL_KNOCKBACK            = 118458,
    SPELL_CONSUMING_HATE       = 124683,
    SPELL_PURE_HATE            = 124690,
    SPELL_ARC_NOVA             = 149180,
    SPELL_LIGHTNING_BREATH     = 149178,
};

enum eCreatureEvents
{
    EVENT_CLAW_ATTACK = 1,
    EVENT_CORROSIVE_STING,
    EVENT_DOUBLE_BLADE_LUNGE,
    EVENT_TIMBERHUSK,
    EVENT_PROTECTIVE_SHELL,
    EVENT_RANK_BITE,
    EVENT_GORED,
    EVENT_BLOOD_RAGE,
    EVENT_SPITBALING,
    EVENT_BELLY_FLOP,
    EVENT_KNOCKBACK,
    EVENT_CONSUMING_HATE,
    EVENT_PURE_HATE,
    EVENT_ARC_NOVA,
    EVENT_LIGHTNING_BREATH,
};

// Seething Flashripper 61299
struct npc_seething_flashripper : public ScriptedAI
{
    npc_seething_flashripper(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid targetGUID;
    uint32 delay;

    void Reset() override
    {
        events.Reset();
        delay      = 0;
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CORROSIVE_STING, urand(3.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_CLAW_ATTACK, 8.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CORROSIVE_STING:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_CORROSIVE_STING);

                    events.ScheduleEvent(EVENT_CORROSIVE_STING, urand(10.5 * IN_MILLISECONDS, 13 * IN_MILLISECONDS));
                    break;
                case EVENT_CLAW_ATTACK:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_CLAW_ATTACK);

                        delay = 0;
                        me->m_Events.Schedule(delay += 2000, 3, [this]()
                        {
                            me->RemoveChanneledCast(targetGUID);
                        });
                    }
                    events.ScheduleEvent(EVENT_CLAW_ATTACK, urand(12 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Kor`thik Timberhusk 61355
struct npc_korthik_timberhusk : public ScriptedAI
{
    npc_korthik_timberhusk(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DOUBLE_BLADE_LUNGE, urand(4 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_TIMBERHUSK, 6 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DOUBLE_BLADE_LUNGE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, urand(0, 1) ? SPELL_DOUBLE_BLADE_LUNGE_1 : SPELL_DOUBLE_BLADE_LUNGE_2);

                    events.ScheduleEvent(EVENT_DOUBLE_BLADE_LUNGE, urand(6 * IN_MILLISECONDS, 17 * IN_MILLISECONDS));
                    break;
                case EVENT_TIMBERHUSK:
                    DoCast(me, SPELL_TIMBERHUSK);
                    events.ScheduleEvent(EVENT_TIMBERHUSK, urand(15 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Rankbite Ancient 66462
struct npc_rankbite_ancient : public ScriptedAI
{
    npc_rankbite_ancient(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_PROTECTIVE_SHELL, urand(12 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_RANK_BITE, urand(3.5 * IN_MILLISECONDS, 11.5 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PROTECTIVE_SHELL:
                    DoCast(me, SPELL_PROTECTIVE_SHELL);
                    events.ScheduleEvent(EVENT_PROTECTIVE_SHELL, urand(18 * IN_MILLISECONDS, 24 * IN_MILLISECONDS));
                    break;
                case EVENT_RANK_BITE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_RANK_BITE);

                    events.ScheduleEvent(EVENT_RANK_BITE, urand(3.5 * IN_MILLISECONDS, 11.5 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Deadtalker Crusher 62844
struct npc_deadtalker_crusher : public ScriptedAI
{
    npc_deadtalker_crusher(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_GORED, 7.5 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BLOOD_RAGE, 4.5 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_GORED:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_GORED);

                    events.ScheduleEvent(EVENT_GORED, 15 * IN_MILLISECONDS);
                    break;
                case EVENT_BLOOD_RAGE:
                    DoCast(me, SPELL_BLOOD_RAGE);
                    events.ScheduleEvent(EVENT_BLOOD_RAGE, urand(16 * IN_MILLISECONDS, 27 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Longshadow Mushan 61618
struct npc_longshadow_mushan : public ScriptedAI
{
    npc_longshadow_mushan(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid targetGUID;
    uint32 delay;

    void Reset() override
    {
        events.Reset();
        delay = 0;
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SPITBALING, urand(8.5 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_BELLY_FLOP, 10 * IN_MILLISECONDS);
    }

    void CastInterrupted(SpellInfo const* /*spell*/) override
    {
        me->RemoveChanneledCast(targetGUID);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BELLY_FLOP:
                    DoCast(me, SPELL_BELLY_FLOP);
                    events.ScheduleEvent(EVENT_BELLY_FLOP, urand(12.5 * IN_MILLISECONDS, 25 * IN_MILLISECONDS));
                    break;
                case EVENT_SPITBALING:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_SPITBALING);

                        delay = 0;
                        me->m_Events.Schedule(delay += 6000, 3, [this]()
                        {
                            me->RemoveChanneledCast(targetGUID);
                        });
                    }
                    events.ScheduleEvent(EVENT_SPITBALING, urand(12 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Seething Hatred 61054, 61092
struct npc_seething_hatred : public ScriptedAI
{
    npc_seething_hatred(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid targetGUID;
    uint32 delay;

    void Reset() override
    {
        events.Reset();
        delay = 0;
        targetGUID = ObjectGuid::Empty;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_PURE_HATE, urand(3.5 * IN_MILLISECONDS, 7 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_CONSUMING_HATE, 10 * IN_MILLISECONDS);
    }

    void CastInterrupted(SpellInfo const* /*spell*/) override
    {
        me->RemoveChanneledCast(targetGUID);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PURE_HATE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_PURE_HATE);
                
                    events.ScheduleEvent(EVENT_PURE_HATE, urand(8.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    events.ScheduleEvent(EVENT_KNOCKBACK, 2.5 * IN_MILLISECONDS);
                    break;
                case EVENT_CONSUMING_HATE:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_CONSUMING_HATE);
                
                        delay = 0;
                        me->m_Events.Schedule(delay += 6500, 4, [this]()
                        {
                            me->RemoveChanneledCast(targetGUID);
                        });
                    }
                    events.ScheduleEvent(EVENT_CONSUMING_HATE, urand(12 * IN_MILLISECONDS, 19 * IN_MILLISECONDS));
                    break;
                case EVENT_KNOCKBACK:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_KNOCKBACK);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Gnathus 66467
struct npc_gnathus : public customCreatureAI
{
    npc_gnathus(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ARC_NOVA, 12 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_LIGHTNING_BREATH, urand(4.5 * IN_MILLISECONDS, 9 * IN_MILLISECONDS));
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
            ExecuteTargetEvent(SPELL_LIGHTNING_BREATH, 10 * IN_MILLISECONDS, EVENT_LIGHTNING_BREATH, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_ARC_NOVA, urand(15 * IN_MILLISECONDS, 20 * IN_MILLISECONDS), EVENT_ARC_NOVA, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

enum eEshelonSpells
{
    SPELL_RAIN_DANCE    = 124860,
    SPELL_TORRENT       = 124935,
    SPELL_WATER_BOLT    = 124854
};

enum eEshelonEvents
{
    EVENT_RAIN_DANCE        = 1,
    EVENT_TORRENT           = 2,
    EVENT_WATER_BOLT        = 3
};

class npc_eshelon : public CreatureScript
{
    public:
        npc_eshelon() : CreatureScript("npc_eshelon") { }

        struct npc_eshelonAI : public ScriptedAI
        {
            npc_eshelonAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RAIN_DANCE,   5000);
                events.ScheduleEvent(EVENT_TORRENT,     15000);
                events.ScheduleEvent(EVENT_WATER_BOLT,  25000);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                summon->DespawnOrUnsummon(12000);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);


                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RAIN_DANCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RAIN_DANCE, false);
                            events.ScheduleEvent(EVENT_RAIN_DANCE,       5000);
                            break;
                        case EVENT_TORRENT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TORRENT, false);
                            events.ScheduleEvent(EVENT_TORRENT, 15000);
                            break;
                        case EVENT_WATER_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_WATER_BOLT, false);
                            events.ScheduleEvent(EVENT_WATER_BOLT, 25000);
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
            return new npc_eshelonAI(creature);
        }
};

class go_sikthik_cage : public GameObjectScript
{
    public:
        go_sikthik_cage() : GameObjectScript("go_sikthik_cage") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            // If counter is 7 (script is called before counting) max is 8
            if (player->GetQuestObjectiveCounter(OBJECTIVE_SIKTHIK_CAGES_SEARCHED) == 7 && player->GetQuestStatus(QUEST_THE_SEARCH_OF_RESTLESS_LENG) == QUEST_STATUS_INCOMPLETE)
            {
                if (Creature* leng = player->SummonCreature(NPC_RESTLESS_LENG, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 20000))
                {
                    ObjectGuid playerGuid = player->GetGUID();

                    uint32 delay = 0;
                    leng->m_Events.Schedule(delay += 2000, [leng]()             { leng->AI()->Talk(0); });
                    leng->m_Events.Schedule(delay += 2000, [leng]()             { leng->SetStandState(UNIT_STAND_STATE_STAND); });
                    leng->m_Events.Schedule(delay += 3000, [leng]()             { leng->AI()->Talk(1); });
                    leng->m_Events.Schedule(delay += 6000, [leng, playerGuid]() { if (Player* player = ObjectAccessor::GetPlayer(*leng, playerGuid)) leng->AI()->Talk(2, player); });
                }
                player->KilledMonsterCredit(NPC_RESTLESS_LENG);
            }

            ObjectGuid goGuid = go->GetGUID();
            uint32 delay = 0;
            player->m_Events.Schedule(delay += 8000, [player, goGuid]()
            {
                if (GameObject* cage = ObjectAccessor::GetGameObject(*player, goGuid))
                    cage->ForcedDespawn();
            });

            return false;
        }
};

// Back on Their Feet quest
class spell_item_cintron_infused_bandage : public SpellScriptLoader
{
    public:
        spell_item_cintron_infused_bandage() : SpellScriptLoader("spell_item_cintron_infused_bandage") { }

        class spell_item_cintron_infused_bandage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_item_cintron_infused_bandage_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    if (auto target = GetTarget()->ToCreature())
                        if (target->GetEntry() == 61692)
                            if (auto player = GetCaster()->ToPlayer())
                            {
                                target->HandleEmoteCommand(EMOTE_STATE_STAND);
                                target->DespawnOrUnsummon(4000);
                                player->KilledMonsterCredit(61692);
                            }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_item_cintron_infused_bandage_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_item_cintron_infused_bandage_AuraScript();
        }
};

// Dust to Dust quest
class spell_item_shado_pan_torch : public SpellScriptLoader
{
public:
    spell_item_shado_pan_torch() : SpellScriptLoader("spell_item_shado_pan_torch") { }

    class spell_item_shado_pan_torch_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_item_shado_pan_torch_AuraScript);

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                if (auto target = GetTarget()->ToCreature())
                    if (target->GetEntry() == 60925)
                    {
                        target->RemoveAurasDueToSpell(106246);
                        target->DespawnOrUnsummon();
                    }
        }

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            auto caster = GetCaster();
            auto target = GetTarget();
            if (!caster || !target)
                return;

            if (target->GetTypeId() == TYPEID_UNIT && target->GetEntry() == 60925 && !target->HasAura(106246))
            {
                if (auto player = caster->ToPlayer())
                    player->KilledMonsterCredit(target->GetEntry());
                target->CastSpell(target, 106246, true);
            }
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_item_shado_pan_torch_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            AfterEffectApply += AuraEffectApplyFn(spell_item_shado_pan_torch_AuraScript::OnApply, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_item_shado_pan_torch_AuraScript();
    }
};

// Quest: Ranger Rescue (30774)
enum RangerRescue
{
    SPELL_SUMMON_LONGYING_RANGER            = 117670,

    EVENT_SUNA_KNEEL_TALK                   = 1,
    EVENT_SUNA_TALK                         = 2,
    EVENT_SUNA_TALK_2                       = 3,
    EVENT_SUNA_STAND                        = 4
};

class go_drywood_cage : public GameObjectScript
{
    public:
        go_drywood_cage() : GameObjectScript("go_drywood_cage") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->GetQuestStatus(QUEST_RANGER_RESCUE) == QUEST_STATUS_INCOMPLETE
                && player->GetQuestObjectiveCounter(QUEST_OBJECTIVE_LONGYIN_RANGER_RESCUED) < 4)
            {
                if (auto ranger = GetClosestCreatureWithEntry(player, NPC_LONGYING_RANGER, 10.f))
                {
                    go->SetGoState(GO_STATE_ACTIVE);

                    player->QuestObjectiveSatisfy(QUEST_OBJECTIVE_LONGYIN_RANGER_RESCUED, 1);
                    player->CastSpell(player, SPELL_SUMMON_LONGYING_RANGER, true);

                    if (auto rangerHelper = GetClosestCreatureWithEntry(player, NPC_LONGYING_RANGER_HELPER, 10.f))
                        rangerHelper->AI()->Talk(0, player);

                    ranger->DisappearAndDie();
                }
            }

            return true;
        }
};

class npc_longying_ranger : public CreatureScript
{
    public:
        npc_longying_ranger() : CreatureScript("npc_longying_ranger") { }

        struct npc_longying_ranger_AI : public ScriptedAI
        {
            npc_longying_ranger_AI(Creature* creature) : ScriptedAI(creature) { }

            void JustAppeared() override
            {
                if (GameObject* cage = GetClosestGameObjectWithEntry(me, GO_DRYWOOD_CAGE, 10.f))
                    cage->SetGoState(GO_STATE_READY);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_longying_ranger_AI(creature);
        }
};

class npc_lin_silentstrike : public CreatureScript
{
    public:
        npc_lin_silentstrike() : CreatureScript("npc_lin_silentstrike") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (player->GetQuestStatus(QUEST_RANGER_RESCUE) == QUEST_STATUS_INCOMPLETE
                && !player->GetQuestObjectiveCounter(QUEST_OBJECTIVE_FREE_LIN_SILENTSTRIKE))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Examine the body.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->QuestObjectiveSatisfy(QUEST_OBJECTIVE_FREE_LIN_SILENTSTRIKE, 1);
                creature->AI()->SetGUID(player->GetGUID(), 0);
                player->CLOSE_GOSSIP_MENU();
            }

            return false;
        }

        struct npc_lin_silentstrikeAI : public ScriptedAI
        {
            npc_lin_silentstrikeAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid sunaGUID;
            EventMap events;

            void Reset() override
            {
                sunaGUID = ObjectGuid::Empty;

                events.Reset();
                events.ScheduleEvent(EVENT_SUNA_KNEEL_TALK, 5000);
                events.ScheduleEvent(EVENT_SUNA_TALK, 13000);
                events.ScheduleEvent(EVENT_SUNA_TALK_2, 21000);
                events.ScheduleEvent(EVENT_SUNA_STAND, 29000);
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                if (auto suna = me->SummonCreature(NPC_SUNA_SILENTSTRIKE, 2659.59f, 3268.618f, 425.33f, 5.56f, TEMPSUMMON_TIMED_DESPAWN, 34000))
                {
                    sunaGUID = suna->GetGUID();

                    suna->AI()->Talk(0);
                    suna->GetMotionMaster()->MovePoint(1, 2674.21f, 3257.52f, 426.31f);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    Creature* suna = me->GetCreature(*me, sunaGUID);
                    if (!suna)
                    {
                        Reset();
                        return;
                    }

                    switch (eventId)
                    {
                        case EVENT_SUNA_KNEEL_TALK:
                        {
                            suna->SetStandState(UNIT_STAND_STATE_KNEEL);
                            suna->AI()->Talk(1);
                            break;
                        }
                        case EVENT_SUNA_TALK:
                        {
                            suna->AI()->Talk(2);
                            break;
                        }
                        case EVENT_SUNA_TALK_2:
                        {
                            suna->AI()->Talk(3);
                            break;
                        }
                        case EVENT_SUNA_STAND:
                        {
                            suna->SetStandState(UNIT_STAND_STATE_STAND);
                            Reset();
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_lin_silentstrikeAI(creature);
        }
};

// Osul Mist-Shaman 60697
class npc_osul_mist_shaman : public CreatureScript
{
    public:
        npc_osul_mist_shaman() : CreatureScript("npc_osul_mist_shaman") { }

        enum eSpells
        {
            SPELL_DESECRATION      = 117464, // cosmetic blue hands + cosmetic casting
            SPELL_CAPACITOR_TOTEM  = 125720,
            SPELL_LIGHTNING_BOLT   = 79085,
        };

        enum eEvents
        {
            EVENT_LIGHTNING_BOLT   = 1,
            EVENT_CAPACITOR_TOTEM  = 2,
        };

        struct npc_osul_mist_shamanAI : public ScriptedAI
        {
            npc_osul_mist_shamanAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid TorchGUID;
            EventMap events;

            void Reset() override
            {
                Unit* Torch = ObjectAccessor::GetUnit(*me, TorchGUID);

                if (!Torch)
                {
                    Position pos = me->GetRandomNearPosition(4.5f);

                    if (TempSummon* Torch = me->SummonCreature(NPC_MIST_SHAMANS_TORCH, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), urand(0, 2 * M_PI), TEMPSUMMON_MANUAL_DESPAWN))
                        TorchGUID = Torch->GetGUID();
                }

                me->CastSpell(me, SPELL_DESECRATION, false);
                events.Reset();
            }

            void JustEngagedWith(Unit* who) override
            {
                events.ScheduleEvent(EVENT_LIGHTNING_BOLT, urand(1500, 3000));
                events.ScheduleEvent(EVENT_CAPACITOR_TOTEM, urand(4000, 6000));
            }

            void JustDied(Unit* killer) override 
            {
                if (Unit* Torch = ObjectAccessor::GetUnit(*me, TorchGUID))
                {
                    if (killer && killer->ToPlayer() && killer->ToPlayer()->GetQuestStatus(QUEST_THE_TORCHES) == QUEST_STATUS_INCOMPLETE)
                        Torch->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);

                    Torch->ToCreature()->DespawnOrUnsummon(10 * IN_MILLISECONDS);
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
                        case EVENT_LIGHTNING_BOLT:
                            if (Unit* victim = me->GetVictim())
                                me->CastSpell(victim, SPELL_LIGHTNING_BOLT, false);

                            events.ScheduleEvent(EVENT_LIGHTNING_BOLT, urand(1500, 3000));
                            break;
                        case EVENT_CAPACITOR_TOTEM:
                            if (Unit* victim = me->GetVictim())
                                me->CastSpell(victim, SPELL_CAPACITOR_TOTEM, false);

                            events.ScheduleEvent(EVENT_CAPACITOR_TOTEM, 12 * IN_MILLISECONDS);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_osul_mist_shamanAI(creature);
        }
};

// Gather Steam 117901
class spell_gather_steam : public SpellScript
{
    PrepareSpellScript(spell_gather_steam);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* obj) { return obj->ToPlayer()->GetQuestStatus(QUEST_ORBISS_FADES) != QUEST_STATUS_INCOMPLETE; });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->ToCreature())
                caster->ToCreature()->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_gather_steam::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_gather_steam::HandleDummy, EFFECT_0, SPELL_EFFECT_KILL_CREDIT2);
    }
};

// Suna Silentstrike 61055
class npc_suna_silentstrike : public CreatureScript
{
    public:
        npc_suna_silentstrike() : CreatureScript("npc_suna_silentstrike") { }

        enum Spells
        {
            SPELL_INNER_PEACE          = 128814,
            SPELL_CONSUMED_BY_HATRED_1 = 128813, // only visual
            SPELL_CONSUMED_BY_HATRED_2 = 127470,
            SPELL_ROUNDHOUSE_KICK      = 128305,
            SPELL_UNLEASH_HATE         = 127474,
        };

        enum Yells
        {
            SAY_SUNA_ON_SPELLHIT = 0,
            SAY_SUNA_AGGRO       = 1,
            SAY_SUNA_END         = 2,
        };

        enum Events
        {
            EVENT_ROUNDHOUSE_KICK = 1,
            EVENT_UNLEASH_HATE    = 2,
        };

        struct npc_suna_silentstrikeAI : public ScriptedAI
        {
            npc_suna_silentstrikeAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;
            bool sayOnHit;

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell) override
            {
                if (spell->Id == SPELL_INNER_PEACE && !sayOnHit)
                {
                    Talk(SAY_SUNA_ON_SPELLHIT);
                    sayOnHit = true;
                }
            }

            void DamageTaken(Unit* who, uint32& damage) override
            {
                if (damage >= me->GetHealth())
                {
                    damage = 0;

                    events.Reset();
                    me->RemoveAura(SPELL_CONSUMED_BY_HATRED_2);
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);

                    Talk(SAY_SUNA_END);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    if (who->GetTypeId() == TYPEID_PLAYER)
                        who->ToPlayer()->KilledMonsterCredit(NPC_SUNA_SILENTSTRIKE_2);
                    else if (Player* player = who->GetCharmerOrOwnerPlayerOrPlayerItself())
                        player->KilledMonsterCredit(NPC_SUNA_SILENTSTRIKE_2);

                    Creature* suna = me;

                    uint32 delay = 0;
                    suna->m_Events.Schedule(delay += 5000, [suna]() { suna->SetStandState(UNIT_STAND_STATE_STAND); });
                    suna->m_Events.Schedule(delay += 5000, [suna]() { suna->setDeathState(JUST_DIED);              });
                    suna->m_Events.Schedule(delay += 5000, [suna]() { suna->DespawnOrUnsummon();                   });
                }
            }

            void Reset() override
            {
                sayOnHit = false;

                me->AddAura(SPELL_CONSUMED_BY_HATRED_1, me);
                me->RemoveAura(SPELL_CONSUMED_BY_HATRED_2);
            }

            void JustEngagedWith(Unit* who) override
            {
                Talk(SAY_SUNA_AGGRO);
                me->CastSpell(me, SPELL_CONSUMED_BY_HATRED_2);
                events.ScheduleEvent(EVENT_ROUNDHOUSE_KICK, urand(8000, 10000));
                events.ScheduleEvent(EVENT_UNLEASH_HATE, 20000);
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
                        case EVENT_ROUNDHOUSE_KICK:
                            if (Unit* victim = me->GetVictim())
                                me->CastSpell(victim, SPELL_ROUNDHOUSE_KICK, false);
                            events.ScheduleEvent(EVENT_ROUNDHOUSE_KICK, urand(10000, 12000));
                            break;
                        case EVENT_UNLEASH_HATE:
                            if (Unit* victim = me->GetVictim())
                                me->CastSpell(me, SPELL_UNLEASH_HATE, false);
                            events.ScheduleEvent(EVENT_UNLEASH_HATE, 20000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_suna_silentstrikeAI(creature);
        }
};

// Wounded Niuzao Sentinel 61570
class npc_wounded_niuzao_sentinel : public CreatureScript
{
    public:
        npc_wounded_niuzao_sentinel() : CreatureScript("npc_wounded_niuzao_sentinel") { }

        struct npc_wounded_niuzao_sentinelAI : public ScriptedAI
        {
            npc_wounded_niuzao_sentinelAI(Creature* creature) : ScriptedAI(creature) { }

            bool HasHealed;

            void Reset() override
            {
                me->setRegeneratingHealth(false);
                me->SetHealth((uint32)me->GetMaxHealth() * 0.1f);
                me->SetStandState(UNIT_STAND_STATE_DEAD);
                HasHealed = false;
            }

            void HealReceived(Unit* healer, uint32& heal) override
            {
                if (me->HealthAbovePct(50) && !HasHealed)
                {
                    HasHealed = true;

                    if (Player* pItr = healer->ToPlayer())
                        if (pItr->GetQuestStatus(QUEST_FALLEN_SENTINELS) == QUEST_STATUS_INCOMPLETE)
                            pItr->KilledMonsterCredit(NPC_SENTINEL_HEALED_CREDIT);

                    me->SetStandState(UNIT_STAND_STATE_STAND);

                    uint32 delay = 0;
                    me->m_Events.Schedule(delay += 1500, 1, [this]()
                    {
                        me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                        me->DespawnOrUnsummon(3 * IN_MILLISECONDS);
                    });
                }
            }

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_wounded_niuzao_sentinelAI(creature);
        }
};

// spells data is missed
const std::map <uint32, Position> invCaskType =
{
    { 61363, { 2525.40f, 3896.17f, 282.68f, 0.86f } }, // weapons
    { 61364, { 2488.04f, 3828.55f, 284.02f, 2.63f } }, // Eggs
};

// Gunpowder Cask 61362
struct npc_townlong_gunpowder_cask : public ScriptedAI
{
    npc_townlong_gunpowder_cask(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid summonerGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        summonerGUID = summoner->GetGUID();

        scheduler
            .Schedule(Milliseconds(3000), [this](TaskContext context)
        {
            if (Player* owner = ObjectAccessor::GetPlayer(*me, summonerGUID))
            {
                for (auto&& itr : invCaskType)
                    if (me->GetExactDist2d(itr.second.GetPositionX(), itr.second.GetPositionY()) < 8.5f)
                        owner->KilledMonsterCredit(itr.first);
            }

            DoCast(me, 105380); // wrong spellId (or not?)
            me->DespawnOrUnsummon();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

class spell_q30959 : public SpellScript
{
    PrepareSpellScript(spell_q30959);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* obj) { return obj->GetEntry() != NPC_SRATHIK_WAR_WAGON; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_q30959::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

// Protective Shell 130728
class spell_protective_shell : public AuraScript
{
    PrepareAuraScript(spell_protective_shell);

    void OnAuraEffectRemove(AuraEffect const* aurEff, AuraEffectHandleModes mode)
    {
        if (Creature* owner = GetOwner()->ToCreature())
            owner->CastSpell(owner, SPELL_BROKEN_SHELL, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_protective_shell::OnAuraEffectRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

class cond_burn_mantid_corpse : public ConditionScript
{
    public:
        cond_burn_mantid_corpse() : ConditionScript("cond_burn_mantid_corpse") { }

        bool OnConditionCheck(const Condition* cond, ConditionSourceInfo& source) override
        {
            Player* player = source.mConditionTargets[0]->ToPlayer();
            if (!player)
                return false;

            Unit* unit = ObjectAccessor::GetUnit(*player, player->GetTarget());
            if (!unit)
                return false;

            if (unit->GetEntry() != 62128)
                return false;

            if (unit->IsAlive())
                return false;

            if (unit->HasAura(122523))
                return false;

            return true;
        }
};

enum LonTheBull
{
    SAY_LONBULL_AGGRO       = 0,

    EVENT_BELLOWING_RAGE    = 1,
    EVENT_EMPOWERING_FLAMES = 2,
    EVENT_HOOF_STOMP        = 3,
    EVENT_RUSHING_CHARGE    = 4,

    SPELL_BELLOWING_RAGE    = 124297,
    SPELL_EMPOWERING_FLAMES = 130388,
    SPELL_HOOF_STOMP        = 124289,
    SPELL_RUSHING_CHARGE    = 124302,
};

// Lon the Bull - 50333
struct npc_lon_the_bull : public ScriptedAI
{
    npc_lon_the_bull(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_LONBULL_AGGRO);

        events.ScheduleEvent(EVENT_BELLOWING_RAGE, 20000);
        events.ScheduleEvent(EVENT_EMPOWERING_FLAMES, 15000);
        events.ScheduleEvent(EVENT_HOOF_STOMP, 10000);
        events.ScheduleEvent(EVENT_RUSHING_CHARGE, 10000);
    }

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12000);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BELLOWING_RAGE:
                    DoCastVictim(SPELL_BELLOWING_RAGE, false);
                    events.ScheduleEvent(EVENT_BELLOWING_RAGE, 30000);
                    break;
                case EVENT_EMPOWERING_FLAMES:
                    DoCastVictim(SPELL_EMPOWERING_FLAMES, false);
                    events.ScheduleEvent(EVENT_EMPOWERING_FLAMES, 15000);
                    break;
                case EVENT_HOOF_STOMP:
                    DoCastVictim(SPELL_HOOF_STOMP, false);
                    events.ScheduleEvent(EVENT_HOOF_STOMP, 15000);
                    break;
                case EVENT_RUSHING_CHARGE:
                    DoCastVictim(SPELL_RUSHING_CHARGE, false);
                    events.ScheduleEvent(EVENT_RUSHING_CHARGE, 10000);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

enum Norlaxx
{
    SAY_NORLAXX_AGGRO = 0,

    EVENT_SHADOWBOLT  = 1,
    EVENT_VOIDCLOUD   = 2,

    SPELL_SHADOWBOLT  = 125212,
    SPELL_VOIDCLOUD   = 125241,
};

// Norlaxx - 50344
struct npc_norlaxx : public ScriptedAI
{
    npc_norlaxx(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_NORLAXX_AGGRO);

        events.ScheduleEvent(EVENT_SHADOWBOLT, 5000);
        events.ScheduleEvent(EVENT_VOIDCLOUD, 15000);
    };

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12000);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SHADOWBOLT:
                    DoCastVictim(SPELL_SHADOWBOLT);
                    events.ScheduleEvent(EVENT_SHADOWBOLT, 5000);
                    break;
                case EVENT_VOIDCLOUD:
                    DoCastVictim(SPELL_VOIDCLOUD);
                    events.ScheduleEvent(EVENT_VOIDCLOUD, 15000);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

enum SilitrissSharpener
{
    SAY_SILTRISS_AGGRO   = 0,

    EVENT_GRAPPLING_HOOK = 1,
    EVENT_VANISH         = 2,
    EVENT_SMOKED_BLADE   = 3,
    EVENT_VICIOUS_REND   = 4,

    SPELL_GRAPPLING_HOOK = 125623,
    SPELL_VANISH         = 125632,
    SPELL_SMOKED_BLADE   = 125633,
    SPELL_VICIOUS_REND   = 125624,
};

// Siltriss the Sharpener - 50791
struct npc_siltriss_sharpener : public ScriptedAI
{
    npc_siltriss_sharpener(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();

        events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 17000);
        events.ScheduleEvent(EVENT_VANISH, 12000);
        events.ScheduleEvent(EVENT_VICIOUS_REND, 7000);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_SILTRISS_AGGRO);
    };

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12000);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_GRAPPLING_HOOK:
                    DoCastVictim(SPELL_GRAPPLING_HOOK);
                    events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 5000);
                    break;
                case EVENT_VANISH:
                    DoCastVictim(SPELL_VANISH);
                    events.ScheduleEvent(EVENT_VANISH, 30000);
                    events.ScheduleEvent(EVENT_SMOKED_BLADE, urand(0, 8000));
                    break;
                case EVENT_SMOKED_BLADE:
                    DoCastVictim(SPELL_SMOKED_BLADE);
                    events.ScheduleEvent(EVENT_SMOKED_BLADE, 31500);
                case EVENT_VICIOUS_REND:
                    DoCastVictim(SPELL_VICIOUS_REND);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

enum YulWildpaw
{
    SAY_YUL_AGGRO             = 0,

    EVENT_CHI_BURST           = 1,
    EVENT_HEALING_MIST        = 2,
    EVENT_SPINNING_CRANE_KICK = 3,

    SPELL_CHI_BURST           = 125817,
    SPELL_HEALING_MIST        = 125802,
    SPELL_SPINNING_CRANE_KICK = 125799,
};

// Yul Wildpaw - 50820
struct npc_yul_wildpaw : public ScriptedAI
{
    npc_yul_wildpaw(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }
    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_YUL_AGGRO);

        events.ScheduleEvent(EVENT_CHI_BURST, 10000);
        events.ScheduleEvent(EVENT_HEALING_MIST, 15000);
        events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 5000);
    };

    void JustSummoned(Creature* summon) override
    {
        summon->DespawnOrUnsummon(12000);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHI_BURST:
                    DoCastVictim(SPELL_CHI_BURST);
                    events.ScheduleEvent(EVENT_CHI_BURST, 5000);
                    break;
                case EVENT_HEALING_MIST:
                    DoCastVictim(SPELL_HEALING_MIST);
                    events.ScheduleEvent(EVENT_HEALING_MIST, 35000);
                    break;
                case EVENT_SPINNING_CRANE_KICK:
                    DoCastVictim(SPELL_SPINNING_CRANE_KICK);
                    events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 15000);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Shado-Pan Spike Trap Eff 119391
class spell_townlong_shado_pan_spike_trap_eff : public SpellScript
{
    PrepareSpellScript(spell_townlong_shado_pan_spike_trap_eff);

    void HandleEffectHitTarget(SpellEffIndex eff_idx)
    {
        if (Player* target = GetHitPlayer())
            PreventHitDamage();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_townlong_shado_pan_spike_trap_eff::HandleEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

void AddSC_townlong_steppes()
{
    // Rare mobs
    new npc_lith_ik();
    new npc_eshelon();
    new creature_script<npc_lon_the_bull>("npc_lon_the_bull");
    new creature_script<npc_norlaxx>("npc_norlaxx");
    new creature_script<npc_siltriss_sharpener>("npc_siltriss_sharpener");
    new creature_script<npc_yul_wildpaw>("npc_yul_wildpaw");
    // Elite mobs
    new npc_darkwoods_faerie();
    new npc_hei_feng();
    // Standard Mobs
    new creature_script<npc_seething_flashripper>("npc_seething_flashripper");
    new creature_script<npc_korthik_timberhusk>("npc_korthik_timberhusk");
    new creature_script<npc_rankbite_ancient>("npc_rankbite_ancient");
    new creature_script<npc_deadtalker_crusher>("npc_deadtalker_crusher");
    new creature_script<npc_longshadow_mushan>("npc_longshadow_mushan");
    new creature_script<npc_seething_hatred>("npc_seething_hatred");
    new creature_script<npc_gnathus>("npc_gnathus");
    // Quests
    new go_sikthik_cage();
    new spell_item_cintron_infused_bandage();
    new spell_item_shado_pan_torch();
    new go_drywood_cage();
    new npc_longying_ranger();
    new npc_lin_silentstrike();
    new npc_osul_mist_shaman();
    new spell_script<spell_gather_steam>("spell_gather_steam");
    new npc_suna_silentstrike();
    new npc_wounded_niuzao_sentinel();
    new creature_script<npc_townlong_gunpowder_cask>("npc_townlong_gunpowder_cask");
    new spell_script<spell_q30959>("spell_q30959");
    new aura_script<spell_protective_shell>("spell_protective_shell");
    new cond_burn_mantid_corpse();
    new spell_script<spell_townlong_shado_pan_spike_trap_eff>("spell_townlong_shado_pan_spike_trap_eff");
}
