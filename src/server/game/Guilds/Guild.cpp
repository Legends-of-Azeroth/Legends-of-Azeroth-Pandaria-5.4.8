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

#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "CalendarMgr.h"
#include "Chat.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "Guild.h"
#include "GuildFinderMgr.h"
#include "GuildMgr.h"
#include "Language.h"
#include "Log.h"
#include "ScriptMgr.h"
#include "SocialMgr.h"
#include "Opcodes.h"
#include "Realm.h"
#include "ReputationMgr.h"

#define MAX_GUILD_BANK_TAB_TEXT_LEN 500
#define EMBLEM_PRICE 10 * GOLD

std::string _GetGuildEventString(GuildEvents event)
{
    switch (event)
    {
        case GE_PROMOTION:
            return "Member promotion";
        case GE_DEMOTION:
            return "Member demotion";
        case GE_MOTD:
            return "Guild MOTD";
        case GE_JOINED:
            return "Member joined";
        case GE_LEFT:
            return "Member left";
        case GE_REMOVED:
            return "Member removed";
        case GE_LEADER_IS:
            return "Leader is";
        case GE_LEADER_CHANGED:
            return "Leader changed";
        case GE_DISBANDED:
            return "Guild disbanded";
        case GE_TABARDCHANGE:
            return "Tabard change";
        case GE_RANK_UPDATED:
            return "Rank updated";
        case GE_RANK_DELETED:
            return "Rank deleted";
        case GE_SIGNED_ON:
            return "Member signed on";
        case GE_SIGNED_OFF:
            return "Member signed off";
        case GE_GUILDBANKBAGSLOTS_CHANGED:
            return "Bank bag slots changed";
        case GE_BANK_TAB_PURCHASED:
            return "Bank tab purchased";
        case GE_BANK_TAB_UPDATED:
            return "Bank tab updated";
        case GE_BANK_MONEY_SET:
            return "Bank money set";
        case GE_BANK_MONEY_CHANGED:
            return "Bank money changed";
        case GE_BANK_TEXT_CHANGED:
            return "Bank tab text changed";
        default:
            break;
    }
    return "<None>";
}

inline uint32 _GetGuildBankTabPrice(uint8 tabId)
{
    switch (tabId)
    {
        case 0: return 100;
        case 1: return 250;
        case 2: return 500;
        case 3: return 1000;
        case 4: return 2500;
        case 5: return 5000;
        default: return 0;
    }
}

void Guild::SendCommandResult(WorldSession* session, GuildCommandType type, GuildCommandError errCode, std::string const& param)
{
    WorldPacket data(SMSG_GUILD_COMMAND_RESULT, 8 + param.size() + 1);
    data << uint32(type);
    data << uint32(errCode);

    data.WriteBits(param.size(), 8);
    data.FlushBits();
    data.WriteString(param);

    session->SendPacket(&data);

    TC_LOG_DEBUG("guild", "SMSG_GUILD_COMMAND_RESULT [%s]: Type: %u, code: %u, param: %s", session->GetPlayerInfo().c_str(), type, errCode, param.c_str());
}

void Guild::SendSaveEmblemResult(WorldSession* session, GuildEmblemError errCode)
{
    WorldPacket data(SMSG_SAVE_GUILD_EMBLEM, 4);
    data << uint32(errCode);
    session->SendPacket(&data);

    TC_LOG_DEBUG("guild", "SMSG_SAVE_GUILD_EMBLEM [%s] Code: %u", session->GetPlayerInfo().c_str(), errCode);
}

// LogHolder
Guild::LogHolder::~LogHolder()
{
    // Cleanup
    for (GuildLog::iterator itr = m_log.begin(); itr != m_log.end(); ++itr)
        delete (*itr);
}

// Adds event loaded from database to collection
inline void Guild::LogHolder::LoadEvent(LogEntry* entry)
{
    if (m_nextGUID == uint32(GUILD_EVENT_LOG_GUID_UNDEFINED))
        m_nextGUID = entry->GetGUID();
    m_log.push_front(entry);
}

// Adds new event happened in game.
// If maximum number of events is reached, oldest event is removed from collection.
inline void Guild::LogHolder::AddEvent(CharacterDatabaseTransaction trans, LogEntry* entry)
{
    // Check max records limit
    if (m_log.size() >= m_maxRecords)
    {
        LogEntry* oldEntry = m_log.front();
        delete oldEntry;
        m_log.pop_front();
    }
    // Add event to list
    m_log.push_back(entry);
    // Save to DB
    entry->SaveToDB(trans);
}

// Writes information about all events into packet.
inline void Guild::LogHolder::WritePacket(WorldPacket& data) const
{
    ByteBuffer buffer;
    data.WriteBits(m_log.size(), 21);
    for (GuildLog::const_iterator itr = m_log.begin(); itr != m_log.end(); ++itr)
        (*itr)->WritePacket(data, buffer);

    data.FlushBits();
    data.append(buffer);
}

inline uint32 Guild::LogHolder::GetNextGUID()
{
    // Next guid was not initialized. It means there are no records for this holder in DB yet.
    // Start from the beginning.
    if (m_nextGUID == uint32(GUILD_EVENT_LOG_GUID_UNDEFINED))
        m_nextGUID = 0;
    else
        m_nextGUID = (m_nextGUID + 1) % m_maxRecords;
    return m_nextGUID;
}

// EventLogEntry
void Guild::EventLogEntry::SaveToDB(CharacterDatabaseTransaction trans) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_EVENTLOG);
    stmt->setUInt32(0, m_guildId);
    stmt->setUInt32(1, m_guid);
    CharacterDatabase.ExecuteOrAppend(trans, stmt);

    uint8 index = 0;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_EVENTLOG);
    stmt->setUInt32(  index, m_guildId);
    stmt->setUInt32(++index, m_guid);
    stmt->setUInt8 (++index, uint8(m_eventType));
    stmt->setUInt32(++index, m_playerGuid1);
    stmt->setUInt32(++index, m_playerGuid2);
    stmt->setUInt8 (++index, m_newRank);
    stmt->setUInt64(++index, m_timestamp);
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::EventLogEntry::WritePacket(WorldPacket& data, ByteBuffer& content) const
{
    ObjectGuid guid1(HighGuid::Player, m_playerGuid1);
    ObjectGuid guid2(HighGuid::Player, m_playerGuid2);

    data.WriteBit(guid1[6]);
    data.WriteBit(guid1[1]);
    data.WriteBit(guid2[5]);
    data.WriteBit(guid2[1]);
    data.WriteBit(guid2[3]);
    data.WriteBit(guid2[0]);
    data.WriteBit(guid2[4]);
    data.WriteBit(guid1[4]);
    data.WriteBit(guid2[7]);
    data.WriteBit(guid1[0]);
    data.WriteBit(guid1[2]);
    data.WriteBit(guid1[7]);
    data.WriteBit(guid1[3]);
    data.WriteBit(guid1[5]);
    data.WriteBit(guid2[2]);
    data.WriteBit(guid2[6]);

    content.WriteByteSeq(guid1[5]);
    content.WriteByteSeq(guid1[4]);
    content.WriteByteSeq(guid2[6]);
    content.WriteByteSeq(guid1[2]);
    content.WriteByteSeq(guid2[4]);

    // Event type
    content << uint8(m_eventType);

    content.WriteByteSeq(guid2[0]);
    content.WriteByteSeq(guid1[7]);
    content.WriteByteSeq(guid1[3]);
    content.WriteByteSeq(guid2[5]);
    content.WriteByteSeq(guid2[2]);
    content.WriteByteSeq(guid1[0]);

    // Event timestamp
    content << uint32(::time(NULL) - m_timestamp);

    content.WriteByteSeq(guid1[1]);
    content.WriteByteSeq(guid1[6]);
    content.WriteByteSeq(guid2[7]);
    content.WriteByteSeq(guid2[1]);

    // New Rank
    content << uint8(m_newRank);

    content.WriteByteSeq(guid2[3]);
}

// BankEventLogEntry
void Guild::BankEventLogEntry::SaveToDB(CharacterDatabaseTransaction trans) const
{
    uint8 index = 0;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_EVENTLOG);
    stmt->setUInt32(  index, m_guildId);
    stmt->setUInt32(++index, m_guid);
    stmt->setUInt8 (++index, m_bankTabId);
    CharacterDatabase.ExecuteOrAppend(trans, stmt);

    index = 0;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_BANK_EVENTLOG);
    stmt->setUInt32(  index, m_guildId);
    stmt->setUInt32(++index, m_guid);
    stmt->setUInt8 (++index, m_bankTabId);
    stmt->setUInt8 (++index, uint8(m_eventType));
    stmt->setUInt32(++index, m_playerGuid);
    stmt->setUInt32(++index, m_itemOrMoney);
    stmt->setUInt16(++index, m_itemStackCount);
    stmt->setUInt8 (++index, m_destTabId);
    stmt->setUInt64(++index, m_timestamp);
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::BankEventLogEntry::WritePacket(WorldPacket& data, ByteBuffer& content) const
{
    ObjectGuid logGuid(HighGuid::Player, m_playerGuid);
    ObjectGuid unkGuid = ObjectGuid::Empty;

    bool hasItem = m_eventType == GUILD_BANK_LOG_DEPOSIT_ITEM || m_eventType == GUILD_BANK_LOG_WITHDRAW_ITEM ||
                   m_eventType == GUILD_BANK_LOG_MOVE_ITEM || m_eventType == GUILD_BANK_LOG_MOVE_ITEM2;

    bool itemMoved = (m_eventType == GUILD_BANK_LOG_MOVE_ITEM || m_eventType == GUILD_BANK_LOG_MOVE_ITEM2);

    bool hasStack = (hasItem && m_itemStackCount > 1) || itemMoved;

    data.WriteBit(IsMoneyEvent());
    data.WriteBit(logGuid[0]);
    data.WriteBit(logGuid[2]);
    data.WriteBit(logGuid[3]);
    data.WriteBit(logGuid[6]);
    data.WriteBit(logGuid[5]);
    data.WriteBit(logGuid[4]);
    data.WriteBit(hasStack);
    data.WriteBit(hasItem);
    data.WriteBit(logGuid[7]);
    data.WriteBit(logGuid[1]);
    data.WriteBit(itemMoved);

    content.WriteByteSeq(logGuid[1]);
    content.WriteByteSeq(logGuid[7]);

    if (itemMoved)
        content << uint8(m_destTabId);

    content.WriteByteSeq(logGuid[2]);
    content << uint32(time(NULL) - m_timestamp);
    content << uint8(m_eventType);
    content.WriteByteSeq(logGuid[0]);
    content.WriteByteSeq(logGuid[4]);

    if (hasItem)
        content << uint32(m_itemOrMoney);

    if (IsMoneyEvent())
        content << uint64(m_itemOrMoney);

    content.WriteByteSeq(logGuid[6]);

    if (hasStack)
        content << uint32(m_itemStackCount);

    content.WriteByteSeq(logGuid[5]);
    content.WriteByteSeq(logGuid[3]);
}

void Guild::NewsLogEntry::SaveToDB(CharacterDatabaseTransaction trans) const
{
    uint8 index = 0;
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_NEWS);
    stmt->setUInt32(  index, m_guildId);
    stmt->setUInt32(++index, GetGUID());
    stmt->setUInt8 (++index, GetType());
    stmt->setUInt32(++index, GetPlayerGuid());
    stmt->setUInt32(++index, GetFlags());
    stmt->setUInt32(++index, GetValue());
    stmt->setUInt64(++index, GetTimestamp());
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::NewsLogEntry::WritePacket(WorldPacket& data, ByteBuffer& /*content*/) const
{
    ObjectGuid guid = GetPlayerGuid();

    data.WriteBit(guid[3]);
    data.WriteBits(0, 24); // Not yet implemented used for guild achievements
    data.WriteBit(guid[4]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[1]);

    data.FlushBits();

    data.WriteByteSeq(guid[2]);
    data << uint32(GetValue());
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data << uint32(0);
    data << uint32(GetType());
    data << uint32(GetFlags());   // 1 sticky
    data.WriteByteSeq(guid[6]);
    data.AppendPackedTime(GetTimestamp());
    data << uint32(GetGUID());
    data.WriteByteSeq(guid[0]);
}

// RankInfo
void Guild::RankInfo::LoadFromDB(Field* fields)
{
    m_rankId            = fields[1].GetUInt8();
    m_name              = fields[2].GetString();
    m_rights            = fields[3].GetUInt32();
    m_bankMoneyPerDay   = fields[4].GetUInt32();
    if (m_rankId == GR_GUILDMASTER)                     // Prevent loss of leader rights
        m_rights |= GR_RIGHT_ALL;
}

void Guild::RankInfo::SaveToDB(CharacterDatabaseTransaction trans, size_t index) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_RANK);
    stmt->setUInt32(0, m_guildId);
    stmt->setUInt8 (1, m_rankId);
    stmt->setString(2, m_name);
    stmt->setUInt32(3, m_rights);
    stmt->setUInt8 (4, uint8(index));
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::RankInfo::CreateMissingTabsIfNeeded(uint8 tabs, CharacterDatabaseTransaction trans, bool logOnCreate /* = false */)
{
    for (uint8 i = 0; i < tabs; ++i)
    {
        GuildBankRightsAndSlots& rightsAndSlots = m_bankTabRightsAndSlots[i];
        if (rightsAndSlots.GetTabId() == i)
            continue;

        rightsAndSlots.SetTabId(i);
        if (m_rankId == GR_GUILDMASTER)
            rightsAndSlots.SetGuildMasterValues();

        if (logOnCreate)
            TC_LOG_ERROR("guild", "Guild %u has broken Tab %u for rank %u. Created default tab.", m_guildId, i, m_rankId);

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_BANK_RIGHT);
        stmt->setUInt32(0, m_guildId);
        stmt->setUInt8(1, i);
        stmt->setUInt8(2, m_rankId);
        stmt->setUInt8(3, rightsAndSlots.GetRights());
        stmt->setUInt32(4, rightsAndSlots.GetSlots());
        trans->Append(stmt);
    }
}

void Guild::RankInfo::SetName(std::string const& name)
{
    if (m_name == name)
        return;

    m_name = name;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_RANK_NAME);
    stmt->setString(0, m_name);
    stmt->setUInt8 (1, m_rankId);
    stmt->setUInt32(2, m_guildId);
    CharacterDatabase.Execute(stmt);
}

void Guild::RankInfo::SetRights(uint32 rights)
{
    if (m_rankId == GR_GUILDMASTER)                     // Prevent loss of leader rights
        rights = GR_RIGHT_ALL;

    if (m_rights == rights)
        return;

    m_rights = rights;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_RANK_RIGHTS);
    stmt->setUInt32(0, m_rights);
    stmt->setUInt8 (1, m_rankId);
    stmt->setUInt32(2, m_guildId);
    CharacterDatabase.Execute(stmt);
}

void Guild::RankInfo::SetBankMoneyPerDay(uint32 money)
{
    if (m_rankId == GR_GUILDMASTER)                     // Prevent loss of leader rights
        money = uint32(GUILD_WITHDRAW_MONEY_UNLIMITED);

    if (m_bankMoneyPerDay == money)
        return;

    m_bankMoneyPerDay = money;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_RANK_BANK_MONEY);
    stmt->setUInt32(0, money);
    stmt->setUInt8 (1, m_rankId);
    stmt->setUInt32(2, m_guildId);
    CharacterDatabase.Execute(stmt);
}

void Guild::RankInfo::SetBankTabSlotsAndRights(GuildBankRightsAndSlots rightsAndSlots, bool saveToDB)
{
    if (m_rankId == GR_GUILDMASTER)                     // Prevent loss of leader rights
        rightsAndSlots.SetGuildMasterValues();

    GuildBankRightsAndSlots& guildBR = m_bankTabRightsAndSlots[rightsAndSlots.GetTabId()];
    guildBR = rightsAndSlots;

    if (saveToDB)
    {
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_BANK_RIGHT);
        stmt->setUInt32(0, m_guildId);
        stmt->setUInt8 (1, guildBR.GetTabId());
        stmt->setUInt8 (2, m_rankId);
        stmt->setUInt8 (3, guildBR.GetRights());
        stmt->setUInt32(4, guildBR.GetSlots());
        CharacterDatabase.Execute(stmt);
    }
}

// BankTab
void Guild::BankTab::LoadFromDB(Field* fields)
{
    m_name = fields[2].GetString();
    m_icon = fields[3].GetString();
    m_text = fields[4].GetString();
}

bool Guild::BankTab::LoadItemFromDB(Field* fields)
{
    uint8 slotId = fields[20].GetUInt8();
    uint32 itemGuid = fields[21].GetUInt32();
    uint32 itemEntry = fields[22].GetUInt32();
    if (slotId >= GUILD_BANK_MAX_SLOTS)
    {
        TC_LOG_ERROR("guild", "Invalid slot for item (GUID: %u, id: %u) in guild bank, skipped.", itemGuid, itemEntry);
        return false;
    }

    ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemEntry);
    if (!proto)
    {
        TC_LOG_ERROR("guild", "Unknown item (GUID: %u, id: %u) in guild bank, skipped.", itemGuid, itemEntry);
        return false;
    }

    Item* pItem = NewItemOrBag(proto);
    if (!pItem->LoadFromDB(itemGuid, ObjectGuid::Empty, fields, itemEntry))
    {
        TC_LOG_ERROR("guild", "Item (GUID %u, id: %u) not found in item_instance, deleting from guild bank!", itemGuid, itemEntry);

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_NONEXISTENT_GUILD_BANK_ITEM);
        stmt->setUInt32(0, m_guildId);
        stmt->setUInt8 (1, m_tabId);
        stmt->setUInt8 (2, slotId);
        CharacterDatabase.Execute(stmt);

        delete pItem;
        return false;
    }

    pItem->AddToWorld();
    m_items[slotId] = pItem;
    return true;
}

// Deletes contents of the tab from the world (and from DB if necessary)
void Guild::BankTab::Delete(CharacterDatabaseTransaction trans, bool removeItemsFromDB)
{
    for (uint8 slotId = 0; slotId < GUILD_BANK_MAX_SLOTS; ++slotId)
        if (Item* pItem = m_items[slotId])
        {
            pItem->RemoveFromWorld();
            if (removeItemsFromDB)
                pItem->DeleteFromDB(trans);
            delete pItem;
            pItem = NULL;
        }
}

void Guild::BankTab::SetInfo(std::string const& name, std::string const& icon)
{
    if (m_name == name && m_icon == icon)
        return;

    m_name = name;
    m_icon = icon;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_BANK_TAB_INFO);
    stmt->setString(0, m_name);
    stmt->setString(1, m_icon);
    stmt->setUInt32(2, m_guildId);
    stmt->setUInt8 (3, m_tabId);
    CharacterDatabase.Execute(stmt);
}

void Guild::BankTab::SetText(std::string const& text)
{
    if (m_text == text)
        return;

    m_text = text;
    utf8truncate(m_text, MAX_GUILD_BANK_TAB_TEXT_LEN);          // DB and client size limitation

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_BANK_TAB_TEXT);
    stmt->setString(0, m_text);
    stmt->setUInt32(1, m_guildId);
    stmt->setUInt8 (2, m_tabId);
    CharacterDatabase.Execute(stmt);
}

// Sets/removes contents of specified slot.
// If pItem == NULL contents are removed.
bool Guild::BankTab::SetItem(CharacterDatabaseTransaction trans, uint8 slotId, Item* item)
{
    if (slotId >= GUILD_BANK_MAX_SLOTS)
        return false;

    m_items[slotId] = item;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_ITEM);
    stmt->setUInt32(0, m_guildId);
    stmt->setUInt8 (1, m_tabId);
    stmt->setUInt8 (2, slotId);
    CharacterDatabase.ExecuteOrAppend(trans, stmt);

    if (item)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_BANK_ITEM);
        stmt->setUInt32(0, m_guildId);
        stmt->setUInt8 (1, m_tabId);
        stmt->setUInt8 (2, slotId);
        stmt->setUInt32(3, item->GetGUID().GetCounter());
        CharacterDatabase.ExecuteOrAppend(trans, stmt);

        item->SetUInt64Value(ITEM_FIELD_CONTAINED_IN, 0);
        item->SetUInt64Value(ITEM_FIELD_OWNER, 0);
        item->FSetState(ITEM_NEW);
        item->SaveToDB(trans);                                 // Not in inventory and can be saved standalone
    }
    return true;
}

