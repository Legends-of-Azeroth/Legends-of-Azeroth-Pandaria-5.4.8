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

#include "ScriptPCH.h"
#include "halls_of_reflection.h"
#include "ScriptedEscortAI.h"
#include "GameObjectAI.h"
#include "Transport.h"
#include "../AI/SmartScripts/SmartAI.h"
#include "Random.h"

enum Yells
{
    SAY_SYLVANAS_INTRO_1                          = 0,
    SAY_SYLVANAS_INTRO_2                          = 1,
    SAY_SYLVANAS_INTRO_3                          = 2,
    SAY_SYLVANAS_INTRO_4                          = 3,
    SAY_SYLVANAS_INTRO_5                          = 4,
    SAY_SYLVANAS_INTRO_6                          = 5,
    SAY_SYLVANAS_INTRO_7                          = 6,
    SAY_SYLVANAS_INTRO_8                          = 7,
    SAY_SYLVANAS_INTRO_END                        = 8,

    SAY_JAINA_INTRO_1                             = 0,
    SAY_JAINA_INTRO_2                             = 1,
    SAY_JAINA_INTRO_3                             = 2,
    SAY_JAINA_INTRO_4                             = 3,
    SAY_JAINA_INTRO_5                             = 4,
    SAY_JAINA_INTRO_6                             = 5,
    SAY_JAINA_INTRO_7                             = 6,
    SAY_JAINA_INTRO_8                             = 7,
    SAY_JAINA_INTRO_9                             = 8,
    SAY_JAINA_INTRO_10                            = 9,
    SAY_JAINA_INTRO_11                            = 10,
    SAY_JAINA_INTRO_END                           = 11,

    SAY_UTHER_INTRO_H2_1                          = 0,
    SAY_UTHER_INTRO_H2_2                          = 1,
    SAY_UTHER_INTRO_H2_3                          = 2,
    SAY_UTHER_INTRO_H2_4                          = 3,
    SAY_UTHER_INTRO_H2_5                          = 4,
    SAY_UTHER_INTRO_H2_6                          = 5,
    SAY_UTHER_INTRO_H2_7                          = 6,
    SAY_UTHER_INTRO_H2_EMOTE                      = 7,
    SAY_UTHER_INTRO_A2_1                          = 8,
    SAY_UTHER_INTRO_A2_2                          = 9,
    SAY_UTHER_INTRO_A2_3                          = 10,
    SAY_UTHER_INTRO_A2_4                          = 11,
    SAY_UTHER_INTRO_A2_5                          = 12,
    SAY_UTHER_INTRO_A2_6                          = 13,
    SAY_UTHER_INTRO_A2_7                          = 14,
    SAY_UTHER_INTRO_A2_8                          = 15,
    SAY_UTHER_INTRO_A2_9                          = 16,
    SAY_UTHER_INTRO_A2_EMOTE                      = 17,

    SAY_LK_INTRO_1                                = 0,
    SAY_LK_INTRO_2                                = 1,
    SAY_LK_INTRO_3                                = 2,
    SAY_LK_SYLVANAS_INTRO_END                     = 3,
    SAY_LK_JAINA_INTRO_END                        = 4,

    SAY_QUELDELAR_INTRO                           = 0,
    SAY_QUELDELAR_COMBAT_REACH,
};

enum Events
{
    EVENT_NONE,

    EVENT_START_INTRO,
    EVENT_SKIP_INTRO,

    EVENT_INTRO_A2_1,
    EVENT_INTRO_A2_2,
    EVENT_INTRO_A2_3,
    EVENT_INTRO_A2_4,
    EVENT_INTRO_A2_5,
    EVENT_INTRO_A2_6,
    EVENT_INTRO_A2_7,
    EVENT_INTRO_A2_8,
    EVENT_INTRO_A2_9,
    EVENT_INTRO_A2_10,
    EVENT_INTRO_A2_11,
    EVENT_INTRO_A2_12,
    EVENT_INTRO_A2_13,
    EVENT_INTRO_A2_14,
    EVENT_INTRO_A2_15,
    EVENT_INTRO_A2_16,
    EVENT_INTRO_A2_17,
    EVENT_INTRO_A2_17_2,
    EVENT_INTRO_A2_18,
    EVENT_INTRO_A2_19,

    EVENT_INTRO_H2_1,
    EVENT_INTRO_H2_2,
    EVENT_INTRO_H2_3,
    EVENT_INTRO_H2_4,
    EVENT_INTRO_H2_5,
    EVENT_INTRO_H2_6,
    EVENT_INTRO_H2_7,
    EVENT_INTRO_H2_8,
    EVENT_INTRO_H2_9,
    EVENT_INTRO_H2_10,
    EVENT_INTRO_H2_11,
    EVENT_INTRO_H2_12,
    EVENT_INTRO_H2_12_2,
    EVENT_INTRO_H2_13,
    EVENT_INTRO_H2_14,
    EVENT_INTRO_H2_15,

    EVENT_INTRO_LK_1,
    EVENT_INTRO_LK_1_2,
    EVENT_INTRO_LK_1_3,
    EVENT_INTRO_LK_2,
    EVENT_INTRO_LK_3,
    EVENT_INTRO_LK_4,
    EVENT_INTRO_LK_4_2,
    EVENT_INTRO_LK_4_3,
    EVENT_INTRO_LK_4_4,
    EVENT_INTRO_LK_4_5,
    EVENT_INTRO_LK_5,
    EVENT_INTRO_LK_6,
    EVENT_INTRO_LK_6_1,
    EVENT_INTRO_LK_7,
    EVENT_INTRO_LK_8,
    EVENT_INTRO_LK_9,

    EVENT_INTRO_END,

    // Quel`Delar
    EVENT_MORTAL_STRIKE,
    EVENT_BLADESTORM,
};

enum Spells
{
    SPELL_CAST_VISUAL                  = 72711,
    SPELL_BOSS_SPAWN_AURA              = 72712, // Falric and Marwyn
    SPELL_UTHER_DESPAWN                = 70693,
    SPELL_TAKE_FROSTMOURNE             = 72729,
    SPELL_FROSTMOURNE_DESPAWN          = 72726,
    SPELL_FROSTMOURNE_VISUAL           = 73220,
    SPELL_FROSTMOURNE_SOUNDS           = 70667,

    // Quel`Delar
    SPELL_VISUAL_WHIRLWIND             = 70300,
    SPELL_MORTAL_STRIKE                = 16856,
    SPELL_WHIRLWIND                    = 67541,
    SPELL_WHIRLWIND_EFF                = 67716,
};

const Position HallsofReflectionLocs[] =
{
    { 5283.234863f, 1990.946777f, 707.695679f, 0.929097f }, // 2 Loralen Follows
    { 5408.031250f, 2102.918213f, 707.695251f, 0.792756f }, // 9 Sylvanas Follows
    { 5401.866699f, 2110.837402f, 707.695251f, 0.800610f }, // 10 Loralen follows
};

const Position SpawnPos              = { 5262.540527f, 1949.693726f, 707.695007f, 0.808736f }; // Jaina/Sylvanas Beginning Position
const Position MoveThronePos         = { 5306.952148f, 1998.499023f, 709.341431f, 1.277278f }; // Jaina/Sylvanas walks to throne
const Position UtherSpawnPos         = { 5307.814000f, 2003.168000f, 709.424400f, 4.537856f };
const Position LichKingSpawnPos      = { 5362.917480f, 2062.307129f, 707.695374f, 3.945812f };
const Position LichKingMoveThronePos = { 5312.080566f, 2009.172119f, 709.341431f, 3.973301f }; // Lich King walks to throne
const Position LichKingMoveAwayPos   = { 5400.069824f, 2102.7131689f, 707.69525f, 0.843803f }; // Lich King walks away

class npc_jaina_and_sylvanas_hor_part1 : public CreatureScript
{
    public:
        npc_jaina_and_sylvanas_hor_part1() : CreatureScript("npc_jaina_and_sylvanas_hor_part1") { }

        // AI of Part1: handle the intro till start of gauntlet event.
        struct npc_jaina_and_sylvanas_hor_part1AI : public ScriptedAI
        {
            npc_jaina_and_sylvanas_hor_part1AI(Creature* creature) : ScriptedAI(creature)
            {
                instance = me->GetInstanceScript();

                if (Player* player = me->FindNearestPlayer(100.0f))
                    playerGUID = player->GetGUID();

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                me->SetVisible(true);

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    Talk(SAY_JAINA_INTRO_1, ObjectAccessor::GetPlayer(*me, playerGUID));
                    me->m_Events.Schedule(2000, [this]() { me->GetMotionMaster()->MovePoint(1, 5263.20f, 1950.96f, 707.6948f); });
                }
                else
                    me->m_Events.Schedule( 800, [this]() { me->GetMotionMaster()->MovePoint(1, 5265.89f, 1952.98f, 707.6978f); });
            }

