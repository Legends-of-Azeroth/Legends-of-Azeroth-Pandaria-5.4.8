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

/* ScriptData
SDName: Tanaris
SD%Complete: 80
SDComment: Quest support: 648, 10277, 10279(Special flight path).
SDCategory: Tanaris
EndScriptData */

/* ContentData
npc_custodian_of_time
npc_steward_of_time
npc_OOX17
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"
#include "Player.h"
#include "WorldSession.h"
#include "Vehicle.h"

enum TanarisData
{
    SPELL_EMERGENCY_ROCKET_PACK             = 75730,
    SPELL_ENABLE_ABILITIES                  = 75990,
    SPELL_TEMP_INVISIBILITY                 = 3680,

    NPC_STEAMWHEEDLE_BALLOON                = 40505,

    EVENT_RIDE_INVOKER                      = 1,
    EVENT_START_WAYPOINT                    = 2,

    ACTION_RIDE_INVOKER                     = 1,
    ACTION_ENABLE_ABILITIES                 = 2,
    ACTION_START_WP                         = 3,

    QUEST_ENTRY_ROCKET_RESCUE_A             = 25050,
    QUEST_ENTRY_ROCKET_RESCUE_H             = 24910
};

/*######
## npc_aquementas
######*/

enum Aquementas
{
    AGGRO_YELL_AQUE     = 0,

    SPELL_AQUA_JET      = 13586,
    SPELL_FROST_SHOCK   = 15089
};