void Guild::BankTab::SendText(Guild const* guild, WorldSession* session) const
{
    WorldPacket data(SMSG_GUILD_BANK_QUERY_TEXT_RESULT, 4 + 2 + m_text.size());
    data << uint32(m_tabId);

    data.WriteBits(m_text.length(), 14);
    data.FlushBits();

    data.WriteString(m_text);

    if (session)
    {
        TC_LOG_DEBUG("guild", "SMSG_GUILD_BANK_QUERY_TEXT_RESULT [%s]: Tabid: %u, Text: %s", session->GetPlayerInfo().c_str(), m_tabId, m_text.c_str());
        session->SendPacket(&data);
    }
    else
    {
        TC_LOG_DEBUG("guild", "SMSG_GUILD_BANK_QUERY_TEXT_RESULT [Broadcast]: Tabid: %u, Text: %s", m_tabId, m_text.c_str());
        guild->BroadcastPacket(&data);
    }
}

// Member
void Guild::Member::SetStats(Player* player)
{
    m_name      = player->GetName();
    m_level     = player->GetLevel();
    m_class     = player->GetClass();
    m_zoneId    = player->GetZoneId();
    m_accountId = player->GetSession()->GetAccountId();
    m_achievementPoints = player->GetAchievementPoints();
    m_gender    = player->GetGender();
    m_reputation = player->GetReputationMgr().GetReputation(GUILD_REPUTATION_ID);

    SetProfessions(player, true);
}

void Guild::Member::SetStats(std::string const& name, uint8 level, uint8 _class, uint32 zoneId, uint32 accountId, uint32 reputation, uint8 gender)
{
    m_name      = name;
    m_level     = level;
    m_class     = _class;
    m_zoneId    = zoneId;
    m_accountId = accountId;
    m_reputation = reputation;
    m_gender    = gender;

    for (uint8 i = 0; i < 2; ++i)
        SetProfession(i, 0, 0, 0, std::vector<uint8>(300));
}

void Guild::Member::SetPublicNote(std::string const& publicNote)
{
    if (m_publicNote == publicNote)
        return;

    m_publicNote = publicNote;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_MEMBER_PNOTE);
    stmt->setString(0, publicNote);
    stmt->setUInt32(1, m_guid.GetCounter());
    CharacterDatabase.Execute(stmt);
}

void Guild::Member::SetOfficerNote(std::string const& officerNote)
{
    if (m_officerNote == officerNote)
        return;

    m_officerNote = officerNote;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_MEMBER_OFFNOTE);
    stmt->setString(0, officerNote);
    stmt->setUInt32(1, m_guid.GetCounter());
    CharacterDatabase.Execute(stmt);
}

void Guild::Member::SetProfessions(Player* player, bool recipes)
{
    uint8 prof = 0;
    std::set<uint32> prevskills;
    for (auto&& it : player->GetSpellMap())
    {
        if (prof >= 2)
            break;

        uint32 spellId = it.first;
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo)
            continue;

        if (!spellInfo->IsPrimaryProfession())
            continue;

        uint32 skillId = 0;
        for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
        {
            if (spellInfo->Effects[i].Effect == SPELL_EFFECT_SKILL)
            {
                skillId = uint32(spellInfo->Effects[i].MiscValue);
                break;
            }
        }
        if (prevskills.count(skillId))
            continue;

        prevskills.insert(skillId);

        uint16 value = player->GetSkillValue(skillId);
        uint8 rank = sSpellMgr->GetSpellRank(spellId);

        m_professions[prof].SkillId = skillId;
        m_professions[prof].Value = value;
        m_professions[prof].Rank = rank;

        if (recipes)
            m_professions[prof].Recipes = player->MakeTradeSkillRecipesData(skillId);
        ++prof;
    }

    for (uint8 i = prevskills.size(); i < 2; ++i)
        SetProfession(i, 0, 0, 0, std::vector<uint8>(300));
}

void Guild::Member::ChangeRank(uint8 newRank)
{
    m_rankId = newRank;

    // Update rank information in player's field, if he is online.
    if (Player* player = FindPlayer())
        player->SetRank(newRank);

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_MEMBER_RANK);
    stmt->setUInt8 (0, newRank);
    stmt->setUInt32(1, m_guid.GetCounter());
    CharacterDatabase.Execute(stmt);
}

void Guild::Member::SaveToDB(CharacterDatabaseTransaction trans) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_MEMBER);
    stmt->setUInt32(0, m_guildId);
    stmt->setUInt32(1, m_guid.GetCounter());
    stmt->setUInt8 (2, m_rankId);
    stmt->setString(3, m_publicNote);
    stmt->setString(4, m_officerNote);
    stmt->setUInt16(5, m_achievementPoints);
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::Member::SaveProfessionsToDB(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_MEMBER_PROFESSIONS);
    stmt->setUInt16(0, m_professions[0].SkillId);
    stmt->setUInt16(1, m_professions[0].Value);
    stmt->setUInt16(2, m_professions[0].Rank);

    std::ostringstream ss;
    for (auto&& it : m_professions[0].Recipes)
        ss << it << ' ';

    stmt->setString(3, ss.str());
    stmt->setUInt16(4, m_professions[1].SkillId);
    stmt->setUInt16(5, m_professions[1].Value);
    stmt->setUInt16(6, m_professions[1].Rank);

    ss.str("");
    for (auto&& it : m_professions[1].Recipes)
        ss << it << ' ';

    stmt->setString(7, ss.str());

    stmt->setUInt32(8, m_guildId);
    stmt->setUInt32(9, m_guid.GetCounter());
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::Member::SaveAchievementsToDB(CharacterDatabaseTransaction trans)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_MEMBER_ACHIEVEMENTS);
    stmt->setUInt16(0, m_achievementPoints);
    stmt->setUInt32(1, m_guildId);
    stmt->setUInt32(2, m_guid.GetCounter());
    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

// Loads member's data from database.
// If member has broken fields (level, class) returns false.
// In this case member has to be removed from guild.
bool Guild::Member::LoadFromDB(Field* fields)
{
    m_publicNote  = fields[3].GetString();
    m_officerNote = fields[4].GetString();

    for (uint8 i = 0; i <= GUILD_BANK_MAX_TABS; ++i)
        m_bankWithdraw[i] = fields[5 + i].GetUInt32();

    SetStats(fields[14].GetString(),
             fields[15].GetUInt8(),                         // characters.level
             fields[16].GetUInt8(),                         // characters.class
             fields[17].GetUInt16(),                        // characters.zone
             fields[18].GetUInt32(),                        // characters.account
             fields[30].GetUInt32(),                        // gr.reputation
             fields[29].GetUInt8());                        // characters.gender
    m_logoutTime = fields[19].GetUInt32();                  // characters.logout_time

    m_totalActivity = 0;
    m_weekActivity = 0;
    
    if (!CheckStats())
        return false;

    m_achievementPoints = fields[20].GetUInt16();

    bool needsProfessionSave = false;
    for (uint32 i = 0; i < 2; ++i)
    {
        uint32 value = fields[21 + 3 * i].GetUInt16();
        uint32 skillId = fields[22 + 3 * i].GetUInt16();
        uint32 rank = fields[23 + 3 * i].GetUInt16();
        std::string dbRecipes = fields[24 + i].GetString();
        std::vector<uint8> recipes;

        if (skillId)
        {
            if (dbRecipes.empty())
            {
                recipes.resize(300, 0);
                SkillLineEntry const* skill = sSkillLineStore.LookupEntry(skillId);
                if (skill && skill->CanLink)
                {
                    recipes = LoadProfessionRecipesData(skillId, value);
                    needsProfessionSave = true;
                }
            }
            else
            {
                std::istringstream ss(dbRecipes);
                recipes.resize(300, 0);
                for (uint32 j = 0; j < 300; ++j)
                {
                    uint32 byte;
                    ss >> byte;
                    recipes[j] = byte;
                }
            }
        }

        SetProfession(i, uint16(value), skillId, uint8(rank), recipes);
    }

    if (needsProfessionSave)
        SaveProfessionsToDB();

    if (!m_zoneId)
    {
        TC_LOG_ERROR("guild", "Player (GUID: %u) has broken zone-data", m_guid.GetCounter());
        m_zoneId = Player::GetZoneIdFromDB(m_guid);
    }
    ResetFlags();
    return true;
}

// Validate player fields. Returns false if corrupted fields are found.
bool Guild::Member::CheckStats() const
{
    if (m_level < 1)
    {
        TC_LOG_ERROR("guild", "Player (GUID: %u) has a broken data in field `characters`.`level`, deleting him from guild!", m_guid.GetCounter());
        return false;
    }

    if (m_class < CLASS_WARRIOR || m_class >= MAX_CLASSES)
    {
        TC_LOG_ERROR("guild", "Player (GUID: %u) has a broken data in field `characters`.`class`, deleting him from guild!", m_guid.GetCounter());
        return false;
    }
    return true;
}

// Decreases amount of money/slots left for today.
// If (tabId == GUILD_BANK_MAX_TABS) decrease money amount.
// Otherwise decrease remaining items amount for specified tab.
void Guild::Member::UpdateBankWithdrawValue(CharacterDatabaseTransaction trans, uint8 tabId, uint32 amount)
{
    m_bankWithdraw[tabId] += amount;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_MEMBER_WITHDRAW);
    stmt->setUInt32(0, m_guid.GetCounter());
    for (uint8 i = 0; i <= GUILD_BANK_MAX_TABS;)
    {
        uint32 withdraw = m_bankWithdraw[i++];
        stmt->setUInt32(i, withdraw);
    }

    CharacterDatabase.ExecuteOrAppend(trans, stmt);
}

void Guild::Member::ResetValues(bool weekly /* = false*/)
{
    for (uint8 tabId = 0; tabId <= GUILD_BANK_MAX_TABS; ++tabId)
        m_bankWithdraw[tabId] = 0;

    if (weekly)
    {
        m_weekActivity = 0;
    }
}

void Guild::Member::SetReputation(int32 val)
{
    m_reputation = val;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUILD_MEMBER_REPUTATION);
    stmt->setUInt32(0, m_guid);
    stmt->setUInt32(1, m_guildId);
    stmt->setUInt32(2, m_reputation);
    CharacterDatabase.Execute(stmt);
}

// Get amount of money/slots left for today.
// If (tabId == GUILD_BANK_MAX_TABS) return money amount.
// Otherwise return remaining items amount for specified tab.
int32 Guild::Member::GetBankWithdrawValue(uint8 tabId) const
{
    // Guild master has unlimited amount.
    if (IsRank(GR_GUILDMASTER))
        return tabId == GUILD_BANK_MAX_TABS ? GUILD_WITHDRAW_MONEY_UNLIMITED : GUILD_WITHDRAW_SLOT_UNLIMITED;

    return m_bankWithdraw[tabId];
}

std::vector<uint8> Guild::Member::LoadProfessionRecipesData(uint32 skillId, uint32 value) const
{
    std::vector<uint8> data;
    data.resize(300, 0);

    SkillLineEntry const* skill = sSkillLineStore.LookupEntry(skillId);
    if (!skill || !skill->CanLink)
        return data;

    std::set<uint32> spells;

    if (QueryResult result = CharacterDatabase.PQuery("SELECT spell FROM character_spell WHERE guid = %u AND active = 1 AND disabled = 0", GetGUID().GetCounter()))
    {
        do
        {
            Field* fields = result->Fetch();
            spells.insert(fields[0].GetUInt32());
        }
        while (result->NextRow());
    }

    for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
    {
        if (SkillLineAbilityEntry const* ability = sSkillLineAbilityStore.LookupEntry(j))
        {
            if (ability->SkillLine == skillId && (ability->AcquireMethod == ABILITY_LEARNED_ON_GET_PROFESSION_SKILL && value >= ability->MinSkillLineRank || spells.find(ability->Spell) != spells.end()))
            {
                uint32 byte = ability->bitOrder / 8;
                if (byte < 300)
                {
                    uint32 bit = ability->bitOrder & 0x80000007;
                    if (bit < 0)
                        bit = (bit & ~0xFF) | (((bit - 1) | 0xF8) + 1);
                    data[byte] |= 1 << bit;
                }
            }
        }
    }

    return data;
}

// EmblemInfo
void EmblemInfo::ReadPacket(WorldPacket& recv)
{
    recv >> m_borderStyle >> m_backgroundColor >> m_borderColor >> m_color >> m_style;
}

void EmblemInfo::LoadFromDB(Field* fields)
{
    m_style             = fields[3].GetUInt8();
    m_color             = fields[4].GetUInt8();
    m_borderStyle       = fields[5].GetUInt8();
    m_borderColor       = fields[6].GetUInt8();
    m_backgroundColor   = fields[7].GetUInt8();
}

void EmblemInfo::WritePacket(WorldPacket& data) const
{
    data << uint32(m_style);
    data << uint32(m_color);
    data << uint32(m_borderStyle);
    data << uint32(m_borderColor);
    data << uint32(m_backgroundColor);
}

void EmblemInfo::SaveToDB(uint32 guildId) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_EMBLEM_INFO);
    stmt->setUInt32(0, m_style);
    stmt->setUInt32(1, m_color);
    stmt->setUInt32(2, m_borderStyle);
    stmt->setUInt32(3, m_borderColor);
    stmt->setUInt32(4, m_backgroundColor);
    stmt->setUInt32(5, guildId);
    CharacterDatabase.Execute(stmt);
}

// MoveItemData
bool Guild::MoveItemData::CheckItem(uint32& splitedAmount)
{
    ASSERT(m_pItem);
    if (splitedAmount > m_pItem->GetCount())
        return false;
    if (splitedAmount == m_pItem->GetCount())
        splitedAmount = 0;
    return true;
}

bool Guild::MoveItemData::CanStore(Item* pItem, bool swap, bool sendError)
{
    m_vec.clear();
    InventoryResult msg = CanStore(pItem, swap);
    if (sendError && msg != EQUIP_ERR_OK)
        m_pPlayer->SendEquipError(msg, pItem);
    return (msg == EQUIP_ERR_OK);
}

bool Guild::MoveItemData::CloneItem(uint32 count)
{
    ASSERT(m_pItem);
    m_pClonedItem = m_pItem->CloneItem(count);
    if (!m_pClonedItem)
    {
        m_pPlayer->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, m_pItem);
        return false;
    }
    return true;
}

void Guild::MoveItemData::LogAction(MoveItemData* pFrom, bool split) const
{
    ASSERT(pFrom->GetItem());

    sScriptMgr->OnGuildItemMove(m_pGuild, m_pPlayer, pFrom->GetItem(),
        pFrom->IsBank(), pFrom->GetContainer(), pFrom->GetSlotId(),
        IsBank(), GetContainer(), GetSlotId());
}

inline void Guild::MoveItemData::CopySlots(SlotIds& ids) const
{
    for (ItemPosCountVec::const_iterator itr = m_vec.begin(); itr != m_vec.end(); ++itr)
        ids.insert(uint8(itr->pos));
}

// PlayerMoveItemData
bool Guild::PlayerMoveItemData::InitItem()
{
    m_pItem = m_pPlayer->GetItemByPos(m_container, m_slotId);
    if (m_pItem)
    {
        // Anti-WPE protection. Do not move non-empty bags to bank.
        if (m_pItem->IsNotEmptyBag())
        {
            m_pPlayer->SendEquipError(EQUIP_ERR_DESTROY_NONEMPTY_BAG, m_pItem);
            m_pItem = NULL;
        }
        // Bound items cannot be put into bank.
        else if (!m_pItem->CanBeTraded())
        {
            m_pPlayer->SendEquipError(EQUIP_ERR_CANT_SWAP, m_pItem);
            m_pItem = NULL;
        }
    }
    return (m_pItem != NULL);
}

void Guild::PlayerMoveItemData::RemoveItem(CharacterDatabaseTransaction trans, MoveItemData* /*pOther*/, uint32 splitedAmount)
{
    if (splitedAmount)
    {
        m_pItem->SetCount(m_pItem->GetCount() - splitedAmount);
        m_pItem->SetState(ITEM_CHANGED, m_pPlayer);
        m_pPlayer->SaveInventoryAndGoldToDB(trans);
    }
    else
    {
        m_pPlayer->MoveItemFromInventory(m_container, m_slotId, true);
        m_pItem->DeleteFromInventoryDB(trans);
        m_pItem = NULL;
    }
}

Item* Guild::PlayerMoveItemData::StoreItem(CharacterDatabaseTransaction trans, Item* pItem)
{
    ASSERT(pItem);
    m_pPlayer->MoveItemToInventory(m_vec, pItem, true);
    m_pPlayer->SaveInventoryAndGoldToDB(trans);
    return pItem;
}

void Guild::PlayerMoveItemData::LogBankEvent(CharacterDatabaseTransaction trans, MoveItemData* pFrom, uint32 count) const
{
    ASSERT(pFrom);
    // Bank -> Char
    m_pGuild->LogBankEvent(trans, GUILD_BANK_LOG_WITHDRAW_ITEM, pFrom->GetContainer(), m_pPlayer->GetGUID().GetCounter(),
        pFrom->GetItem()->GetEntry(), count);
}

void Guild::PlayerMoveItemData::LogAction(MoveItemData* from, bool split) const
{

}

inline InventoryResult Guild::PlayerMoveItemData::CanStore(Item* pItem, bool swap)
{
    return m_pPlayer->CanStoreItem(m_container, m_slotId, m_vec, pItem, swap);
}

// BankMoveItemData
bool Guild::BankMoveItemData::InitItem()
{
    m_pItem = m_pGuild->GetItem(m_container, m_slotId);
    return (m_pItem != NULL);
}

bool Guild::BankMoveItemData::HasStoreRights(MoveItemData* pOther) const
{
    ASSERT(pOther);
    // Do not check rights if item is being swapped within the same bank tab
    if (pOther->IsBank() && pOther->GetContainer() == m_container)
        return true;
    return m_pGuild->MemberHasTabRights(m_pPlayer->GetGUID(), m_container, GUILD_BANK_RIGHT_DEPOSIT_ITEM);
}

bool Guild::BankMoveItemData::HasWithdrawRights(MoveItemData* pOther) const
{
    ASSERT(pOther);
    // Do not check rights if item is being swapped within the same bank tab
    if (pOther->IsBank() && pOther->GetContainer() == m_container)
        return true;

    int32 slots = 0;
    if (Member const* member = m_pGuild->GetMember(m_pPlayer->GetGUID()))
        slots = m_pGuild->GetMemberRemainingSlots(member, m_container);

    return slots != 0;
}

void Guild::BankMoveItemData::RemoveItem(CharacterDatabaseTransaction trans, MoveItemData* pOther, uint32 splitedAmount)
{
    ASSERT(m_pItem);
    if (splitedAmount)
    {
        m_pItem->SetCount(m_pItem->GetCount() - splitedAmount);
        m_pItem->FSetState(ITEM_CHANGED);
        m_pItem->SaveToDB(trans);
    }
    else
    {
        m_pGuild->RemoveItem(trans, m_container, m_slotId);
        m_pItem = NULL;
    }
    // Decrease amount of player's remaining items (if item is moved to different tab or to player)
    if (!pOther->IsBank() || pOther->GetContainer() != m_container)
        m_pGuild->UpdateMemberWithdrawSlots(trans, m_pPlayer->GetGUID(), m_container);
}

