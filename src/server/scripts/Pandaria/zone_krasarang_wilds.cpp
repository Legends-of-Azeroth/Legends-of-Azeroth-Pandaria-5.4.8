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
#include "Vehicle.h"
#include "CombatAI.h"

enum KrasarangWildsCreatureSpells
{
    SPELL_GRAPPLING_HOOK                    = 125623,
    SPELL_VANISH                            = 125632,
    SPELL_VICIOUS_REND                      = 125624,
    SPELL_TORMENTORS_SLASH                  = 128954,
    SPELL_ANCIENT_POISON                    = 111826,
    SPELL_PRESS_THE_ATTACK                  = 135522,
    SPELL_DARK_MENDING                      = 16588,
    SPELL_RUNE_OF_SUFFERING                 = 128964,
    SPELL_OVERWHELMING_SADNESS              = 119556,
    SPELL_THROW_DYNAMITE                    = 133895,
    SPELL_SCUTTLE                           = 117380,
    SPELL_VICE_CLAW                         = 117372,
    SPELL_RAGE_OF_ELDERS                    = 129427,
    SPELL_BLOODTHIRSTY                      = 135544,
    SPELL_DECAPITATE                        = 135549,
    SPELL_ZIN_SPIN                          = 118823,
    SPELL_THICK_SHELL                       = 118142,
    SPELL_HAMMER_OF_WRATH                   = 79971,
    SPELL_CONSECRATION                      = 79966,
    SPELL_AVENGERS_SHIELD                   = 79965,
    SPELL_MOGU_RUNE_WARD                    = 126068, // cosmetic?
    SPELL_MIGHT_MAKES_RIGHT                 = 118986,
    SPELL_COMMANDERS_MIGHT                  = 118982,
    SPELL_ENRAGE                            = 129016,
    SPELL_LEAP_OF_VICTORY                   = 129017,
    SPELL_SHOCKWAVE                         = 129018,
    SPELL_SUNDEN_ARMOR                      = 11971,
    SPELL_RECLAIM_STRENGTH                  = 118906,
    SPELL_ANCIENT_RUNE_OF_STRIKING          = 129031,
    SPELL_SAVAGE_STRIKES                    = 119569,
    SPELL_CALL_OF_DESPAIR                   = 114849,
    SPELL_CALL_OF_DESPAIR_EFF               = 114850,
    SPELL_GLOOM_WHIRL                       = 130288,
    SPELL_LANGUOR                           = 129793,
    SPELL_WAVE_OF_DESPAIR                   = 130153,
    SPELL_GIFT_OF_CHI_JI                    = 129929,
    SPELL_BEAK_STAB                         = 129081,
    SPELL_HEALING_SONG_OF_SINGER            = 131990,
    SPELL_GIFT_OF_CHI_JI_VOID               = 129926,
    SPELL_GIFT_OF_CHI_JI_EFF                = 129929,
    SPELL_BLESSING_OF_CHI_JI                = 129929,
    SPELL_WRATH_OF_CHI_JI                   = 129917,
    SPELL_ALLIANCE_BANNER_VISUAL            = 136126,
    SPELL_MEET_THE_SCOUT_GARROSH            = 134089,
    SPELL_HORDE_BANNER_VISUAL               = 136127,
    SPELL_CANNONBALL_SPIN                   = 135154,
    SPELL_TWINBRAID_KILLCREDIT              = 135493, // this npc u could kill in raid
    SPELL_EXPLOSIVES_PLUNGER                = 135292,
    SPELL_CHAINGUN                          = 135417,
    SPELL_CHAINGUN_EFF                      = 135469,
    SPELL_BLOODHILT_KILLCREDIT              = 135785,
    SPELL_BLOODTHIRSTY_STRIKE               = 135396,
    SPELL_BLOODY_KNIVES                     = 135343,
    SPELL_COUP_DE_GRACE                     = 135397,
    SPELL_BRUTAL_CHARGE_EFF                 = 135427,
    SPELL_BRUTAL_CHARGE                     = 135421,
    SPELL_SUMMON_BROJAI                     = 110553,
    SPELL_EARTH_SPIKE                       = 80117
};

enum KrasarangWildsCreatureEvents
{
    EVENT_GRAPPLING_HOOK                    = 1,
    EVENT_VANISH,
    EVENT_VICIOUS_REND,
    EVENT_TORMENTORS_SLASH,
    EVENT_ANCIENT_POISON,
    EVENT_RUNE_OF_SUFFERING,
    EVENT_DARK_MENDING,
    EVENT_OVERWHELMING_SADNESS,
    EVENT_THROW_DYNAMITE,
    EVENT_VICE_CLAW,
    EVENT_RAGE_OF_ELDERS,
    EVENT_DECAPITATE,
    EVENT_ZIN_SPIN,
    EVENT_HAMMER_OF_WRATH,
    EVENT_CONSECRATION,
    EVENT_AVENGERS_SHIELD,
    EVENT_MIGHT_MAKES_RIGHT,
    EVENT_LEAP_OF_VICTORY,
    EVENT_SHOCKWAVE,
    EVENT_SUNDEN_ARMOR,
    EVENT_RECLAIM_STRENGTH,
    EVENT_ANCIENT_RUNE_OF_STRIKING,
    EVENT_SAVAGE_STRIKES,
    EVENT_CALL_OF_DESPAIR,
    EVENT_GLOOM_WHIRL,
    EVENT_LANGUOR,
    EVENT_WAVE_OF_DESPAIR,
    EVENT_GIFT_OF_CHI_JI,
    EVENT_BEAK_STAB,
    EVENT_HEALING_SONG,
    EVENT_BLESSING_OF_CHI_JI,
    EVENT_WRATH_OF_CHI_JI,
    EVENT_CANNONBALL_SPIN,
    EVENT_EXPLOSIVES_PLUNGER, // missed dynamit?
    EVENT_CHAINGUN,
    EVENT_BRUTAL_CHARGE,
    EVENT_COUP_DE_GRACE,
    EVENT_BLOODY_KNIVES,
    EVENT_BLOODTHIRSTY_STRIKE,
    EVENT_EARTH_SPIKE
};

enum KrasarangWildsCreatures
{
    NPC_KRASARI_RUNEKEEPER                  = 65598,
    NPC_KRASARI_TORMENTOR                   = 58377,
    NPC_BUNNY_DLA                           = 55626,
    NPC_ECHO_OF_DESPAIR                     = 59687,
    NPC_STUDENT_OF_CHI_JI                   = 60601,
    NPC_CHILD_OF_CHI_JI                     = 60616,
    NPC_LANDING_BOAT                        = 68515,
    NPC_KING_VARIAN_WRYNN                   = 68514,
    NPC_7TH_LEGION_CHAMPION                 = 68960,
    NPC_HILDA_HORNSWAGGLE                   = 68519,
    NPC_SCOUT_LYNNA                         = 68311,
    NPC_KING_VARIAN_STATIC                  = 68392,
    NPC_7TH_LEGION_CHAMPION_STATIC          = 68399,
    NPC_HORDE_RAIDER                        = 68332,
    NPC_HILDA_STATIC                        = 68312,
    NPC_MARSHAL_TROTEMAN                    = 68331,
    NPC_KORKRON_BODYGUARD_STATIC            = 68072,
    NPC_GARROSH_HELLSCREAM_STATIC           = 67867,
    NPC_BLOOD_GUARD_GROTASH                 = 67927,
    NPC_GRIZZLE_GEARSLIP                    = 67926,
    NPC_ALLIANCE_SENTINEL                   = 67900,
    NPC_WRATHION_NAVAL_FLEET_CREDIT         = 64681,
    NPC_LANDING_BOAT_HORDE                  = 68087,
    NPC_SCOUT_ROKLA                         = 67812,
    NPC_GARROSH_HELLSCREAM                  = 68088,
    NPC_HORDE_ADDITIONAL_BOAT               = 69072,
    NPC_HORDE_BODYGUARD                     = 69073,
    NPC_GROUNDBREAKER_BROJAI                = 58224,
    NPC_SUNWALKER_DEZCO_QUEST               = 58666,
    NPC_LOREKEEPER_VAELDRIN                 = 58975,
    NPC_LYALIA                              = 58976,
    NPC_KOR_BLOODTUSK                       = 58670
};

enum KrasarangWildsTexts
{
    TALK_INTRO,
    TALK_SPECIAL_1,
    TALK_SPECIAL_2,
    TALK_SPECIAL_3,
    TALK_SPECIAL_4,
    TALK_SPECIAL_5,
    TALK_SPECIAL_6
};

enum KrasarangWildsQuests
{
    QUEST_KING_AMONG_MEN                    = 32247,
    QUEST_MEET_THE_SCOUT_H                  = 32249,
    QUEST_MIGHT_OF_THE_WARCHIEF             = 32250,
    QUEST_INCOMING                          = 31483,
    QUEST_THE_LORD_RECLAIMER                = 30359,
    QUEST_FOR_FAMILY                        = 30174
};

const Position boatPath[7]=
{
    { -1048.432f, -1053.104f,  0.08803847f, 0.0f },
    { -1047.436f, -1053.191f,  0.08803847f, 0.0f },
    { -1037.475f, -1054.012f, -1.38513500f, 0.0f },
    { -1031.498f, -1054.504f, -0.79715970f, 0.0f },
    { -1027.514f, -1054.832f, -0.17215970f, 0.0f },
    { -1026.276f, -1054.936f,  0.14668800f, 0.0f },
    { -1026.276f, -1054.936f,  0.14668800f, 0.0f },
};

const Position reclaimerFriends[3] =
{
    { -1648.94f, 1842.76f, -2.51f, 5.98f },
    { -1626.92f, 1854.67f, -2.49f, 4.73f },
    { -1623.08f, 1836.52f, -2.56f, 3.09f },
};

class npc_arness_the_scale : public CreatureScript
{
    public:
        npc_arness_the_scale() : CreatureScript("npc_arness_the_scale") { }

        struct npc_arness_the_scaleAI : public ScriptedAI
        {
            npc_arness_the_scaleAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 40s);
                events.ScheduleEvent(EVENT_VANISH, 20s);
                events.ScheduleEvent(EVENT_VICIOUS_REND, 15s);
            }

            void JustDied(Unit* /*killer*/) override { }

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
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_GRAPPLING_HOOK, false);
                            events.ScheduleEvent(EVENT_GRAPPLING_HOOK, 10s);
                            break;
                        case EVENT_VANISH:
                            me->CastSpell(me, SPELL_VANISH, false);
                            events.ScheduleEvent(EVENT_VANISH, 20s);
                            break;
                        case EVENT_VICIOUS_REND:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_VICIOUS_REND, false);
                            events.ScheduleEvent(EVENT_VICIOUS_REND, 15s);
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
            return new npc_arness_the_scaleAI(creature);
        }
};

enum TorikEthisSpells
{
    SPELL_BLADE_FURY                        = 125370,
    SPELL_TORNADO                           = 125398,
    SPELL_TORNADO_DMG                       = 131693,
    SPELL_WINDSONG                          = 125373
};

