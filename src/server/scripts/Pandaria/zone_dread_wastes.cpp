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

enum eSpells
{
    SPELL_BANANARANG            = 125311,
    SPELL_GOING_BANANAS         = 125363,
    SPELL_TOSS_FILTH            = 125365,
    SPELL_UNSTABLE_SERUM        = 127373,
    SPELL_KLAXXI_RESONANCE      = 129204,
    SPELL_KLAXXI_SCENE_SHEKZEER = 129388,
    SPELL_KLAXXI_EMPRESS_GAMBIT = 131196, // summon Malik and Zorlok
    SPELL_ZORLOK_SHOUT          = 131201,
};

enum eEvents
{
    EVENT_BANANARANG        = 1,
    EVENT_GOING_BANANAS     = 2,
    EVENT_TOSS_FILTH        = 3,
    EVENT_UNSTABLE_SERUM    = 4
};

enum eQuests
{
    QUEST_OVERTHRONE       = 31782,
    QUEST_EMPRESS_GAMBIT   = 31959,
    QUEST_SHADOW_OF_EMPIRE = 31612,
};

enum eCreatures
{
    NPC_IMPERIAL_VIZIER_ZORLOK_QUEST = 66791,
    NPC_MALIC_QUEST                  = 66776,
    NPC_MALIC_QUEST_1                = 66797,
    NPC_KILRUK_QUEST                 = 66800,
    NPC_KORTHIK_WARCALLER            = 62754,
    NPC_IK_THIK_AMBERSTINGER         = 63728,
};

enum eYells
{
    SAY_INTRO,
    SAY_SPECIAL_1,
    SAY_SPECIAL_2,
    SAY_SPECIAL_3,
    SAY_SPECIAL_4,
    SAY_SPECIAL_5,
    SAY_SPECIAL_6,
    SAY_SPECIAL_7,
    SAY_SPECIAL_8,
    SAY_SPECIAL_9,
    SAY_SPECIAL_10,
    SAY_SPECIAL_11,
    SAY_SPECIAL_12,
    SAY_SPECIAL_13,
};

const Position KilrukChamberPath[10]
{
    { 145.86f, 3198.47f, 214.76f, 6.07f },
    { 216.70f, 3156.54f, 215.39f, 5.74f },
    { 216.40f, 3108.28f, 210.16f, 4.71f },
    { 210.26f, 3060.68f, 183.00f, 4.57f },
    { 186.37f, 3051.77f, 178.75f, 3.49f },
    { 170.02f, 3074.82f, 175.89f, 2.18f },
    { 182.91f, 3137.80f, 153.30f, 1.37f },
    { 184.66f, 3204.41f, 150.87f, 1.48f },
    { 193.52f, 3210.55f, 151.01f, 0.61f },
    { 185.20f, 3252.78f, 151.89f, 1.78f },
};

const Position MalikStairsPath[3]
{
    { 181.20f, 3988.32f, 232.11f, 1.70f },
    { 179.22f, 4000.25f, 239.90f, 1.79f },
    { 178.23f, 4005.13f, 239.96f, 1.71f },
};

class npc_ik_ik_the_nimble : public CreatureScript
{
    public:
        npc_ik_ik_the_nimble() : CreatureScript("npc_ik_ik_the_nimble") { }

        struct npc_ik_ik_the_nimbleAI : public ScriptedAI
        {
            npc_ik_ik_the_nimbleAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_BANANARANG,     10000);
                events.ScheduleEvent(EVENT_GOING_BANANAS,  17000);
                events.ScheduleEvent(EVENT_TOSS_FILTH,     22000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM, 30000);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                summon->DespawnOrUnsummon(15000);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
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
                        case EVENT_BANANARANG:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BANANARANG, false);
                            events.ScheduleEvent(EVENT_BANANARANG,      80000);
                            break;
                        case EVENT_GOING_BANANAS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_GOING_BANANAS, false);
                            events.ScheduleEvent(EVENT_GOING_BANANAS, 70000);
                            break;
                        case EVENT_TOSS_FILTH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TOSS_FILTH, false);
                            events.ScheduleEvent(EVENT_TOSS_FILTH,      40000);
                            break;
                        case EVENT_UNSTABLE_SERUM:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM, 20000);
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
            return new npc_ik_ik_the_nimbleAI(creature);
        }
};

enum eAdjunctKreeZotSpells
{
    SPELL_SHA_SPIT                = 126790,
    SPELL_SHA_SPIT_2              = 126792,
    SPELL_SLICING_STRIKE          = 126799,
    SPELL_WRATH_OF_THE_EMPRESS    = 128395,
};

enum eAdjunctKreeZotEvents
{
    EVENT_SHA_SPIT                = 1,
    EVENT_SHA_SPIT_2              = 2,
    EVENT_SLICING_STRIKE          = 3,
    EVENT_WRATH_OF_THE_EMPRESS    = 4,
};

class npc_adjunct_kree_zot : public CreatureScript
{
    public:
        npc_adjunct_kree_zot() : CreatureScript("npc_adjunct_kree_zot") { }

        struct npc_adjunct_kree_zotAI : public ScriptedAI
        {
            npc_adjunct_kree_zotAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_SHA_SPIT,             10000);
                events.ScheduleEvent(EVENT_SHA_SPIT_2,           20000);
                events.ScheduleEvent(EVENT_SLICING_STRIKE,       25000);
                events.ScheduleEvent(EVENT_WRATH_OF_THE_EMPRESS, 35000);
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
                        case EVENT_SHA_SPIT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHA_SPIT, false);
                            events.ScheduleEvent(EVENT_SHA_SPIT,      10000);
                            break;
                        case EVENT_SHA_SPIT_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHA_SPIT_2, false);
                            events.ScheduleEvent(EVENT_SHA_SPIT_2,      25000);
                            break;
                        case EVENT_SLICING_STRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SLICING_STRIKE, false);
                            events.ScheduleEvent(EVENT_SLICING_STRIKE,      20000);
                            break;
                        case EVENT_WRATH_OF_THE_EMPRESS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_WRATH_OF_THE_EMPRESS, false);
                            events.ScheduleEvent(EVENT_WRATH_OF_THE_EMPRESS,      30000);
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
            return new npc_adjunct_kree_zotAI(creature);
        }
};

enum eDreadFearbringerEvents
{
    EVENT_UNSTABLE_SERUM_2                = 1,
};

class npc_dread_fearbringer : public CreatureScript
{
    public:
        npc_dread_fearbringer() : CreatureScript("npc_dread_fearbringer") { }

        struct npc_dread_fearbringerAI : public ScriptedAI
        {
            npc_dread_fearbringerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_2,     10000);
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
                        case EVENT_UNSTABLE_SERUM_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_2,      10000);
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
            return new npc_dread_fearbringerAI(creature);
        }
};

enum eDreadKunchongSpells
{
    SPELL_DREAD_SLASH                   = 128022,
    SPELL_SEEK_AND_DESTROY              = 127357,
    SPELL_SEEK_AND_DESTROY_2            = 127358,
    SPELL_SEEK_AND_DESTROY_3            = 127359,
    SPELL_SEEK_AND_DESTROY_POWER        = 127859,
    SPELL_SEEK_AND_DESTROY_WISDOM       = 127860,
    SPELL_SEEK_AND_DESTROY_VELOCITY     = 127861,

};

enum eDreadKunchongEvents
{
    EVENT_DREAD_SLASH                   = 1,
    EVENT_SEEK_AND_DESTROY              = 2,
    EVENT_SEEK_AND_DESTROY_2            = 3,
    EVENT_SEEK_AND_DESTROY_3            = 4,
    EVENT_UNSTABLE_SERUM_3              = 5,
};

class npc_dread_kunchong : public CreatureScript
{
    public:
        npc_dread_kunchong() : CreatureScript("npc_dread_kunchong") { }

        struct npc_dread_kunchongAI : public ScriptedAI
        {
            npc_dread_kunchongAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_DREAD_SLASH,     25000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_3, 22000);
            }

            void JustEngagedWith(Unit* who) override
            {
                Player* player = who->ToPlayer();

                if (!player)
                    return;

                switch (player->GetTalentSpecialization())
                {
                    case SPEC_DEATH_KNIGHT_BLOOD:
                    case SPEC_DEATH_KNIGHT_FROST:
                    case SPEC_DEATH_KNIGHT_UNHOLY:
                    case SPEC_PALADIN_PROTECTION:
                    case SPEC_PALADIN_RETRIBUTION:
                    case SPEC_WARRIOR_ARMS:
                    case SPEC_WARRIOR_FURY:
                    case SPEC_WARRIOR_PROTECTION:
                    case SPEC_MONK_BREWMASTER:
                    default:
                        DoCast(me, SPELL_SEEK_AND_DESTROY,   true);
                        break;
                    case SPEC_MAGE_ARCANE:
                    case SPEC_MAGE_FIRE:
                    case SPEC_MAGE_FROST:
                    case SPEC_PALADIN_HOLY:
                    case SPEC_DRUID_BALANCE:
                    case SPEC_PRIEST_DISCIPLINE:
                    case SPEC_PRIEST_HOLY:
                    case SPEC_PRIEST_SHADOW:
                    case SPEC_DRUID_RESTORATION:
                    case SPEC_SHAMAN_ELEMENTAL:
                    case SPEC_SHAMAN_RESTORATION:
                    case SPEC_WARLOCK_AFFLICTION:
                    case SPEC_WARLOCK_DEMONOLOGY:
                    case SPEC_WARLOCK_DESTRUCTION:
                    case SPEC_MONK_MISTWEAVER:
                        DoCast(me, SPELL_SEEK_AND_DESTROY_2, true);
                        break;
                    case SPEC_DRUID_FERAL:
                    case SPEC_DRUID_GUARDIAN:
                    case SPEC_HUNTER_BEAST_MASTERY:
                    case SPEC_HUNTER_MARKSMANSHIP:
                    case SPEC_HUNTER_SURVIVAL:
                    case SPEC_ROGUE_ASSASSINATION:
                    case SPEC_ROGUE_COMBAT:
                    case SPEC_ROGUE_SUBTLETY:
                    case SPEC_SHAMAN_ENHANCEMENT:
                    case SPEC_MONK_WINDWALKER:
                        DoCast(me, SPELL_SEEK_AND_DESTROY_3, true);
                        break;
                }
            }

            void JustDied(Unit* killer) override
            {
                if (me->HasAura(SPELL_SEEK_AND_DESTROY))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_POWER, true);

                if (me->HasAura(SPELL_SEEK_AND_DESTROY_2))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_WISDOM, true);

                if (me->HasAura(SPELL_SEEK_AND_DESTROY_3))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_VELOCITY, true);
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
                        case EVENT_DREAD_SLASH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_DREAD_SLASH, false);
                            events.ScheduleEvent(EVENT_DREAD_SLASH,      25000);
                            break;
                        case EVENT_UNSTABLE_SERUM_3:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_3, 30000);
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
            return new npc_dread_kunchongAI(creature);
        }
};