Item* Guild::BankMoveItemData::StoreItem(CharacterDatabaseTransaction trans, Item* pItem)
{
    if (!pItem)
        return NULL;

    BankTab* pTab = m_pGuild->GetBankTab(m_container);
    if (!pTab)
        return NULL;

    Item* pLastItem = pItem;
    for (ItemPosCountVec::const_iterator itr = m_vec.begin(); itr != m_vec.end(); )
    {
        ItemPosCount pos(*itr);
        ++itr;

        TC_LOG_DEBUG("guild", "GUILD STORAGE: StoreItem tab = %u, slot = %u, item = %u, count = %u",
            m_container, m_slotId, pItem->GetEntry(), pItem->GetCount());
        pLastItem = _StoreItem(trans, pTab, pItem, pos, itr != m_vec.end());
    }
    return pLastItem;
}

void Guild::BankMoveItemData::LogBankEvent(CharacterDatabaseTransaction trans, MoveItemData* pFrom, uint32 count) const
{
    ASSERT(pFrom->GetItem());
    if (pFrom->IsBank())
        // Bank -> Bank
        m_pGuild->LogBankEvent(trans, GUILD_BANK_LOG_MOVE_ITEM, pFrom->GetContainer(), m_pPlayer->GetGUID().GetCounter(),
            pFrom->GetItem()->GetEntry(), count, m_container);
    else
        // Char -> Bank
        m_pGuild->LogBankEvent(trans, GUILD_BANK_LOG_DEPOSIT_ITEM, m_container, m_pPlayer->GetGUID().GetCounter(),
            pFrom->GetItem()->GetEntry(), count);
}

void Guild::BankMoveItemData::LogAction(MoveItemData* pFrom, bool split) const
{
    MoveItemData::LogAction(pFrom, split);
    if (!pFrom->IsBank()) /// @todo Move this to scripts
    {
        if (sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE) && m_pPlayer->GetSession()->GetSecurity() > SEC_PLAYER)
            sLog->outCommand(m_pPlayer->GetSession()->GetAccountId(),
            "GM %s (Guid: %u) (Account: %u) deposit item: %s (Entry: %d Count: %u) to guild bank named: %s (Guild ID: %u)",
            m_pPlayer->GetName().c_str(), m_pPlayer->GetGUID().GetCounter(), m_pPlayer->GetSession()->GetAccountId(),
            pFrom->GetItem()->GetTemplate()->Name1.c_str(), pFrom->GetItem()->GetEntry(), pFrom->GetItem()->GetCount(),
            m_pGuild->GetName().c_str(), m_pGuild->GetId());
    }
}

Item* Guild::BankMoveItemData::_StoreItem(CharacterDatabaseTransaction trans, BankTab* pTab, Item* pItem, ItemPosCount& pos, bool clone) const
{
    uint8 slotId = uint8(pos.pos);
    uint32 count = pos.count;
    if (Item* pItemDest = pTab->GetItem(slotId))
    {
        pItemDest->SetCount(pItemDest->GetCount() + count);
        pItemDest->FSetState(ITEM_CHANGED);
        pItemDest->SaveToDB(trans);
        if (!clone)
        {
            pItem->RemoveFromWorld();
            pItem->DeleteFromDB(trans);
            delete pItem;
        }
        return pItemDest;
    }

    if (clone)
        pItem = pItem->CloneItem(count);
    else
        pItem->SetCount(count);

    if (pItem && pTab->SetItem(trans, slotId, pItem))
        return pItem;

    return NULL;
}

// Tries to reserve space for source item.
// If item in destination slot exists it must be the item of the same entry
// and stack must have enough space to take at least one item.
// Returns false if destination item specified and it cannot be used to reserve space.
bool Guild::BankMoveItemData::_ReserveSpace(uint8 slotId, Item* pItem, Item* pItemDest, uint32& count)
{
    uint32 requiredSpace = pItem->GetMaxStackCount();
    if (pItemDest)
    {
        // Make sure source and destination items match and destination item has space for more stacks.
        if (pItemDest->GetEntry() != pItem->GetEntry() || pItemDest->GetCount() >= pItem->GetMaxStackCount())
            return false;
        requiredSpace -= pItemDest->GetCount();
    }
    // Let's not be greedy, reserve only required space
    requiredSpace = std::min(requiredSpace, count);

    // Reserve space
    ItemPosCount pos(slotId, requiredSpace);
    if (!pos.isContainedIn(m_vec))
    {
        m_vec.push_back(pos);
        count -= requiredSpace;
    }
    return true;
}

void Guild::BankMoveItemData::CanStoreItemInTab(Item* pItem, uint8 skipSlotId, bool merge, uint32& count)
{
    for (uint8 slotId = 0; (slotId < GUILD_BANK_MAX_SLOTS) && (count > 0); ++slotId)
    {
        // Skip slot already processed in CanStore (when destination slot was specified)
        if (slotId == skipSlotId)
            continue;

        Item* pItemDest = m_pGuild->GetItem(m_container, slotId);
        if (pItemDest == pItem)
            pItemDest = NULL;

        // If merge skip empty, if not merge skip non-empty
        if ((pItemDest != NULL) != merge)
            continue;

        _ReserveSpace(slotId, pItem, pItemDest, count);
    }
}

InventoryResult Guild::BankMoveItemData::CanStore(Item* pItem, bool swap)
{
    TC_LOG_DEBUG("guild", "GUILD STORAGE: CanStore() tab = %u, slot = %u, item = %u, count = %u",
        m_container, m_slotId, pItem->GetEntry(), pItem->GetCount());

    uint32 count = pItem->GetCount();
    // Soulbound items cannot be moved
    if (pItem->IsSoulBound() || pItem->IsBoundByEnchant())
        return EQUIP_ERR_DROP_BOUND_ITEM;

    // Make sure destination bank tab exists
    if (m_container >= m_pGuild->GetPurchasedTabsSize())
        return EQUIP_ERR_WRONG_BAG_TYPE;

    // Slot explicitely specified. Check it.
    if (m_slotId != NULL_SLOT)
    {
        Item* pItemDest = m_pGuild->GetItem(m_container, m_slotId);
        // Ignore swapped item (this slot will be empty after move)
        if ((pItemDest == pItem) || swap)
            pItemDest = NULL;

        if (!_ReserveSpace(m_slotId, pItem, pItemDest, count))
            return EQUIP_ERR_CANT_STACK;

        if (count == 0)
            return EQUIP_ERR_OK;
    }

    // Slot was not specified or it has not enough space for all the items in stack
    // Search for stacks to merge with
    if (pItem->GetMaxStackCount() > 1)
    {
        CanStoreItemInTab(pItem, m_slotId, true, count);
        if (count == 0)
            return EQUIP_ERR_OK;
    }

    // Search free slot for item
    CanStoreItemInTab(pItem, m_slotId, false, count);
    if (count == 0)
        return EQUIP_ERR_OK;

    return EQUIP_ERR_BANK_FULL;
}

// Guild
Guild::Guild():
    m_id(0),
    m_leaderGuid(),
    m_createdDate(0),
    m_accountsNumber(0),
    m_bankMoney(0),
    m_eventLog(NULL),
    m_newsLog(NULL),
    m_achievementMgr(new GuildAchievementMgr(this)),
    _level(1),
    _experience(0)
{
    memset(&m_bankEventLog, 0, (GUILD_BANK_MAX_TABS + 1) * sizeof(LogHolder*));
}

Guild::~Guild()
{
    CharacterDatabaseTransaction temp(nullptr);
    DeleteBankItems(temp);

    // Cleanup
    delete m_eventLog;
    m_eventLog = NULL;
    delete m_newsLog;
    m_newsLog = NULL;

    for (uint8 tabId = 0; tabId <= GUILD_BANK_MAX_TABS; ++tabId)
    {
        delete m_bankEventLog[tabId];
        m_bankEventLog[tabId] = NULL;
    }

    for (Members::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        delete itr->second;
        itr->second = NULL;
    }
}

// Creates new guild with default data and saves it to database.
bool Guild::Create(Player* pLeader, std::string const& name)
{
    // Check if guild with such name already exists
    if (sGuildMgr->GetGuildByName(name))
    {
        Guild::SendCommandResult(pLeader->GetSession(), GUILD_COMMAND_CREATE, ERR_GUILD_NAME_EXISTS_S, name);
        return false;
    }

    if (sObjectMgr->IsReservedName(name) || !ObjectMgr::IsValidCharterName(name))
    {
        Guild::SendCommandResult(pLeader->GetSession(), GUILD_COMMAND_CREATE, ERR_GUILD_NAME_INVALID, name);
        return false;
    }

    WorldSession* pLeaderSession = pLeader->GetSession();
    if (!pLeaderSession)
        return false;

    m_id = sGuildMgr->GenerateGuildId();
    m_leaderGuid = pLeader->GetGUID();
    m_name = name;
    m_info = "";
    m_motd = "No message set.";
    m_bankMoney = 0;
    m_createdDate = ::time(NULL);
    _level = 1;
    _experience = 0;
    CreateLogHolders();

    TC_LOG_DEBUG("guild", "GUILD: creating guild [%s] for leader %s (%u)",
        name.c_str(), pLeader->GetName().c_str(), m_leaderGuid.GetCounter());

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_MEMBERS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    uint8 index = 0;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD);
    stmt->setUInt32(  index, m_id);
    stmt->setString(++index, name);
    stmt->setUInt32(++index, m_leaderGuid.GetCounter());
    stmt->setString(++index, m_info);
    stmt->setString(++index, m_motd);
    stmt->setUInt64(++index, uint32(m_createdDate));
    stmt->setUInt32(++index, m_emblemInfo.GetStyle());
    stmt->setUInt32(++index, m_emblemInfo.GetColor());
    stmt->setUInt32(++index, m_emblemInfo.GetBorderStyle());
    stmt->setUInt32(++index, m_emblemInfo.GetBorderColor());
    stmt->setUInt32(++index, m_emblemInfo.GetBackgroundColor());
    stmt->setUInt64(++index, m_bankMoney);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
    CreateDefaultGuildRanks(pLeaderSession->GetSessionDbLocaleIndex()); // Create default ranks
    bool ret = AddMember(m_leaderGuid, GR_GUILDMASTER);                  // Add guildmaster

    if (ret)
    {
        Member* leader = GetMember(m_leaderGuid);
        if (leader)
            SendEventNewLeader(leader, NULL);

        sScriptMgr->OnGuildCreate(this, pLeader, name);
    }

    return ret;
}

// Disbands guild and deletes all related data from database
void Guild::Disband()
{
    // Call scripts before guild data removed from database
    sScriptMgr->OnGuildDisband(this);

    WorldPacket data(SMSG_GUILD_EVENT_DISBANDED, 0);
    BroadcastPacket(&data);

    while (!m_members.empty())
    {
        Members::const_iterator itr = m_members.begin();
        DeleteMember(itr->second->GetGUID(), true);
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_RANKS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_TABS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    // Free bank tab used memory and delete items stored in them
    DeleteBankItems(trans, true);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_ITEMS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_RIGHTS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_EVENTLOGS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_EVENTLOGS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    sGuildFinderMgr->DeleteGuild(m_id);

    sGuildMgr->RemoveGuild(m_id);
}

void Guild::SaveToDB()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_EXPERIENCE);
    stmt->setUInt32(0, GetLevel());
    stmt->setUInt64(1, GetExperience());
    stmt->setUInt32(2, GetId());
    trans->Append(stmt);

    GetAchievementMgr().SaveToDB(trans);

    CharacterDatabase.CommitTransaction(trans);
}

void Guild::UpdateMemberData(Player* player, uint8 dataid, uint32 value)
{
    if (Member* member = GetMember(player->GetGUID()))
    {
        switch (dataid)
        {
            case GUILD_MEMBER_DATA_ZONEID:
                member->SetZoneId(value);
                break;
            case GUILD_MEMBER_DATA_ACHIEVEMENT_POINTS:
                if (member->GetAchievementPoints() != value)
                {
                    member->SetAchievementPoints(value);
                    member->SaveAchievementsToDB();
                }
                break;
            case GUILD_MEMBER_DATA_LEVEL:
                member->SetLevel(value);
                break;
            case GUILD_MEMBER_DATA_PROFESSIONS:
                member->SetProfessions(player, false);
                break;
            case GUILD_MEMBER_DATA_PROFESSION_RECIPES:
                member->SetProfessions(player, true);
                member->SaveProfessionsToDB();
                UpdateGuildRecipes();
                break;
            default:
                TC_LOG_ERROR("guild", "Guild::UpdateMemberData: Called with incorrect DATAID %u (value %u)", dataid, value);
                return;
        }
        //HandleRoster();
    }
}

void Guild::OnPlayerStatusChange(Player* player, uint32 flag, bool state)
{
    if (Member* member = GetMember(player->GetGUID()))
    {
        if (state)
            member->AddFlag(flag);
        else member->RemFlag(flag);
    }
}

bool Guild::SetName(std::string const& name)
{
    // ObjectMgr::IsValidCharterName convert to wchar string and check length > 24, no need check utf8 length
    if (m_name == name || name.empty() || sObjectMgr->IsReservedName(name) || !ObjectMgr::IsValidCharterName(name))
        return false;

    m_name = name;
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_NAME);
    stmt->setString(0, m_name);
    stmt->setUInt32(1, GetId());
    CharacterDatabase.Execute(stmt);

    ObjectGuid guid = GetGUID();
    WorldPacket data(SMSG_GUILD_RENAMED, 24 + 8 + 1);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[6]);
    data.WriteBits(name.length(), 7);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);

    data.FlushBits();

    data.WriteString(name);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[4]);

    BroadcastPacket(&data);
    return true;
}

void Guild::HandleRoster(WorldSession* session /*= NULL*/)
{
    ByteBuffer memberData;

    WorldPacket data(SMSG_GUILD_ROSTER, 100);

    data.WriteBits(m_members.size(), 17);
    data.WriteBits(m_motd.length(), 10);

    for (auto&& itr : m_members)
    {
        Member* member = itr.second;
        size_t pubNoteLength = member->GetPublicNote().length();
        size_t offNoteLength = member->GetOfficerNote().length();

        ObjectGuid guid = member->GetGUID();
        data.WriteBits(offNoteLength, 8);
        data.WriteBit(guid[5]);
        data.WriteBit(0); // Can Scroll of Ressurect
        data.WriteBits(pubNoteLength, 8);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[6]);
        data.WriteBits(member->GetName().length(), 6);
        data.WriteBit(0); // Has Authenticator
        data.WriteBit(guid[3]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[2]);

        memberData << uint8(member->GetClass());
        memberData << uint32(member->GetTotalReputation());
        memberData.WriteString(member->GetName());
        memberData.WriteByteSeq(guid[0]);

        // for (2 professions)
        for (auto&& it : member->GetProfessions())
            memberData << uint32(it.Rank) << uint32(it.SkillId) << uint32(it.Value);

        memberData << uint8(member->GetLevel());
        memberData << uint8(member->GetFlags());
        memberData << uint32(member->GetZoneId());
        memberData << uint32(sWorld->getIntConfig(CONFIG_GUILD_WEEKLY_REP_CAP)); // Cap was removed, it seems unneeded but this number is always in sniffs
        memberData.WriteByteSeq(guid[3]);
        memberData << uint64(member->GetTotalActivity());
        memberData.WriteString(member->GetOfficerNote());
        memberData << float(member->IsOnline() ? 0.0f : float(::time(NULL) - member->GetLogoutTime()) / DAY);
        memberData << uint8(member->GetGender());
        memberData << uint32(member->GetRankId());
        memberData << uint32(realm.Id.Realm);
        memberData.WriteByteSeq(guid[5]);
        memberData.WriteByteSeq(guid[7]);
        memberData.WriteString(member->GetPublicNote());
        memberData.WriteByteSeq(guid[4]);
        memberData << uint64(member->GetWeekActivity());
        memberData << uint32(member->GetAchievementPoints());
        memberData.WriteByteSeq(guid[6]);
        memberData.WriteByteSeq(guid[1]);
        memberData.WriteByteSeq(guid[2]);
    }

    data.WriteBits(m_info.length(), 11);

    data.FlushBits();
    data.append(memberData);

    data << uint32(m_accountsNumber);
    data.AppendPackedTime(m_createdDate);
    data.WriteString(m_info);
    data << uint32(sWorld->getIntConfig(CONFIG_GUILD_WEEKLY_REP_CAP));
    data.WriteString(m_motd);
    data << uint32(0);

    if (session)
    {
        TC_LOG_DEBUG("guild", "SMSG_GUILD_ROSTER [%s]", session->GetPlayerInfo().c_str());
        session->SendPacket(&data);
    }
    else
    {
        TC_LOG_DEBUG("guild", "SMSG_GUILD_ROSTER [Broadcast]");
        BroadcastPacket(&data);
    }
}

void Guild::HandleQuery(WorldSession* session)
{
    ObjectGuid guid = GetGUID();

    WorldPacket data(SMSG_GUILD_QUERY_RESPONSE, 8 * 32 + 200);      // Guess size

    data.WriteBit(guid[5]);
    data.WriteBit(1); // HasData

    // if (hasData)
    {
        data.WriteBits(m_ranks.size(), 21);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[7]);

        for (auto&& it : m_ranks)
            data.WriteBits(it.GetName().length(), 7);

        data.WriteBit(guid[3]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[6]);
        data.WriteBits(m_name.length(), 7);
    }

    data.WriteBit(guid[3]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[6]);

    data.FlushBits();

    //if (hasData)
    {
        data << uint32(m_emblemInfo.GetBorderStyle());
        data << uint32(m_emblemInfo.GetStyle());

        data.WriteByteSeq(guid[2]);
        data.WriteByteSeq(guid[7]);

        data << uint32(m_emblemInfo.GetColor());
        data << uint32(realm.Id.Realm);

        for (uint8 i = 0; i < m_ranks.size(); ++i)
        {
            data << uint32(i);
            data << uint32(m_ranks[i].GetId());
            data.WriteString(m_ranks[i].GetName());
        }

        data.WriteString(m_name);
        data << uint32(m_emblemInfo.GetBackgroundColor());

        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[4]);

        data << uint32(m_emblemInfo.GetBorderColor());

        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[3]);
    }

    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[1]);

    session->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_QUERY_RESPONSE [%s]", session->GetPlayerInfo().c_str());
}

void Guild::SendGuildRankInfo(WorldSession* session) const
{
    WorldPacket data(SMSG_GUILD_RANK, 100);

    data.WriteBits(m_ranks.size(), 17);

    for (auto&& it : m_ranks)
        data.WriteBits(it.GetName().length(), 7);

    data.FlushBits();

    for (uint8 i = 0; i < m_ranks.size(); ++i)
    {
        RankInfo const* rankInfo = &m_ranks[i];
        data << uint32(rankInfo->GetId());
        data << uint32(rankInfo->GetBankMoneyPerDay());

        for (uint8 j = 0; j < GUILD_BANK_MAX_TABS; ++j)
        {
            data << uint32(rankInfo->GetBankTabSlotsPerDay(j));
            data << uint32(rankInfo->GetBankTabRights(j));
        }

        data.WriteString(rankInfo->GetName());
        data << uint32(i);
        data << uint32(rankInfo->GetRights());
    }

    session->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_RANK [%s]", session->GetPlayerInfo().c_str());
}

void Guild::HandleSetAchievementTracking(WorldSession* session, std::set<uint32> const& achievementIds)
{
    Player* player = session->GetPlayer();

    if (Member* member = GetMember(player->GetGUID()))
    {
        std::set<uint32> criteriaIds;

        for (std::set<uint32>::iterator achievementId = achievementIds.begin(); achievementId != achievementIds.end(); ++achievementId)
        {
            if (AchievementEntry const* achievement = sAchievementMgr->GetAchievement(*achievementId))
            {
                if (auto criteriaTree = sAchievementMgr->GetCriteriaTree(achievement->CriteriaTree))
                {
                    AchievementMgr::WalkCriteriaTree(criteriaTree, [&](CriteriaTreeNode const* node)
                    {
                        if (node->Criteria)
                            criteriaIds.insert(node->Criteria->Entry->ID);
                    });
                }
            }
        }

        member->SetTrackedCriteriaIds(criteriaIds);
        GetAchievementMgr().SendAllTrackedCriterias(player, member->GetTrackedCriteriaIds());
    }
}