enum TorikEthisEvents
{
    EVENT_BLADE_FURY                        = 1,
    EVENT_TORNADO                           = 2,
    EVENT_WINDSONG                          = 3
};

enum TorikEthisCreatures
{
    NPC_ENTRY_TORNADO                       = 64267
};

class npc_torik_ethis : public CreatureScript
{
    public:
        npc_torik_ethis() : CreatureScript("npc_torik_ethis") { }

        struct npc_torik_ethisAI : public ScriptedAI
        {
            npc_torik_ethisAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                events.Reset();

                events.ScheduleEvent(EVENT_BLADE_FURY, 8s);
                events.ScheduleEvent(EVENT_TORNADO, 40s);
                events.ScheduleEvent(EVENT_WINDSONG, 32s);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_ENTRY_TORNADO)
                {
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
                        case EVENT_BLADE_FURY:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_BLADE_FURY, false);
                            events.ScheduleEvent(EVENT_BLADE_FURY, 8s);
                            break;
                        case EVENT_TORNADO:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_TORNADO, false);
                            events.ScheduleEvent(EVENT_TORNADO, 40s);
                            break;
                        case EVENT_WINDSONG:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_WINDSONG, false);
                            events.ScheduleEvent(EVENT_WINDSONG, 32s);
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
            return new npc_torik_ethisAI(creature);
        }
};

enum GoKanSpells
{
    SPELL_BELLOWING_RAGE                    = 124297,
    SPELL_RUSHING_CHARGE                    = 124302,
    SPELL_YAUNGOL_STOMP                     = 124289
};

enum GoKanEvents
{
    EVENT_BELLOWING_RAGE                    = 1,
    EVENT_RUSHING_CHARGE                    = 2,
    EVENT_YAUNGOL_STOMP                     = 3
};

#define GO_KAN_SAY_1 "Feel the strength of the Yaungol!"

class npc_go_kan : public CreatureScript
{
    public:
        npc_go_kan() : CreatureScript("npc_go_kan") { }

        struct npc_go_kanAI : public ScriptedAI
        {
            npc_go_kanAI(Creature* creature) : ScriptedAI(creature) { }

            EventMap events;

            void Reset() override
            {
                me->SetStandState(UNIT_STAND_STATE_SIT);
                events.Reset();
                events.ScheduleEvent(EVENT_BELLOWING_RAGE, 8s);
                events.ScheduleEvent(EVENT_RUSHING_CHARGE, 17s);
                events.ScheduleEvent(EVENT_YAUNGOL_STOMP, 25s);
            }

            void JustDied(Unit* /*killer*/) override { }

            void JustEngagedWith(Unit* /*who*/) override
            {
                me->RemoveStandFlags(UNIT_STAND_STATE_SIT);
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
                            events.ScheduleEvent(EVENT_BELLOWING_RAGE, 50s);
                            break;
                        case EVENT_RUSHING_CHARGE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_RUSHING_CHARGE, false);
                            events.ScheduleEvent(EVENT_RUSHING_CHARGE, 40s);
                            break;
                        case EVENT_YAUNGOL_STOMP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
                                me->CastSpell(target, SPELL_YAUNGOL_STOMP, false);
                            events.ScheduleEvent(EVENT_YAUNGOL_STOMP, 1min + 5s);
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
            return new npc_go_kanAI(creature);
        }
};

enum DespondentWardenOfZhuSpells
{
    SPELL_DESPAIR                           = 108450,
    SPELL_APPLY_MASK                        = 110169
};

enum DespondentWardenOfZhuTexts
{
    DESPONDENT_WARDEN_OF_ZHU_TEXT_01        = 0,
    DESPONDENT_WARDEN_OF_ZHU_TEXT_02        = 1
};

enum DespondentWardenOfZhuCreatures
{
    NPC_ENTRY_MANIFESTATION_OF_DESPAIR      = 58312
};

enum DespondentWardenOfZhuMisc
{
    QUEST_APPLY_DIRECTLY_TO_THE_FOREHEAD    = 30089
};

class npc_despondent_warden_of_zhu : public CreatureScript
{
    public:
        npc_despondent_warden_of_zhu() : CreatureScript("npc_despondent_warden_of_zhu") { }

        bool OnGossipHello(Player * player, Creature* creature) override
        {
            creature->AI()->Talk(0);
            player->KilledMonsterCredit(creature->GetEntry());
            return false;
        }

        struct npc_despondent_warden_of_zhuAI : public ScriptedAI
        {
            npc_despondent_warden_of_zhuAI(Creature* creature) : ScriptedAI(creature) { }

            ObjectGuid playerGUID;
            uint32 talkTimer;
            uint32 auraRecastTimer;

            void Reset() override
            {
                me->CastSpell(me, SPELL_DESPAIR, true);
                playerGUID = ObjectGuid::Empty;
                talkTimer = 0;
                auraRecastTimer = 0;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->GetDistance(who) > 5.f || talkTimer != 0)
                    return;

                if (auto player = who->ToPlayer())
                {
                    if (player->GetQuestStatus(QUEST_APPLY_DIRECTLY_TO_THE_FOREHEAD) == QUEST_STATUS_INCOMPLETE)
                    {
                        Talk(DESPONDENT_WARDEN_OF_ZHU_TEXT_01);
                        talkTimer = 20000;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                if (talkTimer != 0)
                    if (talkTimer <= diff)
                    {
                        talkTimer = 0;
                    }
                    else talkTimer -= diff;

                if (auraRecastTimer != 0)
                    if (auraRecastTimer <= diff)
                    {
                        me->CastSpell(me, SPELL_DESPAIR, true);
                        auraRecastTimer = 0;
                    }
                    else auraRecastTimer -= diff;
            }

            void SpellHit(Unit* caster, const SpellInfo* spell) override
            {
                if (auraRecastTimer)
                    return;

                if (spell->Id == SPELL_APPLY_MASK && caster->GetTypeId() == TYPEID_PLAYER)
                {
                    Position pos = me->GetPosition();

                    if (auto summon = me->SummonCreature(NPC_ENTRY_MANIFESTATION_OF_DESPAIR, pos, TEMPSUMMON_TIMED_DESPAWN, 120 * IN_MILLISECONDS))
                        summon->AI()->AttackStart(caster);

                    auraRecastTimer = 10000;
                    playerGUID = caster->GetGUID();
                }
            }

            void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) override
            {
                if (auto player = me->GetPlayer(*me, playerGUID))
                {
                    player->KilledMonsterCredit(58238);
                    me->DespawnOrUnsummon(5000);
                }

                Talk(DESPONDENT_WARDEN_OF_ZHU_TEXT_02);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_despondent_warden_of_zhuAI(creature);
        }
};

class npc_zhus_watch_courier : public CreatureScript
{
    public:
        npc_zhus_watch_courier() : CreatureScript("npc_zhus_watch_courier") { }

        struct npc_zhus_watch_courierAI : public ScriptedAI
        {
            npc_zhus_watch_courierAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                SetCanSeeEvenInPassiveMode(true);
                me->ClearUnitState(UNIT_STATE_SIGHTLESS);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (me->GetDistance(who) > 7.f)
                    return;

                if (auto player = who->ToPlayer())
                    player->KilledMonsterCredit(me->GetEntry());
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_zhus_watch_courierAI(creature);
        }
};

struct krasarang_casterAI : public ScriptedAI
{
    krasarang_casterAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    bool HandleRescheduleEventsIfCastAny(uint32 eventId)
    {
        if (me->HasUnitState(UNIT_STATE_CASTING))
        {
            events.RescheduleEvent(eventId, randtime(1s, 2s));
            return true;
        }

        return false;
    }

    ObjectGuid GetLowestFriendlyGUID()
    {
        std::list<Creature*> tmpTargets;
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KRASARI_RUNEKEEPER, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_KRASARI_TORMENTOR, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_CHILD_OF_CHI_JI, 80.0f);
        GetCreatureListWithEntryInGrid(tmpTargets, me, NPC_STUDENT_OF_CHI_JI, 80.0f);

        if (tmpTargets.empty())
            return ObjectGuid::Empty;

        tmpTargets.sort(Trinity::HealthPctOrderPred());

        if (Creature* lowestTarget = tmpTargets.front())
            return lowestTarget->GetGUID();

        return ObjectGuid::Empty;
    }
};

// Shieldwall Footman 67304
struct npc_shieldwall_footman : public ScriptedAI
{
    npc_shieldwall_footman(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_PRESS_THE_ATTACK);
    }
};

// Krasari Tormentor 58377
struct npc_krasari_tormentor : public ScriptedAI
{
    npc_krasari_tormentor(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    ObjectGuid targetGUID;
    uint32 delay;

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
        delay = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ANCIENT_POISON, 3s);
        events.ScheduleEvent(EVENT_TORMENTORS_SLASH, 8s + 500ms);
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
                case EVENT_TORMENTORS_SLASH:
                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_TORMENTORS_SLASH);

                        delay = 0;
                        me->m_Events.Schedule(delay += 3000, 3, [this]()
                        {
                            me->RemoveChanneledCast(targetGUID);
                        });
                    }
                    events.ScheduleEvent(EVENT_TORMENTORS_SLASH, randtime(10s, 19s));
                    break;
                case EVENT_ANCIENT_POISON:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_ANCIENT_POISON);

                    events.ScheduleEvent(EVENT_ANCIENT_POISON, randtime(9s, 13s + 500ms));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Krasari Runekeeper 65598
struct npc_krasari_runekeeper : public krasarang_casterAI
{
    npc_krasari_runekeeper(Creature* creature) : krasarang_casterAI(creature) { }

