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

#include "QuestDef.h"
#include "GossipDef.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Formulas.h"

GossipMenu::GossipMenu()
{
    _menuId = 0;
    _senderGUID.Clear(); 
}

GossipMenu::~GossipMenu()
{
    ClearMenu();
}

uint32 GossipMenu::AddMenuItem(int32 menuItemId, uint8 icon, std::string const& message, uint32 sender, uint32 action, std::string const& boxMessage, uint32 boxMoney, bool coded /*= false*/)
{
    ASSERT(_menuItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    // Find a free new id - script case
    if (menuItemId == -1)
    {
        menuItemId = 0;
        if (!_menuItems.empty())
        {
            for (GossipMenuItemContainer::const_iterator itr = _menuItems.begin(); itr != _menuItems.end(); ++itr)
            {
                if (int32(itr->first) > menuItemId)
                    break;

                menuItemId = itr->first + 1;
            }
        }
    }

    GossipMenuItem& menuItem = _menuItems[menuItemId];

    menuItem.MenuItemIcon    = icon;
    menuItem.Message         = message;
    menuItem.IsCoded         = coded;
    menuItem.Sender          = sender;
    menuItem.OptionType      = action;
    menuItem.BoxMessage      = boxMessage;
    menuItem.BoxMoney        = boxMoney;
    return menuItemId;
}

/**
 * @name AddMenuItem
 * @brief Adds a localized gossip menu item from db by menu id and menu item id.
 * @param menuId Gossip menu id.
 * @param menuItemId Gossip menu item id.
 * @param sender Identifier of the current menu.
 * @param action Custom action given to OnGossipHello.
 */
void GossipMenu::AddMenuItem(uint32 menuId, uint32 menuItemId, uint32 sender, uint32 action)
{
    /// Find items for given menu id.
    GossipMenuItemsMapBounds bounds = sObjectMgr->GetGossipMenuItemsMapBounds(menuId);
    auto itr = std::find_if(bounds.first, bounds.second, [menuItemId](std::pair<uint32 const, GossipMenuItems> const& itemPair)
    {
        return itemPair.second.OptionID == menuItemId;
    });

    if (itr == bounds.second)
        return;

    /// Store texts for localization.
    std::string strOptionText, strBoxText;
    BroadcastText const* optionBroadcastText = sObjectMgr->GetBroadcastText(itr->second.OptionBroadcastTextID);
    BroadcastText const* boxBroadcastText = sObjectMgr->GetBroadcastText(itr->second.BoxBroadcastTextID);

    /// OptionText
    if (optionBroadcastText)
        strOptionText = optionBroadcastText->GetText(GetLocale());
    else
        strOptionText = itr->second.OptionText;

    /// BoxText
    if (boxBroadcastText)
        strBoxText = boxBroadcastText->GetText(GetLocale());
    else
        strBoxText = itr->second.BoxText;

    /// Check need of localization.
    if (GetLocale() != DEFAULT_LOCALE)
    {
        if (!optionBroadcastText)
        {
            /// Find localizations from database.
            if (GossipMenuItemsLocale const* gossipMenuLocale = sObjectMgr->GetGossipMenuItemsLocale(menuId, menuItemId))
                ObjectMgr::GetLocaleString(gossipMenuLocale->OptionText, GetLocale(), strOptionText);
        }

        if (!boxBroadcastText)
        {
            /// Find localizations from database.
            if (GossipMenuItemsLocale const* gossipMenuLocale = sObjectMgr->GetGossipMenuItemsLocale(menuId, menuItemId))
                ObjectMgr::GetLocaleString(gossipMenuLocale->BoxText, GetLocale(), strBoxText);
        }
    }

    /// Add menu item with existing method. Menu item id -1 is also used in ADD_GOSSIP_ITEM macro.
    AddMenuItem(itr->second.OptionID, itr->second.OptionIcon, strOptionText, sender, action, strBoxText, itr->second.BoxMoney, itr->second.BoxCoded);
    AddGossipMenuItemData(itr->second.OptionID, itr->second.ActionMenuID, itr->second.ActionPoiID);

}

void GossipMenu::AddGossipMenuItemData(uint32 menuItemId, uint32 gossipActionMenuId, uint32 gossipActionPoi)
{
    GossipMenuItemData& itemData = _menuItemData[menuItemId];

    itemData.GossipActionMenuId  = gossipActionMenuId;
    itemData.GossipActionPoi     = gossipActionPoi;
}

uint32 GossipMenu::GetMenuItemSender(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return 0;

    return itr->second.Sender;
}

uint32 GossipMenu::GetMenuItemAction(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return 0;

    return itr->second.OptionType;
}

bool GossipMenu::IsMenuItemCoded(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return false;

    return itr->second.IsCoded;
}

void GossipMenu::ClearMenu()
{
    _menuItems.clear();
    _menuItemData.clear();
}

PlayerMenu::PlayerMenu(WorldSession* session) : _session(session)
{
    if (_session)
        _gossipMenu.SetLocale(_session->GetSessionDbLocaleIndex());
}

PlayerMenu::~PlayerMenu()
{
    ClearMenus();
}

void PlayerMenu::ClearMenus()
{
    _gossipMenu.ClearMenu();
    _questMenu.ClearMenu();
}

void PlayerMenu::SendGossipMenu(uint32 titleTextId, ObjectGuid objectGUID) const
{
    std::vector<std::string> updatedQuestTitles;
    updatedQuestTitles.reserve(_questMenu.GetMenuItemCount());

    //ObjectGuid guid = ObjectGuid(objectGUID);
    ObjectGuid guid = objectGUID;

    WorldPacket data(SMSG_GOSSIP_MESSAGE, 150);             // Guessed

    data.WriteBits(_questMenu.GetMenuItemCount(), 19);      // max count 0x20

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (uint8 i = 0; i < _questMenu.GetMenuItemCount(); ++i)
    {
        uint32 questId = _questMenu.GetItem(i).QuestId;
        Quest const* quest = sObjectMgr->GetQuestTemplate(questId);

        std::string title = quest->GetLogTitle();

        LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
        if (localeConstant != LOCALE_enUS)
            if (QuestTemplateLocale const* localeData = sObjectMgr->GetQuestLocale(questId))
                ObjectMgr::GetLocaleString(localeData->LogTitle, localeConstant, title);

        if (questLevelInTitle)
            AddQuestLevelToTitle(title, quest->GetQuestLevel());

        data.WriteBit(quest->IsRepeatable());               // 3.3.3 changes icon: (0) yellow exclamation or (1) blue question
        data.WriteBits(title.length(), 9);

        updatedQuestTitles.push_back(title);
    }

    data.WriteBit(guid[5]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[0]);
    data.WriteBits(_gossipMenu.GetMenuItemCount(), 20);     // max count 0x10
    data.WriteBit(guid[6]);
    data.WriteBit(guid[2]);

    for (GossipMenuItemContainer::const_iterator itr = _gossipMenu.GetMenuItems().begin(); itr != _gossipMenu.GetMenuItems().end(); ++itr)
    {
        GossipMenuItem const& item = itr->second;

        data.WriteBits(item.BoxMessage.length(), 12);
        data.WriteBits(item.Message.length(), 12);
    }

    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.FlushBits();

    for (uint8 i = 0; i < _questMenu.GetMenuItemCount(); ++i)
    {
        QuestMenuItem const& item = _questMenu.GetItem(i);
        Quest const* quest = sObjectMgr->GetQuestTemplate(item.QuestId);

        data.WriteString(updatedQuestTitles[i]);
        data << int32(quest->GetFlags());
        data << int32(quest->GetQuestLevel());
        data << int32(item.QuestIcon);
        data << int32(item.QuestId);
        data << int32(quest->GetFlagsEx());
    }

    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[0]);

    for (GossipMenuItemContainer::const_iterator itr = _gossipMenu.GetMenuItems().begin(); itr != _gossipMenu.GetMenuItems().end(); ++itr)
    {
        GossipMenuItem const& item = itr->second;

        data << int32(item.BoxMoney);                       // money required to open menu, 2.0.3
        data.WriteString(item.BoxMessage);                  // accept text (related to money) pop up box, 2.0.
        data << int32(itr->first);
        data << int8(item.IsCoded);                         // makes pop up box password
        data.WriteString(item.Message);                     // text for gossip item
        data << int8(item.MenuItemIcon);
    }

    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[3]);
    data << int32(_gossipMenu.GetMenuId());                 // new 2.4.0
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[4]);
    data << int32(0);                                       // friend faction ID?
    data.WriteByteSeq(guid[7]);
    data << int32(titleTextId);

    _session->SendPacket(&data);
}