enum eDreadMatriarchSpells
{
    SPELL_POUNCE                  = 119579,
    SPELL_POUNCE_DAMAGE           = 119578,
    SPELL_RAVAGE                  = 126799,
};

enum eDreadMatriarchEvents
{
    EVENT_POUNCE                  = 1,
    EVENT_RAVAGE                  = 2,
    EVENT_UNSTABLE_SERUM_4        = 3,
};

class npc_dread_matriarch : public CreatureScript
{
    public:
        npc_dread_matriarch() : CreatureScript("npc_dread_matriarch") { }

        struct npc_dread_matriarchAI : public ScriptedAI
        {
            npc_dread_matriarchAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_POUNCE,           17000);
                events.ScheduleEvent(EVENT_RAVAGE,           23000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_4, 8000);
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
                        case EVENT_POUNCE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                            {
                                me->CastSpell(target, SPELL_POUNCE,        false);
                                me->CastSpell(target, SPELL_POUNCE_DAMAGE, false);
                            }
                            events.ScheduleEvent(EVENT_POUNCE,      20000);
                            break;
                        case EVENT_RAVAGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RAVAGE, false);
                            events.ScheduleEvent(EVENT_RAVAGE,      27000);
                            break;
                        case EVENT_UNSTABLE_SERUM_4:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_4,     8000);
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
            return new npc_dread_matriarchAI(creature);
        }
};

enum eElderSpindlegrinSpells
{
    SPELL_CHARGE                 = 32323,
    SPELL_MONSTROUS_UNDERBITE    = 127905,
    SPELL_REND                   = 13445,
};

enum eElderSpindlegrinEvents
{
    EVENT_CHARGE_2                = 1,
    EVENT_MONSTROUS_UNDERBITE     = 2,
    EVENT_REND                    = 3,
    EVENT_UNSTABLE_SERUM_5        = 4,
};

class npc_elder_spindlegrin : public CreatureScript
{
    public:
        npc_elder_spindlegrin() : CreatureScript("npc_elder_spindlegrin") { }

        struct npc_elder_spindlegrinAI : public ScriptedAI
        {
            npc_elder_spindlegrinAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_CHARGE_2,             17000);
                events.ScheduleEvent(EVENT_MONSTROUS_UNDERBITE,  23000);
                events.ScheduleEvent(EVENT_REND,                  8000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_5,     12000);
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
                        case EVENT_CHARGE_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_CHARGE,        false);
                            events.ScheduleEvent(EVENT_CHARGE_2, 20000);
                            break;
                        case EVENT_MONSTROUS_UNDERBITE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_MONSTROUS_UNDERBITE, false);
                            events.ScheduleEvent(EVENT_MONSTROUS_UNDERBITE, 40000);
                            break;
                        case EVENT_REND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_REND, false);
                            events.ScheduleEvent(EVENT_REND, 30000);
                            break;
                        case EVENT_UNSTABLE_SERUM_5:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_5, 22000);
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
            return new npc_elder_spindlegrinAI(creature);
        }

};

enum eHulkingKorThikFleshrenderSpells
{
    SPELL_PINCH_LIMB             = 124800,
    SPELL_VICIOUS_STRIKES        = 124296,
};

enum eHulkingKorThikFleshrenderEvents
{
    EVENT_PINCH_LIMB             = 1,
    EVENT_VICIOUS_STRIKES        = 2,
    EVENT_UNSTABLE_SERUM_6       = 3,
};

class npc_hulking_kor_thik_fleshrender : public CreatureScript
{
    public:
        npc_hulking_kor_thik_fleshrender() : CreatureScript("npc_hulking_kor_thik_fleshrender") { }

        struct npc_hulking_kor_thik_fleshrenderAI : public ScriptedAI
        {
            npc_hulking_kor_thik_fleshrenderAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_PINCH_LIMB,             17000);
                events.ScheduleEvent(EVENT_VICIOUS_STRIKES,        23000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_6,       12000);
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
                        case EVENT_PINCH_LIMB:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_PINCH_LIMB, false);
                            events.ScheduleEvent(EVENT_PINCH_LIMB, 20000);
                            break;
                        case EVENT_VICIOUS_STRIKES:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_VICIOUS_STRIKES, false);
                            events.ScheduleEvent(EVENT_VICIOUS_STRIKES, 40000);
                            break;
                        case EVENT_UNSTABLE_SERUM_6:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_6, 22000);
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
            return new npc_hulking_kor_thik_fleshrenderAI(creature);
        }
};

enum eIkThikAmberstingerSpells
{
    SPELL_CLAW              = 127943,
    SPELL_SONIC_SHOCK       = 124734,
    SPELL_TAIL_STRIKE       = 127935,
};

enum eIkThikAmberstingerEvents
{
    EVENT_CLAW               = 1,
    EVENT_TAIL_STRIKE        = 3,
    EVENT_UNSTABLE_SERUM_7   = 4,
};

class npc_ik_thik_amberstinger : public CreatureScript
{
    public:
        npc_ik_thik_amberstinger() : CreatureScript("npc_ik_thik_amberstinger") { }

        struct npc_ik_thik_amberstingerAI : public ScriptedAI
        {
            npc_ik_thik_amberstingerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_CLAW,                 17000);
                events.ScheduleEvent(EVENT_TAIL_STRIKE,          20000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_7,     15000);

                // Reset health on evade
                me->SetMaxHealth(787900);
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
                        case EVENT_CLAW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_CLAW,        false);
                            events.ScheduleEvent(EVENT_CLAW, 20000);
                            break;
                        case EVENT_TAIL_STRIKE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TAIL_STRIKE, false);
                            events.ScheduleEvent(EVENT_TAIL_STRIKE, 22000);
                            break;
                        case EVENT_UNSTABLE_SERUM_7:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_7, 22000);
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
            return new npc_ik_thik_amberstingerAI(creature);
        }
};

enum eIkThikKunchongEvents
{
    EVENT_DREAD_SLASH_2         = 1,
    EVENT_UNSTABLE_SERUM_8      = 2,
};

class npc_ik_thik_kunchong : public CreatureScript
{
    public:
        npc_ik_thik_kunchong() : CreatureScript("npc_ik_thik_kunchong") { }

        struct npc_ik_thik_kunchongAI : public ScriptedAI
        {
            npc_ik_thik_kunchongAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_DREAD_SLASH_2,    17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_8, 10000);
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
                        case EVENT_DREAD_SLASH_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_DREAD_SLASH,        false);
                            events.ScheduleEvent(EVENT_DREAD_SLASH_2, 10000);
                            break;
                        case EVENT_UNSTABLE_SERUM_8:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_8, 15000);
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
            return new npc_ik_thik_kunchongAI(creature);
        }
};

enum eItThikTerrorclawEvents
{
    EVENT_TAIL_STRIKE_2                   = 1,
    EVENT_CLAW_2                          = 2,
    EVENT_UNSTABLE_SERUM_9                = 3,
};

class npc_ik_thik_terrorclaw : public CreatureScript
{
    public:
        npc_ik_thik_terrorclaw() : CreatureScript("npc_ik_thik_terrorclaw") { }