            void Reset() override
            {
                events.Reset();

                utherGUID = ObjectGuid::Empty;
                lichKingGUID = ObjectGuid::Empty;
            }

            void MovementInform(uint32 type, uint32 point) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (point)
                {
                    case 1:
                        me->SetFacingTo(0.8028514f);
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                        Talk(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? SAY_JAINA_INTRO_2 : SAY_SYLVANAS_INTRO_1, ObjectAccessor::GetPlayer(*me, playerGUID));
                        break;
                    case 2:
                        me->SetFacingTo(1.239184f);
                        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                        {
                            Talk(SAY_JAINA_INTRO_3, ObjectAccessor::GetPlayer(*me, playerGUID));
                            events.ScheduleEvent(EVENT_INTRO_A2_1, 6900);
                        }
                        else
                        {
                            Talk(SAY_SYLVANAS_INTRO_2, ObjectAccessor::GetPlayer(*me, playerGUID));
                            events.ScheduleEvent(EVENT_INTRO_H2_1, 7500);
                        }
                        break;
                }
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 action) override
            {
                if (!player || menuId != (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? 11031 : 10950) || action > 1)
                    return false;

                player->CLOSE_GOSSIP_MENU();
                playerGUID = player->GetGUID();
                events.ScheduleEvent(action ? EVENT_SKIP_INTRO : EVENT_START_INTRO, 0);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                return true;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell) override
            {
                if (spell->Id == SPELL_CAST_VISUAL)
                    me->m_Events.Schedule(1400, [this]() { me->RemoveAurasDueToSpell(SPELL_CAST_VISUAL); });
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_START_INTRO:
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(2, MoveThronePos);

                        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                        {
                            if (Creature* npc = me->SummonCreature(NPC_KORELN, 5232.84f, 1931.634f, 707.7697f))
                            {
                                // Stopping it here, otherwise waypoint movement will be interrupted in SAI OOC update
                                if (npc->GetAIName() == "SmartAI")
                                    CAST_AI(SmartAI, npc->AI())->SetCombatMove(false);

                                uint32 delay = 0;
                                npc->SetWalk(true);
                                npc->GetMotionMaster()->MovePoint(0, 5255.390f, 1956.061f, 707.6948f);
                                npc->m_Events.Schedule(delay += 11700, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5274.258f, 1976.811f, 707.6946f); });
                                npc->m_Events.Schedule(delay += 10800, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5283.226f, 1992.300f, 707.6948f); });
                                npc->m_Events.Schedule(delay +=  9200, [npc]() { npc->SetFacingTo(0.3839724f); });
                            }
                        }
                        else
                        {
                            if (Creature* npc = me->SummonCreature(NPC_LORALEN, 5233.02f, 1931.88f, 707.7608f))
                            {
                                // Stopping it here, otherwise waypoint movement will be interrupted in SAI OOC update
                                if (npc->GetAIName() == "SmartAI")
                                    CAST_AI(SmartAI, npc->AI())->SetCombatMove(false);

                                uint32 delay = 0;
                                npc->SetWalk(true);
                                npc->GetMotionMaster()->MovePoint(0, 5255.390f, 1956.061f, 707.6948f);
                                npc->m_Events.Schedule(delay += 11500, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5274.258f, 1976.811f, 707.6946f); });
                                npc->m_Events.Schedule(delay += 12100, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5283.226f, 1992.300f, 707.6948f); });
                                npc->m_Events.Schedule(delay +=  8500, [npc]() { npc->SetFacingTo(0.3839724f); });
                            }
                        }
                        break;

                    // A2 Intro Events
                    case EVENT_INTRO_A2_1:
                        Talk(SAY_JAINA_INTRO_4, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_2, 6000);
                        break;
                    case EVENT_INTRO_A2_2:
                        DoCast(me, SPELL_CAST_VISUAL);
                        events.ScheduleEvent(EVENT_INTRO_A2_3, 4400);
                        break;
                    case EVENT_INTRO_A2_3:
                        if (Creature* bunny = me->FindNearestCreature(NPC_FROSTMOURNE_ALTAR_BUNNY, 100.0f))
                            bunny->CastSpell(bunny, SPELL_FROSTMOURNE_SOUNDS, true);
                        instance->HandleGameObject(instance->GetGuidData(DATA_FROSTMOURNE), true);
                        events.ScheduleEvent(EVENT_INTRO_A2_4, 9600);
                        break;
                    case EVENT_INTRO_A2_4:
                        // spawn UTHER during speach 2
                        if (Creature* uther = me->SummonCreature(NPC_UTHER, UtherSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            uther->GetMotionMaster()->MoveIdle();
                            uther->SetReactState(REACT_PASSIVE); // be sure he will not aggro arthas
                            utherGUID = uther->GetGUID();
                        }
                        events.ScheduleEvent(EVENT_INTRO_A2_5, 2000);
                        break;
                    case EVENT_INTRO_A2_5:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_1, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_6, 3800);
                        break;
                    case EVENT_INTRO_A2_6:
                        Talk(SAY_JAINA_INTRO_5, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_7, 6000);
                        break;
                    case EVENT_INTRO_A2_7:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_2, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_8, 7200);
                        break;
                    case EVENT_INTRO_A2_8:
                        Talk(SAY_JAINA_INTRO_6, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_9, 1200);
                        break;
                    case EVENT_INTRO_A2_9:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_3, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_10, 10800);
                        break;
                    case EVENT_INTRO_A2_10:
                        Talk(SAY_JAINA_INTRO_7, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_11, 6400);
                        break;
                    case EVENT_INTRO_A2_11:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_4, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_12, 13000);
                        break;
                    case EVENT_INTRO_A2_12:
                        Talk(SAY_JAINA_INTRO_8, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_13, 4800);
                        break;
                    case EVENT_INTRO_A2_13:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_5, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_14, 13000);
                        break;
                    case EVENT_INTRO_A2_14:
                        Talk(SAY_JAINA_INTRO_9, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_15, 12400);
                        break;
                    case EVENT_INTRO_A2_15:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_6, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_16, 25000);
                        break;
                    case EVENT_INTRO_A2_16:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_7, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_17, 4900);
                        break;
                    case EVENT_INTRO_A2_17:
                        Talk(SAY_JAINA_INTRO_10, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_17_2, 3500);
                        break;
                    case EVENT_INTRO_A2_17_2:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_EMOTE, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_18, 1200);
                        break;
                    case EVENT_INTRO_A2_18:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_A2_8, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_A2_19, 11000);
                        break;
                    case EVENT_INTRO_A2_19:
                        Talk(SAY_JAINA_INTRO_11, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_LK_1, 3600);
                        break;

                    // H2 Intro Events
                    case EVENT_INTRO_H2_1:
                        Talk(SAY_SYLVANAS_INTRO_3, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_2, 2000);
                        break;
                    case EVENT_INTRO_H2_2:
                        DoCast(me, SPELL_CAST_VISUAL);
                        events.ScheduleEvent(EVENT_INTRO_H2_3, 4400);
                        break;
                    case EVENT_INTRO_H2_3:
                        instance->HandleGameObject(instance->GetGuidData(DATA_FROSTMOURNE), true);
                        if (Creature* bunny = me->FindNearestCreature(NPC_FROSTMOURNE_ALTAR_BUNNY, 100.0f))
                            bunny->CastSpell(bunny, SPELL_FROSTMOURNE_SOUNDS, true);
                        events.ScheduleEvent(EVENT_INTRO_H2_4, 9700);
                        break;
                    case EVENT_INTRO_H2_4:
                        // spawn UTHER during speach 2
                        if (Creature* uther = me->SummonCreature(NPC_UTHER, UtherSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            uther->GetMotionMaster()->MoveIdle();
                            uther->SetReactState(REACT_PASSIVE); // be sure he will not aggro arthas
                            utherGUID = uther->GetGUID();
                        }
                        events.ScheduleEvent(EVENT_INTRO_H2_5, 2000);
                        break;
                    case EVENT_INTRO_H2_5:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_1, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_6, 11000);
                        break;
                    case EVENT_INTRO_H2_6:
                        Talk(SAY_SYLVANAS_INTRO_4, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_7, 2400);
                        break;
                    case EVENT_INTRO_H2_7:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_2, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_8, 8800);
                        break;
                    case EVENT_INTRO_H2_8:
                        Talk(SAY_SYLVANAS_INTRO_5, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_9, 6000);
                        break;
                    case EVENT_INTRO_H2_9:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_3, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_10, 19200);
                        break;
                    case EVENT_INTRO_H2_10:
                        Talk(SAY_SYLVANAS_INTRO_6, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_11, 2300);
                        break;
                    case EVENT_INTRO_H2_11:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_4, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_12, 22000);
                        break;
                    case EVENT_INTRO_H2_12:
                        Talk(SAY_SYLVANAS_INTRO_7, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_12_2, 3500);
                        break;
                    case EVENT_INTRO_H2_12_2:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_EMOTE, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_13, 2200);
                        break;
                    case EVENT_INTRO_H2_13:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_5, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_14, 12000);
                        break;
                    case EVENT_INTRO_H2_14:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(SAY_UTHER_INTRO_H2_6, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_H2_15, 8800);
                        break;
                    case EVENT_INTRO_H2_15:
                        Talk(SAY_SYLVANAS_INTRO_8, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_LK_1, 3600);
                        break;

                    // Remaining Intro Events common for both faction
                    case EVENT_INTRO_LK_1:
                        // Spawn LK in front of door, and make him move to the sword.
                        if (Creature* lichKing = me->SummonCreature(NPC_LICH_KING_EVENT, LichKingSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            lichKing->SetWalk(true);
                            lichKing->GetMotionMaster()->MovePoint(0, LichKingMoveThronePos);
                            lichKing->SetReactState(REACT_PASSIVE);
                            lichKingGUID = lichKing->GetGUID();

                            if (GameObject* gate = instance->instance->GetGameObject(instance->GetGuidData(DATA_FROSTWORN_DOOR)))
                                gate->SetGoState(GO_STATE_ACTIVE);

                            me->SetFacingToObject(lichKing);
                        }

                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                            uther->AI()->Talk(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? SAY_UTHER_INTRO_A2_9 : SAY_UTHER_INTRO_H2_7, ObjectAccessor::GetPlayer(*me, playerGUID));

                        if (Creature* npc = me->FindNearestCreature(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? NPC_KORELN : NPC_LORALEN, 100.0f))
                            npc->SetFacingTo(0.6981317f);

                        events.ScheduleEvent(EVENT_INTRO_LK_1_2, 2000);
                        break;

                    case EVENT_INTRO_LK_1_2:
                        if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                        {
                            uther->SetFacingTo(0.8377581f);
                            uther->HandleEmoteStateCommand(EMOTE_STATE_COWER);
                        }

                        if (Creature* npc = me->FindNearestCreature(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? NPC_KORELN : NPC_LORALEN, 100.0f))
                            npc->HandleEmoteStateCommand(EMOTE_STATE_READY1H);

                        events.ScheduleEvent(EVENT_INTRO_LK_1_3, 1200);
                        break;

                    case EVENT_INTRO_LK_1_3:
                        if (GameObject* gate = instance->instance->GetGameObject(instance->GetGuidData(DATA_FROSTWORN_DOOR)))
                            gate->SetGoState(GO_STATE_READY);

                        events.ScheduleEvent(EVENT_INTRO_LK_2, 5000);
                        break;

                    case EVENT_INTRO_LK_2:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            lichKing->AI()->Talk(SAY_LK_INTRO_1, ObjectAccessor::GetPlayer(*me, playerGUID));
                        events.ScheduleEvent(EVENT_INTRO_LK_3, 2000);
                        break;

                    case EVENT_INTRO_LK_3:
                         // The Lich King banishes Uther to the abyss.
                         if (Creature* uther = ObjectAccessor::GetCreature(*me, utherGUID))
                         {
                             uther->CastSpell(uther, SPELL_UTHER_DESPAWN);
                             uther->DespawnOrUnsummon(5000);
                             utherGUID = ObjectGuid::Empty;
                         }
                         events.ScheduleEvent(EVENT_INTRO_LK_4, 6800);
                         break;

                    case EVENT_INTRO_LK_4:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            lichKing->SetFacingTo(4.206244f);

                        // He steps forward and removes the runeblade from the heap of skulls.
                        if (GameObject* frostmourne = me->FindNearestGameObject(GO_FROSTMOURNE, 30.0f))
                            frostmourne->SetGoState(GO_STATE_READY);

                        // TODO: Start moving souls after 400ms, 800ms, 1200ms

                        events.ScheduleEvent(EVENT_INTRO_LK_4_2, 1600);
                        break;

                    case EVENT_INTRO_LK_4_2:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            lichKing->AI()->Talk(SAY_LK_INTRO_2, ObjectAccessor::GetPlayer(*me, playerGUID));

                        events.ScheduleEvent(EVENT_INTRO_LK_4_3, 10800);
                        break;

                    case EVENT_INTRO_LK_4_3:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                        {
                            lichKing->CastSpell(lichKing, SPELL_TAKE_FROSTMOURNE, true);
                            lichKing->CastSpell(lichKing, SPELL_FROSTMOURNE_VISUAL, true);

                            me->m_Events.Schedule(1600, [this]()
                            {
                                if (GameObject* frostmourne = me->FindNearestGameObject(GO_FROSTMOURNE, 30.0f))
                                {
                                    frostmourne->SetRespawnTime(MONTH);
                                    frostmourne->UpdateObjectVisibility();
                                }
                            });
                        }

                        me->RemoveAllAuras();

                        events.ScheduleEvent(EVENT_INTRO_LK_4_4, 4800);
                        break;

                    case EVENT_INTRO_LK_4_4:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            lichKing->AI()->Talk(SAY_LK_INTRO_3, ObjectAccessor::GetPlayer(*me, playerGUID));

                        events.ScheduleEvent(EVENT_INTRO_LK_4_5, 4800);
                        break;

                    case EVENT_INTRO_LK_4_5:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                        {
                            me->m_Events.Schedule(12500, [this]()
                            {
                                if (GameObject* gate = instance->instance->GetGameObject(instance->GetGuidData(DATA_FROSTWORN_DOOR)))
                                    gate->SetGoState(GO_STATE_ACTIVE);
                            });
                            me->m_Events.Schedule(29000, [this]()
                            {
                                if (GameObject* gate = instance->instance->GetGameObject(instance->GetGuidData(DATA_FROSTWORN_DOOR)))
                                    gate->SetGoState(GO_STATE_READY);
                            });
                            lichKing->SetWalk(true);
                            lichKing->GetMotionMaster()->MovePoint(0, LichKingMoveAwayPos);
                        }

                        if (Creature* npc = me->FindNearestCreature(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? NPC_KORELN : NPC_LORALEN, 100.0f))
                        {
                            uint32 delay = 0;
                            npc->SetWalk(false);
                            npc->m_Events.Schedule(delay += 15300, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5298.865f, 2016.693f, 707.8198f); });
                            npc->m_Events.Schedule(delay +=  2400, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5317.052f, 2027.351f, 707.6948f); });
                            npc->m_Events.Schedule(delay +=  2300, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5336.175f, 2039.880f, 707.6969f); });
                            npc->m_Events.Schedule(delay +=  3600, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5350.425f, 2053.653f, 707.6958f); });
                            npc->m_Events.Schedule(delay +=  2400, [npc]() { npc->GetMotionMaster()->MovePoint(0, 5360.711f, 2064.797f, 707.6948f); });
                        }

                        events.ScheduleEvent(EVENT_INTRO_LK_5, 500);
                        break;

                    case EVENT_INTRO_LK_5:
                        // summon Falric and Marwyn. then go back to the door
                        if (Creature* falric = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_FALRIC)))
                        {
                            falric->SetVisible(true);
                            falric->CastSpell(falric, SPELL_BOSS_SPAWN_AURA, true);
                            falric->SetWalk(true);
                            falric->m_Events.Schedule(700, [falric]() { falric->GetMotionMaster()->MovePoint(0, 5283.95f, 2030.53f, 709.3191f); });
                        }
                        if (Creature* marwyn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MARWYN)))
                        {
                            marwyn->SetVisible(true);
                            marwyn->CastSpell(marwyn, SPELL_BOSS_SPAWN_AURA, true);
                            marwyn->SetWalk(true);
                            marwyn->m_Events.Schedule(700, [marwyn]() { marwyn->GetMotionMaster()->MovePoint(0, 5335.01f, 1982.37f, 709.3191f); });
                        }

                        me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);

                        events.ScheduleEvent(EVENT_INTRO_LK_6, 5600);
                        break;

                    case EVENT_INTRO_LK_6:
                        if (Creature* falric = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_FALRIC)))
                        {
                            falric->SetWalk(false);
                            falric->AI()->DoAction(ACTION_SAY_INTRO_1);
                            falric->m_Events.Schedule(8500, [falric]() { falric->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION); });
                        }
                        if (Creature* marwyn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MARWYN)))
                        {
                            marwyn->SetWalk(false);
                            marwyn->AI()->DoAction(ACTION_SAY_INTRO_1);
                        }

                        events.ScheduleEvent(EVENT_INTRO_LK_6_1, 4800);
                        break;

                    case EVENT_INTRO_LK_6_1:
                        if (Creature* falric = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_FALRIC)))
                            falric->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                        if (Creature* marwyn = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_MARWYN)))
                            marwyn->HandleEmoteStateCommand(EMOTE_STATE_READY1H);

                        events.ScheduleEvent(EVENT_INTRO_LK_7, 1500);
                        break;

                    case EVENT_INTRO_LK_7:
                        if (Creature* falric = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_FALRIC)))
                            falric->AI()->DoAction(ACTION_SAY_INTRO_2);

                        if (instance)
                        {
                            instance->SetData(DATA_INTRO_EVENT, DONE);
                            instance->SetData(DATA_WAVE_COUNT, SPECIAL);   // start first wave
                        }

                        events.ScheduleEvent(EVENT_INTRO_LK_8, 6600);
                        break;

                    case EVENT_INTRO_LK_8:
                        me->SetSpeed(MOVE_RUN, 10.0f / baseMoveSpeed[MOVE_RUN]);
                        me->GetMotionMaster()->MovePoint(0, LichKingMoveAwayPos);

                        if (Creature* npc = me->FindNearestCreature(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? NPC_KORELN : NPC_LORALEN, 100.0f))
                            npc->GetMotionMaster()->MovePoint(0, 5360.711f, 2064.797f, 707.6948f);

                        Talk(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? SAY_JAINA_INTRO_END : SAY_SYLVANAS_INTRO_END, ObjectAccessor::GetPlayer(*me, playerGUID));

                        events.ScheduleEvent(EVENT_INTRO_END, 500);
                        break;

                    /* Unused on retail
                    case EVENT_INTRO_LK_9:
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            lichKing->AI()->Talk(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? SAY_LK_JAINA_INTRO_END : SAY_LK_SYLVANAS_INTRO_END, playerGUID);

                        events.ScheduleEvent(EVENT_INTRO_END, ?);
                        break;
                    */

                    case EVENT_INTRO_END:
                        if (Creature* npc = me->FindNearestCreature(instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? NPC_KORELN : NPC_LORALEN, 100.0f))
                            npc->DespawnOrUnsummon(30000);

                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            lichKing->DespawnOrUnsummon(30000);

                        me->DespawnOrUnsummon(30000);
                        break;

                    case EVENT_SKIP_INTRO:
                       // Spawn LK in front of door, and make him move to the sword.
                        if (Creature* lichKing = me->SummonCreature(NPC_LICH_KING_EVENT, LichKingSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            lichKing->SetWalk(true);
                            lichKing->GetMotionMaster()->MovePoint(0, LichKingMoveThronePos);
                            lichKing->SetReactState(REACT_PASSIVE);
                            lichKingGUID = lichKing->GetGUID();

                            if (GameObject* gate = instance->instance->GetGameObject(instance->GetGuidData(DATA_FROSTWORN_DOOR)))
                                gate->SetGoState(GO_STATE_ACTIVE);

                            me->m_Events.Schedule(3200, [this]()
                            {
                                if (GameObject* gate = instance->instance->GetGameObject(instance->GetGuidData(DATA_FROSTWORN_DOOR)))
                                    gate->SetGoState(GO_STATE_READY);
                            });
                        }

                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(0, MoveThronePos);
                        me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);

                        events.ScheduleEvent(EVENT_INTRO_LK_4, 20000);
                        break;
                }
            }

        private:
            InstanceScript* instance;
            EventMap events;
            ObjectGuid utherGUID;
            ObjectGuid lichKingGUID;
            ObjectGuid playerGUID;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jaina_and_sylvanas_hor_part1AI(creature);
        }
};

