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

#include "ServiceBoost.h"
#include "WorldSession.h"
#include "BattlePayMgr.h"
#include "Player.h"
#include "ServiceMgr.h"
#include "Realm.h"

void LoadBoostItems()
{
    uint32 oldMSTime = getMSTime();

    mBoostItemsMap.clear();                                // need for reload case

    //                                                0                 1       2
    QueryResult result = WorldDatabase.Query("SELECT specialization, slot - 1, itemId FROM battle_pay_boost_items ORDER BY specialization, slot ASC");
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 boost items. DB table `battle_pay_boost_items` is empty.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        BoostItems* items = new BoostItems();

        items->spec = fields[0].GetUInt32();
        items->slot = fields[1].GetUInt32();
        items->itemId = fields[2].GetUInt32();

        if (!sObjectMgr->GetItemTemplate(items->itemId))
        {
            TC_LOG_ERROR("sql.sql", "Item %u specified in `battle_pay_boost_items` does not exist, skipped.", items->itemId);
            continue;
        }

        mBoostItemsMap.push_back(items);
        count++;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u boost items in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}

CharacterBooster::CharacterBooster(WorldSession* session) : m_session(session), m_timer(0), m_boosting(false), m_sendPacket(false) { }

void SetBoosting(WorldSession* session, uint32 accountId, bool boost)
{
    if (!accountId && !session)
        return;

    if (sWorld->getBoolConfig(CONFIG_BOOST_PROMOTION) && !boost)
    {
        // to be impl new here
    }

    uint32 counter = 0;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BOOST);
    stmt->setUInt32(0, accountId);
    stmt->setUInt32(1, realm.Id.Realm);
    if (PreparedQueryResult result = LoginDatabase.Query(stmt))
    {
        Field* fields = result->Fetch();
        counter = fields[0].GetUInt32();
    }

    if (!boost)
    {
        if (session && counter <= 1)
            session->SetBoost(false);

        counter--;
    }
    else
    {
        if (session)
            session->SetBoost(boost);
        counter++;
    }

    if (counter > 0)
    {
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_BOOST);
        stmt->setUInt32(0, accountId);
        stmt->setUInt32(1, realm.Id.Realm);
        stmt->setUInt32(2, counter);
    }
    else
    {
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ACCOUNT_BOOST);
        stmt->setUInt32(0, accountId);
        stmt->setUInt32(1, realm.Id.Realm);
    }
    LoginDatabase.Execute(stmt);
}

void CharacterBooster::_GetCharBoostItems(PreparedItemsMap& itemsToMail, PreparedItemsMap& itemsToEquip) const
{
    switch (m_charBoostInfo.specialization)
    {
        case SPEC_MAGE_ARCANE:
        case SPEC_MAGE_FIRE:
        case SPEC_MAGE_FROST:
        case SPEC_PRIEST_DISCIPLINE:
        case SPEC_PRIEST_HOLY:
        case SPEC_PRIEST_SHADOW:
        case SPEC_WARLOCK_AFFLICTION:
        case SPEC_WARLOCK_DEMONOLOGY:
        case SPEC_WARLOCK_DESTRUCTION:
            itemsToMail.emplace(0, 82590);
            break;
    }  

    if(m_charBoostInfo.allianceFaction)
        itemsToMail.emplace(1, 25472);
    else
        itemsToMail.emplace(1, 25474);

    for (auto&& item : mBoostItemsMap)
        if (item->spec == m_charBoostInfo.specialization)
            itemsToEquip.emplace(item->slot, item->itemId);
}

void CharacterBooster::SendCharBoostPacket(PreparedItemsMap items) const
{
    ObjectGuid guid = m_charBoostInfo.charGuid;
    WorldPacket data(SMSG_CHARACTER_UPGRADE_COMPLETE, 8 + 3 + items.size());

    data.WriteBit(guid[2]);
    data.WriteBit(guid[0]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[4]);
    data.WriteBit(guid[1]);
    data.WriteBits(items.size(), 22);
    data.WriteBit(guid[6]);

    data.FlushBits();

    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);

    for (auto&& item : items)
        data << uint32(item.second);

    data.WriteByteSeq(guid[0]);
    data.WriteByteSeq(guid[1]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[4]);


    GetSession()->SendPacket(&data);
}

