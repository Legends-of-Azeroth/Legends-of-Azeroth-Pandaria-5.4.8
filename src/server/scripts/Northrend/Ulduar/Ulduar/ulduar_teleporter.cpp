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

#include "ScriptedGossip.h"
#include "GameObjectAI.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ulduar.h"
#include "Spell.h"

#define GOSSIP_SENDER_ULDUAR_PORT 603
#define GOSSIP_MENU_ULDUAR_TELEPORT 10389

class go_ulduar_teleporter : public GameObjectScript
{
    public:
        go_ulduar_teleporter() : GameObjectScript("go_ulduar_teleporter") { }

    

    

        struct go_ulduar_teleporterAI : public GameObjectAI
        {
            go_ulduar_teleporterAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {

                        bool ru = player->GetSession()->GetSessionDbLocaleIndex() == LOCALE_ruRU;

                        player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в Главный лагерь экспедиции." : "Teleport to the Expedition Base Camp.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_BASE_CAMP_TELEPORT);
                        if (InstanceScript* instance = me->GetInstanceScript())
                        {
                            if (instance->GetData(DATA_COLOSSUS) == 2) //count of 2 collossus death
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись на Плац." : "Teleport to the Formation Grounds.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_FORMATION_GROUNDS_TELEPORT);
                            if (instance->GetBossState(BOSS_LEVIATHAN) == DONE)
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в Гигантскую кузню." : "Teleport to the Colossal Forge.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_COLOSSAL_FORGE_TELEPORT);
                            if (instance->GetBossState(BOSS_XT002) == DONE)
                            {
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись на Мусорную свалку." : "Teleport to the Scrapyard.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_SCRAPYARD_TELEPORT);
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в Вестибюль.": "Teleport to the Antechamber of Ulduar.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_ANTECHAMBER_TELEPORT);
                            }
                            if (instance->GetBossState(BOSS_KOLOGARN) == DONE)
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в Обвалившуюся галерею.": "Teleport to the Shattered Walkway.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_SHATTERED_WALKWAY_TELEPORT);
                            if (instance->GetBossState(BOSS_AURIAYA) == DONE)
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в Оранжерею Жизни." : "Teleport to the Conservatory of Life.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_CONSERVATORY_TELEPORT);
                            if (instance->GetData(DATA_TRAM) == DONE || instance->GetBossState(BOSS_MIMIRON) == DONE)
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в Искру Воображения.": "Teleport to the Spark of Imagination.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_SPARK_OF_IMAGINATION_TELEPORT);
                            if (instance->GetBossState(BOSS_VEZAX) == DONE)
                                player->ADD_GOSSIP_ITEM(0, ru ? "Перенестись в темницу Йогг-Сарона." : "Teleport to the Prison of Yogg-Saron.", GOSSIP_SENDER_ULDUAR_PORT, SPELL_DESCENT_INTO_MADNESS_TELEPORT);
                        }

                        player->SEND_GOSSIP_MENU(player->GetGossipTextId(me), me->GetGUID());
                        return true;
        
            }
        
            bool OnGossipSelect(Player* player, uint32 sender, uint32 action) override
            {

                        player->PlayerTalkClass->ClearMenus();
                        player->CLOSE_GOSSIP_MENU();
                        SpellInfo const* spell = sSpellMgr->GetSpellInfo(action);
                        if (!spell)
                            return false;

                        if (player->IsInCombat())
                        {
                            Spell::SendCastResult(player, spell, 0, SPELL_FAILED_AFFECTING_COMBAT);
                            return true;
                        }

                        if (player->GetVehicle())
                            player->ExitVehicle();

                        if (player->IsMounted())
                        {
                            player->Dismount();
                            player->RemoveAurasByType(SPELL_AURA_MOUNTED);
                        }

                        if (sender == GOSSIP_SENDER_ULDUAR_PORT)
                            player->CastSpell(player, spell, true);

                        return true;
        
            }
};

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_ulduar_teleporterAI(go);
        }
};

void AddSC_ulduar_teleporter()
{
    new go_ulduar_teleporter();
}