class npc_aquementas : public CreatureScript
{
public:
    npc_aquementas() : CreatureScript("npc_aquementas") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_aquementasAI (creature);
    }

    struct npc_aquementasAI : public ScriptedAI
    {
        npc_aquementasAI(Creature* creature) : ScriptedAI(creature) { }

        uint32 SendItemTimer;
        uint32 SwitchFactionTimer;
        bool isFriendly;

        uint32 FrostShockTimer;
        uint32 AquaJetTimer;

        void Reset() override
        {
            SendItemTimer = 0;
            SwitchFactionTimer = 10000;
            me->SetFaction(35);
            isFriendly = true;

            AquaJetTimer = 5000;
            FrostShockTimer = 1000;
        }

        void SendItem(Unit* receiver)
        {
            Player* player = receiver->ToPlayer();

            if (player && player->HasItemCount(11169, 1, false) &&
                player->HasItemCount(11172, 11, false) &&
                player->HasItemCount(11173, 1, false) &&
                !player->HasItemCount(11522, 1, true))
            {
                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 11522, 1, NULL);
                if (msg == EQUIP_ERR_OK)
                    player->StoreNewItem(dest, 11522, 1, true);
            }
        }

        void JustEngagedWith(Unit* who) override
        {
            Talk(AGGRO_YELL_AQUE, who);
        }

        void UpdateAI(uint32 diff) override
        {
            if (isFriendly)
            {
                if (SwitchFactionTimer <= diff)
                {
                    me->SetFaction(91);
                    isFriendly = false;
                } else SwitchFactionTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (!isFriendly)
            {
                if (SendItemTimer <= diff)
                {
                    if (me->GetVictim()->GetTypeId() == TYPEID_PLAYER)
                        SendItem(me->GetVictim());
                    SendItemTimer = 5000;
                } else SendItemTimer -= diff;
            }

            if (FrostShockTimer <= diff)
            {
                DoCastVictim(SPELL_FROST_SHOCK);
                FrostShockTimer = 15000;
            } else FrostShockTimer -= diff;

            if (AquaJetTimer <= diff)
            {
                DoCast(me, SPELL_AQUA_JET);
                AquaJetTimer = 15000;
            } else AquaJetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

};

/*######
## npc_custodian_of_time
######*/

enum CustodianOfTime
{
    WHISPER_CUSTODIAN_1     = 0,
    WHISPER_CUSTODIAN_2     = 1,
    WHISPER_CUSTODIAN_3     = 2,
    WHISPER_CUSTODIAN_4     = 3,
    WHISPER_CUSTODIAN_5     = 4,
    WHISPER_CUSTODIAN_6     = 5,
    WHISPER_CUSTODIAN_7     = 6,
    WHISPER_CUSTODIAN_8     = 7,
    WHISPER_CUSTODIAN_9     = 8,
    WHISPER_CUSTODIAN_10    = 9,
    WHISPER_CUSTODIAN_11    = 10,
    WHISPER_CUSTODIAN_12    = 11,
    WHISPER_CUSTODIAN_13    = 12,
    WHISPER_CUSTODIAN_14    = 13
};

struct npc_custodian_of_time : public npc_escortAI
{
    npc_custodian_of_time(Creature* creature) : npc_escortAI(creature) { }

    void WaypointReached(uint32 waypointId) override
    {
        if (Player* player = GetPlayerForEscort())
        {
            switch (waypointId)
            {
                case 0:
                    Talk(WHISPER_CUSTODIAN_1, player);
                    break;
                case 1:
                    Talk(WHISPER_CUSTODIAN_2, player);
                    break;
                case 2:
                    Talk(WHISPER_CUSTODIAN_3, player);
                    break;
                case 3:
                    Talk(WHISPER_CUSTODIAN_4, player);
                    break;
                case 5:
                    Talk(WHISPER_CUSTODIAN_5, player);
                    break;
                case 6:
                    Talk(WHISPER_CUSTODIAN_6, player);
                    break;
                case 7:
                    Talk(WHISPER_CUSTODIAN_7, player);
                    break;
                case 8:
                    Talk(WHISPER_CUSTODIAN_8, player);
                    break;
                case 9:
                    Talk(WHISPER_CUSTODIAN_9, player);
                    break;
                case 10:
                    Talk(WHISPER_CUSTODIAN_4, player);
                    break;
                case 13:
                    Talk(WHISPER_CUSTODIAN_10, player);
                    break;
                case 14:
                    Talk(WHISPER_CUSTODIAN_4, player);
                    break;
                case 16:
                    Talk(WHISPER_CUSTODIAN_11, player);
                    break;
                case 17:
                    Talk(WHISPER_CUSTODIAN_12, player);
                    break;
                case 18:
                    Talk(WHISPER_CUSTODIAN_4, player);
                    break;
                case 22:
                    Talk(WHISPER_CUSTODIAN_13, player);
                    break;
                case 23:
                    Talk(WHISPER_CUSTODIAN_4, player);
                    break;
                case 24:
                    Talk(WHISPER_CUSTODIAN_14, player);
                    DoCast(player, 34883);
                    // below here is temporary workaround, to be removed when spell works properly
                    player->AreaExploredOrEventHappens(10277);
                    break;
            }
        }
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        if (Player* player = who->ToPlayer())
        {
            if (who->HasAura(34877) && player->GetQuestStatus(10277) == QUEST_STATUS_INCOMPLETE)
            {
                float Radius = 10.0f;
                if (me->IsWithinDistInMap(who, Radius))
                {
                    Start(false, false, who->GetGUID());
                }
            }
        }
    }

    void JustEngagedWith(Unit* /*who*/) override { }
    void Reset() override { }

    void UpdateAI(uint32 diff) override
    {
        npc_escortAI::UpdateAI(diff);
    }
};

/*######
## npc_steward_of_time
######*/

class npc_steward_of_time : public CreatureScript
{
public:
    npc_steward_of_time() : CreatureScript("npc_steward_of_time") { }

    bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* quest) override
    {
        if (quest->GetQuestId() == 10279)                      //Quest: To The Master's Lair
            player->CastSpell(player, 34891, true);               //(Flight through Caverns)

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
            player->CastSpell(player, 34891, true);               //(Flight through Caverns)

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(10279) == QUEST_STATUS_INCOMPLETE || player->GetQuestRewardStatus(10279))
        {
            player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(9978, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(9977, creature->GetGUID());

        return true;
    }

};

/*######
## npc_OOX17
######*/

enum Npc00X17
{
    SAY_OOX_START           = 0,
    SAY_OOX_AGGRO           = 1,
    SAY_OOX_AMBUSH          = 2,
    SAY_OOX17_AMBUSH_REPLY  = 0,
    SAY_OOX_END             = 3,

    Q_OOX17                 = 648,
    SPAWN_FIRST             = 7803,
    SPAWN_SECOND_1          = 5617,
    SPAWN_SECOND_2          = 7805
};

class npc_OOX17 : public CreatureScript
{
public:
    npc_OOX17() : CreatureScript("npc_OOX17") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == Q_OOX17)
        {
            creature->SetFaction(113);
            creature->SetFullHealth();
            creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
            creature->AI()->Talk(SAY_OOX_START);

            if (npc_escortAI* pEscortAI = CAST_AI(npc_OOX17::npc_OOX17AI, creature->AI()))
                pEscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_OOX17AI(creature);
    }

    struct npc_OOX17AI : public npc_escortAI
    {
        npc_OOX17AI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 waypointId) override
        {
            if (Player* player = GetPlayerForEscort())
            {
                switch (waypointId)
                {
                    case 23:
                        me->SummonCreature(SPAWN_FIRST, -8350.96f, -4445.79f, 10.10f, 6.20f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000ms);
                        me->SummonCreature(SPAWN_FIRST, -8355.96f, -4447.79f, 10.10f, 6.27f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000ms);
                        me->SummonCreature(SPAWN_FIRST, -8353.96f, -4442.79f, 10.10f, 6.08f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000ms);
                        Talk(SAY_OOX_AMBUSH);
                        break;
                    case 56:
                        me->SummonCreature(SPAWN_SECOND_1, -7510.07f, -4795.50f, 9.35f, 6.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000ms);
                        me->SummonCreature(SPAWN_SECOND_2, -7515.07f, -4797.50f, 9.35f, 6.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000ms);
                        me->SummonCreature(SPAWN_SECOND_2, -7518.07f, -4792.50f, 9.35f, 6.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000ms);
                        Talk(SAY_OOX_AMBUSH);
                        if (Creature* scoff = me->FindNearestCreature(SPAWN_SECOND_2, 30))
                            scoff->AI()->Talk(SAY_OOX17_AMBUSH_REPLY);
                        break;
                    case 86:
                        Talk(SAY_OOX_END);
                        player->GroupEventHappens(Q_OOX17, me);
                        break;
                }
            }
        }

        void Reset()override { }

        void JustEngagedWith(Unit* /*who*/) override
        {
            Talk(SAY_OOX_AGGRO);
        }

        void JustSummoned(Creature* summoned) override
        {
            summoned->AI()->AttackStart(me);
        }
    };
};

