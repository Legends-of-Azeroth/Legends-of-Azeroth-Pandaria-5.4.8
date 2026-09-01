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

//101037 - borean beam
//101053 Orb Staff Fusion

// for event
// 53396 - tarecgosa (visual)
// 53398, 53400, 53406 - firebands
// 53422 - dragonwrath
// 53700 - tarecgosa (invisible)
// 53715 - tarecgosa ?
//INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES (375216, 53392, 1, 1, 1, 38266, 0, 1304.76, -4416.87, 182.595, 6.2128, 300, 0, 0, 17672, 0, 2, 0, 0, 0);
//INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES (305529, 53392, 0, 1, 65535, 38266, 0, -8932.82, 674.983, 183.057, 5.33729, 300, 0, 0, 17672, 0, 2, 0, 0, 0);
//INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES (305518, 53396, 0, 1, 65535, 38312, 0, -8678.7, 732.189, 150.011, 3.83972, 300, 0, 0, 96986, 0, 0, 0, 0, 0);
//INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES (356789, 53396, 1, 1, 65535, 38312, 0, 1655.29, -4346.17, 118.063, 4.98583, 300, 0, 0, 96986, 0, 2, 0, 0, 0);

/*######
## npc_anachronos
## entry 15192
######*/

#define GOSSIP_A_WRINKIE_IN_TIME "Yes, Anachronos. I am ready to witness your vision of the future."
#define ACTION_A_WRINKIE_IN_TIME 1001

#define GOSSIP_ON_A_WING_AND_A_PRAYER "Take the Timeless Eye."
#define ACTION_ON_A_WING_AND_A_PRAYER 1002

class npc_anachronos_15192 : public CreatureScript
{
    public:
        npc_anachronos_15192() : CreatureScript("npc_anachronos_15192") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(29134) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_A_WRINKIE_IN_TIME, GOSSIP_SENDER_MAIN, ACTION_A_WRINKIE_IN_TIME);
            else if (player->GetQuestStatus(29193) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ON_A_WING_AND_A_PRAYER, GOSSIP_SENDER_MAIN, ACTION_ON_A_WING_AND_A_PRAYER);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == ACTION_A_WRINKIE_IN_TIME)
            {
                player->KilledMonsterCredit(52605, ObjectGuid::Empty);
            }
            else if (action == ACTION_ON_A_WING_AND_A_PRAYER)
            {
                uint32 curItemCount = player->GetItemCount(69238, false);
                if (curItemCount >= 1)
                    return false;
                
                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 49661, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = player->StoreNewItem(dest, 69238, true);
                    player->SendNewItem(item, 1, true, false);
                }
                else
                    return false;
            }

            return true;
        }
};

class npc_tarecgosa_52835 : public CreatureScript
{
    public:
        npc_tarecgosa_52835() : CreatureScript("npc_tarecgosa_52835") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/) override
        {
            player->PlayerTalkClass->ClearMenus();
            return false;
        }

        struct npc_tarecgosa_52835AI : ScriptedAI
        {
            npc_tarecgosa_52835AI(Creature* creature) : ScriptedAI(creature) { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!me->IsWithinDistInMap(who, 20.0f))
                    return;

                if (who->ToPlayer()->GetQuestStatus(29193) == QUEST_STATUS_INCOMPLETE)
                {
                    who->ToPlayer()->KilledMonsterCredit(52832, ObjectGuid::Empty);
                    who->ToPlayer()->CompleteQuest(29193);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tarecgosa_52835AI(creature);
        }
};

class npc_tarecgosa_53098 : public CreatureScript
{
    public:
        npc_tarecgosa_53098() : CreatureScript("npc_tarecgosa_53098") { }

        struct npc_tarecgosa_53098AI : ScriptedAI
        {
            npc_tarecgosa_53098AI(Creature* creature) : ScriptedAI(creature) 
            {
                me->SetStandState(UNIT_STAND_STATE_DEAD);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!me->IsWithinDistInMap(who, 20.0f))
                    return;

                if (who->ToPlayer()->GetQuestStatus(29240) == QUEST_STATUS_INCOMPLETE)
                {
                    who->ToPlayer()->KilledMonsterCredit(53047, ObjectGuid::Empty);
                    who->ToPlayer()->CompleteQuest(29240);
                }

