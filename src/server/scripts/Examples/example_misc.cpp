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
SDName: Example_Misc
SD%Complete: 100
SDComment: Item, Areatrigger and other small code examples
SDCategory: Script Examples
EndScriptData */

#include "ScriptMgr.h"
#include "GameObjectAI.h"
#include "Player.h"

class AT_example_areatrigger : public AreaTriggerScript
{
    public:

        AT_example_areatrigger()
            : AreaTriggerScript("example_areatrigger")
        {
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/) override
        {
            player->Kill(player);
            return true;
        }
};

class ItemUse_example_item : public ItemScript
{
    public:

        ItemUse_example_item()
            : ItemScript("example_item")
        {
        }

        bool OnUse(Player* /*player*/, Item* /*item*/, SpellCastTargets const& /*targets*/) override
        {
            sScriptMgr->LoadDatabase();
            return true;
        }
};

class GOHello_example_go_teleporter : public GameObjectScript
{
    public:

        GOHello_example_go_teleporter()
            : GameObjectScript("example_go_teleporter")
        {
        }

        
        struct GOHello_example_go_teleporterAI : public GameObjectAI
        {
            GOHello_example_go_teleporterAI(GameObject* go) : GameObjectAI(go) { }

            bool OnGossipHello(Player* player, bool isUse) override
            {

                player->TeleportTo(0, 1807.07f, 336.105f, 70.3975f, 0.0f);
                return false;
        
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new GOHello_example_go_teleporterAI(go);
        }
};

void AddSC_example_misc()
{
    new AT_example_areatrigger();
    new ItemUse_example_item();
    new GOHello_example_go_teleporter();
}
