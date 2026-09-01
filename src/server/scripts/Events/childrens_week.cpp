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
#include "SpellAuras.h"
#include "Player.h"
#include "PassiveAI.h"
#include "MoveSpline.h"

enum Orphans
{
    ORPHAN_ORACLE                           = 33533,
    ORPHAN_WOLVAR                           = 33532,
    ORPHAN_BLOOD_ELF                        = 22817,
    ORPHAN_DRAENEI                          = 22818,
    ORPHAN_HUMAN                            = 14305,
    ORPHAN_ORCISH                           = 14444,
};

enum Texts
{
    TEXT_ORACLE_ORPHAN_1                    = 1,
    TEXT_ORACLE_ORPHAN_2                    = 2,
    TEXT_ORACLE_ORPHAN_3                    = 3,

    TEXT_THE_BIGGEST_TREE_EVER              = 4,

    TEXT_ORACLE_ORPHAN_5                    = 5,
    TEXT_ORACLE_ORPHAN_6                    = 6,

    TEXT_MEETING_A_GREAT_ONE_1              = 7,
    TEXT_MEETING_A_GREAT_ONE_2              = 8,
    TEXT_MEETING_A_GREAT_ONE_3              = 9,
    TEXT_MEETING_A_GREAT_ONE_4              = 10,

    TEXT_ORACLE_ORPHAN_11                   = 11,
    TEXT_ORACLE_ORPHAN_12                   = 12,
    TEXT_ORACLE_ORPHAN_13                   = 13,
    TEXT_ORACLE_ORPHAN_14                   = 14,

    TEXT_WOLVAR_ORPHAN_1                    = 1,
    TEXT_WOLVAR_ORPHAN_2                    = 2,
    TEXT_WOLVAR_ORPHAN_3                    = 3,
    TEXT_WOLVAR_ORPHAN_4                    = 4,
    TEXT_WOLVAR_ORPHAN_5                    = 5,
    // 6 - 9 used in Nesingwary script
    TEXT_WOLVAR_ORPHAN_10                   = 10,
    TEXT_WOLVAR_ORPHAN_11                   = 11,
    TEXT_WOLVAR_ORPHAN_12                   = 12,
    TEXT_WOLVAR_ORPHAN_13                   = 13,

    TEXT_WINTERFIN_PLAYMATE_1               = 1,
    TEXT_WINTERFIN_PLAYMATE_2               = 2,

    TEXT_SNOWFALL_GLADE_PLAYMATE_1          = 1,
    TEXT_SNOWFALL_GLADE_PLAYMATE_2          = 2,

    TEXT_SOO_ROO_1                          = 1,
    TEXT_ELDER_KEKEK_1                      = 1,

    TEXT_ALEXSTRASZA_2                      = 2,
    TEXT_KRASUS_8                           = 8,

    TEXT_THE_BANSHEE_QUEEN_ORPHAN_1         = 1,
    TEXT_THE_BANSHEE_QUEEN_ORPHAN_2         = 2,
    TEXT_THE_BANSHEE_QUEEN_AMBASSADOR       = 1,
    TEXT_THE_BANSHEE_QUEEN_SYLVANAS_1       = 2,
    TEXT_THE_BANSHEE_QUEEN_SYLVANAS_2       = 3,
    TEXT_THE_BANSHEE_QUEEN_ORPHAN_3         = 3,
    TEXT_THE_BANSHEE_QUEEN_SYLVANAS_3       = 4,

    TEXT_THE_FALLEN_CHIEFTAIN_ORPHAN        = 4,

    TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_1  = 1,
    TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_2  = 2,
    TEXT_THE_BIGGEST_DIAMOND_EVER_GUARD_1   = 1,
    TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_3  = 3,
    TEXT_THE_BIGGEST_DIAMOND_EVER_ADVISOR   = 1,
    TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_4  = 4,
    TEXT_THE_BIGGEST_DIAMOND_EVER_GUARD_2   = 2,

    TEXT_MALFURION_HAS_RETURNED_1_ORPHAN    = 5,
    TEXT_MALFURION_HAS_RETURNED_2_ORPHAN    = 6,
    TEXT_MALFURION_HAS_RETURNED_3_ORPHAN    = 7,
    TEXT_MALFURION_HAS_RETURNED_4_MALFURION = 1,
    TEXT_MALFURION_HAS_RETURNED_5_ORPHAN    = 8,
    TEXT_MALFURION_HAS_RETURNED_6_ORPHAN    = 9
};

enum Quests
{
    QUEST_PLAYMATE_WOLVAR                   = 13951,
    QUEST_PLAYMATE_ORACLE                   = 13950,
    QUEST_THE_BIGGEST_TREE_EVER             = 13929,
    QUEST_THE_BRONZE_DRAGONSHRINE_ORACLE    = 13933,
    QUEST_THE_BRONZE_DRAGONSHRINE_WOLVAR    = 13934,
    QUEST_MEETING_A_GREAT_ONE               = 13956,
    QUEST_THE_MIGHTY_HEMET_NESINGWARY       = 13957,
    QUEST_DOWN_AT_THE_DOCKS                 = 910,
    QUEST_GATEWAY_TO_THE_FRONTIER           = 911,
    QUEST_DARK_PORTAL_H                     = 10951,
    QUEST_DARK_PORTAL_A                     = 10952,
    QUEST_LORDAERON_THRONE_ROOM             = 1800,
    QUEST_AUCHINDOUN_AND_THE_RING           = 10950,
    QUEST_TIME_TO_VISIT_THE_CAVERNS_H       = 10963,
    QUEST_TIME_TO_VISIT_THE_CAVERNS_A       = 10962,
    QUEST_THE_SEAT_OF_THE_NARUU             = 10956,
    QUEST_CALL_ON_THE_FARSEER               = 10968,
    QUEST_JHEEL_IS_AT_AERIS_LANDING         = 10954,
    QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE     = 10945,
    QUEST_VISIT_THE_THRONE_OF_ELEMENTS      = 10953,
    QUEST_NOW_WHEN_I_GROW_UP                = 11975,
    QUEST_HOME_OF_THE_BEAR_MEN              = 13930,
    QUEST_THE_DRAGON_QUEEN_ORACLE           = 13954,
    QUEST_THE_DRAGON_QUEEN_WOLVAR           = 13955,
    QUEST_THE_BIGGEST_DIAMOND_EVER          = 29106,
    QUEST_MALFURION_HAS_RETURNED            = 29107,
    QUEST_THE_FALLEN_CHIEFTAIN              = 29176,
    QUEST_THE_BANSHEE_QUEEN                 = 29167,
};

