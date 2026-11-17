/*
* This file is part of the Pandaria 5.4.8 Project. See THANKS file for Copyright information
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

#include "ServiceMgr.h"
#include "Group.h"
#include "Player.h"
#include "Battleground.h"
#include "Chat.h"
#include "AchievementMgr.h"
#include "Config.h"
#include "Guild.h"

ServiceMgr::ServiceHandler ServiceMgr::_serviceMethods[ISERVICE_END] = 
{
    nullptr,
    &ServiceMgr::HandleAntiError,
    &ServiceMgr::HandleReplaceSkill,
    &ServiceMgr::HandleChangeClass,
};

// so hard, but we have array within array...for such few values sql usage doesn't seem suitable
ServiceMgr::ProfSkill ServiceMgr::_profs[11] =
{
    { 164, { {5, 2018 }, {10, 3100 }, {20, 3538 }, {35, 9785 }, {50, 29844}, {60, 51300}, {70, 76666}, {80, 110396} } },   // Blacksmithing
    { 165, { {5, 2108 }, {10, 3104 }, {20, 3811 }, {35, 10662}, {50, 32549}, {65, 51302}, {75, 81199}, {85, 110423} } },   // Leatherworking
    { 171, { {5, 2259 }, {10, 3101 }, {20, 3464 }, {35, 11611}, {50, 28596}, {65, 51304}, {75, 80731}, {85, 105206} } },   // Alchemy
    { 182, { {5, 2366 }, {10, 2368 }, {10, 3570 }, {25, 11993}, {50, 28695}, {55, 50300}, {65, 74519}, {75, 110413} } },   // Herbalist
    { 186, { {5, 2575 }, {10, 2576 }, {10, 3564 }, {25, 10248}, {0,  29354}, {0,  50310}, {0,  74517}, {0,  102161} } },   // Mining
    { 197, { {5, 3908 }, {10, 3909 }, {20, 3910 }, {35, 12180}, {50, 26790}, {65, 51309}, {75, 75156}, {85, 110426} } },   // Tailoring
    { 202, { {5, 4036 }, {10, 4037 }, {20, 4038 }, {35, 12656}, {50, 30350}, {65, 51306}, {75, 82774}, {85, 110403} } },   // Engineering
    { 333, { {5, 7411 }, {10, 7412 }, {20, 7413 }, {35, 13920}, {50, 28029}, {65, 51313}, {75, 74258}, {85, 110400} } },   // Enchanting
    { 393, { {0, 8613 }, {0,  8617 }, {10, 8618 }, {25, 10768}, {40, 32678}, {55, 50305}, {65, 74522}, {75, 102216} } },   // Skinning
    { 755, { {5, 25229}, {10, 25230}, {20, 28894}, {35, 28895}, {50, 28897}, {60, 51311}, {70, 73318}, {80, 110420} } },   // Jewelcrafting
    { 773, { {5, 45357}, {10, 45358}, {20, 45359}, {35, 45360}, {50, 45361}, {65, 45363}, {75, 86008}, {85, 110417} } }    // Inscription
};

const uint32 auctioneers[2][22] =
{
    { 35594, 18349, 18348, 16707, 15679, 15678, 9859, 8723, 15659, 8720, 8719, 8671, 8670, 8669, 0 },
    { 35607, 18761, 17629, 17628, 17627, 16629, 16628, 16627, 15686, 15684, 15683, 15682, 15676, 15675, 8722, 8721, 8672, 8674, 0 }
};

void ServiceMgr::AddService(Player* target, uint32 guid, ServiceEntry& entry)
{
    ASSERT(entry.Service < ISERVICE_END && entry.Service != ISERVICE_NULL);
    entry.ID = ++_serviceID;
    CharacterDatabase.EscapeString(entry.Data4);
    std::string query = Format("INSERT INTO character_service (id, guid, service, data1, data2, data3, data4, creation_date) "
        "VALUES ('%u', '%u', '%u', '%u', '%u', '%u', '%s', UNIX_TIMESTAMP());",
        entry.ID, guid, entry.Service, entry.Data1, entry.Data2, entry.Data3, entry.Data4.c_str());

    if (target)
    {
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        trans->Append(query.c_str());
        ExecuteService(target, entry, trans);
        CharacterDatabase.CommitTransaction(trans);
    }
    else
    {
        CharacterDatabase.Execute(query.c_str());
    }
}

void ServiceMgr::ExecuteService(Player* player, ServiceEntry const& entry, CharacterDatabaseTransaction trans)
{
    if (!entry.Service || entry.Service >= ISERVICE_END)
        return;

    (this->*_serviceMethods[entry.Service])(player, entry.Data1, entry.Data2, entry.Data3, entry.Data4);

    std::string query = Format("UPDATE character_service SET execution_date = UNIX_TIMESTAMP() WHERE id = %u", entry.ID);
    if (trans)
        trans->Append(query.c_str());
    else
        CharacterDatabase.Execute(query.c_str());
}

void ServiceMgr::HandleAntiError(Player *player, uint32, uint32, uint32, std::string const&)
{
    if (!player->IsAlive())
    {
        player->ResurrectPlayer(1.0f);
        player->SpawnCorpseBones();
    }

    if (Battleground *bg = player->GetBattleground())
        bg->RemovePlayerAtLeave(player->GetGUID(), false, false);

    player->SetBattlegroundId(0, BATTLEGROUND_TYPE_NONE);   // do it manually, before that could occur an error

    if (Group* group = player->GetGroup())
        group->RemoveMember(player->GetGUID());

    if (Group* group = player->GetOriginalGroup())
        group->RemoveMember(player->GetGUID());

    for (auto it = player->GetAppliedAuras().begin(); it != player->GetAppliedAuras().end();)
    {
        bool remove = true;
        if (it->second->GetBase()->GetMaxDuration() > 10 * MINUTE * IN_MILLISECONDS && !it->second->IsPositive())
            remove = false;
        else if (SpellInfo const* spellInfo = it->second->GetBase()->GetSpellInfo())
            if (spellInfo->IsPassive() && (player->HasSpell(spellInfo->Id) || player->HasGlyph(spellInfo->Id)) && player->IsNeedCastPassiveSpellAtLearn(spellInfo) || player->HasItemApplyingAura(spellInfo->Id))
                remove = false;
        if (remove)
            player->RemoveAura(it);
        else ++it;
    }
}

const ServiceMgr::ProfSkill *ServiceMgr::LookupProfession(uint32 entry)
{
    for (int i = 0; i < 11; i++)
    {
        if (_profs[i].ID == entry)
            return &_profs[i];
    }
    return NULL;
}

void ServiceMgr::HandleChangeClass(Player *player, uint32 oldClass, uint32 newClass, uint32 /**/, std::string const& /**/)
{
    player->ChangeClass(oldClass, newClass);
}