void CharacterBooster::LearnNonExistedSpell(CharacterDatabaseTransaction trans, uint32 spell) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_SPELL);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    stmt->setUInt32(1, spell);
    if (!CharacterDatabase.Query(stmt))
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SPELL);
        stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(1, spell);
        stmt->setUInt32(2, 1);
        stmt->setUInt32(3, 0);
        trans->Append(stmt);
    }
}

void CharacterBooster::LearnNonExistedSkill(CharacterDatabaseTransaction trans, uint32 skill) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SKILL_BOOST);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    stmt->setUInt32(1, skill);

    if (!CharacterDatabase.Query(stmt))
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SKILLS);
        stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(1, skill);
        stmt->setUInt32(2, 600);
        stmt->setUInt32(3, 600);
        trans->Append(stmt);
    }
}

uint32 CharacterBooster::_PrepareMail(CharacterDatabaseTransaction trans, std::string const subject, std::string const body) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL);
    uint32 mailId = sObjectMgr->GenerateMailID();

    stmt->setUInt32(0, mailId);
    stmt->setUInt8(1, MAIL_NORMAL);
    stmt->setInt8(2, MAIL_STATIONERY_DEFAULT);
    stmt->setUInt16(3, 0);
    stmt->setUInt32(4, m_charBoostInfo.charGuid.GetCounter());
    stmt->setUInt32(5, m_charBoostInfo.charGuid.GetCounter());
    stmt->setString(6, subject);
    stmt->setString(7, body);
    stmt->setBool(8, true);
    stmt->setUInt64(9, time(NULL) + 180 * DAY);
    stmt->setUInt64(10, time(NULL));
    stmt->setUInt32(11, 0);
    stmt->setUInt32(12, 0);
    stmt->setUInt8(13, 0);
    trans->Append(stmt);

    return mailId;
}

void CharacterBooster::_SendMail(CharacterDatabaseTransaction trans, PreparedItemsMap items) const
{
    if (items.empty())
        return;

    MailTemplateEntry const* mailTemplateEntry = sMailTemplateStore.LookupEntry(MAIL_CHARRACTER_BOOST_EQUIPED_ITEMS_BODY);
    if (!mailTemplateEntry) // should never happen
        return;

    uint32 mailId = _PrepareMail(trans, mailTemplateEntry->subject, mailTemplateEntry->content);
    CharacterDatabasePreparedStatement* stmt = NULL;

    for (auto&& itr : items)
    {
        if (Item* item = Item::CreateItem(itr.second, 1, 0))
        {
            item->SaveToDB(trans);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL_ITEM);
            stmt->setUInt32(0, mailId);
            stmt->setUInt32(1, item->GetGUID().GetCounter());
            stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
            trans->Append(stmt);
        }
        else
            TC_LOG_ERROR("sql.sql", "Can't create item %u for _SendMail in Boost. Skip.", itr.second);
    }
}

