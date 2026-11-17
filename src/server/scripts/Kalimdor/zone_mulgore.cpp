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
SDName: Mulgore
SD%Complete: 100
SDComment: Support for quest: 11129, 861
SDCategory: Mulgore
EndScriptData */

/* ContentData
npc_skorn_whitecloud
npc_kyle_frenzied
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "SpellInfo.h"

/*######
# npc_skorn_whitecloud
######*/

#define GOSSIP_SW "Tell me a story, Skorn."

class npc_skorn_whitecloud : public CreatureScript
{
public:
    npc_skorn_whitecloud() : CreatureScript("npc_skorn_whitecloud") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
            player->SEND_GOSSIP_MENU(523, creature->GetGUID());

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (!player->GetQuestRewardStatus(770))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(522, creature->GetGUID());

        return true;
    }

};

/*#####
# npc_kyle_frenzied
######*/

enum KyleFrenzied
{
    EMOTE_SEE_LUNCH         = 0,
    EMOTE_EAT_LUNCH         = 1,
    EMOTE_DANCE             = 2,

    SPELL_LUNCH             = 42222,
    NPC_KYLE_FRENZIED       = 23616,
    NPC_KYLE_FRIENDLY       = 23622,
    POINT_ID                = 1
};

class npc_kyle_frenzied : public CreatureScript
{
public:
    npc_kyle_frenzied() : CreatureScript("npc_kyle_frenzied") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kyle_frenziedAI (creature);
    }

    struct npc_kyle_frenziedAI : public ScriptedAI
    {
        npc_kyle_frenziedAI(Creature* creature) : ScriptedAI(creature) { }

        bool EventActive;
        bool IsMovingToLunch;
        ObjectGuid PlayerGUID;
        uint32 EventTimer;
        uint8 EventPhase;

        void Reset() override
        {
            EventActive = false;
            IsMovingToLunch = false;
            PlayerGUID = ObjectGuid::Empty;
            EventTimer = 5000;
            EventPhase = 0;

            if (me->GetEntry() == NPC_KYLE_FRIENDLY)
                me->UpdateEntry(NPC_KYLE_FRENZIED);
        }

        void SpellHit(Unit* Caster, SpellInfo const* Spell) override
        {
            if (!me->GetVictim() && !EventActive && Spell->Id == SPELL_LUNCH)
            {
                if (Caster->GetTypeId() == TYPEID_PLAYER)
                    PlayerGUID = Caster->GetGUID();

                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                {
                    me->GetMotionMaster()->MovementExpired();
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                }

                EventActive = true;
                Talk(EMOTE_SEE_LUNCH);
                me->HandleEmoteStateCommand(EMOTE_ONESHOT_CREATURE_SPECIAL);
            }
        }

        void MovementInform(uint32 type, uint32 pointId) override
        {
            if (type != POINT_MOTION_TYPE || !EventActive)
                return;

            if (pointId == POINT_ID)
                IsMovingToLunch = false;
        }

        void UpdateAI(uint32 diff) override
        {
            if (EventActive)
            {
                if (IsMovingToLunch)
                    return;

                if (EventTimer <= diff)
                {
                    EventTimer = 5000;
                    ++EventPhase;

                    switch (EventPhase)
                    {
                        case 1:
                            if (Unit* unit = ObjectAccessor::GetUnit(*me, PlayerGUID))
                            {
                                if (GameObject* go = unit->GetGameObject(SPELL_LUNCH))
                                {
                                    IsMovingToLunch = true;
                                    me->GetMotionMaster()->MovePoint(POINT_ID, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
                                }
                            }
                            break;
                        case 2:
                            Talk(EMOTE_EAT_LUNCH);
                            me->HandleEmoteStateCommand(EMOTE_STATE_USE_STANDING);
                            break;
                        case 3:
                            if (Player* unit = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                                unit->TalkedToCreature(me->GetEntry(), me->GetGUID());

                            me->UpdateEntry(NPC_KYLE_FRIENDLY);
                            break;
                        case 4:
                            EventTimer = 30000;
                            Talk(EMOTE_DANCE);

                            if (Player* unit = ObjectAccessor::GetPlayer(*me, PlayerGUID))
                                unit->KilledMonsterCredit(NPC_KYLE_FRENZIED);

                            me->HandleEmoteStateCommand(EMOTE_STATE_DANCESPECIAL);
                            break;
                        case 5:
                            me->HandleEmoteStateCommand(EMOTE_STATE_NONE);
                            Reset();
                            me->GetMotionMaster()->Clear();
                            break;
                    }
                }
                else
                    EventTimer -= diff;
            }
        }
    };
};

enum Events
{
    EVENT_CAST_WAR_STOMP = 1,
    EVENT_CAST_CLEAVE = 2
};

struct npc_fledgling_brave : public ScriptedAI
{
    npc_fledgling_brave(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        events.ScheduleEvent(EVENT_CAST_WAR_STOMP, 10000);
        events.ScheduleEvent(EVENT_CAST_CLEAVE, 10000);
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker->GetTypeId() != TYPEID_PLAYER && !HealthAbovePct(85))
            damage = 0;
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
                case EVENT_CAST_WAR_STOMP:
                    DoCastVictim(81500);
                    events.ScheduleEvent(EVENT_CAST_WAR_STOMP, urand(3.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    break;
                case EVENT_CAST_CLEAVE:
                    DoCastVictim(81502);
                    events.ScheduleEvent(EVENT_CAST_CLEAVE, urand(3.5 * IN_MILLISECONDS, 12 * IN_MILLISECONDS));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap events;
};

struct npc_bristleback_invader : public ScriptedAI
{
    npc_bristleback_invader(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        cast_rend_timer = 10000;
    }

    void DamageTaken(Unit* attacker, uint32& damage) override
    {
        if (attacker->GetTypeId() != TYPEID_PLAYER && !HealthAbovePct(85))
            damage = 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        cast_rend_timer -= diff;
        if (cast_rend_timer <= 0)
        {
            cast_rend_timer = 30000;
            DoCastVictim(11977);
        }

        DoMeleeAttackIfReady();
    }

private:
    uint32 cast_rend_timer;
};

void AddSC_mulgore()
{
    new npc_skorn_whitecloud();
    new npc_kyle_frenzied();
    new creature_script<npc_fledgling_brave>("npc_fledgling_brave");
    new creature_script<npc_bristleback_invader>("npc_bristleback_invader");
}