void PlayerMenu::SendCloseGossip() const
{
    //_gossipMenu.SetSenderGUID(ObjectGuid::Empty);
    WorldPacket data(SMSG_GOSSIP_COMPLETE, 0);
    _session->SendPacket(&data);
}

void PlayerMenu::SendPointOfInterest(uint32 poiId) const
{
    PointOfInterest const* poi = sObjectMgr->GetPointOfInterest(poiId);
    if (!poi)
    {
        TC_LOG_ERROR("sql.sql", "Request to send non-existing POI (Id: %u), ignored.", poiId);
        return;
    }

    std::string name = poi->Name;
    LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
    if (localeConstant != LOCALE_enUS)
        if (PointOfInterestLocale const* localeData = sObjectMgr->GetPointOfInterestLocale(poiId))
            ObjectMgr::GetLocaleString(localeData->Name, localeConstant, name);

    WorldPacket data(SMSG_GOSSIP_POI, 4 + 4 + 4 + 4 + 4 + 10);  // guess size
    data << uint32(poi->Flags);
    data << float(poi->PositionX);
    data << float(poi->PositionY);
    data << uint32(poi->Icon);
    data << uint32(poi->Importance);
    data << name;

    _session->SendPacket(&data);
}

/*********************************************************/
/***                    QUEST SYSTEM                   ***/
/*********************************************************/

QuestMenu::QuestMenu()
{
    _questMenuItems.reserve(16);                                   // can be set for max from most often sizes to speedup push_back and less memory use
}

QuestMenu::~QuestMenu()
{
    ClearMenu();
}

