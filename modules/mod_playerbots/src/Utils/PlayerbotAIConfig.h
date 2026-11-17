/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license, you may redistribute it
 * and/or modify it under version 2 of the License, or (at your option), any later version.
 */

#ifndef _PLAYERBOT_PLAYERbotAICONFIG_H
#define _PLAYERBOT_PLAYERbotAICONFIG_H

#include <mutex>

#include "Common.h"
#include "DBCEnums.h"
#include "SharedDefines.h"

enum class HealingManaEfficiency : uint8
{
    VERY_LOW = 1,
    LOW = 2,
    MEDIUM = 4,
    HIGH = 8,
    VERY_HIGH = 16,
    SUPERIOR = 32
};

class PlayerbotAIConfig
{
public:
    PlayerbotAIConfig() {};
    static PlayerbotAIConfig* instance()
    {
        static PlayerbotAIConfig instance;
        return &instance;
    }

    bool Initialize();
    
    bool IsPvpProhibited(uint32 zoneId, uint32 areaId);
    bool IsInPvpProhibitedZone(uint32 id);
    bool IsInPvpProhibitedArea(uint32 id);

    bool enabled, perfMonEnabled;
    uint32 globalCoolDown, reactDelay, maxWaitForMove, disableMoveSplinePath, maxMovementSearchTime, expireActionTime,
        dispelAuraDuration, passiveDelay, repeatDelay, errorDelay, rpgDelay, sitDelay, returnDelay, lootDelay;
    bool dynamicReactDelay;
    float sightDistance, spellDistance, reactDistance, grindDistance, lootDistance, shootDistance, fleeDistance,
        tooCloseDistance, meleeDistance, followDistance, whisperDistance, contactDistance, aoeRadius, rpgDistance,
        targetPosRecalcDistance, farDistance, healDistance, aggroDistance;
    uint32 criticalHealth, lowHealth, mediumHealth, almostFullHealth;
    uint32 lowMana, mediumMana, highMana;
    bool autoSaveMana;
    uint32 saveManaThreshold;
    bool autoAvoidAoe;
    float maxAoeAvoidRadius;
    std::set<uint32> aoeAvoidSpellWhitelist;
    bool tellWhenAvoidAoe;

    uint32 openGoSpell;
    bool randomBotAutologin, disableRandomLevels;
    uint32 randomBotMaxLevel, randomBotMinLevel;
    float randomBotMaxLevelChance;
    uint32 minRandomBots, maxRandomBots, maxAddedBotsPerClass, maxAddedBots;
    uint32 randomBotUpdateInterval, randomBotCountChangeMinInterval, randomBotCountChangeMaxInterval;
    uint32 minRandomBotRandomizeTime, maxRandomBotRandomizeTime;
    uint32 minRandomBotTeleportInterval, maxRandomBotTeleportInterval;
    uint32 minRandomBotInWorldTime, maxRandomBotInWorldTime;
    uint32 randomBotsPerInterval;
    float randomChangeMultiplier;
    bool randomBotLoginAtStartup;
    std::string randomBotAccountPrefix;
    uint32 randomBotAccountCount;
    bool randomBotRandomPassword;
    std::vector<uint32> randomBotAccounts;

    std::string premadeSpecName[MAX_CLASSES][MAX_SPECIALIZATIONS - 1];
    std::vector<uint16> premadeSpecLink[MAX_CLASSES][MAX_SPECIALIZATIONS - 1];

    uint32 iterationsPerTick;

    std::string combatStrategies, nonCombatStrategies;
    std::string randomBotCombatStrategies, randomBotNonCombatStrategies;
    std::vector<uint32> pvpProhibitedZoneIds;
    std::vector<uint32> pvpProhibitedAreaIds;

    std::mutex m_logMtx;
    std::vector<std::string> allowedLogFiles;
    std::unordered_map<std::string, std::pair<FILE*, bool>> logFiles;

    // METHODS
    bool IsInRandomAccountList(uint32 id);

    std::string const GetTimestampStr();
    bool hasLog(std::string const fileName)
    {
        return std::find(allowedLogFiles.begin(), allowedLogFiles.end(), fileName) != allowedLogFiles.end();
    };
    bool openLog(std::string const fileName, char const* mode = "a");
    bool isLogOpen(std::string const fileName)
    {
        auto it = logFiles.find(fileName);
        return it != logFiles.end() && it->second.second;
    }
    void log(std::string const fileName, const char* str, ...);
};

#define sPlayerbotAIConfig PlayerbotAIConfig::instance()

#endif
