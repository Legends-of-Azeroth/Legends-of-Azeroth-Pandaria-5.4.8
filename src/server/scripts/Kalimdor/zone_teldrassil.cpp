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

/* ScriptData
SDName: Teldrassil
SD%Complete: 100
SDComment: Quest support: 938, 28727, 2561
SDCategory: Teldrassil
EndScriptData */

/* ContentData
npc_mist
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedFollowerAI.h"
#include "Player.h"

/*####
# npc_tarindrella
####*/

enum VileTouch
{
    QUEST_VILE_TOUCH = 28727,

    EVENT_CLEANSE_SPIRIT = 1,
    EVENT_ENTANGLING_ROOTS = 2,
    EVENT_SUMMON_NATURES_BITE = 3,

    SPELL_ENTANGLING_ROOTS = 33844,
    SPELL_SUMMON_NATURES_BITE = 92573,
    SPELL_CLEANSE_SPIRIT = 66056
};

enum VileTouchSays
{
    SAY_ON_COMPLETE = 0,
    SAY_ON_SPAWN = 1,
    SAY_ON_KILL = 2
};
struct npc_tarindrellaAI : public ScriptedAI
{
    npc_tarindrellaAI(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(Unit* unit) override
    {
        Talk(SAY_ON_SPAWN, unit);
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Talk(SAY_ON_KILL, me->GetOwner());
    }

    void OnQuestReward(Player* player, Quest const* quest, uint32 /*opt*/) override
    {
        if (quest->GetQuestId() == QUEST_VILE_TOUCH)
        {
            Talk(SAY_ON_COMPLETE, player);
            me->DespawnOrUnsummon();
        }
    }

    void Reset() override
    {
        events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, 2000);
        events.ScheduleEvent(EVENT_SUMMON_NATURES_BITE, 6000);
        events.ScheduleEvent(EVENT_CLEANSE_SPIRIT, 10000);
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
                case EVENT_ENTANGLING_ROOTS:
                {
                    DoCast(me->GetVictim(), SPELL_ENTANGLING_ROOTS);
                    events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, 10000);
                    break;
                }
                case EVENT_CLEANSE_SPIRIT:
                {
                    if (me->HasAura(6751))
                    {
                        DoCast(me, SPELL_ENTANGLING_ROOTS);
                        events.ScheduleEvent(EVENT_CLEANSE_SPIRIT, 10000);
                    }
                    else if (me->GetOwner()->HasAura(6751))
                    {
                        DoCast(me->GetOwner(), SPELL_ENTANGLING_ROOTS);
                        events.ScheduleEvent(EVENT_CLEANSE_SPIRIT, 10000);
                    }
                    else
                    {
                        events.ScheduleEvent(EVENT_CLEANSE_SPIRIT, 10000);
                    }
                    break;
                }
                case EVENT_SUMMON_NATURES_BITE:
                {
                    DoCast(me->GetVictim(), SPELL_SUMMON_NATURES_BITE);
                    events.ScheduleEvent(EVENT_SUMMON_NATURES_BITE, 10000);
                    break;
                }
                default:
                    break;
            }
        }
        DoMeleeAttackIfReady();
    }
    EventMap events;
};

/*####
# npc_mist
####*/

enum Mist
{
    SAY_AT_HOME             = 0,
    EMOTE_AT_HOME           = 1,
    QUEST_MIST              = 938,
    NPC_ARYNIA              = 3519,
    FACTION_DARNASSUS       = 79
};

class npc_mist : public CreatureScript
{
public:
    npc_mist() : CreatureScript("npc_mist") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_MIST)
            if (npc_mistAI* pMistAI = CAST_AI(npc_mist::npc_mistAI, creature->AI()))
                pMistAI->StartFollow(player, FACTION_DARNASSUS, quest);

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_mistAI(creature);
    }

    struct npc_mistAI : public FollowerAI
    {
        npc_mistAI(Creature* creature) : FollowerAI(creature) { }

        void Reset() override { }

        void MoveInLineOfSight(Unit* who) override

        {
            FollowerAI::MoveInLineOfSight(who);

            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && who->GetEntry() == NPC_ARYNIA)
            {
                if (me->IsWithinDistInMap(who, 10.0f))
                {
                    Talk(SAY_AT_HOME, who);
                    DoComplete();
                }
            }
        }

        void DoComplete()
        {
            Talk(EMOTE_AT_HOME);

            Player* player = GetLeaderForFollower();
            if (player && player->GetQuestStatus(QUEST_MIST) == QUEST_STATUS_INCOMPLETE)
                player->GroupEventHappens(QUEST_MIST, me);

            //The follow is over (and for later development, run off to the woods before really end)
            SetFollowComplete();
        }

        //call not needed here, no known abilities
        /*void UpdateFollowerAI(const uint32 Diff) override
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }*/
    };
};

enum
{
    GO_BARROW_DEN_DOOR      = 195112,
    QUEST_DRUID_OF_THE_CLAW = 2561,
};

class at_barrow_den_door : public AreaTriggerScript
{
    public:
        at_barrow_den_door() : AreaTriggerScript("at_barrow_den_door") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const*) override
        {
            if (player->GetQuestStatus(QUEST_DRUID_OF_THE_CLAW) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_DRUID_OF_THE_CLAW) == QUEST_STATUS_COMPLETE)
                if (GameObject* go = GetClosestGameObjectWithEntry(player, GO_BARROW_DEN_DOOR, 20.0f))
                    if (go->getLootState() == GO_READY)
                        go->UseDoorOrButton();
            return false;
        }
};

void AddSC_teldrassil()
{
    RegisterCreatureAI(npc_tarindrellaAI);
    new npc_mist();
    new at_barrow_den_door();
}
