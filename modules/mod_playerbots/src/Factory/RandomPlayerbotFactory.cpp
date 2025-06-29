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

#include "Helper.h"
#include "RandomPlayerbotFactory.h"

#include "AccountMgr.h"
#include "DBCStore.h"
#include "DatabaseEnv.h"
#include "CharacterCache.h"
#include "GuildMgr.h"
#include "Playerbots.h"
#include "PlayerbotAIConfig.h"
#include "ScriptMgr.h"
#include "SocialMgr.h"

#include <future>
#include <algorithm>
#include <cctype>

std::map<uint8, std::vector<uint8>> RandomPlayerbotFactory::availableRaces;

RandomPlayerbotFactory::RandomPlayerbotFactory(uint32 accountId) : accountId(accountId)
{
    uint32 const expansion = sWorld->getIntConfig(WorldIntConfigs::CONFIG_EXPANSION);

    // warrior
    // alliance
    {
        availableRaces[CLASS_WARRIOR].push_back(RACE_HUMAN);
        availableRaces[CLASS_WARRIOR].push_back(RACE_DWARF);
        availableRaces[CLASS_WARRIOR].push_back(RACE_NIGHTELF);
        availableRaces[CLASS_WARRIOR].push_back(RACE_GNOME);
    }
    // horde
    {
        availableRaces[CLASS_WARRIOR].push_back(RACE_ORC);
        availableRaces[CLASS_WARRIOR].push_back(RACE_UNDEAD_PLAYER);
        availableRaces[CLASS_WARRIOR].push_back(RACE_TROLL);
        availableRaces[CLASS_WARRIOR].push_back(RACE_TAUREN);
    }

    // paladin
    // alliance
    {
        availableRaces[CLASS_PALADIN].push_back(RACE_HUMAN);
        availableRaces[CLASS_PALADIN].push_back(RACE_DWARF);
    }
    // horde
    {
        availableRaces[CLASS_PALADIN].push_back(RACE_TAUREN);
    }

    // hunter
    // alliance
    {
        availableRaces[CLASS_HUNTER].push_back(RACE_HUMAN);
        availableRaces[CLASS_HUNTER].push_back(RACE_DWARF);
        availableRaces[CLASS_HUNTER].push_back(RACE_NIGHTELF);
    }
    // horde
    {
        availableRaces[CLASS_HUNTER].push_back(RACE_ORC);
        availableRaces[CLASS_HUNTER].push_back(RACE_UNDEAD_PLAYER);
        availableRaces[CLASS_HUNTER].push_back(RACE_TROLL);
        availableRaces[CLASS_HUNTER].push_back(RACE_TAUREN);
    }

    // rogue
    // alliance
    {
        availableRaces[CLASS_ROGUE].push_back(RACE_HUMAN);
        availableRaces[CLASS_ROGUE].push_back(RACE_DWARF);
        availableRaces[CLASS_ROGUE].push_back(RACE_NIGHTELF);
        availableRaces[CLASS_ROGUE].push_back(RACE_GNOME);
    }
    // horde
    {
        availableRaces[CLASS_ROGUE].push_back(RACE_ORC);
        availableRaces[CLASS_ROGUE].push_back(RACE_UNDEAD_PLAYER);
        availableRaces[CLASS_ROGUE].push_back(RACE_TROLL);
    }

    // priest
    // alliance
    {
        availableRaces[CLASS_PRIEST].push_back(RACE_HUMAN);
        availableRaces[CLASS_PRIEST].push_back(RACE_DWARF);
        availableRaces[CLASS_PRIEST].push_back(RACE_NIGHTELF);
        availableRaces[CLASS_PRIEST].push_back(RACE_GNOME);
    }
    // horde
    {
        availableRaces[CLASS_PRIEST].push_back(RACE_UNDEAD_PLAYER);
        availableRaces[CLASS_PRIEST].push_back(RACE_TROLL);
        availableRaces[CLASS_PRIEST].push_back(RACE_TAUREN);
    }

    // shaman
    // alliance
    {
        availableRaces[CLASS_SHAMAN].push_back(RACE_DWARF);
    }
    // horde
    {
        availableRaces[CLASS_SHAMAN].push_back(RACE_ORC);
        availableRaces[CLASS_SHAMAN].push_back(RACE_TROLL);
        availableRaces[CLASS_SHAMAN].push_back(RACE_TAUREN);
    }

    // mage
    // alliance
    {
        availableRaces[CLASS_MAGE].push_back(RACE_HUMAN);
        availableRaces[CLASS_MAGE].push_back(RACE_DWARF);
        availableRaces[CLASS_MAGE].push_back(RACE_NIGHTELF);
        availableRaces[CLASS_MAGE].push_back(RACE_GNOME);
    }
    // horde
    {
        availableRaces[CLASS_MAGE].push_back(RACE_ORC);
        availableRaces[CLASS_MAGE].push_back(RACE_UNDEAD_PLAYER);
        availableRaces[CLASS_MAGE].push_back(RACE_TROLL);
    }

    // warlock
    // alliance
    {
        availableRaces[CLASS_WARLOCK].push_back(RACE_HUMAN);
        availableRaces[CLASS_WARLOCK].push_back(RACE_DWARF);
        availableRaces[CLASS_WARLOCK].push_back(RACE_GNOME);
    }
    // horde
    {
        availableRaces[CLASS_WARLOCK].push_back(RACE_ORC);
        availableRaces[CLASS_WARLOCK].push_back(RACE_UNDEAD_PLAYER);
        availableRaces[CLASS_WARLOCK].push_back(RACE_TROLL);
    }

    // druid
    // alliance
    {
        availableRaces[CLASS_DRUID].push_back(RACE_NIGHTELF);
    }
    // horde
    {
        availableRaces[CLASS_DRUID].push_back(RACE_TROLL);
        availableRaces[CLASS_DRUID].push_back(RACE_TAUREN);
    }
    if (expansion >= EXPANSION_THE_BURNING_CRUSADE)
    {
        // Draenei
        availableRaces[CLASS_WARRIOR].push_back(RACE_DRAENEI);
        availableRaces[CLASS_PALADIN].push_back(RACE_DRAENEI);
        availableRaces[CLASS_MAGE].push_back(RACE_DRAENEI);
        availableRaces[CLASS_SHAMAN].push_back(RACE_DRAENEI);
        availableRaces[CLASS_HUNTER].push_back(RACE_DRAENEI);
        availableRaces[CLASS_PRIEST].push_back(RACE_DRAENEI);

        // Blood
        availableRaces[CLASS_WARRIOR].push_back(RACE_BLOODELF);
        availableRaces[CLASS_PALADIN].push_back(RACE_BLOODELF);
        availableRaces[CLASS_HUNTER].push_back(RACE_BLOODELF);
        availableRaces[CLASS_ROGUE].push_back(RACE_BLOODELF);
        availableRaces[CLASS_PRIEST].push_back(RACE_BLOODELF);
        availableRaces[CLASS_MAGE].push_back(RACE_BLOODELF);
        availableRaces[CLASS_WARLOCK].push_back(RACE_BLOODELF);
    }

    if (expansion >= EXPANSION_CATACLYSM)
    {
        // Worgen
        availableRaces[CLASS_WARRIOR].push_back(RACE_WORGEN);
        availableRaces[CLASS_HUNTER].push_back(RACE_WORGEN);
        availableRaces[CLASS_ROGUE].push_back(RACE_WORGEN);
        availableRaces[CLASS_MAGE].push_back(RACE_WORGEN);
        availableRaces[CLASS_WARLOCK].push_back(RACE_WORGEN);
        availableRaces[CLASS_PRIEST].push_back(RACE_WORGEN);
        availableRaces[CLASS_DRUID].push_back(RACE_WORGEN);

        // Goblin
        availableRaces[CLASS_WARRIOR].push_back(RACE_GOBLIN);
        availableRaces[CLASS_HUNTER].push_back(RACE_GOBLIN);
        availableRaces[CLASS_ROGUE].push_back(RACE_GOBLIN);
        availableRaces[CLASS_PRIEST].push_back(RACE_GOBLIN);
        availableRaces[CLASS_SHAMAN].push_back(RACE_GOBLIN);
        availableRaces[CLASS_MAGE].push_back(RACE_GOBLIN);
        availableRaces[CLASS_WARLOCK].push_back(RACE_GOBLIN);
    }

    if (expansion >= EXPANSION_WRATH_OF_THE_LICH_KING)
    {
        // deathnight
        // alliance
        {
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_HUMAN);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_DWARF);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_DWARF);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_GNOME);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_DRAENEI);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_BLOODELF);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_WORGEN);
        }
        // horde
        {
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_ORC);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_UNDEAD_PLAYER);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_TROLL);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_TAUREN);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_BLOODELF);
            availableRaces[CLASS_DEATH_KNIGHT].push_back(RACE_GOBLIN);
        }
    }
    if (expansion >= EXPANSION_MISTS_OF_PANDARIA)
    {
        /*availableRaces[CLASS_WARRIOR].push_back(RACE_PANDAREN_ALLIANCE);
        availableRaces[CLASS_WARRIOR].push_back(RACE_PANDAREN_HORDE);

        availableRaces[CLASS_HUNTER].push_back(RACE_PANDAREN_ALLIANCE);
        availableRaces[CLASS_HUNTER].push_back(RACE_PANDAREN_HORDE);

        availableRaces[CLASS_ROGUE].push_back(RACE_PANDAREN_ALLIANCE);
        availableRaces[CLASS_ROGUE].push_back(RACE_PANDAREN_HORDE);

        availableRaces[CLASS_PRIEST].push_back(RACE_PANDAREN_ALLIANCE);
        availableRaces[CLASS_PRIEST].push_back(RACE_PANDAREN_HORDE);

        availableRaces[CLASS_MAGE].push_back(RACE_PANDAREN_ALLIANCE);
        availableRaces[CLASS_MAGE].push_back(RACE_PANDAREN_HORDE);

        availableRaces[CLASS_SHAMAN].push_back(RACE_PANDAREN_ALLIANCE);
        availableRaces[CLASS_SHAMAN].push_back(RACE_PANDAREN_HORDE);*/

        availableRaces[CLASS_WARRIOR].push_back(RACE_PANDAREN_NEUTRAL);
        availableRaces[CLASS_HUNTER].push_back(RACE_PANDAREN_NEUTRAL);
        availableRaces[CLASS_ROGUE].push_back(RACE_PANDAREN_NEUTRAL);
        availableRaces[CLASS_PRIEST].push_back(RACE_PANDAREN_NEUTRAL);
        availableRaces[CLASS_MAGE].push_back(RACE_PANDAREN_NEUTRAL);
        availableRaces[CLASS_SHAMAN].push_back(RACE_PANDAREN_NEUTRAL);
        availableRaces[CLASS_MONK].push_back(RACE_PANDAREN_NEUTRAL);
        // monk
        // alliance
        {
            availableRaces[CLASS_MONK].push_back(RACE_HUMAN);
            availableRaces[CLASS_MONK].push_back(RACE_DWARF);
            availableRaces[CLASS_MONK].push_back(RACE_NIGHTELF);
            availableRaces[CLASS_MONK].push_back(RACE_GNOME);
            availableRaces[CLASS_MONK].push_back(RACE_DRAENEI);
            availableRaces[CLASS_MONK].push_back(RACE_BLOODELF);
            //availableRaces[CLASS_MONK].push_back(RACE_PANDAREN_ALLIANCE);
        }
        // horde
        {
            availableRaces[CLASS_MONK].push_back(RACE_ORC);
            availableRaces[CLASS_MONK].push_back(RACE_UNDEAD_PLAYER);
            availableRaces[CLASS_MONK].push_back(RACE_TROLL);
            availableRaces[CLASS_MONK].push_back(RACE_TAUREN);
            availableRaces[CLASS_MONK].push_back(RACE_BLOODELF);
            //availableRaces[CLASS_MONK].push_back(RACE_PANDAREN_HORDE);
        }
    }
}