enum Areatriggers
{
    AT_DOWN_AT_THE_DOCKS                    = 3551,
    AT_GATEWAY_TO_THE_FRONTIER              = 3549,
    AT_LORDAERON_THRONE_ROOM                = 3547,
    AT_DARK_PORTAL                          = 4356,

    NPC_CAVERNS_OF_TIME_CW_TRIGGER          = 22872,
    NPC_EXODAR_01_CW_TRIGGER                = 22851,
    NPC_EXODAR_02_CW_TRIGGER                = 22905,
    NPC_AERIS_LANDING_CW_TRIGGER            = 22838,
    NPC_AUCHINDOUN_CW_TRIGGER               = 22831,
    NPC_SPOREGGAR_CW_TRIGGER                = 22829,
    NPC_THRONE_OF_ELEMENTS_CW_TRIGGER       = 22839,
    NPC_SILVERMOON_01_CW_TRIGGER            = 22866,
    NPC_KRASUS                              = 27990,
    NPC_AZEROTH_CHILDRENS_WEEK_TRIGGER      = 34281,
    NPC_NORTHREND_CHILDRENS_WEEK_TRIGGER    = 34381,
};

enum Misc
{
    SPELL_SNOWBALL                          = 21343,
    SPELL_ORPHAN_OUT                        = 58818,
    SPELL_KITE_STRING                       = 45192,
    SPELL_ORPHANS_ICE_CREAM                 = 98381,
    SPELL_MALFURION_GIVES_YOU_WINGS         = 96807,

    DISPLAY_INVISIBLE                       = 11686,

    NPC_AMBASSADOR                          = 16287,
    NPC_SYLVANAS                            = 10181,
    NPC_ADVISOR_BELGRUM                     = 2918,
    NPC_HONOR_GUARD_DUNSTAD                 = 52321,
    NPC_MALFURION                           = 43845,
    NPC_ETYMIDIAN                           = 28092,
};

ObjectGuid getOrphanGUID(Player* player, uint32 orphan)
{
    if (Aura* orphanOut = player->GetAura(SPELL_ORPHAN_OUT))
        if (orphanOut->GetCaster() && orphanOut->GetCaster()->GetEntry() == orphan)
            return orphanOut->GetCaster()->GetGUID();

    return ObjectGuid::Empty;
}

/*######
## npc_winterfin_playmate
######*/
class npc_winterfin_playmate : public CreatureScript
{
    public:
        npc_winterfin_playmate() : CreatureScript("npc_winterfin_playmate") { }

        struct npc_winterfin_playmateAI : public ScriptedAI
        {
            npc_winterfin_playmateAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
            }

            void Reset() override
            {
                timer = 0;
                phase = 0;
                playerGUID = ObjectGuid::Empty;
                orphanGUID = ObjectGuid::Empty;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!phase && who && who->GetDistance2d(me) < 10.0f)
                    if (Player* player = who->ToPlayer())
                        if (player->GetQuestStatus(QUEST_PLAYMATE_ORACLE) == QUEST_STATUS_INCOMPLETE)
                        {
                            playerGUID = player->GetGUID();
                            orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
                            if (orphanGUID)
                                phase = 1;
                        }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!phase)
                    return;

                if (timer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, playerGUID);
                    Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGUID);

                    if (!orphan || !player)
                    {
                        Reset();
                        return;
                    }

                    switch (phase)
                    {
                        case 1:
                            orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_1);
                            timer = 3000;
                            break;
                        case 2:
                            orphan->SetFacingToObject(me);
                            Talk(TEXT_WINTERFIN_PLAYMATE_1);
                            me->HandleEmoteCommand(EMOTE_STATE_DANCE);
                            timer = 3000;
                            break;
                        case 3:
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_2);
                            timer = 3000;
                            break;
                        case 4:
                            Talk(TEXT_WINTERFIN_PLAYMATE_2);
                            timer = 5000;
                            break;
                        case 5:
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_3);
                            me->HandleEmoteCommand(EMOTE_STATE_NONE);
                            player->GroupEventHappens(QUEST_PLAYMATE_ORACLE, me);
                            orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            Reset();
                            return;
                    }
                    ++phase;
                }
                else
                    timer -= diff;
            }

        private:
            uint32 timer;
            int8 phase;
            ObjectGuid playerGUID;
            ObjectGuid orphanGUID;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_winterfin_playmateAI(creature);
        }
};

/*######
## npc_snowfall_glade_playmate
######*/
class npc_snowfall_glade_playmate : public CreatureScript
{
    public:
        npc_snowfall_glade_playmate() : CreatureScript("npc_snowfall_glade_playmate") { }