enum TrashTexts
{
    SAY_TRASH_AGGRO = 0, // Unused on retail
    SAY_TRASH_DEATH = 1,
};

enum TrashSpells
{
    // Ghostly Priest
    SPELL_SHADOW_WORD_PAIN                        = 72318,
    SPELL_CIRCLE_OF_DESTRUCTION                   = 72320,
    SPELL_COWER_IN_FEAR                           = 72321,
    SPELL_DARK_MENDING                            = 72322,

    // Phantom Mage
    SPELL_FIREBALL                                = 72163,
    SPELL_FLAMESTRIKE                             = 72169,
    SPELL_FROSTBOLT                               = 72166,
    SPELL_CHAINS_OF_ICE                           = 72171,
    SPELL_HALLUCINATION                           = 72342,

    // Phantom Hallucination (same as phantom mage + HALLUCINATION_2 when dies)
    SPELL_HALLUCINATION_2                         = 72344,

    // Shadowy Mercenary
    SPELL_SHADOW_STEP                             = 72326,
    SPELL_DEADLY_POISON                           = 72329,
    SPELL_ENVENOMED_DAGGER_THROW                  = 72333,
    SPELL_KIDNEY_SHOT                             = 72335,

    // Spectral Footman
    SPELL_SPECTRAL_STRIKE                         = 72198,
    SPELL_SHIELD_BASH                             = 72194,
    SPELL_TORTURED_ENRAGE                         = 72203,