void ServiceMgr::HandleReplaceSkill(Player* player, uint32 oldSkill, uint32 newSkill, uint32 value, std::string const& /**/)
{
    ProfSkill const* oldProf = LookupProfession(oldSkill);
    ProfSkill const* newProf = LookupProfession(newSkill);

    if (!oldProf || !newProf)
    {
        TC_LOG_ERROR("misc", "ServiceManager::HandleReplaceSkill - Unknown profession skill: {} (GUID: {})", oldProf ? newSkill : oldSkill, player->GetGUID().GetCounter());
        return;
    }

    if (!player->HasSkill(oldSkill))
    {
        TC_LOG_ERROR("misc", "ServiceManager::HandleReplaceSkill - Player ({}) hasn't skill: {}", player->GetGUID().GetCounter(), oldSkill);
        return;
    }

    player->SetSkill(oldSkill, 0, 0, 0);
    player->RemoveSpell(oldProf->Rank[0].SpellID);

    int32 rank = 7;
    while (rank > 0 && newProf->Rank[rank].Level > player->GetLevel())
        --rank;
    uint32 maxValue = (rank + 1) * 75;
    value = std::min(value, maxValue);

    player->LearnSpell(newProf->Rank[rank].SpellID, false);
    player->SetSkill(newSkill, player->GetSkillStep(newSkill), value, maxValue);
}