        struct npc_snowfall_glade_playmateAI : public ScriptedAI
        {
            npc_snowfall_glade_playmateAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
            }

            void Reset() override
            {
                timer = 0;
                phase = 0;
                playerGUID = ObjectGuid::Empty;
                orphanGUID = ObjectGuid::Empty;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!phase && who && who->GetDistance2d(me) < 10.0f)
                    if (Player* player = who->ToPlayer())
                        if (player->GetQuestStatus(QUEST_PLAYMATE_WOLVAR) == QUEST_STATUS_INCOMPLETE)
                        {
                            playerGUID = player->GetGUID();
                            orphanGUID = getOrphanGUID(player, ORPHAN_WOLVAR);
                            if (orphanGUID)
                                phase = 1;
                        }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!phase)
                    return;

                if (timer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, playerGUID);
                    Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGUID);

                    if (!orphan || !player)
                    {
                        Reset();
                        return;
                    }

                    switch (phase)
                    {
                        case 1:
                            orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_1);
                            timer = 5000;
                            break;
                        case 2:
                            orphan->SetFacingToObject(me);
                            Talk(TEXT_SNOWFALL_GLADE_PLAYMATE_1);
                            DoCast(orphan, SPELL_SNOWBALL);
                            timer = 5000;
                            break;
                        case 3:
                            Talk(TEXT_SNOWFALL_GLADE_PLAYMATE_2);
                            timer = 5000;
                            break;
                        case 4:
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_2);
                            orphan->AI()->DoCast(me, SPELL_SNOWBALL);
                            timer = 5000;
                            break;
                        case 5:
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_3);
                            player->GroupEventHappens(QUEST_PLAYMATE_WOLVAR, me);
                            orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            Reset();
                            return;
                    }
                    ++phase;
                }
                else
                    timer -= diff;
            }

        private:
            uint32 timer;
            int8 phase;
            ObjectGuid playerGUID;
            ObjectGuid orphanGUID;
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_snowfall_glade_playmateAI(pCreature);
        }
};

/*######
## npc_high_oracle_soo_roo
######*/
class npc_high_oracle_soo_roo : public CreatureScript
{
    public:
        npc_high_oracle_soo_roo() : CreatureScript("npc_high_oracle_soo_roo") { }

        struct npc_high_oracle_soo_rooAI : public ScriptedAI
        {
            npc_high_oracle_soo_rooAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
            }

            void Reset() override
            {
                timer = 0;
                phase = 0;
                playerGUID = ObjectGuid::Empty;
                orphanGUID = ObjectGuid::Empty;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!phase && who && who->GetDistance2d(me) < 10.0f)
                    if (Player* player = who->ToPlayer())
                        if (player->GetQuestStatus(QUEST_THE_BRONZE_DRAGONSHRINE_ORACLE) == QUEST_STATUS_INCOMPLETE)
                        {
                            playerGUID = player->GetGUID();
                            orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
                            if (orphanGUID)
                                phase = 1;
                        }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!phase)
                    return;

                if (timer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, playerGUID);
                    Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGUID);

                    if (!orphan || !player)
                    {
                        Reset();
                        return;
                    }

                    switch (phase)
                    {
                        case 1:
                            orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_5);
                            timer = 3000;
                            break;
                        case 2:
                            orphan->SetFacingToObject(me);
                            Talk(TEXT_SOO_ROO_1);
                            timer = 6000;
                            break;
                        case 3:
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_6);
                            player->GroupEventHappens(QUEST_THE_BRONZE_DRAGONSHRINE_ORACLE, me);
                            orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            Reset();
                            return;
                    }
                    ++phase;
                }
                else
                    timer -= diff;
            }

        private:
            uint32 timer;
            int8 phase;
            ObjectGuid playerGUID;
            ObjectGuid orphanGUID;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_high_oracle_soo_rooAI(creature);
        }
};

/*######
## npc_elder_kekek
######*/
class npc_elder_kekek : public CreatureScript
{
    public:
        npc_elder_kekek() : CreatureScript("npc_elder_kekek") { }

        struct npc_elder_kekekAI : public ScriptedAI
        {
            npc_elder_kekekAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
            }

            void Reset() override
            {
                timer = 0;
                phase = 0;
                playerGUID = ObjectGuid::Empty;
                orphanGUID = ObjectGuid::Empty;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!phase && who && who->GetDistance2d(me) < 10.0f)
                    if (Player* player = who->ToPlayer())
                        if (player->GetQuestStatus(QUEST_THE_BRONZE_DRAGONSHRINE_WOLVAR) == QUEST_STATUS_INCOMPLETE)
                        {
                            playerGUID = player->GetGUID();
                            orphanGUID = getOrphanGUID(player, ORPHAN_WOLVAR);
                            if (orphanGUID)
                                phase = 1;
                        }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!phase)
                    return;

                if (timer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, playerGUID);
                    Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGUID);

                    if (!player || !orphan)
                    {
                        Reset();
                        return;
                    }

                    switch (phase)
                    {
                        case 1:
                            orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_4);
                            timer = 3000;
                            break;
                        case 2:
                            Talk(TEXT_ELDER_KEKEK_1);
                            timer = 6000;
                            break;
                        case 3:
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_5);
                            player->GroupEventHappens(QUEST_THE_BRONZE_DRAGONSHRINE_WOLVAR, me);
                            orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            Reset();
                            return;
                    }
                    ++phase;
                }
                else
                    timer -= diff;
            }

        private:
            uint32 timer;
            int8 phase;
            ObjectGuid playerGUID;
            ObjectGuid orphanGUID;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_elder_kekekAI(creature);
        }
};