    // Tortured Rifleman
    SPELL_SHOOT                                   = 72208,
    SPELL_CURSED_ARROW                            = 72222,
    SPELL_FROST_TRAP                              = 72215,
    SPELL_ICE_SHOT                                = 72268,

    // Quel'Delar Event
    SPELL_QUELDELAR_AURA                          = 70013,
};

enum TrashEvents
{
    EVENT_TRASH_NONE,

    // Ghostly Priest
    EVENT_SHADOW_WORD_PAIN,
    EVENT_CIRCLE_OF_DESTRUCTION,
    EVENT_COWER_IN_FEAR,
    EVENT_DARK_MENDING,

    // Phantom Mage
    EVENT_FIREBALL_FROSTBOLT,
    EVENT_FLAMESTRIKE_HALLUCINATION,
    EVENT_CHAINS_OF_ICE,

    // Shadowy Mercenary
    EVENT_SHADOW_STEP,
    EVENT_DEADLY_POISON,
    EVENT_ENVENOMED_DAGGER_THROW,
    EVENT_KIDNEY_SHOT,

    // Spectral Footman
    EVENT_SPECTRAL_STRIKE,
    EVENT_SHIELD_BASH,
    EVENT_TORTURED_ENRAGE,

    // Tortured Rifleman
    EVENT_SHOOT,
    EVENT_CURSED_ARROW,
    EVENT_FROST_TRAP,
    EVENT_ICE_SHOT,
};

enum TrashCreatures
{
    NPC_PHANTOM_HALLUCINATION = 38567,
};

struct npc_hor_trash_ai : ScriptedAI
{
    npc_hor_trash_ai(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        events.Reset();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        //Talk(SAY_TRASH_AGGRO, who->GetGUID()); // Unused on retail
    }

    void EnterEvadeMode() override
    {
        ScriptedAI::EnterEvadeMode();

        if (InstanceScript* instance = me->GetInstanceScript())
            instance->SetData(DATA_WAVE_COUNT, NOT_STARTED);
    }

    void JustDied(Unit* killer) override
    {
        if (roll_chance_i(25))
            Talk(SAY_TRASH_DEATH, killer);

        if (InstanceScript* instance = me->GetInstanceScript())
            instance->SetData64(DATA_WAVE_COUNT, me->GetGUID());
    }

    void JustSummoned(Creature* summon) override
    {
        DoZoneInCombat(summon, 100.0f);
    }

    bool CanAIAttack(Unit const* who) const override
    {
        return who->GetExactDist2d(&GauntletEvadeCheckPosition) > GauntletEvadeCheckDistance;
    }

protected:
    EventMap events;
};