    EventMap events;
    ObjectGuid targetGUID;
    uint32 delay;

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
        delay = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ANCIENT_POISON, 3s);
        events.ScheduleEvent(EVENT_RUNE_OF_SUFFERING, 6s + 500ms);
        events.ScheduleEvent(EVENT_DARK_MENDING, 13s);
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
                case EVENT_ANCIENT_POISON:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_ANCIENT_POISON);

                    events.ScheduleEvent(EVENT_ANCIENT_POISON, randtime(9s, 13s + 500ms));
                    break;
                case EVENT_RUNE_OF_SUFFERING:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_RUNE_OF_SUFFERING);

                    events.ScheduleEvent(EVENT_RUNE_OF_SUFFERING, randtime(15s + 500ms, 25s));
                    break;
                case EVENT_DARK_MENDING:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                        DoCast(target, SPELL_DARK_MENDING);

                    events.ScheduleEvent(EVENT_DARK_MENDING, randtime(13s, 21s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Weeping Horror 57649
struct npc_weeping_horror : public ScriptedAI
{
    npc_weeping_horror(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_OVERWHELMING_SADNESS, randtime(3s + 500ms, 6s));
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
            if (eventId == EVENT_OVERWHELMING_SADNESS)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_OVERWHELMING_SADNESS);

                events.ScheduleEvent(EVENT_OVERWHELMING_SADNESS, randtime(6s + 500ms, 7s + 500ms));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Bilgewater Sapper 67929
struct npc_bilgewater_sapper : public ScriptedAI
{
    npc_bilgewater_sapper(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_THROW_DYNAMITE, randtime(3s + 500ms, 6s));
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
            if (eventId == EVENT_THROW_DYNAMITE)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_THROW_DYNAMITE);

                events.ScheduleEvent(EVENT_THROW_DYNAMITE, randtime(6s + 500ms, 14s));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Viseclaw Scuttler 58881
struct npc_viceclaw_scuttler : public ScriptedAI
{
    npc_viceclaw_scuttler(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool triggered;

    void Reset() override
    {
        events.Reset();
        triggered = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RAGE_OF_ELDERS, 3s);
        events.ScheduleEvent(EVENT_VICE_CLAW, randtime(5s + 500ms, 12s));
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(31) && !triggered)
        {
            triggered = true;
            DoCast(me, SPELL_SCUTTLE);
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
                case EVENT_RAGE_OF_ELDERS:
                    DoCast(me, SPELL_RAGE_OF_ELDERS);
                    events.ScheduleEvent(EVENT_RAGE_OF_ELDERS, 14s + 500ms);
                    break;
                case EVENT_VICE_CLAW:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_VICE_CLAW);

                    events.ScheduleEvent(EVENT_VICE_CLAW, randtime(10s + 500ms, 17s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dominance Grunt 67598
struct npc_dominance_grunt : public ScriptedAI
{
    npc_dominance_grunt(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_BLOODTHIRSTY);
        events.ScheduleEvent(EVENT_DECAPITATE, randtime(4s + 500ms, 11s + 500ms));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_DECAPITATE)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_DECAPITATE);

                events.ScheduleEvent(EVENT_DECAPITATE, randtime(4s + 500ms, 11s + 500ms));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Elder Seadragon 58940
struct npc_elder_seadragon : public ScriptedAI
{
    npc_elder_seadragon(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool triggered;

    void Reset() override
    {
        events.Reset();
        triggered = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ZIN_SPIN, randtime(8s + 500ms, 11s + 500ms));
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(31) && !triggered)
        {
            triggered = true;
            DoCast(me, SPELL_THICK_SHELL);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_ZIN_SPIN)
            {
                DoCast(me, SPELL_ZIN_SPIN);
                me->ClearUnitState(UNIT_STATE_CASTING);
                events.ScheduleEvent(EVENT_ZIN_SPIN, randtime(13s + 500ms, 18s));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sunwalker Scout 58850
struct npc_sunwalker_scout : public ScriptedAI
{
    npc_sunwalker_scout(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CONSECRATION, 3s);
        events.ScheduleEvent(EVENT_AVENGERS_SHIELD, 12s);
        events.ScheduleEvent(EVENT_HAMMER_OF_WRATH, randtime(4s + 500ms, 12s));
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
                case EVENT_CONSECRATION:
                    DoCast(me, SPELL_CONSECRATION);
                    events.ScheduleEvent(EVENT_CONSECRATION, 11s);
                    break;
                case EVENT_AVENGERS_SHIELD:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_AVENGERS_SHIELD);

                    events.ScheduleEvent(EVENT_AVENGERS_SHIELD, 12s + 500ms);
                    break;
                case EVENT_HAMMER_OF_WRATH:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_HAMMER_OF_WRATH);

                    events.ScheduleEvent(EVENT_HAMMER_OF_WRATH, randtime(4s + 500ms, 12s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dojani Dominator 58165
struct npc_dojani_dominator : public ScriptedAI
{
    npc_dojani_dominator(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool triggered;

    void Reset() override
    {
        events.Reset();
        triggered = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_MOGU_RUNE_WARD);
        events.ScheduleEvent(EVENT_MIGHT_MAKES_RIGHT, randtime(8s + 500ms, 11s + 500ms));
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(46) && !triggered)
        {
            triggered = true;
            DoCast(me, SPELL_COMMANDERS_MIGHT);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_MIGHT_MAKES_RIGHT)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_MIGHT_MAKES_RIGHT);

                events.ScheduleEvent(EVENT_MIGHT_MAKES_RIGHT, randtime(13s + 500ms, 18s));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dojani Enforcer 65626
struct npc_dojani_enforcer : public ScriptedAI
{
    npc_dojani_enforcer(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;
    bool triggered;

    void Reset() override
    {
        events.Reset();
        triggered = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SHOCKWAVE, 4s + 500ms);
        events.ScheduleEvent(EVENT_LEAP_OF_VICTORY, randtime(8s + 500ms, 18s));
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (HealthBelowPct(51) && !triggered)
        {
            triggered = true;
            DoCast(me, SPELL_ENRAGE);
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
                case EVENT_LEAP_OF_VICTORY:
                    if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST))
                        DoCast(target, SPELL_LEAP_OF_VICTORY);

                    events.ScheduleEvent(EVENT_LEAP_OF_VICTORY, randtime(8s + 500ms, 18s));
                    break;
                case EVENT_SHOCKWAVE:
                    DoCast(me, SPELL_SHOCKWAVE);
                    events.ScheduleEvent(EVENT_SHOCKWAVE, 12s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Dojani Surveyor 58068
struct npc_dojani_surveyor : public ScriptedAI
{
    npc_dojani_surveyor(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SUNDEN_ARMOR, randtime(4s + 500ms, 13s));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SUNDEN_ARMOR)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_SUNDEN_ARMOR);

                events.ScheduleEvent(EVENT_SUNDEN_ARMOR, randtime(8s + 500ms, 19s + 500ms));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Dojani Reclaimer 58117
struct npc_dojani_reclaimer : public ScriptedAI
{
    npc_dojani_reclaimer(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_RECLAIM_STRENGTH, 6s + 500ms);
        events.ScheduleEvent(EVENT_ANCIENT_RUNE_OF_STRIKING, 1s + 500ms);
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
                case EVENT_RECLAIM_STRENGTH:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_RECLAIM_STRENGTH);

                    events.ScheduleEvent(EVENT_RECLAIM_STRENGTH, randtime(13s + 500ms, 23s));
                    break;
                case EVENT_ANCIENT_RUNE_OF_STRIKING:
                    me->CastSpell(me->GetPositionX() + frand(-7.0f, 7.0f), me->GetPositionY() + frand(-10.0f, 10.0f), me->GetPositionZ(), SPELL_ANCIENT_RUNE_OF_STRIKING, true);
                    events.ScheduleEvent(EVENT_ANCIENT_RUNE_OF_STRIKING, randtime(10s + 500ms, 20s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Riverblade Slayer 58273
struct npc_riverblade_slayer : public ScriptedAI
{
    npc_riverblade_slayer(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_SAVAGE_STRIKES, randtime(4s + 500ms, 13s));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_SAVAGE_STRIKES)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_SAVAGE_STRIKES);

                events.ScheduleEvent(EVENT_SAVAGE_STRIKES, randtime(8s + 500ms, 19s + 500ms));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sha of Despair 59651
struct npc_sha_of_despair : public ScriptedAI
{
    npc_sha_of_despair(Creature* creature) : ScriptedAI(creature), summons(me) { }

    SummonList summons;
    EventMap events;

    void Reset() override
    {
        events.Reset();
        summons.DespawnAll();
        SetCombatMovement(false);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_CALL_OF_DESPAIR, randtime(8s, 20s));
        events.ScheduleEvent(EVENT_GLOOM_WHIRL, 13s);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (Unit* target = me->GetVictim())
            summon->RemoveChanneledCast(target->GetGUID());
    }

    void JustDied(Unit* /*killer*/) override 
    {
        summons.DespawnAll();
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
                case EVENT_CALL_OF_DESPAIR:
                    DoCast(me, SPELL_CALL_OF_DESPAIR);
                    events.ScheduleEvent(EVENT_CALL_OF_DESPAIR, randtime(18s, 36s));
                    break;
                case EVENT_GLOOM_WHIRL:
                    DoCast(me, SPELL_GLOOM_WHIRL);
                    events.ScheduleEvent(EVENT_GLOOM_WHIRL, 19s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Echo of Despair 59687
struct npc_echo_of_despair : public ScriptedAI
{
    npc_echo_of_despair(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        me->SetDisplayId(40884);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LANGUOR, randtime(4s + 500ms, 17s));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_LANGUOR)
            {
                if (Unit* target = me->GetVictim())
                    DoCast(target, SPELL_LANGUOR);

                events.ScheduleEvent(EVENT_LANGUOR, randtime(8s + 500ms, 19s + 500ms));
            }
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Source of Despair 65873
struct npc_source_of_despair : public ScriptedAI
{
    npc_source_of_despair(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_LANGUOR, randtime(4s + 500ms, 17s));
        events.ScheduleEvent(EVENT_WAVE_OF_DESPAIR, 10s);
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
                case EVENT_LANGUOR:
                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_LANGUOR);

                    events.ScheduleEvent(EVENT_LANGUOR, randtime(8s + 500ms, 19s + 500ms));
                    break;
                case EVENT_WAVE_OF_DESPAIR:
                    DoCast(me, SPELL_WAVE_OF_DESPAIR);
                    events.ScheduleEvent(EVENT_WAVE_OF_DESPAIR, 10s);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Child of Chi-Ji 60616
struct npc_child_of_chi_ji_krasarang : public krasarang_casterAI
{
    npc_child_of_chi_ji_krasarang(Creature* creature) : krasarang_casterAI(creature) { }

    EventMap events;
    ObjectGuid targetGUID;
    uint32 delay;

    void Reset() override
    {
        events.Reset();
        targetGUID = ObjectGuid::Empty;
        delay = 0;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_HEALING_SONG, 15s);
        events.ScheduleEvent(EVENT_BEAK_STAB, randtime(8s + 500ms, 14s));
        events.ScheduleEvent(EVENT_GIFT_OF_CHI_JI, 4s);
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
                case EVENT_HEALING_SONG:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    DoCast(me, SPELL_HEALING_SONG_OF_SINGER);
                    events.ScheduleEvent(EVENT_HEALING_SONG, randtime(15s, 25s));
                    break;
                case EVENT_BEAK_STAB:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    if (Unit* target = me->GetVictim())
                    {
                        targetGUID = target->GetGUID();
                        me->PrepareChanneledCast(me->GetAngle(target), SPELL_BEAK_STAB);

                        delay = 0;
                        me->m_Events.Schedule(delay += 3500, 4, [this]()
                        {
                            me->RemoveChanneledCast(targetGUID);
                        });
                    }
                    events.ScheduleEvent(EVENT_BEAK_STAB, randtime(14s + 500ms, 19s + 500ms));
                    break;
                case EVENT_GIFT_OF_CHI_JI:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    DoCast(me, SPELL_GIFT_OF_CHI_JI);
                    events.ScheduleEvent(EVENT_GIFT_OF_CHI_JI, 13s + 500ms);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Student of Chi-Ji 60601
struct npc_student_of_chi_ji : public krasarang_casterAI
{
    npc_student_of_chi_ji(Creature* creature) : krasarang_casterAI(creature) { }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_BLESSING_OF_CHI_JI, 15s);
        events.ScheduleEvent(EVENT_WRATH_OF_CHI_JI, randtime(4s + 500ms, 15s));
        events.ScheduleEvent(EVENT_GIFT_OF_CHI_JI, 4s);
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
                case EVENT_BLESSING_OF_CHI_JI:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    if (Unit* target = ObjectAccessor::GetUnit(*me, GetLowestFriendlyGUID()))
                        DoCast(target, SPELL_BLESSING_OF_CHI_JI);

                    events.ScheduleEvent(EVENT_BLESSING_OF_CHI_JI, randtime(15s, 20s));
                    break;
                case EVENT_WRATH_OF_CHI_JI:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    if (Unit* target = me->GetVictim())
                        DoCast(target, SPELL_WRATH_OF_CHI_JI);

                    events.ScheduleEvent(EVENT_WRATH_OF_CHI_JI, randtime(4s + 500ms, 15s));
                    break;
                case EVENT_GIFT_OF_CHI_JI:
                    if (HandleRescheduleEventsIfCastAny(eventId))
                        break;

                    DoCast(me, SPELL_GIFT_OF_CHI_JI);
                    events.ScheduleEvent(EVENT_GIFT_OF_CHI_JI, 13s + 500ms);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

// Alliance Landing Boat 68515
struct npc_alliance_landing_boat : public ScriptedAI
{
    npc_alliance_landing_boat(Creature* creature) : ScriptedAI(creature) { }

    EventMap nonCombatEvents;
    ObjectGuid ownerGUID, varianGUID;
    float jumpOri, x, y;
    std::vector<ObjectGuid> champGUIDs;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetPhaseMask(2, true);
        x = 0.0f; y = 0.0f;
        ownerGUID = summoner->GetGUID();
        DoCast(me, SPELL_ALLIANCE_BANNER_VISUAL, true);
        jumpOri = Position::NormalizeOrientation(me->GetOrientation() - M_PI / 4);

        int32 bp = 1;

        if (Creature* varian = me->SummonCreature(NPC_KING_VARIAN_WRYNN, *me, TEMPSUMMON_TIMED_DESPAWN, 67 * IN_MILLISECONDS))
        {
            varian->SetPhaseMask(2, true);
            varianGUID = varian->GetGUID();
            varian->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &bp, 0, 0, true);
        }

        for (int32 i = 2; i < 4; i++)
        {
            if (Creature* champion = me->SummonCreature(NPC_7TH_LEGION_CHAMPION, *me, TEMPSUMMON_TIMED_DESPAWN, 67 * IN_MILLISECONDS))
            {
                champion->SetPhaseMask(2, true);
                champGUIDs.push_back(champion->GetGUID());
                champion->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &i, 0, 0, true);
            }
        }

        // Duration before start move or boat will stay at spawn (visual issue with vehicles)
        me->m_Events.Schedule(1500, [this]()
        {
            Movement::MoveSplineInit init(me);
            for (auto itr : boatPath)
                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

            init.SetSmooth();
            init.SetUncompressed();
            init.Launch();

            me->m_Events.Schedule(me->GetSplineDuration(), [this]()
            {
                if (Creature* varian = ObjectAccessor::GetCreature(*me, varianGUID))
                {
                    GetPositionWithDistInOrientation(varian, 7.0f, me->GetOrientation(), x, y);
                    varian->ExitVehicle();
                    varian->GetMotionMaster()->MoveJump({ x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ()), me->GetOrientation() }, 20.0f, 15.0f);
                }

                for (auto&& itr : champGUIDs)
                {
                    if (Creature* champion = ObjectAccessor::GetCreature(*me, itr))
                    {
                        GetPositionWithDistInOrientation(champion, 7.0f, Position::NormalizeOrientation(jumpOri), x, y);
                        jumpOri += M_PI / 2;
                        champion->ExitVehicle();
                        champion->GetMotionMaster()->MoveJump({ x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ()), me->GetOrientation() }, 20.0f, 15.0f);
                    }
                }
            });
        });
    }
};

// King Varian Wrynn 68514
struct npc_varian_wrynn_krasarang_landing : public ScriptedAI
{
    npc_varian_wrynn_krasarang_landing(Creature* creature) : ScriptedAI(creature) { }

    uint32 delay;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetPhaseMask(2, true);

        delay = 0;
        me->m_Events.Schedule(delay += 2000, [this]()
        {
            if (Creature* hilda = me->FindNearestCreature(NPC_HILDA_HORNSWAGGLE, 50.0f, true))
                me->SetFacingTo(me->GetAngle(hilda));

            Talk(TALK_INTRO);
        });

        me->m_Events.Schedule(delay += 3000, [this]()
        {
            Talk(TALK_SPECIAL_1);
        });

        me->m_Events.Schedule(delay += 3500, [this]()
        {
            Talk(TALK_SPECIAL_2);
        });

        me->m_Events.Schedule(delay += 8000, [this]()
        {
            Talk(TALK_SPECIAL_3);
        });

        me->m_Events.Schedule(delay += 6000, [this]()
        {
            if (Creature* lynna = me->FindNearestCreature(NPC_SCOUT_LYNNA, 50.0f, true))
                me->SetFacingTo(me->GetAngle(lynna));

            Talk(TALK_SPECIAL_4);
        });

        me->m_Events.Schedule(delay += 6500, [this]()
        {
            Talk(TALK_SPECIAL_5);
        });

        me->m_Events.Schedule(delay += 3500, [this]()
        {
            Talk(TALK_SPECIAL_6);
        });
    }
};

// Hilda Hornswaggle 68519
struct npc_hilda_hornswaggle_krasarang_landing : public ScriptedAI
{
    npc_hilda_hornswaggle_krasarang_landing(Creature* creature) : ScriptedAI(creature) { }

    uint32 delay;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetPhaseMask(2, true);

        delay = 0;
        me->m_Events.Schedule(delay += 5500, [this]()
        {
            if (Creature* varian = me->FindNearestCreature(NPC_KING_VARIAN_WRYNN, 50.0f, true))
                me->SetFacingTo(me->GetAngle(varian));

            Talk(TALK_INTRO);
        });

        me->m_Events.Schedule(delay += 4000, [this]()
        {
            Talk(TALK_SPECIAL_1);
        });

        me->m_Events.Schedule(delay += 22000, [this]()
        {
            Talk(TALK_SPECIAL_2);
        });
    }
};

// Horde Landing Boat 68087, 69072
struct npc_horde_landing_boat : public ScriptedAI
{
    npc_horde_landing_boat(Creature* creature) : ScriptedAI(creature) { }

    EventMap nonCombatEvents;
    ObjectGuid ownerGUID;
    float jumpOri, x, y;
    std::vector<ObjectGuid> landingGUIDs;

    void IsSummonedBy(Unit* summoner) override
    {
        me->SetPhaseMask(2, true);
        x = 0.0f; y = 0.0f;
        ownerGUID = summoner->GetGUID();
        DoCast(me, SPELL_HORDE_BANNER_VISUAL, true);
        jumpOri = Position::NormalizeOrientation(me->GetOrientation() - M_PI / 4);

        if (me->GetEntry() == NPC_LANDING_BOAT_HORDE)
        {
            int32 bp = 1;

            if (Creature* garrosh = me->SummonCreature(NPC_GARROSH_HELLSCREAM, *me, TEMPSUMMON_TIMED_DESPAWN, 37 * IN_MILLISECONDS))
            {
                landingGUIDs.push_back(garrosh->GetGUID());
                garrosh->SetPhaseMask(2, true);
                garrosh->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &bp, 0, 0, true);
                bp++;
            }

            if (Creature* bodyGuard = me->SummonCreature(NPC_HORDE_BODYGUARD, *me, TEMPSUMMON_TIMED_DESPAWN, 67 * IN_MILLISECONDS))
            {
                landingGUIDs.push_back(bodyGuard->GetGUID());
                bodyGuard->SetPhaseMask(2, true);
                bodyGuard->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &bp, 0, 0, true);
            }
        }
        else
        {
            for (int32 i = 2; i < 4; i++)
            {
                if (Creature* bodyGuard = me->SummonCreature(NPC_HORDE_BODYGUARD, *me, TEMPSUMMON_TIMED_DESPAWN, 67 * IN_MILLISECONDS))
                {
                    bodyGuard->SetPhaseMask(2, true);
                    landingGUIDs.push_back(bodyGuard->GetGUID());
                    bodyGuard->CastCustomSpell(me, VEHICLE_SPELL_RIDE_HARDCODED, &i, 0, 0, true);
                }
            }
        }

        // Duration before start move or boat will stay at spawn (visual issue with vehicles)
        me->m_Events.Schedule(1500, [this]()
        {
            GetPositionWithDistInOrientation(me, 30.0f, me->GetOrientation(), x, y);

            Movement::MoveSplineInit init(me);
            init.MoveTo(x, y, me->GetPositionZ());

            init.SetSmooth();
            init.SetUncompressed();
            init.Launch();

            me->m_Events.Schedule(me->GetSplineDuration(), [this]()
            {
                for (auto&& itr : landingGUIDs)
                {
                    if (Creature* guard = ObjectAccessor::GetCreature(*me, itr))
                    {
                        GetPositionWithDistInOrientation(guard, 7.0f, Position::NormalizeOrientation(jumpOri), x, y);
                        jumpOri += M_PI / 2;
                        guard->ExitVehicle();
                        guard->GetMotionMaster()->MoveJump({ x, y, me->GetMap()->GetHeight(x, y, me->GetPositionZ()), me->GetOrientation() }, 20.0f, 15.0f);
                    }
                }
            });
        });
    }
};

// Garrosh Hellscream 68088
struct npc_garrosh_hellscream_krasarang_landing : public ScriptedAI
{
    npc_garrosh_hellscream_krasarang_landing(Creature* creature) : ScriptedAI(creature) { }

    uint32 delay;
    float x, y;

    void IsSummonedBy(Unit* summoner) override
    {
        delay = 0;
        me->m_Events.Schedule(delay += 2000, [this]()
        {
            Talk(TALK_INTRO);
        });

        me->m_Events.Schedule(delay += 8000, [this]()
        {
            GetPositionWithDistInOrientation(me, 15.0f, me->GetOrientation(), x, y);
            me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ(), me->GetOrientation());
            Talk(TALK_SPECIAL_1);
        });

        me->m_Events.Schedule(delay += 12000, [this]()
        {
            Talk(TALK_SPECIAL_2);
        });
    }
};

// High Marshal Twinbraid 67801
struct npc_high_marshal_twinbraid : public customCreatureAI
{
    npc_high_marshal_twinbraid(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(TALK_SPECIAL_1);
        events.ScheduleEvent(EVENT_CANNONBALL_SPIN, randtime(8s + 500ms, 19s));
        events.ScheduleEvent(EVENT_CHAINGUN, 25s);
        events.ScheduleEvent(EVENT_EXPLOSIVES_PLUNGER, randtime(15s, 20s));
    }

    void JustDied(Unit* /*killer*/) override 
    {
        Talk(TALK_SPECIAL_2);
        DoCastAOE(SPELL_TWINBRAID_KILLCREDIT);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasAura(SPELL_CANNONBALL_SPIN))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            if (eventId == EVENT_CANNONBALL_SPIN)
                Talk(TALK_INTRO);

            ExecuteTargetEvent(SPELL_CANNONBALL_SPIN, urand(15 * IN_MILLISECONDS, 30 * IN_MILLISECONDS), EVENT_CANNONBALL_SPIN, eventId, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_CHAINGUN, 30 * IN_MILLISECONDS, EVENT_CHAINGUN, eventId, PRIORITY_CHANNELED);
            ExecuteTargetEvent(SPELL_EXPLOSIVES_PLUNGER, 25 * IN_MILLISECONDS, EVENT_EXPLOSIVES_PLUNGER, eventId, PRIORITY_SELF);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Warlord Bloodhilt 67461
struct npc_warlord_bloodhilt : public customCreatureAI
{
    npc_warlord_bloodhilt(Creature* creature) : customCreatureAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_COUP_DE_GRACE, randtime(3s + 500ms, 8s));
        events.ScheduleEvent(EVENT_BLOODY_KNIVES, 12s);
        events.ScheduleEvent(EVENT_BLOODTHIRSTY_STRIKE, randtime(19s, 35s));
        events.ScheduleEvent(EVENT_BRUTAL_CHARGE, 17s);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastAOE(SPELL_BLOODHILT_KILLCREDIT);
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
            ExecuteTargetEvent(SPELL_COUP_DE_GRACE, urand(6.5 * IN_MILLISECONDS, 15 * IN_MILLISECONDS), EVENT_COUP_DE_GRACE, eventId);
            ExecuteTargetEvent(SPELL_BLOODY_KNIVES, 15 * IN_MILLISECONDS, EVENT_BLOODY_KNIVES, eventId);
            ExecuteTargetEvent(SPELL_BRUTAL_CHARGE, 20 * IN_MILLISECONDS, EVENT_BRUTAL_CHARGE, eventId, PRIORITY_NOT_VICTIM);
            ExecuteTargetEvent(SPELL_BLOODTHIRSTY_STRIKE, urand(19 * IN_MILLISECONDS, 35 * IN_MILLISECONDS), EVENT_BLOODTHIRSTY_STRIKE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Call of Despair Target Picker 114856
class spell_call_of_despair_target_picker : public SpellScript
{
    PrepareSpellScript(spell_call_of_despair_target_picker);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([=](WorldObject* target) { return target->GetEntry() != NPC_BUNNY_DLA; });

        if (targets.size() > 1)
            Trinity::Containers::RandomResizeList(targets, 1);
    }

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_CALL_OF_DESPAIR_EFF, true);

    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_call_of_despair_target_picker::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_call_of_despair_target_picker::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Call of Despair Effect 114850
class spell_call_of_despair_missle : public SpellScript
{
    PrepareSpellScript(spell_call_of_despair_missle);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->SummonCreature(NPC_ECHO_OF_DESPAIR, *target, TEMPSUMMON_MANUAL_DESPAWN);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_call_of_despair_missle::HandleEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// Krasarang Landing Cinematic 134653, 135954
class spell_krasarang_landing_cinematic : public AuraScript
{
    PrepareAuraScript(spell_krasarang_landing_cinematic);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Player* owner = GetOwner()->ToPlayer())
            if (owner->GetQuestStatus(QUEST_INCOMING) == QUEST_STATUS_INCOMPLETE)
                owner->KilledMonsterCredit(NPC_WRATHION_NAVAL_FLEET_CREDIT);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_krasarang_landing_cinematic::OnApply, EFFECT_1, SPELL_AURA_SCREEN_EFFECT, AURA_EFFECT_HANDLE_REAL);
    }
};

// Chaingun 135417
class spell_chaingun : public AuraScript
{
    PrepareAuraScript(spell_chaingun);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->CastSpell(owner, SPELL_CHAINGUN_EFF, true);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->RemoveAurasDueToSpell(SPELL_CHAINGUN_EFF);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_chaingun::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectRemoveFn(spell_chaingun::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Cannonball Spin 135154
class spell_cannonball_spin : public AuraScript
{
    PrepareAuraScript(spell_cannonball_spin);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* owner = GetOwner()->ToUnit())
            owner->ClearUnitState(UNIT_STATE_CASTING);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_cannonball_spin::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// Brutal Charge 135421
class spell_bloodhilt_brutal_charge : public SpellScript
{
    PrepareSpellScript(spell_bloodhilt_brutal_charge);

    void HandleEffectHit(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster())
            if (Unit* target = GetHitUnit())
                caster->CastSpell(target, SPELL_BRUTAL_CHARGE_EFF, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_bloodhilt_brutal_charge::HandleEffectHit, EFFECT_0, SPELL_EFFECT_CHARGE);
    }
};

// 453. Summoned by 129926 - Gift of Chi-Ji
class sat_gift_of_chi_ji : public IAreaTriggerAura
{
    bool CheckTriggering(WorldObject* triggering) override
    {
        return triggering->ToUnit();
    }

    void OnTriggeringApply(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            m_caster->CastSpell(target, SPELL_GIFT_OF_CHI_JI_EFF, true);
    }

    void OnTriggeringRemove(WorldObject* triggering) override
    {
        if (Unit* target = triggering->ToUnit())
            target->RemoveAurasDueToSpell(SPELL_GIFT_OF_CHI_JI_EFF);
    }
};

enum StoneplowEnvoySpells
{
    SPELL_PERMANENT_FEIGN_DEATH_STUN        = 108806,
    SPELL_REVIVE_STONEPLOW_ENVOY            = 111446
};

struct npc_stoneplow_envoy : public ScriptedAI
{
    npc_stoneplow_envoy(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
        me->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
        me->CastSpell(me, SPELL_PERMANENT_FEIGN_DEATH_STUN);
        me->setRegeneratingHealth(false);
        me->SetHealth(46087);
        script_timer = 0, player_guid = ObjectGuid::Empty;
    }

    void SpellHit(Unit* caster, const SpellInfo* spell) override
    {
        if (spell->Id == SPELL_REVIVE_STONEPLOW_ENVOY && me->HasAura(SPELL_PERMANENT_FEIGN_DEATH_STUN))
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
            me->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            me->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH_STUN);
            script_timer = 500;
            if (Player* player = caster->ToPlayer())
            {
                player->KilledMonsterCredit(58955, ObjectGuid::Empty);
                player_guid = player->GetGUID();
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (script_timer)
        {
            if (script_timer <= diff)
            {
                script_timer = 0;

                if (Player* player = ObjectAccessor::GetPlayer(*me, player_guid))
                    Talk(0, player);

                const float angle = (me->GetOrientation()) - static_cast<float>(M_PI / 2);
                const float x = me->GetPositionX() - 20 * std::sin(angle);
                const float y = me->GetPositionY() + 20 * std::cos(angle);
                const float z = me->GetMap()->GetHeight(x, y, me->GetPositionZ());
                me->GetMotionMaster()->MovePoint(0, x, y, z);

                me->DespawnOrUnsummon(5500);
            }
            else script_timer -= diff;
        }
    }

private:
    ObjectGuid player_guid;
    uint32 script_timer;
};

enum NahassaSpells
{
    SPELL_NAHASSA_INJURED_ANIM              = 116342,
    SPELL_DYING                             = 116348,
    SPELL_JAW_SNAP                          = 118990,
    SPELL_FEVERISH_DEATHBITE                = 116343
};

enum NahassaEvents
{
    EVENT_01                                = 1,
    EVENT_02                                = 2
};

struct npc_nahassa : public ScriptedAI
{
    npc_nahassa(Creature* creature) : ScriptedAI(creature)
    {
        me->setRegeneratingHealth(false);
        JustAppeared();
    }

    void JustAppeared() override
    {
        DoCast(SPELL_NAHASSA_INJURED_ANIM);
        DoCast(SPELL_DYING);
        me->SetHealth(429970);
        me->ResetPlayerDamageReq();
    }

    void JustReachedHome() override
    {
        JustAppeared();
    }

    void JustEngagedWith(Unit* who) override
    {
        me->RemoveAurasDueToSpell(SPELL_NAHASSA_INJURED_ANIM);
        me->RemoveAurasDueToSpell(SPELL_DYING);
        events.ScheduleEvent(EVENT_01, 0ms);
        events.ScheduleEvent(EVENT_02, 5s);
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
                case EVENT_01:
                    DoCastVictim(SPELL_JAW_SNAP);
                    events.ScheduleEvent(EVENT_01, randtime(10s, 14s));
                    break;
                case EVENT_02:
                    DoCastVictim(SPELL_FEVERISH_DEATHBITE);
                    events.ScheduleEvent(EVENT_02, randtime(13s, 20s));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap events;
};

// Lyalia 58976, Kor 58670
class npc_krasarang_wilds_lyalia : public CreatureScript
{
    public:
        npc_krasarang_wilds_lyalia() : CreatureScript("npc_krasarang_wilds_lyalia") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                // Friendly
                player->SummonCreature(NPC_SUNWALKER_DEZCO_QUEST, reclaimerFriends[0], TEMPSUMMON_TIMED_DESPAWN, 180 * IN_MILLISECONDS, 0, ObjectGuid(uint64(1)));
                player->SummonCreature(NPC_LOREKEEPER_VAELDRIN, reclaimerFriends[1], TEMPSUMMON_TIMED_DESPAWN, 180 * IN_MILLISECONDS, 0, ObjectGuid(uint64(1)));
                player->SummonCreature(NPC_GROUNDBREAKER_BROJAI, reclaimerFriends[2], TEMPSUMMON_TIMED_DESPAWN, 180 * IN_MILLISECONDS);
            }

            player->CLOSE_GOSSIP_MENU();

            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if ((player->GetQuestStatus(QUEST_THE_LORD_RECLAIMER) == QUEST_STATUS_INCOMPLETE && creature->GetEntry() == NPC_LYALIA) || (player->GetQuestStatus(QUEST_FOR_FAMILY) == QUEST_STATUS_INCOMPLETE && creature->GetEntry() == NPC_KOR_BLOODTUSK))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "It`s trap!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        struct npc_krasarang_wilds_lyaliaAI : public ScriptedAI
        {
            npc_krasarang_wilds_lyaliaAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_krasarang_wilds_lyaliaAI(creature);
        }
};

// Groundbreaker Brojai 58224
struct npc_groundbreaker_brojai : public customCreatureAI
{
    npc_groundbreaker_brojai(Creature* creature) : customCreatureAI(creature) { }

    TaskScheduler scheduler;
    float x, y;
    ObjectGuid targetGUID;

    void InitializeAI() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);
        GetPositionWithDistInOrientation(me, 5.0f, me->GetOrientation(), x, y);
        me->GetMotionMaster()->MovePoint(0, x, y, me->GetPositionZ());

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            Talk(TALK_INTRO);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED | UNIT_FLAG_IMMUNE_TO_PC);

            if (Creature* lyalia = me->FindNearestCreature(NPC_LYALIA, 40.0f, true))
                me->Kill(lyalia);

            if (Creature* kor = me->FindNearestCreature(NPC_KOR_BLOODTUSK, 40.0f, true))
                me->Kill(kor);

            if (Player* owner = ObjectAccessor::GetPlayer(*me, targetGUID))
            {
                std::list<TempSummon*> lyaliaFriends;
                owner->GetSummons(lyaliaFriends, NPC_SUNWALKER_DEZCO_QUEST);

                if (!lyaliaFriends.empty())
                    lyaliaFriends.front()->AI()->Talk(TALK_INTRO);
            }
        });
    }

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
    }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_EARTH_SPIKE, randtime(3s + 500ms, 12s));
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* owner = ObjectAccessor::GetPlayer(*me, targetGUID))
        {
            std::list<TempSummon*> lyaliaFriends;
            owner->GetSummons(lyaliaFriends, NPC_SUNWALKER_DEZCO_QUEST);

            if (!lyaliaFriends.empty())
                lyaliaFriends.front()->AI()->DoAction(0);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            ExecuteTargetEvent(SPELL_EARTH_SPIKE, 10 * IN_MILLISECONDS, EVENT_EARTH_SPIKE, eventId);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

// Sunwalker Dezco 58666
struct npc_krasarang_wilds_sunwalker_dezco_reclaimer : public ScriptedAI
{
    npc_krasarang_wilds_sunwalker_dezco_reclaimer(Creature* creature) : ScriptedAI(creature) { }

    TaskScheduler scheduler;
    ObjectGuid lorekeeperGUID;
    ObjectGuid targetGUID;

    void IsSummonedBy(Unit* summoner) override
    {
        targetGUID = summoner->GetGUID();
    }

    void DoAction(int32 actionId) override
    {
        if (Player* owner = ObjectAccessor::GetPlayer(*me, targetGUID))
        {
            std::list<TempSummon*> lyaliaFriends;
            owner->GetSummons(lyaliaFriends, NPC_LOREKEEPER_VAELDRIN);

            if (!lyaliaFriends.empty())
            {
                lyaliaFriends.front()->AI()->Talk(TALK_INTRO);
                lorekeeperGUID = lyaliaFriends.front()->GetGUID();
            }
        }

        uint32 delay = 3500;
        scheduler
            .Schedule(Milliseconds(delay), [this](TaskContext context)
        {
            Talk(TALK_SPECIAL_1);
        });

        scheduler
            .Schedule(Milliseconds(delay += 5100), [this](TaskContext context)
        {
            if (Creature* lorekeeper = ObjectAccessor::GetCreature(*me, lorekeeperGUID))
                lorekeeper->AI()->Talk(TALK_SPECIAL_1);
        });

        scheduler
            .Schedule(Milliseconds(delay += 4000), [this](TaskContext context)
        {
            if (Creature* lorekeeper = ObjectAccessor::GetCreature(*me, lorekeeperGUID))
                lorekeeper->AI()->Talk(TALK_SPECIAL_2);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);
    }
};

// 64950 Must be as properly scripted event "Wanderer's Festival"...someday...maybe...
struct npc_lorewalker_cho_wanderers_festival : public ScriptedAI
{
    npc_lorewalker_cho_wanderers_festival(Creature* c) : ScriptedAI(c) { }

    enum { SPELL_WANDERERS_DREAMERS_AND_YOU_CREDIT = 131331 };

    void sOnGameEvent(bool, uint16 eventId)
    {
        if (eventId == 96)
        {
            std::list<Player*> players;
            me->GetPlayerListInGrid(players, 80.0f);
            for (auto&& player : players)
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_WANDERERS_DREAMERS_AND_YOU_CREDIT);
        }
    }
};

enum MysteriousWhirlpoolData
{
    NPC_ENTRY_NARJON                        = 59689,
    NARJON_TEX_01                           = 0
};

class go_mysterious_whirlpool : public GameObjectScript
{
    public:
        go_mysterious_whirlpool() : GameObjectScript("go_mysterious_whirlpool") { }

        bool OnGossipHello(Player* player, GameObject* go) override
        {
            if (go->FindNearestCreature(NPC_ENTRY_NARJON, 100.0f, true))
                return false;

            ObjectGuid guid = player->GetGUID();

            if (Creature* narjon = player->SummonCreature(NPC_ENTRY_NARJON, { -1126.79f, 1368.609f, 18.94868f, 5.59949f }))
            {
                narjon->SetDisableGravity(true);
                narjon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                narjon->AI()->Talk(NARJON_TEX_01);
                narjon->m_Events.Schedule(5000, [narjon, go]
                {
                    narjon->GetMotionMaster()->MovePoint(1, { -1100.35f, 1347.089f, 18.97906f });
                    narjon->SetSpeed(MOVE_WALK, 2.5f);
                    narjon->OverrideInhabitType(INHABIT_AIR);
                });
                narjon->m_Events.Schedule(12000, [narjon, guid]
                {
                    narjon->SetHomePosition(narjon->GetPosition());
                    narjon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                    narjon->SetControlled(true, UNIT_STATE_ROOT);
                    if (Player* player = ObjectAccessor::GetPlayer(*narjon, guid))
                        narjon->AI()->AttackStart(player);
                });
            }
            return true;
        }
};

// http://www.wowhead.com/quest=30274/the-arcanic-oubliette
enum QuestTheArcanicOublietteData
{
    QUESTID_THE_ARCANIC_OUBLIETTE           = 30274,
    SPELL_ARCANIC_OUBLIETTE                 = 113119,
    NPC_KANG_BRAMBLESTAFF                   = 56114,
    KANG_BRAMBLESTAFF_TEXT_TO_PLAYER        = 0
};

class at_the_arcanic_oubliette : public AreaTriggerScript
{
    public:
        at_the_arcanic_oubliette() : AreaTriggerScript("at_the_arcanic_oubliette") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (!player->isDead() && player->GetQuestStatus(QUESTID_THE_ARCANIC_OUBLIETTE) == QUEST_STATUS_INCOMPLETE)
            {
                player->CastSpell(player, SPELL_ARCANIC_OUBLIETTE, true);

                if (Creature* kang_bramblestaff = player->FindNearestCreature(NPC_KANG_BRAMBLESTAFF, 20.0f))
                    kang_bramblestaff->AI()->Talk(KANG_BRAMBLESTAFF_TEXT_TO_PLAYER, player);
            }
            return false;
        }
};