        struct npc_ik_thik_terrorclawAI : public ScriptedAI
        {
            npc_ik_thik_terrorclawAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_TAIL_STRIKE,       5000);
                events.ScheduleEvent(EVENT_CLAW,             17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_9, 22000);
            }

            void JustEngagedWith(Unit* who) override
            {
                Player* player = who->ToPlayer();

                if (!player)
                    return;

                switch (player->GetTalentSpecialization())
                {
                    case SPEC_DEATH_KNIGHT_BLOOD:
                    case SPEC_DEATH_KNIGHT_FROST:
                    case SPEC_DEATH_KNIGHT_UNHOLY:
                    case SPEC_PALADIN_PROTECTION:
                    case SPEC_PALADIN_RETRIBUTION:
                    case SPEC_WARRIOR_ARMS:
                    case SPEC_WARRIOR_FURY:
                    case SPEC_WARRIOR_PROTECTION:
                    case SPEC_MONK_BREWMASTER:
                    default:
                        DoCast(me, SPELL_SEEK_AND_DESTROY,   true);
                        break;
                    case SPEC_MAGE_ARCANE:
                    case SPEC_MAGE_FIRE:
                    case SPEC_MAGE_FROST:
                    case SPEC_PALADIN_HOLY:
                    case SPEC_DRUID_BALANCE:
                    case SPEC_PRIEST_DISCIPLINE:
                    case SPEC_PRIEST_HOLY:
                    case SPEC_PRIEST_SHADOW:
                    case SPEC_DRUID_RESTORATION:
                    case SPEC_SHAMAN_ELEMENTAL:
                    case SPEC_SHAMAN_RESTORATION:
                    case SPEC_WARLOCK_AFFLICTION:
                    case SPEC_WARLOCK_DEMONOLOGY:
                    case SPEC_WARLOCK_DESTRUCTION:
                    case SPEC_MONK_MISTWEAVER:
                        DoCast(me, SPELL_SEEK_AND_DESTROY_2, true);
                        break;
                    case SPEC_DRUID_FERAL:
                    case SPEC_DRUID_GUARDIAN:
                    case SPEC_HUNTER_BEAST_MASTERY:
                    case SPEC_HUNTER_MARKSMANSHIP:
                    case SPEC_HUNTER_SURVIVAL:
                    case SPEC_ROGUE_ASSASSINATION:
                    case SPEC_ROGUE_COMBAT:
                    case SPEC_ROGUE_SUBTLETY:
                    case SPEC_SHAMAN_ENHANCEMENT:
                    case SPEC_MONK_WINDWALKER:
                        DoCast(me, SPELL_SEEK_AND_DESTROY_3, true);
                        break;
                }
            }

            void JustDied(Unit* killer) override
            {
                if (me->HasAura(SPELL_SEEK_AND_DESTROY))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_POWER, true);

                if (me->HasAura(SPELL_SEEK_AND_DESTROY_2))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_WISDOM, true);

                if (me->HasAura(SPELL_SEEK_AND_DESTROY_3))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_VELOCITY, true);
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
                        case EVENT_TAIL_STRIKE_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TAIL_STRIKE, false);
                            events.ScheduleEvent(EVENT_TAIL_STRIKE_2,      25000);
                            break;
                        case EVENT_CLAW_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_CLAW, false);
                            events.ScheduleEvent(EVENT_CLAW_2, 30000);
                            break;
                        case EVENT_UNSTABLE_SERUM_9:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_9, 30000);
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
            return new npc_ik_thik_terrorclawAI(creature);
        }
};

enum eKaranoshSpells
{
    SPELL_CORROSIVE_RESIN              = 128425,
};

enum eKaranoshEvents
{
    EVENT_CORROSIVE_RESIN         = 1,
    EVENT_UNSTABLE_SERUM_10       = 2,
};

class npc_karanosh : public CreatureScript
{
    public:
        npc_karanosh() : CreatureScript("npc_karanosh") { }

        struct npc_karanoshAI : public ScriptedAI
        {
            npc_karanoshAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_CORROSIVE_RESIN,       17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_10,     12000);
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
                        case EVENT_CORROSIVE_RESIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_CORROSIVE_RESIN, false);
                            events.ScheduleEvent(EVENT_CORROSIVE_RESIN, 20000);
                            break;
                        case EVENT_UNSTABLE_SERUM_10:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_10, 14000);
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
            return new npc_karanoshAI(creature);
        }
};

enum eKypaRakSpells
{
    SPELL_BURROW             = 128104,
    SPELL_CRACKING_STONE     = 128135,
    SPELL_RAISE_EARTH        = 128107,
};

enum eKypaRakEvents
{
    EVENT_BURROW                  = 1,
    EVENT_CRACKING_STONE          = 2,
    EVENT_RAISE_EARTH             = 3,
    EVENT_UNSTABLE_SERUM_11       = 4,
    EVENT_UNAURA_BURROW           = 5,
};

class npc_kypa_rak : public CreatureScript
{
    public:
        npc_kypa_rak() : CreatureScript("npc_kypa_rak") { }

        struct npc_kypa_rakAI : public ScriptedAI
        {
            npc_kypa_rakAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_BURROW,                 8000);
                events.ScheduleEvent(EVENT_RAISE_EARTH,           12000);
                events.ScheduleEvent(EVENT_UNAURA_BURROW,         26000);
                events.ScheduleEvent(EVENT_CRACKING_STONE,        27000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_11,     33000);
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
                        case EVENT_BURROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BURROW, false);
                            events.ScheduleEvent(EVENT_RAISE_EARTH, 23000);
                            events.ScheduleEvent(EVENT_UNAURA_BURROW, 15000);
                            events.ScheduleEvent(EVENT_BURROW, 20000);
                            break;
                        case EVENT_UNAURA_BURROW:
                                me->RemoveAura(SPELL_BURROW);
                            break;
                        case EVENT_CRACKING_STONE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_CRACKING_STONE, false);
                            events.ScheduleEvent(EVENT_CRACKING_STONE, 10000);
                            break;
                        case EVENT_RAISE_EARTH:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RAISE_EARTH, false);
                            break;
                        case EVENT_UNSTABLE_SERUM_11:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_11, 14000);
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
            return new npc_kypa_rakAI(creature);
        }
};

enum eManifestationOfTerrorSpells
{
    SPELL_DREAD_CLAW           = 128026,
    SPELL_LASH_OUT_IN_FEAR     = 131375,
    SPELL_SHA_BOLT             = 131376,
};

enum eManifestationOfTerrorEvents
{
    EVENT_DREAD_CLAW                  = 1,
    EVENT_LASH_OUT_IN_FEAR            = 2,
    EVENT_SHA_BOLT                    = 3,
    EVENT_UNSTABLE_SERUM_12           = 4,
};

class npc_manifestation_of_terror : public CreatureScript
{
    public:
        npc_manifestation_of_terror() : CreatureScript("npc_manifestation_of_terror") { }

        struct npc_manifestation_of_terrorAI : public ScriptedAI
        {
            npc_manifestation_of_terrorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_DREAD_CLAW,                8000);
                events.ScheduleEvent(EVENT_LASH_OUT_IN_FEAR,         12000);
                events.ScheduleEvent(EVENT_SHA_BOLT,                 17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_12,        25000);
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
                        case EVENT_DREAD_CLAW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_DREAD_CLAW,        false); // Nonfunctional
                            events.ScheduleEvent(EVENT_DREAD_CLAW, 25000);
                            break;
                        case EVENT_LASH_OUT_IN_FEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_LASH_OUT_IN_FEAR, false);
                            events.ScheduleEvent(EVENT_LASH_OUT_IN_FEAR, 10000);
                            break;
                        case EVENT_SHA_BOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHA_BOLT,        false);
                            events.ScheduleEvent(EVENT_SHA_BOLT, 20000);
                            break;
                        case EVENT_UNSTABLE_SERUM_12:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_12, 14000);
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
            return new npc_manifestation_of_terrorAI(creature);
        }
};

enum eShoxTikSpells
{
    SPELL_RAPID_SHOT           = 96259,
    SPELL_SHOOT                = 130414,
    SPELL_TOXIC_SHOT           = 82844,
};

enum eShoxTikEvents
{
    EVENT_RAPID_SHOT                  = 1,
    EVENT_SHOOT                       = 2,
    EVENT_TOXIC_SHOT                  = 3,
    EVENT_UNSTABLE_SERUM_13           = 4,
};

class npc_shox_tik : public CreatureScript
{
    public:
        npc_shox_tik() : CreatureScript("npc_shox_tik") { }

        struct npc_shox_tikAI : public ScriptedAI
        {
            npc_shox_tikAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RAPID_SHOT,                 8000);
                events.ScheduleEvent(EVENT_SHOOT,                     12000);
                events.ScheduleEvent(EVENT_TOXIC_SHOT,                17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_13,         25000);
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
                        case EVENT_RAPID_SHOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RAPID_SHOT,        false); // Nonfunctional
                            events.ScheduleEvent(EVENT_RAPID_SHOT, 25000);
                            break;
                        case EVENT_SHOOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHOOT, false);
                            events.ScheduleEvent(EVENT_SHOOT, 10000);
                            break;
                        case EVENT_TOXIC_SHOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TOXIC_SHOT,        false);
                            events.ScheduleEvent(EVENT_TOXIC_SHOT, 20000);
                            break;
                        case EVENT_UNSTABLE_SERUM_13:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_13, 14000);
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
            return new npc_shox_tikAI(creature);
        }
};

enum eWakeOfHorrorSpells
{
    SPELL_SHA_CORRUPTION    = 128099,
};

enum eWakeOfHorrorEvents
{
    EVENT_SHA_CORRUPTION                   = 1,
    EVENT_UNSTABLE_SERUM_14                = 2,
};

class npc_wake_of_horror : public CreatureScript
{
    public:
        npc_wake_of_horror() : CreatureScript("npc_wake_of_horror") { }