class npc_ghostly_priest : public CreatureScript
    {
    public:
        npc_ghostly_priest() : CreatureScript("npc_ghostly_priest") { }

        struct npc_ghostly_priestAI : public npc_hor_trash_ai
        {
            npc_ghostly_priestAI(Creature* creature) : npc_hor_trash_ai(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                npc_hor_trash_ai::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, urand(3000, 8000));
                events.ScheduleEvent(EVENT_CIRCLE_OF_DESTRUCTION, urand(8000, 10000));
                events.ScheduleEvent(EVENT_COWER_IN_FEAR, 10000);
                events.ScheduleEvent(EVENT_DARK_MENDING, DUNGEON_MODE(20000, 5000));
            }

            void AttackStart(Unit* who) override
            {
                if (who && me->Attack(who, true))
                    casterMovement.Chase(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHADOW_WORD_PAIN:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_SHADOW_WORD_PAIN);
                            events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, urand(6000, 12000));
                            return;
                        case EVENT_CIRCLE_OF_DESTRUCTION:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 10.0f))
                                DoCast(target, SPELL_CIRCLE_OF_DESTRUCTION);
                            events.ScheduleEvent(EVENT_CIRCLE_OF_DESTRUCTION, 13000);
                            return;
                        case EVENT_COWER_IN_FEAR:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 20.0f))
                                DoCast(target, SPELL_COWER_IN_FEAR);
                            events.ScheduleEvent(EVENT_COWER_IN_FEAR, 10000);
                            return;
                        case EVENT_DARK_MENDING:
                            // find an ally with missing HP
                            if (Unit* target = DoSelectLowestHpFriendly(40, DUNGEON_MODE(30000, 50000)))
                            {
                                DoCast(target, SPELL_DARK_MENDING);
                                events.ScheduleEvent(EVENT_DARK_MENDING, DUNGEON_MODE(20000, 5000));
                            }
                            else
                            {
                                // no friendly unit with missing hp. re-check in just 5 sec.
                                events.ScheduleEvent(EVENT_DARK_MENDING, 1000);
                            }
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHADOW_WORD_PAIN).Spell(SPELL_CIRCLE_OF_DESTRUCTION).Spell(SPELL_COWER_IN_FEAR);
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_ghostly_priestAI(creature);
        }
};

class npc_phantom_mage : public CreatureScript
{
    public:
        npc_phantom_mage() : CreatureScript("npc_phantom_mage") { }

        struct npc_phantom_mageAI : public npc_hor_trash_ai
        {
            npc_phantom_mageAI(Creature* creature) : npc_hor_trash_ai(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                npc_hor_trash_ai::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_FIREBALL_FROSTBOLT, 0);
                events.ScheduleEvent(EVENT_FLAMESTRIKE_HALLUCINATION, urand(4000, 9000));
                events.ScheduleEvent(EVENT_CHAINS_OF_ICE, 12000);
            }

            void AttackStart(Unit* who) override
            {
                if (who && me->Attack(who, true))
                    casterMovement.Chase(who);
            }

            void UpdateAI(uint32 diff) override
            {
                if (me->HasAuraType(SPELL_AURA_MOD_INVISIBILITY) || !UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FIREBALL_FROSTBOLT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, RAND(SPELL_FIREBALL, SPELL_FROSTBOLT));
                            events.ScheduleEvent(EVENT_FIREBALL_FROSTBOLT, urand(2000, 4000));
                            return;
                        case EVENT_FLAMESTRIKE_HALLUCINATION:
                            if (me->GetEntry() != NPC_PHANTOM_HALLUCINATION && roll_chance_i(25))
                                DoCast(me, SPELL_HALLUCINATION);
                            else if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_FLAMESTRIKE);
                            events.ScheduleEvent(EVENT_FLAMESTRIKE_HALLUCINATION, 15000);
                            return;
                        case EVENT_CHAINS_OF_ICE:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_CHAINS_OF_ICE);
                            events.ScheduleEvent(EVENT_CHAINS_OF_ICE, 15000);
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_FIREBALL).Spell(SPELL_FROSTBOLT).Spell(SPELL_FLAMESTRIKE).Spell(SPELL_CHAINS_OF_ICE);
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_phantom_mageAI(creature);
        }
};

class npc_phantom_hallucination : public CreatureScript
{
    public:
        npc_phantom_hallucination() : CreatureScript("npc_phantom_hallucination") { }

        struct npc_phantom_hallucinationAI : public npc_phantom_mage::npc_phantom_mageAI
        {
            npc_phantom_hallucinationAI(Creature* creature) : npc_phantom_mage::npc_phantom_mageAI(creature) { }

            void JustDied(Unit* killer) override
            {
                DoCastAOE(SPELL_HALLUCINATION_2, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_phantom_hallucinationAI(creature);
        }
};

class npc_shadowy_mercenary : public CreatureScript
{
    public:
        npc_shadowy_mercenary() : CreatureScript("npc_shadowy_mercenary") { }

        struct npc_shadowy_mercenaryAI : public npc_hor_trash_ai
        {
            npc_shadowy_mercenaryAI(Creature* creature) : npc_hor_trash_ai(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                npc_hor_trash_ai::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_SHADOW_STEP, 30000);
                events.ScheduleEvent(EVENT_DEADLY_POISON, 5000);
                events.ScheduleEvent(EVENT_ENVENOMED_DAGGER_THROW, 10000);
                events.ScheduleEvent(EVENT_KIDNEY_SHOT, 20000);
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
                        case EVENT_SHADOW_STEP:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
                            {
                                me->StopMoving();
                                me->GetMotionMaster()->Clear();

                                DoCast(target, SPELL_SHADOW_STEP);

                                AttackStart(target);
                                me->GetMotionMaster()->MoveChase(target);
                            }
                            events.ScheduleEvent(EVENT_SHADOW_STEP, 30000);
                            return;
                        case EVENT_DEADLY_POISON:
                            DoCastVictim(SPELL_DEADLY_POISON);
                            events.ScheduleEvent(EVENT_DEADLY_POISON, urand(7000, 12000));
                            return;
                        case EVENT_ENVENOMED_DAGGER_THROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_ENVENOMED_DAGGER_THROW);
                            events.ScheduleEvent(EVENT_ENVENOMED_DAGGER_THROW, 10000);
                            return;
                        case EVENT_KIDNEY_SHOT:
                            DoCastVictim(SPELL_KIDNEY_SHOT);
                            events.ScheduleEvent(EVENT_KIDNEY_SHOT, 20000);
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_shadowy_mercenaryAI(creature);
        }
};

class npc_spectral_footman : public CreatureScript
{
    public:
        npc_spectral_footman() : CreatureScript("npc_spectral_footman") { }

        struct npc_spectral_footmanAI : public npc_hor_trash_ai
        {
            npc_spectral_footmanAI(Creature* creature) : npc_hor_trash_ai(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                npc_hor_trash_ai::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_SPECTRAL_STRIKE, urand(4000, 8000));
                events.ScheduleEvent(EVENT_SHIELD_BASH, 10000);
                events.ScheduleEvent(EVENT_TORTURED_ENRAGE, urand(8000, 16000));
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
                        case EVENT_SPECTRAL_STRIKE:
                            DoCastVictim(SPELL_SPECTRAL_STRIKE);
                            events.ScheduleEvent(EVENT_SPECTRAL_STRIKE, urand(3000, 13000));
                            return;
                        case EVENT_SHIELD_BASH:
                            DoCastVictim(SPELL_SHIELD_BASH);
                            events.ScheduleEvent(EVENT_SHIELD_BASH, 5000);
                            return;
                        case EVENT_TORTURED_ENRAGE:
                            DoCast(me, SPELL_TORTURED_ENRAGE);
                            events.ScheduleEvent(EVENT_TORTURED_ENRAGE, urand(15000, 17000));
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spectral_footmanAI(creature);
        }
};

class npc_tortured_rifleman : public CreatureScript
{
    public:
        npc_tortured_rifleman() : CreatureScript("npc_tortured_rifleman") { }

        struct npc_tortured_riflemanAI : public npc_hor_trash_ai
        {
            npc_tortured_riflemanAI(Creature* creature) : npc_hor_trash_ai(creature) { }

            void JustEngagedWith(Unit* who) override
            {
                npc_hor_trash_ai::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_SHOOT, 0);
                events.ScheduleEvent(EVENT_CURSED_ARROW, urand(11000, 15000));
                events.ScheduleEvent(EVENT_FROST_TRAP, urand(12000, 22000));
                events.ScheduleEvent(EVENT_ICE_SHOT, 15000);
            }