void ServiceMgr::InsertServiceEntry(uint32 guid, uint32 service, uint32 data1, uint32 data2, uint32 data3, std::string const& data4)
{
    std::string real = data4;
    CharacterDatabase.EscapeString(real);
    CharacterDatabase.PExecute("INSERT INTO character_service (guid, service, data1, data2, data3, data4, creation_date) "
        "VALUES (%u, %u, %u, %u, %u, '%s', UNIX_TIMESTAMP());",
        guid, service, data1, data2, data3, real.c_str());
}

void ServiceMgr::SetExecuted(uint32 id)
{
    CharacterDatabase.PExecute("UPDATE character_service SET execution_data = UNIX_TIMESTAMP() WHERE id = {}", id);
}

std::set<uint32> const* ServiceMgr::GetClassSpells(uint8 classID, uint32 team)
{
    if (classID > CLASS_DRUID)
        return nullptr;

    if (team == ALLIANCE)
        return &_classSpells[0][classID];
    if (team == HORDE)
        return &_classSpells[1][classID];
    return nullptr;
}

ServiceMgr::ReclassData* ServiceMgr::GetReclassData(uint8 classID)
{
    return &_reclassData[classID];
}

uint32 ServiceMgr::GetSpellFamilyByClass(uint32 classID)
{
    switch (classID)
    {
        case CLASS_WARRIOR:
            return SPELLFAMILY_WARRIOR;
        case CLASS_PALADIN:
            return SPELLFAMILY_PALADIN;
        case CLASS_HUNTER:
            return SPELLFAMILY_HUNTER;
        case CLASS_ROGUE:
            return SPELLFAMILY_ROGUE;
        case CLASS_PRIEST:
            return SPELLFAMILY_PRIEST;
        case CLASS_DEATH_KNIGHT:
            return SPELLFAMILY_DEATHKNIGHT;
        case CLASS_SHAMAN:
            return SPELLFAMILY_SHAMAN;
        case CLASS_MAGE:
            return SPELLFAMILY_MAGE;
        case CLASS_WARLOCK:
            return SPELLFAMILY_WARLOCK;
        case CLASS_DRUID:
            return SPELLFAMILY_DRUID;
        case CLASS_MONK:
            return SPELLFAMILY_MONK;
        default:
            return 0;
    }
}

void ServiceMgr::_LoadReClassRoutine()
{
    for (int32 i = 0; i < 12; i++)
         _reclassData[i].clear();

    QueryResult result = WorldDatabase.PQuery("SELECT class, entry, learn, data, flag FROM reclass_routine;");

    if (!result)
    {
        TC_LOG_ERROR("misc", "Table reclass_routine empty.");
        return;
    }

    do
    {
        Field *fields = result->Fetch();
        ReclassInfo info;
        info.entry = fields[1].GetUInt32();
        info.learn = fields[2].GetUInt8();
        info.data = fields[3].GetUInt32();
        info.flag = fields[4].GetUInt32();
        _reclassData[fields[0].GetUInt8()].push_back(info);
    }
    while (result->NextRow());
}

void ServiceMgr::LoadFromDB()
{
    QueryResult result = CharacterDatabase.Query("SELECT max(id) FROM character_service");
    _serviceID = result ? (*result)[0].GetUInt32() : 0;
    _LoadPremium();
    _LoadReClassRoutine();
    LoadSpells();
}

void ServiceMgr::RemoveOldSkillsFromDB(uint32 guid, uint8 classID)
{
    if (classID > 11)
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    ReclassData data = _reclassData[classID];

    for (ReclassData::iterator iter = data.begin(); iter != data.end(); ++iter)
    {
        if (iter->flag == 15)
        {
            trans->PAppend("DELETE FROM character_spell WHERE guid = {} AND spell = {};", guid, iter->entry);
            trans->PAppend("DELETE FROM character_skills WHERE guid = {} and skill = {};", guid, iter->data);
        }
    }
    CharacterDatabase.CommitTransaction(trans);
}

void ServiceMgr::ApplyRetroactiveFixes(Player* player)
{
    for (auto&& fix : _retroactiveFixes)
        if (fix->IsActive() && fix->IsApplicable(player) && fix->Execute(player))
            TC_LOG_INFO("retroactivefix", "Performed fix on player {} (GUID: {}): {}", player->GetName().c_str(), player->GetGUID().GetCounter(), fix->GetDescription());
}