void Guild::HandleSetMOTD(WorldSession* session, std::string const& motd)
{
    if (m_motd == motd)
        return;

    // Player must have rights to set MOTD
    if (!HasRankRight(session->GetPlayer(), GR_RIGHT_SETMOTD))
        SendCommandResult(session, GUILD_COMMAND_EDIT_MOTD, ERR_GUILD_PERMISSIONS);
    else
    {
        m_motd = motd;

        sScriptMgr->OnGuildMOTDChanged(this, motd);

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_MOTD);
        stmt->setString(0, motd);
        stmt->setUInt32(1, m_id);
        CharacterDatabase.Execute(stmt);

        SendEventMOTD(session, true);
    }
}

void Guild::HandleSetInfo(WorldSession* session, std::string const& info)
{
    if (m_info == info)
        return;

    // Player must have rights to set guild's info
    if (HasRankRight(session->GetPlayer(), GR_RIGHT_MODIFY_GUILD_INFO))
    {
        m_info = info;

        sScriptMgr->OnGuildInfoChanged(this, info);

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_INFO);
        stmt->setString(0, info);
        stmt->setUInt32(1, m_id);
        CharacterDatabase.Execute(stmt);
    }
}

void Guild::HandleSetEmblem(WorldSession* session, const EmblemInfo& emblemInfo)
{
    Player* player = session->GetPlayer();
    if (!IsLeader(player))
        SendSaveEmblemResult(session, ERR_GUILDEMBLEM_NOTGUILDMASTER); // "Only guild leaders can create emblems."
    else if (!player->HasEnoughMoney(uint64(EMBLEM_PRICE)))
        SendSaveEmblemResult(session, ERR_GUILDEMBLEM_NOTENOUGHMONEY); // "You can't afford to do that."
    else
    {
        player->ModifyMoney(-int64(EMBLEM_PRICE));

        m_emblemInfo = emblemInfo;
        m_emblemInfo.SaveToDB(m_id);

        SendSaveEmblemResult(session, ERR_GUILDEMBLEM_SUCCESS); // "Guild Emblem saved."

        HandleQuery(session);
    }

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_TABARD, 1, 0, 0, nullptr, player);
}

void Guild::HandleSetNewGuildMaster(WorldSession* session, std::string const& name, bool isDethrone)
{
    Player* player = session->GetPlayer();
    // Only the guild master can throne a new guild master
    if (!IsLeader(player))
    {
        if (!isDethrone || (time(NULL) - GetMember(GetLeaderGUID())->GetLogoutTime()) < 90 * DAY)
        {
            SendCommandResult(session, GUILD_COMMAND_CHANGE_LEADER, ERR_GUILD_PERMISSIONS);
            return;
        }
    }
    // Old GM must be a guild member
    if (Member* oldGuildMaster = GetMember(GetLeaderGUID()))
    {
        // Same for the new one
        if (Member* newGuildMaster = GetMember(name))
        {
            SetLeaderGUID(newGuildMaster);
            oldGuildMaster->ChangeRank(GR_INITIATE);
            SendEventNewLeader(newGuildMaster, oldGuildMaster, isDethrone);
        }
    }
}

void Guild::HandleSetBankTabInfo(WorldSession* session, uint8 tabId, std::string const& name, std::string const& icon)
{
    BankTab* tab = GetBankTab(tabId);
    if (!tab)
    {
        TC_LOG_ERROR("guild", "Guild::HandleSetBankTabInfo: Player %s trying to change bank tab info from unexisting tab %d.",
                       session->GetPlayerInfo().c_str(), tabId);
        return;
    }

    if (!HasRankRight(session->GetPlayer(), GR_RIGHT_MODIFY_BANK_TAB))
    {
        TC_LOG_ERROR("guild", "Guild::HandleSetBankTabInfo: Player %s is trying to modify bank tab info but doesn't have permission!",
            session->GetPlayerInfo().c_str());
        return;
    }

    tab->SetInfo(name, icon);

    WorldPacket data(SMSG_GUILD_EVENT_BANK_TAB_MODIFIED, 2 + 4 + name.size() + icon.size());
    data.WriteBits(icon.size(), 9);
    data.WriteBits(name.size(), 7);
    data.FlushBits();

    data.WriteString(name);
    data << uint32(tabId);
    data.WriteString(icon);

    BroadcastPacket(&data);
}

void Guild::HandleSetMemberNote(WorldSession* session, std::string const& note, ObjectGuid guid, bool isPublic)
{
    // Player must have rights to set public/officer note
    if (!HasRankRight(session->GetPlayer(), isPublic ? GR_RIGHT_EPNOTE : GR_RIGHT_EOFFNOTE))
        SendCommandResult(session, GUILD_COMMAND_PUBLIC_NOTE, ERR_GUILD_PERMISSIONS);
    else if (Member* member = GetMember(guid))
    {
        if (isPublic)
            member->SetPublicNote(note);
        else
            member->SetOfficerNote(note);

        ObjectGuid memberGuid = member->GetGUID();

        WorldPacket data(SMSG_GUILD_MEMBER_UPDATE_NOTE, 1 + 1 + 1 + 8 + note.length());

        data.WriteBit(memberGuid[2]);
        data.WriteBits(note.length(), 8);
        data.WriteBit(isPublic);
        data.WriteBit(memberGuid[5]);
        data.WriteBit(memberGuid[0]);
        data.WriteBit(memberGuid[4]);
        data.WriteBit(memberGuid[3]);
        data.WriteBit(memberGuid[1]);
        data.WriteBit(memberGuid[6]);
        data.WriteBit(memberGuid[7]);
        data.FlushBits();

        data.WriteByteSeq(memberGuid[7]);
        data.WriteByteSeq(memberGuid[5]);
        data.WriteByteSeq(memberGuid[0]);
        data.WriteByteSeq(memberGuid[1]);
        data.WriteString(note);
        data.WriteByteSeq(memberGuid[3]);
        data.WriteByteSeq(memberGuid[6]);
        data.WriteByteSeq(memberGuid[4]);
        data.WriteByteSeq(memberGuid[2]);

        if (session)
        {
            TC_LOG_DEBUG("guild", "SMSG_GUILD_MEMBER_UPDATE_NOTE [%s]", session->GetPlayerInfo().c_str());
            session->SendPacket(&data);
        }
        else
        {
            TC_LOG_DEBUG("guild", "SMSG_GUILD_MEMBER_UPDATE_NOTE [Broadcast]");
            BroadcastPacket(&data);
        }
    }
}

void Guild::HandleSetRankInfo(WorldSession* session, uint32 rankIndex, std::string const& name, uint32 rights, uint32 moneyPerDay, const GuildBankRightsAndSlotsVec& rightsAndSlots)
{
    // Only leader can modify ranks
    if (!IsLeader(session->GetPlayer()))
        SendCommandResult(session, GUILD_COMMAND_CHANGE_RANK, ERR_GUILD_PERMISSIONS);
    else if (RankInfo* rankInfo = GetRankInfoByIndex(rankIndex))
    {
        TC_LOG_DEBUG("guild", "Changed RankName to '%s', rights to 0x%08X", name.c_str(), rights);

        rankInfo->SetName(name);
        rankInfo->SetRights(rights);
        SetRankBankMoneyPerDay(rankInfo->GetId(), moneyPerDay);

        for (GuildBankRightsAndSlotsVec::const_iterator itr = rightsAndSlots.begin(); itr != rightsAndSlots.end(); ++itr)
            SetRankBankTabRightsAndSlots(rankInfo->GetId(), *itr);

        WorldPacket data(SMSG_GUILD_EVENT_RANK_CHANGED, 4);
        data << uint32(rankInfo->GetId());
        BroadcastPacket(&data);
    }
}

void Guild::HandleBuyBankTab(WorldSession* session, uint8 tabId)
{
    Player* player = session->GetPlayer();
    if (!player)
        return;

    Member const* member = GetMember(player->GetGUID());
    if (!member)
        return;

    if (GetPurchasedTabsSize() >= GUILD_BANK_MAX_TABS)
        return;

    if (tabId != GetPurchasedTabsSize())
        return;

    // Do not get money for bank tabs that the GM bought, we had to buy them already.
    // This is just a speedup check, GetGuildBankTabPrice will return 0.
    if (tabId < GUILD_BANK_MAX_TABS - 2) // 7th tab is actually the 6th
    {
        uint32 tabCost = _GetGuildBankTabPrice(tabId) * GOLD;
        if (!tabCost)
            return;

        if (!player->HasEnoughMoney(uint64(tabCost)))                   // Should not happen, this is checked by client
            return;

        player->ModifyMoney(-int64(tabCost));
    }

    CreateNewBankTab();

    WorldPacket data(SMSG_GUILD_EVENT_BANK_TAB_ADDED, 0);
    BroadcastPacket(&data);

    SendPermissions(session); /// Hack to force client to update permissions

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS, tabId + 1, 0, 0, nullptr, player);
}

void Guild::HandleInviteMember(WorldSession* session, std::string const& name)
{
    Player* pInvitee = ObjectAccessor::FindPlayerByName(name);
    if (!pInvitee)
    {
        SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_GUILD_PLAYER_NOT_FOUND_S, name);
        return;
    }

    Player* player = session->GetPlayer();
    // Do not show invitations from ignored players
    if (pInvitee->GetSocial()->HasIgnore(player->GetGUID()))
        return;

    if ((!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && pInvitee->GetTeam() != player->GetTeam()) || pInvitee->GetRace() == RACE_PANDAREN_NEUTRAL)
    {
        SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_GUILD_NOT_ALLIED, name);
        return;
    }

    // Invited player cannot be in another guild
    if (pInvitee->GetGuildId())
    {
        SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_ALREADY_IN_GUILD_S, name);
        return;
    }

    // Invited player cannot be invited
    if (pInvitee->GetGuildIdInvited())
    {
        SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_ALREADY_INVITED_TO_GUILD_S, name);
        return;
    }

    // Inviting player must have rights to invite
    if (!HasRankRight(player, GR_RIGHT_INVITE))
    {
        SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_GUILD_PERMISSIONS);
        return;
    }

    if (m_members.size() >= GUILD_MEMBERS_LIMIT)
    {
        SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_GUILD_INTERNAL);
        return;
    }

    SendCommandResult(session, GUILD_COMMAND_INVITE, ERR_GUILD_COMMAND_SUCCESS, name);

    TC_LOG_DEBUG("guild", "Player %s invited %s to join his Guild", player->GetName().c_str(), name.c_str());

    // Auto decline invite
    if (pInvitee->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_AUTO_DECLINE_GUILD))
    {
        player->SendDeclineGuildInvitation(pInvitee->GetName(), true);
        return;
    }

    pInvitee->SetGuildIdInvited(m_id);
    pInvitee->SetLastGuildInviterGUID(player->GetGUID());

    LogEvent(GUILD_EVENT_LOG_INVITE_PLAYER, player->GetGUID().GetCounter(), pInvitee->GetGUID().GetCounter());

    ObjectGuid oldGuildGuid;
    if (uint32 oldId = pInvitee->GetGuildId())
        oldGuildGuid = ObjectGuid(HighGuid::Guild, oldId);
    ObjectGuid newGuildGuid = GetGUID();

    WorldPacket data(SMSG_GUILD_INVITE, 100);
    data.WriteBit(newGuildGuid[4]);
    data.WriteBits(m_name.length(), 7);
    data.WriteBit(oldGuildGuid[4]);
    data.WriteBit(newGuildGuid[6]);
    data.WriteBit(oldGuildGuid[2]);
    data.WriteBit(oldGuildGuid[1]);
    data.WriteBit(oldGuildGuid[5]);
    data.WriteBit(oldGuildGuid[7]);
    data.WriteBit(newGuildGuid[0]);
    data.WriteBit(oldGuildGuid[3]);
    data.WriteBit(newGuildGuid[5]);
    data.WriteBit(oldGuildGuid[6]);
    data.WriteBits(player->GetName().length(), 6);
    data.WriteBit(newGuildGuid[1]);
    data.WriteBit(newGuildGuid[3]);
    data.WriteBit(oldGuildGuid[0]);
    data.WriteBit(newGuildGuid[7]);
    data.WriteBits(pInvitee->GetGuildName().length(), 7);
    data.WriteBit(newGuildGuid[2]);

    data.FlushBits();

    data.WriteByteSeq(newGuildGuid[1]);
    data << uint32(m_emblemInfo.GetBackgroundColor());
    data.WriteByteSeq(newGuildGuid[4]);
    data.WriteString(player->GetName());
    data << uint32(m_emblemInfo.GetBorderStyle());
    data.WriteByteSeq(oldGuildGuid[7]);
    data.WriteByteSeq(newGuildGuid[0]);
    data.WriteByteSeq(newGuildGuid[2]);
    data << uint32(m_emblemInfo.GetColor());
    data.WriteByteSeq(oldGuildGuid[2]);
    data.WriteByteSeq(oldGuildGuid[5]);
    data << uint32(GetLevel());
    data << uint32(pInvitee->GetGuildId() ? realm.Id.Realm : 0);
    data.WriteByteSeq(newGuildGuid[7]);
    data.WriteByteSeq(newGuildGuid[3]);
    data.WriteByteSeq(oldGuildGuid[4]);
    data << uint32(m_emblemInfo.GetBorderColor());
    data.WriteString(m_name);
    data << uint32(realm.Id.Realm);
    data << uint32(m_emblemInfo.GetStyle());
    data.WriteByteSeq(oldGuildGuid[0]);
    data.WriteString(pInvitee->GetGuildName());
    data.WriteByteSeq(newGuildGuid[5]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(oldGuildGuid[1]);
    data.WriteByteSeq(newGuildGuid[6]);
    data.WriteByteSeq(oldGuildGuid[3]);
    data.WriteByteSeq(oldGuildGuid[6]);

    pInvitee->GetSession()->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_INVITE [%s]", pInvitee->GetName().c_str());
}

void Guild::HandleAcceptMember(WorldSession* session)
{
    Player* player = session->GetPlayer();
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) &&
        player->GetTeam() != sObjectMgr->GetPlayerTeamByGUID(GetLeaderGUID()))
        return;

    if (m_members.size() >= GUILD_MEMBERS_LIMIT)
        return;

    AddMember(player->GetGUID());
}

void Guild::HandleLeaveMember(WorldSession* session)
{
    Player* player = session->GetPlayer();
    bool disband = false;

    // If leader is leaving
    if (IsLeader(player))
    {
        if (m_members.size() > 1)
            // Leader cannot leave if he is not the last member
            SendCommandResult(session, GUILD_COMMAND_QUIT, ERR_GUILD_LEADER_LEAVE);
        else if (GetLevel() >= sWorld->getIntConfig(CONFIG_GUILD_UNDELETABLE_LEVEL))
            SendCommandResult(session, GUILD_COMMAND_QUIT, ERR_GUILD_UNDELETABLE_DUE_TO_LEVEL);
        else
        {
            // Guild is disbanded if leader leaves.
            Disband();
            disband = true;
        }
    }
    else
    {
        LogEvent(GUILD_EVENT_LOG_LEAVE_GUILD, player->GetGUID().GetCounter());
        SendEventPlayerLeft(GetMember(player->GetGUID()));

        DeleteMember(player->GetGUID(), false, false);

        SendCommandResult(session, GUILD_COMMAND_QUIT, ERR_GUILD_COMMAND_SUCCESS, m_name);
    }

    sCalendarMgr->RemovePlayerGuildEventsAndSignups(player->GetGUID(), GetId());

    if (disband)
        delete this;
}

void Guild::HandleRemoveMember(WorldSession* session, ObjectGuid guid)
{
    Player* player = session->GetPlayer();

    // Player must have rights to remove members
    if (!HasRankRight(player, GR_RIGHT_REMOVE))
        SendCommandResult(session, GUILD_COMMAND_REMOVE, ERR_GUILD_PERMISSIONS);
    else if (Member* member = GetMember(guid))
    {
        std::string name = member->GetName();

        // Guild masters cannot be removed
        if (member->IsRank(GR_GUILDMASTER))
            SendCommandResult(session, GUILD_COMMAND_REMOVE, ERR_GUILD_LEADER_LEAVE);
        // Do not allow to remove player with the same rank or higher
        else
        {
            Member* memberMe = GetMember(player->GetGUID());
            if (!memberMe || GetRankIndex(memberMe->GetRankId()) >= GetRankIndex(member->GetRankId()))
                SendCommandResult(session, GUILD_COMMAND_REMOVE, ERR_GUILD_RANK_TOO_HIGH_S, name);
            else
            {
                LogEvent(GUILD_EVENT_LOG_UNINVITE_PLAYER, player->GetGUID().GetCounter(), guid.GetCounter());
                SendEventPlayerLeft(member, memberMe, true);

                // After call to DeleteMember pointer to member becomes invalid
                DeleteMember(guid, false, true);

                SendCommandResult(session, GUILD_COMMAND_REMOVE, ERR_GUILD_COMMAND_SUCCESS, name);
            }
        }
    }
}

void Guild::HandleUpdateMemberRank(WorldSession* session, ObjectGuid guid, bool demote)
{
    Member* member = GetMember(guid);
    if (!member)
        return;

    int32 newRankIndex = GetRankIndex(member->GetRankId()) + (demote ? 1 : -1);
    HandleSetMemberRank(session, guid, session->GetPlayer()->GetGUID(), newRankIndex);
}

void Guild::HandleSetMemberRank(WorldSession* session, ObjectGuid targetGuid, ObjectGuid setterGuid, uint32 rankIndex)
{
    if (!IsMember(targetGuid))
        return;

    Player* player = session->GetPlayer();
    Member* actor = GetMember(setterGuid);
    Member* target = GetMember(targetGuid);
    GuildRankRights rights = GR_RIGHT_PROMOTE;
    GuildCommandType type = GUILD_COMMAND_PROMOTE;

    ASSERT(actor && target);

    int32 myRankIndex = GetRankIndex(actor->GetRankId());
    int32 hisRankIndex = GetRankIndex(target->GetRankId());

    ASSERT(hisRankIndex >= 0);
    RankInfo* newRank = GetRankInfoByIndex(rankIndex);
    if (!newRank)
        return;

    // Only if the actor's rank is higher (rank index lower).
    if (myRankIndex >= hisRankIndex)
    {
        SendCommandResult(session, type, ERR_GUILD_RANK_TOO_HIGH_S, target->GetName());
        return;
    }

    if (rankIndex > uint32(hisRankIndex))
    {
        rights = GR_RIGHT_DEMOTE;
        type = GUILD_COMMAND_DEMOTE;

        // Lowest rank cannot be demoted
        if (hisRankIndex == int32(m_ranks.size()) - 1)
        {
            SendCommandResult(session, type, ERR_GUILD_RANK_TOO_LOW_S, target->GetName());
            return;
        }
    }
    else
    {
        // Allow to promote only to lower rank than member's rank
        if (myRankIndex == hisRankIndex - 1)
        {
            SendCommandResult(session, type, ERR_GUILD_RANK_TOO_HIGH_S, target->GetName());
            return;
        }
    }

    // Promoted player must be a member of guild
    if (!HasRankRight(player, rights))
    {
        SendCommandResult(session, type, ERR_GUILD_PERMISSIONS);
        return;
    }

    // Player cannot promote himself
    if (target->IsSamePlayer(player->GetGUID()))
    {
        SendCommandResult(session, type, ERR_GUILD_NAME_INVALID);
        return;
    }

    target->ChangeRank(newRank->GetId());
    LogEvent(type == GUILD_COMMAND_PROMOTE ? GUILD_EVENT_LOG_PROMOTE_PLAYER : GUILD_EVENT_LOG_DEMOTE_PLAYER, setterGuid.GetCounter(), targetGuid.GetCounter(), newRank->GetId());
    SendGuildRanksUpdate(setterGuid, target, type == GUILD_COMMAND_PROMOTE);
}