            void AttackStart(Unit* victim) override
            {
                if (victim && me->Attack(victim, true))
                    casterMovement.Chase(victim);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                casterMovement.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SHOOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_SHOOT);
                            events.ScheduleEvent(EVENT_SHOOT, RAND(2400, 3600)); // yes, RAND
                            return;
                        case EVENT_CURSED_ARROW:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_CURSED_ARROW);
                            events.ScheduleEvent(EVENT_CURSED_ARROW, 18000);
                            return;
                        case EVENT_FROST_TRAP:
                            DoCast(me, SPELL_FROST_TRAP);
                            events.ScheduleEvent(EVENT_FROST_TRAP, 30000);
                            return;
                        case EVENT_ICE_SHOT:
                            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 40.0f))
                                DoCast(target, SPELL_ICE_SHOT);
                            events.ScheduleEvent(EVENT_ICE_SHOT, 15000);
                            return;
                    }
                }

                if (!casterMovement.IsUnableToCast())
                    DoMeleeAttackIfReady();
            }

        private:
            CasterMovement casterMovement = CasterMovement(me, 1000).Spell(SPELL_SHOOT).Spell(SPELL_CURSED_ARROW).Spell(SPELL_ICE_SHOT);
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tortured_riflemanAI(creature);
        }
};

enum GeneralEvent
{
    SAY_AGGRO                    = 0,
    SAY_DEATH                    = 1,

    SPELL_SHIELD_THROWN          = 69222,
    SPELL_SPIKE                  = 69184,
    SPELL_CLONE_NAME             = 57507,
    SPELL_CLONE_MODEL            = 45204,

    // Reflection'Spells
    SPELL_BALEFUL_STRIKE         = 69933,
    SPELL_SPIRIT_BURST           = 69900,
};

class npc_frostworn_general : public CreatureScript
{
    public:
        npc_frostworn_general() : CreatureScript("npc_frostworn_general") { }

        struct npc_frostworn_generalAI : public ScriptedAI
        {
            npc_frostworn_generalAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 uiShieldTimer;
            uint32 uiSpikeTimer;
            uint32 uiCloneTimer;

            void Reset() override
            {
                if (!instance)
                    return;

                uiShieldTimer = 5000;
                uiSpikeTimer = 14000;
                uiCloneTimer = 22000;
                instance->SetData(DATA_FROSWORN_EVENT, NOT_STARTED);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void JustDied(Unit* killer) override
            {
                if (!instance)
                    return;

                Talk(SAY_DEATH, killer);
                instance->SetData(DATA_FROSWORN_EVENT, DONE);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!instance)
                    return;

                if (me->GetVictim())
                    return;

                if (who->GetTypeId() != TYPEID_PLAYER
                    || instance->GetData(DATA_MARWYN_EVENT) != DONE
                    || !me->IsWithinDistInMap(who, 20.0f)
                    ) return;

                if (Player* player = (Player*)who)
                    if (player->IsGameMaster())
                        return;

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                AttackStart(who);
            }

            void JustEngagedWith(Unit* /*victim*/) override
            {
                if (!instance)
                    return;

                Talk(SAY_AGGRO);
                instance->SetData(DATA_FROSWORN_EVENT, IN_PROGRESS);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiShieldTimer < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SHIELD_THROWN);
                    uiShieldTimer = urand(8000, 12000);
                }
                else
                    uiShieldTimer -= diff;

                if (uiSpikeTimer < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SPIKE);
                    uiSpikeTimer = urand(15000, 20000);
                }
                else
                    uiSpikeTimer -= diff;

                if (uiCloneTimer < diff)
                {
                    SummonClones();
                    uiCloneTimer = 60000;
                }
                else
                    uiCloneTimer -= diff;

                DoMeleeAttackIfReady();
            }

            void SummonClones()
            {
                std::list<Unit*> playerList;
                SelectTargetList(playerList, 5, SELECT_TARGET_TOPAGGRO, 0, true);

                if (playerList.empty())
                    return;

                for (auto&& unit : playerList)
                {
                    if (unit)
                    {
                        if (Creature* pReflection = me->SummonCreature(NPC_REFLECTION, unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), unit->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000))
                        {
                            pReflection->SetName(unit->GetName());
                            unit->CastSpell(pReflection, SPELL_CLONE_NAME, true);
                            unit->CastSpell(pReflection, SPELL_CLONE_MODEL, true);
                            pReflection->SetFaction(me->GetFaction());
                            pReflection->AI()->AttackStart(unit);
                        }
                    }
                }

            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_frostworn_generalAI(creature);
        }
};

class npc_spiritual_reflection : public CreatureScript
{
    public:
        npc_spiritual_reflection() : CreatureScript("npc_spiritual_reflection") { }

        struct npc_spiritual_reflectionAI : public ScriptedAI
        {
            npc_spiritual_reflectionAI(Creature* creature) : ScriptedAI(creature) { }

            InstanceScript* instance;    
            uint32 uiStrikeTimer;

            void Reset() override
            {
                uiStrikeTimer = urand(1000, 3000);    
            }

            void JustDied(Unit* killer) override
            {
                DoCast(killer, SPELL_SPIRIT_BURST);
            }
            
            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (uiStrikeTimer < diff)
                {
                    if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_BALEFUL_STRIKE);
                    uiStrikeTimer = urand(3000, 8000);
                }
                else
                    uiStrikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spiritual_reflectionAI(creature);
        }
};

const Position CannonSpawns[4] = 
{
    { 5230.00f, 1658.75f, 802.22f, 0.00f },
    { 5245.74f, 1644.44f, 802.35f, 0.00f },
    { 5260.64f, 1636.07f, 802.16f, 0.00f },
    { 5275.90f, 1633.22f, 802.25f, 0.00f },
};

class npc_jaina_and_sylvanas_hor_part2 : public CreatureScript
{
    public:
        npc_jaina_and_sylvanas_hor_part2() : CreatureScript("npc_jaina_and_sylvanas_hor_part2") { }

        struct npc_jaina_and_sylvanas_hor_part2AI : public npc_escortAI
        {
            npc_jaina_and_sylvanas_hor_part2AI(Creature* creature) : npc_escortAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 StepTimer;
            uint32 Step;
            int32 HoldTimer;
            uint32 Count;
            bool Fight;
            bool Event;
            bool WallCast;

            void Reset() override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS)
                    return;