/*####
# npc_tooga
####*/

enum Tooga
{
    SAY_TOOG_WORRIED            = 0,
    SAY_TOOG_POST_1             = 1,
    SAY_TORT_POST_2             = 0,
    SAY_TOOG_POST_3             = 2,
    SAY_TORT_POST_4             = 1,
    SAY_TOOG_POST_5             = 3,
    SAY_TORT_POST_6             = 2,

    QUEST_TOOGA                 = 1560,
    NPC_TORTA                   = 6015,

    POINT_ID_TO_WATER           = 1,
    FACTION_TOOG_ESCORTEE       = 113
};

Position const ToWaterLoc = {-7032.664551f, -4906.199219f, -1.606446f, 0.0f};

class npc_tooga : public CreatureScript
{
public:
    npc_tooga() : CreatureScript("npc_tooga") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest) override
    {
        if (quest->GetQuestId() == QUEST_TOOGA)
        {
            if (npc_toogaAI* pToogaAI = CAST_AI(npc_tooga::npc_toogaAI, creature->AI()))
                pToogaAI->StartFollow(player, FACTION_TOOG_ESCORTEE, quest);
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_toogaAI(creature);
    }

    struct npc_toogaAI : public FollowerAI
    {
        npc_toogaAI(Creature* creature) : FollowerAI(creature) { }

        uint32 CheckSpeechTimer;
        uint32 PostEventTimer;
        uint32 PhasePostEvent;

        ObjectGuid TortaGUID;

        void Reset() override
        {
            CheckSpeechTimer = 2500;
            PostEventTimer = 1000;
            PhasePostEvent = 0;

            TortaGUID = ObjectGuid::Empty;
        }

        void MoveInLineOfSight(Unit* who) override
        {
            FollowerAI::MoveInLineOfSight(who);

            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE | STATE_FOLLOW_POSTEVENT) && who->GetEntry() == NPC_TORTA)
            {
                if (me->IsWithinDistInMap(who, INTERACTION_DISTANCE))
                {
                    Player* player = GetLeaderForFollower();
                    if (player && player->GetQuestStatus(QUEST_TOOGA) == QUEST_STATUS_INCOMPLETE)
                        player->GroupEventHappens(QUEST_TOOGA, me);

                    TortaGUID = who->GetGUID();
                    SetFollowComplete(true);
                }
            }
        }

        void MovementInform(uint32 MotionType, uint32 PointId) override
        {
            FollowerAI::MovementInform(MotionType, PointId);

            if (MotionType != POINT_MOTION_TYPE)
                return;

            if (PointId == POINT_ID_TO_WATER)
                SetFollowComplete();
        }

        void UpdateFollowerAI(uint32 Diff) override
        {
            if (!UpdateVictim())
            {
                //we are doing the post-event, or...
                if (HasFollowState(STATE_FOLLOW_POSTEVENT))
                {
                    if (PostEventTimer <= Diff)
                    {
                        PostEventTimer = 5000;

                        Creature* torta = Creature::GetCreature(*me, TortaGUID);
                        if (!torta || !torta->IsAlive())
                        {
                            //something happened, so just complete
                            SetFollowComplete();
                            return;
                        }

                        switch (PhasePostEvent)
                        {
                            case 1:
                                Talk(SAY_TOOG_POST_1);
                                break;
                            case 2:
                                torta->AI()->Talk(SAY_TORT_POST_2);
                                break;
                            case 3:
                                Talk(SAY_TOOG_POST_3);
                                break;
                            case 4:
                                torta->AI()->Talk(SAY_TORT_POST_4);
                                break;
                            case 5:
                                Talk(SAY_TOOG_POST_5);
                                break;
                            case 6:
                                torta->AI()->Talk(SAY_TORT_POST_6);
                                me->GetMotionMaster()->MovePoint(POINT_ID_TO_WATER, ToWaterLoc);
                                break;
                        }

                        ++PhasePostEvent;
                    }
                    else
                        PostEventTimer -= Diff;
                }
                //...we are doing regular speech check
                else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
                {
                    if (CheckSpeechTimer <= Diff)
                    {
                        CheckSpeechTimer = 5000;

                        if (urand(0, 9) > 8)
                            Talk(SAY_TOOG_WORRIED);
                    }
                    else
                        CheckSpeechTimer -= Diff;
                }

                return;
            }

            DoMeleeAttackIfReady();
        }
    };

};