void CharacterBooster::_PrepareInventory(CharacterDatabaseTransaction trans) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_INVENTORY_BOOST);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return;

    MailTemplateEntry const* mailTemplateEntry = sMailTemplateStore.LookupEntry(MAIL_CHARRACTER_BOOST_EQUIPED_ITEMS_BODY);
    if (!mailTemplateEntry) // should never happen
        return;

    uint32 mailId = _PrepareMail(trans, mailTemplateEntry->subject, mailTemplateEntry->content);

    uint32 itemCount = 0;
    do
    {
        if (itemCount > 11)
        {
            itemCount = 0;
            mailId = _PrepareMail(trans, mailTemplateEntry->subject, mailTemplateEntry->content);
        }

        uint32 itemGuid = (*result)[0].GetUInt32();

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL_ITEM);
        stmt->setUInt32(0, mailId);
        stmt->setUInt32(1, itemGuid);
        stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
        trans->Append(stmt);

        itemCount++;
    } while (result->NextRow());

    // unequip after sending
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_INVENTORY_BOOST);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    trans->Append(stmt);

    // move or create hearthstone to first slot
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_HEARTHSTONE_BOOST);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    if (!CharacterDatabase.Query(stmt))
    {
        if (Item* item = Item::CreateItem(ITEM_HEARTHSTONE, 1, 0))
        {
            item->SaveToDB(trans);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_INVENTORY);
            stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
            stmt->setUInt32(1, 0);
            stmt->setUInt8(2, INVENTORY_SLOT_ITEM_START);
            stmt->setUInt32(3, item->GetGUID().GetCounter());
            trans->Append(stmt);
        }
    }
    else
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_HEARTHSTONE_BOOST);
        stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
        trans->Append(stmt);
    }

    // insert food to second slot
    if (Item* item = Item::CreateItem(ITEM_LEMON_FLAVOUR_PUDING, 20, 0))
    {
        item->SaveToDB(trans);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_INVENTORY);
        stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(1, 0);
        stmt->setUInt8(2, INVENTORY_SLOT_ITEM_START + 1);
        stmt->setUInt32(3, item->GetGUID().GetCounter());
        trans->Append(stmt);
    }

    // insert bag in inventory slots
    uint8 slot = INVENTORY_SLOT_BAG_START;
    for (uint8 i = 0; i < 4; i++)
    {
        if (Item* item = Item::CreateItem(ITEM_EMBERSILK_BAG, 1, 0))
        {
            item->SaveToDB(trans);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_INVENTORY);
            stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
            stmt->setUInt32(1, 0);
            stmt->setUInt8(2, slot);
            stmt->setUInt32(3, item->GetGUID().GetCounter());
            trans->Append(stmt);
        }
        slot++;
    }
}

std::string CharacterBooster::_SetSpecialization(CharacterDatabaseTransaction trans, uint8 const classId) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_TALENT);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        do
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_BY_SPELL);
            stmt->setUInt32(0, (*result)[0].GetUInt32());
            stmt->setUInt32(1, m_charBoostInfo.charGuid.GetCounter());
            trans->Append(stmt);
        } while (result->NextRow());
    }

    for (auto&& spec : dbc::GetClassSpecializations(classId))
    {
        if (std::vector<uint32> const* spells = dbc::GetSpecializetionSpells(spec))
        {
            for (auto&& spell : *spells)
            {
                stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_BY_SPELL);
                stmt->setUInt32(0, spell);
                stmt->setUInt32(1, m_charBoostInfo.charGuid.GetCounter());
                trans->Append(stmt);
            }
        }
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_TALENT);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
    trans->Append(stmt);

    std::ostringstream talentTree;
    talentTree << m_charBoostInfo.specialization << " 0 ";
    return talentTree.str();
}

void CharacterBooster::_LearnSpells(CharacterDatabaseTransaction trans) const
{
    std::vector<uint32> spellsToLearn = { SPELL_ARTISAN_RIDING, SPELL_COLD_WHEATHER_FLYING, SPELL_FLIGHT_MASTER_LICENSE, SPELL_WISDOM_OF_FOUR_WINDS };
    spellsToLearn.push_back(m_charBoostInfo.allianceFaction ? SPELL_SWIFT_PURPLE_GRYPGON : SPELL_SWIFT_PURPLE_WIND_RIDER);

    for (auto&& spell : spellsToLearn)
        LearnNonExistedSpell(trans, spell);
}

void CharacterBooster::_GetBoostedCharacterData(uint8& raceId, uint8& classId, uint8& level) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_RACE);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        raceId = (*result)[0].GetUInt8();
        if (raceId == RACE_PANDAREN_NEUTRAL)
            raceId = m_charBoostInfo.allianceFaction ? RACE_PANDAREN_ALLIANCE : RACE_PANDAREN_HORDE;
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_CLASS);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
        classId = (*result)[0].GetUInt8();

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_LEVEL);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
        level = (*result)[0].GetUInt8();
}