                StepTimer = 500;
                Step = 0;
                HoldTimer = 0;
                Count = 0;
                Fight = true;
                Event = false;
                WallCast = false;

                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                if (me->GetEntry() == NPC_JAINA_OUTRO)
                {
                    me->CastSpell(me, SPELL_ICE_BARRIER, false);
                    me->HandleEmoteStateCommand(EMOTE_STATE_READY2HL);
                }
                if (instance->GetData(DATA_LICHKING_EVENT) == DONE)
                me->SetVisible(false);
            }

            void AttackStart(Unit* who) override
            {
                if (!who)
                    return;

                if (me->GetEntry() != NPC_SYLVANAS_OUTRO)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) == IN_PROGRESS || Fight != true)
                    return;

                npc_escortAI::AttackStart(who);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (!instance)
                    return;

                instance->SetData(DATA_LICHKING_EVENT, NOT_STARTED);
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 action) override
            {
                if (!player || menuId != (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE ? 10860 : 10909) || action != 0)
                    return false;

                if (instance->GetData(DATA_LICHKING_EVENT) == DONE)
                    return false;

                player->CLOSE_GOSSIP_MENU();
                Start(false, true);
                me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                me->SetTarget(ObjectGuid::Empty);
                me->setActive(true);

                if (instance)
                {
                    instance->SetData64(DATA_ESCAPE_LIDER, me->GetGUID());
                    instance->SetData(DATA_LICHKING_EVENT, IN_PROGRESS);
                }
                return true;
            }

            void WaypointReached(uint32 waypointId) override
            {
                switch (waypointId)
                {
                    case 2:
                        instance->SetData(DATA_ICE_WALL_1, IN_PROGRESS);
                        break;
                    case 4:
                        Talk(SAY_JAINA_SYLVANAS_WALL_01);
                        HoldTimer = 30000;
                        SetEscortPaused(true);
                        DoCast(me, me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_DESTROY_ICE_WALL_A : SPELL_DESTROY_ICE_WALL_H);
                        WallCast = true;
                        break;
                    case 6:
                        instance->SetData(DATA_ICE_WALL_2, IN_PROGRESS);
                        break;
                    case 8:
                        Talk(SAY_JAINA_SYLVANAS_WALL_02);
                        HoldTimer = 30000;
                        SetEscortPaused(true);
                        DoCast(me, me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_DESTROY_ICE_WALL_A : SPELL_DESTROY_ICE_WALL_H);
                        WallCast = true;
                        break;
                    case 9:
                        Talk(SAY_JAINA_SYLVANAS_ESCAPE_01);
                        break;
                    case 11:
                        instance->SetData(DATA_ICE_WALL_3, IN_PROGRESS);
                        break;
                    case 12:
                        Talk(SAY_JAINA_SYLVANAS_WALL_03);
                        HoldTimer = 30000;
                        SetEscortPaused(true);
                        DoCast(me, me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_DESTROY_ICE_WALL_A : SPELL_DESTROY_ICE_WALL_H);
                        WallCast = true;
                        break;
                    case 15:
                        instance->SetData(DATA_ICE_WALL_4, IN_PROGRESS);
                        break;
                    case 16:
                        Talk(SAY_JAINA_SYLVANAS_WALL_04);
                        HoldTimer = 30000;
                        SetEscortPaused(true);
                        DoCast(me, me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_DESTROY_ICE_WALL_A : SPELL_DESTROY_ICE_WALL_H);
                        WallCast = true;
                        break;
                    case 17:
                        Talk(SAY_JAINA_SYLVANAS_ESCAPE_01);
                        break;
                    case 19:
                        Talk(SAY_JAINA_SYLVANAS_ESCAPE_02);
                        break;
                    case 20:
                        SetEscortPaused(true);
                        if (me->GetEntry() == NPC_JAINA_OUTRO)
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY2HL);
                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                            me->HandleEmoteStateCommand(EMOTE_STATE_READY1H);
                        break;
                }
            }

            void MoveInLineOfSight(Unit* /*who*/) override { }

            void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (!instance)
                    return;

                if (instance->GetData(DATA_LICHKING_EVENT) != IN_PROGRESS)
                {
                    damage = 0;
                    return;
                }
            }

            void JumpNextStep(uint32 time)
            {
                StepTimer = time;
                Step++;
            }

            void Intro()
            {
                switch (Step)
                {
                    case 0:
                        me->SetWalk(false);
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                            me->SetTarget(lichKing->GetGUID());
                        JumpNextStep(100);
                        break;
                    case 1:
                        DoCastAOE(me->GetEntry() == NPC_SYLVANAS_OUTRO ? SPELL_CREDIT_FINDING_SYLVANAS : SPELL_CREDIT_FINDING_JAINA, true);
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                        {
                            if (me->GetEntry() == NPC_JAINA_OUTRO)
                                lichKing->AI()->Talk(SAY_LICH_KING_AGGRO_A);
                            if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                                lichKing->AI()->Talk(SAY_LICH_KING_AGGRO_H);
                            lichKing->AI()->AttackStart(me);
                            AttackStart(lichKing);
                        }
                        JumpNextStep(3000);
                        break;
                    case 2:
                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        {
                            Fight = false;
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                                me->GetMotionMaster()->MovePoint(0, (me->GetPositionX() - 5) + rand() % 10, (me->GetPositionY() - 5) + rand() % 10, me->GetPositionZ());
                            JumpNextStep(3000);
                        }
                        else
                            JumpNextStep(100);
                        break;
                    case 3:
                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                            Fight = true;
                        JumpNextStep(100);
                        break;
                    case 4:
                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        {
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                                me->CastSpell(lichKing, SPELL_SYLVANA_STEP, false);
                            JumpNextStep(3000);
                        }
                        else
                            JumpNextStep(100);
                        break;
                    case 5:
                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        {
                            Fight = false;
                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                                me->GetMotionMaster()->MovePoint(0, (me->GetPositionX() - 5) + rand() % 10, (me->GetPositionY() - 5) + rand() % 10, me->GetPositionZ());
                            JumpNextStep(3000);
                        }
                        else
                            JumpNextStep(12000);
                        break;
                    case 6:
                        Fight = true;

                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                        {
                            if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                                DoCast(SPELL_SYLVANA_JUMP);
                            DoCast(lichKing, me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON : SPELL_DARK_ARROW, false);
                            lichKing->AttackStop();
                        }
                        JumpNextStep(1480);
                        break;
                    case 7:
                        me->RemoveAllAuras();
                        me->HandleEmoteStateCommand(EMOTE_STATE_STAND);
                        me->AttackStop();

                        Talk(SAY_JAINA_SYLVANAS_AGGRO);

                        JumpNextStep(3000);
                        break;
                    case 8:
                        me->GetMotionMaster()->MovePoint(0, 5577.187f, 2236.003f, 733.012f);
                        JumpNextStep(10000);
                        break;
                    case 9:
                        me->RemoveAllAuras();
                        me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        Count = 1;
                        JumpNextStep(100);
                        break;
                }
            }

            void Outro()
            {
                switch (Step)
                {
                    case 10:
                        JumpNextStep(10000);
                        break;
                    case 11:
                        JumpNextStep(DAY * IN_MILLISECONDS);
                        break;
                }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_GUNSHIP_ARRIVAL:
                        me->m_Events.Schedule(5000, [this]
                        {
                            me->RemoveAllAuras();

                            if (Transport* gunship = ObjectAccessor::GetTransport(*me, instance->GetGuidData(DATA_GUNSHIP)))
                                gunship->EnableMovement(true);

                            instance->SetData(DATA_LICHKING_EVENT, DONE);

                            if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                                lichKing->DespawnOrUnsummon();
                        });
                        break;
                    case ACTION_GUNSHIP_ARRIVAL_2:
                        me->m_Events.Schedule(5000, [this]
                        {
                            me->m_Events.Schedule(2000, [this] { Talk(SAY_JAINA_SYLVANAS_FINAL_2); });
                            if (me->GetEntry() == NPC_JAINA_OUTRO)
                                me->m_Events.Schedule(17000, [this] { Talk(SAY_JAINA_FINAL_3); });

                            me->HandleEmoteStateCommand(EMOTE_STATE_STAND);

                            DoCastAOE(SPELL_CREDIT_ESCAPING_ARTHAS, true);

                            me->SetGossip(true, me->GetEntry() == NPC_JAINA_OUTRO ? 10930 : 10931);
                            me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        });
                        break;
                    default:
                        break;
                }
            }

            void UpdateEscortAI(uint32 const diff) override
            {
                if (!instance)
                    return;

                DoMeleeAttackIfReady();

                if (instance->GetData(DATA_PHASE) == 4 && instance->GetData(DATA_LICHKING_EVENT) != IN_PROGRESS)
                {
                    if (StepTimer < diff)
                        Intro();
                    else
                        StepTimer -= diff;
                }

                if (instance->GetData(DATA_LICHKING_EVENT) == SPECIAL && instance->GetData(DATA_PHASE) != 6) // End Cinematic
                {
                    instance->SetData(DATA_PHASE, 6);
                    Step = 10;
                }

                if (instance->GetData(DATA_PHASE) == 6)
                {
                    if (StepTimer < diff)
                        Outro();
                    else
                        StepTimer -= diff;
                    return;
                }

                if (WallCast && HoldTimer < 10000 && (instance->GetData(DATA_SUMMONS) == 0 || !me->IsInCombat()))
                {
                    WallCast = false;
                    me->InterruptNonMeleeSpells(false);
                    SetEscortPaused(false);
                    if (Creature* target = me->FindNearestCreature(NPC_ICE_WALL_TARGET, 50.0f))
                        target->DespawnOrUnsummon();
                    if (GameObject* wall = me->FindNearestGameObject(GO_ICE_WALL, 50.0f))
                        wall->SetGoState(GO_STATE_ACTIVE);
                    if (++Count == 5)
                    {
                        if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_LICHKING)))
                        {
                            lichKing->RemoveAurasDueToSpell(SPELL_REMORSELESS_WINTER);
                            lichKing->RemoveAurasDueToSpell(SPELL_PAIN_AND_SUFFERING);
                            lichKing->AI()->DoAction(ACTION_LICH_KING_STOP_FOLLOW);
                            Step = 0;
                        }
                    }
                }
                else
                {
                    HoldTimer -= diff;
                    if (HoldTimer <= 0)
                        HoldTimer = 0;
                }

                return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_jaina_and_sylvanas_hor_part2AI(creature);
        }
};

class at_hor_intro_npc_spawn : public AreaTriggerScript
{
    public:
        at_hor_intro_npc_spawn() : AreaTriggerScript("at_hor_intro_npc_spawn") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger) override
        {
            InstanceScript* instance = player->GetInstanceScript();

            if (instance->GetData(DATA_INTRO_EVENT) != NOT_STARTED)
                return true;

            instance->SetData(DATA_INTRO_EVENT, IN_PROGRESS);
            return true;
        }
};

class at_hor_waves_restarter : public AreaTriggerScript
{
    public:
        at_hor_waves_restarter() : AreaTriggerScript("at_hor_waves_restarter") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            InstanceScript* instance = player->GetInstanceScript();

            if (player->IsGameMaster())
                return true;

            if (instance->GetData(DATA_WAVE_COUNT))
                return true;