/*######
## npc_cw_alexstrasza_trigger
######*/
class npc_alexstraza_the_lifebinder : public CreatureScript
{
    public:
        npc_alexstraza_the_lifebinder() : CreatureScript("npc_alexstraza_the_lifebinder") { }

        struct npc_alexstraza_the_lifebinderAI : public ScriptedAI
        {
            npc_alexstraza_the_lifebinderAI(Creature* creature) : ScriptedAI(creature) { }

            void Reset() override
            {
                timer = 0;
                phase = 0;
                playerGUID = ObjectGuid::Empty;
                orphanGUID = ObjectGuid::Empty;
                me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
            }

            void SetData(uint32 type, uint32 data) override
            {
                // Existing SmartAI
                if (type == 0)
                {
                    switch (data)
                    {
                        case 1:
                            me->SetOrientation(1.6049f);
                            break;
                        case 2:
                            me->SetOrientation(me->GetHomePosition().GetOrientation());
                            break;
                    }
                }
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!phase && who && who->GetDistance2d(me) < 10.0f)
                    if (Player* player = who->ToPlayer())
                    {
                        if (player->GetQuestStatus(QUEST_THE_DRAGON_QUEEN_ORACLE) == QUEST_STATUS_INCOMPLETE)
                        {
                            playerGUID = player->GetGUID();
                            orphanGUID = getOrphanGUID(player, ORPHAN_ORACLE);
                            if (orphanGUID)
                                phase = 1;
                        }
                        else if (player->GetQuestStatus(QUEST_THE_DRAGON_QUEEN_WOLVAR) == QUEST_STATUS_INCOMPLETE)
                        {
                            playerGUID = player->GetGUID();
                            orphanGUID = getOrphanGUID(player, ORPHAN_WOLVAR);
                            if (orphanGUID)
                                phase = 7;
                        }
                    }
            }

            void UpdateAI(uint32 diff) override
            {
                if (!phase)
                    return;

                if (timer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, playerGUID);
                    Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGUID);

                    if (!orphan || !player)
                    {
                        Reset();
                        return;
                    }

                    switch (phase)
                    {
                        case 1:
                            orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_11);
                            timer = 5000;
                            break;
                        case 2:
                            orphan->SetFacingToObject(me);
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_12);
                            timer = 5000;
                            break;
                        case 3:
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_13);
                            timer = 5000;
                            break;
                        case 4:
                            Talk(TEXT_ALEXSTRASZA_2);
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            me->SetFacingToObject(orphan);
                            timer = 5000;
                            break;
                        case 5:
                            orphan->AI()->Talk(TEXT_ORACLE_ORPHAN_14);
                            timer = 5000;
                            break;
                        case 6:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            me->SetOrientation(me->GetHomePosition().GetOrientation());
                            player->GroupEventHappens(QUEST_THE_DRAGON_QUEEN_ORACLE, me);
                            orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            Reset();
                            return;
                        case 7:
                            orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_11);
                            timer = 5000;
                            break;
                        case 8:
                            if (Creature* krasus = me->FindNearestCreature(NPC_KRASUS, 10.0f))
                            {
                                orphan->SetFacingToObject(krasus);
                                krasus->AI()->Talk(TEXT_KRASUS_8);
                            }
                            timer = 5000;
                            break;
                        case 9:
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_12);
                            timer = 5000;
                            break;
                        case 10:
                            orphan->SetFacingToObject(me);
                            Talk(TEXT_ALEXSTRASZA_2);
                            timer = 5000;
                            break;
                        case 11:
                            orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_13);
                            timer = 5000;
                            break;
                        case 12:
                            player->GroupEventHappens(QUEST_THE_DRAGON_QUEEN_WOLVAR, me);
                            orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            Reset();
                            return;
                    }
                    ++phase;
                }
                else
                    timer -= diff;
            }

            private:
                int8 phase;
                uint32 timer;
                ObjectGuid playerGUID;
                ObjectGuid orphanGUID;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_alexstraza_the_lifebinderAI(creature);
        }
};

/*######
## at_bring_your_orphan_to
######*/

class at_bring_your_orphan_to : public AreaTriggerScript
{
    public:
        at_bring_your_orphan_to() : AreaTriggerScript("at_bring_your_orphan_to") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (player->isDead() || !player->HasAura(SPELL_ORPHAN_OUT))
                return false;

            uint32 questId = 0;
            uint32 orphanId = 0;

            switch (trigger->ID)
            {
                case AT_DOWN_AT_THE_DOCKS:
                    questId = QUEST_DOWN_AT_THE_DOCKS;
                    orphanId = ORPHAN_ORCISH;
                    break;
                case AT_GATEWAY_TO_THE_FRONTIER:
                    questId = QUEST_GATEWAY_TO_THE_FRONTIER;
                    orphanId = ORPHAN_ORCISH;
                    break;
                case AT_LORDAERON_THRONE_ROOM:
                    questId = QUEST_LORDAERON_THRONE_ROOM;
                    orphanId = ORPHAN_ORCISH;
                    break;
                case AT_DARK_PORTAL:
                    questId = player->GetTeam() == ALLIANCE ? QUEST_DARK_PORTAL_A : QUEST_DARK_PORTAL_H;
                    orphanId = player->GetTeam() == ALLIANCE ? ORPHAN_DRAENEI : ORPHAN_BLOOD_ELF;
                    break;
            }

            if (questId && orphanId && getOrphanGUID(player, orphanId) && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                player->AreaExploredOrEventHappens(questId);

            return true;
        }
};

struct DialogBuilder
{
    DialogBuilder(Unit* me) : me(me) { }