                if (who->ToPlayer()->GetQuestStatus(29269) == QUEST_STATUS_INCOMPLETE)
                {
                    who->ToPlayer()->KilledMonsterCredit(53182, ObjectGuid::Empty);
                    who->ToPlayer()->CompleteQuest(29269);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tarecgosa_53098AI(creature);
        }
};

class npc_kalecgos_53149 : public CreatureScript
{
    public:
        npc_kalecgos_53149() : CreatureScript("npc_kalecgos_53149") { }

        struct npc_kalecgos_53149AI : ScriptedAI
        {
            npc_kalecgos_53149AI(Creature* creature) : ScriptedAI(creature) 
            {
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kalecgos_53149AI(creature);
        }
};

class npc_thyrinar : public CreatureScript
{
    public:
        npc_thyrinar() : CreatureScript("npc_thyrinar") { }

        struct npc_thyrinarAI : ScriptedAI
        {
            npc_thyrinarAI(Creature* creature) : ScriptedAI(creature) 
            {
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetReactState(REACT_PASSIVE);
            }

            void Reset() override
            {
                me->SetCanFly(true);
                me->SetDisableGravity(true);
                me->SetReactState(REACT_PASSIVE);
            }

            void JustDied(Unit* who) override
            {
                if (!who)
                    return;

                Unit* originalKiller = who->GetCharmerOrOwnerOrSelf();
                if (!originalKiller || originalKiller->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (Creature* pTarecgosa = me->FindNearestCreature(53567, 200.0f))
                    pTarecgosa->RemoveAura(99561);

                if (originalKiller->ToPlayer()->GetQuestStatus(29194) == QUEST_STATUS_INCOMPLETE)
                    originalKiller->ToPlayer()->KilledMonsterCredit(52867, ObjectGuid::Empty);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_thyrinarAI(creature);
        }
};

class npc_kalecgos_53349 : public CreatureScript
{
    public:
        npc_kalecgos_53349() : CreatureScript("npc_kalecgos_53349") { }

        struct npc_kalecgos_53349AI : ScriptedAI
        {
            npc_kalecgos_53349AI(Creature* creature) : ScriptedAI(creature) { }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (!me->IsWithinDistInMap(who, 20.0f))
                    return;

