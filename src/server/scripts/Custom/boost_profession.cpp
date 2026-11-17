#include "ScriptPCH.h"
#include "Chat.h"
#include "ServiceMgr.h"

enum skillsId
{
    FIRST_AID       = 129, // first Aid
    BLACKSMITHING   = 164, // Blacksmithing
    LEATHERWORKING  = 165, // Leatherworking
    ALCHEMY         = 171, // Alchemy
    HERBALISM       = 182, // Herbalism
    COOKING         = 185, // Cooking
    MINING          = 186, // Mining
    TAILORING       = 197, // Tailoring
    ENGINEERING     = 202, // Engineering
    ENCHANTING      = 333, // Enchanting
    FISHING         = 356, // Fishing
    SKINNING        = 393, // Skinning
    JEWELCRAFTING   = 755, // Jewelcrafting
    INSCRIPTION     = 773, // Inscription
    ARCHAEOLOGY     = 794  // Archaeology
}; 

enum spellId_600
{
    SPELL_600_FIRST_AID       = 110406, // first Aid
    SPELL_600_BLACKSMITHING   = 110396, // Blacksmithing
    SPELL_600_LEATHERWORKING  = 110423, // Leatherworking
    SPELL_600_ALCHEMY         = 105206, // Alchemy
    SPELL_600_HERBALISM       = 110413, // Herbalism
    SPELL_600_COOKING         = 104381, // Cooking
    SPELL_600_MINING          = 102161, // Mining
    SPELL_600_TAILORING       = 110426, // Tailoring
    SPELL_600_ENGINEERING     = 110403, // Engineering
    SPELL_600_ENCHANTING      = 110400, // Enchanting
    SPELL_600_FISHING         = 110410, // Fishing
    SPELL_600_SKINNING        = 102216, // Skinning
    SPELL_600_JEWELCRAFTING   = 110420, // Jewelcrafting
    SPELL_600_INSCRIPTION     = 110417, // Inscription
    SPELL_600_ARCHAEOLOGY     = 110393  // Archaeology
};  

class Boost_Profession : public ItemScript
{
public:
    Boost_Profession() : ItemScript("Boost_Profession") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        player->PlayerTalkClass->ClearMenus();        
        player->SaveToDB();
        
        if(player->HasSkill(FIRST_AID))
            player->ADD_GOSSIP_ITEM_DB(51002, 0, GOSSIP_SENDER_MAIN, FIRST_AID);
        if(player->HasSkill(BLACKSMITHING))
            player->ADD_GOSSIP_ITEM_DB(51002, 1, GOSSIP_SENDER_MAIN, BLACKSMITHING);
        if(player->HasSkill(LEATHERWORKING))
            player->ADD_GOSSIP_ITEM_DB(51002, 2, GOSSIP_SENDER_MAIN, LEATHERWORKING);
        if(player->HasSkill(ALCHEMY))
            player->ADD_GOSSIP_ITEM_DB(51002, 3, GOSSIP_SENDER_MAIN, ALCHEMY);
        if(player->HasSkill(HERBALISM))
            player->ADD_GOSSIP_ITEM_DB(51002, 4, GOSSIP_SENDER_MAIN, HERBALISM);
        if(player->HasSkill(COOKING))
            player->ADD_GOSSIP_ITEM_DB(51002, 5, GOSSIP_SENDER_MAIN, COOKING);
        if(player->HasSkill(MINING))
            player->ADD_GOSSIP_ITEM_DB(51002, 6, GOSSIP_SENDER_MAIN, MINING);
        if(player->HasSkill(TAILORING))
            player->ADD_GOSSIP_ITEM_DB(51002, 7, GOSSIP_SENDER_MAIN, TAILORING);
        if(player->HasSkill(ENGINEERING))
            player->ADD_GOSSIP_ITEM_DB(51002, 8, GOSSIP_SENDER_MAIN, ENGINEERING);
        if(player->HasSkill(ENCHANTING))
            player->ADD_GOSSIP_ITEM_DB(51002, 9, GOSSIP_SENDER_MAIN, ENCHANTING);
        if(player->HasSkill(FISHING))
            player->ADD_GOSSIP_ITEM_DB(51002, 10, GOSSIP_SENDER_MAIN, FISHING);
        if(player->HasSkill(SKINNING))
            player->ADD_GOSSIP_ITEM_DB(51002, 11, GOSSIP_SENDER_MAIN, SKINNING);
        if(player->HasSkill(JEWELCRAFTING))
            player->ADD_GOSSIP_ITEM_DB(51002, 12, GOSSIP_SENDER_MAIN, JEWELCRAFTING);
        if(player->HasSkill(INSCRIPTION))
            player->ADD_GOSSIP_ITEM_DB(51002, 13, GOSSIP_SENDER_MAIN, INSCRIPTION);
        if(player->HasSkill(ARCHAEOLOGY))
            player->ADD_GOSSIP_ITEM_DB(51002, 14, GOSSIP_SENDER_MAIN, ARCHAEOLOGY);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bye", GOSSIP_SENDER_MAIN, 1);