class npc_steamwheedle_balloon : public CreatureScript
{
public:
   npc_steamwheedle_balloon() : CreatureScript("npc_steamwheedle_balloon") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (player->GetQuestStatus(QUEST_ENTRY_ROCKET_RESCUE_H) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_ENTRY_ROCKET_RESCUE_A))
        {
            if (!player->GetVehicleBase() && !creature->HasAura(SPELL_TEMP_INVISIBILITY))
            {
                player->SummonCreature(NPC_STEAMWHEEDLE_BALLOON, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 600000ms);
                creature->AddAura(SPELL_TEMP_INVISIBILITY, creature);
                return true;
            }
        }
        return true;
    }
};

class npc_steamwheedle_balloon_escort : public CreatureScript
{
public:
    npc_steamwheedle_balloon_escort() : CreatureScript("npc_steamwheedle_balloon_escort") { }

    struct npc_steamwheedle_balloon_escortAI : public npc_escortAI
    {
        npc_steamwheedle_balloon_escortAI(Creature* creature) : npc_escortAI(creature)
        {
            playerQuester = nullptr;
        }

        EventMap events;

        void OnCharmed(bool apply) override { }

        void WaypointReached(uint32 point) override
        {
            switch (point)
            {
                case 3:
                {
                    if (playerQuester && playerQuester != NULL)
                    {
                        if (Creature* vehicle = playerQuester->GetVehicleCreatureBase())
                            vehicle->AI()->DoAction(ACTION_ENABLE_ABILITIES);
                    }
                    break;
                }
                case 16:
                {
                    SetNextWaypoint(3, false);
                    break;
                }
                default:
                    break;
            }
        }