enum FrenziedReefSharkTexts
{
    TEXT_01                                 = 0
};

enum FrenziedReefSharkSpells
{
    SPELL_SHARK_RIDE_GRIP_AURA              = 116668
};

enum FrenziedReefSharkEvents
{
    EVENT_DECREASE_POWER                    = 1,
    EVENT_CHECK_POWER                       = 2
};

enum FrenziedReefSharkMisc
{
    QUEST_JUMPING_THE_SHARK                 = 30753
};

// Frenzied Reef Shark - 60408
struct npc_frenzied_reef_shark : public VehicleAI
{
    npc_frenzied_reef_shark(Creature* creature) : VehicleAI(creature) { }

    EventMap events;

    void Reset() override
    {
        VehicleAI::Reset();

        events.Reset();
        me->GetMotionMaster()->MoveRandom(5.0f);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (!apply)
        {
            if (Player* passenger = who->ToPlayer())
                passenger->RemoveAura(SPELL_SHARK_RIDE_GRIP_AURA);

            me->DespawnOrUnsummon(1);
            VehicleAI::Reset();
        }
    }

    void OnSpellClick(Unit* clicker, bool& result) override
    {
        VehicleAI::OnSpellClick(clicker, result);

        if (clicker->GetTypeId() != TYPEID_PLAYER)
            return;

        if (clicker->ToPlayer()->GetQuestStatus(QUEST_JUMPING_THE_SHARK) != QUEST_STATUS_INCOMPLETE)
            return;

        if (me->GetVehicleKit() && me->GetVehicleKit()->HasEmptySeat(0) && !clicker->IsOnVehicle())
        {
            if (clicker->HasAura(SPELL_SHARK_RIDE_GRIP_AURA))
                clicker->RemoveAura(SPELL_SHARK_RIDE_GRIP_AURA);

            clicker->EnterVehicle(me, 0);
            clicker->CastSpell(clicker, SPELL_SHARK_RIDE_GRIP_AURA, true);
            clicker->SetPower(POWER_ALTERNATE_POWER, 50);
            me->GetMotionMaster()->MovePoint(0, { -1455.209f, -256.231f, -1.524521f, 4.992734f });
            me->m_Events.Schedule(4000, [this]()
            {
                me->GetMotionMaster()->MovePoint(0, { -1484.709f, -298.791f, -1.524468f, 4.039788f });
            });
            me->m_Events.Schedule(8000, [this]()
            {
                if (me->GetVehicleKit() && !me->GetVehicleKit()->HasEmptySeat(0))
                    if (Player* passenger = me->GetVehicleKit()->GetPassenger(0)->ToPlayer())
                        Talk(TEXT_01, passenger);
                me->GetMotionMaster()->MovePath(me->GetEntry(), true);
            });
            events.ScheduleEvent(EVENT_DECREASE_POWER, 3s);
            events.ScheduleEvent(EVENT_CHECK_POWER, 3s);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (me->GetVehicleKit() && !me->GetVehicleKit()->HasEmptySeat(0))
            if (Player* passenger = me->GetVehicleKit()->GetPassenger(0)->ToPlayer())
                passenger->RemoveAura(SPELL_SHARK_RIDE_GRIP_AURA);
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);

        if (!me->GetVehicleKit() || me->GetVehicleKit()->HasEmptySeat(0))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DECREASE_POWER:
                    if (Player* passenger = me->GetVehicleKit()->GetPassenger(0)->ToPlayer())
                        if (int32 power = passenger->GetPower(POWER_ALTERNATE_POWER))
                            passenger->SetPower(POWER_ALTERNATE_POWER, power - 5);
                    events.ScheduleEvent(EVENT_DECREASE_POWER, 3s);
                    break;
                case EVENT_CHECK_POWER:
                    if (Player* passenger = me->GetVehicleKit()->GetPassenger(0)->ToPlayer())
                    {
                        if (passenger->GetPower(POWER_ALTERNATE_POWER) <= 0)
                        {
                            passenger->RemoveAura(SPELL_SHARK_RIDE_GRIP_AURA);
                            me->DespawnOrUnsummon();
                            VehicleAI::Reset();
                            return;
                        }
                    }
                    events.ScheduleEvent(EVENT_CHECK_POWER, 3s);
                    break;
            }
        }
    }
};

