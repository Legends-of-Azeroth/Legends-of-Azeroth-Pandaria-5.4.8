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

#include "PlayerbotAIConfig.h"

#include <iostream>

#include "Config.h"
#include "Helper.h"
#include "Playerbots.h"
#include "RandomPlayerbotMgr.h"
#include "Factory/RandomPlayerbotFactory.h"

template <class T>
void LoadList(std::string const value, T& list)
{
    std::vector<std::string> ids = split(value, ',');
    for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        uint32 id = atoi((*i).c_str());
        // if (!id)
        //     continue;
        list.push_back(id);
    }
}

template <class T>
void LoadSet(std::string const value, T& set)
{
    std::vector<std::string> ids = split(value, ',');
    for (std::vector<std::string>::iterator i = ids.begin(); i != ids.end(); i++)
    {
        uint32 id = atoi((*i).c_str());
        // if (!id)
        //     continue;
        set.insert(id);
    }
}

template <class T>
void LoadListString(std::string const value, T& list)
{
    std::vector<std::string> strings = split(value, ',');
    for (std::vector<std::string>::iterator i = strings.begin(); i != strings.end(); i++)
    {
        std::string const string = *i;
        if (string.empty())
            continue;

        list.push_back(string);
    }
}

bool PlayerbotAIConfig::Initialize()
{
    TC_LOG_INFO("server.loading", "Initializing AI Playerbots");
    enabled = sConfigMgr->GetBoolDefault("AiPlayerbot.Enabled", true);
    if (!enabled)
    {
        TC_LOG_INFO("server.loading", "AI Playerbots is Disabled in aiplayerbot.conf");
        return false;
    }

    perfMonEnabled = sConfigMgr->GetBoolDefault("AiPlayerbot.PerfMonEnabled", false);
    globalCoolDown = sConfigMgr->GetIntDefault("AiPlayerbot.GlobalCooldown", 1500);
    maxWaitForMove = sConfigMgr->GetIntDefault("AiPlayerbot.MaxWaitForMove", 5000);
    disableMoveSplinePath = sConfigMgr->GetIntDefault("AiPlayerbot.DisableMoveSplinePath", 0);
    maxMovementSearchTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxMovementSearchTime", 3);
    expireActionTime = sConfigMgr->GetIntDefault("AiPlayerbot.ExpireActionTime", 5000);
    dispelAuraDuration = sConfigMgr->GetIntDefault("AiPlayerbot.DispelAuraDuration", 7000);
    reactDelay = sConfigMgr->GetIntDefault("AiPlayerbot.ReactDelay", 100);
    dynamicReactDelay = sConfigMgr->GetBoolDefault("AiPlayerbot.DynamicReactDelay", true);
    passiveDelay = sConfigMgr->GetIntDefault("AiPlayerbot.PassiveDelay", 10000);
    repeatDelay = sConfigMgr->GetIntDefault("AiPlayerbot.RepeatDelay", 2000);
    errorDelay = sConfigMgr->GetIntDefault("AiPlayerbot.ErrorDelay", 5000);
    rpgDelay = sConfigMgr->GetIntDefault("AiPlayerbot.RpgDelay", 10000);
    sitDelay = sConfigMgr->GetIntDefault("AiPlayerbot.SitDelay", 30000);
    returnDelay = sConfigMgr->GetIntDefault("AiPlayerbot.ReturnDelay", 7000);
    lootDelay = sConfigMgr->GetIntDefault("AiPlayerbot.LootDelay", 1000);

    farDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.FarDistance", 20.0f);
    sightDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.SightDistance", 75.0f);
    spellDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.SpellDistance", 25.0f);
    shootDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.ShootDistance", 25.0f);
    healDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.HealDistance", 25.0f);
    lootDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.LootDistance", 15.0f);
    fleeDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.FleeDistance", 7.5f);
    aggroDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.AggroDistance", 22.0f);
    tooCloseDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.TooCloseDistance", 5.0f);
    meleeDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.MeleeDistance", 0.75f);
    followDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.FollowDistance", 1.5f);
    whisperDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.WhisperDistance", 6000.0f);
    contactDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.ContactDistance", 0.5f);
    aoeRadius = sConfigMgr->GetFloatDefault("AiPlayerbot.AoeRadius", 5.0f);
    rpgDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.RpgDistance", 200.0f);
    grindDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.GrindDistance", 75.0f);
    reactDistance = sConfigMgr->GetFloatDefault("AiPlayerbot.ReactDistance", 150.0f);

    criticalHealth = sConfigMgr->GetIntDefault("AiPlayerbot.CriticalHealth", 20);
    lowHealth = sConfigMgr->GetIntDefault("AiPlayerbot.LowHealth", 45);
    mediumHealth = sConfigMgr->GetIntDefault("AiPlayerbot.MediumHealth", 65);
    almostFullHealth = sConfigMgr->GetIntDefault("AiPlayerbot.AlmostFullHealth", 85);
    lowMana = sConfigMgr->GetIntDefault("AiPlayerbot.LowMana", 15);
    mediumMana = sConfigMgr->GetIntDefault("AiPlayerbot.MediumMana", 40);
    highMana = sConfigMgr->GetIntDefault("AiPlayerbot.HighMana", 65);
    autoSaveMana = sConfigMgr->GetBoolDefault("AiPlayerbot.AutoSaveMana", true);
    saveManaThreshold = sConfigMgr->GetIntDefault("AiPlayerbot.SaveManaThreshold", 60);
    autoAvoidAoe = sConfigMgr->GetBoolDefault("AiPlayerbot.AutoAvoidAoe", true);
    maxAoeAvoidRadius = sConfigMgr->GetFloatDefault("AiPlayerbot.MaxAoeAvoidRadius", 15.0f);
    LoadSet<std::set<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.AoeAvoidSpellWhitelist", "50759,57491,13810"),
        aoeAvoidSpellWhitelist);
    tellWhenAvoidAoe = sConfigMgr->GetBoolDefault("AiPlayerbot.TellWhenAvoidAoe", false);

    iterationsPerTick = sConfigMgr->GetIntDefault("AiPlayerbot.IterationsPerTick", 100);

    LoadList<std::vector<uint32>>(
        sConfigMgr->GetStringDefault("AiPlayerbot.PvpProhibitedZoneIds",
            "2255,656,2361,2362,2363,976,35,2268,3425,392,541,1446,3828,3712,3738,3565,"
            "3539,3623,4152,3988,4658,4284,4418,4436,4275,4323,4395"),
        pvpProhibitedZoneIds);
    LoadList<std::vector<uint32>>(sConfigMgr->GetStringDefault("AiPlayerbot.PvpProhibitedAreaIds", "976,35"),
        pvpProhibitedAreaIds);
 
    randomBotAutologin = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotAutologin", true);
    randomBotAccountPrefix = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotAccountPrefix", "rndbot");
    randomBotAccountCount = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotAccountCount", 200);
    maxAddedBotsPerClass = sConfigMgr->GetIntDefault("AiPlayerbot.MaxAddedBotsPerClass", 50);
    maxAddedBots = sConfigMgr->GetIntDefault("AiPlayerbot.MaxAddedBots", 200);
    minRandomBots = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBots", 50);
    maxRandomBots = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBots", 200);
    disableRandomLevels = sConfigMgr->GetBoolDefault("AiPlayerbot.DisableRandomLevels", true);
    randomBotMaxLevel = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotMaxLevel", 90);
    randomBotMinLevel = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotMinLevel", 1);
    randomBotMaxLevelChance = sConfigMgr->GetFloatDefault("AiPlayerbot.RandomBotMaxLevelChance", 0.25f);
    randomBotsPerInterval = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotsPerInterval", 60);
    randomBotUpdateInterval = sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotUpdateInterval", 20);
    randomBotCountChangeMinInterval =
        sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotCountChangeMinInterval", 30 * MINUTE);
    randomBotCountChangeMaxInterval =
        sConfigMgr->GetIntDefault("AiPlayerbot.RandomBotCountChangeMaxInterval", 2 * HOUR);
    minRandomBotInWorldTime = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotInWorldTime", 2 * HOUR);
    maxRandomBotInWorldTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotInWorldTime", 14 * 24 * HOUR);
    minRandomBotRandomizeTime = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotRandomizeTime", 2 * HOUR);
    maxRandomBotRandomizeTime = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotRandomizeTime", 14 * 24 * HOUR);
    minRandomBotTeleportInterval = sConfigMgr->GetIntDefault("AiPlayerbot.MinRandomBotTeleportInterval", 1 * HOUR);
    maxRandomBotTeleportInterval = sConfigMgr->GetIntDefault("AiPlayerbot.MaxRandomBotTeleportInterval", 5 * HOUR);

    randomBotLoginAtStartup = sConfigMgr->GetBoolDefault("AiPlayerbot.RandomBotLoginAtStartup", true);
    openGoSpell = sConfigMgr->GetIntDefault("AiPlayerbot.OpenGoSpell", 6477);

    randomChangeMultiplier = sConfigMgr->GetFloatDefault("AiPlayerbot.RandomChangeMultiplier", 1.0);

    randomBotCombatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotCombatStrategies", "-threat");
    randomBotNonCombatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.RandomBotNonCombatStrategies", "");
    combatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.CombatStrategies", "+custom::say");
    nonCombatStrategies = sConfigMgr->GetStringDefault("AiPlayerbot.NonCombatStrategies", "+custom::say,+return");


    RandomPlayerbotFactory::CreateRandomBots();
    if (World::IsStopped())
    {
        return true;
    }

    sRandomPlayerbotMgr->PrepareAddclassCache();
    sRandomPlayerbotMgr->PrepareTeleportCache();

    TC_LOG_INFO("server.loading", "---------------------------------------");
    TC_LOG_INFO("server.loading", "          Loading TalentSpecs          ");
    TC_LOG_INFO("server.loading", "---------------------------------------");
    for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
    {
        for (uint32 spec = 0; spec < MAX_SPECIALIZATIONS; ++spec)
        {
            std::ostringstream os;
            os << "AiPlayerbot.PremadeSpecName." << cls << "." << spec;
            std::string loadSpec = sConfigMgr->GetStringDefault(os.str().c_str(), "", false);
            if (!loadSpec.empty())
            {
                std::string premadeSpecLinkLoader[MAX_CLASSES][MAX_SPECIALIZATIONS];
                premadeSpecName[cls][spec] = loadSpec;

                os.str("");
                os.clear();
                os << "AiPlayerbot.PremadeSpecLink." << cls << "." << spec;

                premadeSpecLinkLoader[cls][spec] = sConfigMgr->GetStringDefault(os.str().c_str(), "", true);
                std::vector<std::string> splitSpec = split(premadeSpecLinkLoader[cls][spec], ',');
                for (std::string& split : splitSpec)
                {
                    if (split.size() != 0)
                    {
                        premadeSpecLink[cls][spec].push_back(atoi(split.c_str()));
                    }
                }

                TC_LOG_DEBUG("playerbots", "Premade spec loaded: %s - %s", loadSpec.c_str(), premadeSpecLinkLoader[cls][spec].c_str());
            }
            os.str("");
            os.clear();
        }
    }


    TC_LOG_INFO("server.loading", "---------------------------------------");
    TC_LOG_INFO("server.loading", "        AI Playerbots initialized       ");
    TC_LOG_INFO("server.loading", "---------------------------------------");

    return true;
}