Player* RandomPlayerbotFactory::CreateRandomBot(WorldSession* session, Classes cls, std::unordered_map<Gender, std::vector<std::string>>& nameCache)
{
    TC_LOG_DEBUG("playerbots", "Creating new random bot for class %s", ClassToString(cls).c_str());

    uint8 gender = rand() % 2 ? GENDER_MALE : GENDER_FEMALE;
    bool alliance = rand() % 2 ? true : false;
    std::vector<uint8> raceOptions;
    for (const auto& race : availableRaces[cls])
    {
        if (alliance == IsAlliance(race))
        {
            raceOptions.push_back(race);
        }
    }

    if (raceOptions.empty())
    {
        TC_LOG_ERROR("playerbots", "No races available for class: %s", ClassToString(cls).c_str());
        return nullptr;
    }

    uint8 race = raceOptions[std::rand() % raceOptions.size()];
    std::string name;
    if (nameCache.empty())
    {
        name = CreateRandomBotName((Gender)gender);
    }
    else
    {
        if (nameCache[(Gender)gender].empty())
        {
            TC_LOG_ERROR("playerbots", "No name found for race and gender: %u %s %u", gender, RaceToString((Races)race).c_str(), gender);
            return nullptr;
        }
        uint32 i = std::rand() % nameCache[(Gender)gender].size();
        name = nameCache[(Gender)gender][i];
        swap(nameCache[(Gender)gender][i], nameCache[(Gender)gender].back());
        nameCache[(Gender)gender].pop_back();
    }
    if (name.empty())
    {
        TC_LOG_ERROR("playerbots", "Unable to get random bot name!");
        return nullptr;
    }

    std::vector<uint8> skinColors, facialHairTypes;
    std::vector<std::pair<uint8, uint8>> faces, hairs;

    for (uint32 index = 0; index < sChrSectionStore.GetNumRows(); ++index)
    {
        const CharSectionsEntry* charSection = sChrSectionStore.LookupEntry(index);

        if (!charSection) continue;
        if (charSection->RaceID != race || charSection->SexID != gender) continue;

        switch (charSection->BaseSection)
        {
            case SECTION_TYPE_SKIN:
                skinColors.push_back(charSection->ColorIndex);
                break;
            case SECTION_TYPE_FACE:
                faces.push_back(std::pair<uint8, uint8>(charSection->VariationIndex, charSection->ColorIndex));
                break;
            case SECTION_TYPE_FACIAL_HAIR:
                facialHairTypes.push_back(charSection->VariationIndex);
                break;
            case SECTION_TYPE_HAIR:
                hairs.push_back(std::pair<uint8, uint8>(charSection->VariationIndex, charSection->ColorIndex));
                break;
        }
    }

    uint8 skinColor = skinColors[std::rand() % skinColors.size()];
    std::pair<uint8, uint8> face = faces[std::rand() % faces.size()];
    std::pair<uint8, uint8> hair = hairs[std::rand() % hairs.size()];

    bool excludeCheck = (race == RACE_TAUREN) || (race == RACE_DRAENEI) ||
        (gender == GENDER_FEMALE && race != RACE_NIGHTELF && race != RACE_UNDEAD_PLAYER);
    uint8 facialHair = excludeCheck ? 0 : facialHairTypes[std::rand() % facialHairTypes.size()];

    WorldPacket p;
    std::unique_ptr<CharacterCreateInfo> characterInfo = std::make_unique<CharacterCreateInfo>(
        name, race, cls, gender, face.second, face.first, hair.first, hair.second, facialHair, 0, p);

    Player* player = new Player(session);
    player->GetMotionMaster()->Initialize();
    if (!player->Create(sObjectMgr->GetGenerator<HighGuid::Player>().Generate(), characterInfo.get()))
    {
        player->CleanupsBeforeDelete();
        delete player;

        TC_LOG_ERROR("playerbots", "Unable to create random bot for account %u - name: \"%s\"; race: %s; class: %s",
            accountId, name.c_str(), RaceToString((Races)race).c_str(), ClassToString(cls).c_str());
        return nullptr;
    }

    player->setCinematic(2);
    player->SetAtLoginFlag(AT_LOGIN_NONE);

    if (player->GetClass() == CLASS_DEATH_KNIGHT)
    {
        player->LearnSpell(50977, false);
    }
    TC_LOG_DEBUG("playerbots", "Random bot created for account %u - name: \"%s\"; race: %s; class: %s", accountId,
        name.c_str(), RaceToString((Races)race).c_str(), ClassToString(cls).c_str());

    return player;
}