void QuestMenu::AddMenuItem(uint32 QuestId, uint8 Icon)
{
    if (!sObjectMgr->GetQuestTemplate(QuestId))
        return;

    ASSERT(_questMenuItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    QuestMenuItem questMenuItem;

    questMenuItem.QuestId        = QuestId;
    questMenuItem.QuestIcon      = Icon;

    _questMenuItems.push_back(questMenuItem);
}

bool QuestMenu::HasItem(uint32 questId) const
{
    for (QuestMenuItemList::const_iterator i = _questMenuItems.begin(); i != _questMenuItems.end(); ++i)
        if (i->QuestId == questId)
            return true;

    return false;
}

void QuestMenu::ClearMenu()
{
    _questMenuItems.clear();
}

void PlayerMenu::SendQuestGiverQuestList(QEmote eEmote, const std::string& Title, ObjectGuid npcGUID)
{
    ByteBuffer questData;

    WorldPacket data(SMSG_QUESTGIVER_QUEST_LIST, 100);      // guess size
    data << uint32(eEmote._Emote);                          // NPC emote
    data << uint32(eEmote._Delay);                          // player emote

    data.WriteBit(npcGUID[2]);
    data.WriteBits(Title.size(), 11);
    data.WriteBit(npcGUID[6]);
    data.WriteBit(npcGUID[0]);

    uint32 count = 0;
    size_t countPos = data.bitwpos();
    data.WriteBits(count, 19);

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (uint32 i = 0; i < _questMenu.GetMenuItemCount(); ++i)
    {
        QuestMenuItem const& qmi = _questMenu.GetItem(i);

        uint32 questID = qmi.QuestId;

        if (Quest const* quest = sObjectMgr->GetQuestTemplate(questID))
        {
            ++count;
            std::string title = quest->GetLogTitle();

            LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
            if (localeConstant != LOCALE_enUS)
                if (QuestTemplateLocale const* localeData = sObjectMgr->GetQuestLocale(questID))
                    ObjectMgr::GetLocaleString(localeData->LogTitle, localeConstant, title);

            if (questLevelInTitle)
                AddQuestLevelToTitle(title, quest->GetQuestLevel());

            data.WriteBit(quest->IsRepeatable());           // 3.3.3 changes icon: (0) yellow exclamation or (1) blue question
            data.WriteBits(title.size(), 9);

            questData << uint32(quest->GetFlags());
            questData << uint32(questID);
            questData.WriteString(title);
            questData << uint32(quest->GetFlagsEx());
            questData << uint32(qmi.QuestIcon);
            questData << uint32(quest->GetQuestLevel());
        }
    }

    data.WriteBit(npcGUID[1]);
    data.WriteBit(npcGUID[3]);
    data.WriteBit(npcGUID[4]);
    data.WriteBit(npcGUID[5]);
    data.WriteBit(npcGUID[7]);
    data.PutBits(countPos, count, 19);
    data.FlushBits();

    data.WriteByteSeq(npcGUID[1]);
    data.WriteByteSeq(npcGUID[0]);
    data.WriteByteSeq(npcGUID[6]);
    data.WriteByteSeq(npcGUID[7]);
    data.append(questData);
    data.WriteByteSeq(npcGUID[5]);
    data.WriteByteSeq(npcGUID[3]);
    data.WriteByteSeq(npcGUID[2]);
    data.WriteString(Title);
    data.WriteByteSeq(npcGUID[4]);

    _session->SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_QUESTGIVER_QUEST_LIST NPC Guid=%u", npcGUID.GetCounter());
}

void PlayerMenu::SendQuestGiverStatus(QuestGiverStatus questStatus, ObjectGuid npcGUID) const
{
    WorldPacket data(SMSG_QUESTGIVER_STATUS, 1 + 8 + 4);
    data.WriteBit(npcGUID[1]);
    data.WriteBit(npcGUID[7]);
    data.WriteBit(npcGUID[4]);
    data.WriteBit(npcGUID[2]);
    data.WriteBit(npcGUID[5]);
    data.WriteBit(npcGUID[3]);
    data.WriteBit(npcGUID[6]);
    data.WriteBit(npcGUID[0]);

    data.WriteByteSeq(npcGUID[7]);
    data << uint32(questStatus);
    data.WriteByteSeq(npcGUID[4]);
    data.WriteByteSeq(npcGUID[6]);
    data.WriteByteSeq(npcGUID[1]);
    data.WriteByteSeq(npcGUID[5]);
    data.WriteByteSeq(npcGUID[2]);
    data.WriteByteSeq(npcGUID[0]);
    data.WriteByteSeq(npcGUID[3]);

    _session->SendPacket(&data);
    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_QUESTGIVER_STATUS NPC Guid=%u, status=%u", npcGUID.GetCounter(), questStatus);
}

void PlayerMenu::SendQuestGiverQuestDetails(Quest const* quest, ObjectGuid npcGUID, bool activateAccept, bool startedByAreaTrigger) const
{
    std::string logTitle             = quest->GetLogTitle();
    std::string logDescription       = quest->GetLogDescription();
    std::string questDescription     = quest->GetQuestDescription();
    std::string portraitGiverText    = quest->GetPortraitGiverText();
    std::string portraitGiverName    = quest->GetPortraitGiverName();
    std::string portraitTurnInText   = quest->GetPortraitTurnInText();
    std::string portraitTurnInName   = quest->GetPortraitTurnInName();

    LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
    if (localeConstant != LOCALE_enUS)
    {
        if (QuestTemplateLocale const* localeData = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(localeData->LogTitle, localeConstant, logTitle);
            ObjectMgr::GetLocaleString(localeData->LogDescription, localeConstant, logDescription);
            ObjectMgr::GetLocaleString(localeData->QuestDescription, localeConstant, questDescription);
            ObjectMgr::GetLocaleString(localeData->PortraitGiverText, localeConstant, portraitGiverText);
            ObjectMgr::GetLocaleString(localeData->PortraitGiverName, localeConstant, portraitGiverName);
            ObjectMgr::GetLocaleString(localeData->PortraitTurnInText, localeConstant, portraitTurnInText);
            ObjectMgr::GetLocaleString(localeData->PortraitTurnInName, localeConstant, portraitTurnInName);
        }
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(logTitle, quest->GetQuestLevel());

    ByteBuffer objData;
    for (const auto& objective : quest->Objectives)
    {
        objData << uint8(objective.Type);
        objData << uint32(objective.ID);
        objData << int32(objective.Amount);
        objData << uint32(objective.ObjectID);
    }

    uint32 rewItemDisplayId[QUEST_REWARD_ITEM_COUNT];
    uint32 rewChoiceItemDisplayId[QUEST_REWARD_CHOICES_COUNT];

    std::vector<std::pair<uint32 /*itemId*/, uint32 /*count*/>> rewInfo;
    for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; i++)
    {
        uint32 id = quest->RewardItemId[i];
        uint32 count = quest->RewardItemCount[i];

        if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(id))
            rewItemDisplayId[i] = itemTemplate->DisplayInfoID;
        else
            rewItemDisplayId[i] = 0;

        rewInfo.push_back(std::make_pair(id, count));
    }

    for (uint8 i = 0; i < QUEST_REWARD_CHOICES_COUNT; i++)
    {
        if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[i]))
            rewChoiceItemDisplayId[i] = itemTemplate->DisplayInfoID;
        else
            rewChoiceItemDisplayId[i] = 0;
    }

    ObjectGuid guid2 = npcGUID;
    ObjectGuid guid = _session->GetPlayer()->GetDivider();

    WorldPacket data(SMSG_QUESTGIVER_QUEST_DETAILS, 100);   // guess size
    data << uint32(rewInfo[3].second);
    data << uint32(rewChoiceItemDisplayId[4]);
    data << uint32(quest->RewardChoiceItemId[2]);

    for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; i++)
    {
        data << uint32(quest->RewardCurrencyCount[i]);
        data << uint32(quest->RewardCurrencyId[i]);
    }

    data << uint32(quest->GetRewChoiceItemsCount());
    data << uint32(quest->RewardChoiceItemCount[2]);
    data << uint32(rewInfo[1].second);
    data << uint32(rewChoiceItemDisplayId[5]);
    data << uint32(rewInfo[0].second);
    data << uint32(rewItemDisplayId[3]);
    data << uint32(quest->RewardChoiceItemId[0]);
    data << uint32(quest->RewardChoiceItemCount[3]);
    data << uint32(quest->GetQuestGiverPortrait());
    data << uint32(rewChoiceItemDisplayId[3]);
    data << uint32(rewInfo[0].first);
    data << uint32(quest->GetQuestId());
    data << uint32(quest->GetSuggestedPlayers());
    data << uint32(rewChoiceItemDisplayId[0]);
    data << uint32(quest->RewardChoiceItemCount[4]);
    data << uint32(quest->RewardChoiceItemCount[5]);
    data << uint32(0); // quest->GetBonusTalents()
    data << uint32(quest->RewardChoiceItemCount[1]);
    data << uint32(rewChoiceItemDisplayId[2]);

    for (uint8 i = 0; i < QUEST_REWARD_REPUTATIONS_COUNT; i++)
    {
        data << uint32(quest->RewardFactionId[i]);
        data << uint32(quest->RewardFactionOverride[i]);
        data << uint32(quest->RewardFactionId[i]);
    }

    data << uint32(rewInfo[3].first);
    data << uint32(quest->GetRewardSkillId());
    data << uint32(quest->XPValue(_session->GetPlayer()) * sWorld->getRate(RATE_XP_QUEST));
    data << uint32(quest->GetRewardReputationMask());
    data << uint32(rewItemDisplayId[2]);
    data << uint32(rewInfo[1].first);
    data << uint32(quest->RewardChoiceItemId[1]);
    data << uint32(quest->RewardChoiceItemId[5]);
    data << uint32(quest->GetRewSpell());
    data << uint32(quest->GetFlags());
    data << uint32(quest->GetRewTitle());
    data << uint32(rewInfo[2].first);
    data << uint32(quest->GetRewOrReqMoney(_session->GetPlayer()));
    data << uint32(rewInfo[2].second);
    data << uint32(quest->GetFlagsEx());
    data << uint32(0); // should be quest->GetRewSpellCast(), but need to know in which cases this need sends
    data << uint32(quest->RewardChoiceItemId[3]);
    data << uint32(quest->GetRewItemsCount());
    data << uint32(quest->GetRewardSkillPoints());
    data << uint32(rewItemDisplayId[0]);
    data << uint32(quest->RewardChoiceItemId[4]);
    data << uint32(quest->GetQuestPackageID());
    data << uint32(quest->RewardChoiceItemCount[0]);
    data << uint32(rewItemDisplayId[1]);
    data << uint32(rewChoiceItemDisplayId[1]);
    data << uint32(quest->GetQuestTurnInPortrait());

    data.WriteBit(guid[7]);
    data.WriteBit(guid2[1]);
    data.WriteBits(portraitTurnInName.size(), 8);
    data.WriteBit(guid2[2]);
    data.WriteBits(portraitGiverText.size(), 10);
    data.WriteBit(startedByAreaTrigger);
    data.WriteBit(guid[2]);
    data.WriteBits(logTitle.size(), 9);
    data.WriteBits(QUEST_EMOTE_COUNT, 21);
    data.WriteBit(guid[0]);
    data.WriteBit(guid2[6]);
    data.WriteBit(guid2[5]);
    data.WriteBits(portraitGiverName.size(), 8);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid2[0]);
    data.WriteBit(0);                                       // CGQuestInfo::m_startQuestCheat
    data.WriteBit(guid2[4]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[4]);
    data.WriteBits(portraitTurnInText.size(), 10);
    data.WriteBit(activateAccept);                          // CGQuestInfo::m_autoLaunched
    data.WriteBit(guid[6]);
    data.WriteBit(guid2[7]);
    data.WriteBits(questDescription.size(), 12);
    data.WriteBits(0, 22);                                  // LearnSpellsCount
    data.WriteBits(quest->Objectives.size(), 20);
    data.WriteBits(logDescription.size(), 12);
    data.FlushBits();

    data.WriteByteSeq(guid[0]);
    data.WriteString(portraitGiverName);
    data.WriteString(portraitTurnInText);
    data.WriteString(logTitle);
    data.WriteByteSeq(guid2[6]);
    data.WriteString(logDescription);
    data.WriteByteSeq(guid[2]);
    data.WriteString(portraitGiverText);
    data.append(objData);
    data.WriteString(portraitTurnInName);
    data.WriteString(questDescription);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid2[7]);
    data.WriteByteSeq(guid2[3]);
    data.WriteByteSeq(guid2[0]);

    for (uint8 i = 0; i < QUEST_EMOTE_COUNT; i++)
    {
        data << uint32(quest->DetailsEmoteDelay[i]);        // DetailsEmoteDelay (in ms)
        data << uint32(quest->DetailsEmote[i]);
    }

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid2[5]);
    data.WriteByteSeq(guid2[1]);
    data.WriteByteSeq(guid2[2]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid2[4]);

    /*for (uint i = 0; i < unkCounterBits22; i++)
        data << uint32(0);*/ // LearnSpells

    _session->SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_QUESTGIVER_QUEST_DETAILS NPCGuid=%u, questid=%u", npcGUID.GetCounter(), quest->GetQuestId());
}