        struct npc_wake_of_horrorAI : public ScriptedAI
        {
            npc_wake_of_horrorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_SHA_CORRUPTION,    10000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_14, 20000);
            }

            void JustEngagedWith(Unit* who) override
            {
                Player* player = who->ToPlayer();

                if (!player)
                    return;

                switch (player->GetTalentSpecialization())
                {
                    case SPEC_DEATH_KNIGHT_BLOOD:
                    case SPEC_DEATH_KNIGHT_FROST:
                    case SPEC_DEATH_KNIGHT_UNHOLY:
                    case SPEC_PALADIN_PROTECTION:
                    case SPEC_PALADIN_RETRIBUTION:
                    case SPEC_WARRIOR_ARMS:
                    case SPEC_WARRIOR_FURY:
                    case SPEC_WARRIOR_PROTECTION:
                    case SPEC_MONK_BREWMASTER:
                    default:
                        DoCast(me, SPELL_SEEK_AND_DESTROY,   true);
                        break;
                    case SPEC_MAGE_ARCANE:
                    case SPEC_MAGE_FIRE:
                    case SPEC_MAGE_FROST:
                    case SPEC_PALADIN_HOLY:
                    case SPEC_DRUID_BALANCE:
                    case SPEC_PRIEST_DISCIPLINE:
                    case SPEC_PRIEST_HOLY:
                    case SPEC_PRIEST_SHADOW:
                    case SPEC_DRUID_RESTORATION:
                    case SPEC_SHAMAN_ELEMENTAL:
                    case SPEC_SHAMAN_RESTORATION:
                    case SPEC_WARLOCK_AFFLICTION:
                    case SPEC_WARLOCK_DEMONOLOGY:
                    case SPEC_WARLOCK_DESTRUCTION:
                    case SPEC_MONK_MISTWEAVER:
                        DoCast(me, SPELL_SEEK_AND_DESTROY_2, true);
                        break;
                    case SPEC_DRUID_FERAL:
                    case SPEC_DRUID_GUARDIAN:
                    case SPEC_HUNTER_BEAST_MASTERY:
                    case SPEC_HUNTER_MARKSMANSHIP:
                    case SPEC_HUNTER_SURVIVAL:
                    case SPEC_ROGUE_ASSASSINATION:
                    case SPEC_ROGUE_COMBAT:
                    case SPEC_ROGUE_SUBTLETY:
                    case SPEC_SHAMAN_ENHANCEMENT:
                    case SPEC_MONK_WINDWALKER:
                        DoCast(me, SPELL_SEEK_AND_DESTROY_3, true);
                        break;
                }
            }

            void JustDied(Unit* killer) override
            {
                if (me->HasAura(SPELL_SEEK_AND_DESTROY))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_POWER, true);

                if (me->HasAura(SPELL_SEEK_AND_DESTROY_2))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_WISDOM, true);

                if (me->HasAura(SPELL_SEEK_AND_DESTROY_3))
                    me->CastSpell(killer, SPELL_SEEK_AND_DESTROY_VELOCITY, true);
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
                        case EVENT_SHA_CORRUPTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHA_CORRUPTION, false);
                            events.ScheduleEvent(EVENT_SHA_CORRUPTION,      15000);
                            break;
                        case EVENT_UNSTABLE_SERUM_14:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_14, 15000);
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
            return new npc_wake_of_horrorAI(creature);
        }
};

enum eWarlordGurthanSpells
{
    SPELL_CRUSHING_SHADOWS     = 131704,
    SPELL_RUNE_OF_SHADOWS      = 131696,
    SPELL_SHADOW_POWER         = 131698,
};

enum eWarlordGurthanEvents
{
    EVENT_CRUSHING_SHADOWS            = 1,
    EVENT_RUNE_OF_SHADOWS             = 2,
    EVENT_SHADOW_POWER                = 3,
    EVENT_UNSTABLE_SERUM_15           = 4,
};

class npc_warlord_gurthan : public CreatureScript
{
    public:
        npc_warlord_gurthan() : CreatureScript("npc_warlord_gurthan") { }

        struct npc_warlord_gurthanAI : public ScriptedAI
        {
            npc_warlord_gurthanAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_CRUSHING_SHADOWS,           8000);
                events.ScheduleEvent(EVENT_RUNE_OF_SHADOWS,           12000);
                events.ScheduleEvent(EVENT_SHADOW_POWER,              17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_15,         25000);
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
                        case EVENT_CRUSHING_SHADOWS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_CRUSHING_SHADOWS, false); // Nonfunctional
                            events.ScheduleEvent(EVENT_CRUSHING_SHADOWS, 25000);
                            break;
                        case EVENT_RUNE_OF_SHADOWS:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RUNE_OF_SHADOWS, false);
                            events.ScheduleEvent(EVENT_RUNE_OF_SHADOWS, 10000);
                            break;
                        case EVENT_SHADOW_POWER:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SHADOW_POWER, false);
                            events.ScheduleEvent(EVENT_SHADOW_POWER, 20000);
                            break;
                        case EVENT_UNSTABLE_SERUM_15:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_15, 14000);
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
            return new npc_warlord_gurthanAI(creature);
        }
};

 enum eAiLiSkymirrorSpells
 {
     SPELL_CHI_BURST           = 125817,
     SPELL_HEALING_MISTS       = 125802,
     SPELL_SPINNING_CRANE_KICK = 125799,
 };

 enum eAiLiSkymirrorEvents
 {
     EVENT_CHI_BURST                  = 1,
     EVENT_HEALING_MISTS              = 2,
     EVENT_SPINNING_CRANE_KICK        = 3,
     EVENT_UNSTABLE_SERUM_16          = 4,
 };

class npc_ai_li_skymirror : public CreatureScript
{
    public:
        npc_ai_li_skymirror() : CreatureScript("npc_ai_li_skymirror") { }

        struct npc_ai_li_skymirrorAI : public ScriptedAI
        {
            npc_ai_li_skymirrorAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            { 
                events.Reset();

                events.ScheduleEvent(EVENT_CHI_BURST,                  8000);
                events.ScheduleEvent(EVENT_HEALING_MISTS,             12000);
                events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK,       17000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_16,         25000);
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
                        case EVENT_CHI_BURST:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, [=](Unit const* conditionTarget) {return conditionTarget && conditionTarget->GetExactDist2d(me)>15.0f; }))
                                DoCast(target, SPELL_CHI_BURST);

                            events.ScheduleEvent(EVENT_CHI_BURST, 10000);
                            break;
                        case EVENT_HEALING_MISTS:
                            DoCast(me, SPELL_HEALING_MISTS);
                            events.ScheduleEvent(EVENT_HEALING_MISTS, 17000);
                            break;
                        case EVENT_SPINNING_CRANE_KICK:
                            DoCast(me, SPELL_SPINNING_CRANE_KICK);
                            events.ScheduleEvent(EVENT_SPINNING_CRANE_KICK, 21000);
                            break;
                        case EVENT_UNSTABLE_SERUM_16:
                            DoCast(me, SPELL_UNSTABLE_SERUM);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_16, 25000);
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
             return new npc_ai_li_skymirrorAI(creature);
         }
};

enum eOvergrownSeacarpSpells
{
    SPELL_SNAPJAW     = 79442,
};

enum eOvergrownSeacarpEvents
{
    EVENT_SNAPJAW                = 1,
    EVENT_UNSTABLE_SERUM_17      = 2,
};

class npc_overgrown_seacarp : public CreatureScript
{
    public:
        npc_overgrown_seacarp() : CreatureScript("npc_overgrown_seacarp") { }

        struct npc_overgrown_seacarpAI : public ScriptedAI
        {
            npc_overgrown_seacarpAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_SNAPJAW,           8000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_17,           12000);
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
                        case EVENT_SNAPJAW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SNAPJAW,        false);
                            events.ScheduleEvent(EVENT_SNAPJAW, 10000);
                            break;
                        case EVENT_UNSTABLE_SERUM_17:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_17, 14000);
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
            return new npc_overgrown_seacarpAI(creature);
        }
};

enum eGarLokSpells
{
    SPELL_WINDSONG      = 125373,
    SPELL_BLADE_FURRY   = 125370,
    SPELL_TORNADO       = 125398,
    SPELL_TORNADO_DMG   = 131693
};

enum eGarLokEvents
{
    EVENT_SNAPJAW_2              = 1,
    EVENT_UNSTABLE_SERUM_18      = 2,
    EVENT_BLADE_FURRY            = 3,
    EVENT_TORNADO                = 4
};

class npc_gar_lok : public CreatureScript
{
    public:
        npc_gar_lok() : CreatureScript("npc_gar_lok") { }

        struct npc_gar_lokAI : public ScriptedAI
        {
            npc_gar_lokAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_SNAPJAW,               8000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_18,    12000);
                events.ScheduleEvent(EVENT_BLADE_FURRY,          20000);
                events.ScheduleEvent(EVENT_TORNADO,              27000);
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
                        case EVENT_SNAPJAW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_SNAPJAW,        false);
                            events.ScheduleEvent(EVENT_SNAPJAW, 8000);
                            break;
                        case EVENT_UNSTABLE_SERUM_18:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_18, 12000);
                            break;
                        case EVENT_BLADE_FURRY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BLADE_FURRY,        false);
                            events.ScheduleEvent(EVENT_BLADE_FURRY, 20000);
                            break;
                        case EVENT_TORNADO:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TORNADO, false);
                            events.ScheduleEvent(EVENT_TORNADO, 27000);
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
            return new npc_gar_lokAI(creature);
        }
};

enum eDakTheBreakerSpells
{
    SPELL_BELLOWING_RAGE    = 124297,
    SPELL_RUSHING_RAGE      = 124302,
    SPELL_YAUNGOL_STOMP     = 124289
};

enum eDakTheBreakerEvents
{
    EVENT_BELLOWING_RAGE        = 1,
    EVENT_UNSTABLE_SERUM_19     = 2,
    EVENT_RUSHING_RAGE          = 3,
    EVENT_YAUNGOL_STOMP         = 4
};

class npc_dak_the_breaker : public CreatureScript
{
    public:
        npc_dak_the_breaker() : CreatureScript("npc_dak_the_breaker") { }

        struct npc_dak_the_breakerAI : public ScriptedAI
        {
            npc_dak_the_breakerAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RUSHING_RAGE,         5000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_19,   12000);
                events.ScheduleEvent(EVENT_YAUNGOL_STOMP,       20000);
                events.ScheduleEvent(EVENT_BELLOWING_RAGE,      28000);
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
                        case EVENT_BELLOWING_RAGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BELLOWING_RAGE, false);
                            events.ScheduleEvent(EVENT_BELLOWING_RAGE,       25000);
                            break;
                        case EVENT_UNSTABLE_SERUM_19:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_19, 12000);
                            break;
                        case EVENT_RUSHING_RAGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RUSHING_RAGE, false);
                            events.ScheduleEvent(EVENT_RUSHING_RAGE, 5000);
                            break;
                        case EVENT_YAUNGOL_STOMP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_YAUNGOL_STOMP, false);
                            events.ScheduleEvent(EVENT_YAUNGOL_STOMP, 15000);
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
            return new npc_dak_the_breakerAI(creature);
        }
};