            if (instance->GetData(DATA_INTRO_EVENT) == DONE && instance->GetData(DATA_MARWYN_EVENT) != DONE)
            {
                instance->SetData(DATA_WAVE_COUNT, SPECIAL);

                if (Creature* falric = player->GetCreature(*player, instance->GetGuidData(DATA_FALRIC)))
                {
                    if (instance->GetData(DATA_FALRIC_EVENT) != DONE)
                    {
                        falric->CastSpell(falric, SPELL_BOSS_SPAWN_AURA, true);
                        falric->SetVisible(true);
                    }
                }
                if (Creature* marwyn = player->GetCreature(*player, instance->GetGuidData(DATA_MARWYN)))
                {
                    marwyn->AI()->DoAction(ACTION_SAY_GAUNTLET_RESUME_AFTER_WIPE);

                    if (instance->GetData(DATA_MARWYN_EVENT) != DONE)
                    {
                        marwyn->CastSpell(marwyn, SPELL_BOSS_SPAWN_AURA, true);
                        marwyn->SetVisible(true);
                    }
                }
            }
            return true;
        }
};

class npc_queldelar : public CreatureScript
{
    public:
        npc_queldelar() : CreatureScript("npc_queldelar") { }

        struct npc_queldelarAI  : public ScriptedAI
        {
            npc_queldelarAI(Creature* creature) : ScriptedAI(creature) { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who)
                    return;

                if (me->IsWithinDistInMap(who, 20) && who->HasAura(SPELL_QUELDELAR_AURA))
                {
                    me->SummonCreature(NPC_QUELDELAR, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->DisappearAndDie();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_queldelarAI(creature);
        }
};

// Quel`Delar Weapon 37158
struct npc_queldelar_weapon : public customCreatureAI
{
    npc_queldelar_weapon(Creature* creature) : customCreatureAI(creature) 
    {
        isIntro = false;
    }

    TaskScheduler scheduler;
    float x, y, z;
    bool isIntro;

    void Reset() override
    {
        events.Reset();

        if (isIntro)
            return;

        isIntro = true;

        me->SetDisplayFromModel(1);
        Talk(SAY_QUELDELAR_INTRO);
        z = me->GetPositionZ();
        me->OverrideInhabitType(INHABIT_AIR);
        me->UpdateMovementFlags();

        DoCast(me, SPELL_VISUAL_WHIRLWIND);
        // Drow in whirlwind before combat reach.
        Movement::MoveSplineInit init(me);
        for (uint8 i = 1; i < 25; ++i)
        {
            x = me->GetHomePosition().GetPositionX() + 15.0f * cos(Position::NormalizeOrientation(me->GetAngle(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) + (i * M_PI / 12) - M_PI));
            y = me->GetHomePosition().GetPositionY() + 15.0f * sin(Position::NormalizeOrientation(me->GetAngle(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) + (i * M_PI / 12) - M_PI));

            if (i > 11)
                z -= 2.0f;
            else
                z += 2.0f;

            init.Path().push_back(G3D::Vector3(x, y, z));
        }

        init.SetSmooth();
        init.SetFly();
        init.Launch();

        scheduler
            .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
        {
            me->StopMoving();
            me->GetMotionMaster()->MovePoint(0, quelDelarCombatReachPos);

            scheduler
                .Schedule(Milliseconds(me->GetSplineDuration()), [this](TaskContext context)
            {
                me->SetHomePosition(*me);
                Talk(SAY_QUELDELAR_COMBAT_REACH);
                me->RemoveAurasDueToSpell(SPELL_VISUAL_WHIRLWIND);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_PACIFIED);
            });
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MORTAL_STRIKE, 3 * IN_MILLISECONDS);
        events.ScheduleEvent(EVENT_BLADESTORM, 8.5 * IN_MILLISECONDS);
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
            ExecuteTargetEvent(SPELL_WHIRLWIND, 15 * IN_MILLISECONDS, eventId, EVENT_BLADESTORM, PRIORITY_SELF);
            ExecuteTargetEvent(SPELL_MORTAL_STRIKE, 7.5 * IN_MILLISECONDS, eventId, EVENT_MORTAL_STRIKE);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

class go_ice_wall : public GameObjectScript
{
    public:
        go_ice_wall() : GameObjectScript("go_ice_wall") { }

        struct go_ice_wallAI : GameObjectAI
        {
            go_ice_wallAI(GameObject* go) : GameObjectAI(go)
            {
                go->SetObjectScale(2.0f);
                go->SetGoState(GO_STATE_ACTIVE);
            }

            void UpdateAI(uint32 /*diff*/) override
            {
                if (delay && !--delay)
                    me->SetGoState(GO_STATE_READY);
            }

        private:
            uint32 delay = 2;
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_ice_wallAI(go);
        }
};

class transport_hor_gunship : public TransportScript
{
    public:
        transport_hor_gunship() : TransportScript("transport_hor_gunship") { }

        void OnCreate(Transport* transport) override
        {
            if (InstanceScript* instance = transport->GetInstanceScript())
            {
                transport->EnableMovement(instance->GetData(DATA_LICHKING_EVENT) == DONE);
                transport->SetGoState(instance->GetData(DATA_LICHKING_EVENT) == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
            }
        }
};

// 5752
class at_hor_shadow_throne_entrance : public AreaTriggerScript
{
    public:
        at_hor_shadow_throne_entrance() : AreaTriggerScript("at_hor_shadow_throne_entrance") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            InstanceScript* _instance = player->GetInstanceScript();

            if (_instance->GetData(DATA_FROSWORN_EVENT) == DONE &&
                _instance->GetData(DATA_LICHKING_EVENT) != DONE &&
                _instance->GetData(DATA_PHASE) == 2)
                _instance->SetData(DATA_PHASE, 3);

            return true;
        }
};

// 5605
class at_hor_shadow_throne : public AreaTriggerScript
{
    public:
        at_hor_shadow_throne() : AreaTriggerScript("at_hor_shadow_throne") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->IsGameMaster())
                return true;

            InstanceScript* _instance = player->GetInstanceScript();

            if (_instance->GetData(DATA_FROSWORN_EVENT) == DONE &&
                _instance->GetData(DATA_LICHKING_EVENT) != DONE &&
                _instance->GetData(DATA_PHASE) == 3)
                _instance->SetData(DATA_PHASE, 4);

            return true;
        }
};

// 70017 - Gunship Cannon Fire
class spell_hor_gunship_cannon_fire : public SpellScriptLoader
{
    public:
        spell_hor_gunship_cannon_fire() : SpellScriptLoader("spell_hor_gunship_cannon_fire") { }

        class spell_hor_gunship_cannon_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hor_gunship_cannon_fire_AuraScript);

            void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                if (!urand(0, 2))
                {
                    if (GetTarget()->GetEntry() == NPC_GUNSHIP_CANNON_HORDE)
                        GetTarget()->CastSpell((Unit*)NULL, SPELL_GUNSHIP_CANNON_FIRE_MISSILE_HORDE, true);
                    else
                        GetTarget()->CastSpell((Unit*)NULL, SPELL_GUNSHIP_CANNON_FIRE_MISSILE_ALLIANCE, true);
                }
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hor_gunship_cannon_fire_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_hor_gunship_cannon_fire_AuraScript();
        }
};

// Areatrigger 5740
class at_hor_impenetrable_door : public AreaTriggerScript
{
    public:
        at_hor_impenetrable_door() : AreaTriggerScript("at_hor_impenetrable_door") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            if (player->GetInstanceScript() && (player->GetInstanceScript()->GetData(DATA_FALRIC_EVENT) != DONE || player->GetInstanceScript()->GetData(DATA_MARWYN_EVENT) != DONE))
                player->NearTeleportTo(impenetrableDoorPos.GetPositionX(), impenetrableDoorPos.GetPositionY(), impenetrableDoorPos.GetPositionZ(), impenetrableDoorPos.GetOrientation());

            return true;
        }
};

void AddSC_halls_of_reflection()
{
    new npc_jaina_and_sylvanas_hor_part1();
    new npc_jaina_and_sylvanas_hor_part2();
    new npc_ghostly_priest();
    new npc_phantom_mage();
    new npc_phantom_hallucination();
    new npc_shadowy_mercenary();
    new npc_spectral_footman();
    new npc_tortured_rifleman();
    new at_hor_intro_npc_spawn();
    new at_hor_waves_restarter();
    new npc_frostworn_general();
    new npc_spiritual_reflection();
    new npc_queldelar();
    new creature_script<npc_queldelar_weapon>("npc_queldelar_weapon");
    new go_ice_wall();
    new transport_hor_gunship();
    new at_hor_shadow_throne_entrance();
    new at_hor_shadow_throne();
    new spell_hor_gunship_cannon_fire();
    new at_hor_impenetrable_door();
}