void ServiceMgr::LoadSpells()
{
    for (auto&& iter : _classSpells)
        for (auto&& it : iter)
            it.clear();

    QueryResult result = WorldDatabase.Query("SELECT class, spell FROM player_class_spells");
    if (!result)
    {
        TC_LOG_ERROR("misc", "Table `player_class_spells` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        uint8 classID = fields[0].GetUInt8();
        uint32 spell = fields[1].GetUInt32();
        if (!classID || classID > CLASS_DRUID)
        {
            TC_LOG_ERROR("misc", "ServiceManager::LoadSpells - Invalid class ({}) for spell {}.", classID, spell);
            continue;
        }

        SpellInfo const* info = sSpellMgr->GetSpellInfo(spell);
        if (!info)
        {
            TC_LOG_ERROR("misc", "ServiceManager::LoadSpells - Not existing spell {}.", spell);
            continue;
        }

        if (info->AttributesEx7 & SPELL_ATTR7_ALLIANCE_ONLY)
        {
            _classSpells[0][classID].insert(spell);
            continue;
        }
        if (info->AttributesEx7 & SPELL_ATTR7_HORDE_ONLY)
        {
            _classSpells[1][classID].insert(spell);
            continue;
        }

        _classSpells[0][classID].insert(spell);
        _classSpells[1][classID].insert(spell);
    } while (result->NextRow());
}

void ServiceMgr::ExecutedServices(uint32 guid, uint8 type, std::string oldData, std::string newData)
{
    if (!sWorld->getBoolConfig(CONFIG_EXECUTED_SERVICES_LOG))
        return;

    CharacterDatabase.PQuery("INSERT INTO executed_services (type, guid, old_data, new_data, execute_date) VALUES ('{}', '{}', '{}', '{}', '{}')",
        uint8(type), guid, oldData.c_str(), newData.c_str(), TimeToTimestampStr(time(NULL)).c_str());
}

void ServiceMgr::AddSpecificPlayerData(uint32 guid, uint32 oldRace, uint32 race, uint32 playerClass, Player* player, bool add, bool remove)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    CharacterDatabasePreparedStatement* stmt;

    const uint32 raceQuests[MAX_RACES][120] =
    {
        { 0 }, // RACE_NONE
        { 0 }, // RACE_HUMAN
        { 0 }, // RACE_ORC
        { 0 }, // RACE_DWARF
        { 0 }, // RACE_NIGHTELF
        { 0 }, // RACE_UNDEAD_PLAYER
        { 0 }, // RACE_TAUREN
        { 0 }, // RACE_GNOME
        { 0 }, // RACE_TROLL
        {
            // RACE_GOBLIN
            14001,14007,14008,14009,14010,14011,14012,14014,14019,14021,
            14031,14069,14070,14071,14075,14109,14110,14113,14115,14116,
            14120,14121,14122,14123,14124,14125,14126,14138,14153,14233,
            14234,14235,14236,14237,14238,14240,14241,14242,14243,14244,
            14245,14248,14303,14326,14445,14473,14474,24488,24502,24520,
            24567,24671,24741,24744,24816,24817,24856,24858,24859,24864,
            24868,24897,24901,24924,24925,24929,24937,24940,24942,24945,
            24946,24952,24954,24958,25023,25024,25058,25066,25093,25098,
            25099,25100,25109,25110,25122,25123,25125,25184,25200,25201,
            25202,25203,25204,25207,25213,25214,25243,25244,25251,25265,
            25266,25267,25275,25473,26711,26712,26803,26804,26806,26807,
            27139,28349
        },
        { 0 }, // RACE_BLOODELF
        { 0 }, // RACE_DRAENEI
        { 0 }, // RACE_FEL_ORC
        { 0 }, // RACE_NAGA
        { 0 }, // RACE_BROKEN
        { 0 }, // RACE_SKELETON
        { 0 }, // RACE_VRYKUL
        { 0 }, // RACE_TUSKARR
        { 0 }, // RACE_FOREST_TROLL
        { 0 }, // RACE_TAUNKA
        { 0 }, // RACE_NORTHREND_SKELETON
        { 0 }, // RACE_ICE_TROLL
        {
            // RACE_WORGEN
            14078,14091,14093,14094,14098,14099,14154,14157,14159,14204,
            14212,14214,14218,14221,14222,14265,14266,14269,14272,14273,
            14274,14275,14276,14277,14278,14279,14280,14281,14283,14285,
            14286,14287,14288,14289,14290,14291,14293,14294,14313,14320,
            14321,14336,14347,14348,14366,14367,14368,14369,14382,14386,
            14395,14396,14397,14398,14399,14400,14401,14402,14403,14404,
            14405,14406,14412,14416,14463,14465,14466,24438,24468,24472,
            24483,24484,24495,24501,24575,24578,24592,24593,24602,24616,
            24617,24627,24628,24646,24672,24673,24674,24675,24676,24677,
            24678,24679,24680,24681,24902,24903,24904,24920,24930,26129,
            26385,26706,28517,28787,28795,28821,28850
        },
        { 0 }, // RACE_GILNEAN
        { 0 }, // RACE_PANDAREN_NEUTRAL
        {
            // RACE_PANDAREN_ALLIANCE
            29406,29408,29409,29410,29414,29417,29418,29419,29420,29421,
            29422,29423,29424,29521,29522,29523,29524,29661,29662,29663,
            29664,29665,29666,29676,29677,29678,29679,29680,29768,29769,
            29770,29771,29772,29774,29775,29776,29777,29778,29779,29780,
            29781,29783,29784,29784,29785,29786,29787,29788,29789,29790,
            29791,29792,29793,29794,29795,29796,29797,29798,29799,29800,
            30027,30033,30034,30035,30036,30037,30038,30039,30040,30041,
            30042,30043,30044,30045,30589,30590,30591,30767,31450,30987
        },
        {
            // RACE_PANDAREN_HORDE
            29406,29408,29409,29410,29414,29417,29418,29419,29420,29421,
            29422,29423,29424,29521,29522,29523,29524,29661,29662,29663,
            29664,29665,29666,29676,29677,29678,29679,29680,29768,29769,
            29770,29771,29772,29774,29775,29776,29777,29778,29779,29780,
            29781,29783,29784,29784,29785,29786,29787,29788,29789,29790,
            29791,29792,29793,29794,29795,29796,29797,29798,29799,29800,
            30027,30033,30034,30035,30036,30037,30038,30039,30040,30041,
            30042,30043,30044,30045,30589,30590,30591,30767,31450,31012
        },
    };
    const uint32 raceSpells[MAX_RACES][10]
    {
        { 0 }, // RACE_NONE
        { 0 }, // RACE_HUMAN
        { 0 }, // RACE_ORC
        { 0 }, // RACE_DWARF
        { 0 }, // RACE_NIGHTELF
        { 0 }, // RACE_UNDEAD_PLAYER
        { 0 }, // RACE_TAUREN
        { 0 }, // RACE_GNOME
        { 0 }, // RACE_TROLL
        { 69046, 69070, 69041, 69045, 69042, 69044 }, // RACE_GOBLIN
        { 0 }, // RACE_BLOODELF
        { 0 }, // RACE_DRAENEI
        { 0 }, // RACE_FEL_ORC
        { 0 }, // RACE_NAGA
        { 0 }, // RACE_BROKEN
        { 0 }, // RACE_SKELETON
        { 0 }, // RACE_VRYKUL
        { 0 }, // RACE_TUSKARR
        { 0 }, // RACE_FOREST_TROLL
        { 0 }, // RACE_TAUNKA
        { 0 }, // RACE_NORTHREND_SKELETON
        { 0 }, // RACE_ICE_TROLL
        { 68996, 68975, 68978, 68976, 68975, 68992 }, // RACE_WORGEN
        { 0 }, // RACE_GILNEAN
        { 0 }, // RACE_PANDAREN_NEUTRAL
        { 0 }, // RACE_PANDAREN_ALLIANCE
        { 0 }, // RACE_PANDAREN_HORDE
    };

    if (remove && playerClass != CLASS_DEATH_KNIGHT)
    {
        std::vector<uint32> delQuests(&raceQuests[oldRace][0], &raceQuests[oldRace][0] + sizeof(raceQuests[oldRace]) / sizeof(uint32));
        std::vector<uint32> delSpells(&raceSpells[oldRace][0], &raceSpells[oldRace][0] + sizeof(raceSpells[oldRace]) / sizeof(uint32));

        for (auto&& questId : delQuests)
        {
            if (!questId)
                break;
            Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
            if (!quest)
                continue;

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_QUESTSTATUS_REWARDED_BY_QUEST);
            stmt->setUInt32(0, guid);
            stmt->setUInt32(1, questId);
            trans->Append(stmt);

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_QUESTSTATUS_BY_QUEST);
            stmt->setUInt32(0, guid);
            stmt->setUInt32(1, questId);
            trans->Append(stmt);
        }
        for (auto&& spellId : delSpells)
        {
            if (!spellId)
                break;

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_BY_SPELL);
            stmt->setUInt32(0, spellId);
            stmt->setUInt32(1, guid);
            trans->Append(stmt);
        }
    }

    if (add)
    {
        std::vector<uint32> addQuests(&raceQuests[race][0], &raceQuests[race][0] + sizeof(raceQuests[race]) / sizeof(uint32));
        std::vector<uint32> addSpells(&raceSpells[race][0], &raceSpells[race][0] + sizeof(raceSpells[race]) / sizeof(uint32));

        if (playerClass == CLASS_DEATH_KNIGHT)
        {
            addQuests =
            {
                12756,13166,12725,12680,12848,13165,12751,12714,12755,12657,
                12733,12697,12670,12723,12711,12754,12720,12678,12850,12778,
                12779,12727,12738,12700,12593,12719,12722,12636,12717,12706,
                12698,12800,12716,12687,12724,12679,12849,12801,12715,12757,
                12619,12641,12701,12842
            };

            if (race == RACE_ORC   || race == RACE_UNDEAD_PLAYER || race == RACE_TAUREN   || race == RACE_TROLL || race == RACE_BLOODELF || race == RACE_GOBLIN)
                addQuests.push_back(13189);
            if (race == RACE_HUMAN || race == RACE_DWARF         || race == RACE_NIGHTELF || race == RACE_GNOME || race == RACE_DRAENEI  || race == RACE_WORGEN)
                addQuests.push_back(13188);

            switch (race)
            {
                case RACE_HUMAN:         addQuests.push_back(12742); break;
                case RACE_ORC:           addQuests.push_back(12748); break;
                case RACE_DWARF:         addQuests.push_back(12744); break;
                case RACE_NIGHTELF:      addQuests.push_back(12743); break;
                case RACE_UNDEAD_PLAYER: addQuests.push_back(12750); break;
                case RACE_TAUREN:        addQuests.push_back(12739); break;
                case RACE_GNOME:         addQuests.push_back(12745); break;
                case RACE_TROLL:         addQuests.push_back(12749); break;
                case RACE_BLOODELF:      addQuests.push_back(12747); break;
                case RACE_DRAENEI:       addQuests.push_back(12746); break;
                case RACE_GOBLIN:        addQuests.push_back(28650); break;
                case RACE_WORGEN:        addQuests.push_back(28649); break;
            }
        }

        for (auto&& questId : addQuests)
        {
            if (!questId)
                break;
            Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
            if (!quest)
                continue;
            if (quest->GetAllowableClasses() && !(quest->GetAllowableClasses() & (1 << (playerClass - 1))))
                continue; // Not acceptable by player's class
            if (player && player->GetQuestRewardStatus(questId))
                continue;

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_QUESTSTATUS);
            stmt->setUInt32(0, guid);
            stmt->setUInt32(1, questId);
            trans->Append(stmt);

            if (player)
                player->learnQuestRewardedSpells(quest);
        }

        for (auto&& spell : addSpells)
        {
            if (!spell)
                break;

            if (player && player->HasSpell(spell))
                continue;

            stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_CHAR_SPELL);
            stmt->setUInt32(0, guid);
            stmt->setUInt32(1, spell);
            stmt->setUInt32(2, 1);
            stmt->setUInt32(3, 0);
            trans->Append(stmt);

            if (player)
                player->LearnSpell(spell, false);
        }
    }

    CharacterDatabase.CommitTransaction(trans);
}