Position guardiansPos[4] =
{
    { -1157.97f, 494.984f, 60.8149f, 6.22643f },
    { -1112.00f, 496.151f, 61.2683f, 3.18850f },
    { -1109.17f, 536.760f, 61.0410f, 3.15622f },
    { -1158.51f, 537.078f, 61.9583f, 6.22643f },
};

Position waterPos[17] =
{
    { -1118.24f, 526.908f, 60.3002f, 1.10080f },
    { -1155.48f, 521.648f, 60.2913f, 4.85362f },
    { -1141.55f, 491.727f, 59.9428f,     0.0f },
    { -1149.42f, 525.325f, 59.9428f, 4.85362f },
    { -1144.60f, 497.361f, 60.2956f,     0.0f },
    { -1149.97f, 511.602f, 60.2929f, 4.85362f },
    { -1123.84f, 496.293f, 60.3002f,     0.0f },
    { -1122.19f, 509.057f, 59.9428f, 1.10080f },
    { -1115.11f, 508.597f, 60.2913f, 1.10080f },
    { -1144.28f, 529.139f, 60.2956f, 4.85362f },
    { -1121.62f, 518.418f, 60.2854f, 1.10080f },
    { -1113.86f, 519.984f, 60.2929f, 1.10080f },
    { -1138.75f, 485.215f, 60.2913f,     0.0f },
    { -1144.51f, 517.332f, 60.2854f, 4.85362f },
    { -1132.94f, 495.464f, 60.2854f,     0.0f },
    { -1128.04f, 489.254f, 60.2929f,     0.0f },
    { -1142.42f, 508.441f, 60.3002f, 4.85362f },
};