    DialogBuilder& Move(Creature* who, Position const& pos)
    {
        who->GetMotionMaster()->MovePoint(0, pos);
        currentTimer += Milliseconds(who->movespline->Duration());
        return *this;
    }

    DialogBuilder& MoveRandom(Creature* who, float dist, TimeValue time)
    {
        Unit* host = me;
        ObjectGuid guid = who->GetGUID();

        host->m_Events.Schedule(currentTimer.ToMilliseconds(), [=]
        {
            if (Creature* npc = ObjectAccessor::GetCreature(*host, guid))
                who->GetMotionMaster()->MoveRandom(dist);
        });

        currentTimer += time;
        return *this;
    }

    DialogBuilder& Turn(Creature* who, WorldObject* to)
    {
        Unit* host = me;
        ObjectGuid guid = who->GetGUID();
        ObjectGuid toGuid = to->GetGUID();

        host->m_Events.Schedule(currentTimer.ToMilliseconds(), [=]
        {
            if (Creature* npc = ObjectAccessor::GetCreature(*host, guid))
            {
                npc->StopMoving();
                if (WorldObject* to = ObjectAccessor::GetWorldObject(*host, toGuid))
                    npc->SetFacingToObject(to);
            }
        });
        return *this;
    }

    DialogBuilder& Talk(Creature* speaker, uint32 text, TimeValue delay)
    {
        Unit* host = me;
        ObjectGuid guid = speaker->GetGUID();

        host->m_Events.Schedule(currentTimer.ToMilliseconds(), [=]
        {
            if (Creature* npc = ObjectAccessor::GetCreature(*host, guid))
                npc->AI()->Talk(text);
        });
        currentTimer += delay;
        return *this;
    }

    DialogBuilder& Evade(Creature* who)
    {
        Unit* host = me;
        ObjectGuid guid = who->GetGUID();

        host->m_Events.Schedule(currentTimer.ToMilliseconds(), [=]
        {
            if (Creature* npc = ObjectAccessor::GetCreature(*host, guid))
                npc->AI()->EnterEvadeMode();
        });
        return *this;
    }

    DialogBuilder& Cast(Creature* who, uint32 spell, Unit* target)
    {
        Unit* host = me;
        ObjectGuid guid = who->GetGUID();

        host->m_Events.Schedule(currentTimer.ToMilliseconds(), [=]
        {
            if (Creature* npc = ObjectAccessor::GetCreature(*host, guid))
                npc->CastSpell(target, spell);
        });
        return *this;
    }

    TimeValue currentTimer;
    Unit* me;
};

/*######
## npc_cw_area_trigger
######*/
struct npc_cw_area_trigger : public ScriptedAI
{
    npc_cw_area_trigger(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
        me->SetDisplayId(DISPLAY_INVISIBLE);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who && me->GetDistance2d(who) < 15.0f)
            if (Player* player = who->ToPlayer())
                if (player->HasAura(SPELL_ORPHAN_OUT))
                {
                    uint32 questId = 0;
                    uint32 orphanId = 0;
                    switch (me->GetEntry())
                    {
                        case NPC_CAVERNS_OF_TIME_CW_TRIGGER:
                            questId = player->GetTeam() == ALLIANCE ? QUEST_TIME_TO_VISIT_THE_CAVERNS_A : QUEST_TIME_TO_VISIT_THE_CAVERNS_H;
                            orphanId = player->GetTeam() == ALLIANCE ? ORPHAN_DRAENEI : ORPHAN_BLOOD_ELF;
                            break;
                        case NPC_EXODAR_01_CW_TRIGGER:
                            questId = QUEST_THE_SEAT_OF_THE_NARUU;
                            orphanId = ORPHAN_DRAENEI;
                            break;
                        case NPC_EXODAR_02_CW_TRIGGER:
                            questId = QUEST_CALL_ON_THE_FARSEER;
                            orphanId = ORPHAN_DRAENEI;
                            break;
                        case NPC_AERIS_LANDING_CW_TRIGGER:
                            questId = QUEST_JHEEL_IS_AT_AERIS_LANDING;
                            orphanId = ORPHAN_DRAENEI;
                            break;
                        case NPC_AUCHINDOUN_CW_TRIGGER:
                            questId = QUEST_AUCHINDOUN_AND_THE_RING;
                            orphanId = ORPHAN_DRAENEI;
                            break;
                        case NPC_SPOREGGAR_CW_TRIGGER:
                            questId = QUEST_HCHUU_AND_THE_MUSHROOM_PEOPLE;
                            orphanId = ORPHAN_BLOOD_ELF;
                            break;
                        case NPC_THRONE_OF_ELEMENTS_CW_TRIGGER:
                            questId = QUEST_VISIT_THE_THRONE_OF_ELEMENTS;
                            orphanId = ORPHAN_BLOOD_ELF;
                            break;
                        case NPC_SILVERMOON_01_CW_TRIGGER:
                            if (player->GetQuestStatus(QUEST_NOW_WHEN_I_GROW_UP) == QUEST_STATUS_INCOMPLETE && getOrphanGUID(player, ORPHAN_BLOOD_ELF))
                            {
                                player->AreaExploredOrEventHappens(QUEST_NOW_WHEN_I_GROW_UP);
                                if (player->GetQuestStatus(QUEST_NOW_WHEN_I_GROW_UP) == QUEST_STATUS_COMPLETE)
                                    if (Creature* samuro = me->FindNearestCreature(25151, 20.0f))
                                    {
                                        uint32 emote = 0;
                                        switch (urand(1, 5))
                                        {
                                        case 1:
                                            emote = EMOTE_ONESHOT_WAVE;
                                            break;
                                        case 2:
                                            emote = EMOTE_ONESHOT_ROAR;
                                            break;
                                        case 3:
                                            emote = EMOTE_ONESHOT_FLEX;
                                            break;
                                        case 4:
                                            emote = EMOTE_ONESHOT_SALUTE;
                                            break;
                                        case 5:
                                            emote = EMOTE_ONESHOT_DANCE;
                                            break;
                                        }
                                        samuro->HandleEmoteCommand(emote);
                                    }
                            }
                            break;
                        case NPC_AZEROTH_CHILDRENS_WEEK_TRIGGER:
                            HandleAzerothTirgger(player, orphanId, questId);
                            break;
                        case NPC_NORTHREND_CHILDRENS_WEEK_TRIGGER:
                            HandleNorthrendTrigger(player, orphanId, questId);
                            break;
                        default:
                            return;
                    }

                    ObjectGuid orphanGuid = getOrphanGUID(player, orphanId);
                    if (orphanGuid && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                        CompleteQuest(player, questId, orphanGuid);
                }
    }