void ServiceMgr::_LoadPremium()
{
    for (int32 i = 0; i < 2; i++)
    {
        _aucGUIDs[i].clear();
        int32 j = 0;
        while (auctioneers[i][j] != 0)
        {
            if (QueryResult result = WorldDatabase.PQuery("SELECT guid FROM creature WHERE id = {};", auctioneers[i][j]))
            {
                Field* fields = result->Fetch();
                uint32 guid = fields[0].GetUInt32();
                _aucGUIDs[i].push_back(ObjectGuid(HighGuid::Unit, auctioneers[i][j++], guid));
            }
        }
    }
    if (!_aucGUIDs[TEAM_ALLIANCE].size() && !_aucGUIDs[TEAM_HORDE].size())
        TC_LOG_ERROR("misc", "Invalid premium auctioneers entry! Not found in DB");
}

Creature* ServiceMgr::GetPremiumAuc(Player *player)
{
    uint8 faction;
    if (player->GetRaceMask() & RACEMASK_ALLIANCE)
        faction = TEAM_ALLIANCE;
    else if (player->GetRaceMask() & RACEMASK_HORDE)
        faction = TEAM_HORDE;
    else // just in case
        return nullptr;

    Creature* creature = nullptr;
    for (auto&& guid : _aucGUIDs[faction])
    {
        // generally, on x2 first list member always should be aviable
        creature = ObjectAccessor::GetCreature(*player, guid);
        if (creature) // it's may be dead or removed from world, then check next (list contain all game auctioneers for faction)
            break;
    }

    return creature;
}