void Guild::HandleAddNewRank(WorldSession* session, std::string const& name)
{
    if (m_ranks.size() >= GUILD_RANKS_MAX_COUNT)
        return;

    // Only leader can add new rank
    if (IsLeader(session->GetPlayer()))
    {
        if (CreateRank(name, GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK))
        {
            WorldPacket data(SMSG_GUILD_EVENT_RANKS_UPDATED, 0);
            BroadcastPacket(&data);
        }
    }
}

void Guild::HandleRemoveRank(WorldSession* session, uint32 rankIndex)
{
    // Cannot remove rank if total count is minimum allowed by the client or is not leader
    if (!IsLeader(session->GetPlayer()) || m_ranks.size() <= GUILD_RANKS_MIN_COUNT)
        return;

    if (rankIndex >= m_ranks.size())
        return;

    m_ranks.erase(m_ranks.begin() + rankIndex);

    // Delete all ranks from DB and insert new.
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_RANKS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    for (size_t i = 0; i < m_ranks.size(); ++i)
        m_ranks[i].SaveToDB(trans, i);
    CharacterDatabase.CommitTransaction(trans);

    WorldPacket data(SMSG_GUILD_EVENT_RANKS_UPDATED, 0);
    BroadcastPacket(&data);
}

void Guild::HandleSwitchRank(WorldSession* session, uint32 targetIndex, bool up)
{
    if (!IsLeader(session->GetPlayer()) || targetIndex >= m_ranks.size())
        return;

    uint32 destIndex;
    if (up)
    {
        if (!targetIndex)
            return;
        destIndex = targetIndex - 1;
    }
    else 
    {
        if (targetIndex + 1 == m_ranks.size())
            return;
        destIndex = targetIndex + 1;
    }

    std::swap(m_ranks[targetIndex], m_ranks[destIndex]);

    // Delete all ranks from DB and insert new.
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_RANKS);
    stmt->setUInt32(0, m_id);
    trans->Append(stmt);

    for (size_t i = 0; i < m_ranks.size(); ++i)
        m_ranks[i].SaveToDB(trans, i);
    CharacterDatabase.CommitTransaction(trans);

    WorldPacket data(SMSG_GUILD_EVENT_RANKS_UPDATED, 0);
    BroadcastPacket(&data);
}

void Guild::HandleMemberDepositMoney(WorldSession* session, uint64 amount, bool cashFlow /*=false*/)
{
    Player* player = session->GetPlayer();

    // Call script after validation and before money transfer.
    sScriptMgr->OnGuildMemberDepositMoney(this, player, amount);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    ModifyBankMoney(trans, amount, true);
    if (!cashFlow)
    {
        player->ModifyMoney(-int64(amount));
        player->SaveGoldToDB(trans);
    }

    LogBankEvent(trans, cashFlow ? GUILD_BANK_LOG_CASH_FLOW_DEPOSIT : GUILD_BANK_LOG_DEPOSIT_MONEY, uint8(0), player->GetGUID().GetCounter(), amount);
    CharacterDatabase.CommitTransaction(trans);

    SendEventBankMoneyChanged();

    if (sWorld->getBoolConfig(CONFIG_GM_LOG_TRADE) && player->GetSession()->GetSecurity() > SEC_PLAYER)
    {
        sLog->outCommand(player->GetSession()->GetAccountId(),
            "GM %s (Account: %u) deposit money (Amount: " UI64FMTD ") to guild bank (Guild ID %u)",
            player->GetName().c_str(), player->GetSession()->GetAccountId(), amount, m_id);
    }
}

bool Guild::HandleMemberWithdrawMoney(WorldSession* session, uint64 amount, bool repair)
{
    // clamp amount to MAX_MONEY_AMOUNT, Players can't hold more than that anyway
    amount = std::min(amount, uint64(MAX_MONEY_AMOUNT));

    if (m_bankMoney < amount)                               // Not enough money in bank
        return false;

    Player* player = session->GetPlayer();

    Member* member = GetMember(player->GetGUID());
    if (!member)
        return false;

   if (uint64((GetMemberRemainingMoney(member)) * GOLD) < amount)   // Check if we have enough slot/money today
       return false;

    // Call script after validation and before money transfer.
    sScriptMgr->OnGuildMemberWitdrawMoney(this, player, amount, repair);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    // Add money to player (if required)
    if (!repair)
    {
        if (!player->ModifyMoney(amount))
            return false;

        player->SaveGoldToDB(trans);
    }

    // Update remaining money amount
    member->UpdateBankWithdrawValue(trans, GUILD_BANK_MAX_TABS, amount / GOLD);
    // Remove money from bank
    ModifyBankMoney(trans, amount, false);

    // Log guild bank event
    LogBankEvent(trans, repair ? GUILD_BANK_LOG_REPAIR_MONEY : GUILD_BANK_LOG_WITHDRAW_MONEY, uint8(0), player->GetGUID().GetCounter(), amount);
    CharacterDatabase.CommitTransaction(trans);

    SendEventBankMoneyChanged();

    if (repair)
        UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_SPENT_GOLD_GUILD_REPAIRS, amount, 0, 0, nullptr, player);

    return true;
}

void Guild::HandleMemberLogout(WorldSession* session)
{
    Player* player = session->GetPlayer();
    if (Member* member = GetMember(player->GetGUID()))
    {
        member->SetStats(player);
        member->UpdateLogoutTime();
        member->ResetFlags();
        UpdateGuildRecipes();
    }

    SendEventPresenceChanged(session, false, true);
    SaveToDB();
}

void Guild::HandleDisband(WorldSession* session)
{
    // Only leader can disband guild
    if (IsLeader(session->GetPlayer()))
    {
        Disband();
        TC_LOG_DEBUG("guild", "Guild Successfully Disbanded");
        delete this;
    }
}

void Guild::SendEventLog(WorldSession* session) const
{
    WorldPacket data(SMSG_GUILD_EVENT_LOG_QUERY_RESULT, 3 + m_eventLog->GetSize() * (1 + 9 + 9 + 4 + 4));
    m_eventLog->WritePacket(data);
    session->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_EVENT_LOG_QUERY_RESULT [%s]", session->GetPlayerInfo().c_str());
}

void Guild::SendNewsUpdate(WorldSession* session)
{
    uint32 size = m_newsLog->GetSize();
    GuildLog* logs = m_newsLog->GetGuildLog();

    if (!logs)
        return;

    WorldPacket data(SMSG_GUILD_NEWS_UPDATE, (21 + size * (26 + 8)) / 8 + (8 + 6 * 4) * size);
    data.WriteBits(size, 19);

    for (GuildLog::const_iterator itr = logs->begin(); itr != logs->end(); ++itr)
    {
        ObjectGuid guid = ((NewsLogEntry*)(*itr))->GetPlayerGuid();

        data.WriteBit(guid[3]);
        data.WriteBits(0, 24); // Not yet implemented used for guild achievements
        data.WriteBit(guid[4]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[1]);
    }

    data.FlushBits();

    for (GuildLog::const_iterator itr = logs->begin(); itr != logs->end(); ++itr)
    {
        NewsLogEntry* news = (NewsLogEntry*)(*itr);
        ObjectGuid guid = news->GetPlayerGuid();

        data.WriteByteSeq(guid[2]);
        data << uint32(news->GetValue());
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[5]);
        data << uint32(0);
        data << uint32(news->GetType());
        data << uint32(news->GetFlags());   // 1 sticky
        data.WriteByteSeq(guid[6]);
        data.AppendPackedTime(news->GetTimestamp());
        data << uint32(news->GetGUID());
        data.WriteByteSeq(guid[0]);
    }

    session->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_NEWS_UPDATE [%s]", session->GetPlayerInfo().c_str());
}

void Guild::SendBankLog(WorldSession* session, uint8 tabId) const
{
    // GUILD_BANK_MAX_TABS send by client for money log
    if (tabId < GetPurchasedTabsSize() || tabId == GUILD_BANK_MAX_TABS)
    {
        LogHolder const* log = m_bankEventLog[tabId];
        WorldPacket data(SMSG_GUILD_BANK_LOG_QUERY_RESULT, log->GetSize() * (4 * 4 + 1) + 1 + 1);
        data << uint32(tabId);
        data.WriteBit(GetLevel() >= 5 && tabId == GUILD_BANK_MAX_TABS);     // has Cash Flow perk
        log->WritePacket(data);
        if (GetLevel() >= 5 && tabId == GUILD_BANK_MAX_TABS)
            data << uint64(0);
        session->SendPacket(&data);
        TC_LOG_DEBUG("guild", "SMSG_GUILD_BANK_LOG_QUERY_RESULT [%s] TabId: %u", session->GetPlayerInfo().c_str(), tabId);
    }
}

void Guild::SendBankTabText(WorldSession* session, uint8 tabId) const
{
    if (BankTab const* tab = GetBankTab(tabId))
        tab->SendText(this, session);
}

void Guild::SendPermissions(WorldSession* session) const
{
    Member const* member = GetMember(session->GetPlayer()->GetGUID());
    if (!member)
        return;

    uint8 rankId = member->GetRankId();

    WorldPacket data(SMSG_GUILD_PERMISSIONS_QUERY_RESULTS, 4 * 15 + 1);
    data << uint32(rankId);
    data << uint32(GetRankBankMoneyPerDay(rankId));
    data << uint32(GetPurchasedTabsSize());
    data << uint32(GetRankRights(rankId));

    data.WriteBits(GUILD_BANK_MAX_TABS, 21);
    data.FlushBits();

    for (uint8 tabId = 0; tabId < GUILD_BANK_MAX_TABS; ++tabId)
    {
        data << uint32(GetMemberRemainingSlots(member, tabId));
        data << uint32(GetRankBankTabRights(rankId, tabId));
    }

    session->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_PERMISSIONS_QUERY_RESULTS [%s] Rank: %u", session->GetPlayerInfo().c_str(), rankId);
}

void Guild::SendMoneyInfo(WorldSession* session) const
{
    Member const* member = GetMember(session->GetPlayer()->GetGUID());
    if (!member)
        return;

    int32 amount = GetMemberRemainingMoney(member);
    WorldPacket data(SMSG_GUILD_BANK_MONEY_WITHDRAWN, 8);
    data << int64(amount > 0 ? int64(amount * GOLD) : amount);
    session->SendPacket(&data);
    TC_LOG_DEBUG("guild", "SMSG_GUILD_BANK_MONEY_WITHDRAWN [%s] Money: %u", session->GetPlayerInfo().c_str(), amount);
}

void Guild::SendLoginInfo(WorldSession* session)
{
    Player* player = session->GetPlayer();
    Member* member = GetMember(player->GetGUID());
    if (!member)
        return;

    /*
        Login sequence:
          SMSG_GUILD_EVENT_MOTD
          SMSG_GUILD_RANK
          SMSG_GUILD_ACHIEVEMENT_DATA
          SMSG_GUILD_MEMBER_DAILY_RESET // bank withdrawal reset
          SMSG_GUILD_REPUTATION_WEEKLY_CAP
          SMSG_GUILD_EVENT_PRESENCE_CHANGE
          -- learn perks
    */

    SendEventMOTD(session);
    SendGuildRankInfo(session);

    GetAchievementMgr().SendAllAchievementData(player);

    WorldPacket data(SMSG_GUILD_MEMBER_DAILY_RESET, 0);  // tells the client to request bank withdrawal limit
    session->SendPacket(&data);

    SendGuildReputationWeeklyCap(session);
    SendEventPresenceChanged(session, true, true);      // Broadcast

    // Send to self separately, player is not in world yet and is not found by _BroadcastEvent
    SendEventPresenceChanged(session, true);

    if (member->GetGUID() == GetLeaderGUID())
    {
        WorldPacket data(SMSG_GUILD_FLAGGED_FOR_RENAME);
        data.WriteBit(0);
        data.FlushBits();
        player->GetSession()->SendPacket(&data);
    }

    member->SetStats(player);
    member->AddFlag(GUILDMEMBER_STATUS_ONLINE);
}

void Guild::SendEventBankMoneyChanged()
{
    WorldPacket data(SMSG_GUILD_EVENT_BANK_MONEY_CHANGED, 8);
    data << m_bankMoney;
    BroadcastPacket(&data);
}

void Guild::SendEventMOTD(WorldSession* session, bool broadcast)
{
    WorldPacket data(SMSG_GUILD_EVENT_MOTD, m_motd.size() + 2);
    data.WriteBits(m_motd.size(), 10);
    data.FlushBits();
    data.WriteString(m_motd.size() > 0 ? m_motd : "");

    if (broadcast)
        BroadcastPacket(&data);
    else
    {
        session->SendPacket(&data);
        TC_LOG_DEBUG("guild", "SMSG_GUILD_EVENT_MOTD [%s] ", session->GetPlayerInfo().c_str());
    }
}