Position brojaiPos = { -1132.637f, 527.422f, 59.307f, 4.680599f };

enum NaLekTexts
{
    NA_LEK_TEXT_01                          = 0,
    GROUNDBREAKER_BROJAI_TEXT_01            = 0,
    GROUNDBREAKER_BROJAI_TEXT_02            = 1,
    KANG_BRAMBLESTAFF_TEXT_01               = 0
};

enum NaLekSpells
{
    SPELL_MOGU_RUNE_PRISON                  = 129039,
    SPELL_COSMETIC_TELEPORT_EFFECT          = 52096,
    SPELL_DISPERSE                          = 129042,
    SPELL_DISPERSE_2                        = 129043
};

enum NaLekCreatures
{
    NPC_ENTRY_GROUNDBREAKER_BROJAI          = 58606,
    NPC_ENTRY_SLEEPING_GUARDIAN             = 58419,
    NPC_ENTRY_KANG_BRAMBLESTAFF             = 58206
};

enum NaLekObjects
{
    OBJECT_WATER_OF_YOUTH                   = 210229
};

// Na Lek - 55597
struct npc_na_lek : public ScriptedAI
{
    npc_na_lek(Creature* creature) : ScriptedAI(creature), summons(me)
    {
        counter = 0;
    }

    SummonList summons;
    uint32 counter;

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->CLOSE_GOSSIP_MENU();
        me->RemoveAura(SPELL_MOGU_RUNE_PRISON);
        me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        ObjectGuid guid = player->GetGUID();
        me->m_Events.Schedule(2000, [this]()
        {
            me->SetFacingTo(me->GetAngle(brojaiPos.GetPositionX(), brojaiPos.GetPositionY()));
        });
        me->m_Events.Schedule(5000, [this]()
        {
            if (Creature* brojai = me->SummonCreature(NPC_ENTRY_GROUNDBREAKER_BROJAI, brojaiPos))
                brojai->CastSpell(brojai, SPELL_COSMETIC_TELEPORT_EFFECT, true);
        });
        me->m_Events.Schedule(10000, [this, guid]()
        {
            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                if (Creature* brojai = me->FindNearestCreature(NPC_ENTRY_GROUNDBREAKER_BROJAI, 50.0f))
                    brojai->AI()->Talk(GROUNDBREAKER_BROJAI_TEXT_01, player);
        });
        me->m_Events.Schedule(17000, [this]()
        {
            if (Creature* brojai = me->FindNearestCreature(NPC_ENTRY_GROUNDBREAKER_BROJAI, 50.0f))
                brojai->AI()->Talk(GROUNDBREAKER_BROJAI_TEXT_02);
        });
        me->m_Events.Schedule(18000, [this, guid]()
        {
            if (Creature* brojai = me->FindNearestCreature(NPC_ENTRY_GROUNDBREAKER_BROJAI, 50.0f))
                brojai->DespawnOrUnsummon();

            if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
            {
                for (auto&& guid : summons)
                {
                    if (Creature* summon = ObjectAccessor::GetCreature(*me, guid))
                    {
                        summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                        summon->SetStandState(UNIT_STAND_STATE_STAND);
                        summon->AI()->AttackStart(player);
                    }
                }
            }
        });
        // reset event
        me->m_Events.Schedule(120000, [this]()
        {
            summons.DespawnAll();
            me->DespawnOrUnsummon();
        });
        return true;
    }

    void Reset() override
    {
        me->CastSpell(me, SPELL_MOGU_RUNE_PRISON, true);
        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        counter = 0;
        summons.DespawnAll();

        for (uint32 i = 0; i < 4; i++)
            me->SummonCreature(NPC_ENTRY_SLEEPING_GUARDIAN, guardiansPos[i]);
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        if (summon->GetEntry() == NPC_ENTRY_SLEEPING_GUARDIAN)
        {
            summon->SetStandState(UNIT_STAND_STATE_SIT);
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
        }
    }

    void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) override
    {
        counter++;

        if (counter >= 4)
        {
            me->m_Events.KillAllEvents(true);
            me->CastSpell(me, SPELL_DISPERSE);
            summons.DespawnAll();

            for (uint32 i = 0; i < 10; i++)
                me->CastSpell(me, SPELL_DISPERSE_2);

            Talk(NA_LEK_TEXT_01);

            for (uint32 i = 0; i < 17; i++)
                if (GameObject* water = me->SummonGameObject(OBJECT_WATER_OF_YOUTH, waterPos[i].GetPositionX(), waterPos[i].GetPositionY(), waterPos[i].GetPositionZ(), waterPos[i].GetOrientation(), { }, 10 * IN_MILLISECONDS))
                    water->ForcedDespawn(30000);
            me->m_Events.Schedule(4000, [this]()
            {
                if (Creature* kang = me->FindNearestCreature(NPC_ENTRY_KANG_BRAMBLESTAFF, 50.0f))
                    kang->AI()->Talk(KANG_BRAMBLESTAFF_TEXT_01);
            });
            me->DespawnOrUnsummon(6000);
        }
    }
};