struct MountAchievementFix : public RetroactiveFix
{
    MountAchievementFix(uint32 achievementId)
        : m_achievementId(achievementId)
    {
        m_name = "MountAchievementFix (" + std::to_string(m_achievementId) + ")";
    }

    char const* GetDescription() const override { return m_name.c_str(); };

    bool Execute(Player* player) override
    {
        if (!player->HasAchieved(m_achievementId))
            return false;

        AchievementEntry const* achievement = sAchievementStore.LookupEntry(m_achievementId);
        if (!achievement)
            return false;

        if ((achievement->RequiredFaction == ACHIEVEMENT_FACTION_HORDE    && player->GetTeam() != HORDE) ||
            (achievement->RequiredFaction == ACHIEVEMENT_FACTION_ALLIANCE && player->GetTeam() != ALLIANCE))
            return false;

        AchievementReward const* reward = sAchievementMgr->GetAchievementReward(achievement);
        if (!reward)
            return false;

        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(reward->ItemId);
        if (!proto)
            return false;

        if (player->HasSpell(proto->Spells[1].SpellId))
            return false;

        if (player->DoForEachItem([=](Item* item) { return item->GetEntry() == proto->ItemId; }))
            return false;

        bool found = false;
        for (auto&& itr2 : player->mMitems)
        {
            if (itr2.second->GetEntry() == proto->ItemId)
            {
                found = true;
                break;
            }
        }
        if (found)
            return false;

        Item* item = Item::CreateItem(reward->ItemId, 1, player);

        LocaleConstant index = player->GetSession()->GetSessionDbLocaleIndex();

        std::string subject = reward->Subject;
        std::string text = reward->Text;
        if (index >= 0)
        {
            if (AchievementRewardLocale const* loc = sAchievementMgr->GetAchievementRewardLocale(achievement))
            {
                ObjectMgr::GetLocaleString(loc->Subject, index, subject);
                ObjectMgr::GetLocaleString(loc->Text, index, text);
            }
        }

        MailDraft draft(subject, text);

        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
        if (item)
        {
            item->SaveToDB(trans);
            draft.AddItem(item);
        }

        draft.SendMailTo(trans, player, MailSender(MAIL_CREATURE, reward->Sender));
        CharacterDatabase.CommitTransaction(trans);
        return true;
    }
private:
    uint32 m_achievementId;
    std::string m_name;
};

