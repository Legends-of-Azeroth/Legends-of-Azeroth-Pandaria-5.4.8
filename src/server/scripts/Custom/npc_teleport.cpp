#include "Vehicle.h"
#include "ScriptPCH.h"
#include "Common.h"
#include "Chat.h"
#include "Group.h"
#include "Player.h"
#include "DisableMgr.h"
#include "ChannelMgr.h"
#include "Config.h"
#include "Language.h"
#include "World.h"
#include "ScriptedGossip.h"
#include "DatabaseEnv.h"
#include <sstream>

#include "ScriptPCH.h"
#include "Chat.h"
#include "Player.h"

enum NPC_TELEPORT
{
    GOSSIP_MENU_NPCTELEPORT                                       = 900000,  
    NPC_TEXT_NPCTELEPORT                                          = 9000000, 
    
    GOSSIP_OPTION_ALLIANCE_SHRINE_OF_THE_SEVEN_STARS              = 1,
    GOSSIP_OPTION_ALLIANCE_DARNASSUS                              = 2,
    GOSSIP_OPTION_ALLIANCE_EXODAR                                 = 3,
    GOSSIP_OPTION_ALLIANCE_IRONFORGE                              = 4,
    GOSSIP_OPTION_ALLIANCE_STORMWIND                              = 5,

    GOSSIP_OPTION_HORDE_SHRINE_OF_THE_TWO_MOONS                   = 6,
    GOSSIP_OPTION_HORDE_ORGRIMMAR                                 = 7,
    GOSSIP_OPTION_HORDE_SLIVERMOON                                = 8,
    GOSSIP_OPTION_HORDE_THUNDERBLUFF                              = 9,
    GOSSIP_OPTION_HORDE_UNDERCITY                                 = 10,

    GOSSIP_OPTION_GLOBAL_DALARAN                                  = 11,
    GOSSIP_OPTION_GLOBAL_SHATTRATH                                = 12,

};


/**
 * ALLIANCE
	Shrine of the Seven Stars (870, 821f, 253f, 503f, 0.59f);
	Darnassus (1, 9949f, 2284f, 1341f, 1.59f);
	Exodar (530, -3965f, -11654.6f, -138.9f, 0.85f);
	Ironforge (0, -4918f, -940f, 501f, 5.42f);
	Stormwind (0, -8833f, 622f, 94f, 2.44f);

	HORDE
	Shrine of the two moons (870, 1677f, 931f, 471f, 3.27f);
	Orgrimmar (1, 1569f, -4397f, 16f, 0.54f);
	Silvermoon (530, 9487f, -7279f, 14.5f, 6,16f);
	Thunder Bluff (1, -1277f, 124f, 131f, 6,16f);
	Undercity (0, 1580f, 256f, -61f, 5,39f);

	GLOBAL:
	Dalaran (571, 5805f, 626f, 647f, 2f);
	Shattrath (530, -1842f, 5388f, -12f, 2f);
 * 
 * 
 * 
 * /




/*######
## npc_teleport
######*/

struct npc_teleport : public ScriptedAI
{
    npc_teleport(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        switch (action)
        {
            // Shrine of the Seven Stars
	        case GOSSIP_ACTION_INFO_DEF + 1:
				player->TeleportTo(870, 907.901f, 336.61f, 506.095f, 3.92009f);
	            break;
	        // Darnassus
	        case GOSSIP_ACTION_INFO_DEF + 2:
	            player->TeleportTo(1, 9949.56f, 2284.21f, 1341.4f, 1.59587f);
	            break;
			// Exodar
	        case GOSSIP_ACTION_INFO_DEF + 3:
	            player->TeleportTo(530, -3965.0f, -11654.6f, -138.9f, 0.85f);
	            break;
	        // Ironforge
	        case GOSSIP_ACTION_INFO_DEF + 4:
	            player->TeleportTo(0, -4918.88f, -940.406f, 501.564f, 5.42347f);
	            break;
	        // Stormwind
	        case GOSSIP_ACTION_INFO_DEF + 5:
	            player->TeleportTo(0, -8833.07f, 622.778f, 93.9317f, 0.6771f);
				break;
	        // Shrine of the two moons
	        case GOSSIP_ACTION_INFO_DEF + 6:
				player->TeleportTo(870, 1570.2f, 894.047f, 473.6f, 0.48004f);
	            break;
	        // Orgrimmar
	        case GOSSIP_ACTION_INFO_DEF + 7:
				player->TeleportTo(1, 1569.97f, -4397.41f, 16.0472f, 0.543025f);
	            break;
	        // Silvermoon
	        case GOSSIP_ACTION_INFO_DEF + 8:
				player->TeleportTo(530, 9487.69f, -7279.2f, 14.2866f, 6.16478f);
	            break;
	        // Thunder Bluff
	        case GOSSIP_ACTION_INFO_DEF + 9:
				player->TeleportTo(1, -1277.37f, 124.804f, 131.287f, 5.22274f);
	            break;
	        // Undercity
	        case GOSSIP_ACTION_INFO_DEF + 10:
				player->TeleportTo(0, 1584.07f, 241.987f, -52.1534f, 0.049647f);
	            break;	  
	        // Dalaran
	        case GOSSIP_ACTION_INFO_DEF + 11:
				player->TeleportTo(571, 5804.15f, 624.771f, 647.767f, 1.64f);
	            break;
	        // Shattrath
	        case GOSSIP_ACTION_INFO_DEF + 12:
				player->TeleportTo(530, -1838.16f, 5301.79f, -12.428f, 5.9517f);
	            break;	                      
        }
        CloseGossipMenuFor(player);
        return true;
    }

    bool OnGossipHello(Player* player) override
    {
        InitGossipMenuFor(player, GOSSIP_MENU_NPCTELEPORT);
        if (player->IsInCombat())
        {
            player->GetSession()->SendNotification("You are in combat!");
            CloseGossipMenuFor(player);
        }
		if (player->GetTeam() == ALLIANCE)
		{
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_ALLIANCE_SHRINE_OF_THE_SEVEN_STARS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_ALLIANCE_DARNASSUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_ALLIANCE_EXODAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_ALLIANCE_IRONFORGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_ALLIANCE_STORMWIND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
		}
		if (player->GetTeam() == HORDE)
		{
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_HORDE_SHRINE_OF_THE_TWO_MOONS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_HORDE_ORGRIMMAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_HORDE_SLIVERMOON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_HORDE_THUNDERBLUFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
			AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_HORDE_UNDERCITY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
		}
		AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_GLOBAL_DALARAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
        AddGossipItemFor(player, GOSSIP_MENU_NPCTELEPORT, GOSSIP_OPTION_GLOBAL_SHATTRATH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);

        SendGossipMenuFor(player, 48, me->GetGUID());
        return true;
    }
};
 
void AddSC_npc_teleport()
{
    RegisterCreatureAI(npc_teleport);
}

