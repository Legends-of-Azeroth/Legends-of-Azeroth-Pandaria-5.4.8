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
#include "BattlefieldMgr.h"
#include "BattlefieldTB.h"
#include "Battlefield.h"
#include "ScriptSystem.h"
#include "WorldSession.h"
#include "ObjectMgr.h"

enum eTBqueuenpctext
{
    TB_NPCQUEUE_TEXT_H_NOWAR       = 110006,
    TB_NPCQUEUE_TEXT_H_QUEUE       = 110002,
    TB_NPCQUEUE_TEXT_H_WAR         = 110005,
    TB_NPCQUEUE_TEXT_A_NOWAR       = 110003,
    TB_NPCQUEUE_TEXT_A_QUEUE       = 110001,
    TB_NPCQUEUE_TEXT_A_WAR         = 110004,
    TB_NPCQUEUE_TEXTOPTION_JOIN    = -1732009,
};

class npc_tb_spiritguide : public CreatureScript
{
    public:
        npc_tb_spiritguide() : CreatureScript("npc_tb_spiritguide") { }

        

    
        struct npc_tb_spiritguideAI : public ScriptedAI
        {
            npc_tb_spiritguideAI(Creature* creature) : ScriptedAI(creature) { }

            bool OnGossipHello(Player* player) override
            {

                if (me->isQuestGiver())
                    player->PrepareQuestMenu(me->GetGUID());

                Battlefield* BfTB = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
                if (BfTB)
                {
                    GraveyardVect graveyard = BfTB->GetGraveyardVector();
                    for (uint8 i = 0; i < graveyard.size(); i++)
                        if (graveyard[i]->GetControlTeamId() == player->GetTeamId())
                           player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetTrinityStringForDBCLocale(((BfGraveyardTB*)graveyard[i])->GetTextId()), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + i);
                }

                player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
                return true;
        
            }
        
            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {

                        player->CLOSE_GOSSIP_MENU();

                        Battlefield* BfTB = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
                        if (BfTB)
                        {
                            GraveyardVect graveyard = BfTB->GetGraveyardVector();
                            for (uint8 i = 0; i < graveyard.size(); i++)
                                if (action - GOSSIP_ACTION_INFO_DEF == i && graveyard[i]->GetControlTeamId() == player->GetTeamId())
                                    if (WorldSafeLocsEntry const* safeLoc = sWorldSafeLocsStore.LookupEntry(graveyard[i]->GetGraveyardId()))
                                    player->TeleportTo(safeLoc->map_id, safeLoc->x, safeLoc->y, safeLoc->z, 0);
                        }
                        return true;
        
            }
};

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tb_spiritguideAI(creature);
        }
};

class npc_tol_barad_battlemage : public CreatureScript
{
    public:
        npc_tol_barad_battlemage() : CreatureScript("npc_tol_barad_battlemage") { }

        

    
        struct npc_tol_barad_battlemageAI : public ScriptedAI
        {
            npc_tol_barad_battlemageAI(Creature* creature) : ScriptedAI(creature) { }

            bool OnGossipHello(Player* player) override
            {

                if (me->isQuestGiver())
                    player->PrepareQuestMenu(me->GetGUID());

                Battlefield* BfTB = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
                if (BfTB)
                {
                    if (BfTB->IsWarTime())
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetTrinityStringForDBCLocale(TB_NPCQUEUE_TEXTOPTION_JOIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                        player->SEND_GOSSIP_MENU(BfTB->GetDefenderTeam() ? TB_NPCQUEUE_TEXT_H_WAR : TB_NPCQUEUE_TEXT_A_WAR, me->GetGUID());
                    }
                    else
                    {
                        uint32 uiTime = BfTB->GetTimer()/1000;
                        player->SendUpdateWorldState(5332, time(NULL)+uiTime);
                        if (uiTime < 15 * MINUTE)
                        {
                            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetTrinityStringForDBCLocale(TB_NPCQUEUE_TEXTOPTION_JOIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                            player->SEND_GOSSIP_MENU(BfTB->GetDefenderTeam() ? TB_NPCQUEUE_TEXT_H_QUEUE : TB_NPCQUEUE_TEXT_A_QUEUE, me->GetGUID());
                        }
                        else
                        {
                            player->SEND_GOSSIP_MENU(BfTB->GetDefenderTeam() ? TB_NPCQUEUE_TEXT_H_NOWAR:TB_NPCQUEUE_TEXT_A_NOWAR, me->GetGUID());
                        }
                    }
                }
                return true;
        
            }
        
            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {

                        player->CLOSE_GOSSIP_MENU();

                        Battlefield* BfTB = sBattlefieldMgr->GetBattlefieldByBattleId(BATTLEFIELD_BATTLEID_TB);
                        if (BfTB)
                        {
                            if (BfTB->IsWarTime()){
                                BfTB->InvitePlayerToWar(player);
                            }
                            else
                            {
                                uint32 uiTime = BfTB->GetTimer()/1000;
                                if (uiTime < 15 * MINUTE)
                                    BfTB->InvitePlayerToQueue(player);
                            }
                        }
                        return true;
        
            }
};

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_tol_barad_battlemageAI(creature);
        }
};

// 85123 - Siege Cannon - selects random target
class spell_siege_cannon : public SpellScriptLoader
{
public:
    spell_siege_cannon() : SpellScriptLoader("spell_siege_cannon") { }

    class spell_siege_cannon_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_siege_cannon_SpellScript);

        void SelectRandomTarget(std::list<WorldObject*>& targets)
        {
            if (targets.empty())
                return;

            WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
            targets.clear();
            targets.push_back(target);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_siege_cannon_SpellScript::SelectRandomTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_siege_cannon_SpellScript();
    }
};

void AddSC_tol_barad()
{
   new npc_tol_barad_battlemage();
   new npc_tb_spiritguide();
}