        player->SEND_GOSSIP_MENU(20010, item->GetGUID());
        return true;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        uint32 confirm = 0;
        
        switch (action)
        {
        case 1:
            player->CLOSE_GOSSIP_MENU();
            break;
        case FIRST_AID:
            if (!player->HasSpell(SPELL_600_FIRST_AID))
            {
                player->LearnSpell(SPELL_600_FIRST_AID, false);
                confirm = 1;
            }
            break;
        case BLACKSMITHING:
            if (!player->HasSpell(SPELL_600_BLACKSMITHING))
            {
                player->LearnSpell(SPELL_600_BLACKSMITHING, false);
                confirm = 1;
            }
            break;
        case LEATHERWORKING:
            if (!player->HasSpell(SPELL_600_LEATHERWORKING))
            {
                player->LearnSpell(SPELL_600_LEATHERWORKING, false);
                confirm = 1;
            }
            break;
        case ALCHEMY:
            if (!player->HasSpell(SPELL_600_ALCHEMY))
            {
                player->LearnSpell(SPELL_600_ALCHEMY, false);
                confirm = 1;
            }
            break;
        case HERBALISM:
            if (!player->HasSpell(SPELL_600_HERBALISM))
            {
                player->LearnSpell(SPELL_600_HERBALISM, false);
                confirm = 1;
            }
            break;
        case COOKING:
            if (!player->HasSpell(SPELL_600_COOKING))
            {
                player->LearnSpell(SPELL_600_COOKING, false);
                confirm = 1;
            }
            break;
        case MINING:
            if (!player->HasSpell(SPELL_600_MINING))
            {
                player->LearnSpell(SPELL_600_MINING, false);
                confirm = 1;
            }
            break;
        case TAILORING:
            if (!player->HasSpell(SPELL_600_TAILORING))
            {
                player->LearnSpell(SPELL_600_TAILORING, false);
                confirm = 1;
            }
            break;
        case ENGINEERING:
            if (!player->HasSpell(SPELL_600_ENGINEERING))
            {
                player->LearnSpell(SPELL_600_ENGINEERING, false);
                confirm = 1;
            }
            break;
		case ENCHANTING:
			if (!player->HasSpell(SPELL_600_ENCHANTING))
			{
				player->LearnSpell(SPELL_600_ENCHANTING, false);
				confirm = 1;
			}
			break;
        case FISHING:
            if (!player->HasSpell(SPELL_600_FISHING))
            {
                player->LearnSpell(SPELL_600_FISHING, false);
                confirm = 2;
            }
            break;
        case SKINNING:
            if (!player->HasSpell(SPELL_600_SKINNING))
            {
                player->LearnSpell(SPELL_600_SKINNING, false);
                confirm = 1;
            }
            break;
        case JEWELCRAFTING:
            if (!player->HasSpell(SPELL_600_JEWELCRAFTING))
            {
                player->LearnSpell(SPELL_600_JEWELCRAFTING, false);
                confirm = 1;
            }
            break;
        case INSCRIPTION:
            if (!player->HasSpell(SPELL_600_INSCRIPTION))
            {
                player->LearnSpell(SPELL_600_INSCRIPTION, false);
                confirm = 1;
            }
            break;
        case ARCHAEOLOGY:
            if (!player->HasSpell(SPELL_600_ARCHAEOLOGY))
            {
                player->LearnSpell(SPELL_600_ARCHAEOLOGY, false);
                confirm = 1;
            }
            break;
        }

        if(confirm == 1)
        {
            int32 maxvalueprof = player->GetMaxSkillValue(action);
            if(player->HasSkill(action) && player->GetSkillValue(action) < maxvalueprof)
                player->SetSkill(action, player->GetSkillStep(action), maxvalueprof, maxvalueprof);
            
            ChatHandler(player->GetSession()).PSendSysMessage("Thanks for supporting the project!");
            player->DestroyItemCount(item->GetEntry(), 1, true); //Item is destroyed on useage.

            std::ostringstream infoSkill;
            infoSkill << "Skill: " << uint32(action) << " Value: " << uint32(maxvalueprof);
            sServiceMgr->ExecutedServices(player->GetGUID(), SERVICE_TYPE_BOOST_PROFESSION, std::string("Boosted name: ") + player->GetName(), infoSkill.str());
            player->CLOSE_GOSSIP_MENU();
        }
        else if(confirm == 2)
        {
            player->SetSkill(action, player->GetSkillStep(action), 600, 600);
            
            ChatHandler(player->GetSession()).PSendSysMessage("Thanks for supporting the project!");
            player->DestroyItemCount(item->GetEntry(), 1, true); //Item is destroyed on useage.

            std::ostringstream infoSkill;
            infoSkill << "Skill: " << uint32(action) << " Value: 600";
            sServiceMgr->ExecutedServices(player->GetGUID(), SERVICE_TYPE_BOOST_PROFESSION, std::string("Boosted name: ") + player->GetName(), infoSkill.str());
            player->CLOSE_GOSSIP_MENU();
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("You don't have that profession");
        }
        player->SaveToDB();
    }
};