bool PlayerbotAIConfig::IsPvpProhibited(uint32 zoneId, uint32 areaId)
{
    return IsInPvpProhibitedZone(zoneId) || IsInPvpProhibitedArea(areaId) || IsInPvpProhibitedZone(areaId);
}

bool PlayerbotAIConfig::IsInPvpProhibitedZone(uint32 id)
{
    return std::find(pvpProhibitedZoneIds.begin(), pvpProhibitedZoneIds.end(), id) != pvpProhibitedZoneIds.end();
}

bool PlayerbotAIConfig::IsInPvpProhibitedArea(uint32 id)
{
    return std::find(pvpProhibitedAreaIds.begin(), pvpProhibitedAreaIds.end(), id) != pvpProhibitedAreaIds.end();
}

std::string const PlayerbotAIConfig::GetTimestampStr()
{
    time_t t = time(nullptr);
    tm* aTm = localtime(&t);
    //       YYYY   year
    //       MM     month (2 digits 01-12)
    //       DD     day (2 digits 01-31)
    //       HH     hour (2 digits 00-23)
    //       MM     minutes (2 digits 00-59)
    //       SS     seconds (2 digits 00-59)
    char buf[20];
    snprintf(buf, 20, "%04d-%02d-%02d %02d-%02d-%02d", aTm->tm_year + 1900, aTm->tm_mon + 1, aTm->tm_mday, aTm->tm_hour,
        aTm->tm_min, aTm->tm_sec);
    return std::string(buf);
}