void Guild::SendEventNewLeader(Member* newLeader, Member* oldLeader, bool isSelfPromoted)
{
    ObjectGuid newLeaderGUID = newLeader ? newLeader->GetGUID() : ObjectGuid::Empty;
    std::string newLeaderName = newLeader ? newLeader->GetName() : "";
    ObjectGuid oldLeaderGUID = oldLeader ? oldLeader->GetGUID() : ObjectGuid::Empty;
    std::string oldLeaderName = oldLeader ? oldLeader->GetName() : "";

    WorldPacket data(SMSG_GUILD_EVENT_NEW_LEADER);
    data.WriteBit(newLeaderGUID[4]);
    data.WriteBit(newLeaderGUID[2]);
    data.WriteBit(newLeaderGUID[7]);
    data.WriteBit(oldLeaderGUID[4]);
    data.WriteBits(oldLeaderName.size(), 6);
    data.WriteBit(oldLeaderGUID[0]);
    data.WriteBit(newLeaderGUID[6]);
    data.WriteBit(newLeaderGUID[3]);
    data.WriteBit(isSelfPromoted);
    data.WriteBit(newLeaderGUID[1]);
    data.WriteBit(newLeaderGUID[0]);
    data.WriteBit(oldLeaderGUID[1]);
    data.WriteBit(oldLeaderGUID[7]);
    data.WriteBit(oldLeaderGUID[3]);
    data.WriteBit(oldLeaderGUID[6]);
    data.WriteBit(oldLeaderGUID[2]);
    data.WriteBits(newLeaderName.size(), 6);
    data.WriteBit(oldLeaderGUID[5]);
    data.WriteBit(newLeaderGUID[5]);

    data.FlushBits();

    data.WriteByteSeq(newLeaderGUID[5]);
    data.WriteByteSeq(newLeaderGUID[6]);
    data.WriteString(oldLeaderName);
    data.WriteString(newLeaderName);
    data.WriteByteSeq(newLeaderGUID[3]);
    data.WriteByteSeq(newLeaderGUID[4]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(oldLeaderGUID[6]);
    data.WriteByteSeq(newLeaderGUID[0]);
    data.WriteByteSeq(oldLeaderGUID[5]);
    data.WriteByteSeq(newLeaderGUID[2]);
    data.WriteByteSeq(newLeaderGUID[7]);
    data.WriteByteSeq(oldLeaderGUID[7]);
    data.WriteByteSeq(oldLeaderGUID[4]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(newLeaderGUID[1]);
    data.WriteByteSeq(oldLeaderGUID[2]);
    data.WriteByteSeq(oldLeaderGUID[1]);
    data.WriteByteSeq(oldLeaderGUID[3]);
    data.WriteByteSeq(oldLeaderGUID[0]);

    BroadcastPacket(&data);
}

void Guild::SendEventPlayerLeft(Member* leaver, Member* remover, bool isRemoved)
{
    ObjectGuid LeaverGUID = leaver->GetGUID();
    ObjectGuid RemoverGUID = remover ? remover->GetGUID() : ObjectGuid::Empty;
    std::string RemoverName = remover ? remover->GetName() : "";
    bool hasRemover = isRemoved && remover;
    bool hasRemoverVirtualAdress = hasRemover;
    bool hasRemoverName = hasRemover;

    WorldPacket data(SMSG_GUILD_EVENT_PLAYER_LEFT);
    data.WriteBit(LeaverGUID[2]);
    data.WriteBits(leaver->GetName().size(), 6);
    data.WriteBit(LeaverGUID[6]);
    data.WriteBit(LeaverGUID[5]);
    data.WriteBit(hasRemover);
    if (hasRemover)
    {
        data.WriteBit(!hasRemoverName);
        data.WriteBit(!isRemoved);
        if (hasRemoverName)
            data.WriteBits(RemoverName.size(), 6);
        data.WriteBit(RemoverGUID[1]);
        data.WriteBit(RemoverGUID[3]);
        data.WriteBit(RemoverGUID[4]);
        data.WriteBit(RemoverGUID[2]);
        data.WriteBit(RemoverGUID[5]);
        data.WriteBit(RemoverGUID[7]);
        data.WriteBit(RemoverGUID[6]);
        data.WriteBit(RemoverGUID[0]);
        data.WriteBit(!hasRemoverVirtualAdress);
    }
    data.WriteBit(LeaverGUID[1]);
    data.WriteBit(LeaverGUID[0]);
    data.WriteBit(LeaverGUID[3]);
    data.WriteBit(LeaverGUID[4]);
    data.WriteBit(LeaverGUID[7]);

    data.FlushBits();

    if (hasRemover)
    {
        data.WriteByteSeq(RemoverGUID[1]);
        data.WriteByteSeq(RemoverGUID[3]);
        data.WriteByteSeq(RemoverGUID[5]);
        data.WriteByteSeq(RemoverGUID[2]);
        data.WriteByteSeq(RemoverGUID[0]);
        data.WriteByteSeq(RemoverGUID[4]);
        data.WriteByteSeq(RemoverGUID[6]);
        data.WriteByteSeq(RemoverGUID[7]);

        data.WriteString(RemoverName);
        if (hasRemoverVirtualAdress)
            data << uint32(realm.Id.Realm);
    }
    data.WriteString(leaver->GetName());
    data.WriteByteSeq(LeaverGUID[1]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(LeaverGUID[0]);
    data.WriteByteSeq(LeaverGUID[4]);
    data.WriteByteSeq(LeaverGUID[2]);
    data.WriteByteSeq(LeaverGUID[3]);
    data.WriteByteSeq(LeaverGUID[6]);
    data.WriteByteSeq(LeaverGUID[5]);
    data.WriteByteSeq(LeaverGUID[7]);

    BroadcastPacket(&data);
}

void Guild::SendEventPresenceChanged(WorldSession* session, bool loggedOn, bool broadcast)
{
    Player* player = session->GetPlayer();
    ObjectGuid playerGuid = player->GetGUID();

    WorldPacket data(SMSG_GUILD_EVENT_PRESENCE_CHANGE);
    data.WriteBit(playerGuid[0]);
    data.WriteBit(playerGuid[6]);
    data.WriteBit(0);                 // Mobile
    data.WriteBit(playerGuid[2]);
    data.WriteBit(playerGuid[5]);
    data.WriteBit(playerGuid[3]);
    data.WriteBits(player->GetName().size(), 6);
    data.WriteBit(playerGuid[1]);
    data.WriteBit(playerGuid[7]);
    data.WriteBit(playerGuid[4]);

    data.WriteBit(loggedOn);

    data.FlushBits();

    data.WriteByteSeq(playerGuid[3]);
    data.WriteByteSeq(playerGuid[2]);
    data.WriteByteSeq(playerGuid[0]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(playerGuid[6]);

    data.WriteString(player->GetName());
    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(playerGuid[7]);
    data.WriteByteSeq(playerGuid[1]);

    if (broadcast)
        BroadcastPacket(&data);
    else
        session->SendPacket(&data);
}

// Loading methods
bool Guild::LoadFromDB(Field* fields)
{
    m_id            = fields[0].GetUInt32();
    m_name          = fields[1].GetString();
    m_leaderGuid    = ObjectGuid(HighGuid::Player, fields[2].GetUInt32());
    m_emblemInfo.LoadFromDB(fields);
    m_info          = fields[8].GetString();
    m_motd          = fields[9].GetString();
    m_createdDate   = time_t(fields[10].GetUInt32());
    m_bankMoney     = fields[11].GetUInt64();
    _level          = fields[12].GetUInt32();
    _experience     = fields[13].GetUInt64();

    uint8 purchasedTabs = uint8(fields[14].GetUInt64());
    if (purchasedTabs > GUILD_BANK_MAX_TABS)
        purchasedTabs = GUILD_BANK_MAX_TABS;

    m_bankTabs.resize(purchasedTabs);
    for (uint8 i = 0; i < purchasedTabs; ++i)
        m_bankTabs[i] = new BankTab(m_id, i);

    CreateLogHolders();
    return true;
}

void Guild::LoadRankFromDB(Field* fields)
{
    RankInfo rankInfo(m_id);

    rankInfo.LoadFromDB(fields);

    m_ranks.push_back(rankInfo);
}

bool Guild::LoadMemberFromDB(Field* fields)
{
    uint32 lowguid = fields[1].GetUInt32();
    Member *member = new Member(m_id, ObjectGuid(HighGuid::Player, lowguid), fields[2].GetUInt8());
    if (!member->LoadFromDB(fields))
    {
        DeleteMemberFromDB(lowguid);
        delete member;
        return false;
    }
    m_members[lowguid] = member;
    return true;
}

void Guild::LoadBankRightFromDB(Field* fields)
{
                                           // tabId              rights                slots
    GuildBankRightsAndSlots rightsAndSlots(fields[1].GetUInt8(), fields[3].GetUInt8(), fields[4].GetUInt32());
                                  // rankId
    SetRankBankTabRightsAndSlots(fields[2].GetUInt8(), rightsAndSlots, false);
}

bool Guild::LoadEventLogFromDB(Field* fields)
{
    if (m_eventLog->CanInsert())
    {
        m_eventLog->LoadEvent(new EventLogEntry(
            m_id,                                       // guild id
            fields[1].GetUInt32(),                      // guid
            time_t(fields[6].GetUInt32()),              // timestamp
            GuildEventLogTypes(fields[2].GetUInt8()),   // event type
            fields[3].GetUInt32(),                      // player guid 1
            fields[4].GetUInt32(),                      // player guid 2
            fields[5].GetUInt8()));                     // rank
        return true;
    }
    return false;
}

bool Guild::LoadBankEventLogFromDB(Field* fields)
{
    uint8 dbTabId = fields[1].GetUInt8();
    bool isMoneyTab = (dbTabId == GUILD_BANK_MONEY_LOGS_TAB);
    if (dbTabId < GetPurchasedTabsSize() || isMoneyTab)
    {
        uint8 tabId = isMoneyTab ? uint8(GUILD_BANK_MAX_TABS) : dbTabId;
        LogHolder* pLog = m_bankEventLog[tabId];
        if (pLog->CanInsert())
        {
            uint32 guid = fields[2].GetUInt32();
            GuildBankEventLogTypes eventType = GuildBankEventLogTypes(fields[3].GetUInt8());
            if (BankEventLogEntry::IsMoneyEvent(eventType))
            {
                if (!isMoneyTab)
                {
                    TC_LOG_ERROR("guild", "GuildBankEventLog ERROR: MoneyEvent(LogGuid: %u, Guild: %u) does not belong to money tab (%u), ignoring...", guid, m_id, dbTabId);
                    return false;
                }
            }
            else if (isMoneyTab)
            {
                TC_LOG_ERROR("guild", "GuildBankEventLog ERROR: non-money event (LogGuid: %u, Guild: %u) belongs to money tab, ignoring...", guid, m_id);
                return false;
            }
            pLog->LoadEvent(new BankEventLogEntry(
                m_id,                                   // guild id
                guid,                                   // guid
                time_t(fields[8].GetUInt32()),          // timestamp
                dbTabId,                                // tab id
                eventType,                              // event type
                fields[4].GetUInt32(),                  // player guid
                fields[5].GetUInt32(),                  // item or money
                fields[6].GetUInt16(),                  // itam stack count
                fields[7].GetUInt8()));                 // dest tab id
        }
    }
    return true;
}

void Guild::LoadGuildNewsLogFromDB(Field* fields)
{
    if (!m_newsLog->CanInsert())
        return;

    m_newsLog->LoadEvent(new NewsLogEntry(
    m_id,                                       // guild id
    fields[1].GetUInt32(),                      // guid
    fields[6].GetUInt32(),                      // timestamp //64 bits?
    GuildNews(fields[2].GetUInt8()),            // type
    ObjectGuid(HighGuid::Player, fields[3].GetUInt32()), // player guid
    fields[4].GetUInt32(),                      // Flags
    fields[5].GetUInt32()));                    // value
}

void Guild::LoadBankTabFromDB(Field* fields)
{
    uint8 tabId = fields[1].GetUInt8();
    if (tabId >= GetPurchasedTabsSize())
        TC_LOG_ERROR("guild", "Invalid tab (tabId: %u) in guild bank, skipped.", tabId);
    else
        m_bankTabs[tabId]->LoadFromDB(fields);
}

bool Guild::LoadBankItemFromDB(Field* fields)
{
    uint8 tabId = fields[19].GetUInt8();
    if (tabId >= GetPurchasedTabsSize())
    {
        TC_LOG_ERROR("guild", "Invalid tab for item (GUID: %u, id: #%u) in guild bank, skipped.",
            fields[21].GetUInt32(), fields[22].GetUInt32());
        return false;
    }
    return m_bankTabs[tabId]->LoadItemFromDB(fields);
}

// Validates guild data loaded from database. Returns false if guild should be deleted.
bool Guild::Validate()
{
    // Validate ranks data
    // GUILD RANKS represent a sequence starting from 0 = GUILD_MASTER (ALL PRIVILEGES) to max 9 (lowest privileges).
    // The lower rank id is considered higher rank - so promotion does rank-- and demotion does rank++
    // Between ranks in sequence cannot be gaps - so 0, 1, 2, 4 is impossible
    // Min ranks count is 2 and max is 10.
    bool broken_ranks = false;
    if (m_ranks.size() < GUILD_RANKS_MIN_COUNT || m_ranks.size() > GUILD_RANKS_MAX_COUNT)
    {
        TC_LOG_ERROR("guild", "Guild %u has invalid number of ranks, creating new...", m_id);
        broken_ranks = true;
    }

    if (broken_ranks)
    {
        m_ranks.clear();
        CreateDefaultGuildRanks(DEFAULT_LOCALE);
    }

    // Repair the structure of the guild.
    // If the guildmaster doesn't exist or isn't member of the guild
    // attempt to promote another member.
    Member* pLeader = GetMember(m_leaderGuid);
    if (!pLeader)
    {
        DeleteMember(m_leaderGuid);
        // If no more members left, disband guild
        if (m_members.empty())
        {
            Disband();
            return false;
        }
    }
    else if (!pLeader->IsRank(GR_GUILDMASTER))
        SetLeaderGUID(pLeader);

    // Check config if multiple guildmasters are allowed
    if (!sConfigMgr->GetBoolDefault("Guild.AllowMultipleGuildMaster", 0))
        for (auto&& it : m_members)
            if (it.second->GetRankId() == GR_GUILDMASTER && !it.second->IsSamePlayer(m_leaderGuid))
                it.second->ChangeRank(GR_OFFICER);

    UpdateAccountsNumber();
    return true;
}

// Broadcasts
void Guild::BroadcastToGuild(WorldSession* session, bool officerOnly, std::string const& msg, uint32 language) const
{
    if (session && session->GetPlayer() && HasRankRight(session->GetPlayer(), officerOnly ? GR_RIGHT_OFFCHATSPEAK : GR_RIGHT_GCHATSPEAK))
    {
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, officerOnly ? CHAT_MSG_OFFICER : CHAT_MSG_GUILD, Language(language), session->GetPlayer(), NULL, msg);
        for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
            if (Player* player = itr->second->FindPlayer())
                if (player->GetSession() && HasRankRight(player, officerOnly ? GR_RIGHT_OFFCHATLISTEN : GR_RIGHT_GCHATLISTEN) &&
                    !player->GetSocial()->HasIgnore(session->GetPlayer()->GetGUID()))
                    player->GetSession()->SendPacket(&data);
    }
}

void Guild::BroadcastAddonToGuild(WorldSession* session, bool officerOnly, std::string const& msg, std::string const& prefix) const
{
    if (session && session->GetPlayer() && HasRankRight(session->GetPlayer(), officerOnly ? GR_RIGHT_OFFCHATSPEAK : GR_RIGHT_GCHATSPEAK))
    {
        WorldPacket data;
        ChatHandler::BuildChatPacket(data, officerOnly ? CHAT_MSG_OFFICER : CHAT_MSG_GUILD, LANG_ADDON, session->GetPlayer(), NULL, msg, 0, "", DEFAULT_LOCALE, prefix);
        for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
            if (Player* player = itr->second->FindPlayer())
                if (player->GetSession() && HasRankRight(player, officerOnly ? GR_RIGHT_OFFCHATLISTEN : GR_RIGHT_GCHATLISTEN) &&
                    !player->GetSocial()->HasIgnore(session->GetPlayer()->GetGUID()) &&
                    player->GetSession()->IsAddonRegistered(prefix))
                        player->GetSession()->SendPacket(&data);
    }
}

void Guild::BroadcastPacketToRank(WorldPacket* packet, uint8 rankId) const
{
    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        if (itr->second->IsRank(rankId))
            if (Player* player = itr->second->FindPlayer())
                player->GetSession()->SendPacket(packet);
}

void Guild::BroadcastPacket(WorldPacket* packet) const
{
    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        if (Player* player = itr->second->FindPlayer())
            player->GetSession()->SendPacket(packet);
}

void Guild::BroadcastPacketIfTrackingAchievement(WorldPacket* packet, uint32 criteriaId) const
{
    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        if (itr->second->IsTrackingCriteriaId(criteriaId))
            if (Player* player = itr->second->FindPlayer())
                player->GetSession()->SendPacket(packet);
}

void Guild::MassInviteToEvent(WorldSession* session, uint32 minLevel, uint32 maxLevel, uint32 minRank)
{
    uint32 count = 0;

    ByteBuffer inviteeData;
    WorldPacket data(SMSG_CALENDAR_EVENT_INITIAL_INVITE);
    data.WriteBits(count, 23); // count placeholder

    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        // not sure if needed, maybe client checks it as well
        if (count >= CALENDAR_MAX_INVITES)
        {
            if (Player* player = session->GetPlayer())
                sCalendarMgr->SendCalendarCommandResult(player->GetGUID(), CALENDAR_ERROR_INVITES_EXCEEDED);
            return;
        }

        Member* member = itr->second;
        uint32 level = Player::GetLevelFromDB(member->GetGUID());

        if (member->GetGUID() != session->GetPlayer()->GetGUID() && level >= minLevel && level <= maxLevel && GetRankIndex(member->GetRankId()) <= GetRankIndex(minRank))
        {
            ObjectGuid guid = member->GetGUID();
            data.WriteBit(guid[1]);
            data.WriteBit(guid[7]);
            data.WriteBit(guid[5]);
            data.WriteBit(guid[0]);
            data.WriteBit(guid[4]);
            data.WriteBit(guid[3]);
            data.WriteBit(guid[6]);
            data.WriteBit(guid[2]);

            inviteeData << uint8(level);
            inviteeData.WriteByteSeq(guid[3]);
            inviteeData.WriteByteSeq(guid[5]);
            inviteeData.WriteByteSeq(guid[4]);
            inviteeData.WriteByteSeq(guid[6]);
            inviteeData.WriteByteSeq(guid[7]);
            inviteeData.WriteByteSeq(guid[0]);
            inviteeData.WriteByteSeq(guid[2]);
            inviteeData.WriteByteSeq(guid[1]);

            ++count;
        }
    }

    data.FlushBits();
    data.PutBits(0, count, 23);
    data.append(inviteeData);

    session->SendPacket(&data);
}

// Members handling
bool Guild::AddMember(ObjectGuid guid, uint8 rankId)
{
    Player* player = ObjectAccessor::FindPlayer(guid);
    // Player cannot be in guild
    if (player)
    {
        if (player->GetGuildId() != 0)
            return false;
    }
    else if (Player::GetGuildIdFromDB(guid) != 0)
        return false;

    // Remove all player signs from another petitions
    // This will be prevent attempt to join many guilds and corrupt guild data integrity
    Player::RemovePetitionsAndSigns(guid, GUILD_CHARTER_TYPE);

    uint32 lowguid = guid.GetCounter();

    // If rank was not passed, assign lowest possible rank
    if (rankId == GUILD_RANK_NONE)
        rankId = GetLowestRankId();

    Member* member = new Member(m_id, guid, rankId);
    std::string name = "";
    if (player)
    {
        m_members[lowguid] = member;
        player->SetInGuild(m_id);
        player->SetGuildIdInvited(0);
        player->SetRank(rankId);
        player->SetGuildLevel(GetLevel());
        SendLoginInfo(player->GetSession());
        name = player->GetName();
        // FIXME: Level down
        member->SetStats(player);

        if (sWorld->getBoolConfig(CONFIG_GUILD_LEVELING_ENABLED))
            for (uint32 i = 0; i < sGuildPerkSpellsStore.GetNumRows(); ++i)
                if (GuildPerkSpellsEntry const* entry = sGuildPerkSpellsStore.LookupEntry(i))
                    if (entry->Level <= GetLevel())
                        player->LearnSpell(entry->SpellId, true);
    }
    else
    {
        member->ResetFlags();

        bool ok = false;
        // Player must exist
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_DATA_FOR_GUILD);
        stmt->setUInt32(0, lowguid);
        if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
        {
            Field* fields = result->Fetch();
            name = fields[0].GetString();
            member->SetStats(
                name,
                fields[1].GetUInt8(),
                fields[2].GetUInt8(),
                fields[3].GetUInt16(),
                fields[4].GetUInt32(),
                0,
                fields[5].GetUInt8());

            ok = member->CheckStats();
        }

        if (!ok)
        {
            delete member;
            return false;
        }
        m_members[lowguid] = member;
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MEMBER_REPUTATION);
    stmt->setInt32(0, guid.GetCounter());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (result)
    {
        Field* fields = result->Fetch();
        uint32 guild = fields[0].GetUInt32();
        if (guild != GetId())
        {
            if (player)
            {
                int32 val = 0;
                if (FactionEntry const* faction = sFactionStore.LookupEntry(GUILD_REPUTATION_ID))
                {
                    ReputationRank rank = player->GetReputationMgr().GetRank(faction);
                    for (int32 r = REP_NEUTRAL; r < rank; ++r)
                        val += ReputationMgr::PointsInRank[r];

                    player->GetReputationMgr().SetReputation(faction, val);
                    member->SetReputation(player->GetReputationMgr().GetReputation(faction));
                }
            }
            else
            {
                // FIXME
            }
        }
    }
    else
    {
        if (player)
            if (FactionEntry const* faction = sFactionStore.LookupEntry(GUILD_REPUTATION_ID))
                player->GetReputationMgr().SetVisible(faction);
        member->SetReputation(0);
    }

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    member->SaveToDB(trans);
    member->SaveProfessionsToDB(trans);
    CharacterDatabase.CommitTransaction(trans);

    UpdateAccountsNumber();
    UpdateGuildRecipes();
    LogEvent(GUILD_EVENT_LOG_JOIN_GUILD, lowguid);

    ObjectGuid Guid = guid;
    WorldPacket data(SMSG_GUILD_EVENT_PLAYER_JOINED);
    data.WriteBit(Guid[6]);
    data.WriteBit(Guid[1]);
    data.WriteBit(Guid[3]);
    data.WriteBits(name.size(), 6);
    data.WriteBit(Guid[7]);
    data.WriteBit(Guid[4]);
    data.WriteBit(Guid[2]);
    data.WriteBit(Guid[5]);
    data.WriteBit(Guid[0]);
    data.WriteByteSeq(Guid[2]);
    data.WriteByteSeq(Guid[4]);
    data.WriteByteSeq(Guid[1]);
    data.WriteByteSeq(Guid[6]);
    data.WriteByteSeq(Guid[5]);
    data << uint32(realm.Id.Realm);
    data.WriteByteSeq(Guid[3]);
    data.WriteByteSeq(Guid[0]);
    data.WriteString(name);
    data.WriteByteSeq(Guid[7]);

    sGuildFinderMgr->RemoveAllMembershipRequestsFromPlayer(lowguid);

    // Call scripts if member was succesfully added (and stored to database)
    sScriptMgr->OnGuildAddMember(this, player, rankId);

    return true;
}

void Guild::DeleteMember(ObjectGuid guid, bool isDisbanding, bool isKicked, bool canDeleteGuild)
{
    uint32 lowguid = guid.GetCounter();
    Player* player = ObjectAccessor::FindPlayer(guid);

    // Guild master can be deleted when loading guild and guid doesn't exist in characters table
    // or when he is removed from guild by gm command
    if (m_leaderGuid == guid && !isDisbanding)
    {
        Member* oldLeader = NULL;
        Member* newLeader = NULL;
        for (Guild::Members::iterator i = m_members.begin(); i != m_members.end(); ++i)
        {
            if (i->first == lowguid)
                oldLeader = i->second;
            else if (!newLeader || newLeader->GetRankId() > i->second->GetRankId())
                newLeader = i->second;
        }

        if (!newLeader)
        {
            Disband();
            if (canDeleteGuild)
                delete this;
            return;
        }

        SetLeaderGUID(newLeader);

        // If player not online data in data field will be loaded from guild tabs no need to update it !!
        if (Player* newLeaderPlayer = newLeader->FindPlayer())
            newLeaderPlayer->SetRank(GR_GUILDMASTER);

        // If leader does not exist (at guild loading with deleted leader) do not send broadcasts
        if (oldLeader)
        {
            SendEventNewLeader(newLeader, oldLeader, true);
            SendEventPlayerLeft(oldLeader);
        }
    }
    // Call script on remove before member is actually removed from guild (and database)
    sScriptMgr->OnGuildRemoveMember(this, player, isDisbanding, isKicked);

    if (Member* member = GetMember(guid))
        delete member;
    m_members.erase(lowguid);

    // If player not online data in data field will be loaded from guild tabs no need to update it !!
    if (player)
    {
        player->SetInGuild(0);
        player->SetRank(0);
        player->SetGuildLevel(0);

        for (uint32 i = 0; i < sGuildPerkSpellsStore.GetNumRows(); ++i)
            if (GuildPerkSpellsEntry const* entry = sGuildPerkSpellsStore.LookupEntry(i))
                if (entry->Level <= GetLevel())
                    player->RemoveSpell(entry->SpellId, false, false);
    }

    DeleteMemberFromDB(lowguid);
    if (!isDisbanding)
        UpdateAccountsNumber();
}

bool Guild::ChangeMemberRank(ObjectGuid guid, uint32 newRank)
{
    if (newRank <= GetLowestRankId())                    // Validate rank (allow only existing ranks)
        if (Member* member = GetMember(guid))
        {
            member->ChangeRank(newRank);
            return true;
        }
    return false;
}

bool Guild::IsMember(ObjectGuid guid) const
{
    Members::const_iterator itr = m_members.find(guid.GetCounter());
    return itr != m_members.end();
}

// Bank (items move)
void Guild::SwapItems(Player* player, uint8 tabId, uint8 slotId, uint8 destTabId, uint8 destSlotId, uint32 splitedAmount)
{
    if (tabId >= GetPurchasedTabsSize() || slotId >= GUILD_BANK_MAX_SLOTS ||
        destTabId >= GetPurchasedTabsSize() || destSlotId >= GUILD_BANK_MAX_SLOTS)
        return;

    if (tabId == destTabId && slotId == destSlotId)
        return;

    BankMoveItemData from(this, player, tabId, slotId);
    BankMoveItemData to(this, player, destTabId, destSlotId);
    MoveItems(&from, &to, splitedAmount);
}

void Guild::SwapItemsWithInventory(Player* player, bool toChar, uint8 tabId, uint8 slotId, uint8 playerBag, uint8 playerSlotId, uint32 splitedAmount)
{
    if ((slotId >= GUILD_BANK_MAX_SLOTS && slotId != NULL_SLOT) || tabId >= GetPurchasedTabsSize())
        return;

    BankMoveItemData bankData(this, player, tabId, slotId);
    PlayerMoveItemData charData(this, player, playerBag, playerSlotId);
    if (toChar)
        MoveItems(&bankData, &charData, splitedAmount);
    else
        MoveItems(&charData, &bankData, splitedAmount);
}

// Private methods
void Guild::CreateLogHolders()
{
    m_eventLog = new LogHolder(sWorld->getIntConfig(CONFIG_GUILD_EVENT_LOG_COUNT));
    m_newsLog = new LogHolder(sWorld->getIntConfig(CONFIG_GUILD_NEWS_LOG_COUNT));
    for (uint8 tabId = 0; tabId <= GUILD_BANK_MAX_TABS; ++tabId)
        m_bankEventLog[tabId] = new LogHolder(sWorld->getIntConfig(CONFIG_GUILD_BANK_EVENT_LOG_COUNT));
}

void Guild::CreateNewBankTab()
{
    uint8 tabId = GetPurchasedTabsSize();                      // Next free id
    m_bankTabs.push_back(new BankTab(m_id, tabId));

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_TAB);
    stmt->setUInt32(0, m_id);
    stmt->setUInt8 (1, tabId);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_GUILD_BANK_TAB);
    stmt->setUInt32(0, m_id);
    stmt->setUInt8 (1, tabId);
    trans->Append(stmt);

    ++tabId;
    for (Ranks::iterator itr = m_ranks.begin(); itr != m_ranks.end(); ++itr)
        (*itr).CreateMissingTabsIfNeeded(tabId, trans, false);

    CharacterDatabase.CommitTransaction(trans);
}