void PlayerMenu::SendQuestQueryResponse(Quest const* quest) const
{
    std::string logTitle             = quest->GetLogTitle();
    std::string logDescription       = quest->GetLogDescription();
    std::string questDescription     = quest->GetQuestDescription();
    std::string areaDescription      = quest->GetAreaDescription();
    std::string questCompletionLog   = quest->GetQuestCompletionLog();
    std::string portraitGiverText    = quest->GetPortraitGiverText();
    std::string portraitGiverName    = quest->GetPortraitGiverName();
    std::string portraitTurnInText   = quest->GetPortraitTurnInText();
    std::string portraitTurnInName   = quest->GetPortraitTurnInName();

    LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
    if (localeConstant != LOCALE_enUS)
    {
        if (QuestTemplateLocale const* localeData = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(localeData->LogTitle, localeConstant, logTitle);
            ObjectMgr::GetLocaleString(localeData->LogDescription, localeConstant, logDescription);
            ObjectMgr::GetLocaleString(localeData->QuestDescription, localeConstant, questDescription);
            ObjectMgr::GetLocaleString(localeData->AreaDescription, localeConstant, areaDescription);
            ObjectMgr::GetLocaleString(localeData->QuestCompletionLog, localeConstant, questCompletionLog);
            ObjectMgr::GetLocaleString(localeData->PortraitGiverText, localeConstant, portraitGiverText);
            ObjectMgr::GetLocaleString(localeData->PortraitGiverName, localeConstant, portraitGiverName);
            ObjectMgr::GetLocaleString(localeData->PortraitTurnInText, localeConstant, portraitTurnInText);
            ObjectMgr::GetLocaleString(localeData->PortraitTurnInName, localeConstant, portraitTurnInName);
        }

    }

    std::vector<std::pair<uint32 /*itemId*/, uint32 /*count*/>> rewInfo;
    for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; i++)
    {
        uint32 id = quest->RewardItemId[i];
        uint32 count = quest->RewardItemCount[i];

        rewInfo.push_back(std::make_pair(id, count));
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(logTitle, quest->GetQuestLevel());

    WorldPacket data(SMSG_QUEST_QUERY_RESPONSE, 100);   // guess size
    data << uint32(quest->GetQuestId());

    data.WriteBit(1);                                   // has data
    data.WriteBits(portraitTurnInText.size(), 10);
    data.WriteBits(logTitle.size(), 9);
    data.WriteBits(questCompletionLog.size(), 11);
    data.WriteBits(questDescription.size(), 12);
    data.WriteBits(portraitTurnInName.size(), 8);
    data.WriteBits(portraitGiverName.size(), 8);
    data.WriteBits(portraitGiverText.size(), 10);
    data.WriteBits(areaDescription.size(), 9);
    data.WriteBits(quest->Objectives.size(), 19);
    data.WriteBits(logDescription.size(), 12);

    ByteBuffer objData;
    for (const auto& questObjective : quest->Objectives)
    {
        std::string descriptionText = questObjective.Description;
        if (localeConstant != LOCALE_enUS)
            if (QuestObjectivesLocale const* questObjectiveLocale = sObjectMgr->GetQuestObjectivesLocale(questObjective.ID))
            //if (QuestObjectiveLocale const* questObjectiveLocale = sObjectMgr->GetQuestObjectiveLocale(questObjective->ID))
                ObjectMgr::GetLocaleString(questObjectiveLocale->Description, localeConstant, descriptionText);

        data.WriteBits(descriptionText.size(), 8);
        data.WriteBits(questObjective.VisualEffects.size(), 22);

        objData << int32(questObjective.Amount);
        objData << uint32(questObjective.ID);
        objData.WriteString(descriptionText);
        objData << uint32(questObjective.Flags);
        objData << int8(questObjective.StorageIndex);
        objData << uint8(questObjective.Type);
        objData << uint32(questObjective.ObjectID);

        for (auto citrEffects : questObjective.VisualEffects)
            objData << uint32(citrEffects);
    }

    data.FlushBits();

    bool hiddenReward = quest->HasFlag(QUEST_FLAGS_HIDE_REWARD);

    // values need rechecking and zero values need more research
    data.append(objData);
    data << uint32(quest->ItemDrop[0]);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemId[4]);
    data << uint32(hiddenReward ? 0 : rewInfo[3].first);
    data << uint32(hiddenReward ? 0 : rewInfo[1].second);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemCount[2]);

    for (uint32 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; i++)
    {
        data << uint32(quest->RewardCurrencyId[i]);
        data << uint32(quest->RewardCurrencyCount[i]);
    }

    data << uint32(0);                                                      // quest->GetBonusTalents()
    data << float(quest->GetPOIy());
    data << uint32(quest->GetSoundTurnIn());

    for (int i = 0; i < QUEST_REWARD_REPUTATIONS_COUNT; i++)
    {
        data << uint32(quest->RewardFactionOverride[i]);                    // unknown usage
        data << uint32(quest->RewardFactionId[i]);                          // columnid+1 QuestFactionReward.dbc?
        data << uint32(quest->RewardFactionId[i]);                          // reward factions ids
    }

    data << uint32(hiddenReward ? 0 : quest->GetRewOrReqMoney(_session->GetPlayer()));    // reward money (below max lvl)
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemCount[4]);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemCount[1]);
    data << uint32(quest->GetFlagsEx());
    data.WriteString(areaDescription);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemId[1]);
    data << uint32(quest->GetRewMoneyMaxLevel());                           // used in XP calculation at client
    data << uint32(hiddenReward ? 0 : rewInfo[0].first);
    data.WriteString(questCompletionLog);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemId[3]);
    data << uint32(quest->GetRewHonor());                           // rewarded honor points
    data.WriteString(portraitGiverText);
    data.WriteString(logDescription);
    data << uint32(quest->GetRewardSkillPoints());                          // reward skill points
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemId[5]);
    data << uint32(quest->GetSuggestedPlayers());                           // suggested players count
    data << uint32(quest->GetQuestId());                                    // quest id
    data << uint32(quest->ItemDrop[1]);
    data << uint32(hiddenReward ? 0 : rewInfo[1].first);
    data << int32(quest->GetMinLevel());                                    // min level
    data << uint32(quest->GetRewardReputationMask());                       // rep mask (unsure on what it does)
    data << uint32(quest->GetPOIPriority());
    data << int32(quest->GetQuestLevel());                                  // may be -1, static data, in other cases must be used dynamic level: Player::GetQuestLevel (0 is not known, but assuming this is no longer valid for quest intended for client)
    data << uint32(quest->GetQuestType());                                  // Accepted values: 0, 1 or 2. 0 == IsAutoComplete() (skip objectives/details)
    data << uint32(quest->ItemDropQuantity[2]);
    data << uint32(quest->GetXPDifficulty());                               // seems to always have the same value as the first XP ID field
    data.WriteString(questDescription);
    data << uint32(hiddenReward ? 0 : rewInfo[0].second);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemCount[5]);
    data << uint32(hiddenReward ? 0 : rewInfo[2].second);
    data << uint32(0);                                                      // casted spell (quest->GetRewSpellCast() (see above))
    data << uint32(0);                                                      // unknown
    data.WriteString(portraitTurnInName);
    data << uint32(0);                                                      // unknown
    data << uint32(quest->ItemDropQuantity[1]);
    data << uint32(quest->ItemDrop[2]);
    data << uint32(quest->GetQuestTurnInPortrait());
    data.WriteString(logTitle);
    data << uint32(quest->GetQuestInfoID());                                  // quest type
    data << uint32(quest->GetXPDifficulty());                               // used for calculating rewarded experience
    data << uint32(0);                                                      // unknown
    data << uint32(0);                                                      // unknown
    data << uint32(quest->GetPOIContinent());
    data << uint32(quest->GetNextQuestInChain());                           // client will request this quest from NPC, if not 0
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemId[0]);
    data.WriteString(portraitGiverName);
    data << uint32(0);                                                      // unknown
    data << uint32(quest->ItemDrop[3]);
    data << float(quest->GetPOIx());
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemId[2]);
    data << uint32(0);                                                      // unknown
    data << uint32(hiddenReward ? 0 : rewInfo[3].second);
    data << uint32(quest->GetSoundAccept());
    data << uint32(hiddenReward ? 0 : rewInfo[2].first);
    data << float(quest->GetRewKillHonor());
    data << uint32(quest->GetRewTitle());                                   // CharTitleId, new 2.4.0, player gets this title (id from CharTitles)
    data.WriteString(portraitTurnInText);
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemCount[3]);
    data << uint32(quest->ItemDropQuantity[0]);
    data << int32(quest->GetZoneOrSort());                                  // zone or sort to display in quest log
    data << uint32(quest->GetRewardSkillId());                              // reward skill id
    data << uint32(hiddenReward ? 0 : quest->RewardChoiceItemCount[0]);
    data << uint32(quest->GetRewSpell());                                   // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(quest->GetQuestGiverPortrait());
    data << uint32(0);                                                      // unknown
    data << uint32(quest->ItemDropQuantity[3]);
    data << uint32(quest->GetFlags());                                      // quest flags
    data << uint32(quest->GetQuestPackageID());
    data << uint32(quest->GetSrcItemId());                                  // source item id

    _session->SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_QUEST_QUERY_RESPONSE questid=%u", quest->GetQuestId());
}

