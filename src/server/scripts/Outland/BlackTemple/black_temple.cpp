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
SDName: Black_Temple
SD%Complete: 95
SDComment: Spirit of Olum: Player Teleporter to Seer Kanai Teleport after defeating Naj'entus and Supremus. TODO: Find proper gossip.
SDCategory: Black Temple
EndScriptData */

/* ContentData
npc_spirit_of_olum
EndContentData */

#include "ScriptPCH.h"
#include "black_temple.h"

#define SPELL_TELEPORT      41566 // s41566 - Teleport to Ashtongue NPC's
#define GOSSIP_OLUM1        8750

class npc_spirit_of_olum : public CreatureScript
{
    public:
        npc_spirit_of_olum() : CreatureScript("npc_spirit_of_olum") { }

    

        
        struct npc_spirit_of_olumAI : public ScriptedAI
        {
            npc_spirit_of_olumAI(Creature* creature) : ScriptedAI(creature) { }

            bool OnGossipHello(Player* player) override
            {

                InstanceScript* instance = me->GetInstanceScript();

                if (instance && (instance->GetData(DATA_SUPREMUS_EVENT) >= DONE) && (instance->GetData(DATA_HIGH_WARLORD_NAJENTUS_EVENT) >= DONE))
                    player->ADD_GOSSIP_ITEM_DB(GOSSIP_OLUM1, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
                return true;
        
            }
        
            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {

                        player->PlayerTalkClass->ClearMenus();
                        if (action == GOSSIP_ACTION_INFO_DEF + 1)
                            player->CLOSE_GOSSIP_MENU();

                        player->InterruptNonMeleeSpells(false);
                        player->CastSpell(player, SPELL_TELEPORT, false);
                        return true;
        
            }
};

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_spirit_of_olumAI(creature);
        }
};

void AddSC_black_temple()
{
    new npc_spirit_of_olum();
}