void Guild::CreateDefaultGuildRanks(LocaleConstant loc)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_RANKS);
    stmt->setUInt32(0, m_id);
    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GUILD_BANK_RIGHTS);
    stmt->setUInt32(0, m_id);
    CharacterDatabase.Execute(stmt);

    CreateRank(sObjectMgr->GetTrinityString(LANG_GUILD_MASTER,   loc), GR_RIGHT_ALL);
    CreateRank(sObjectMgr->GetTrinityString(LANG_GUILD_OFFICER,  loc), GR_RIGHT_ALL);
    CreateRank(sObjectMgr->GetTrinityString(LANG_GUILD_VETERAN,  loc), GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
    CreateRank(sObjectMgr->GetTrinityString(LANG_GUILD_MEMBER,   loc), GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
    CreateRank(sObjectMgr->GetTrinityString(LANG_GUILD_INITIATE, loc), GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
}

bool Guild::CreateRank(std::string const& name, uint32 rights)
{
    if (m_ranks.size() >= GUILD_RANKS_MAX_COUNT)
        return false;

    uint32 newRankId = [=]() -> uint32
    {
        for (uint32 i = 0; i < 255; ++i)
        {
            bool found = true;
            for (auto&& it : m_ranks)
            {
                if (it.GetId() == i)
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                ASSERT(i > 0 || m_ranks.empty());
                return i;
            }
        }
        ASSERT(false);
        return 0;
    }();

    // Ranks represent sequence 0, 1, 2, ... where 0 means guildmaster
    RankInfo info(m_id, newRankId, name, rights, 0);
    m_ranks.push_back(info);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    info.CreateMissingTabsIfNeeded(GetPurchasedTabsSize(), trans);
    info.SaveToDB(trans, m_ranks.size() - 1);
    CharacterDatabase.CommitTransaction(trans);

    return true;
}

// Updates the number of accounts that are in the guild
// Player may have many characters in the guild, but with the same account
void Guild::UpdateAccountsNumber()
{
    // We use a set to be sure each element will be unique
    std::set<uint32> accountsIdSet;
    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        accountsIdSet.insert(itr->second->GetAccountId());

    m_accountsNumber = accountsIdSet.size();
}

// Detects if player is the guild master.
// Check both leader guid and player's rank (otherwise multiple feature with
// multiple guild masters won't work)
bool Guild::IsLeader(Player* player) const
{
    if (player->GetGUID() == m_leaderGuid)
        return true;
    if (const Member* member = GetMember(player->GetGUID()))
        return member->IsRank(GR_GUILDMASTER);
    return false;
}

void Guild::DeleteBankItems(CharacterDatabaseTransaction trans, bool removeItemsFromDB)
{
    for (uint8 tabId = 0; tabId < GetPurchasedTabsSize(); ++tabId)
    {
        m_bankTabs[tabId]->Delete(trans, removeItemsFromDB);
        delete m_bankTabs[tabId];
        m_bankTabs[tabId] = NULL;
    }
    m_bankTabs.clear();
}

bool Guild::ModifyBankMoney(CharacterDatabaseTransaction trans, uint64 amount, bool add)
{
    if (add)
        m_bankMoney += amount;
    else
    {
        // Check if there is enough money in bank.
        if (m_bankMoney < amount)
            return false;
        m_bankMoney -= amount;
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_BANK_MONEY);
    stmt->setUInt64(0, m_bankMoney);
    stmt->setUInt32(1, m_id);
    trans->Append(stmt);
    return true;
}

void Guild::SetLeaderGUID(Member* pLeader)
{
    if (!pLeader)
        return;

    m_leaderGuid = pLeader->GetGUID();
    pLeader->ChangeRank(GR_GUILDMASTER);

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GUILD_LEADER);
    stmt->setUInt32(0, m_leaderGuid.GetCounter());
    stmt->setUInt32(1, m_id);
    CharacterDatabase.Execute(stmt);
}

void Guild::SetRankBankMoneyPerDay(uint32 rankId, uint32 moneyPerDay)
{
    if (RankInfo* rankInfo = GetRankInfo(rankId))
        rankInfo->SetBankMoneyPerDay(moneyPerDay);
}

void Guild::SetRankBankTabRightsAndSlots(uint32 rankId, GuildBankRightsAndSlots rightsAndSlots, bool saveToDB)
{
    if (rightsAndSlots.GetTabId() >= GetPurchasedTabsSize())
        return;

    if (RankInfo* rankInfo = GetRankInfo(rankId))
        rankInfo->SetBankTabSlotsAndRights(rightsAndSlots, saveToDB);
}

inline std::string Guild::GetRankName(uint32 rankId) const
{
    if (RankInfo const* rankInfo = GetRankInfo(rankId))
        return rankInfo->GetName();
    return "<unknown>";
}

inline uint32 Guild::GetRankRights(uint32 rankId) const
{
    if (RankInfo const* rankInfo = GetRankInfo(rankId))
        return rankInfo->GetRights();
    return 0;
}

inline int32 Guild::GetRankBankMoneyPerDay(uint32 rankId) const
{
    if (RankInfo const* rankInfo = GetRankInfo(rankId))
        return rankInfo->GetBankMoneyPerDay();
    return 0;
}

inline int32 Guild::GetRankBankTabSlotsPerDay(uint32 rankId, uint8 tabId) const
{
    if (tabId < GetPurchasedTabsSize())
        if (RankInfo const* rankInfo = GetRankInfo(rankId))
            return rankInfo->GetBankTabSlotsPerDay(tabId);
    return 0;
}

inline int8 Guild::GetRankBankTabRights(uint32 rankId, uint8 tabId) const
{
    if (RankInfo const* rankInfo = GetRankInfo(rankId))
        return rankInfo->GetBankTabRights(tabId);
    return 0;
}

inline int32 Guild::GetMemberRemainingSlots(Member const* member, uint8 tabId) const
{
    if (member)
    {
        uint32 rankId = member->GetRankId();
        if (rankId == GR_GUILDMASTER)
            return GUILD_WITHDRAW_SLOT_UNLIMITED;
        if ((GetRankBankTabRights(rankId, tabId) & GUILD_BANK_RIGHT_VIEW_TAB) != 0)
        {
            int32 remaining = GetRankBankTabSlotsPerDay(rankId, tabId) - member->GetBankWithdrawValue(tabId);
            if (remaining > 0)
                return remaining;
        }
    }
    return 0;
}

inline int32 Guild::GetMemberRemainingMoney(Member const* member) const
{
    if (member)
    {
        uint32 rankId = member->GetRankId();
        if (rankId == GR_GUILDMASTER)
            return GUILD_WITHDRAW_MONEY_UNLIMITED;

        if ((GetRankRights(rankId) & (GR_RIGHT_WITHDRAW_REPAIR | GR_RIGHT_WITHDRAW_GOLD)) != 0)
        {
            int32 remaining = GetRankBankMoneyPerDay(rankId) - member->GetBankWithdrawValue(GUILD_BANK_MAX_TABS);
            if (remaining > 0)
                return remaining;
        }
    }
    return 0;
}

inline void Guild::UpdateMemberWithdrawSlots(CharacterDatabaseTransaction trans, ObjectGuid guid, uint8 tabId)
{
    if (Member* member = GetMember(guid))
    {
        uint8 rankId = member->GetRankId();
        if (rankId != GR_GUILDMASTER
            && member->GetBankWithdrawValue(tabId) < GetRankBankTabSlotsPerDay(rankId, tabId))
            member->UpdateBankWithdrawValue(trans, tabId, 1);
    }
}

inline bool Guild::MemberHasTabRights(ObjectGuid guid, uint8 tabId, uint32 rights) const
{
    if (const Member* member = GetMember(guid))
    {
        // Leader always has full rights
        if (member->IsRank(GR_GUILDMASTER) || m_leaderGuid == guid)
            return true;
        return (GetRankBankTabRights(member->GetRankId(), tabId) & rights) == rights;
    }
    return false;
}

// Add new event log record
inline void Guild::LogEvent(GuildEventLogTypes eventType, ObjectGuid::LowType playerGuid1, ObjectGuid::LowType playerGuid2, uint8 newRank)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    m_eventLog->AddEvent(trans, new EventLogEntry(m_id, m_eventLog->GetNextGUID(), eventType, playerGuid1, playerGuid2, newRank));
    CharacterDatabase.CommitTransaction(trans);

    sScriptMgr->OnGuildEvent(this, uint8(eventType), playerGuid1, playerGuid2, newRank);
}

// Add new bank event log record
void Guild::LogBankEvent(CharacterDatabaseTransaction trans, GuildBankEventLogTypes eventType, uint8 tabId, uint32 lowguid, uint32 itemOrMoney, uint16 itemStackCount, uint8 destTabId)
{
    if (tabId > GUILD_BANK_MAX_TABS)
        return;

    // not logging moves within the same tab
    if (eventType == GUILD_BANK_LOG_MOVE_ITEM && tabId == destTabId)
        return;

    uint8 dbTabId = tabId;
    if (BankEventLogEntry::IsMoneyEvent(eventType))
    {
        tabId = GUILD_BANK_MAX_TABS;
        dbTabId = GUILD_BANK_MONEY_LOGS_TAB;
    }
    LogHolder* pLog = m_bankEventLog[tabId];
    pLog->AddEvent(trans, new BankEventLogEntry(m_id, pLog->GetNextGUID(), eventType, dbTabId, lowguid, itemOrMoney, itemStackCount, destTabId));

    sScriptMgr->OnGuildBankEvent(this, uint8(eventType), tabId, lowguid, itemOrMoney, itemStackCount, destTabId);
}

inline Item* Guild::GetItem(uint8 tabId, uint8 slotId) const
{
    if (const BankTab* tab = GetBankTab(tabId))
        return tab->GetItem(slotId);
    return NULL;
}

inline void Guild::RemoveItem(CharacterDatabaseTransaction trans, uint8 tabId, uint8 slotId)
{
    if (BankTab* pTab = GetBankTab(tabId))
        pTab->SetItem(trans, slotId, NULL);
}

void Guild::MoveItems(MoveItemData* pSrc, MoveItemData* pDest, uint32 splitedAmount)
{
    // 1. Initialize source item
    if (!pSrc->InitItem())
        return; // No source item

    // 2. Check source item
    if (!pSrc->CheckItem(splitedAmount))
        return; // Source item or splited amount is invalid
    /*
    if (pItemSrc->GetCount() == 0)
    {
        TC_LOG_FATAL("guild", "Guild::SwapItems: Player %s(GUIDLow: %u) tried to move item %u from tab %u slot %u to tab %u slot %u, but item %u has a stack of zero!",
            player->GetName(), player->GetGUIDLow(), pItemSrc->GetEntry(), tabId, slotId, destTabId, destSlotId, pItemSrc->GetEntry());
        //return; // Commented out for now, uncomment when it's verified that this causes a crash!!
    }
    // */

    // 3. Check destination rights
    if (!pDest->HasStoreRights(pSrc))
        return; // Player has no rights to store item in destination

    // 4. Check source withdraw rights
    if (!pSrc->HasWithdrawRights(pDest))
        return; // Player has no rights to withdraw items from source

    // 5. Check split
    if (splitedAmount)
    {
        // 5.1. Clone source item
        if (!pSrc->CloneItem(splitedAmount))
            return; // Item could not be cloned

        // 5.2. Move splited item to destination
        DoItemsMove(pSrc, pDest, true, splitedAmount);
    }
    else // 6. No split
    {
        // 6.1. Try to merge items in destination (pDest->GetItem() == NULL)
        if (!DoItemsMove(pSrc, pDest, false)) // Item could not be merged
        {
            // 6.2. Try to swap items
            // 6.2.1. Initialize destination item
            if (!pDest->InitItem())
                return;

            // 6.2.2. Check rights to store item in source (opposite direction)
            if (!pSrc->HasStoreRights(pDest))
                return; // Player has no rights to store item in source (opposite direction)

            if (!pDest->HasWithdrawRights(pSrc))
                return; // Player has no rights to withdraw item from destination (opposite direction)

            // 6.2.3. Swap items (pDest->GetItem() != NULL)
            DoItemsMove(pSrc, pDest, true);
        }
    }
    // 7. Send changes
    SendBankContentUpdate(pSrc, pDest);
}

bool Guild::DoItemsMove(MoveItemData* pSrc, MoveItemData* pDest, bool sendError, uint32 splitedAmount)
{
    Item* pDestItem = pDest->GetItem();
    bool swap = (pDestItem != NULL);

    Item* pSrcItem = pSrc->GetItem(splitedAmount);
    // 1. Can store source item in destination
    if (!pDest->CanStore(pSrcItem, swap, sendError))
        return false;

    // 2. Can store destination item in source
    if (swap)
        if (!pSrc->CanStore(pDestItem, true, true))
            return false;

    // GM LOG (@todo move to scripts)
    pDest->LogAction(pSrc, splitedAmount);
    if (swap)
        pSrc->LogAction(pDest, splitedAmount);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    // 3. Log bank events
    pDest->LogBankEvent(trans, pSrc, pSrcItem->GetCount());
    if (swap)
        pSrc->LogBankEvent(trans, pDest, pDestItem->GetCount());

    // 4. Remove item from source
    pSrc->RemoveItem(trans, pDest, splitedAmount);

    // 5. Remove item from destination
    if (swap)
        pDest->RemoveItem(trans, pSrc);

    // 6. Store item in destination
    pDest->StoreItem(trans, pSrcItem);

    // 7. Store item in source
    if (swap)
        pSrc->StoreItem(trans, pDestItem);

    CharacterDatabase.CommitTransaction(trans);
    return true;
}

void Guild::UpdateGuildRecipes()
{
    m_guildRecipes.clear();

    for (auto&& it : m_members)
        if (Member* member = it.second)
            UpdateGuildRecipesFor(member);
}

void Guild::UpdateGuildRecipesFor(Member* member)
{
    for (auto&& prof : member->GetProfessions())
    {
        uint32 skillId = prof.SkillId;

        if (!skillId || prof.Recipes.empty())
            continue;

        std::vector<uint8>& grecipes = m_guildRecipes[skillId];
        if (grecipes.size() != 300)
            grecipes.resize(300, 0);

        for (uint32 i = 0; i < 300; ++i)
            grecipes[i] |= prof.Recipes[i];
    }
}

void Guild::SendBankContentUpdate(MoveItemData* pSrc, MoveItemData* pDest) const
{
    ASSERT(pSrc->IsBank() || pDest->IsBank());

    uint8 tabId = 0;
    SlotIds slots;
    if (pSrc->IsBank()) // B ->
    {
        tabId = pSrc->GetContainer();
        slots.insert(pSrc->GetSlotId());
        if (pDest->IsBank()) // B -> B
        {
            // Same tab - add destination slots to collection
            if (pDest->GetContainer() == pSrc->GetContainer())
                pDest->CopySlots(slots);
            else // Different tabs - send second message
            {
                SlotIds destSlots;
                pDest->CopySlots(destSlots);
                SendBankContentUpdate(pDest->GetContainer(), destSlots);
            }
        }
    }
    else if (pDest->IsBank()) // C -> B
    {
        tabId = pDest->GetContainer();
        pDest->CopySlots(slots);
    }

    SendBankContentUpdate(tabId, slots);
}

void Guild::SendBankContentUpdate(uint8 tabId, SlotIds slots) const
{
    if (BankTab const* tab = GetBankTab(tabId))
    {
        ByteBuffer slotsData;
        WorldPacket data(SMSG_GUILD_BANK_LIST, 500);                                  // guess size
        data << uint32(tabId);
        data << uint64(m_bankMoney);

        size_t pos = data.wpos();
        data << int32(0);                                                             // item withdrawal limit placeholder
        data.WriteBit(0);                                                             // not a full update
        data.WriteBits(0, 21);                                                        // tab count (doesn't need to be sent here)

        data.WriteBits(slots.size(), 18);                                             // item count

        for (auto&& slot : slots)
        {
            Item const* tabItem = tab->GetItem(slot);

            slotsData << uint32(tabItem ? tabItem->GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS) : 0);
            slotsData << uint32(0);

            uint32 enchants = 0;
            if (tabItem)
            {
                for (uint32 i = 0; i < MAX_GEM_SOCKETS; ++i)
                {
                    if (uint32 gemId = tabItem->GetEnchantmentId(EnchantmentSlot(SOCK_ENCHANTMENT_SLOT + i)))
                    {
                        slotsData << uint32(i);
                        slotsData << uint32(gemId);
                        ++enchants;
                    }
                }
            }

            data.WriteBit(0);                                                         // Locked
            data.WriteBits(enchants, 21);

            slotsData << uint32(tabItem ? tabItem->GetEnchantmentId(EnchantmentSlot(PERM_ENCHANTMENT_SLOT)) : 0);

            if (tabItem)
                tabItem->BuildItemDynamicModifiersData(&slotsData);
            else
                slotsData << uint32(0);

            slotsData << uint32(tabItem ? tabItem->GetEntry() : 0);
            slotsData << uint32(tabItem ? abs(tabItem->GetSpellCharges()) : 0);       // Spell charges
            slotsData << uint32(tabItem ? tabItem->GetCount() : 0);
            slotsData << uint32(slot);
            slotsData << int32(tabItem ? tabItem->GetItemRandomPropertyId() : 0);
            slotsData << uint32(tabItem ? tabItem->GetItemSuffixFactor() : 0);        // SuffixFactor
        }

        data.FlushBits();

        data.append(slotsData);

        for (auto&& member : m_members)
            if (MemberHasTabRights(member.second->GetGUID(), tabId, GUILD_BANK_RIGHT_VIEW_TAB))
                if (Player* player = member.second->FindPlayer())
                {
                    data.put<int32>(pos, int32(GetMemberRemainingSlots(member.second, tabId)));
                    player->GetSession()->SendPacket(&data);
                }

        TC_LOG_DEBUG("guild", "WORLD: Sent (SMSG_GUILD_BANK_LIST)");
    }
}

