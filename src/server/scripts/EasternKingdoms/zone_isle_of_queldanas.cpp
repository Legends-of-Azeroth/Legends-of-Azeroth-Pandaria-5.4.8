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
SDName: Isle_of_Queldanas
SD%Complete: 100
SDComment: Quest support: 11524, 11525, 11532, 11533, 11542, 11543, 11541
SDCategory: Isle Of Quel'Danas
EndScriptData */

/* ContentData
npc_converted_sentry
npc_greengill_slave
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Pet.h"
#include "SpellInfo.h"

/*######
## npc_converted_sentry
######*/
enum ConvertedSentry
{
    SAY_CONVERTED           = 0,

    SPELL_CONVERT_CREDIT    = 45009
};

class npc_converted_sentry : public CreatureScript
{
public:
    npc_converted_sentry() : CreatureScript("npc_converted_sentry") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_converted_sentryAI(creature);
    }

    struct npc_converted_sentryAI : public ScriptedAI
    {
        npc_converted_sentryAI(Creature* creature) : ScriptedAI(creature) { }

        bool Credit;
        uint32 Timer;

        void Reset() override
        {
            Credit = false;
            Timer = 2500;
        }

        void MoveInLineOfSight(Unit* /*who*/) override { }

        void JustEngagedWith(Unit* /*who*/) override { }

        void UpdateAI(uint32 diff) override
        {
            if (!Credit)
            {
                if (Timer <= diff)
                {
                    Talk(SAY_CONVERTED);

                    DoCast(me, SPELL_CONVERT_CREDIT);
                    if (me->IsPet())
                        me->ToPet()->SetDuration(7500);
                    Credit = true;
                } else Timer -= diff;
            }
        }
    };
};

/*######
## npc_greengill_slave
######*/

#define ENRAGE  45111
#define ORB     45109
#define QUESTG  11541
#define DM      25060

class npc_greengill_slave : public CreatureScript
{
public:
    npc_greengill_slave() : CreatureScript("npc_greengill_slave") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_greengill_slaveAI(creature);
    }

    struct npc_greengill_slaveAI : public ScriptedAI
    {
        npc_greengill_slaveAI(Creature* creature) : ScriptedAI(creature) { }

        void JustEngagedWith(Unit* /*who*/) override { }

        void SpellHit(Unit* caster, SpellInfo const* spellInfo) override
        {
            Player* player = caster->ToPlayer();
            if (!player)
                return;

            if (spellInfo->Id == ORB && !me->HasAura(ENRAGE))
            {
                if (player->GetQuestStatus(QUESTG) == QUEST_STATUS_INCOMPLETE)
                    DoCast(player, 45110, true);

                DoCast(me, ENRAGE);

                if (Creature* Myrmidon = me->FindNearestCreature(DM, 70))
                {
                    me->GetThreatManager().AddThreat(Myrmidon, 100000.0f);
                    AttackStart(Myrmidon);
                }
            }
        }

        void UpdateAI(uint32 /*diff*/) override
        {
            DoMeleeAttackIfReady();
        }
    };
};

enum wardenSpells
{
    SPELL_TELEPORT_INTO_SUNWELL = 70746,
};

const std::vector<uint32> queldelarQuests =
{
    24598,
    24553,
    24595,
    24564,
    24594,
    24596,
};

// Warden of the Sunwell 37523
class npc_warden_of_the_sunwell : public CreatureScript
{
    public:
        npc_warden_of_the_sunwell() : CreatureScript("npc_warden_of_the_sunwell") { }

        bool OnGossipHello(Player* player, Creature* creature) override
        {
            if (HasAnyQueldelarQuest(player))
                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(creature), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
                player->CastSpell(player, SPELL_TELEPORT_INTO_SUNWELL, true);

            player->CLOSE_GOSSIP_MENU();
            return false;
        }

        bool HasAnyQueldelarQuest(Player* questHolder)
        {
            for (auto&& itr : queldelarQuests)
                if (questHolder->GetQuestStatus(itr) == QUEST_STATUS_INCOMPLETE)
                    return true;

            return false;
        }
};

void AddSC_isle_of_queldanas()
{
    new npc_converted_sentry();
    new npc_greengill_slave();
    new npc_warden_of_the_sunwell();
}