                if (who->ToPlayer()->GetQuestStatus(29285) == QUEST_STATUS_INCOMPLETE)
                    who->ToPlayer()->KilledMonsterCredit(53227, ObjectGuid::Empty);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kalecgos_53349AI(creature);
        }
};

enum ScriptedText_kalecgos_53392
{
    SAY_53392_1 = 0, 
    SAY_53392_2 = 1, 
    SAY_53392_3 = 2,
    SAY_53392_4 = 3, 
    SAY_53392_5 = 4, 
    SAY_53392_6 = 5, 
    SAY_53392_7 = 6, 
    SAY_53392_8 = 7,
};

enum Spells
{
    SPELL_HALEGOSA_TRANSFORM        = 98948,
    SPELL_HALLEGOSA_SUMMON_DISC_1   = 98939, // alliance
    SPELL_HALLEGOSA_SUMMON_DISC_2   = 99094, // horde
    SPELL_PLAYER_CAST_DISC_SUMMON_1 = 98944, // alliance
    SPELL_PLAYER_CAST_DISC_SUMMON_2 = 99096, // horde
    SPELL_DISC_SUMMON_AURA          = 98946,
    SPELL_RIDE_VEHICLE              = 43671, // wrong
    SPELL_SUMMON_TARECGOSA_DISC     = 98634,
    SPELL_SUMMON_DRAGON_DISC_1      = 98943, // alliance
    SPELL_SUMMON_DRAGON_DISC_2      = 99098, // horde
    SPELL_ESSENCE_OF_TARECGOSA      = 98439,
    SPELL_PULSE_OF_TARECGOSA        = 99025,
};

enum Events_kalecgos_53392
{
    EVENT_HALLEGOSA_MOVE_1  = 1,
    EVENT_HALLEGOSA_MOVE_2  = 2,
    EVENT_HALLEGOSA_DISC_1  = 3,
    EVENT_HALLEGOSA_TRANS   = 4,
    EVENT_HALLEGOSA_END     = 5,
    EVENT_TALK_1            = 6,
    EVENT_TALK_2            = 7,
    EVENT_TALK_3            = 8,
    EVENT_TALK_4            = 9,
    EVENT_TALK_5            = 10,
    EVENT_ESSENCE_1         = 11,
    EVENT_ESSENCE_2         = 12,
    EVENT_TALK_6            = 13,
    EVENT_TALK_7            = 14,
    EVENT_TALK_8            = 15,
    EVENT_START_FINAL       = 16,
    EVENT_END_FINAL         = 17,
};

enum FinalAdds
{
    NPC_HALLEGOSA_53371     = 53371,
    NPC_HALLEGOSA_53466     = 53466,
    NPC_KALECGOS_53392      = 53392,
    NPC_TARECGOSA_53396     = 53396,
    NPC_STALKER_53422       = 53422,
    NPC_BLUE_DRAGON_53394   = 53394,
    NPC_BLUE_DRAKE_53408    = 53408,
};

enum Points
{
    POINT_HALLEGOSA_1   = 1,
    POINT_HALLEGOSA_2   = 2,
};

const Position finalPos1[10] =
{
    {-8642.198242f, 780.237244f, 97.042953f, 0.0f  }, // Hallegosa 1
    {-8635.111328f, 787.129761f, 97.045670f, 0.0f  }, // Hallegosa 2
    {-8637.245117f, 798.395203f, 96.929199f, 0.0f  }, // Hallegosa 3
    {-8667.672852f, 743.323975f, 148.60f,    3.82f }, // Hallegosa 4 (fly)
    {-8706.003906f, 712.394714f, 159.57f,    0.64f }, // Kalecgos (fly)
    {-8726.765625f, 738.313660f, 148.60f,    6.14f }, // Tarecgosa (fly)
    {-8698.031250f, 757.344666f, 148.60f,    4.92f }, // Blue Dragon (fly)
    {-8655.565430f, 717.462524f, 149.95f,    2.86f }, // Blue Dragon (fly)
    {-8681.251953f, 698.383423f, 154.26f,    1.72f }, // Blue Dragon (fly)
    {-8686.417969f, 728.565369f, 152.66f,    3.73f }  // Stalker
};

const Position finalPos2[13] =
{
    { 1646.289795f, -4354.969238f, 26.337263f, 0.0f  }, // Hallegosa 1
    { 1632.526611f, -4362.582031f, 26.775732f, 0.0f  }, // Hallegosa 2
    { 1627.358765f, -4365.393555f, 24.574675f, 0.0f  }, // Hallegosa 3
    { 1622.515015f, -4375.128906f, 24.620348f, 0.0f  }, // Hallegosa 4
    { 1611.619873f, -4373.713867f, 24.628357f, 0.0f  }, // Hallegosa 5
    { 1604.297729f, -4377.556152f, 20.968573f, 0.0f  }, // Hallegosa 6
    { 1582.353271f, -4370.833496f, 20.658976f, 0.0f  }, // Hallegosa 7
    { 1615.443115f, -4357.785156f, 125.10f,    0.18f }, // Hallegosa 8 (fly)
    { 1656.803711f, -4350.011719f, 136.80f,    3.63f }, // Kalecgos (fly)
    { 1631.014893f, -4339.889160f, 133.39f,    5.08f }, // Tarecgosa (fly)
    { 1649.036011f, -4379.650391f, 123.84f,    2.12f }, // Blue Dragon (fly)
    { 1627.399048f, -4381.077637f, 125.10f,    1.06f }, // Blue Dragon (fly)
    { 1634.151245f, -4358.372559f, 124.68f,    0.55f }  // Stalker
};

class npc_hallegosa : public CreatureScript
{
    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        creature->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        return true;
    }

    public:
        npc_hallegosa() : CreatureScript("npc_hallegosa") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 /*action*/) override
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            creature->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            
            creature->SetWalk(true);
            creature->GetMotionMaster()->MovePoint(POINT_HALLEGOSA_1, ((creature->GetMapId() == 1) ? finalPos2[0] : finalPos1[0]));
            return true;
        }

        struct npc_hallegosaAI : ScriptedAI
        {
            npc_hallegosaAI(Creature* creature) : ScriptedAI(creature) 
            {
                curPoint = 0;
                me->RemoveAllAuras();
                me->SetCanFly(false);
                me->SetDisableGravity(false);
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                me->setActive(true);
            }

            void JustAppeared() override
            {
                ScriptedAI::JustAppeared();
                me->RemoveAllAuras();
                me->SetCanFly(false);
                curPoint = 0;
                me->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
            }