void Guild::SendBankList(WorldSession* session, uint8 tabId, bool withContent, bool withTabInfo) const
{
    Member const* member = GetMember(session->GetPlayer()->GetGUID());
    if (!member) // Shouldn't happen, just in case
        return;

    uint32 itemCount = 0;
    ByteBuffer slotsData;
    ByteBuffer tabData;
    WorldPacket data(SMSG_GUILD_BANK_LIST, 500);
    data << uint32(tabId);
    data << uint64(m_bankMoney);
    data << int32(GetMemberRemainingSlots(member, tabId));
    data.WriteBit(withContent && withTabInfo);
    data.WriteBits(withTabInfo ? GetPurchasedTabsSize() : 0, 21);

    size_t slotsCountPos = data.bitwpos();
    data.WriteBits(itemCount, 18);

    if (withTabInfo)
    {
        for (uint8 i = 0; i < GetPurchasedTabsSize(); ++i)
        {
            data.WriteBits(m_bankTabs[i]->GetIcon().length(), 9);
            data.WriteBits(m_bankTabs[i]->GetName().length(), 7);

            tabData << uint32(i);
            tabData.WriteString(m_bankTabs[i]->GetIcon());
            tabData.WriteString(m_bankTabs[i]->GetName());
        }
    }

    if (withContent && MemberHasTabRights(session->GetPlayer()->GetGUID(), tabId, GUILD_BANK_RIGHT_VIEW_TAB))
    {
        if (BankTab const* tab = GetBankTab(tabId))
        {
            for (uint8 slotId = 0; slotId < GUILD_BANK_MAX_SLOTS; ++slotId)
            {
                Item* tabItem = tab->GetItem(slotId);
                if (!tabItem)
                    continue;

                slotsData << uint32(tabItem->GetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS));
                slotsData << uint32(0);

                uint32 enchants = 0;
                for (uint32 i = 0; i < MAX_GEM_SOCKETS; ++i)
                {
                    if (uint32 gemId = tabItem->GetEnchantmentId(EnchantmentSlot(SOCK_ENCHANTMENT_SLOT + i)))
                    {
                        slotsData << uint32(i);
                        slotsData << uint32(gemId);
                        ++enchants;
                    }
                }

                data.WriteBit(0);                                         // Locked
                data.WriteBits(enchants, 21);

                slotsData << uint32(tabItem->GetEnchantmentId(EnchantmentSlot(PERM_ENCHANTMENT_SLOT)));
                tabItem->BuildItemDynamicModifiersData(&slotsData);
                slotsData << uint32(tabItem->GetEntry());
                slotsData << uint32(abs(tabItem->GetSpellCharges()));     // Spell charges
                slotsData << uint32(tabItem->GetCount());
                slotsData << uint32(slotId);
                slotsData << int32(tabItem->GetItemRandomPropertyId());
                slotsData << uint32(tabItem->GetItemSuffixFactor());      // SuffixFactor

                ++itemCount;
            }
        }
    }

    data.FlushBits();

    data.append(slotsData);
    data.append(tabData);

    data.PutBits(slotsCountPos, itemCount, 18);

    session->SendPacket(&data);

    TC_LOG_DEBUG("guild", "WORLD: Sent (SMSG_GUILD_BANK_LIST)");
}

void Guild::SendGuildRanksUpdate(ObjectGuid setterGuid, Member* member, bool promote)
{
    ObjectGuid tarGuid = member->GetGUID();
    ObjectGuid setGuid = setterGuid;

    WorldPacket data(SMSG_GUILD_RANKS_UPDATE, 100);
    data.WriteBit(tarGuid[5]);
    data.WriteBit(tarGuid[6]);
    data.WriteBit(setGuid[0]);
    data.WriteBit(setGuid[1]);
    data.WriteBit(tarGuid[3]);
    data.WriteBit(setGuid[4]);
    data.WriteBit(tarGuid[2]);
    data.WriteBit(setGuid[6]);
    data.WriteBit(setGuid[3]);
    data.WriteBit(setGuid[7]);
    data.WriteBit(tarGuid[4]);
    data.WriteBit(tarGuid[0]);
    data.WriteBit(tarGuid[1]);
    data.WriteBit(setGuid[2]);
    data.WriteBit(tarGuid[7]);
    data.WriteBit(promote); // 1 == higher, 0 = lower?
    data.WriteBit(setGuid[5]);

    data.FlushBits();

    data.WriteByteSeq(tarGuid[2]);
    data.WriteByteSeq(setGuid[1]);
    data.WriteByteSeq(tarGuid[6]);
    data.WriteByteSeq(tarGuid[1]);
    data.WriteByteSeq(tarGuid[5]);
    data.WriteByteSeq(setGuid[0]);
    data << uint32(member->GetRankId());
    data.WriteByteSeq(setGuid[3]);
    data.WriteByteSeq(setGuid[7]);
    data.WriteByteSeq(tarGuid[7]);
    data.WriteByteSeq(setGuid[2]);
    data.WriteByteSeq(tarGuid[3]);
    data.WriteByteSeq(tarGuid[4]);
    data.WriteByteSeq(setGuid[6]);
    data.WriteByteSeq(setGuid[5]);
    data.WriteByteSeq(tarGuid[0]);
    data.WriteByteSeq(setGuid[4]);
    BroadcastPacket(&data);

    TC_LOG_DEBUG("network", "SMSG_GUILD_RANKS_UPDATE [Broadcast] Target: %u, Issuer: %u, RankId: %u",
        member->GetGUID().GetCounter(), setterGuid.GetCounter(), member->GetRankId());
}

void Guild::GiveXP(uint32 xp, Player* source)
{
    if (!sWorld->getBoolConfig(CONFIG_GUILD_LEVELING_ENABLED))
        return;

    /// @todo: Award reputation and count activity for player

    if (GetLevel() >= sWorld->getIntConfig(CONFIG_GUILD_MAX_LEVEL))
        xp = 0; // SMSG_GUILD_XP_GAIN is always sent, even for no gains

    WorldPacket data(SMSG_GUILD_XP_GAIN, 8);
    data << uint64(xp);

    if (source)
        source->GetSession()->SendPacket(&data);

    _experience += xp;

    if (!xp)
        return;

    uint32 oldLevel = GetLevel();

    // Ding, mon!
    while (GetExperience() >= sGuildMgr->GetXPForGuildLevel(GetLevel()) && GetLevel() < sWorld->getIntConfig(CONFIG_GUILD_MAX_LEVEL))
    {
        _experience -= sGuildMgr->GetXPForGuildLevel(GetLevel());
        LevelUp(oldLevel, source);
    }
}

void Guild::LevelUp(uint32 oldLevel, Player *source)
{
    ++_level;

    // Find all guild perks to learn
    std::vector<uint32> perksToLearn;

    for (uint32 i = 0; i < sGuildPerkSpellsStore.GetNumRows(); ++i)
        if (GuildPerkSpellsEntry const* entry = sGuildPerkSpellsStore.LookupEntry(i))
            if (entry->Level > oldLevel && entry->Level <= GetLevel())
                perksToLearn.push_back(entry->SpellId);

    // Notify all online players that guild level changed and learn perks
    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        if (Player* player = itr->second->FindPlayer())
        {
            player->SetGuildLevel(GetLevel());

            for (size_t i = 0; i < perksToLearn.size(); ++i)
                player->LearnSpell(perksToLearn[i], true);
        }
    }

    AddGuildNews(GUILD_NEWS_LEVEL_UP, ObjectGuid::Empty, 0, _level);
    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_REACH_GUILD_LEVEL, GetLevel(), 0, 0, NULL, source);

    ++oldLevel;
}

void Guild::SendGuildXP(WorldSession* session /* = NULL */) const
{
    Member const* member = GetMember(session->GetGUID());

    WorldPacket data(SMSG_GUILD_XP, 32);
    data << uint64(GetExperience());
    data << uint64(member ? member->GetWeekActivity() : 0);
    data << uint64(member ? member->GetTotalActivity() : 0);
    data << uint64(sGuildMgr->GetXPForGuildLevel(GetLevel()) - GetExperience());    // XP missing for next level
    session->SendPacket(&data);
}

void Guild::SendGuildReputationWeeklyCap(WorldSession* session) const
{
    WorldPacket data(SMSG_GUILD_REPUTATION_WEEKLY_CAP, 4);
    data << uint32(sWorld->getIntConfig(CONFIG_GUILD_WEEKLY_REP_CAP));
    session->SendPacket(&data);
}

void Guild::ResetTimes(bool weekly)
{
    for (Members::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        itr->second->ResetValues(weekly);
        if (Player* player = itr->second->FindPlayer())
        {
            //SendGuildXP(player->GetSession());
            WorldPacket data(SMSG_GUILD_MEMBER_DAILY_RESET, 0);  // tells the client to request bank withdrawal limit
            player->GetSession()->SendPacket(&data);
        }
    }
}

void Guild::AddGuildNews(uint8 type, ObjectGuid guid, uint32 flags, uint32 value)
{
    NewsLogEntry* news = new NewsLogEntry(m_id, m_newsLog->GetNextGUID(), GuildNews(type), guid, flags, value);

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    m_newsLog->AddEvent(trans, news);
    CharacterDatabase.CommitTransaction(trans);

    WorldPacket data(SMSG_GUILD_NEWS_UPDATE, 7 + 32);
    data.WriteBits(1, 19); // size, we are only sending 1 news here
    ByteBuffer buffer;
    news->WritePacket(data, buffer);

    BroadcastPacket(&data);
}

bool Guild::HasAchieved(uint32 achievementId) const
{
    return GetAchievementMgr().HasAchieved(achievementId);
}

void Guild::UpdateAchievementCriteria(AchievementCriteriaTypes type, uint64 miscValue1, uint64 miscValue2, uint64 miscValue3, Unit* unit, Player* player)
{
    GetAchievementMgr().UpdateAchievementCriteria(type, miscValue1, miscValue2, miscValue3, unit, player);
}

void Guild::RewardReputation(Player* player, float val)
{
    if (auto member = GetMember(player->GetGUID()))
    {
        if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(GUILD_REPUTATION_ID))
        {
            // Or just multipla
            AddPct(val, player->GetTotalAuraModifier(SPELL_AURA_MOD_REPUTATION_GAIN));
            player->GetReputationMgr().ModifyReputation(factionEntry, val);
            member->SetReputation(player->GetReputationMgr().GetReputation(GUILD_REPUTATION_ID));
        }
    }
}

void Guild::LoadGuildChallenges(Field* fields)
{

    for (int32 i = CHALLENGE_DUNGEON; i < CHALLENGE_MAX; ++i)
        m_guildChallenges[i] = fields[i].GetUInt8();
}

void Guild::ResetGuildChallenges(CharacterDatabaseTransaction trans)
{
    // FIXME: m_guildChallenges[0] is useless
    for (int32 i = CHALLENGE_NONE; i < CHALLENGE_MAX; ++i)
        m_guildChallenges[i] = 0;

    SaveChallengesToDB(trans);
}

void Guild::CompleteGuildChallenge(GuildChallengeType type, Player* player)
{
    ASSERT(type < CHALLENGE_MAX);

    GuildChallengeRewardData const& reward = sObjectMgr->GetGuildChallengeRewardData();
    uint32 maxCount = reward[type].ChallengeCount;
    uint32 curCount = m_guildChallenges[type];
    if (curCount >= maxCount)
        return;

    uint32 exp = GetLevel() < sWorld->getIntConfig(CONFIG_GUILD_MAX_LEVEL) ? reward[type].Experience : 0;
    uint64 gold = GetLevel() >= sWorld->getIntConfig(CONFIG_GUILD_MAX_LEVEL) ? reward[type].MaxLevelGold : reward[type].Gold;

    m_guildChallenges[type]++;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    ModifyBankMoney(trans, gold * 10000, true);

    GiveXP(exp, nullptr);

    Group* group = player->GetGroup();
    ASSERT(group);
    for (auto&& member : *group)
        if (member->GetGuildId() == GetId() && member->IsInMap(player))
            RewardReputation(member, exp / 360.0f);

    SaveChallengesToDB(trans);

    CharacterDatabase.CommitTransaction(trans);

    WorldPacket data{ SMSG_GUILD_CHALLENGE_COMPLETED, 5 * 4 };

    data << uint32(maxCount);
    data << uint32(exp);
    data << uint32(m_guildChallenges[type]);
    data << uint32(gold);
    data << uint32(type);

    BroadcastPacket(&data);

    // Achievements
    GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE, 1, 0, 0, nullptr, player);
    GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_GUILD_CHALLENGE_TYPE, type, 1, 0, nullptr, player);
}

void Guild::SaveChallengesToDB(CharacterDatabaseTransaction trans)
{
    bool standalone = trans.get() == nullptr;
    if (standalone)
        trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_GUILD_CHALLENGE);
    stmt->setUInt32(0, GetId());
    for (int32 i = CHALLENGE_DUNGEON; i < CHALLENGE_MAX; ++i)
        stmt->setUInt8(i, m_guildChallenges[i]);

    trans->Append(stmt);

    if (standalone)
        CharacterDatabase.CommitTransaction(trans);
}

void Guild::HandleNewsSetSticky(WorldSession* session, uint32 newsId, bool sticky)
{
    GuildLog* logs = m_newsLog->GetGuildLog();
    GuildLog::iterator itr = logs->begin();
    while (itr != logs->end() && (*itr)->GetGUID() != newsId)
        ++itr;

    if (itr == logs->end())
    {
        TC_LOG_DEBUG("guild", "HandleNewsSetSticky: [%s] requested unknown newsId %u - Sticky: %u",
            session->GetPlayerInfo().c_str(), newsId, sticky);
        return;
    }

    NewsLogEntry* news = (NewsLogEntry*)(*itr);
    news->SetSticky(sticky);

    TC_LOG_DEBUG("guild", "HandleNewsSetSticky: [%s] chenged newsId %u sticky to %u",
        session->GetPlayerInfo().c_str(), newsId, sticky);

    WorldPacket data(SMSG_GUILD_NEWS_UPDATE, 7 + 32);
    data.WriteBits(1, 19);
    ByteBuffer buffer;
    news->WritePacket(data, buffer);
    session->SendPacket(&data);
}

void Guild::HandleSetBankTabNote(WorldSession* session, uint32 tabId, std::string note)
{
    BankTab* bankTab = GetBankTab(tabId);
    if (!bankTab)
    {
        TC_LOG_ERROR("guild", "Guild::HandleSetBankTabNote: Player %s is trying to modify bank tab note for non existing tab %d.",
            session->GetPlayerInfo().c_str(), tabId);
        return;
    }

    if (!HasRankRight(session->GetPlayer(), GR_RIGHT_MODIFY_BANK_TAB))
    {
        TC_LOG_ERROR("guild", "Guild::HandleSetBankTabNote: Player %s is trying to modify a bank tab note but doesn't have permission!",
            session->GetPlayerInfo().c_str());
        return;
    }

    bankTab->SetText(note);

    WorldPacket data(SMSG_GUILD_EVENT_BANK_TAB_TEXT_CHANGED, 4);
    data << tabId;
    BroadcastPacket(&data);
}

void Guild::HandleQueryGuildRecipes(WorldSession* session)
{
    WorldPacket data(SMSG_GUILD_RECIPES);
    data.WriteBits(m_guildRecipes.size(), 15);

    data.FlushBits();

    for (auto&& it : m_guildRecipes)
    {
        data.append(it.second.data(), 300);
        data << uint32(it.first);
    }
    session->SendPacket(&data);
}

void Guild::HandleQueryGuildMembersForRecipe(WorldSession* session, uint32 spellId, uint32 skillId, uint32 skillValue)
{
    SkillLineEntry const* skill = sSkillLineStore.LookupEntry(skillId);
    if (!skill || !skill->CanLink)
        return;

    uint32 byte = 0, bit = 0;
    for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
    {
        if (SkillLineAbilityEntry const* ability = sSkillLineAbilityStore.LookupEntry(j))
        {
            if (ability->SkillLine == skillId && ability->Spell == spellId)
            {
                byte = ability->bitOrder / 8;
                if (byte < 300)
                {
                    bit = ability->bitOrder & 0x80000007;
                    if (bit < 0)
                        bit = (bit & ~0xFF) | (((bit - 1) | 0xF8) + 1);
                }
                break;
            }
        }
    }

    ASSERT(byte < 300);

    std::vector<uint64> guids;
    for (auto&& it : m_members)
        for (auto&& prof : it.second->GetProfessions())
            if (prof.SkillId == skillId && prof.Recipes[byte] & (1 << bit))
                guids.push_back(it.second->GetGUID());

    WorldPacket data{ SMSG_GUILD_MEMBERS_FOR_RECIPE, guids.size() * 9 };
    data << uint32(skillId) << uint32(spellId);
    data.WriteBits(guids.size(), 24);
    for (auto&& it : guids)
    {
        ObjectGuid guid(it);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[6]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[3]);
    }

    data.FlushBits();

    for (auto&& it : guids)
    {
        ObjectGuid guid(it);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[4]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[3]);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[6]);
        data.WriteByteSeq(guid[2]);
    }
    session->SendPacket(&data);
}

void Guild::HandleQueryGuildMemberRecipes(WorldSession* session, ObjectGuid memberGuid, uint32 skillId)
{
    GetMember(session->GetPlayer()->GetGUID())->SetStats(session->GetPlayer());
    UpdateGuildRecipes();

    Member const* member = GetMember(memberGuid);
    if (!member)
        return;

    for (auto&& prof : member->GetProfessions())
    {
        if (prof.SkillId != skillId)
            continue;

        ObjectGuid guid = member->GetGUID();

        WorldPacket data(SMSG_GUILD_MEMBER_RECIPES, 1 + 8 + 4 + 300 + 4 + 4);

        data.WriteBit(guid[3]);
        data.WriteBit(guid[2]);
        data.WriteBit(guid[4]);
        data.WriteBit(guid[1]);
        data.WriteBit(guid[7]);
        data.WriteBit(guid[0]);
        data.WriteBit(guid[5]);
        data.WriteBit(guid[6]);

        data << uint32(prof.Value);
        data.WriteByteSeq(guid[0]);
        data.WriteByteSeq(guid[3]);
        data << uint32(prof.Rank);
        data.WriteByteSeq(guid[7]);
        data.WriteByteSeq(guid[5]);
        data.WriteByteSeq(guid[1]);
        data.WriteByteSeq(guid[2]);
        data.append(prof.Recipes.data(), 300);
        data.WriteByteSeq(guid[6]);
        data << uint32(prof.SkillId);
        data.WriteByteSeq(guid[4]);

        session->SendPacket(&data);
        return;
    }
}

void Guild::HandleRequestChallengeUpdate(WorldSession* session)
{
    WorldPacket data{ SMSG_GUILD_CHALLENGE_UPDATED, 4 * 6 * 5 };
    auto& rewards = sObjectMgr->GetGuildChallengeRewardData();
    ASSERT(rewards.size() == CHALLENGE_MAX);

    for (auto&& itr : rewards)
        data << uint32(itr.ChallengeCount);

    for (auto&& itr : rewards)
        data << uint32(itr.Gold);

    for (auto&& itr : rewards)
        data << uint32(itr.MaxLevelGold);

    for (auto&& itr : rewards)
        data << uint32(itr.Experience);

    for (auto&& itr : m_guildChallenges)
        data << uint32(itr);

    session->SendPacket(&data);
}