    void HandleAzerothTirgger(Player* player, uint32& orphanId, uint32& questId)
    {
        if (player->GetTeam() == ALLIANCE)
        {
            orphanId = ORPHAN_HUMAN;
            questId = player->GetMapId() ? QUEST_MALFURION_HAS_RETURNED : QUEST_THE_BIGGEST_DIAMOND_EVER;
        }
        else
        {
            orphanId = ORPHAN_ORCISH;
            questId = player->GetMapId() ? QUEST_THE_FALLEN_CHIEFTAIN : QUEST_THE_BANSHEE_QUEEN;
        }
    }

    void HandleNorthrendTrigger(Player* player, uint32& orphanId, uint32& questId)
    {
        if (player->GetZoneId() == 490) // Un'Goro Crater, Etymidian
        {
            orphanId = ORPHAN_ORACLE;
            questId = QUEST_MEETING_A_GREAT_ONE;
        }
        else if (player->GetZoneId() == 394) // Grizzly Hills
        {
            orphanId = ORPHAN_ORACLE;
            questId = QUEST_THE_BIGGEST_TREE_EVER;
        }
    }

    void CompleteQuest(Player* player, uint32 questId, ObjectGuid orphanGuid)
    {
        player->AreaExploredOrEventHappens(questId);

        switch (questId)
        {
            case QUEST_THE_BANSHEE_QUEEN:           RunSceneTheBanseeQueen(orphanGuid);         break;
            case QUEST_THE_FALLEN_CHIEFTAIN:        RunSceneTheFallenChieftain(orphanGuid);     break;
            case QUEST_THE_BIGGEST_DIAMOND_EVER:    RunSceneTheBiggestDiamondEver(orphanGuid);  break;
            case QUEST_MALFURION_HAS_RETURNED:      RunSceneMalfurionHasReturned(orphanGuid);   break;
            case QUEST_THE_BIGGEST_TREE_EVER:       RunSceneTheBiggestTreeEver(orphanGuid);     break;
            case QUEST_MEETING_A_GREAT_ONE:         RunSceneMeetingGreateOne(orphanGuid);       break;
        }
    }

    void RunSceneTheBanseeQueen(ObjectGuid orphanGuid)
    {
        Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGuid);
        Creature* ambassador = me->FindNearestCreature(NPC_AMBASSADOR, 50.0f);
        Creature* sylvanas = me->FindNearestCreature(NPC_SYLVANAS, 50.0f);

        if (!orphan || !ambassador || !sylvanas)
            return;