enum eNalashVerdantisSpells
{
    SPELL_RAIN_DANCE    = 124860,
    SPELL_TORRENT       = 124935,
    SPELL_WATER_BOLT    = 124854
};

enum eNalashVerdantisEvents
{
    EVENT_UNSTABLE_SERUM_20     = 1,
    EVENT_RAIN_DANCE            = 2,
    EVENT_TORRENT               = 3,
    EVENT_WATER_BOLT            = 4
};

class npc_nalash_verdantis : public CreatureScript
{
    public:
        npc_nalash_verdantis() : CreatureScript("npc_nalash_verdantis") { }

        struct npc_nalash_verdantisAI : public ScriptedAI
        {
            npc_nalash_verdantisAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RUSHING_RAGE,         5000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_20,   12000);
                events.ScheduleEvent(EVENT_YAUNGOL_STOMP,       20000);
                events.ScheduleEvent(EVENT_BELLOWING_RAGE,      28000);
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
                        case EVENT_UNSTABLE_SERUM_20:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_20, 12000);
                            break;
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
            return new npc_nalash_verdantisAI(creature);
        }
};

class npc_hisek_the_swarmkeeper_2 : public CreatureScript
{
    public:
        npc_hisek_the_swarmkeeper_2() : CreatureScript("npc_hisek_the_swarmkeeper_2") { }

        struct npc_hisek_the_swarmkeeper_2AI : public ScriptedAI
        {
            npc_hisek_the_swarmkeeper_2AI(Creature* creature) : ScriptedAI(creature) { }

            void UpdateAI(uint32 /*diff*/) override
            {
                std::list<Player*> playerList;
                playerList.clear();
                GetPlayerListInGrid(playerList, me, 5.0f);

                for (auto &&player: playerList)
                    if (player->GetQuestStatus(31439) == QUEST_STATUS_INCOMPLETE)
                        player->KilledMonsterCredit(64645);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hisek_the_swarmkeeper_2AI(creature);
        }
};

enum eCreatureSpells
{
    SPELL_SHA_BLAST                 = 122034,
    SPELL_DREAD_QUICKENING          = 128282, // this spell haven`t correct information. just mod size?
    SPELL_SHADOWSTEP                = 80576,
    SPELL_SLOWING_POISON            = 7992,
    SPELL_ENRAGE_HATCHLING          = 124840,
    SPELL_INFECTED_BITE             = 49861,
    SPELL_SNAPJAW_1                 = 124835,
    SPELL_SUMMON_COLDBITE_HATCHLING = 124846,
    SPELL_RAIN_OF_SHA               = 131640,
    SPELL_DREAD_STRENGTH            = 131174,
    SPELL_WARCRY                    = 128360,
};

enum eCreatureEvents
{
    EVENT_SHA_BLAST = 1,
    EVENT_DREAD_QUICK,
    EVENT_SHADOWSTEP,
    EVENT_SLOWING_POISON,
    EVENT_INFECTED_BITE,
    EVENT_SNAPJAW_1,
    EVENT_SUMMON_HATCHLING,
    EVENT_RAIN_OF_SHA,
    EVENT_DREAD_STRENGTH,
};

// Shekzeer Clutch-Keeper 64559
struct npc_shekzeer_clutch_keeper : public ScriptedAI
{
    npc_shekzeer_clutch_keeper(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHA_BLAST, urand(3.5 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_DREAD_QUICK, 11 * IN_MILLISECONDS);
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
                case EVENT_DREAD_QUICK:
                    DoCast(me, SPELL_DREAD_QUICKENING);
                    break;
                case EVENT_SHA_BLAST:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SHA_BLAST);