enum Data
{
    SPELL_SUMMON_TAK_TAK_S_KITE_C           = 135364,
    SPELL_SUMMON_FENNIE_S_KITE_L            = 135395,

    NPC_ENTRY_TAK_TAK_S_KITE                = 68720,
    NPC_ENTRY_FENNIE_S_KITE                 = 68748,
    NPC_ENTRY_TAK_TAK                       = 68718,
    NPC_ENTRY_FENNIE_HORNSWAGGLE            = 68740,

    TEXT_01_H                               = 0,
    TEXT_02_H                               = 1,
    TEXT_03_H                               = 2,
    TEXT_04_H                               = 3,
    TEXT_05_H                               = 4,
    TEXT_06_H                               = 5,

    TEXT_01_A                               = 0,
    TEXT_02_A                               = 1,
    TEXT_03_A                               = 2,
    TEXT_04_A                               = 3,
    TEXT_05_A                               = 4,
    TEXT_06_A                               = 5,
    TEXT_07_A                               = 6,
    TEXT_08_A                               = 7,
    TEXT_09_A                               = 8,
    TEXT_10_A                               = 9,
    TEXT_11_A                               = 10
};

struct npc_the_bell_peaks_start : public ScriptedAI
{
    npc_the_bell_peaks_start(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 /*action*/) override
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, player->GetTeamId() == TEAM_HORDE ? SPELL_SUMMON_TAK_TAK_S_KITE_C : SPELL_SUMMON_FENNIE_S_KITE_L);
        Creature* kite = GetClosestCreatureWithEntry(me, player->GetTeamId() == TEAM_HORDE ? NPC_ENTRY_TAK_TAK_S_KITE : NPC_ENTRY_FENNIE_S_KITE, 50.0f, true);

        if (!kite)
            return false;

        Creature* starter = kite->SummonCreature(player->GetTeamId() == TEAM_HORDE ? NPC_ENTRY_TAK_TAK : NPC_ENTRY_FENNIE_HORNSWAGGLE, kite->GetPosition(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 420 * IN_MILLISECONDS);
        ObjectGuid guid = player->GetGUID();

        if (starter)
        {
            player->EnterVehicle(kite, 0);
            starter->EnterVehicle(kite, 1);

            kite->SetSpeed(MOVE_WALK, 15.0f);
            kite->OverrideInhabitType(INHABIT_AIR);

            kite->GetMotionMaster()->MovePath(kite->GetEntry(), false);

            uint32 delay = 0;

            if (player->GetTeamId() == TEAM_HORDE)
            {
                kite->m_Events.Schedule(delay += 1 * IN_MILLISECONDS,  [starter]() { starter->AI()->Talk(TEXT_01_H); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_02_H); });
                kite->m_Events.Schedule(delay += 20 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_03_H); });
                kite->m_Events.Schedule(delay += 20 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_04_H); });
                kite->m_Events.Schedule(delay += 95 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_05_H); });
                kite->m_Events.Schedule(delay += 5 * IN_MILLISECONDS,  [starter]() { starter->AI()->Talk(TEXT_06_H); });
                kite->m_Events.Schedule(delay += 30 * IN_MILLISECONDS, [starter, guid, kite]()
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*starter, guid))
                        player->KilledMonsterCredit(68726);
                });
            }
            else
            {
                kite->m_Events.Schedule(delay += 2 * IN_MILLISECONDS,  [starter]() { starter->AI()->Talk(TEXT_01_A); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_02_A); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_03_A); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_04_A); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_05_A); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_06_A); });
                kite->m_Events.Schedule(delay += 15 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_07_A); });
                kite->m_Events.Schedule(delay += 20 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_08_A); });
                kite->m_Events.Schedule(delay += 60 * IN_MILLISECONDS, [starter]() { starter->AI()->Talk(TEXT_09_A); });
                kite->m_Events.Schedule(delay += 5 * IN_MILLISECONDS,  [starter]() { starter->AI()->Talk(TEXT_10_A); });
                kite->m_Events.Schedule(delay += 7 * IN_MILLISECONDS,  [starter]() { starter->AI()->Talk(TEXT_11_A); });
                kite->m_Events.Schedule(delay += 5 * IN_MILLISECONDS,  [starter, guid, kite]()
                {
                    if (Player* player = ObjectAccessor::GetPlayer(*starter, guid))
                        player->KilledMonsterCredit(68748);
                });
            }
        }
        return true;
    }

};