void PlayerMenu::SendQuestGiverOfferReward(Quest const* quest, ObjectGuid npcGuid, bool enableNext) const
{
    std::string logTitle = quest->GetLogTitle();
    std::string rewardText = quest->GetOfferRewardText();
    std::string portraitGiverText = quest->GetPortraitGiverText();
    std::string portraitGiverName = quest->GetPortraitGiverName();
    std::string portraitTurnInText = quest->GetPortraitTurnInText();
    std::string questTurnTargetName = quest->GetPortraitTurnInName();

    LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
    if (localeConstant != LOCALE_enUS)
    {
        if (QuestTemplateLocale const* questTemplateLocale = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
        {
            ObjectMgr::GetLocaleString(questTemplateLocale->LogTitle,           localeConstant, logTitle);
            ObjectMgr::GetLocaleString(questTemplateLocale->PortraitGiverText,  localeConstant, portraitGiverText);
            ObjectMgr::GetLocaleString(questTemplateLocale->PortraitGiverName,  localeConstant, portraitGiverName);
            ObjectMgr::GetLocaleString(questTemplateLocale->PortraitTurnInText, localeConstant, portraitTurnInText);
            ObjectMgr::GetLocaleString(questTemplateLocale->PortraitTurnInName, localeConstant, questTurnTargetName);
        }

        if (QuestOfferRewardLocale const* questOfferRewardLocale = sObjectMgr->GetQuestOfferRewardLocale(quest->GetQuestId()))
            ObjectMgr::GetLocaleString(questOfferRewardLocale->RewardText, localeConstant, rewardText);
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(logTitle, quest->GetQuestLevel());

    std::vector<std::pair<uint32 /*itemId*/, uint32 /*count*/>> rewInfo;
    uint32 rewItemDisplayId[QUEST_REWARD_ITEM_COUNT];
    for (uint8 i = 0; i < QUEST_REWARD_ITEM_COUNT; i++)
    {
        uint32 id = quest->RewardItemId[i];
        uint32 count = quest->RewardItemCount[i];

        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(id);
        rewItemDisplayId[i] = itemTemplate ? itemTemplate->DisplayInfoID : 0;

        rewInfo.push_back(std::make_pair(id, count));
    }

    uint32 rewChoiceItemDisplayId[QUEST_REWARD_CHOICES_COUNT];
    for (uint8 i = 0; i < QUEST_REWARD_CHOICES_COUNT; i++)
    {
        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(quest->RewardChoiceItemId[i]);
        rewChoiceItemDisplayId[i] = itemTemplate ? itemTemplate->DisplayInfoID : 0;
    }

    uint32 rewEmoteCount = 0;
    for (uint8 i = 0; i < QUEST_EMOTE_COUNT; i++)
    {
        if (quest->OfferRewardEmote[i] <= 0)
            break;

        rewEmoteCount++;
    }

    uint32 questEnderEntry = 0;
    if (npcGuid)
        if (Object* object = ObjectAccessor::GetObjectByTypeMask(*(_session->GetPlayer()), npcGuid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM))
            if (object->hasInvolvedQuest(quest->GetQuestId()))
                questEnderEntry = object->GetEntry();

    ObjectGuid guid = npcGuid;

    WorldPacket data(SMSG_QUESTGIVER_OFFER_REWARD, 50);     // guess size
    data << uint32(rewInfo[2].second);
    data << uint32(quest->GetQuestId());
    data << uint32(rewInfo[3].first);
    data << uint32(rewChoiceItemDisplayId[2]);

    for (uint8 i = 0; i < QUEST_REWARD_REPUTATIONS_COUNT; i++)
    {
        data << uint32(quest->RewardFactionId[i]);
        data << uint32(quest->RewardFactionValue[i]);
        data << uint32(quest->RewardFactionOverride[i]);
    }

    data << uint32(rewInfo[0].second);
    data << uint32(rewInfo[3].second);
    data << uint32(rewItemDisplayId[3]);
    data << uint32(rewInfo[1].first);
    data << uint32(quest->RewardChoiceItemId[3]);
    data << uint32(rewChoiceItemDisplayId[3]);
    data << uint32(quest->GetRewChoiceItemsCount());
    data << uint32(0); // quest->GetRewSpellCast() (see above)
    data << uint32(rewItemDisplayId[1]);
    data << uint32(quest->RewardChoiceItemCount[5]);
    data << uint32(rewChoiceItemDisplayId[4]);
    data << uint32(quest->RewardChoiceItemCount[1]);
    data << uint32(rewChoiceItemDisplayId[0]);
    data << uint32(rewItemDisplayId[0]);
    data << uint32(quest->GetQuestPackageID());
    data << uint32(quest->GetQuestTurnInPortrait());
    data << uint32(rewInfo[1].second);
    data << uint32(quest->GetRewardReputationMask());
    data << uint32(quest->RewardChoiceItemId[0]);
    data << uint32(quest->RewardChoiceItemCount[3]);
    data << uint32(quest->RewardChoiceItemCount[4]);
    data << uint32(quest->RewardChoiceItemId[1]);
    data << uint32(0); // quest->GetBonusTalents()
    data << uint32(quest->GetRewardSkillId());

    for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; i++)
    {
        data << uint32(quest->RewardCurrencyId[i]);
        data << uint32(quest->RewardCurrencyCount[i]);
    }

    data << uint32(quest->GetFlags());
    data << uint32(quest->GetFlagsEx());
    data << uint32(quest->XPValue(_session->GetPlayer()) * sWorld->getRate(RATE_XP_QUEST));
    data << uint32(quest->GetRewTitle());
    data << uint32(quest->RewardChoiceItemId[2]);
    data << uint32(quest->GetRewItemsCount());
    data << uint32(quest->GetSuggestedPlayers());
    data << uint32(quest->RewardChoiceItemId[4]);
    data << uint32(questEnderEntry);
    data << uint32(rewInfo[2].first);
    data << uint32(quest->RewardChoiceItemCount[0]);
    data << uint32(quest->GetRewardSkillPoints());
    data << uint32(quest->GetQuestGiverPortrait());
    data << uint32(quest->GetRewOrReqMoney(_session->GetPlayer()));
    data << uint32(quest->RewardChoiceItemId[5]);
    data << uint32(rewChoiceItemDisplayId[1]);
    data << uint32(quest->RewardChoiceItemCount[2]);
    data << uint32(rewItemDisplayId[2]);
    data << uint32(quest->GetRewSpell());
    data << uint32(rewInfo[0].first);
    data << uint32(rewChoiceItemDisplayId[5]);

    data.WriteBits(portraitTurnInText.size(), 10);
    data.WriteBits(portraitGiverName.size(), 8);
    data.WriteBit(guid[6]);
    data.WriteBits(rewEmoteCount, 21);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBits(logTitle.size(), 9);
    data.WriteBit(guid[4]);
    data.WriteBits(questTurnTargetName.size(), 8);
    data.WriteBits(portraitGiverText.size(), 10);
    data.WriteBits(rewardText.size(), 12);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[5]);
    data.WriteBit(enableNext);
    data.FlushBits();

    data.WriteString(portraitGiverName);
    data.WriteString(logTitle);

    for (uint8 i = 0; i < rewEmoteCount; i++)
    {
        data << uint32(quest->OfferRewardEmoteDelay[i]);
        data << uint32(quest->OfferRewardEmote[i]);
    }

    data.WriteByteSeq(guid[2]);
    data.WriteString(rewardText);
    data.WriteString(portraitTurnInText);
    data.WriteString(questTurnTargetName);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[1]);
    data.WriteString(portraitGiverText);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);

    _session->SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD NPCGuid=%u, questid=%u", npcGuid.GetCounter(), quest->GetQuestId());
}