                    events.ScheduleEvent(EVENT_SHA_BLAST, urand(6 * IN_MILLISECONDS, 12.5 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Mistblade Reapper 61970
struct npc_mistblade_reapper : public ScriptedAI
{
    npc_mistblade_reapper(Creature* creature) : ScriptedAI(creature) { }

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
        events.ScheduleEvent(EVENT_SLOWING_POISON, urand(4.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SHADOWSTEP, 7.5 * IN_MILLISECONDS);
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
                case EVENT_SHADOWSTEP:
                    if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST))
                    {
                        targetGUID = target->GetGUID();
                        DoCast(target, SPELL_SHADOWSTEP);

                        delay = 0;
                        me->m_Events.Schedule(delay += 500, 3, [this]()
                        {
                            if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                                DoCast(target, SPELL_UNSTABLE_SERUM);
                        });
                    }

                    events.ScheduleEvent(EVENT_SHADOWSTEP, urand(8 * IN_MILLISECONDS, 21 * IN_MILLISECONDS));
                    break;
                case EVENT_SLOWING_POISON:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SLOWING_POISON);

                    events.ScheduleEvent(EVENT_SLOWING_POISON, urand(4.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Coldbite Matriarch 62008
struct npc_coldbite_matriarch : public ScriptedAI
{
    npc_coldbite_matriarch(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool triggered;

    void Reset() override
    {
        events.Reset();
        triggered = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_INFECTED_BITE, urand(4.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
        events.ScheduleEvent(EVENT_SNAPJAW_1, 8 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_SUMMON_HATCHLING, 5 * IN_MILLISECONDS);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(41) && !triggered)
        {
            triggered = true;
            DoCast(me, SPELL_ENRAGE_HATCHLING);
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
                case EVENT_INFECTED_BITE:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_INFECTED_BITE);

                    events.ScheduleEvent(EVENT_INFECTED_BITE, urand(4.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
                    break;
                case EVENT_SNAPJAW_1:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_SNAPJAW_1);

                    events.ScheduleEvent(EVENT_SNAPJAW_1, urand(10 * IN_MILLISECONDS, 16 * IN_MILLISECONDS));
                    break;
                case EVENT_SUMMON_HATCHLING:
                    DoCast(me, SPELL_SUMMON_COLDBITE_HATCHLING);
                    events.ScheduleEvent(EVENT_SUMMON_HATCHLING, 25 * IN_MILLISECONDS);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// ik`thik Whisperer 64622
struct npc_ik_thik_whisperer : public ScriptedAI
{
    npc_ik_thik_whisperer(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RAIN_OF_SHA, urand(3.5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_RAIN_OF_SHA)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_RAIN_OF_SHA);

                events.ScheduleEvent(EVENT_RAIN_OF_SHA, urand(8.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Ik`thik Clutch-Guard 66465, Kor`thik Warcaller 62754
struct npc_ik_thik_clutch_guard : public ScriptedAI
{
    npc_ik_thik_clutch_guard(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DREAD_STRENGTH, 6 * IN_MILLISECONDS);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_DREAD_STRENGTH)
            {
                DoCast(me, me->GetEntry() == NPC_KORTHIK_WARCALLER ? SPELL_WARCRY : SPELL_DREAD_STRENGTH);
                events.ScheduleEvent(EVENT_DREAD_STRENGTH, urand(16 * IN_MILLISECONDS, 18 * IN_MILLISECONDS));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

enum eVengefulSpiritEvents
{
    EVENT_UNSTABLE_SERUM_21       = 1,
    EVENT_RAIN_DANCE_2            = 2
};

#define SPELL_ICE_TRAP 135382

class npc_vengeful_spirit : public CreatureScript
{
    public:
        npc_vengeful_spirit() : CreatureScript("npc_vengeful_spirit") { }

        struct npc_vengeful_spiritAI : public ScriptedAI
        {
            npc_vengeful_spiritAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_RAIN_DANCE_2,         5000);
                events.ScheduleEvent(EVENT_UNSTABLE_SERUM_21,   12000);
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
                        case EVENT_UNSTABLE_SERUM_21:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_UNSTABLE_SERUM, false);
                            events.ScheduleEvent(EVENT_UNSTABLE_SERUM_21, 15000);
                            break;
                        case EVENT_RAIN_DANCE_2:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RAIN_DANCE, false);
                            events.ScheduleEvent(EVENT_RAIN_DANCE_2, 15000);
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
            return new npc_vengeful_spiritAI(creature);
        }
};

// Adjunct Zet'uk (65478) - Sha Eruption (130061)

enum
{
    SPELL_SHA_ERUPTION_SUMMON       = 130062,
    SPELL_SHA_ERUPTION_SUMMON_FIRE  = 130065, // NYI target type 138
    SPELL_SHA_ERUPTION_PERIODIC     = 130063,
    SPELL_SHA_ERUPTION_DAMAGE       = 130066,

    NPC_SHA_ERUPTION_FIRE           = 66146
};

// Sha Eruption targeting 130061
class spell_zet_uk_sha_eruption : public SpellScriptLoader
{
    public:
        spell_zet_uk_sha_eruption() : SpellScriptLoader("spell_zet_uk_sha_eruption") { }

        class spell_zet_uk_sha_eruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_zet_uk_sha_eruption_SpellScript);

            void ResizeTargets(std::list<WorldObject*>& targets)
            {
                if (targets.size() > 1)
                    Trinity::Containers::RandomResizeList(targets, 1);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    caster->SetFacingToObject(target);
                    caster->SetOrientation(caster->GetAngle(target));
                    caster->CastSpell(caster, SPELL_SHA_ERUPTION_SUMMON, false);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zet_uk_sha_eruption_SpellScript::ResizeTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_zet_uk_sha_eruption_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_zet_uk_sha_eruption_SpellScript();
        }
};

// Sha Eruption Periodic trigger - 130063
class spell_zet_uk_sha_eruption_periodic_summon : public SpellScriptLoader
{
    public:
        spell_zet_uk_sha_eruption_periodic_summon() : SpellScriptLoader("spell_zet_uk_sha_eruption_periodic_summon") { }

        class spell_zet_uk_sha_eruption_periodic_summon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_zet_uk_sha_eruption_periodic_summon_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                Unit*  caster = GetCaster();
                if (!caster)
                    return;

                float dist = (float)aurEff->GetTickNumber() * 6.0f; // radius of damage spell * 2
                Position pos = caster->GetNearPosition(dist, 0.0f);
                if (Creature* summon = caster->SummonCreature(NPC_SHA_ERUPTION_FIRE, pos, TEMPSUMMON_TIMED_DESPAWN, 20000)) // Summon spell target type NYI (138)
                    summon->CastSpell(summon, SPELL_SHA_ERUPTION_DAMAGE, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                if (TempSummon * casterTrigger = GetCaster()->ToTempSummon())
                    casterTrigger->DespawnOrUnsummon(0);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_zet_uk_sha_eruption_periodic_summon_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_zet_uk_sha_eruption_periodic_summon_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_zet_uk_sha_eruption_periodic_summon_AuraScript();
        }
};

class AreaTrigger_at_q_wood_and_shade : public AreaTriggerScript
{
    public:
        AreaTrigger_at_q_wood_and_shade() : AreaTriggerScript("at_q_wood_and_shade") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (trigger->ID == 8124)
                player->KilledMonsterCredit(62955);
            else
                player->KilledMonsterCredit(62956);
            return true;
        }
};

// On the Crab quest
class go_full_crab_pot : public GameObjectScript
{
    public:
        go_full_crab_pot() : GameObjectScript("go_full_crab_pot") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (player->GetQuestStatus(31187) == QUEST_STATUS_INCOMPLETE)
            {
                /*player->CastSpell(player, 89404, true);
                player->TeleportTo(player->GetMapId(), -9207.99f, -1560.32f, 65.46f, 0.82f);*/
                player->KilledMonsterCredit(64006);
                Position pos = go->GetPosition();
                if (auto crabTrap = player->SummonCreature(64009, pos, TEMPSUMMON_TIMED_DESPAWN, 10000))
                {
                    crabTrap->CastSpell(crabTrap, 124959, true);
                    pos.m_positionZ = pos.m_positionZ + 20;
                    crabTrap->GetMotionMaster()->MovePoint(1, pos);
                }
                go->SetRespawnTime(60);
            }
            return false;
        }
};

// Living Amber quest
class spell_item_living_amber : public SpellScriptLoader
{
    public:
        spell_item_living_amber() : SpellScriptLoader("spell_item_living_amber") { }

        class spell_item_living_amber_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_item_living_amber_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                auto target = GetHitUnit();
                if (!target || target->GetTypeId() != TYPEID_UNIT || GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return;

                auto player = GetCaster()->ToPlayer();
                if (player->GetQuestStatus(31021) == QUEST_STATUS_INCOMPLETE)
                {
                    uint16 questProgress = player->GetQuestSlotCounter(player->FindQuestSlot(31021), 0);
                    target->ToCreature()->AI()->Talk(questProgress);
                    player->KilledMonsterCredit(63204);
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_item_living_amber_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_item_living_amber_SpellScript();
        }
};

class npc_hisek_the_swarmkeeper : public CreatureScript
{
    public:
        npc_hisek_the_swarmkeeper() : CreatureScript("npc_hisek_the_swarmkeeper") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == 31441)
            {
                Position pos = creature->GetPosition();
                ObjectGuid playerGUID = player->GetGUID();
                if (auto qgiver = creature->SummonCreature(64705, pos))
                {
                    qgiver->SetExplicitSeerGuid(playerGUID);
                    qgiver->AI()->SetGUID(playerGUID);
                }
            }

            return true;
        }
};

class npc_hisek_the_swarmkeeper_summon : public CreatureScript
{
    public:
        npc_hisek_the_swarmkeeper_summon() : CreatureScript("npc_hisek_the_swarmkeeper_summon") { }

        struct npc_hisek_the_swarmkeeper_summonAI : public ScriptedAI
        {
            npc_hisek_the_swarmkeeper_summonAI(Creature* creature) : ScriptedAI(creature) { }

            uint8 phase;
            uint32 phaseTimer;
            ObjectGuid playerGUID;
            ObjectGuid traitorGUID;

            void Reset() override
            {
                phase = 0;
                phaseTimer = 500;
                traitorGUID = ObjectGuid::Empty;
                playerGUID = ObjectGuid::Empty;
            }

            void SetGUID(ObjectGuid guid, int32 /*type*/) override
            {
                playerGUID = guid;
            }

            void UpdateAI(uint32 diff) override
            {
                if (playerGUID && phaseTimer <= diff)
                {
                    if (phase == 0)
                    {
                        Talk(0);
                        phaseTimer = 2000;
                    }
                    else if (phase == 1)
                    {
                        if (auto traitor = me->SummonCreature(64813, -572.95f, 3015.31f, 181.15f, 2.17f))
                        {
                            traitorGUID = traitor->GetGUID();
                            me->GetMotionMaster()->MovePoint(1, -577.2f, 3021.62f, 183.7f);
                            traitor->SetExplicitSeerGuid(playerGUID);
                            traitor->AI()->Talk(0);
                        }
                        phaseTimer = 4000;
                    }
                    else if (phase == 2)
                    {
                        Talk(1);
                        phaseTimer = 5000;
                    }
                    else if (phase == 3 || phase == 4)
                    {
                        if (auto traitor = me->GetCreature(*me, traitorGUID))
                            traitor->AI()->Talk(phase == 3 ? 1 : 2);
                        phaseTimer = 6000;
                    }
                    else if (phase == 5 || phase == 6)
                    {
                        Talk(phase == 5 ? 2 : 3);
                        phaseTimer = 6000;
                    }
                    else if (phase == 7)
                    {
                        if (auto traitor = me->GetCreature(*me, traitorGUID))
                            traitor->AI()->Talk(3);
                        phaseTimer = 4000;
                    }
                    else if (phase == 8)
                    {
                        if (auto traitor = me->GetCreature(*me, traitorGUID))
                        {
                            traitor->AI()->Talk(4);
                            traitor->UpdateEntry(64583);
                            AttackStart(traitor);
                            playerGUID = ObjectGuid::Empty;
                        }
                    }

                    phase++;
                }
                else
                    phaseTimer -= diff;

                if (me->GetVictim())
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hisek_the_swarmkeeper_summonAI(creature);
        }
};

// Klaxxi`va Ik 65767
class npc_klaxxiva_ik : public CreatureScript
{
    public:
        npc_klaxxiva_ik() : CreatureScript("npc_klaxxiva_ik") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_OVERTHRONE)
                player->CastSpell(player, SPELL_KLAXXI_SCENE_SHEKZEER, true);

            return false;
        }

        struct npc_klaxxiva_ikAI : public ScriptedAI
        {
            npc_klaxxiva_ikAI(Creature* creature) : ScriptedAI(creature) { }

            void InitializeAI() override
            {
                uint32 delay = 0;
                me->m_Events.Schedule(delay += 1000, 1, [this]()
                {
                    DoCast(me, SPELL_KLAXXI_RESONANCE);
                });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_klaxxiva_ikAI(creature);
        }
};

enum q31185
{
    QUEST_WALKING_DOG       = 31185,

    AT_SILT_VENS            = 8169,
    AT_MIST_HOPPER          = 8230,
    AT_WHALE_CORPSE         = 8231,

    NPC_CREDIT_SILT_VENS    = 63879,
    NPC_CREDIT_MIST_HOPPER  = 63880,
    NPC_CREDIT_WHALE_CORPSE = 63881,

    NPC_DOG                 = 63955,

    OBJ_CREDIT_SILT_VENS    = 268448,
    OBJ_CREDIT_MIST_HOPPER  = 268449,
    OBJ_CREDIT_WHALE_CORPSE = 268450,
};

class AreaTrigger_q31185 : public AreaTriggerScript
{
    public:
        AreaTrigger_q31185() : AreaTriggerScript("AreaTrigger_q31185") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetQuestStatus(QUEST_WALKING_DOG) != QUEST_STATUS_INCOMPLETE)
                return true;

            std::list<TempSummon*> summons;
            player->GetSummons(summons, NPC_DOG);

            if (summons.empty())
                return true;

            Creature* dog = summons.back();

            if (player->GetDistance(dog) > 10.0f)
                return true;

            switch (trigger->ID)
            {
                case AT_SILT_VENS:
                    if (!player->GetQuestObjectiveCounter(OBJ_CREDIT_SILT_VENS))
                    {
                        player->KilledMonsterCredit(NPC_CREDIT_SILT_VENS);
                        dog->AI()->Talk(0);
                    }
                    break;
                case AT_MIST_HOPPER:
                    if (!player->GetQuestObjectiveCounter(OBJ_CREDIT_MIST_HOPPER))
                    {
                        player->KilledMonsterCredit(NPC_CREDIT_MIST_HOPPER);
                        dog->AI()->Talk(1);
                    }
                    break;
                case AT_WHALE_CORPSE:
                    if (!player->GetQuestObjectiveCounter(OBJ_CREDIT_WHALE_CORPSE))
                    {
                        player->KilledMonsterCredit(NPC_CREDIT_WHALE_CORPSE);
                        dog->AI()->Talk(2);
                    }
                    break;
            }

            return true;
        }
};

enum q31182
{
    NPC_COLDWHISKER_OTTER = 63376,
    SPELL_FED             = 123803,
};

class spell_q31182 : public SpellScript
{
    PrepareSpellScript(spell_q31182);

    SpellCastResult CheckTarget()
    {
        if (!GetExplTargetUnit())
            return SPELL_FAILED_BAD_TARGETS;

        if (GetExplTargetUnit()->GetEntry() != NPC_COLDWHISKER_OTTER || GetExplTargetUnit()->HasAura(SPELL_FED))
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_q31182::CheckTarget);
    }
};

enum q31487
{
    NPC_DREAD_KUNCHONG = 64717,
};

class spell_q31487 : public SpellScript
{
    PrepareSpellScript(spell_q31487);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* obj) { return obj->GetEntry() != NPC_DREAD_KUNCHONG; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_q31487::FilterTargets, EFFECT_ALL, TARGET_UNIT_DEST_AREA_ENTRY);
    }
};

enum q31233
{
    SPELL_SAP_EXTRACTOR      = 124354, // visual npc model
    SPELL_AMBER_STREAM       = 124348, // visual spell
    SPELL_SUMMON_AMBER_POT   = 124364,
    SPELL_SUMMON_AMBER_POT_2 = 124344,