std::string CharacterBooster::_EquipItems(CharacterDatabaseTransaction trans, PreparedItemsMap itemsToEquip) const
{
    std::ostringstream items;
    CharacterDatabasePreparedStatement* stmt;
    for (uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)
    {
        auto itr = itemsToEquip.find(i);
        if (itr != itemsToEquip.end())
        {
            if (Item* item = Item::CreateItem(itr->second, 1, 0))
            {
                item->SaveToDB(trans);

                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_INVENTORY);
                stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());
                stmt->setUInt32(1, 0);
                stmt->setUInt8(2, itr->first);
                stmt->setUInt32(3, item->GetGUID().GetCounter());
                trans->Append(stmt);

                items << (itr->second) << " 0 ";
            }
            else
            {
                TC_LOG_ERROR("sql.sql", "Can't create item %u for _EquipItems in Boost. Skip.", itr->second);

                items << "0 0 ";
            }
        }
        else
            items << "0 0 ";
    }

    for (uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        items << ITEM_EMBERSILK_BAG << " 0 ";

    return items.str();
}

void CharacterBooster::_SaveBoostedChar(CharacterDatabaseTransaction trans, std::string items, uint8 const raceId, uint8 const classId) const
{
    float const* position = m_charBoostInfo.allianceFaction ? startPosition[1] : startPosition[0];

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_FOR_BOOST);
    stmt->setUInt8(0, raceId);
    stmt->setFloat(1, position[0]);
    stmt->setFloat(2, position[1]);
    stmt->setFloat(3, position[2]);
    stmt->setFloat(4, position[3]);
    stmt->setUInt16(5, MAP_VALE_OF_ETERNAL_BLOSSOMS);
    stmt->setString(6, _SetSpecialization(trans, classId));
    stmt->setUInt16(7, AT_LOGIN_FIRST);
    stmt->setString(8, items);
    stmt->setUInt32(9, m_charBoostInfo.charGuid.GetCounter());
    trans->Append(stmt);
}