void PlayerMenu::SendQuestGiverRequestItems(Quest const* quest, ObjectGuid npcGuid, bool canComplete, bool closeOnCancel) const
{
    // We can always call to RequestItems, but this packet only goes out if there are actually
    // items.  Otherwise, we'll skip straight to the OfferReward
    if (!quest->HasQuestObjectiveType(QUEST_OBJECTIVE_ITEM) && canComplete)
    {
        SendQuestGiverOfferReward(quest, npcGuid, true);
        return;
    }

    std::string questTitle = quest->GetLogTitle();
    std::string requestItemsText = quest->GetRequestItemsText();

    LocaleConstant localeConstant = _session->GetSessionDbLocaleIndex();
    if (localeConstant != LOCALE_enUS)
    {
        if (QuestTemplateLocale const* questTemplateLocale = sObjectMgr->GetQuestLocale(quest->GetQuestId()))
            ObjectMgr::GetLocaleString(questTemplateLocale->LogTitle, localeConstant, questTitle);

        if (QuestRequestItemsLocale const* questRequestItemsLocale = sObjectMgr->GetQuestRequestItemsLocale(quest->GetQuestId()))
            ObjectMgr::GetLocaleString(questRequestItemsLocale->CompletionText, localeConstant, requestItemsText);        
    }

    uint32 requiredMoney = 0;
    ByteBuffer currencyData, itemData;

    uint32 currencyCounter = 0;
    uint32 itemCounter = 0;
    for (const auto& questObjective : quest->Objectives)
    {
        switch (questObjective.Type)
        {
            case QUEST_OBJECTIVE_ITEM:
            {
                ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(questObjective.ObjectID);

                itemData << uint32(itemTemplate ? itemTemplate->DisplayInfoID : 0);
                itemData << uint32(questObjective.ObjectID);
                itemData << uint32(questObjective.Amount);

                itemCounter++;
                break;
            }
            case QUEST_OBJECTIVE_CURRENCY:
            {
                currencyData << uint32(questObjective.Amount);
                currencyData << uint32(questObjective.ObjectID);

                currencyCounter++;
                break;
            }
            case QUEST_OBJECTIVE_MONEY:
            {
                requiredMoney = questObjective.Amount;
                break;
            }
            default:
                break;
        }
    }

    if (sWorld->getBoolConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

    ObjectGuid guid = npcGuid;

    uint32 questGiverEntry = 0;
    if (npcGuid)
        if (Object* object = ObjectAccessor::GetObjectByTypeMask(*(_session->GetPlayer()), npcGuid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM))
            if (object->hasInvolvedQuest(quest->GetQuestId()))
                questGiverEntry = object->GetEntry();

    WorldPacket data(SMSG_QUESTGIVER_REQUEST_ITEMS, 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 4 + 1 + 8 + 8 +
        questTitle.size() + requestItemsText.size() + itemCounter * (4 + 4 + 4) + currencyCounter * (4 + 4));

    data << uint32(quest->GetSuggestedPlayers());
    data << uint32(quest->GetFlags());
    data << uint32(0);                         // emote delay
    data << uint32(canComplete ? 0x5F : 0x5B); // status flags
    data << uint32(requiredMoney);
    data << uint32(questGiverEntry);
    data << uint32(quest->GetFlagsEx());
    data << uint32(canComplete ? quest->GetCompleteEmote() : quest->GetIncompleteEmote());
    data << uint32(quest->GetQuestId());

    data.WriteBits(currencyCounter, 21);
    data.WriteBit(closeOnCancel);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[1]);
    data.WriteBits(questTitle.size(), 9);
    data.WriteBits(requestItemsText.size(), 12);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBits(itemCounter, 20);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.FlushBits();

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[2]);
    data.WriteString(questTitle);
    data.append(currencyData);
    data.append(itemData);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[1]);
    data.WriteString(requestItemsText);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[6]);


    _session->SendPacket(&data);

    TC_LOG_DEBUG("network", "WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS NPCGuid=%u, questid=%u", npcGuid.GetCounter(), quest->GetQuestId());
}

void PlayerMenu::AddQuestLevelToTitle(std::string &title, int32 level)
{
    // Adds the quest level to the front of the quest title
    // example: [13] Westfall Stew

    std::stringstream questTitlePretty;
    questTitlePretty << "[" << level << "] " << title;
    title = questTitlePretty.str();
}
