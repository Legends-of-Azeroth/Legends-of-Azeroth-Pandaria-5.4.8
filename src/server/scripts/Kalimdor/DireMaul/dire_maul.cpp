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
#include "dire_maul.h"

// Old Ironbark 11491
class npc_old_ironbark : public CreatureScript
{
    public:
        npc_old_ironbark() : CreatureScript("npc_old_ironbark") { }

    

        

        struct npc_old_ironbarkAI : public ScriptedAI
        {
            npc_old_ironbarkAI(Creature* creature) : ScriptedAI(creature) { }

            void UpdateAI(uint32 /*diff*/) override { }
                    bool OnGossipHello(Player* player) override
            {

                if (me->IsQuestGiver())
                    player->PrepareQuestMenu(me->GetGUID());

                player->ADD_GOSSIP_ITEM_DB(player->GetDefaultGossipMenuForSource(me), 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

                player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
                return true;
        
            }
        
            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {

                        player->PlayerTalkClass->ClearMenus();

                        if (action == GOSSIP_ACTION_INFO_DEF + 1)
                        {
                            me->RemoveFlag(UNIT_FIELD_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                            uint32 delay = 0;
                            ObjectGuid guid = me->GetGUID();

                            Movement::MoveSplineInit init(me);
                            for (auto itr : ironbarkPath)
                                init.Path().push_back(G3D::Vector3(itr.GetPositionX(), itr.GetPositionY(), itr.GetPositionZ()));

                            init.SetSmooth();
                            init.SetUncompressed();
                            init.Launch();
                            me->m_Events.Schedule(delay += me->GetSplineDuration(), 1, [this,guid]()
                            {
                                if (Unit* ironbark = ObjectAccessor::GetUnit(*me, guid))
                                    if (InstanceScript* instance = ironbark->GetInstanceScript())
                                        if (GameObject* door = ObjectAccessor::GetGameObject(*ironbark, instance->GetGuidData(GO_CONSERVATORY_DOOR)))
                                            door->UseDoorOrButton();
                            });
                        }

                        player->CLOSE_GOSSIP_MENU();

                        return true;
        
            }
};

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_old_ironbarkAI(creature);
        }
};

void AddSC_dire_maul()
{
    new npc_old_ironbark();
}