        DialogBuilder{ me }
            .Move(orphan, Position{ 1292.0f, 322.68f, -57.85f })
            .Turn(orphan, sylvanas)
            .Talk(orphan,     TEXT_THE_BANSHEE_QUEEN_ORPHAN_1,   Seconds(4))
            .Talk(orphan,     TEXT_THE_BANSHEE_QUEEN_ORPHAN_2,   Seconds(7))
            .Talk(ambassador, TEXT_THE_BANSHEE_QUEEN_AMBASSADOR, Seconds(6))
            .Talk(sylvanas,   TEXT_THE_BANSHEE_QUEEN_SYLVANAS_1, Seconds(7))
            .Talk(sylvanas,   TEXT_THE_BANSHEE_QUEEN_SYLVANAS_2, Seconds(6))
            .Talk(orphan,     TEXT_THE_BANSHEE_QUEEN_ORPHAN_3,   Seconds(6))
            .Talk(sylvanas,   TEXT_THE_BANSHEE_QUEEN_SYLVANAS_3, Seconds(5))
            .Evade(orphan);
    }

    void RunSceneTheFallenChieftain(ObjectGuid orphanGuid)
    {
        Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGuid);
        if (orphan)

        DialogBuilder{ me }
            .Move(orphan, Position{ -1008.22f, -1105.63f, 44.41f })
            .Talk(orphan, TEXT_THE_FALLEN_CHIEFTAIN_ORPHAN, Seconds(5))
            .Evade(orphan);
    }

    void RunSceneTheBiggestDiamondEver(ObjectGuid orphanGuid)
    {
        Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGuid);
        Creature* guard = me->FindNearestCreature(NPC_HONOR_GUARD_DUNSTAD, 50.0f);
        Creature* advisor = me->FindNearestCreature(NPC_ADVISOR_BELGRUM, 50.0f);

        if (!orphan || !guard || !advisor)
            return;

        DialogBuilder{ me }
            .Move(orphan,  Position{ -4822.51f, -977.43f, 464.7f })
            .Talk(orphan,  TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_1, Seconds(5))
            .Talk(orphan,  TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_2, Seconds(5))
            .Turn(guard,   orphan)
            .Talk(guard,   TEXT_THE_BIGGEST_DIAMOND_EVER_GUARD_1,  Seconds(6))
            .Talk(orphan,  TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_3, Seconds(5))
            .Turn(advisor, guard)
            .Talk(advisor, TEXT_THE_BIGGEST_DIAMOND_EVER_ADVISOR,  Seconds(5))
            .Talk(orphan,  TEXT_THE_BIGGEST_DIAMOND_EVER_ORPHAN_4, Seconds(5))
            .Talk(guard,   TEXT_THE_BIGGEST_DIAMOND_EVER_GUARD_2,  Seconds(3))
            .Evade(orphan)
            .Evade(guard)
            .Evade(advisor);
    }

    void RunSceneMalfurionHasReturned(ObjectGuid orphanGuid)
    {
        Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGuid);
        Creature* malfurion = me->FindNearestCreature(NPC_MALFURION, 50.0f);

        if (!orphan || !malfurion)
            return;

        DialogBuilder{ me }
            .Move(orphan, Position{ 9665.75f, 2523.22f, 1359.99f })
            .Turn(orphan, malfurion)
            .Talk(orphan,    TEXT_MALFURION_HAS_RETURNED_1_ORPHAN,    Seconds(4))
            .Talk(orphan,    TEXT_MALFURION_HAS_RETURNED_2_ORPHAN,    Seconds(6))
            .Talk(orphan,    TEXT_MALFURION_HAS_RETURNED_3_ORPHAN,    Seconds(6))
            .Talk(malfurion, TEXT_MALFURION_HAS_RETURNED_4_MALFURION, Seconds(9))
            .Talk(orphan,    TEXT_MALFURION_HAS_RETURNED_5_ORPHAN,    Seconds(6))
            .Talk(orphan,    TEXT_MALFURION_HAS_RETURNED_6_ORPHAN,    Seconds(0))
            .Cast(orphan,    SPELL_MALFURION_GIVES_YOU_WINGS,         orphan);
        // Random movement started in aura script and handled in AI
    }

    void RunSceneTheBiggestTreeEver(ObjectGuid orphanGuid)
    {
        Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGuid);
        if (!orphan)
            return;

        Position pos = *me;
        pos.RelocateOffset(0, 5);

        DialogBuilder{ me }
            .Move(orphan, pos)
            .Turn(orphan, me)
            .Talk(orphan, TEXT_THE_BIGGEST_TREE_EVER, Seconds(5))
            .Evade(orphan);
    }

    void RunSceneMeetingGreateOne(ObjectGuid orphanGuid)
    {
        Creature* orphan = ObjectAccessor::GetCreature(*me, orphanGuid);
        Creature* etymidian = me->FindNearestCreature(NPC_ETYMIDIAN, 50.0f);
        if (!orphan || !etymidian)
            return;

        Position pos = etymidian->GetPosition();
        pos.RelocateOffset(0, 5);

        DialogBuilder{ me }
            .Move(orphan, pos)
            .Turn(orphan, etymidian)
            .Talk(orphan, TEXT_MEETING_A_GREAT_ONE_1, Seconds(5))
            .Talk(orphan, TEXT_MEETING_A_GREAT_ONE_2, Seconds(5))
            .Talk(orphan, TEXT_MEETING_A_GREAT_ONE_3, Seconds(5))
            .Talk(orphan, TEXT_MEETING_A_GREAT_ONE_4, Seconds(5))
            .Evade(orphan);
    }
};

/*######
## npc_grizzlemaw_cw_trigger
######*/
class npc_grizzlemaw_cw_trigger : public CreatureScript
{
    public:
        npc_grizzlemaw_cw_trigger() : CreatureScript("npc_grizzlemaw_cw_trigger") { }

        struct npc_grizzlemaw_cw_triggerAI : public ScriptedAI
        {
            npc_grizzlemaw_cw_triggerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetDisplayId(DISPLAY_INVISIBLE);
                me->SetReactState(REACT_AGGRESSIVE); // To make MoveInLineOfSight work
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (who && who->GetDistance2d(me) < 10.0f)
                    if (Player* player = who->ToPlayer())
                        if (player->GetQuestStatus(QUEST_HOME_OF_THE_BEAR_MEN) == QUEST_STATUS_INCOMPLETE)
                            if (Creature* orphan = Creature::GetCreature(*me, getOrphanGUID(player, ORPHAN_WOLVAR)))
                            {
                                player->AreaExploredOrEventHappens(QUEST_HOME_OF_THE_BEAR_MEN);
                                orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_10);
                            }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_grizzlemaw_cw_triggerAI(creature);
        }
};

// 14444, 14305, 22817, 22818, 33533, 33532
struct npc_summoned_orphan : public PassiveAI
{
    npc_summoned_orphan(Creature* c) : PassiveAI(c) { }

    enum { RANDOM_POINT = 1000 };

    TaskScheduler scheduler;

    void IsSummonedBy(Unit* summoner) override
    {
        me->GetMotionMaster()->Clear(false);
        me->GetMotionMaster()->MoveFollow(summoner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
        me->setActive(true); // To execute code in UpdateAI then player teleports
    }

    void UpdateAI(uint32 diff) override
    {
        scheduler.Update(diff);

        if (Unit* owner = me->GetOwner())
            if (owner->GetDistance(me) > 100.0f)
                me->DespawnOrUnsummon();
    }

    void EnterEvadeMode(EvadeReason why = EVADE_REASON_OTHER) override
    {
        scheduler.CancelAll();
        PassiveAI::EnterEvadeMode();
    }

    void StartMoveRandom(TimeValue delay)
    {
        scheduler.Schedule(delay.GetDuration(), [=](TaskContext)
        {
            MoveRandom();
        });
    }

    void MoveRandom()
    {
        Position pos{ 9673.0f, 2524.45f, 1360.0f };
        me->MovePosition(pos, 10.0f, (float)rand_norm() * static_cast<float>(2 * M_PI));
        me->GetMotionMaster()->MovePoint(RANDOM_POINT, pos);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == RANDOM_POINT)
            StartMoveRandom(Milliseconds(100));
    }
};