        void IsSummonedBy(Unit* owner) override
        {
            playerQuester = owner;
            me->SetReactState(REACT_PASSIVE);
            events.ScheduleEvent(EVENT_START_WAYPOINT, 100ms);
        }

        void DoAction(int32 action) override
        {
            switch (action)
            {
                case ACTION_START_WP:
                {
                    Start(false, true, ObjectGuid::Empty, NULL, false, true, true);
                    SetDespawnAtEnd(false);
                    break;
                }
                default:
                    break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            events.Update(diff);
            npc_escortAI::UpdateAI(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_START_WAYPOINT:
                    {
                        events.CancelEvent(EVENT_START_WAYPOINT);
                        DoAction(ACTION_START_WP);
                        break;
                    }
                    default:
                        break;
                }
            }
        }

    protected:
        Unit* playerQuester;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_steamwheedle_balloon_escortAI(creature);
    }
};

struct npc_balloon_throwing_station : public ScriptedAI
{
    npc_balloon_throwing_station(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void OnCharmed(bool apply) override { }

    void IsSummonedBy(Unit* owner) override
    {
        events.ScheduleEvent(EVENT_RIDE_INVOKER, 1s + 500ms);
        me->SetReactState(REACT_PASSIVE);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_RIDE_INVOKER:
            {
                if (Unit* myOwner = me->ToTempSummon()->GetSummoner())
                {
                    if (Unit* ownerOwner = myOwner->ToTempSummon()->GetSummoner())
                        ownerOwner->EnterVehicle(me, 0);
                }
                break;
            }
            case ACTION_ENABLE_ABILITIES:
            {
                me->AddAura(SPELL_ENABLE_ABILITIES, me);
                break;
            }
            default:
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
                case EVENT_RIDE_INVOKER:
                {
                    events.CancelEvent(EVENT_RIDE_INVOKER);
                    DoAction(ACTION_RIDE_INVOKER);
                    break;
                }
                default:
                    break;
            }
        }
    }
};

class spell_emergency_rocket_pack : public SpellScript
{
    PrepareSpellScript(spell_emergency_rocket_pack);

    void HandleReturnToGadgetzan()
    {
        if (Unit* caster = GetCaster())
        {
            if (Unit* passenger = caster->GetVehicleKit()->GetPassenger(0))
            {
                if (Vehicle* vehicle = caster->GetVehicleKit())
                    vehicle->GetBase()->ToCreature()->DespawnOrUnsummon(2000);

                passenger->ExitVehicle();
                passenger->CastSpell(caster, SPELL_EMERGENCY_ROCKET_PACK, true);
                passenger->GetMotionMaster()->MoveJump(-7114.65f, -3888.82f, 75.0f, 45.0f, 25.0f, 10);
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_emergency_rocket_pack::HandleReturnToGadgetzan);
    }
};

void AddSC_tanaris()
{
    RegisterCreatureAI(npc_custodian_of_time);
    new npc_steward_of_time();
    new npc_OOX17();
    new npc_steamwheedle_balloon();
    new npc_steamwheedle_balloon_escort();
    new creature_script<npc_balloon_throwing_station>("npc_balloon_throwing_station");
    new spell_script<spell_emergency_rocket_pack>("spell_emergency_rocket_pack");
}