std::string const RandomPlayerbotFactory::CreateRandomBotName(Gender gender)
{
    std::string botName = "";
    int tries = 3;
    while (--tries)
    {
        QueryResult result = CharacterDatabase.PQuery(
            "SELECT n.name "
            "FROM playerbots_names n "
            "LEFT OUTER JOIN characters c ON c.name = n.name "
            "WHERE c.guid IS NULL and n.gender = '%d' "
            "ORDER BY RAND() LIMIT 1",
            (uint8)gender);
        if (!result)
        {
            TC_LOG_ERROR("playerbots", "Failed to query database to get bot names.");
            break;
        }

        Field* fields = result->Fetch();
        botName = fields[0].GetString();
        if (ObjectMgr::CheckPlayerName(botName) == ResponseCodes::CHAR_NAME_SUCCESS)  // Checks for reservation & profanity, too
        {
            return botName;
        }
    }

    // CONLANG NAME GENERATION
    TC_LOG_ERROR("playerbots", "No more names left for random bots. Attempting conlang name generation.");
    const std::string groupCategory = "SCVKRU";
    const std::string groupFormStart[2][4] = { {"SV", "SV", "VK", "RV"}, {"V", "SU", "VS", "RV"} };
    const std::string groupFormMid[2][6] = { {"CV", "CVC", "CVC", "CVK", "VC", "VK"},
                                            {"CV", "CVC", "CVK", "KVC", "VC", "KV"} };
    const std::string groupFormEnd[2][4] = { {"CV", "VC", "VK", "CV"}, {"RU", "UR", "VR", "V"} };
    const std::string groupLetter[2][6] = {
        // S           C                            V               K           R         U
        {"dtspkThfS", "bcCdfghjkmnNqqrrlsStTvwxyz", "aaeeiouA", "ppttkkbdg", "lmmnrr", "AEO"},
        {"dtskThfS", "bcCdfghjkmmnNqrrlssStTvwyz", "aaaeeiiuAAEIO", "ppttkbbdg", "lmmnrrr", "AEOy"} };
    const std::string replaceRule[2][17] = {
        {"ST", "ka", "ko", "ku", "kr", "S", "T", "C", "N", "jj", "AA", "AI", "A", "E", "O", "I", "aa"},
        {"sth", "ca", "co", "cu", "cr", "sh", "th", "ch", "ng", "dg", "A", "ayu", "ai", "ei", "ou", "iu", "ae"} };

    tries = 10;
    while (--tries)
    {
        botName.clear();
        // Build name from groupForms
        // Pick random start group
        botName = groupFormStart[gender][rand() % 4];
        // Pick up to 2 and then up to 1 additional middle group
        for (int i = 0; i < rand() % 3 + rand() % 2; i++)
        {
            botName += groupFormMid[gender][rand() % 6];
        }
        // Pick up to 1 end group
        botName += rand() % 2 ? groupFormEnd[gender][rand() % 4] : "";
        // If name is single letter add random end group
        botName += (botName.size() < 2) ? groupFormEnd[gender][rand() % 4] : "";

        // Replace Catagory value with random Letter from that Catagory's Letter string for a given bot gender
        for (int i = 0; i < botName.size(); i++)
        {
            botName[i] = groupLetter[gender][groupCategory.find(botName[i])]
                [rand() % groupLetter[gender][groupCategory.find(botName[i])].size()];
        }

        // Itterate over replace rules
        for (int i = 0; i < 17; i++)
        {
            int j = botName.find(replaceRule[0][i]);
            while (j > -1)
            {
                botName.replace(j, replaceRule[0][i].size(), replaceRule[1][i]);
                j = botName.find(replaceRule[0][i]);
            }
        }

        // Capitalize first letter
        botName[0] -= 32;

        if (ObjectMgr::CheckPlayerName(botName) != ResponseCodes::CHAR_NAME_SUCCESS) // Checks for reservation & profanity, too
        {
            botName.clear();
            continue;
        }
        return std::move(botName);
    }

    tries = 10;
    while (--tries)
    {
        for (uint8 i = 0; i < 10; i++)
        {
            botName += (i == 0 ? 'A' : 'a') + rand() % 26;
        }
        if (ObjectMgr::CheckPlayerName(botName) != ResponseCodes::CHAR_NAME_SUCCESS)  // Checks for reservation & profanity, too
        {
            botName.clear();
            continue;
        }
        return std::move(botName);
    }
    TC_LOG_ERROR("playerbots", "Random name generation failed.");
    botName.clear();
    return std::move(botName);
}