class Boost_Profession_Small : public ItemScript
{
public:
	Boost_Profession_Small() : ItemScript("Boost_Profession_Small") {}

    bool OnUse(Player* player, Item* item, SpellCastTargets const& targets) override
    {
        player->PlayerTalkClass->ClearMenus();        
        player->SaveToDB();
        
        if(player->HasSkill(FIRST_AID))
            player->ADD_GOSSIP_ITEM_DB(51002, 0, GOSSIP_SENDER_MAIN, FIRST_AID);
        if(player->HasSkill(BLACKSMITHING))
            player->ADD_GOSSIP_ITEM_DB(51002, 1, GOSSIP_SENDER_MAIN, BLACKSMITHING);
        if(player->HasSkill(LEATHERWORKING))
            player->ADD_GOSSIP_ITEM_DB(51002, 2, GOSSIP_SENDER_MAIN, LEATHERWORKING);
        if(player->HasSkill(ALCHEMY))
            player->ADD_GOSSIP_ITEM_DB(51002, 3, GOSSIP_SENDER_MAIN, ALCHEMY);
        if(player->HasSkill(HERBALISM))
            player->ADD_GOSSIP_ITEM_DB(51002, 4, GOSSIP_SENDER_MAIN, HERBALISM);
        if(player->HasSkill(COOKING))
            player->ADD_GOSSIP_ITEM_DB(51002, 5, GOSSIP_SENDER_MAIN, COOKING);
        if(player->HasSkill(MINING))
            player->ADD_GOSSIP_ITEM_DB(51002, 6, GOSSIP_SENDER_MAIN, MINING);
        if(player->HasSkill(TAILORING))
            player->ADD_GOSSIP_ITEM_DB(51002, 7, GOSSIP_SENDER_MAIN, TAILORING);
        if(player->HasSkill(ENGINEERING))
            player->ADD_GOSSIP_ITEM_DB(51002, 8, GOSSIP_SENDER_MAIN, ENGINEERING);
        if(player->HasSkill(ENCHANTING))
            player->ADD_GOSSIP_ITEM_DB(51002, 9, GOSSIP_SENDER_MAIN, ENCHANTING);
        if(player->HasSkill(FISHING))
            player->ADD_GOSSIP_ITEM_DB(51002, 10, GOSSIP_SENDER_MAIN, FISHING);
        if(player->HasSkill(SKINNING))
            player->ADD_GOSSIP_ITEM_DB(51002, 11, GOSSIP_SENDER_MAIN, SKINNING);
        if(player->HasSkill(JEWELCRAFTING))
            player->ADD_GOSSIP_ITEM_DB(51002, 12, GOSSIP_SENDER_MAIN, JEWELCRAFTING);
        if(player->HasSkill(INSCRIPTION))
            player->ADD_GOSSIP_ITEM_DB(51002, 13, GOSSIP_SENDER_MAIN, INSCRIPTION);
        if(player->HasSkill(ARCHAEOLOGY))
            player->ADD_GOSSIP_ITEM_DB(51002, 14, GOSSIP_SENDER_MAIN, ARCHAEOLOGY);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Bye", GOSSIP_SENDER_MAIN, 1);

        player->SEND_GOSSIP_MENU(20011, item->GetGUID());
        return true;
    }

    void OnGossipSelect(Player* player, Item* item, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();
        
        if(player->HasSkill(action))
        {
            int32 maxvalueprof = player->GetMaxSkillValue(action);

            if(player->HasSkill(action) && player->GetSkillValue(action) < maxvalueprof)
                player->SetSkill(action, player->GetSkillStep(action), maxvalueprof, maxvalueprof);
            
            ChatHandler(player->GetSession()).PSendSysMessage("Thanks for supporting the project!");
            player->DestroyItemCount(item->GetEntry(), 1, true); //Item is destroyed on useage.

            std::ostringstream infoSkill;
            infoSkill << "Skill: " << uint32(action) << " Value: " << uint32(maxvalueprof);
            sServiceMgr->ExecutedServices(player->GetGUID(), SERVICE_TYPE_BOOST_PROFESSION_SMALL, std::string("Boosted name: ") + player->GetName(), infoSkill.str());
            player->SaveToDB();
        }
        else
        {
            ChatHandler(player->GetSession()).PSendSysMessage("You don't have that profession");
        }
        
        player->CLOSE_GOSSIP_MENU();
    }
};

void AddSC_Boost_Profession()
{
    new Boost_Profession();
    new Boost_Profession_Small();
}