    OBJECT_AMBER_POT         = 213326,
};

struct npc_amber_tap : public ScriptedAI
{
    npc_amber_tap(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->CastSpell(me, SPELL_SAP_EXTRACTOR);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void OnSpellClick(Unit* clicker, bool& /*result*/) override
    {
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        me->CastSpell(me, SPELL_AMBER_STREAM);
        me->DespawnOrUnsummon(5000);

        uint32 delay = 0;
        me->m_Events.Schedule(delay += 4000, [this]() { me->CastSpell(me, SPELL_SUMMON_AMBER_POT_2); });
    }
};

// Kilruk Wind-Reaver 62538
class npc_kilruk_wind_reaver : public CreatureScript
{
    public:
        npc_kilruk_wind_reaver() : CreatureScript("npc_kilruk_wind_reaver") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            // Missed Gossip ID
            if (player->GetQuestStatus(QUEST_SHADOW_OF_EMPIRE) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->SetPhaseMask(3, true);

                if (Creature* Kilruk_reaver = player->SummonCreature(NPC_KILRUK_QUEST, *creature, TEMPSUMMON_TIMED_DESPAWN, 300 * IN_MILLISECONDS))
                    Kilruk_reaver->SetPhaseMask(2, true);
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }
};

// kil`ruk wind-reaver quest shadow of empire 66800
class npc_kilruk_quest_shadow_of_empire : public CreatureScript
{
    public:
        npc_kilruk_quest_shadow_of_empire() : CreatureScript("npc_kilruk_quest_shadow_of_empire") { }

        struct npc_kilruk_quest_shadow_of_empireAI : public ScriptedAI
        {
            npc_kilruk_quest_shadow_of_empireAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
                me->SetSpeed(MOVE_WALK, 1.15f);
                delay = 0;
                me->m_Events.Schedule(delay += 1500, 1, [this]()
                {
                    Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 7200, 2, [this]()
                {
                    Talk(SAY_SPECIAL_1);

                    delay = 0;
                    me->m_Events.Schedule(delay += 1000, 3, [this]()
                    {
                        Movement::MoveSplineInit init(me);
                        for (auto itr : KilrukChamberPath)
                            init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                        init.SetWalk(true);
                        init.Launch();

                        me->m_Events.Schedule(delay += me->GetSplineDuration(), 17, [this]()
                        {
                            if (Player* itr = ObjectAccessor::GetPlayer(*me, summonerGUID))
                            {
                                itr->KilledMonsterCredit(NPC_KILRUK_QUEST);

                                delay = 0;
                                me->m_Events.Schedule(delay += 5000, 20, [this, itr]()
                                {
                                    itr->SetPhaseMask(1, true);
                                    me->DespawnOrUnsummon();
                                });
                            }
                        });

                        delay = 0;
                        me->m_Events.Schedule(delay += 10200, 4, [this]()
                        {
                            Talk(SAY_SPECIAL_2);
                        });

                        me->m_Events.Schedule(delay += 10000, 5, [this]()
                        {
                            Talk(SAY_SPECIAL_3);
                        });

                        me->m_Events.Schedule(delay += 9000, 6, [this]()
                        {
                            Talk(SAY_SPECIAL_4);
                        });

                        me->m_Events.Schedule(delay += 11100, 7, [this]()
                        {
                            Talk(SAY_SPECIAL_5);
                        });

                        me->m_Events.Schedule(delay += 12000, 8, [this]()
                        {
                            Talk(SAY_SPECIAL_6);
                        });

                        me->m_Events.Schedule(delay += 8000, 9, [this]()
                        {
                            Talk(SAY_SPECIAL_7);
                        });

                        me->m_Events.Schedule(delay += 13000, 10, [this]()
                        {
                            Talk(SAY_SPECIAL_8);
                        });

                        me->m_Events.Schedule(delay += 8000, 11, [this]()
                        {
                            Talk(SAY_SPECIAL_9);
                        });

                        me->m_Events.Schedule(delay += 13400, 12, [this]()
                        {
                            Talk(SAY_SPECIAL_10);
                        });

                        me->m_Events.Schedule(delay += 6000, 13, [this]()
                        {
                            Talk(SAY_SPECIAL_11);
                        });

                        me->m_Events.Schedule(delay += 6000, 14, [this]()
                        {
                            Talk(SAY_SPECIAL_12);
                        });

                        me->m_Events.Schedule(delay += 8000, 15, [this]()
                        {
                            Talk(SAY_SPECIAL_13);
                        });
                    });
                });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kilruk_quest_shadow_of_empireAI(creature);
        }
};

// Malik the Unscathed 66776
class npc_malik_the_unscathed_quest_empress_gambit : public CreatureScript
{
    public:
        npc_malik_the_unscathed_quest_empress_gambit() : CreatureScript("npc_malik_the_unscathed_quest_empress_gambit") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            // Missed Gossip ID
            if (player->GetQuestStatus(QUEST_EMPRESS_GAMBIT) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->CastSpell(player, SPELL_KLAXXI_EMPRESS_GAMBIT, true);

            player->CLOSE_GOSSIP_MENU();

            return true;
        }
};

// Malik Unscathed Trigger 66797
class npc_malik_the_unscathed_quest_empress_gambit_trigger : public CreatureScript
{
    public:
        npc_malik_the_unscathed_quest_empress_gambit_trigger() : CreatureScript("npc_malik_the_unscathed_quest_empress_gambit_trigger") { }

        struct npc_malik_the_unscathed_quest_empress_gambit_triggerAI : public ScriptedAI
        {
            npc_malik_the_unscathed_quest_empress_gambit_triggerAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;
            float x, y;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
                me->SetPhaseMask(2, true);
                delay = 0;
                me->m_Events.Schedule(delay += 1000, 1, [this]()
                {
                    Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 14000, 2, [this]()
                {
                    Talk(SAY_SPECIAL_1);
                });

                me->m_Events.Schedule(delay += 12000, 3, [this]()
                {
                    Talk(SAY_SPECIAL_2);

                    Movement::MoveSplineInit init(me);
                    for (auto itr : MalikStairsPath)
                        init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                    init.SetWalk(true);
                    init.Launch();
                });

                me->m_Events.Schedule(delay += 17000, 4, [this]()
                {
                    Talk(SAY_SPECIAL_3);

                    me->OverrideInhabitType(INHABIT_AIR);
                    me->UpdateMovementFlags();
                    me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 17.0f, me->GetOrientation());
                });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_malik_the_unscathed_quest_empress_gambit_triggerAI(creature);
        }
};

// Grand Vizier Zorlok 66791
class npc_grand_vizier_zorlok_quest_empress_gambit : public CreatureScript
{
    public:
        npc_grand_vizier_zorlok_quest_empress_gambit() : CreatureScript("npc_grand_vizier_zorlok_quest_empress_gambit") { }

        struct npc_grand_vizier_zorlok_quest_empress_gambitAI : public ScriptedAI
        {
            npc_grand_vizier_zorlok_quest_empress_gambitAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid summonerGUID;
            uint32 delay;
            float x, y;

            void IsSummonedBy(Unit* summoner) override
            {
                summonerGUID = summoner->GetGUID();
                me->SetPhaseMask(2, true);
                delay = 0;
                x = 0.0f; y = 0.0f;
                me->m_Events.Schedule(delay += 1000, 1, [this]()
                {
                    GetPositionWithDistInOrientation(me, 30.0f, me->GetOrientation(), x, y);
                    me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), me->GetOrientation());
                });

                me->m_Events.Schedule(delay += 6500, 2, [this]()
                {
                    Talk(SAY_INTRO);
                });

                me->m_Events.Schedule(delay += 15000, 3, [this]()
                {
                    Talk(SAY_SPECIAL_1);
                });

                me->m_Events.Schedule(delay += 16000, 4, [this]()
                {
                    Talk(SAY_SPECIAL_2);
                });

                me->m_Events.Schedule(delay += 9800, 5, [this]()
                {
                    Talk(SAY_SPECIAL_3);
                });

                me->m_Events.Schedule(delay += 8700, 6, [this]()
                {
                    DoCast(me, SPELL_ZORLOK_SHOUT);
                });

                me->m_Events.Schedule(delay += 8300, 7, [this]()
                {
                    Talk(SAY_SPECIAL_4);
                });

                me->m_Events.Schedule(delay += 6200, 8, [this]()
                {
                    Talk(SAY_SPECIAL_5);

                    if (Player* itr = ObjectAccessor::GetPlayer(*me, summonerGUID))
                        itr->KilledMonsterCredit(NPC_MALIC_QUEST);
                });

                me->m_Events.Schedule(delay += 8400, 9, [this]()
                {
                    Movement::MoveSplineInit init(me);
                    init.MoveTo(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ());

                    init.SetWalk(true);
                    init.Launch();
                    me->DespawnOrUnsummon(me->GetSplineDuration());
                });
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_grand_vizier_zorlok_quest_empress_gambitAI(creature);
        }
};

class spell_q31233 : public SpellScript
{
    PrepareSpellScript(spell_q31233);