struct ChildWeekSchoolOfHardKnocksFix : public RetroactiveFix
{
    ChildWeekSchoolOfHardKnocksFix() { }

    char const* GetDescription() const override { return "ChildWeekSchoolOfHardKnocksFix"; }

    bool Execute(Player* player) override
    {
        if (player->GetAchievementMgr().HasAchieved(1786))
            return false;

        if (!player->GetAchievementMgr().GetCriteriaProgress(6641))
            return false;
        if (!player->GetAchievementMgr().GetCriteriaProgress(6643))
            return false;
        if (!player->GetAchievementMgr().GetCriteriaProgress(6644))
            return false;

        if (Criteria const* criteria = sAchievementMgr->GetCriteria(6642))
        {
            player->GetAchievementMgr().SetCriteriaProgress(criteria, 1, player);
            player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, 1);
            player->CompletedAchievement(sAchievementMgr->GetAchievement(1786));
        }
        return true;
    }
};

struct GuildVaultAchievementFix : public RetroactiveFix
{
    GuildVaultAchievementFix() { }

    char const* GetDescription() const override { return "GuildVaultAchievementFix"; }

    bool Execute(Player* player) override
    {
        Guild* guild = player->GetGuild();
        if (!guild)
            return false;

        uint8 tabs = guild->GetPurchasedTabsSize();
        if (tabs >= 7 && guild->HasAchieved(5196))
            return false;

        guild->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BUY_GUILD_BANK_SLOTS, tabs, 0, 0, nullptr, player);
        return true;
    }
};