// 23012, 23013 - Summon Orphan
// 39478 - Call for Salandria
// 39479 - Call for Dornaa
// 65352 - Call for Oracle Orphan
// 65353 - Call for Wolvar Orphan
class spell_summon_orphan : public SpellScript
{
    PrepareSpellScript(spell_summon_orphan);

    void HandleHit(SpellEffIndex eff)
    {
        for (auto&& itr : GetCaster()->GetSummons())
        {
            if (itr->GetEntry() == GetSpellInfo()->Effects[eff].MiscValue)
            {
                PreventDefaultEffect(eff);
                itr->DespawnOrUnsummon();
                return;
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_summon_orphan::HandleHit, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

// 96864 - Use Dragon Kite 2 - pack
class spell_use_dragon_kite_2_pack : public SpellScript
{
    PrepareSpellScript(spell_use_dragon_kite_2_pack);

    void HandleCast()
    {
        if (Aura* orphanOut = GetCaster()->GetAura(SPELL_ORPHAN_OUT))
        {
            if (Unit* orphan = orphanOut->GetCaster())
            {
                orphan->CastSpell(orphan, GetSpellInfo()->Id);
                orphan->ClearUnitState(UNIT_STATE_CASTING);
            }
        }

        if (auto const* summonSpell = sSpellMgr->GetSpellInfo(GetSpellInfo()->Effects[EFFECT_0].TriggerSpell))
            for (auto&& itr : GetCaster()->GetSummons())
                if (itr->GetEntry() == summonSpell->Effects[EFFECT_0].MiscValue)
                    GetCaster()->CastSpell(itr, SPELL_KITE_STRING);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_use_dragon_kite_2_pack::HandleCast);
    }
};

// 97020 - Eat Cone of Cold
class spell_eat_cone_of_cold : public SpellScript
{
    PrepareSpellScript(spell_eat_cone_of_cold);

    void HandleCast()
    {
        if (Aura* orphanOut = GetCaster()->GetAura(SPELL_ORPHAN_OUT))
            if (Unit* orphan = orphanOut->GetCaster())
                orphan->CastSpell(orphan, SPELL_ORPHANS_ICE_CREAM);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_eat_cone_of_cold::HandleCast);
    }
};

// 96807 - Malfurion Gives You Wings!
class spell_malfurion_gives_you_wings : public AuraScript
{
    PrepareAuraScript(spell_malfurion_gives_you_wings);

    void HandleApply(AuraEffect const*, AuraEffectHandleModes)
    {
        if (auto ai = dynamic_cast<npc_summoned_orphan*>(GetUnitOwner()->GetAI()))
        {
            GetUnitOwner()->SetSpeed(MOVE_RUN, 1.4f);
            ai->StartMoveRandom(Seconds(2));
        }
    }

    void HandleRemove(AuraEffect const*, AuraEffectHandleModes)
    {
        if (auto ai = dynamic_cast<npc_summoned_orphan*>(GetUnitOwner()->GetAI()))
        {
            GetUnitOwner()->UpdateSpeed(MOVE_RUN, true);
            GetUnitOwner()->StopMoving();
            ai->EnterEvadeMode();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_malfurion_gives_you_wings::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectRemove += AuraEffectApplyFn(spell_malfurion_gives_you_wings::HandleRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 65349 - Throw Paper Zeppelin
class spell_throw_paper_zeppelin_to_orphan : public SpellScript
{
    PrepareSpellScript(spell_throw_paper_zeppelin_to_orphan);

    void SelectTargets(std::list<WorldObject*>& targets)
    {
        if (Aura* orphanOut = GetCaster()->GetAura(SPELL_ORPHAN_OUT))
            if (Unit* orphan = orphanOut->GetCaster())
                targets.remove_if([=](WorldObject const* target) { return target != orphan; });
    }

    void HandleHit()
    {
        if (Player* player = GetCaster()->ToPlayer())
            player->KilledMonsterCredit(36209);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_throw_paper_zeppelin_to_orphan::SelectTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnHit += SpellHitFn(spell_throw_paper_zeppelin_to_orphan::HandleHit);
    }
};

void AddSC_event_childrens_week()
{
    new npc_elder_kekek();
    new npc_high_oracle_soo_roo();
    new npc_winterfin_playmate();
    new npc_snowfall_glade_playmate();
    new at_bring_your_orphan_to();
    new npc_grizzlemaw_cw_trigger();
    new creature_script<npc_cw_area_trigger>("npc_cw_area_trigger");
    new npc_alexstraza_the_lifebinder();
    new creature_script<npc_summoned_orphan>("npc_summoned_orphan");
    new spell_script<spell_summon_orphan>("spell_summon_orphan");
    new spell_script<spell_use_dragon_kite_2_pack>("spell_use_dragon_kite_2_pack");
    new spell_script<spell_eat_cone_of_cold>("spell_eat_cone_of_cold");
    new aura_script<spell_malfurion_gives_you_wings>("spell_malfurion_gives_you_wings");
    new spell_script<spell_throw_paper_zeppelin_to_orphan>("spell_throw_paper_zeppelin_to_orphan");
}