    void ChangeSummonPos(SpellEffIndex /*effIndex*/)
    {
        WorldLocation summonPos = *GetExplTargetDest();
        summonPos.RelocateOffset(0.0f, 1.5f, -3.0f);
        SetExplTargetDest(summonPos);
        GetHitDest()->RelocateOffset(0.0f, 1.5f, -3.0f);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_q31233::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON_OBJECT_WILD);
    }
};

// Empress Gambit Quest 131196
class spell_empress_gambit_quest : public AuraScript
{
    PrepareAuraScript(spell_empress_gambit_quest);

    void Remove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
            pCaster->SetPhaseMask(1, true);
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* pCaster = GetCaster()->ToPlayer())
            pCaster->SetPhaseMask(3, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_empress_gambit_quest::Remove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectApply += AuraEffectApplyFn(spell_empress_gambit_quest::Apply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// Zorlok Shout 131201 
class spell_zorlok_shout : public SpellScript
{
    PrepareSpellScript(spell_zorlok_shout);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_MALIC_QUEST_1; });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->Kill(target);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_zorlok_shout::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnHit += SpellHitFn(spell_zorlok_shout::HandleOnHit);
    }
};

// Scene 72 (Package 212)
class scene_klaxxi_head_explosion : public SceneScript
{
    public:
        scene_klaxxi_head_explosion() : SceneScript("scene_klaxxi_head_explosion") { }

        void OnSceneStart(Player* player, uint32 /*sceneInstanceID*/, SceneTemplate const* /*sceneTemplate*/) override
        {
            player->SetPhaseMask(2, true);
        }

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
            player->SetPhaseMask(1, true);
            player->KilledMonsterCredit(65772);
        }
};

Position eventPos[5] =
{
    { -435.141f, 3614.94f, 61.9383f, 1.02147f }, // 65486
    { -421.821f, 3628.65f, 61.8022f, 3.91377f }, // 65486
    { -396.978f, 3595.15f, 47.5945f, 2.49012f }, // 65486
    { -354.528f, 3564.46f, 39.9380f, 2.43198f }, // 65475
    { -359.121f, 3565.36f, 39.9380f, 6.11604f }, // 65478
};

class AreaTrigger_q31087 : public AreaTriggerScript
{
    public:
        AreaTrigger_q31087() : AreaTriggerScript("AreaTrigger_q31087") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            if (player->GetQuestStatus(31087) != QUEST_STATUS_INCOMPLETE)
                return true;

            player->KilledMonsterCredit(65328);

            if (player->FindNearestCreature(65478, 100.0f) || player->FindNearestCreature(65486, 100.0f) || player->FindNearestCreature(65475, 100.0f))
                return true;

            for (uint32 i = 0; i < 3; i++)
                player->SummonCreature(65486, eventPos[i], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

            player->m_Events.Schedule(10000, [=]()
            {
                if (Creature* korik = player->SummonCreature(65475, eventPos[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 66000))
                    korik->HandleEmoteStateCommand(EMOTE_STATE_STRANGULATE);
                if (Creature* adjunct = player->SummonCreature(65478, eventPos[4], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 66000))
                    adjunct->AI()->Talk(0);
            });

            return true;
        }
};

class go_silent_beacon : public GameObjectScript
{
    public:
        go_silent_beacon() : GameObjectScript("go_silent_beacon") { }

        bool OnReportUse(Player* player, GameObject* go) override
        {
            if (Creature* korik = go->FindNearestCreature(65475, 50.0f))
                korik->AI()->Talk(0);

            return true;
        }
};

enum BoundWithShadesType
{
    SPELL_DREAD_CLAW_2      = 128026,
    SPELL_SHADOW_CLAW       = 128059,
    SPELL_GATHER_SHADE_10   = 122723,
    SPELL_GATHER_SHADE_2    = 122721,

    EVENT_DREAD_CLAW_2 = 1,
    EVENT_SHADOW_CLAW,

    QUEST_BOUND_WITH_SHADES = 31069,

    NPC_SHADE_CREDIT        = 62817,
};

// Dread Lurker 62751
struct npc_dread_waster_dread_lurker : public customCreatureAI
{
    npc_dread_waster_dread_lurker(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_DREAD_CLAW_2, 4 * IN_MILLISECONDS);
    }

    void JustDied(Unit* killer) override 
    {
        if (killer->ToPlayer())
            killer->ToPlayer()->CastSpell(killer->ToPlayer(), SPELL_GATHER_SHADE_10, true);
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
            ExecuteTargetEvent(SPELL_DREAD_CLAW_2, urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_DREAD_CLAW_2, eventId, PRIORITY_CHANNELED);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Nagging Dreadling 65996
struct npc_dread_waster_nagging_dreadling : public customCreatureAI
{
    npc_dread_waster_nagging_dreadling(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHADOW_CLAW, urand(3 * IN_MILLISECONDS, 8 * IN_MILLISECONDS));
    }

    void JustDied(Unit* killer) override
    {
        if (killer->ToPlayer())
            killer->ToPlayer()->CastSpell(killer->ToPlayer(), SPELL_GATHER_SHADE_2, true);
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
            ExecuteTargetEvent(SPELL_SHADOW_CLAW, urand(8 * IN_MILLISECONDS, 10 * IN_MILLISECONDS), EVENT_SHADOW_CLAW, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Gather Shade 122721, 122723
class spell_dread_waster_gather_shade : public SpellScript
{
    PrepareSpellScript(spell_dread_waster_gather_shade);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Player* target = GetHitPlayer())
            if (target->GetQuestStatus(QUEST_BOUND_WITH_SHADES) == QUEST_STATUS_INCOMPLETE)
                target->KilledMonsterCredit(NPC_SHADE_CREDIT, ObjectGuid::Empty, GetSpellInfo()->Effects[effIndex].BasePoints);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dread_waster_gather_shade::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Sonic Emission 124729
class spell_dread_waster_sonic_emission : public SpellScript
{
    PrepareSpellScript(spell_dread_waster_sonic_emission);

    void HandleHit(SpellEffIndex effIndex)
    {
        if (Creature* target = GetHitCreature())
        {
            if (target->GetEntry() == NPC_IK_THIK_AMBERSTINGER)
            {
                target->CastSpell(target, SPELL_SONIC_SHOCK, true);

                // no info how it should work, but debuf only visual (model scale), just set default hp value for 90 lvl creature on apply
                target->SetMaxHealth(393900);
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_dread_waster_sonic_emission::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_dread_wastes()
{
    // Rare Mobs
    new npc_ik_ik_the_nimble();
    new npc_ai_li_skymirror();
    new npc_gar_lok();
    new npc_dak_the_breaker();
    new npc_nalash_verdantis();
    // Elite Mobs
    new npc_adjunct_kree_zot();
    new npc_dread_fearbringer();
    new npc_dread_kunchong();
    new npc_dread_matriarch();
    new npc_elder_spindlegrin();
    new npc_hulking_kor_thik_fleshrender();
    new npc_ik_thik_amberstinger();
    new npc_ik_thik_kunchong();
    new npc_ik_thik_terrorclaw();
    new npc_karanosh();
    new npc_kypa_rak();
    new npc_manifestation_of_terror();
    new npc_shox_tik();
    new npc_wake_of_horror();
    new npc_warlord_gurthan();
    new npc_vengeful_spirit();
    // Standard Mobs
    new npc_overgrown_seacarp();
    new npc_hisek_the_swarmkeeper_2();
    new creature_script<npc_shekzeer_clutch_keeper>("npc_shekzeer_clutch_keeper");
    new creature_script<npc_mistblade_reapper>("npc_mistblade_reapper");
    new creature_script<npc_coldbite_matriarch>("npc_coldbite_matriarch");
    new creature_script<npc_ik_thik_whisperer>("npc_ik_thik_whisperer");
    new creature_script<npc_ik_thik_clutch_guard>("npc_ik_thik_clutch_guard");
    // Extending the Vocerage
    new spell_zet_uk_sha_eruption();
    new spell_zet_uk_sha_eruption_periodic_summon();
    // Quest scripts
    new AreaTrigger_at_q_wood_and_shade();
    new go_full_crab_pot();
    new spell_item_living_amber();
    new npc_hisek_the_swarmkeeper();
    new npc_hisek_the_swarmkeeper_summon();
    new npc_klaxxiva_ik();
    new AreaTrigger_q31185;
    new spell_script<spell_q31182>("spell_q31182");
    new spell_script<spell_q31487>("spell_q31487");
    new creature_script<npc_amber_tap>("npc_amber_tap");
    new npc_kilruk_wind_reaver();
    new npc_kilruk_quest_shadow_of_empire();
    new npc_malik_the_unscathed_quest_empress_gambit();
    new npc_malik_the_unscathed_quest_empress_gambit_trigger();
    new npc_grand_vizier_zorlok_quest_empress_gambit();
    new spell_script<spell_q31233>("spell_q31233");
    new aura_script<spell_empress_gambit_quest>("spell_empress_gambit_quest");
    new spell_script<spell_zorlok_shout>("spell_zorlok_shout");
    new scene_klaxxi_head_explosion();
    new AreaTrigger_q31087();
    new go_silent_beacon();
    new creature_script<npc_dread_waster_dread_lurker>("npc_dread_waster_dread_lurker");
    new creature_script<npc_dread_waster_nagging_dreadling>("npc_dread_waster_nagging_dreadling");
    new spell_script<spell_dread_waster_gather_shade>("spell_dread_waster_gather_shade");
    new spell_script<spell_dread_waster_sonic_emission>("spell_dread_waster_sonic_emission");
}