            void MovementInform(uint32 type, uint32 data) override
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (data)
                    {
                        case POINT_HALLEGOSA_1:
                        {
                            curPoint++;
                            uint8 max_point = ((me->GetMapId() == 1) ? 7 : 3);

                            if (curPoint == max_point)
                                events.ScheduleEvent(EVENT_HALLEGOSA_DISC_1, 500);
                            else
                                events.ScheduleEvent(EVENT_HALLEGOSA_MOVE_1, 500);
                            break;
                        }
                        case POINT_HALLEGOSA_2:
                            events.ScheduleEvent(EVENT_HALLEGOSA_END, 5000);
                            break;
                    }
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HALLEGOSA_MOVE_1:
                            me->GetMotionMaster()->MovePoint(POINT_HALLEGOSA_1, ((me->GetMapId() == 1) ? finalPos2[curPoint] : finalPos1[curPoint]));
                            break;
                        case EVENT_HALLEGOSA_DISC_1:
                            DoCast(me, SPELL_HALLEGOSA_SUMMON_DISC_1);
                            events.ScheduleEvent(EVENT_HALLEGOSA_TRANS, 5000);
                            break;
                        case EVENT_HALLEGOSA_TRANS:
                            DoCast(me, SPELL_HALEGOSA_TRANSFORM);
                            events.ScheduleEvent(EVENT_HALLEGOSA_MOVE_2, 3000);
                            break;
                        case EVENT_HALLEGOSA_MOVE_2:
                            me->SetCanFly(true);
                            me->SetDisableGravity(true);
                            me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_CAN_FLY);
                            me->GetMotionMaster()->MovePoint(POINT_HALLEGOSA_2, ((me->GetMapId() == 1) ? finalPos2[curPoint] : finalPos1[curPoint]));
                            break;
                        case EVENT_HALLEGOSA_END:
                            me->SummonCreature(NPC_KALECGOS_53392, ((me->GetMapId() == 1) ? finalPos2[8] : finalPos1[4]), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * MINUTE * IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }
            }

        private:
            EventMap events;
            uint8 curPoint;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_hallegosaAI(creature);
        }
};

class npc_kalecgos_53392 : public CreatureScript
{
    public:
        npc_kalecgos_53392() : CreatureScript("npc_kalecgos_53392") { }

        struct npc_kalecgos_53392AI : ScriptedAI
        {
            npc_kalecgos_53392AI(Creature* creature) : ScriptedAI(creature) 
            {
                me->setActive(true);
            }

            void IsSummonedBy(Unit* /*owner*/) override
            {
                events.ScheduleEvent(EVENT_TALK_1, 5000);
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (me->GetDistance(who) >= 20.0f)
                    return;

                if (who->ToPlayer()->GetTeam() == HORDE)
                {
                    if (who->ToPlayer()->GetQuestStatus(29309) == QUEST_STATUS_INCOMPLETE)
                        who->ToPlayer()->KilledMonsterCredit(53715, ObjectGuid::Empty);
                }
                else if (who->ToPlayer()->GetTeam() == ALLIANCE)
                {
                    if (who->ToPlayer()->GetQuestStatus(29312) == QUEST_STATUS_INCOMPLETE)
                        who->ToPlayer()->KilledMonsterCredit(53715, ObjectGuid::Empty);
                }
            }