std::vector<RetroactiveFix*> ServiceMgr::_retroactiveFixes
{
    new MountAchievementFix{ 2536 }, // Mountain o' Mounts Alliance
    new MountAchievementFix{ 2537 }, // Mountain o' Mounts Horde
    new MountAchievementFix{ 7860 }, // We're Going to Need More Saddles Alliance
    new MountAchievementFix{ 7862 }, // We're Going to Need More Saddles Horde
    new MountAchievementFix{ 8302 }, // Mount Parade Horde
    new MountAchievementFix{ 8304 }, // Mount Parade Alliance
    new ChildWeekSchoolOfHardKnocksFix{},
    new GuildVaultAchievementFix{},
};

void ServiceMgr::DeletedItemNotify(uint32 guidLow, Item* item, uint8 type)
{
    if (!sWorld->getBoolConfig(CONFIG_DELETING_ITEM_LOG))
        return;

    bool checkQuality = item->GetTemplate()->Quality >= sWorld->getIntConfig(CONFIG_DELETING_ITEM_MIN_QUALITY) && item->GetTemplate()->Quality <= sWorld->getIntConfig(CONFIG_DELETING_ITEM_MAX_QUALITY);
    bool checkEquip = (item->GetTemplate()->Class == ITEM_CLASS_ARMOR || item->GetTemplate()->Class == ITEM_CLASS_WEAPON) && item->GetTemplate()->ItemLevel >= sWorld->getIntConfig(CONFIG_DELETING_ITEM_MIN_ITEM_LEVEL_EQUIP);
    bool checkOther = item->GetTemplate()->Class != ITEM_CLASS_ARMOR && item->GetTemplate()->Class != ITEM_CLASS_WEAPON && item->GetTemplate()->ItemLevel >= sWorld->getIntConfig(CONFIG_DELETING_ITEM_MIN_ITEM_LEVEL_OTHER);
    bool hasReputationReq = item->GetTemplate()->RequiredReputationFaction > 0;
    if (!checkQuality || (!checkEquip && !checkOther) || hasReputationReq)
        return;

    CharacterDatabase.PQuery("INSERT INTO item_deleted (owner_guid, old_item_guid, item_entry, item_count, delete_date, delete_type) VALUES ('{}', '{}', '{}', '{}', '{}', '{}')",
        guidLow, item->GetGUID().GetCounter(), item->GetEntry(), item->GetCount(), uint32(time(NULL)), uint32(type));

    item->SetHasDeletedItemRecord(true); // If the player will buy this item back after being saved - it will be removed from item_deleted table
}