bool PlayerbotAIConfig::openLog(std::string const fileName, char const* mode)
{
    if (!hasLog(fileName))
        return false;

    auto logFileIt = logFiles.find(fileName);
    if (logFileIt == logFiles.end())
    {
        logFiles.insert(std::make_pair(fileName, std::make_pair(nullptr, false)));
        logFileIt = logFiles.find(fileName);
    }

    FILE* file = logFileIt->second.first;
    bool fileOpen = logFileIt->second.second;

    if (fileOpen)  // close log file
        fclose(file);

    std::string m_logsDir = sConfigMgr->GetStringDefault("LogsDir", "");
    if (!m_logsDir.empty())
    {
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");
    }

    file = fopen((m_logsDir + fileName).c_str(), mode);
    fileOpen = true;

    logFileIt->second.first = file;
    logFileIt->second.second = fileOpen;

    return true;
}

void PlayerbotAIConfig::log(std::string const fileName, char const* str, ...)
{
    if (!str)
        return;

    std::lock_guard<std::mutex> guard(m_logMtx);

    if (!isLogOpen(fileName) && !openLog(fileName, "a"))
        return;

    FILE* file = logFiles.find(fileName)->second.first;

    va_list ap;
    va_start(ap, str);
    vfprintf(file, str, ap);
    fprintf(file, "\n");
    va_end(ap);
    fflush(file);

    fflush(stdout);
}

bool PlayerbotAIConfig::IsInRandomAccountList(uint32 id)
{
    return std::find(randomBotAccounts.begin(), randomBotAccounts.end(), id) != randomBotAccounts.end();
}