void CharacterBooster::_LearnVeteranBonuses(CharacterDatabaseTransaction trans, uint8 const classId) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SKILLS_BOOST);
    stmt->setUInt32(0, m_charBoostInfo.charGuid.GetCounter());

    std::vector<uint32> primarySkills;
    bool fistAidBoosted = false;
    bool cookingBoosted = false;
    bool fishingBoosted = false;
    bool archaeologyBoosted = false;
    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        do
        {
            uint32 skillId = (*result)[0].GetUInt16();
            if (skillId == SKILL_BLACKSMITHING || skillId == SKILL_LEATHERWORKING || skillId == SKILL_ALCHEMY || skillId == SKILL_HERBALISM ||
                skillId == SKILL_MINING || skillId == SKILL_TAILORING || skillId == SKILL_ENGINEERING || skillId == SKILL_ENCHANTING ||
                skillId == SKILL_SKINNING || skillId == SKILL_JEWELCRAFTING || skillId == SKILL_INSCRIPTION)
                primarySkills.push_back(skillId);
            if (skillId == SKILL_FIRST_AID)
                fistAidBoosted = true;
            if (skillId == SKILL_COOKING)
                cookingBoosted = true;
            if (skillId == SKILL_FISHING)
                fishingBoosted = true;
            if (skillId == SKILL_ARCHAEOLOGY)
                archaeologyBoosted = true;
        } while (result->NextRow());
    }

    for (auto&& skill : primarySkills)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UDP_CHAR_SKILLS);
        stmt->setUInt32(0, 600);
        stmt->setUInt32(1, 600);
        stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(3, skill);
        trans->Append(stmt);

        uint32 spell = 0;
        switch (skill)
        {
            case SKILL_BLACKSMITHING:  spell = SPELL_BLACKSMITHING;  break;
            case SKILL_LEATHERWORKING: spell = SPELL_LEATHERWORKING; break;
            case SKILL_ALCHEMY:        spell = SPELL_ALCHEMY;        break;
            case SKILL_HERBALISM:      spell = SPELL_HERBALISM;      break;
            case SKILL_MINING:         spell = SPELL_MINING;         break;
            case SKILL_TAILORING:      spell = SPELL_TAILORING;      break;
            case SKILL_ENGINEERING:    spell = SPELL_ENGINEERING;    break;
            case SKILL_ENCHANTING:     spell = SPELL_ENCHANTING;     break;
            case SKILL_SKINNING:       spell = SPELL_SKINNING;       break;
            case SKILL_JEWELCRAFTING:  spell = SPELL_JEWELCRAFTING;  break;
            case SKILL_INSCRIPTION:    spell = SPELL_INSCRIPTION;    break;
        }
        LearnNonExistedSpell(trans, spell);
    }

    if (fistAidBoosted)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UDP_CHAR_SKILLS);
        stmt->setUInt32(0, 600);
        stmt->setUInt32(1, 600);
        stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(3, SKILL_FIRST_AID);
        trans->Append(stmt);
        LearnNonExistedSpell(trans, SPELL_FIRST_AID);
    }

    if (cookingBoosted)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UDP_CHAR_SKILLS);
        stmt->setUInt32(0, 600);
        stmt->setUInt32(1, 600);
        stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(3, SKILL_COOKING);
        trans->Append(stmt);
        LearnNonExistedSpell(trans, SPELL_COOKING);
    }

    if (fishingBoosted)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UDP_CHAR_SKILLS);
        stmt->setUInt32(0, 600);
        stmt->setUInt32(1, 600);
        stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(3, SKILL_FISHING);
        trans->Append(stmt);
        LearnNonExistedSpell(trans, SPELL_FISHING);
    }

    if (archaeologyBoosted)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UDP_CHAR_SKILLS);
        stmt->setUInt32(0, 600);
        stmt->setUInt32(1, 600);
        stmt->setUInt32(2, m_charBoostInfo.charGuid.GetCounter());
        stmt->setUInt32(3, SKILL_ARCHAEOLOGY);
        trans->Append(stmt);
        LearnNonExistedSpell(trans, SPELL_ARCHAEOLOGY);
    }

    if (primarySkills.empty())
    {
        std::vector<std::pair<uint32, uint32>> skillsAndSpells;
        switch (classId)
        {
            case CLASS_PRIEST:
            case CLASS_MAGE:
            case CLASS_WARLOCK:
                skillsAndSpells.emplace_back(SKILL_ENCHANTING, SPELL_ENCHANTING);
                skillsAndSpells.emplace_back(SKILL_TAILORING, SPELL_TAILORING);
                break;
            case CLASS_ROGUE:
            case CLASS_MONK:
            case CLASS_DRUID:
            case CLASS_HUNTER:
            case CLASS_SHAMAN:
                skillsAndSpells.emplace_back(SKILL_LEATHERWORKING, SPELL_LEATHERWORKING);
                skillsAndSpells.emplace_back(SKILL_SKINNING, SPELL_SKINNING);
                break;
            case CLASS_WARRIOR:
            case CLASS_PALADIN:
            case CLASS_DEATH_KNIGHT:
                skillsAndSpells.emplace_back(SKILL_BLACKSMITHING, SPELL_BLACKSMITHING);
                skillsAndSpells.emplace_back(SKILL_MINING, SPELL_MINING);
                break;
        }
        for (auto&& itr : skillsAndSpells)
        {
            LearnNonExistedSkill(trans, itr.first);
            LearnNonExistedSpell(trans, itr.second);
        }
    }

    if (!fistAidBoosted)
    {
        LearnNonExistedSkill(trans, SKILL_FIRST_AID);
        LearnNonExistedSpell(trans, SPELL_FIRST_AID);
    }

    if (!cookingBoosted)
    {
        LearnNonExistedSkill(trans, SKILL_COOKING);
        LearnNonExistedSpell(trans, SPELL_COOKING);
    }

    if (!fishingBoosted)
    {
        LearnNonExistedSkill(trans, SKILL_FISHING);
        LearnNonExistedSpell(trans, SPELL_FISHING);
    }

    if (!archaeologyBoosted)
    {
        LearnNonExistedSkill(trans, SKILL_ARCHAEOLOGY);
        LearnNonExistedSpell(trans, SPELL_ARCHAEOLOGY);
    }
}