            void UpdateAI(uint32 diff) override
            {
                events.Update(diff);

                if (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TALK_1:
                            Talk(SAY_53392_1, 0);
                            events.ScheduleEvent(EVENT_TALK_2, 6000);
                            break;
                        case EVENT_TALK_2:
                            Talk(SAY_53392_2, 0);
                            if (me->GetMapId() == 1)
                            {
                                for (uint8 i = 10; i < 12; ++i)
                                    me->SummonCreature(NPC_BLUE_DRAGON_53394, finalPos2[i], TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * MINUTE * IN_MILLISECONDS)); 
                            }
                            else
                            {
                                for (uint8 i = 6; i < 9; ++i)
                                    me->SummonCreature(NPC_BLUE_DRAGON_53394, finalPos1[i], TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * MINUTE * IN_MILLISECONDS)); 
                            }
                            events.ScheduleEvent(EVENT_TALK_3, 7000);
                            break;
                        case EVENT_TALK_3:
                            Talk(SAY_53392_3, 0);
                            events.ScheduleEvent(EVENT_TALK_4, 6000);
                            break;
                        case EVENT_TALK_4:
                            Talk(SAY_53392_4, 0);
                            me->SummonCreature(NPC_TARECGOSA_53396, ((me->GetMapId() == 1) ? finalPos2[9] : finalPos1[5]), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * MINUTE * IN_MILLISECONDS));
                            events.ScheduleEvent(EVENT_TALK_5, 7000);
                            break;
                        case EVENT_TALK_5:
                            Talk(SAY_53392_5, 0);
                            if (Creature* pStalker = me->SummonCreature(NPC_STALKER_53422, ((me->GetMapId() == 1) ? finalPos2[12] : finalPos1[9]), TEMPSUMMON_TIMED_DESPAWN, Milliseconds(30 * MINUTE * IN_MILLISECONDS)))
                            {
                                pStalker->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
                                pStalker->SetUInt32Value(UNIT_FIELD_VIRTUAL_ITEM_ID + 0, 69859);
                                pStalker->SetDisplayId(21888);
                            }
                            events.ScheduleEvent(EVENT_ESSENCE_1, 9000);
                            break;
                        case EVENT_ESSENCE_1:
                            if (Creature* pStalker = me->FindNearestCreature(NPC_STALKER_53422, 100.0f))
                                pStalker->CastSpell(pStalker, SPELL_ESSENCE_OF_TARECGOSA);
                            events.ScheduleEvent(EVENT_ESSENCE_2, 15000);
                            break;
                        case EVENT_ESSENCE_2:
                            if (Creature* pTarecgosa = me->FindNearestCreature(NPC_TARECGOSA_53396, 100.0f))
                                pTarecgosa->DespawnOrUnsummon();
                            if (Creature* pStalker = me->FindNearestCreature(NPC_STALKER_53422, 100.0f))
                                pStalker->CastSpell(pStalker, SPELL_PULSE_OF_TARECGOSA);
                            events.ScheduleEvent(EVENT_TALK_6, 6000);
                            break;
                        case EVENT_TALK_6:
                            Talk(SAY_53392_6, 0);
                            events.ScheduleEvent(EVENT_TALK_7, 13000);
                            break;
                        case EVENT_TALK_7:
                            Talk(SAY_53392_7, 0);
                            if (Creature* pStalker = me->FindNearestCreature(NPC_STALKER_53422, 100.0f))
                                pStalker->SetFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                            events.ScheduleEvent(EVENT_TALK_8, 7000);
                            break;
                        case EVENT_TALK_8:
                            Talk(SAY_53392_8, 0);
                            if (Creature* pHallegosa = me->FindNearestCreature(((me->GetMapId() == 1) ? NPC_HALLEGOSA_53371 : NPC_HALLEGOSA_53466), 100.0f))
                            {
                                pHallegosa->NearTeleportTo(pHallegosa->GetHomePosition().GetPositionX(), pHallegosa->GetHomePosition().GetPositionY(), pHallegosa->GetHomePosition().GetPositionZ(), pHallegosa->GetHomePosition().GetOrientation());
                                pHallegosa->DespawnOrUnsummon();
                            }
                            break;
                    }
                }
            }

        private:
            EventMap events;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_kalecgos_53392AI(creature);
        }
};

class npc_tarecgosa_53396 : public CreatureScript
{
    public:
        npc_tarecgosa_53396() : CreatureScript("npc_tarecgosa_53396") { }

        struct npc_tarecgosa_53396AI : ScriptedAI
        {
            npc_tarecgosa_53396AI(Creature* creature) : ScriptedAI(creature) 
            {
                me->setActive(true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tarecgosa_53396AI(creature);
        }
};

class npc_blue_dragon_53394 : public CreatureScript
{
    public:
        npc_blue_dragon_53394() : CreatureScript("npc_blue_dragon_53394") { }

        struct npc_blue_dragon_53394AI : ScriptedAI
        {
            npc_blue_dragon_53394AI(Creature* creature) : ScriptedAI(creature) 
            {
                me->setActive(true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_blue_dragon_53394AI(creature);
        }
};

class npc_stalker_53422 : public CreatureScript
{
    public:
        npc_stalker_53422() : CreatureScript("npc_stalker_53422") { }

        struct npc_stalker_53422AI : ScriptedAI
        {
            npc_stalker_53422AI(Creature* creature) : ScriptedAI(creature) 
            {
                me->setActive(true);
            }

            void OnQuestReward(Player* player, const Quest* quest, uint32 data) override
            {
                me->DespawnOrUnsummon();
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_stalker_53422AI(creature);
        }
};

void AddSC_tarecgosa_scripts()
{
    new npc_anachronos_15192();
    new npc_tarecgosa_52835();
    new npc_thyrinar();
    new npc_tarecgosa_53098();
    new npc_kalecgos_53149();
    new npc_kalecgos_53349();
    new npc_hallegosa();
    new npc_kalecgos_53392();
    new npc_tarecgosa_53396();
    new npc_blue_dragon_53394();
    new npc_stalker_53422();
}