enum LorekeeperVaeldrinData
{
    QUEST_THE_WATERS_OF_YOUTH               = 30445,
    QUEST_IMMORTALITY                       = 30348,
    SPELL_TRANSFER_LIFE                     = 111849,
    NPC_ENTRY_LOREKEEPER_VAELDRIN           = 58745,
    NPC_ENTRY_LOREKEEPER_VAELDRIN_NEW       = 58970,
    NPC_ENTRY_LYALIA_T                      = 58735,
    NPC_ENTRY_LYALIA                        = 58821,
    NPC_ENTRY_LYALIA_NEW                    = 58833,
    TEXT_LOREKEEPER_VAELDRIN_01             = 0,
    TEXT_LOREKEEPER_VAELDRIN_02             = 1,
    TEXT_LOREKEEPER_VAELDRIN_03             = 2,
    TEXT_LYALIA_01                          = 0,
    TEXT_LOREKEEPER_VAELDRIN_NEW_01         = 0,
    TEXT_LOREKEEPER_VAELDRIN_NEW_02         = 1,
    TEXT_LOREKEEPER_VAELDRIN_NEW_03         = 2,
    TEXT_LOREKEEPER_VAELDRIN_NEW_04         = 3,
    TEXT_LYALIA_NEW_01                      = 0,
};

// Lorekeeper Vaeldrin 58745
struct npc_lorekeeper_vaeldrin : public ScriptedAI
{
    npc_lorekeeper_vaeldrin(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        if (me->GetAreaId() == 6009)
            me->HandleEmoteStateCommand(EMOTE_STATE_READ);
    }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        ObjectGuid playerGuid = player->GetGUID();
        uint32 delay = 0;

        std::function<void()> reset = [this]()
        {
            me->SetVisible(true);
            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            if (Creature* lyalia = me->FindNearestCreature(NPC_ENTRY_LYALIA, 50.0f))
            {
                lyalia->SetVisible(true);
                lyalia->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        };

        if (quest->GetQuestId() == QUEST_IMMORTALITY && me->GetAreaId() == 6009)
        {
            me->SetVisible(false);
            Creature* vaeldrinNew = player->SummonCreature(NPC_ENTRY_LOREKEEPER_VAELDRIN, me->GetPosition());

            if (!vaeldrinNew)
            {
                reset();
                return;
            }

            vaeldrinNew->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            vaeldrinNew->HandleEmoteStateCommand(EMOTE_ONESHOT_NONE);
            vaeldrinNew->m_Events.Schedule(delay +=2000, [vaeldrinNew]
            {
                vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_01);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew]
            {
                vaeldrinNew->HandleEmoteCommand(EMOTE_ONESHOT_USE_STANDING);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew]
            {
                vaeldrinNew->HandleEmoteCommand(EMOTE_ONESHOT_EAT);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew]
            {
                vaeldrinNew->CastSpell(vaeldrinNew, 112853);
                vaeldrinNew->SetStandState(UNIT_STAND_STATE_KNEEL);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew]
            {
                if (Creature* lyalia = vaeldrinNew->FindNearestCreature(NPC_ENTRY_LYALIA_T, 50.0f))
                    lyalia->AI()->Talk(TEXT_LYALIA_01);
            });
            vaeldrinNew->m_Events.Schedule(delay += 2000, [vaeldrinNew]
            {
                vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_02);
            });
            vaeldrinNew->m_Events.Schedule(delay += 8000, [playerGuid]
            {
                if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                    player->KilledMonsterCredit(58894);
            });
            vaeldrinNew->m_Events.Schedule(delay += 2000, [vaeldrinNew]
            {
                vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_03);
                vaeldrinNew->SetStandState(UNIT_STAND_STATE_STAND);
            });
            vaeldrinNew->m_Events.Schedule(delay += 2000, [vaeldrinNew, reset]
            {
                vaeldrinNew->HandleEmoteStateCommand(EMOTE_STATE_READ);
                vaeldrinNew->DespawnOrUnsummon();
                reset();
            });
        }

        // todo: add phasing
        if (quest->GetQuestId() == QUEST_THE_WATERS_OF_YOUTH && me->GetAreaId() == 6016)
        {
            me->SetVisible(false);
            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            Creature* lyalia = me->FindNearestCreature(NPC_ENTRY_LYALIA, 50.0f);

            if (!lyalia)
            {
                reset();
                return;
            }
            lyalia->SetVisible(false);
            lyalia->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            Creature* vaeldrinNew = me->SummonCreature(NPC_ENTRY_LOREKEEPER_VAELDRIN_NEW, me->GetPosition());
            Creature* lyaliaNew = me->SummonCreature(NPC_ENTRY_LYALIA_NEW, lyalia->GetPosition());

            if (!vaeldrinNew || !lyaliaNew)
            {
                reset();
                return;
            }

            vaeldrinNew->SetFacingTo(me->GetAngle(lyaliaNew));
            vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_NEW_01);
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew]
            {
                vaeldrinNew->SetStandState(UNIT_STAND_STATE_KNEEL);
            });
            vaeldrinNew->m_Events.Schedule(delay += 3000, [vaeldrinNew]
            {
                vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_NEW_02);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew]
            {
                vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_NEW_03);
            });
            vaeldrinNew->m_Events.Schedule(delay += 3000, [vaeldrinNew]
            {
                vaeldrinNew->AI()->Talk(TEXT_LOREKEEPER_VAELDRIN_NEW_04);
                vaeldrinNew->SetStandState(UNIT_STAND_STATE_STAND);
            });
            vaeldrinNew->m_Events.Schedule(delay += 1000, [vaeldrinNew]
            {
                vaeldrinNew->HandleEmoteCommand(EMOTE_ONESHOT_EAT);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew, lyaliaNew]
            {
                vaeldrinNew->CastSpell(lyaliaNew, SPELL_TRANSFER_LIFE);
            });
            vaeldrinNew->m_Events.Schedule(delay += 7000, [vaeldrinNew, lyaliaNew]
            {
                lyaliaNew->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);
            });
            vaeldrinNew->m_Events.Schedule(delay += 2000, [vaeldrinNew, lyaliaNew, playerGuid]
            {
                lyaliaNew->AI()->Talk(TEXT_LYALIA_NEW_01);
                vaeldrinNew->setDeathState(JUST_DIED);

                if (Player* player = ObjectAccessor::GetPlayer(*vaeldrinNew, playerGuid))
                    player->KilledMonsterCredit(58970);
            });
            vaeldrinNew->m_Events.Schedule(delay += 5000, [vaeldrinNew, lyaliaNew, reset]
            {
                lyaliaNew->DespawnOrUnsummon();
                vaeldrinNew->DespawnOrUnsummon();
                reset();
            });
        }
    }
};

void AddSC_krasarang_wilds()
{
    new npc_arness_the_scale();
    new npc_torik_ethis();
    new npc_go_kan();
    new npc_despondent_warden_of_zhu();
    new npc_zhus_watch_courier();
    new creature_script<npc_shieldwall_footman>("npc_shieldwall_footman");
    new creature_script<npc_krasari_tormentor>("npc_krasari_tormentor");
    new creature_script<npc_krasari_runekeeper>("npc_krasari_runekeeper");
    new creature_script<npc_weeping_horror>("npc_weeping_horror");
    new creature_script<npc_bilgewater_sapper>("npc_bilgewater_sapper");
    new creature_script<npc_viceclaw_scuttler>("npc_viceclaw_scuttler");
    new creature_script<npc_dominance_grunt>("npc_dominance_grunt");
    new creature_script<npc_elder_seadragon>("npc_elder_seadragon");
    new creature_script<npc_sunwalker_scout>("npc_sunwalker_scout");
    new creature_script<npc_dojani_dominator>("npc_dojani_dominator");
    new creature_script<npc_dojani_enforcer>("npc_dojani_enforcer");
    new creature_script<npc_dojani_surveyor>("npc_dojani_surveyor");
    new creature_script<npc_dojani_reclaimer>("npc_dojani_reclaimer");
    new creature_script<npc_riverblade_slayer>("npc_riverblade_slayer");
    new creature_script<npc_sha_of_despair>("npc_sha_of_despair");
    new creature_script<npc_echo_of_despair>("npc_echo_of_despair");
    new creature_script<npc_source_of_despair>("npc_source_of_despair");
    new creature_script<npc_child_of_chi_ji_krasarang>("npc_child_of_chi_ji_krasarang");
    new creature_script<npc_student_of_chi_ji>("npc_student_of_chi_ji");
    new creature_script<npc_alliance_landing_boat>("npc_alliance_landing_boat");
    new creature_script<npc_varian_wrynn_krasarang_landing>("npc_varian_wrynn_krasarang_landing");
    new creature_script<npc_hilda_hornswaggle_krasarang_landing>("npc_hilda_hornswaggle_krasarang_landing");
    new creature_script<npc_horde_landing_boat>("npc_horde_landing_boat");
    new creature_script<npc_garrosh_hellscream_krasarang_landing>("npc_garrosh_hellscream_krasarang_landing");
    new creature_script<npc_stoneplow_envoy>("npc_stoneplow_envoy");
    new creature_script<npc_nahassa>("npc_nahassa");
    new creature_script<npc_high_marshal_twinbraid>("npc_high_marshal_twinbraid");
    new creature_script<npc_warlord_bloodhilt>("npc_warlord_bloodhilt");
    new npc_krasarang_wilds_lyalia();
    new creature_script<npc_groundbreaker_brojai>("npc_groundbreaker_brojai");
    new creature_script<npc_krasarang_wilds_sunwalker_dezco_reclaimer>("npc_krasarang_wilds_sunwalker_dezco_reclaimer");
    new creature_script<npc_lorewalker_cho_wanderers_festival>("npc_lorewalker_cho_wanderers_festival");
    new go_mysterious_whirlpool();
    new spell_script<spell_call_of_despair_target_picker>("spell_call_of_despair_target_picker");
    new spell_script<spell_call_of_despair_missle>("spell_call_of_despair_missle");
    new aura_script<spell_krasarang_landing_cinematic>("spell_krasarang_landing_cinematic");
    new aura_script<spell_chaingun>("spell_chaingun");
    new aura_script<spell_cannonball_spin>("spell_cannonball_spin");
    new spell_script<spell_bloodhilt_brutal_charge>("spell_bloodhilt_brutal_charge");
    new atrigger_script<sat_gift_of_chi_ji>("sat_gift_of_chi_ji");
    new at_the_arcanic_oubliette();
    new creature_script<npc_frenzied_reef_shark>("npc_frenzied_reef_shark");
    new creature_script<npc_na_lek>("npc_na_lek");
    new creature_script<npc_the_bell_peaks_start>("npc_the_bell_peaks_start");
    new creature_script<npc_lorekeeper_vaeldrin>("npc_lorekeeper_vaeldrin");
}