void CharacterBooster::_HandleCharacterBoost() const
{
    if (!GetSession()->HasBoost())
        return;

    if (sWorld->getBoolConfig(CONFIG_BOOST_PROMOTION))
    {
        auto paid = LoginDatabase.PQuery("SELECT counter FROM account_boost WHERE id = '%d' AND realmid = '%d' AND counter > 0", GetSession()->GetAccountId(), realm.Id.Realm);
        if (!paid)
            return;
    }

    uint8 raceId = 0, classId = 0, level = 0;
    _GetBoostedCharacterData(raceId, classId, level);
    if (!raceId || !classId || !level)
        return;

    if (ChrSpecializationEntry const* specEntry = sChrSpecializationStore.LookupEntry(m_charBoostInfo.specialization))
        if (classId != specEntry->classId)
            return;

    PreparedItemsMap itemsToMail, itemsToEquip;
    _GetCharBoostItems(itemsToMail, itemsToEquip);
    if (itemsToEquip.empty())
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    _PrepareInventory(trans);
    _SendMail(trans, itemsToMail);
    _LearnSpells(trans);
    _SaveBoostedChar(trans, _EquipItems(trans, itemsToEquip), raceId, classId);
    if (level >= 60)
        _LearnVeteranBonuses(trans, classId);
    if (raceId == RACE_GOBLIN || raceId == RACE_WORGEN || raceId == RACE_PANDAREN_ALLIANCE || raceId == RACE_PANDAREN_HORDE || classId == CLASS_DEATH_KNIGHT)
        sServiceMgr->AddSpecificPlayerData(m_charBoostInfo.charGuid, 0, raceId, classId, nullptr, true, false);
    CharacterDatabase.CommitTransaction(trans);
    SetBoosting(GetSession(), GetSession()->GetAccountId(), false);
    SendCharBoostPacket(itemsToEquip);

    auto data = sWorld->GetCharacterNameData(m_charBoostInfo.charGuid);
    sServiceMgr->ExecutedServices(m_charBoostInfo.charGuid, SERVICE_TYPE_BOOST, std::string("Boosted name: ") + (data ? data->m_name : std::string("error")), "");
}

void CharacterBooster::HandleCharacterBoost()
{
    if (!m_charBoostInfo.charGuid)
        return;

    switch (m_charBoostInfo.action)
    {
        case CHARACTER_BOOST_ITEMS:
            sBattlePayMgr->SendBattlePayDistributionUpdate(GetSession(), BATTLE_PAY_SERVICE_BOOST, m_charBoostInfo.action);
            m_charBoostInfo.action = CHARACTER_BOOST_APPLIED;
            m_timer = 500;
            m_sendPacket = true;
            break;
        case CHARACTER_BOOST_APPLIED:
            sBattlePayMgr->SendBattlePayDistributionUpdate(GetSession(), BATTLE_PAY_SERVICE_BOOST, m_charBoostInfo.action);
            m_charBoostInfo = CharacterBoostData();
            break;
        default:
            break;
    }
}

void CharacterBooster::SetBoostedCharInfo(ObjectGuid guid, uint32 action, uint32 specialization, bool allianceFaction)
{
    m_boosting = true;
    m_charBoostInfo.charGuid = guid;
    m_charBoostInfo.action = action;
    m_charBoostInfo.specialization = specialization;
    m_charBoostInfo.allianceFaction = allianceFaction;
}

void CharacterBooster::Update(uint32 diff)
{
    if (m_sendPacket)
    {
        if (m_timer <= diff)
        {
            m_sendPacket = false;
            _HandleCharacterBoost();
        }
        else
            m_timer -= diff;
    }
}