void RandomPlayerbotFactory::CreateRandomBots()
{
    TC_LOG_INFO("playerbots", "Creating random bot accounts...");

    std::unordered_map<Gender, std::vector<std::string>> nameCache;
    uint32 totalAccCount = sPlayerbotAIConfig->randomBotAccountCount;
    std::vector<std::future<void>> account_creations;
    int account_creation = 0;

    TC_LOG_INFO("playerbots", "Creating cache for names per gender and race.");
    QueryResult result = CharacterDatabase.Query("SELECT name, gender FROM playerbots_names");
    if (!result)
    {
        TC_LOG_ERROR("playerbots", "No more unused names left");
        return;
    }
    do
    {
        Field* fields = result->Fetch();
        std::string name = fields[0].GetString();
        Gender gender = static_cast<Gender>(fields[1].GetUInt8());
        const auto name_result = sObjectMgr->CheckPlayerName(name);
        if (name_result == ResponseCodes::CHAR_NAME_SUCCESS)
            nameCache[gender].push_back(name);
    } while (result->NextRow());

    for (uint32 accountNumber = 0; accountNumber < sPlayerbotAIConfig->randomBotAccountCount; ++accountNumber)
    {
        std::ostringstream out;
        out << sPlayerbotAIConfig->randomBotAccountPrefix << accountNumber;
        std::string const accountName = out.str();

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME);
        stmt->setString(0, accountName);
        PreparedQueryResult result = LoginDatabase.Query(stmt);
        if (result)
        {
            continue;
        }
        account_creation++;
        std::string password = "";
        if (sPlayerbotAIConfig->randomBotRandomPassword)
        {
            for (int i = 0; i < 10; i++)
            {
                password += static_cast<char>('!' + (std::rand() % ('z' - '!' + 1)));

            }
        }
        else
            password = accountName;

        AccountOpResult res = sAccountMgr->CreateAccount(accountName, password, "");
        if (AccountOpResult::AOR_OK == res)
            TC_LOG_INFO("playerbots", "Account %s created for random bots", accountName.c_str());
    }

    if (account_creation)
    {
        /* wait for async accounts create to make character create correctly, same as account delete */
        TC_LOG_INFO("playerbots", "Waiting for %u accounts loading into database...", account_creation);
        std::this_thread::sleep_for(std::chrono::milliseconds(10) * sPlayerbotAIConfig->randomBotAccountCount);
    }

    TC_LOG_INFO("playerbots", "Creating random bot characters...");
    uint32 totalRandomBotChars = 0;
    uint32 totalCharCount = sPlayerbotAIConfig->randomBotAccountCount * 10;
    std::vector<std::pair<Player*, uint32>> playerBots;
    std::vector<WorldSession*> sessionBots;
    int bot_creation = 0;
    for (uint32 accountNumber = 0; accountNumber < sPlayerbotAIConfig->randomBotAccountCount; ++accountNumber)
    {
        std::ostringstream out;
        out << sPlayerbotAIConfig->randomBotAccountPrefix << accountNumber;
        std::string accountName = out.str();
        std::transform(accountName.begin(), accountName.end(), accountName.begin(), ::toupper);

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_GET_ACCOUNT_ID_BY_USERNAME);
        stmt->setString(0, accountName);
        PreparedQueryResult result = LoginDatabase.Query(stmt);
        if (!result)
            continue;

        Field* fields = result->Fetch();
        uint32 accountId = fields[0].GetUInt32();

        sPlayerbotAIConfig->randomBotAccounts.push_back(accountId);

        uint32 character_count = AccountMgr::GetCharactersCount(accountId);
        if (character_count >= 10)
        {
            continue;
        }
        TC_LOG_INFO("playerbots", "Creating random bot characters for account: [%u/%u]", accountNumber + 1, sPlayerbotAIConfig->randomBotAccountCount);
        RandomPlayerbotFactory factory(accountId);

        WorldSession* session = new WorldSession(accountId, nullptr, AccountTypes::SEC_PLAYER, EXPANSION_MISTS_OF_PANDARIA,
            time_t(0), LOCALE_enUS, 0, false, false, true);
        //session->SetVirtualRealmID(1);
        sessionBots.push_back(session);

        for (uint8 cls = CLASS_WARRIOR; cls < MAX_CLASSES - character_count; ++cls)
        {
            // skip nonexistent classes
            if (!((1 << (cls - 1)) & CLASSMASK_ALL_PLAYABLE) || !sChrClassesStore.LookupEntry(cls))
                continue;

            if (bool const isClassDeathKnight = cls == CLASS_DEATH_KNIGHT;
                isClassDeathKnight && sWorld->getIntConfig(WorldIntConfigs::CONFIG_EXPANSION) < EXPANSION_WRATH_OF_THE_LICH_KING)
            {
                continue;
            }
            if (bool const isClassMonk = cls == CLASS_MONK;
                isClassMonk&& sWorld->getIntConfig(WorldIntConfigs::CONFIG_EXPANSION) < EXPANSION_MISTS_OF_PANDARIA)
            {
                continue;
            }

            if (cls <= CLASS_DRUID)
            {
                if (Player* playerBot = factory.CreateRandomBot(session, (Classes)cls, nameCache))
                {
                    playerBot->SaveToDB(true);

                    sWorld->AddCharacterNameData(playerBot->GetGUID(), playerBot->GetName(), playerBot->GetGender(), playerBot->GetRace(), playerBot->GetClass(), playerBot->GetLevel());
                    sCharacterCache->AddCharacterCacheEntry(playerBot->GetGUID(), accountId, playerBot->GetName(), playerBot->GetGender(), playerBot->GetRace(), playerBot->GetClass(), playerBot->GetLevel());

                    playerBot->CleanupsBeforeDelete();

                    delete playerBot;
                    playerBot = nullptr;
                    session->SetPlayer(nullptr);

                    bot_creation++;
                }
                else
                {
                    TC_LOG_ERROR("playerbots", "Fail to create character for account %u", accountId);
                }
            }
        }
    }

    if (bot_creation)
    {
        TC_LOG_INFO("playerbots", "Waiting for %u characters loading into database...", bot_creation);
        /* wait for characters load into database, or characters will fail to loggin */
        std::this_thread::sleep_for(std::chrono::seconds(5) + bot_creation * std::chrono::milliseconds(5));
    }

    for (WorldSession* session : sessionBots)
        delete session;

    for (uint32 accountId : sPlayerbotAIConfig->randomBotAccounts)
    {
        totalRandomBotChars += AccountMgr::GetCharactersCount(accountId);
    }

    TC_LOG_INFO("server.loading", ">> %u random bot accounts with %u characters available", sPlayerbotAIConfig->randomBotAccounts.size(), totalRandomBotChars);
}

